Project(zlib)

IF(MSVC)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc /RTCsu /W3")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /EHsc /RTCsu /W3")

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc /DRELEASE_MODE /fp:fast")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /EHsc /DRELEASE_MODE /fp:fast")

SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /EHsc /fp:fast")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /EHsc /fp:fast")

SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /EHsc")
SET(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /EHsc")

#The above code does not work for setting the fp mode, try this
add_definitions(/fp:fast)

ELSE(MSVC)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -stdlib=libstdc++ -rdynamic -g -Wall -Wno-unused-variable -fPIC")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -stdlib=libstdc++ -rdynamic -g -Wall -Wno-unused-variable -fPIC")

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -stdlib=libstdc++ -rdynamic -g -DRELEASE_MODE -DNDEBUG -fPIC")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -stdlib=libstdc++ -rdynamic -g -DRELEASE_MODE -DNDEBUG -fPIC")

SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -stdlib=libstdc++ -rdynamic -g -DNDEBUG -fPIC")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -stdlib=libstdc++ -rdynamic -g -DNDEBUG -fPIC")

ENDIF(MSVC)

IF(NOT MYPROJ_RAN_ONCE)
   SET(MYPROJ_RAN_ONCE 1 CACHE INTERNAL "")

	SET(
	CMAKE_INSTALL_PREFIX
	"${CMAKE_SOURCE_DIR}/out/"
	CACHE
	PATH
	"Install path prefix, prepended onto install directories."
	FORCE
	)

	SET(
	EXECUTABLE_OUTPUT_PATH
	"${CMAKE_SOURCE_DIR}/out/"
	CACHE
	PATH
	"Single output directory for building all executables."
	FORCE
	)

	SET(
	LIBRARY_OUTPUT_PATH
	"${CMAKE_SOURCE_DIR}/out/"
	CACHE
	PATH
	"Single output directory for building all libraries."
	FORCE
	)

	SET(
	CMAKE_BUILD_TYPE
	"Release"
	CACHE
	STRING
	"Description: Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
	FORCE
	)

ENDIF(NOT MYPROJ_RAN_ONCE)

INCLUDE_DIRECTORIES(
	./
)

ADD_LIBRARY(
	z
	SHARED
	
	crc32.h
	deflate.h
	inffast.h
	inffixed.h
	inflate.h
	inftrees.h
	trees.h
	zconf.h
	zconf.in.h
	zlib.h
	zutil.h

	adler32.c
	compress.c
	crc32.c
	deflate.c
	gzio.c
	infback.c
	inffast.c
	inflate.c
	inftrees.c
	trees.c
	uncompr.c
	zutil.c
)

SET_PROPERTY(TARGET z APPEND PROPERTY COMPILE_DEFINITIONS ZLIB_DLL)

SET_TARGET_PROPERTIES(z PROPERTIES DEBUG_POSTFIX _d)


ADD_LIBRARY(
	zlib
	STATIC
	
	crc32.h
	deflate.h
	inffast.h
	inffixed.h
	inflate.h
	inftrees.h
	trees.h
	zconf.h
	zconf.in.h
	zlib.h
	zutil.h

	adler32.c
	compress.c
	crc32.c
	deflate.c
	gzio.c
	infback.c
	inffast.c
	inflate.c
	inftrees.c
	trees.c
	uncompr.c
	zutil.c
)

SET_TARGET_PROPERTIES(zlib PROPERTIES DEBUG_POSTFIX _d)

