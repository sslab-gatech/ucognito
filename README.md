private-browsing
================

Evaluation and Improvements on "Incognito" Browsing

ucognito: build
---------------
1. cd ucognito/mbox-browser/
2. follow the README file to build and install mbox-browser
3. (optional) sudo ln -s ucognito/ucognito /usr/bin/ucognito

ucognito: launch
----------------
1. ucognito -P <path-to-profile> -- <browser-bin>

uverifier: preparation
----------------------

0. (optional) install firefox at uverifier/firefox/build/
1. modify uverifier/firefox/script/settings.sh
2. uverifier/firefox/script/create-empty-profile

uverifier: launch (firefox)
---------------------------

1. use start/stop script to start/stop the browser
(it will always use the empty profile created)
2. use visit-normal/visit-private <URL> to browse
3. use strace to trace file related syscalls
4. log available at firefox/strace/syscall.log
