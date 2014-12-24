project(NEAT)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(CMAKE_MODULE_PATH 
  "${CMAKE_CURRENT_SOURCE_DIR}/"
)
include(PCHSupport)

IF(MSVC)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc /RTCsu /W3 /Zm1000")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /EHsc /RTCsu /W3 /Zm1000")

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc /DRELEASE_MODE /fp:fast /arch:SSE2 /Zm1000")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /EHsc /DRELEASE_MODE /fp:fast /arch:SSE2 /Zm1000")

SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /EHsc /fp:fast /arch:SSE2 /Zm1000")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /EHsc /fp:fast /arch:SSE2 /Zm1000")

SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /EHsc /Zm1000")
SET(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /EHsc /Zm1000")

#The above code does not work for setting the fp mode, try this
add_definitions(/fp:fast)

ELSE(MSVC)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -stdlib=libstdc++ -rdynamic -g -Wall -Wno-unused-variable -fPIC")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -stdlib=libstdc++ -rdynamic -g -Wall -Wno-unused-variable -fPIC")

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -stdlib=libstdc++ -rdynamic -g -DRELEASE_MODE -msse2 -DNDEBUG -fPIC")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -stdlib=libstdc++ -rdynamic -g -DRELEASE_MODE -msse2 -DNDEBUG -fPIC")

SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -stdlib=libstdc++ -rdynamic -g -msse2 -DNDEBUG -fPIC")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -stdlib=libstdc++ -rdynamic -g -msse2 -DNDEBUG -fPIC")

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

cmake_minimum_required(VERSION 2.6)

SET(
	BUILD_PYTHON
	OFF
	CACHE 
	BOOL
	"Build the python extension?"
	)

SET(
	BUILD_MPI
	OFF 
	CACHE 
	BOOL
	"Build the MPI extension?"
	)

SET(
	BUILD_GPU
	OFF
	CACHE 
	BOOL
	"Build the GPU ANN implementation"
	)

IF(BUILD_GPU)
	ADD_DEFINITIONS(
		-DUSE_GPU
	)
ENDIF(BUILD_GPU)

#subdirs(cake-1.20)
subdirs(cake_fixeddepth)
subdirs(cliche-1.2 NEAT Hypercube_NEAT)
subdirs(GoPlayer)

IF(BUILD_PYTHON)
subdirs(PyHyperNEAT)
ENDIF(BUILD_PYTHON)

