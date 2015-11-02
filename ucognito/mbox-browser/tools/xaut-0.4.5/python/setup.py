from distutils.core import setup
from distutils.command.install import INSTALL_SCHEMES

for scheme in INSTALL_SCHEMES.values():
    scheme['data'] = scheme['purelib']

setup(
    name='xaut',
    version='0.4.5',
    description='Python library to manipulate X11 windows',
    author='Chris Parker',
    author_email='chrsprkr3@gmail.com',
    url='http://xautomation.sourceforge.net',
    license='Python License version 2.5 or later',
    platforms='X11 only',
    long_description='Simulate keystrokes, move windows, move the mouse, or manipulate the clipboard from Python',
    py_modules=['xaut'],
    data_files=[('', ['_xautpy.so'])]
    )
