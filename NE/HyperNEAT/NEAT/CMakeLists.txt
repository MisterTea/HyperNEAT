Project(NEAT)

ADD_DEFINITIONS(
    -DBUILD_NEAT_DLL 
    -DTIXML_USE_STL
    -DNOMINMAX
    -DBOOST_ALL_NO_LIB
    -DBOOST_FILESYSTEM_DEPRECATED
)

INCLUDE_REGULAR_EXPRESSION( "^.*[Nn][Ee][Aa][Tt][_].*$" )

#GET_CMAKE_PROPERTY(CMAKETESTPROP CACHE_VARIABLES)

SET(
TINYXMLDLL_INCLUDE
"${CMAKE_CURRENT_SOURCE_DIR}/../../../tinyxmldll/include"
CACHE
PATH 
"TinyXMLDLL include directory"
) 

SET(
BOOST_ROOT
"${CMAKE_CURRENT_SOURCE_DIR}/../../../boost_1_57_0"
CACHE
PATH 
"Boost Base Directory"
)

SET(
JGTL_INCLUDE
"${CMAKE_CURRENT_SOURCE_DIR}/../../../JGTL/include"
CACHE
PATH 
"JGTL include directory"
) 

SET(
ZLIB_INCLUDE
"${CMAKE_CURRENT_SOURCE_DIR}/../../../zlib"
CACHE
PATH 
"Zlib include directory"
) 

SET(
	BOARD_INCLUDE
	"${CMAKE_CURRENT_SOURCE_DIR}/../../../Board/include/"
	CACHE
	PATH 
	"libboard include directory"
	) 

INCLUDE_DIRECTORIES(
include
${TINYXMLDLL_INCLUDE}
${BOOST_ROOT}
${JGTL_INCLUDE}
${ZLIB_INCLUDE}
${BOARD_INCLUDE}
)

IF(BUILD_GPU)
        SET(
        OPENCL_INCLUDE
        "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK/OpenCL/common/inc"
        CACHE
        PATH 
        "OpenCL include directory"
        )
        INCLUDE_DIRECTORIES(
        ${OPENCL_INCLUDE}
        )
ENDIF(BUILD_GPU)

SET(
	BOOST_STAGE
	"${CMAKE_CURRENT_SOURCE_DIR}/../../../boost_1_57_0/stage/lib"
	CACHE
	PATH 
	"Boost Stage Directory"
)

SET(
TINYXMLDLL_LIB
"${CMAKE_CURRENT_SOURCE_DIR}/../../../tinyxmldll/out/"
CACHE
PATH 
"TinyXMLDLL Library Directory"
) 

SET(
ZLIB_LIB
"${CMAKE_CURRENT_SOURCE_DIR}/../../../zlib/out/"
CACHE
PATH 
"Zlib Library Directory"
) 

SET(
	BOARD_LIB
	"${CMAKE_CURRENT_SOURCE_DIR}/../../../Board/out/"
	CACHE
	PATH 
	"libboard library directory"
	) 

SET(
OPENCL_LIB
"C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK/OpenCL/common/lib/x64"
CACHE
PATH 
"OpenCL Library Directory"
) 

LINK_DIRECTORIES(
${TINYXMLDLL_LIB}
${ZLIB_LIB}
${BOOST_STAGE}
${BOARD_LIB}
${OPENCL_LIB}
)

IF(BUILD_GPU)
	SET(
		GPU_FILES

		include/NEAT_GPUANN.h
		src/NEAT_GPUANN.cpp

		../out/OpenCL/VectorAdd.cl
		../out/OpenCL/IteratedDotProduct.cl
		../out/OpenCL/ANN.cl
	)
ELSE(BUILD_GPU)
	SET(
		GPU_FILES
	
	)
ENDIF(BUILD_GPU)


ADD_LIBRARY(
NEATLib
STATIC 

${GPU_FILES}

src/NEAT_Defines.cpp
#src/NEAT_CoEvoExperiment.cpp
src/NEAT_FastNetwork.cpp
src/NEAT_FastLayeredNetwork.cpp
src/NEAT_FastBiasNetwork.cpp
src/NEAT_FractalNetwork.cpp
src/NEAT_GeneticGene.cpp
src/NEAT_GeneticGeneration.cpp
src/NEAT_CoEvoGeneticGeneration.cpp
src/NEAT_GeneticIndividual.cpp
src/NEAT_GeneticLinkGene.cpp
src/NEAT_GeneticNodeGene.cpp
src/NEAT_GeneticPopulation.cpp
src/NEAT_GeneticSpecies.cpp
src/NEAT_Globals.cpp
src/NEAT_ModularNetwork.cpp
src/NEAT_VectorNetwork.cpp
src/NEAT_Network.cpp
src/NEAT_NetworkLink.cpp
src/NEAT_NetworkNode.cpp
src/NEAT_Random.cpp
src/NEAT_LayeredSubstrate.cpp

include/NEAT_CoEvoExperiment.h
include/NEAT_FastNetwork.h
include/NEAT_FastLayeredNetwork.h
include/NEAT_FastBiasNetwork.h
include/NEAT_FractalNetwork.h
include/NEAT_GeneticGene.h
include/NEAT_GeneticGeneration.h
include/NEAT_CoEvoGeneticGeneration.h
include/NEAT_GeneticIndividual.h
include/NEAT_GeneticLinkGene.h
include/NEAT_GeneticNodeGene.h
include/NEAT_GeneticPopulation.h
include/NEAT_GeneticSpecies.h
include/NEAT_Globals.h
include/NEAT.h
include/NEAT_Defines.h
include/NEAT_ModularNetwork.h
include/NEAT_Network.h
include/NEAT_VectorNetwork.h
include/NEAT_NetworkLink.h
include/NEAT_NetworkNode.h
include/NEAT_Random.h
include/NEAT_STL.h
include/NEAT_LayeredSubstrate.h
)

use_precompiled_header(
	NEATLib 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/NEAT_Defines.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/NEAT_Defines.cpp"
	)

SET_TARGET_PROPERTIES(NEATLib PROPERTIES DEBUG_POSTFIX _d)

