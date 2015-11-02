#pragma once

static struct sock_filter filter[] = {
    LD_SYSCALL,

    // File related
    TRACE_SYSCALL(open),
    TRACE_SYSCALL(openat),
    TRACE_SYSCALL(stat),
    TRACE_SYSCALL(lstat),
    TRACE_SYSCALL(newfstatat),
    TRACE_SYSCALL(mkdir),
    TRACE_SYSCALL(mkdirat),
    TRACE_SYSCALL(rmdir),
    TRACE_SYSCALL(unlink),
    TRACE_SYSCALL(unlinkat),
    //10

    TRACE_SYSCALL(access),
    TRACE_SYSCALL(faccessat),
    TRACE_SYSCALL(getdents),
    TRACE_SYSCALL(getcwd),
    TRACE_SYSCALL(chdir),
    TRACE_SYSCALL(creat),
    TRACE_SYSCALL(utimensat),
    TRACE_SYSCALL(utime),
    TRACE_SYSCALL(chmod),
    TRACE_SYSCALL(chown),
    // 20

    TRACE_SYSCALL(lchown),
    TRACE_SYSCALL(execve),
    TRACE_SYSCALL(truncate),
    TRACE_SYSCALL(rename),
    TRACE_SYSCALL(renameat),
    TRACE_SYSCALL(link),
    TRACE_SYSCALL(symlink),
    TRACE_SYSCALL(readlink),
    TRACE_SYSCALL(mknod),
    TRACE_SYSCALL(readlinkat),
    //30

    TRACE_SYSCALL(fchmodat),
    TRACE_SYSCALL(linkat),
    TRACE_SYSCALL(symlinkat),
    TRACE_SYSCALL(mknodat),
    TRACE_SYSCALL(fchownat),
    TRACE_SYSCALL(futimesat),
    TRACE_SYSCALL(utimes),
    TRACE_SYSCALL(uselib),
    TRACE_SYSCALL(statfs),
    TRACE_SYSCALL(acct),
    // 40

    TRACE_SYSCALL(setxattr),
    TRACE_SYSCALL(lsetxattr),
    TRACE_SYSCALL(getxattr),
    TRACE_SYSCALL(lgetxattr),
    TRACE_SYSCALL(removexattr),
    TRACE_SYSCALL(lremovexattr),
    TRACE_SYSCALL(listxattr),
    TRACE_SYSCALL(llistxattr),
    // 48

    // file -related end

    //TRACE_SYSCALL(socket),
    TRACE_SYSCALL(bind),        //49
    //TRACE_SYSCALL(connect),
    //TRACE_SYSCALL(getuid),
    //TRACE_SYSCALL(getgid),
    //TRACE_SYSCALL(geteuid),
    //TRACE_SYSCALL(getegid),
    TRACE_SYSCALL(fchown),      //50
    //TRACE_SYSCALL(prctl),
    //TRACE_SYSCALL(mprotect),
    //TRACE_SYSCALL(mmap),
    //TRACE_SYSCALL(mremap),
    //TRACE_SYSCALL(brk),
    ALLOWED,
};