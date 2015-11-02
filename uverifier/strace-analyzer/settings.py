'''
Created on Apr 26, 2015

@author: meng
'''

import os

URL_LIST=os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "url", "urllist")

# for firefox
#LOG_DIR=os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "firefox", "strace")
#PROFILE_DIR=os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "firefox", "profile")

# for chrome
LOG_DIR=os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "chrome", "strace")
PROFILE_DIR=os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "chrome", "profile")

