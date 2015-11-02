#This is a heavily modified version of the .py generated
#by swig.  Virtually all of the native C functions have
#translated versions as opposed to direct translation.

import _xautpy
import new
new_instancemethod = new.instancemethod
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types

#Clipboard functions - Uses the following native calls:
#extern char* get_primary();
#extern char* get_secondary();
#extern char* get_clipboard();
#extern void put_primary(char*);
#extern void put_secondary(char*);
#extern void put_clipboard(char*);
#extern void clear_primary();
#extern void clear_secondary();
#extern void clear_clipboard();
class clipboard:
    def put_primary(self, str):
        _xautpy.put_primary(str)
    def put_secondary(self, str):
        _xautpy.put_secondary(str)
    def put_clipboard(self, str):
        _xautpy.put_clipboard(str)
    def get_primary(self):
        return _xautpy.get_primary()
    def get_secondary(self):
        return _xautpy.get_secondary()
    def get_clipboard(self):
        return _xautpy.get_clipboard()
    def __init__(self):
        pass

#Keyboard functions - Uses the following native calls:
# extern short caps_lock_on();
# extern short caps_lock_off();
# extern short num_lock_on();
# extern short num_lock_off();
# extern short scroll_lock_on();
# extern short scroll_lock_off();
# extern short is_caps_lock();
# extern short is_num_lock();
# extern short is_scroll_lock();
# extern void key_down_delay(unsigned short);
# extern void key_click_delay(unsigned short);
# extern unsigned short key_click(unsigned int);
# extern unsigned short key_down(unsigned int);
# extern unsigned short key_up(unsigned int);
# extern unsigned short type(char*);
# extern void interpret_meta_symbols(unsigned short);
# extern void print_keycodes();
class keyboard:
    def capslock(self, tf = -1):
        if tf == -1:
            pass
        elif tf == 0:
            _xautpy.caps_lock_off()
        else:
            _xautpy.caps_lock_on()
        return _xautpy.is_caps_lock()
    def numlock(self, tf = -1):
        if tf == -1:
            pass
        elif tf == 0:
            _xautpy.num_lock_off()
        else:
            _xautpy.num_lock_on()
        return _xautpy.is_num_lock()
    def scroll_lock(self, tf = -1):
        if tf == -1:
            pass
        elif tf == 0:
            _xautpy.scroll_lock_off()
        else:
            _xautpy.scroll_lock_on()
        return _xautpy.is_scroll_lock()
    def down_delay(self, delay = -1):
        num = int(delay)
        return _xautpy.key_down_delay(num)
    def click_delay(self, delay = -1):
        num = int(delay)
        return _xautpy.key_click_delay(delay)
    def click(self, key):
        num = int(key)
        return _xautpy.key_click(num)
    def down(self, key):
        num = int(key)
        return _xautpy.key_down(num)
    def up(self, key):
        num = int(key)
        return _xautpy.key_up(num)
    def type(self, str):
        return _xautpy.type(str)
    def interpret_meta_symbols(self, tf):
        _xautpy.interpret_meta_symbols(tf)
    def print_keycodes(self):
        _xautpy.print_keycodes()
    def __init__(self):
        pass

#Mouse functions - uses the following native calls:
# extern void mouse_move_delay(unsigned short);
# extern void mouse_down_delay(unsigned short);
# extern void mouse_click_delay(unsigned short);
# extern unsigned short mouse_click(unsigned short,...);
# extern unsigned short mouse_down(unsigned short);
# extern unsigned short move_mouse(int, int, ...);
# extern unsigned short mouse_up(unsigned short);
# extern int mouse_x(unsigned long);
# extern int mouse_y(unsigned long);
class mouse:
    def move_delay(self, delay = -1):
        num = int(delay)
        return _xautpy.mouse_move_delay(num)
    def down_delay(self, delay = -1):
        num = int(delay)
        return _xautpy.mouse_down_delay(num)
    def click_delay(self, delay = -1):
        num = int(delay)
        return _xautpy.mouse_click_delay(num)
    def click(self, btn, cnt = 1):
        numBtn = int(btn)
        numCnt = int(cnt)
        return _xautpy.mouse_click(numBtn, numCnt)
    def btn_down(self, btn):
        num = int(btn)
        return _xautpy.mouse_down(btn)
    def btn_up(self, btn):
        num = int(btn)
        return _xautpy.mouse_up(btn)
    def move(self, x, y, win = 0):
        numX = int(x)
        numY = int(y)
        win_id = 0
        if(win):
            win_id = win.id
        return _xautpy.move_mouse(numX, numY, win_id)
    def x(self, win = 0):
        win_id = 0
        if(win):
            win_id = win.id
        return _xautpy.mouse_x(win_id)
    def y(self, win = 0):
        win_id = 0
        if(win):
            win_id = win.id
        return _xautpy.mouse_y(win_id)
    def __init__(self):
        pass

#Display functions - uses the following native calls
# extern int display_h();
# extern int display_w();
# extern long desktop_count();
# extern long desktop(long);
class display:
    def w(self):
        return _xautpy.display_w()
    def h(self):
        return _xautpy.display_h()
    def desktop(self, desk = -1):
        return _xautpy.desktop(desk)
    def desktop_count(self):
        return _xautpy.desktop_count()
    def __init__(self):
        pass

#Window functions - uses the following native calls
# extern unsigned short activate_window(unsigned long);
# extern unsigned long active_window();
# extern unsigned long find_window(char*);
# extern unsigned long find_outer_parent(unsigned long);
# extern unsigned long search_for_window(char*);
# extern unsigned short maximize_window(unsigned long, ...);
# extern unsigned short maximize_window_horz(unsigned long, ...);
# extern unsigned short maximize_window_vert(unsigned long, ...);
# extern unsigned short minimize_window(unsigned long, ...);
# extern unsigned short iconify_window(unsigned long, ...);
# extern unsigned short full_screen_window(unsigned long, ...);
# extern unsigned short restore_window(unsigned long);
# extern unsigned short shade_window(unsigned long, ...);
# extern unsigned short move_window(unsigned long, int, int, ...);
# extern unsigned short resize_window(unsigned long, unsigned int, unsigned int);
# extern int window_x(unsigned long);
# extern int window_y(unsigned long);
# extern int window_w(unsigned long);
# extern int window_h(unsigned long);
# extern char* window_name(unsigned long);
# extern long window_desktop(unsigned long);
class window:
    @staticmethod
    def active_window():
        id = _xautpy.active_window()
        if(id):
            win = window(id)
            return win
        else:
            return None
    @staticmethod
    def find_window(title):
        id = _xautpy.find_window(title)
        if(id):
            win = window(id)
            return win
        else:
            return None
    @staticmethod
    def search_for_window(name):
        ids = _xautpy.search_for_window(name)
        wins = []
        for i in range(len(ids)):
            win = window(ids[i])
            wins.append(win)
        return wins
    @staticmethod
    def wait_exists(title, timeout = 0):
        import time
        sleep = .05
        start = time.time()
        while(1):
            win = window.find_window(title)
            if(win):
                return win
            else:
                #A crude timeout function.  Using threads and interrupts
                #is too complicated for such a simple loop.
                if(timeout > 0):
                    now = time.time()
                    if((now - start) >= timeout):
                        return None
            time.sleep(sleep)

    def is_valid(self):
        return _xautpy.is_valid(self.id)
    def is_active(self):
        active_id = _xautpy.active_window()
        return _xautpy.find_outer_parent(active_id) == _xautpy.find_outer_parent(self.id)
    def activate(self):
        return _xautpy.activate_window(self.id)
    def find_outer_parent(self):
        op_id = _xautpy.find_outer_parent(self.id)
        if op_id > 0:
            return window(op_id)
        else:
            return None
    def maximize(self, tf = 1):
        return _xautpy.maximize_window(self.id, tf)
    def maximize_horz(self, tf = 1):
        return _xautpy.maximize_window_horz(self.id, tf)
    def maximize_vert(self, tf = 1):
        return _xautpy.maximize_window_vert(self.id, tf)
    def minimize(self, tf = 1):
        return _xautpy.minimize_window(self.id, tf)
    def iconify(self, tf = 1):
        return _xautpy.iconify_window(self.id, tf)
    def full_screen(self, tf = 1):
        return _xautpy.full_screen_window(self.id, tf)
    def restore(self):
        return _xautpy.restore_window(self.id)
    def shade(self, tf = 1):
        return _xautpy.shade_window(self.id, tf)
    def move(self, x, y, desk = -1):
        numX = int(x)
        numY = int(y)
        numDesk = long(desk)
        return _xautpy.move_window(self.id, numX, numY, numDesk)
    def resize(self, w, h):
        numW = int(w)
        numH = int(h)
        return _xautpy.resize_window(self.id, numW, numH)
    def x(self):
        return _xautpy.window_x(self.id)
    def y(self):
        return _xautpy.window_y(self.id)
    def w(self):
        return _xautpy.window_w(self.id)
    def h(self):
        return _xautpy.window_h(self.id)
    def name(self):
        return _xautpy.window_name(self.id)
    def desktop(self):
        return _xautpy.window_desktop(self.id)
    def info(self):
        inf = [0, 0, 0, 0, "", 0, 0] #Note to self: window info pattern...
        inf[0] = self.x()
        inf[1] = self.y()
        inf[2] = self.w()
        inf[3] = self.h()
        inf[4] = self.name()
        inf[5] = self.desktop()
        inf[6] = self.id
        return tuple(inf)
    def wait_active(self, timeout = 0):
        import time
        sleep = .05
        start = time.time()
        parent_id = _xautpy.find_outer_parent(self.id)
        while(1):
            active_id = _xautpy.active_window()
            active_parent_id = _xautpy.find_outer_parent(active_id)
            if(parent_id == active_parent_id):
                return True
            #A crude timeout function.  Using threads and interrupts
            #is too complicated for such a simple loop.
            if(timeout > 0):
                now = time.time()
                if((now - start) >= timeout):
                    return False
            time.sleep(sleep)
    def __init__(self, id):
        self.id = id

#Miscellaneous other calls which don't fit neatly in objects -
#uses the following native calls
# extern void cleanup();
# extern void verbose(unsigned short);
# extern void extra_verbose(unsigned short);
# extern unsigned short init_defaults();
def cleanup():
    return _xautpy.cleanup()

def init_defaults():
    return _xautpy.init_defaults()

def verbose(tf = 1):
    return _xautpy.verbose(tf)

def extra_verbose(tf = 1):
    return _xautpy.extra_verbose(tf)
