#!/bin/bash

WORKSPACE=$(readlink -m "../../")

URL_LIST=$WORKSPACE/url/urllist

FIREFOX_BIN=$WORKSPACE/firefox/build/firefox

TRACE_LOG=$WORKSPACE/firefox/strace/syscall.log

PROFILE_DIR=$WORKSPACE/firefox/profile
PROFILE_EMPTY=$PROFILE_DIR/empty
PROFILE_PBTEST=$PROFILE_DIR/pbtest

STRACE_SET="open,close,read,write,mmap,stat,fstat,lstat,preadv,pwritev,pread64,pwrite64,readv,writev,access,dup,dup2,dup3,clone,fork,vfork,execve,truncate,ftruncate,mkdir,creat,link,unlink,symlink,readlink,rename,openat,mkdirat,mknodat,unlinkat,renameat,linkat,symlinkat,readlinkat,faccessat"

FIREFOX_LAUNCH_TIME=3
WEBPAGE_LOAD_TIME=20
LOG_BUFFERING_TIME=3

