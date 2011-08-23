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
CakeExe

book.h
cakepp.h
consts.h
db.h
movegen.h
structs.h
switches.h
#testcake.h

ansicake.c
book.c
cakepp.c
db.c
#interface.c
movegen.c
#testcake.c
)

ADD_LIBRARY(
CakeLib
STATIC

book.h
cakepp.h
consts.h
db.h
movegen.h
structs.h
switches.h
testcake.h

#ansicake.c
book.c
cakepp.c
db.c
interface.c
movegen.c
testcake.c
)

SET_TARGET_PROPERTIES(CakeLib PROPERTIES DEBUG_POSTFIX _d)

