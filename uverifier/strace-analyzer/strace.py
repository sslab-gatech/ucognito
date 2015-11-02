'''
Created on Apr 30, 2015

@author: meng
'''
import re
import sys

#
# Initialize regular expressions
#

global re_get_pid
re_get_pid \
        = re.compile(r"\s*(\d+) .*")

global re_extract
re_extract \
        = re.compile(r"\s*(\w+)(\(.*)$")

global re_extract_unfinished
re_extract_unfinished \
        = re.compile(r"\s*(\w+)(\(.*)<unfinished \.\.\.>$")

global re_extract_resumed
re_extract_resumed \
        = re.compile(r"\s*<\.\.\. (\w+) resumed>(.*)$")

global re_extract_detached
re_extract_detached \
        = re.compile(r".*<detached \.\.\.>$")
        
global re_extract_signal
re_extract_signal \
        = re.compile(r"\s*--- .* ---$")

global re_extract_exit
re_extract_exit \
        = re.compile(r"\s*\+\+\+ .* \+\+\+$")
        
global re_extract_arguments_and_return_value_none
re_extract_arguments_and_return_value_none \
        = re.compile(r"\((.*)\)[ \t]*= (\?)$")

global re_extract_arguments_and_return_value_ok
re_extract_arguments_and_return_value_ok \
        = re.compile(r"\((.*)\)[ \t]*= (-?\d+)$")

global re_extract_arguments_and_return_value_ok_hex
re_extract_arguments_and_return_value_ok_hex \
        = re.compile(r"\((.*)\)[ \t]*= (-?0[xX][a-fA-F\d]+)$")

global re_extract_arguments_and_return_value_error
re_extract_arguments_and_return_value_error \
        = re.compile(r"\((.*)\)[ \t]*= (-?\d+) (\w+) \([\w ]+\)$")

global re_extract_arguments_and_return_value_error_unknown
re_extract_arguments_and_return_value_error_unknown \
        = re.compile(r"\((.*)\)[ \t]*= (\?) (\w+) \([\w ]+\)$")

global re_extract_arguments_and_return_value_ext
re_extract_arguments_and_return_value_ext \
        = re.compile(r"\((.*)\)[ \t]*= (-?\d+) \(([^()]+)\)$")

global re_extract_arguments_and_return_value_ext_hex
re_extract_arguments_and_return_value_ext_hex \
        = re.compile(r"\((.*)\)[ \t]*= (-?0[xX][a-fA-F\d]+) \(([^()]+)\)$")

        
def unquote_arg(arg):
    re_quoted_arg = re.compile(r"\s*(.*)\s*")
    r = re_quoted_arg.match(arg)
    if r is not None:
        return r.group(0)
    else:
        return None   
    
def excape_control_character(x):
    return "".join([i for i in x if 31 < ord(i) < 127])    
    
#
# One syscall entry
#
class StraceEntry:

    def __init__(self, pid, seq,
                 syscall_name, syscall_arguments, return_value):
        self.pid = pid
        self.seq= seq
        self.syscall_name = syscall_name
        self.syscall_arguments = syscall_arguments
        self.return_value = return_value
#
# A process in a strace output
#
class StraceTracedProcess:
    
    def __init__(self, pid):

        self.pid = pid
        self.entries = []
        self.unfinished = dict()
        self.exited = False
        self.next_seq = 0

#
# One strace log
#
class StraceLog:
    
    def __init__(self, path):
        
        self.path = path
        self.processes = dict()
        
        with open(path) as f:       
            lineno = 0
            for line in f:
                self.__parse_line(line, lineno)
                lineno += 1

    def __parse_line(self, line, lineno):

        line = excape_control_character(line.strip())
        pos_start = 0
        
        # Get the PID
        
        pid = None
        m_pid = re_get_pid.match(line)
        if m_pid is not None:
            pid = int(m_pid.group(1))
            pos_start = len(m_pid.group(1)) + 1
            if pid not in self.processes:
                self.processes[pid] = StraceTracedProcess(pid)
        else:
            print >> sys.stderr, ("Invalid line - line %d does not begin with pid: %s") % (lineno,  line)
            return
            #raise Exception("Invalid line - line does not begin with pid: " + line)
                
        
        # Signals
        
        if line.endswith("---"):
            r = re_extract_signal.match(line, pos_start)
            if r is None:
                print >> sys.stderr, ("Invalid line - line %d not a signal: %s)") % (lineno, line)
                return                
                # raise Exception("Invalid line - not a signal: " + line)
            else:
                return
            
        # exits
        
        if line.endswith("+++"):
            r = re_extract_exit.match(line, pos_start)
            if r is None:
                print >> sys.stderr, ("Invalid line - line %d is not an exit: %s") % (lineno, line)
                return                   
                # raise Exception("Invalid line - not an exit: " + line)
            else:
                self.processes[pid].exited = True
                return
            
        # detach
        
        if line.endswith("<detached ...>"):
            r = re_extract_detached.match(line, pos_start)
            if r is None:
                print >> sys.stderr, ("Invalid line - line %d is not a \"detached\" statement: %s") % (lineno, line)
                return                
                # raise Exception("Invalid line - not a \"detached\" statement: " + line)
            else:
                return
                
        # Unfinished and resumed syscalls
        
        if line.endswith("<unfinished ...>"):
            r = re_extract_unfinished.match(line, pos_start)
            if r is None:
                print >> sys.stderr, ("Invalid line - line %d is not an \"unfinished\" statement: %s") % (lineno, line)
                return  
                # raise Exception("Invalid line - not an \"unfinished\" statement: " + line)
            else:
                syscall_name = r.group(1)
                entry = StraceEntry(pid, self.processes[pid].next_seq, syscall_name, r.group(2), None)
                self.processes[pid].entries.append(entry)
                self.processes[pid].unfinished[syscall_name] = entry
                self.processes[pid].next_seq = self.processes[pid].next_seq + 1
                return
        
        r = re_extract_resumed.match(line, pos_start)
        if r is not None:
            syscall_name = r.group(1)
            if syscall_name not in self.processes[pid].unfinished.keys():
                print >> sys.stderr, ("Invalid line - line %d no line to resume: %s") % (lineno, line)
                return
                # raise Exception("Invalid line - No line to resume: " + line)
            else:
                entry = self.processes[pid].unfinished[syscall_name]
                resumed_line = entry.syscall_arguments + r.group(2)
                arguments, return_value = self.__parse_args_and_result(resumed_line)
                entry.syscall_arguments = arguments
                entry.return_value = return_value
                del self.processes[pid].unfinished[syscall_name]
                return
                
        # Extract basic information
        
        r = re_extract.match(line, pos_start)
        if r is not None:
            syscall_name = r.group(1)
            args_and_result_str = r.group(2)
        else:
            print >> sys.stderr, ("Invalid line - line %d invalid format: %s") % (lineno, line)
            return
            # raise Exception("Invalid line - invalid format:" + line)

        arguments, return_value = self.__parse_args_and_result(args_and_result_str)

        # Finish
        
        if arguments is not None:
            self.processes[pid].entries.append(StraceEntry(pid, self.processes[pid].next_seq, syscall_name, arguments, return_value)) 
            self.processes[pid].next_seq = self.processes[pid].next_seq + 1
        else:
            print >> sys.stderr, ("Invalid line - line %d argument parsing error: %s") % (lineno, line)
            return
    
    def __parse_args_and_result(self, args_and_result_str):
        
        # Extract the return value
        m_args_and_result \
          = re_extract_arguments_and_return_value_ok.match(args_and_result_str)
        if m_args_and_result != None:
            return_value = int(m_args_and_result.group(2))
            arguments_str = m_args_and_result.group(1)
        if m_args_and_result == None:
            m_args_and_result \
              = re_extract_arguments_and_return_value_ok_hex.match(args_and_result_str)
            if m_args_and_result != None:
                return_value = m_args_and_result.group(2)
                arguments_str = m_args_and_result.group(1)
        if m_args_and_result == None:
            m_args_and_result \
              = re_extract_arguments_and_return_value_error.match(args_and_result_str)
            if m_args_and_result != None:
                return_value = m_args_and_result.group(2)
                arguments_str = m_args_and_result.group(1)
        if m_args_and_result == None:
            m_args_and_result \
              = re_extract_arguments_and_return_value_error_unknown.match(args_and_result_str)
            if m_args_and_result != None:
                return_value = m_args_and_result.group(2)
                arguments_str = m_args_and_result.group(1)
        if m_args_and_result == None:
            m_args_and_result \
              = re_extract_arguments_and_return_value_ext.match(args_and_result_str)
            if m_args_and_result != None:
                return_value = m_args_and_result.group(2)
                arguments_str = m_args_and_result.group(1)
        if m_args_and_result == None:
            m_args_and_result \
              = re_extract_arguments_and_return_value_ext_hex.match(args_and_result_str)
            if m_args_and_result != None:
                return_value = m_args_and_result.group(2)
                arguments_str = m_args_and_result.group(1)
        if m_args_and_result == None:
            m_args_and_result \
              = re_extract_arguments_and_return_value_none.match(args_and_result_str)
            if m_args_and_result != None:
                return_value = None
                arguments_str = m_args_and_result.group(1)
        if m_args_and_result == None:
            print >> sys.stderr, "Invalid args and result string:" + args_and_result_str
            return None, None
            # raise Exception("Invalid line - invalid args and result string:" + args_and_result_str)
        
        
        arguments = self.__parse_arguments(arguments_str)
        
        return arguments, return_value

    def __parse_arguments(self, arguments_str, include_quotes=False,
            include_ellipsis=True):
        '''
        Parse the given argument string and return an array of substrings
        '''

        arguments = []
        current_arg = ""
        quote_type = None
        escaped = False
        expect_comma = False
        between_arguments = False
        nest_stack = []

        for c in arguments_str:

            # Characters between arguments

            if between_arguments and c in [' ', '\t']:
                continue
            else:
                between_arguments = False
            if expect_comma:
                assert quote_type is None
                if c == '.':
                    if include_ellipsis:
                        current_arg += c
                elif c == ',':
                    expect_comma = False
                    between_arguments = True
                    arguments.append(current_arg)
                    current_arg = ""
                elif c in [' ', '\t']:
                    continue
                else:
                    print >> sys.stderr, ("'%s' found where comma expected; " \
                            + "offending string: %s") % (c, arguments_str)
                    return None
                continue


            # Arguments

            if escaped:
                current_arg += c
                escaped = False
            elif c == '\\':
                current_arg += c
                escaped = True
            elif c in ['"', '\'', '[', ']', '{', '}']:
                if quote_type in ['"', '\''] and c != quote_type:
                    current_arg += c
                elif c == quote_type:
                    if include_quotes or len(nest_stack) > 0:
                        current_arg += c
                    if len(nest_stack) > 1:
                        nest_stack.pop()
                        quote_type = nest_stack[-1]
                    else:
                        nest_stack.pop()
                        quote_type = None
                        expect_comma = True
                elif c in [']', '}']:
                    current_arg += c
                else:
                    if include_quotes or len(nest_stack) > 0:
                        current_arg += c
                    if c == '[': c = ']'
                    if c == '{': c = '}'
                    quote_type = c
                    nest_stack.append(c)
            elif c == ',' and quote_type is None:
                arguments.append(current_arg)
                current_arg = ""
                between_arguments = True
            else:
                current_arg += c

        if quote_type is not None:
            print >> sys.stderr, ("Expected '%s' but found end of the string; " \
                    + "offending string: %s") % (quote_type, arguments_str)
            return None
            #raise Exception(("Expected '%s' but found end of the string; " \
            #        + "offending string: %s") % (quote_type, arguments_str))

        if len(current_arg) > 0:
            arguments.append(current_arg)
            
        return arguments
