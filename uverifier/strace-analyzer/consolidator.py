'''
Created on Apr 30, 2015

@author: meng
'''

from strace import unquote_arg

class ProcessTreeNode():
    
    def __init__(self, pid):
        self.pid = pid
        self.cutpoint = -1
        self.parent = None
        self.children = []

class ProcessTree():
    
    def __init__(self, log):
        
        self.tree = dict()
        
        self.__build_tree(log)
    
    def __build_tree(self, log):

        for pid in log.processes.keys():
            if pid not in self.tree.keys():
                self.tree[pid] = ProcessTreeNode(pid)
            for entry in log.processes[pid].entries:
                if entry.syscall_name in ["fork", "clone", "vfork"]:
                    child_pid = int(entry.return_value)
                    if child_pid not in self.tree.keys():
                        self.tree[child_pid] = ProcessTreeNode(child_pid)
                    self.tree[child_pid].cutpoint = entry.seq
                    self.tree[pid].children.append(self.tree[child_pid])
                    self.tree[child_pid].parent = self.tree[pid]
        
                
class FileUsage:
    
    def __init__(self, name, rw):
        self.name = name
        self.rw= rw
        self.r_entries = []
        self.w_entries = []
        self.deleted = False
        
    def add_entry(self, rw, text):
        if rw == "r":
            self.r_entries.append(text)
        else:
            self.w_entries.append(text)
        
class ProcessAnalyzer():
    
    def __init__(self, log):
        self.log = log
        self.tree = ProcessTree(log)
        
        self.entries = dict()
        self.fileusages = dict()
        
        self.__complete_process()
        self.__anaylze_process()
        
    def __complete_process(self):
        for pid in self.log.processes.keys():
            if pid not in self.entries.keys():
                self.entries[pid] = []
            self.entries[pid].extend(self.log.processes[pid].entries)
            node = self.tree.tree[pid]
            while node.parent is not None:
                parent = node.parent
                for i in range(node.cutpoint, 0, -1):
                    entry = self.log.processes[parent.pid].entries[i]
                    if entry.syscall_name in ["open", "openat", "creat", "close", "rename", "unlink"]:
                        self.entries[pid].insert(0, entry)
                node = parent
    
    def __anaylze_process(self):
        for pid in self.entries.keys():
            self.fileusages[pid] = self.__analyze_fileusage(self.entries[pid])
        
    def __analyze_fileusage(self, entries):
        fn_table = dict()
        fd_table = dict()
        for entry in entries:
            if entry.syscall_name == "open" \
            or entry.syscall_name == "creat" \
            or entry.syscall_name == "openat":
                if entry.syscall_name == "open" or entry.syscall_name == "creat":
                    fn = unquote_arg(entry.syscall_arguments[0])
                    fd = str(entry.return_value)
                    if "O_RDONLY" in entry.syscall_arguments[1]:
                        fu = FileUsage(fn, False)
                    else:
                        fu = FileUsage(fn, True)
                    fd_table[fd] = fu
                    fn_table[fn] = fu                       
                elif entry.syscall_name == "openat":
                    fn = unquote_arg(entry.syscall_arguments[1])
                    fd = str(entry.return_value)
                    if "O_RDONLY" in entry.syscall_arguments[2]:
                        fu = FileUsage(fn, False)
                    else:
                        fu = FileUsage(fn, True)
                    fd_table[fd] = fu
                    fn_table[fn] = fu    

            elif entry.syscall_name == "close":
                fd = unquote_arg(entry.syscall_arguments[0])
                if fd in fd_table.keys():
                    del fd_table[fd]
                    
            elif entry.syscall_name == "write":
                fd = unquote_arg(entry.syscall_arguments[0])
                fu = fd_table.get(fd, None)
                if fu is not None:
                    fu.add_entry("w", unquote_arg(entry.syscall_arguments[1]))
            elif entry.syscall_name == "pwrite64":
                fd = unquote_arg(entry.syscall_arguments[0])
                fu = fd_table.get(fd, None)
                if fu is not None:
                    fu.add_entry("w", unquote_arg(entry.syscall_arguments[1]))  
            elif entry.syscall_name == "writev":
                fd = unquote_arg(entry.syscall_arguments[0])
                fu = fd_table.get(fd, None)
                if fu is not None:
                    fu.add_entry("w", "writev at "+str(entry.seq))    
            elif entry.syscall_name == "pwritev":
                fd = unquote_arg(entry.syscall_arguments[0])
                fu = fd_table.get(fd, None)
                if fu is not None:
                    fu.add_entry("w", "pwritev at "+str(entry.seq))   
                                                                             
            elif entry.syscall_name == "read":
                fd = unquote_arg(entry.syscall_arguments[0])
                fu = fd_table.get(fd, None)
                if fu is not None:
                    fu.add_entry("r", unquote_arg(entry.syscall_arguments[1]))
            elif entry.syscall_name == "pread64":
                fd = unquote_arg(entry.syscall_arguments[0])
                fu = fd_table.get(fd, None)
                if fu is not None:
                    fu.add_entry("r", unquote_arg(entry.syscall_arguments[1])) 
            elif entry.syscall_name == "readv":
                fd = unquote_arg(entry.syscall_arguments[0])
                fu = fd_table.get(fd, None)
                if fu is not None:
                    fu.add_entry("r", "readv at "+str(entry.seq)) 
            elif entry.syscall_name == "preadv":
                fd = unquote_arg(entry.syscall_arguments[0])
                fu = fd_table.get(fd, None)
                if fu is not None:
                    fu.add_entry("r", "preadv at "+str(entry.seq))     
            
            elif entry.syscall_name == "rename":
                fn = unquote_arg(entry.syscall_arguments[0])
                new_fn = unquote_arg(entry.syscall_arguments[1])
                fu = fn_table.get(fn, None)
                if fu is not None:
                    fu.name = new_fn     
#                else:
#                    print fn + " renamed to " + new_fn                    
                      
            elif entry.syscall_name == "unlink":
                fn = unquote_arg(entry.syscall_arguments[0])
                fu = fn_table.get(fn, None)
                if fu is not None:
                    fu.deleted = True
#                else:
#                    print fn + " deleted"
                     
        return fn_table
