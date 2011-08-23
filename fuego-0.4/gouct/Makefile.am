noinst_LIBRARIES = libfuego_gouct.a

libfuego_gouct_a_SOURCES = \
GoUctBoard.cpp \
GoUctCommands.cpp \
GoUctDefaultPriorKnowledge.cpp \
GoUctDefaultRootFilter.cpp \
GoUctEstimatorStat.cpp \
GoUctGlobalSearch.cpp \
GoUctObjectWithSearch.cpp \
GoUctPlayoutPolicy.cpp \
GoUctRootFilter.cpp \
GoUctSearch.cpp \
GoUctUtil.cpp

noinst_HEADERS = \
GoUctBoard.h \
GoUctBookBuilder.h \
GoUctBookBuilderCommands.h \
GoUctCommands.h \
GoUctDefaultPriorKnowledge.h \
GoUctDefaultRootFilter.h \
GoUctEstimatorStat.h \
GoUctGlobalSearch.h \
GoUctObjectWithSearch.h \
GoUctPatterns.h \
GoUctPlayer.h \
GoUctPlayoutPolicy.h \
GoUctPureRandomGenerator.h \
GoUctRootFilter.h \
GoUctSearch.h \
GoUctUtil.h

libfuego_gouct_a_CPPFLAGS = \
$(BOOST_CPPFLAGS) \
-I@top_srcdir@/gtpengine \
-I@top_srcdir@/smartgame \
-I@top_srcdir@/go

DISTCLEANFILES = *~
