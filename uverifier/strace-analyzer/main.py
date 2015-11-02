'''
Created on Apr 30, 2015

@author: meng
'''

import sys
import os

from analyzer import analyze_inter_session
from analyzer import analyze_intra_session
from settings import LOG_DIR
from settings import URL_LIST

def analyze_list(t):
    if t == "intra":
        with open(URL_LIST) as f:
            url_list = f.readlines()
            i = 0
            for url in url_list:
                url = url[:-1] # trim \n
                print "analyzing syscall log " + str(i) + ", url " + url
                log_path = os.path.join(LOG_DIR, "syscall.log-"+str(i))
                analyze_intra_session(log_path)
                i = i+1
    elif t == "inter":
        with open(URL_LIST) as f:
            url_list = f.readlines()
            i = 0
            for url in url_list:
                url = url[:-1] # trim \n
                print "analyzing syscall log " + str(i) + ", url " + url
                log_path0 = os.path.join(LOG_DIR, "syscall.log-"+str(i)+"-s0")
                log_path1 = os.path.join(LOG_DIR, "syscall.log-"+str(i)+"-s1")
                log_path2 = os.path.join(LOG_DIR, "syscall.log-"+str(i)+"-s2")
                analyze_inter_session(log_path0, log_path1, log_path2)
                i = i+1
                
def analyze_single(t, name):
    if t == "intra":
        log_path = os.path.join(LOG_DIR, "syscall.log-"+name)
        analyze_intra_session(log_path)
    elif t == "inter":
        log_path0 = os.path.join(LOG_DIR, "syscall.log-"+name+"-s0")
        log_path1 = os.path.join(LOG_DIR, "syscall.log-"+name+"-s1")
        log_path2 = os.path.join(LOG_DIR, "syscall.log-"+name+"-s2")    
        analyze_inter_session(log_path0, log_path1, log_path2)


if __name__ == '__main__':
    if sys.argv[1] == "s":
        analyze_single(sys.argv[2], sys.argv[3])
    elif sys.argv[1] == "l":
        analyze_list(sys.argv[2])
    
