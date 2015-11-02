#!/bin/bash

WORKSPACE=$(readlink -m "../../../")

URL_LIST=$WORKSPACE/url/urllist

# XXX please change
CHROME_BIN=

TRACE_LOG=$WORKSPACE/uverifier/chrome/strace/syscall.log

PROFILE_DIR=$WORKSPACE/uverifier/chrome/profile
PROFILE_EMPTY=$PROFILE_DIR/empty
PROFILE_PBTEST=$PROFILE_DIR/pbtest

STRACE_SET="open,close,read,write,mmap,stat,fstat,lstat,preadv,pwritev,pread64,pwrite64,readv,writev,access,dup,dup2,dup3,clone,fork,vfork,execve,truncate,ftruncate,mkdir,creat,link,unlink,symlink,readlink,rename,openat,mkdirat,mknodat,unlinkat,renameat,linkat,symlinkat,readlinkat,faccessat"

CHROME_LAUNCH_TIME=3
WEBPAGE_LOAD_TIME=20
LOG_BUFFERING_TIME=3

