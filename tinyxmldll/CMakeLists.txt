Project(tinyxmldll)

cmake_minimum_required(VERSION 2.6)

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


ADD_DEFINITIONS(
-DTIXML_USE_STL
-D_USE_MATH_DEFINES
-DTINYXMLDLL_EXPORTS
)

SET(
	ZLIB_INCLUDE
	"../zlib"
	CACHE
	PATH 
	"zlib include Directory"
)

INCLUDE_DIRECTORIES(
include
${ZLIB_INCLUDE}
)

SET(
	ZLIB_LIB
	"${CMAKE_CURRENT_SOURCE_DIR}/../zlib/out"
	CACHE
	PATH 
	"zlib Library directory"
	)

LINK_DIRECTORIES(
	${ZLIB_LIB}
	)

ADD_LIBRARY(
	tinyxmlplus 
	SHARED

	include/tinyxmlplus.h

	src/tinyxmlplus.cpp
	src/tinyxmlparserplus.cpp
	src/tinyxmlerrorplus.cpp
)

TARGET_LINK_LIBRARIES(
	tinyxmlplus

	debug
	zlib_d

	optimized
	zlib
	)

SET_PROPERTY(TARGET tinyxmlplus APPEND PROPERTY COMPILE_DEFINITIONS TINYXML_USE_DLL)

SET_TARGET_PROPERTIES(tinyxmlplus PROPERTIES DEBUG_POSTFIX _d)

ADD_LIBRARY(
	tinyxmlpluslib
	STATIC

	include/tinyxmlplus.h

	src/tinyxmlplus.cpp
	src/tinyxmlparserplus.cpp
	src/tinyxmlerrorplus.cpp
)

SET_TARGET_PROPERTIES(tinyxmlpluslib PROPERTIES DEBUG_POSTFIX _d)
