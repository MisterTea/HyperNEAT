IF(WIN32)
ADD_DEFINITIONS(
	 -DSYS_WINDOWS
)
ELSE(WIN32)	
ADD_DEFINITIONS(
	 -DSYS_UNIX
)
ENDIF(WIN32)

ADD_LIBRARY(
CakeFixedDepthLib
STATIC

cake_eval.h
cake_misc.h
cakepp.h
consts.h
dblookup.h
initcake.h
move_gen.h
structs.h
switches.h

bookht.cpp
cake_eval.cpp
cake_misc.cpp
#cakedll.cpp
cakepp.cpp
dblookup.cpp
initcake.cpp
move_gen.cpp
spa.cpp
)

SET_TARGET_PROPERTIES(CakeFixedDepthLib PROPERTIES DEBUG_POSTFIX _d)

