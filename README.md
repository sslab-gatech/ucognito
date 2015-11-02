private-browsing
================

Evaluation and Improvements on "Incognito" Browsing

preparation
-----------

0. (optional) install firefox at firefox/build/
1. modify firefox/script/settings.sh
2. firefox/script/create-empty-profile

firefox
-------

1. use start/stop script to start/stop the browser
(it will always use the empty profile created)
2. use visit-normal/visit-private <URL> to browse
3. use strace to trace file related syscalls
4. log available at firefox/strace/syscall.log
