dnl Process this file with autoconf to produce a configure script.

AC_INIT
AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([go/GoBoard.cpp])
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([fuego], [0.4.1.SVN])
AM_MAINTAINER_MODE
AC_PROG_CXX
AC_PROG_RANLIB
AX_BOOST_BASE([1.33.1])
AX_BOOST_THREAD
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_UNIT_TEST_FRAMEWORK


dnl Check for __sync_synchronize
dnl
AC_MSG_CHECKING([for __sync_synchronize])
AC_LINK_IFELSE([int main (int argc, char** argv) {__sync_synchronize();}],
	       [has_synchronize=yes],
	       [has_synchronize=no])
AC_MSG_RESULT([$has_synchronize])
		
if test "x$has_synchronize" = "xyes"
then
	AC_DEFINE(HAVE_SYNC_SYNCHRONIZE,1,"define if __sync_synchronize() is available")
fi


dnl TODO: Find a better (more portable) way to enable more optimizations by
dnl default.
AC_ARG_ENABLE([optimize],
	      AS_HELP_STRING([--enable-optimize], [set CXXFLAGS to -O3 -g (default is yes)]),
	      [optimize=$enableval],
	      [optimize=yes])
if test "x$optimize" = "xyes"
then
	CXXFLAGS="-O3 -g"
fi

AC_ARG_ENABLE([assert],
	      AS_HELP_STRING([--enable-assert], [enable assertions (default is no)]),
	      [assert=$enableval],
	      [assert=no])
if test "x$assert" = "xno"
then
	CXXFLAGS="$CXXFLAGS -DNDEBUG"
else
	CXXFLAGS="$CXXFLAGS"
fi


dnl ./configure switch to enable explicit synchronization of CPU caches.
dnl
AC_ARG_ENABLE([cache-sync],
	      AS_HELP_STRING([--enable-cache-sync],
	      [Enable explicit synchronization of the CPU cache.
	      Required for most non-Intel architectures to support
	      lock-free multi-threading (default is no)]),
	      [cachesync=$enableval],
	      [cachesync=no])

if test "x$cachesync" = "xyes"
then
	if test "x$has_synchronize" = "xyes"
	then 
		AC_DEFINE(ENABLE_CACHE_SYNC, 1, [define to enable explicit synchronization of the CPU cache])
	else
		AC_MSG_ERROR(Explicit cache synchronization requires __sync_synchronize() builtin);	   fi

fi


AX_CXXFLAGS_WARN_ALL
AX_CXXFLAGS_GCC_OPTION(-Wextra)

AC_OUTPUT([Makefile book/Makefile regression/Makefile misctests/Makefile fuegomain/Makefile fuegotest/Makefile go/Makefile gouct/Makefile gtpengine/Makefile simpleplayers/Makefile smartgame/Makefile unittestmain/Makefile])
