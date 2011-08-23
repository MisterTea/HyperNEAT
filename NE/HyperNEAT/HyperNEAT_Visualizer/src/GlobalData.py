import os,platform,sys

VERSION_NUMBER = 13

#0 = no fake OS
#1 = Linux 32-bit
OS_TYPE_LINUX = 1;
#3 = Mac OS/X
OS_TYPE_OSX = 2;
#4 = Windows
OS_TYPE_WINDOWS = 3;

is64bit = False

osType = 0;

USE_GUI = True

BASE_DIR = os.path.split(sys.argv[0])[0]
if len(BASE_DIR)==0:
    BASE_DIR="./"

if platform.architecture()[0][0:2]=='32':
    is64bit = False
else:
    is64bit = True

if str.upper(sys.platform[0:3])=='WIN' \
or str.upper(sys.platform[0:3])=='CYG':
    print 'In windows'
    osType = OS_TYPE_WINDOWS
elif str.upper(sys.platform[0:3])=='DAR':
    osType = OS_TYPE_OSX
    BASE_DIR = os.path.split(BASE_DIR)[0]
    BASE_DIR = os.path.split(BASE_DIR)[0]
    BASE_DIR = os.path.split(BASE_DIR)[0]
else:
    osType = OS_TYPE_LINUX
        
def ReplaceTags(text):
    return text.replace("$APPLICATION$","Underworld Hockey Club")


