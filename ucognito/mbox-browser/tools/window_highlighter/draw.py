#!/usr/bin/env python


import os;
import sys;
import xaut;
import time;

def draw_border_at(x, y, w, h, r, g, b):
    path = "/".join(os.path.realpath(__file__).split('/')[0:-1])
    v = 10
    x += v;
    y += v;
    w -= 2*v
    h -= 2*v
    os.system("cd " + path + "; java Main " + ("%d %d %d %d 1 %d %d %d" %
        (x, y, w, h, r, g, b)) + "&");

def remove_all_borders():
    os.system("killall -9 java");

def check_window(name):
    wins = xaut.window.search_for_window(name)
    print wins
    chosen_window = None
    for window in wins:
        if window.is_active():
            chosen_window = window
    if chosen_window == None:
        return False
    x = chosen_window.x()
    y = chosen_window.y()
    w = chosen_window.w()
    h = chosen_window.h()
    remove_all_borders()
    draw_border_at(x, y, w, h, 255, 255, 0)
    return True

while True:
    time.sleep(2)
    flag = False
    flag |= check_window('Firefox')
    flag |= check_window('Chromium')
    if flag:
        pass
    else:
        remove_all_borders()

if len(sys.argv) != 9:
    quit();
data = " ".join(sys.argv[1:])
path = "/".join(os.path.realpath(__file__).split('/')[0:-1])

os.system("cd " + path + "; java Main " + data);
