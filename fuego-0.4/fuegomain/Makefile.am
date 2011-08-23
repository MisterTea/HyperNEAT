bin_PROGRAMS = fuego

fuego_SOURCES = \
FuegoMain.cpp \
FuegoMainEngine.cpp \
FuegoMainUtil.cpp

noinst_HEADERS = \
FuegoMainEngine.h \
FuegoMainUtil.h

fuego_LDFLAGS = $(BOOST_LDFLAGS)

fuego_LDADD = \
../gouct/libfuego_gouct.a \
../go/libfuego_go.a \
../smartgame/libfuego_smartgame.a \
../gtpengine/libfuego_gtpengine.a \
$(BOOST_PROGRAM_OPTIONS_LIB) \
$(BOOST_FILESYSTEM_LIB) \
$(BOOST_SYSTEM_LIB) \
$(BOOST_THREAD_LIB)

fuego_DEPENDENCIES = \
../gouct/libfuego_gouct.a \
../go/libfuego_go.a \
../smartgame/libfuego_smartgame.a \
../gtpengine/libfuego_gtpengine.a

fuego_CPPFLAGS = \
-DDATADIR='"@datadir@"' \
-DABS_TOP_SRCDIR='"@abs_top_srcdir@"' \
$(BOOST_CPPFLAGS) \
-I@top_srcdir@/gtpengine \
-I@top_srcdir@/smartgame \
-I@top_srcdir@/go \
-I@top_srcdir@/gouct

DISTCLEANFILES = *~
