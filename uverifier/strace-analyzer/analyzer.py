'''
Created on Apr 30, 2015

@author: meng
'''
from strace import StraceLog
from consolidator import ProcessAnalyzer

def get_fu_group_incremental(fu_p_new, fu_p_old):
    fu_p_i = dict()
    
    for pid in fu_p_new.keys():
        if pid not in fu_p_old.keys():
            fu_p_i[pid] = fu_p_new[pid]
        else:
            fu_p_i[pid] = get_fu_incremental(fu_p_new[pid], fu_p_old[pid])
            
    return fu_p_i

def get_fu_incremental(fu_new, fu_old):

    fu_i = dict()
    
    for fn in fu_new.keys():
        if fn not in fu_old.keys():
            fu_i[fn] = fu_new[fn]
        else:
            fu_n = fu_new[fn]
            fu_o = fu_old[fn]
            if not fu_o.deleted:
                if fu_n.rw != fu_o.rw:
                    fu_i[fn] = fu_n
                else:
                    if len(fu_n.r_entries) != len(fu_o.r_entries) or \
                    len(fu_n.w_entries) != len(fu_o.w_entries):
                        fu_i[fn] = fu_n
                    
    return fu_i

def analyze_intra_session(filepath):
    log = StraceLog(filepath)
    fu = ProcessAnalyzer(log).fileusages
    
    for pid in fu.keys():
        for fn in fu[pid].keys():
            if fu[pid][fn].rw and not fu[pid][fn].deleted and \
            len(fu[pid][fn].w_entries)>0:
                print fn
                for entry in fu[pid][fn].w_entries:
                    print "<<<" + entry

def analyze_inter_session(filepath0, filepath1, filepath2):
    log0 = StraceLog(filepath0)
    log1 = StraceLog(filepath1)
    log2 = StraceLog(filepath2)
    
    fu0 = ProcessAnalyzer(log0).fileusages
    fu1 = ProcessAnalyzer(log1).fileusages
    fu2 = ProcessAnalyzer(log2).fileusages
    
    fu_10 = get_fu_group_incremental(fu1, fu0)
    fu_21 = get_fu_group_incremental(fu2, fu1)
    
    for pid in fu_10.keys():
        for fn in fu_10[pid].keys():
            if fu_10[pid][fn].rw and not fu_10[pid][fn].deleted and \
            len(fu_10[pid][fn].w_entries)>0:
                for pid2 in fu_21.keys():
                    if fn in fu_21[pid2].keys() and len(fu_21[pid2][fn].r_entries)>0:
                        print fn
#                        fu_w = fu_10[pid][fn]
#                        for entry in fu_w.w_entries:
#                            print "<<< "+entry
#                        fu_r = fu_21[pid2][fn]
#                        for entry in fu_r.r_entries:
#                            print ">>> "+entry