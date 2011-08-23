IF(WIN32)
ADD_DEFINITIONS(
	 -DSYS_WINDOWS
)
ELSE(WIN32)	
ADD_DEFINITIONS(
	 -DSYS_UNIX
)
ENDIF(WIN32)

ADD_EXECUTABLE(
ClicheExe

simplech.h

cliche.c
simplech.c
)

SET_TARGET_PROPERTIES(ClicheExe PROPERTIES DEBUG_POSTFIX _d)


ADD_LIBRARY(
ClicheLib
STATIC

simplech.h

cliche.c
simplech.c
)

SET_TARGET_PROPERTIES(ClicheLib PROPERTIES DEBUG_POSTFIX _d)

