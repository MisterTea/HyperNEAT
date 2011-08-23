TinyXMLDLL,
By Jason Gauci
http://ucfpawn.homelinux.com/joomla/
jgmath2000@gmail.com

Original TinyXML by Lee Thomason
http://www.grinninglizard.com/tinyxml/

1. Instructions
2. Release Notes

---------------
1. INSTRUCTIONS
---------------
UNIX/LINUX/CYGWIN/MACOSX:

Inside the zlib/build directory, create two subdirectories, one for debug and one for release:

mkdir zlib/build/cygwin_debug
mkdir zlib/build/cygwin_release

Now, run cmake on each of those directories with the -i option, and put the appropriate configuration (debug for *_debug, release for *_release).

Now, just run "make" on the directories and the dlls & libraries should be created in the zlib/out/ folder

Now, do the same steps as above but for the tinyxmldll directory instead of the zlib directory

WINDOWS:
Inside the zlib/build directory, create a subdirectory:

zlib/build/msvc8

Now, run cmake on that subdirectory.

Then, open the project and build a debug and release version.

Now, do the same steps as above but for the tinyxmldll directory instead of the zlib directory


----------------
2. RELEASE NOTES
----------------

Version 2.0:

Added support for creating gzip files using zlib.

Bug fixes.

Version 1.0:

Initial Release
