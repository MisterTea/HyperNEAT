Project(NEAT)

if(CYGWIN)
	ADD_DEFINITIONS(
		-DTIXML_USE_STL
		-DWIN32
		-D_CONSOLE
		-D__WXMSW__
		-DWXUSINGDLL
		-DNOPCH
		-D_USE_MATH_DEFINES
		-DBOOST_ALL_NO_LIB
		-DBOOST_PYTHON_STATIC_LIB
		-DNOMINMAX
    -DBOOST_FILESYSTEM_DEPRECATED
		)
ELSE(CYGWIN)
	if(WIN32)
		ADD_DEFINITIONS(
			-DTIXML_USE_STL
			-DWIN32
			-D_CONSOLE
			-D__WXMSW__
			-DWXUSINGDLL
			-DNOPCH
			-D_USE_MATH_DEFINES
			-DBOOST_ALL_NO_LIB
			-DBOOST_PYTHON_STATIC_LIB
			-DNOMINMAX
      -DBOOST_FILESYSTEM_DEPRECATED
			)
	else(WIN32)
		IF(APPLE)
		ADD_DEFINITIONS(
			-DTIXML_USE_STL
			-D__WXMAC__
			-DWXUSINGDLL
			-DNOPCH
			-D_USE_MATH_DEFINES
			-pthread
			-DBOOST_ALL_NO_LIB
			-DBOOST_PYTHON_STATIC_LIB
			-DNOMINMAX
      -DBOOST_FILESYSTEM_DEPRECATED
			)
		ELSE(APPLE)
		ADD_DEFINITIONS(
			-DTIXML_USE_STL
			-D__WXGTK__
			-DWXUSINGDLL
			-DNOPCH
			-D_USE_MATH_DEFINES
			-pthread
			-DBOOST_ALL_NO_LIB
			-DBOOST_PYTHON_STATIC_LIB
			-DNOMINMAX
      -DBOOST_FILESYSTEM_DEPRECATED
			)
		ENDIF(APPLE)
	endif(WIN32)
endif(CYGWIN)

IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	ADD_DEFINITIONS(-fPIC)
ENDIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

SET(
	USE_GUI 
	OFF
	CACHE 
	BOOL
	"Should we add support for WxWidgets?"
	)

IF(USE_GUI)
	REMOVE_DEFINITIONS(-DHCUBE_NOGUI)
ELSE(USE_GUI)
	ADD_DEFINITIONS(-DHCUBE_NOGUI)
ENDIF(USE_GUI)

SET(CMAKE_CXX_FLAGS "${CMAKE_WX_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D__WXDEBUG__ -DWXDEBUG=1")

INCLUDE_REGULAR_EXPRESSION( "^.*([Hh][Cc][Uu][Bb][Ee]|[Nn][Ee][Aa][Tt]|main).*$" )

SET(
	WXWIDGETS_INCLUDE
	"../../../../Libraries/wxWidgets-2.8.0/include"
	CACHE
	PATH 
	"WxWidgets include directory"
	)

SET(
	JGTL_INCLUDE
	"../../../JGTL/include"
	CACHE
	PATH 
	"JGTL include directory"
	) 

SET(
	MPI_INCLUDE_PATH
	"/opt/lam/gnu/include"
	CACHE
	PATH 
	"MPI include directory"
	)

SET(
	ZLIB_INCLUDE
	"../../../zlib"
	CACHE
	PATH 
	"Zlib include directory"
	)

SET(
	PYTHON_INCLUDE
	"C:/Python25/include"
	CACHE
	PATH 
	"Python include directory"
	)

SET(
	FUEGO_INCLUDE
	"../../../fuego-0.4"
	CACHE
	PATH 
	"Fuego include directory"
	)

IF(CYGWIN)
	SET(
		WXWIDGETS_BUILD_INCLUDE_DEBUG
		"../../../../Libraries/wxWidgets-2.8.0/build/cygwin_release/lib/wx/include/msw-unicode-debug-2.8"
		CACHE
		PATH 
		"WxWidgets setup.h include directory"
		)
	SET(
		WXWIDGETS_BUILD_INCLUDE_RELEASE
		"../../../../Libraries/wxWidgets-2.8.0/build/cygwin_release/lib/wx/include/msw-unicode-release-2.8"
		CACHE
		PATH 
		"WxWidgets setup.h include directory"
		)
ELSE(CYGWIN)
	IF(WIN32)
		SET(
			WXWIDGETS_BUILD_INCLUDE_DEBUG
			"../../../../Libraries/wxWidgets-2.8.0/lib/vc_dll/mswud"
			CACHE
			PATH 
			"WxWidgets setup.h include directory"
			)
		SET(
			WXWIDGETS_BUILD_INCLUDE_RELEASE
			"../../../../Libraries/wxWidgets-2.8.0/lib/vc_dll/mswu"
			CACHE
			PATH 
			"WxWidgets setup.h include directory"
			)
	ELSE(WIN32)
		SET(
			WXWIDGETS_BUILD_INCLUDE_DEBUG
			"../../../../Libraries/wxWidgets-2.8.0/build/linux_release/lib/wx/include/gtk2-unicode-debug-2.8"
			CACHE
			PATH 
			"WxWidgets setup.h include directory"
			)
		SET(
			WXWIDGETS_BUILD_INCLUDE_RELEASE
			"../../../../Libraries/wxWidgets-2.8.0/build/linux_release/lib/wx/include/gtk2-unicode-release-2.8"
			CACHE
			PATH 
			"WxWidgets setup.h include directory"
			)
	ENDIF(WIN32)
ENDIF(CYGWIN)

INCLUDE_DIRECTORIES(
	include
	../NEAT/include
	../cake_fixeddepth
	../cliche-1.2
	${TINYXMLDLL_INCLUDE}
	${BOOST_ROOT}
	${WXWIDGETS_INCLUDE}
	${WXWIDGETS_BUILD_INCLUDE_DEBUG}
	${WXWIDGETS_BUILD_INCLUDE_RELEASE}
	${JGTL_INCLUDE}
	${MPI_INCLUDE_PATH}
	${MPI_INCLUDE}
	${ZLIB_INCLUDE}
	${PYTHON_INCLUDE}
	${FUEGO_INCLUDE}
	${FUEGO_INCLUDE}/go
	${FUEGO_INCLUDE}/gouct
	${FUEGO_INCLUDE}/gtpengine
	${FUEGO_INCLUDE}/simpleplayers
	${FUEGO_INCLUDE}/smartgame
	)
IF(WIN32)
INCLUDE_DIRECTORIES(
../../../C99_support/msvc
)
ENDIF(WIN32)

IF(CYGWIN)

	SET(
		WXWIDGETS_DEBUG_LIB
		"${CMAKE_CURRENT_SOURCE_DIR}/../../../../Libraries/wxWidgets-2.8.0/build/cygwin_debug/lib/"
		CACHE
		PATH 
		"WxWidgets Debug Library Directory"
		)

	SET(
		WXWIDGETS_RELEASE_LIB
		"${CMAKE_CURRENT_SOURCE_DIR}/../../../../Libraries/wxWidgets-2.8.0/build/cygwin_release/lib/"
		CACHE
		PATH 
		"WxWidgets Release Library Directory"
		)

ELSE(CYGWIN)
	IF(WIN32)

		SET(
			WXWIDGETS_DEBUG_LIB
			"${CMAKE_CURRENT_SOURCE_DIR}/../../../../Libraries/wxWidgets-2.8.0/lib/vc_dll"
			CACHE
			PATH 
			"WxWidgets Debug Library Directory"
			)

		SET(
			WXWIDGETS_RELEASE_LIB
			"${CMAKE_CURRENT_SOURCE_DIR}/../../../../Libraries/wxWidgets-2.8.0/lib/vc_dll"
			CACHE
			PATH 
			"WxWidgets Release Library Directory"
			)

	ELSE(WIN32)

		SET(
			WXWIDGETS_DEBUG_LIB
			"${CMAKE_CURRENT_SOURCE_DIR}/../../../../Libraries/wxWidgets-2.8.0/build/linux_debug/lib/"
			CACHE
			PATH 
			"WxWidgets Debug Library Directory"
			)

		SET(
			WXWIDGETS_RELEASE_LIB
			"${CMAKE_CURRENT_SOURCE_DIR}/../../../../Libraries/wxWidgets-2.8.0/build/linux_release/lib/"
			CACHE
			PATH 
			"WxWidgets Release Library Directory"
			)

	ENDIF(WIN32)
ENDIF(CYGWIN)

SET(
	MPI_LIB
	"/opt/lam/gnu/lib"
	CACHE
	PATH
	"MPI Library Directory"
	)

SET(
	PYTHON_LIB
	"C:/Python25/libs"
	CACHE
	PATH 
	"Python library directory"
	)

SET(
    FUEGO_LIB
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../fuego-0.4/out/"
    CACHE
    PATH 
    "Fuego Library Directory"
    ) 

LINK_DIRECTORIES(
	${TINYXMLDLL_LIB}
	${ZLIB_LIB}
	${BOOST_STAGE}
	${WXWIDGETS_DEBUG_LIB}
	${WXWIDGETS_RELEASE_LIB}
	${MPI_LIB}
	${PYTHON_LIB}
	${LIBRARY_OUTPUT_PATH}
	${FUEGO_LIB}
	${BOARD_LIB}
	${OPENCL_LIB}
	)

SOURCE_GROUP("Experiment Header Files" REGULAR_EXPRESSION include/Experiments/)
SOURCE_GROUP("Experiment Source Files" REGULAR_EXPRESSION src/Experiments/)

ADD_LIBRARY(
	Hypercube_NEAT_Base
	STATIC

	src/HCUBE_Defines.cpp
	src/HCUBE_EvaluationPanel.cpp
	src/HCUBE_ExperimentPanel.cpp
	src/HCUBE_ExperimentRun.cpp
	src/HCUBE_EvaluationSet.cpp
	src/HCUBE_MainApp.cpp
	src/HCUBE_MainFrame.cpp
	src/HCUBE_NetworkPanel.cpp
	src/HCUBE_SubstratePanel.cpp
	src/HCUBE_GUITemplate.cpp
	src/HCUBE_ViewIndividualFrame.cpp
	src/HCUBE_UserEvaluationFrame.cpp
	src/HCUBE_NeuralNetworkPanel.cpp
	src/HCUBE_NeuralNetworkWeightGrid.cpp

	src/Experiments/HCUBE_XorExperiment.cpp
	src/Experiments/HCUBE_XorCoExperiment.cpp
	src/Experiments/HCUBE_CheckersCommon.cpp
	src/Experiments/HCUBE_GoExperiment.cpp
	src/Experiments/HCUBE_CheckersExperiment.cpp
	src/Experiments/HCUBE_CheckersExperimentPruning.cpp
	src/Experiments/HCUBE_CheckersExperimentFogel.cpp
	src/Experiments/HCUBE_CheckersExperimentOriginalFogel.cpp
	src/Experiments/HCUBE_CheckersScalingExperiment.cpp
	src/Experiments/HCUBE_ChessExperiment.cpp
	src/Experiments/HCUBE_OthelloCommon.cpp
	src/Experiments/HCUBE_OthelloExperiment.cpp
	src/Experiments/HCUBE_OthelloCoExperiment.cpp
	src/Experiments/HCUBE_CoCheckersExperiment.cpp
	src/Experiments/HCUBE_CheckersExperimentNoGeom.cpp
	src/Experiments/HCUBE_CheckersExperimentSubstrateGeom.cpp
	src/Experiments/HCUBE_FindClusterExperiment.cpp
	src/Experiments/HCUBE_FindClusterBPExperiment.cpp
	src/Experiments/HCUBE_FindClusterNoGeomExperiment.cpp
	src/Experiments/HCUBE_FindPointExperiment.cpp
	src/Experiments/HCUBE_RobotArmExperiment.cpp
	src/Experiments/HCUBE_RobotArmLatticeExperiment.cpp
	src/Experiments/HCUBE_SimpleImageExperiment.cpp
	src/Experiments/HCUBE_SpatialExperiment.cpp
	src/Experiments/HCUBE_SpatialCasinoExperiment.cpp
	src/Experiments/HCUBE_TicTacToeExperiment.cpp
	src/Experiments/HCUBE_TicTacToeGameExperiment.cpp
	src/Experiments/HCUBE_TicTacToeGameNoGeomExperiment.cpp
	src/Experiments/HCUBE_TicTacToeGameTreeSearch.cpp
	src/Experiments/HCUBE_ImageCompressionExperiment.cpp
	src/Experiments/HCUBE_BinaryCompressionExperiment.cpp
	src/Experiments/HCUBE_LogicBuilderExperiment.cpp

	include/HCUBE_Boost.h
	include/HCUBE_Defines.h
	include/HCUBE_EvaluationPanel.h
	include/HCUBE_EvaluationSet.h
	include/HCUBE_ExperimentPanel.h
	include/HCUBE_ExperimentRun.h
	include/HCUBE_MainApp.h
	include/HCUBE_MainFrame.h
	include/HCUBE_NetworkPanel.h
	include/HCUBE_STL.h
	include/HCUBE_SubstratePanel.h
	include/HCUBE_GUITemplate.h
	include/HCUBE_UserEvaluationFrame.h
	include/HCUBE_NeuralNetworkPanel.h
	include/HCUBE_NeuralNetworkWeightGrid.h
	include/HCUBE_ViewIndividualFrame.h

	include/Experiments/HCUBE_XorExperiment.h
	include/Experiments/HCUBE_XorCoExperiment.h
	include/Experiments/HCUBE_Experiment.h
	include/Experiments/HCUBE_OthelloExperiment.h
	include/Experiments/HCUBE_OthelloCoExperiment.h
	include/Experiments/HCUBE_OthelloCommon.h
	include/Experiments/HCUBE_GoExperiment.h
	include/Experiments/HCUBE_CheckersExperiment.h
	include/Experiments/HCUBE_CheckersExperimentPruning.h
	include/Experiments/HCUBE_CheckersExperimentFogel.h
	include/Experiments/HCUBE_CheckersExperimentOriginalFogel.h
	include/Experiments/HCUBE_CheckersScalingExperiment.h
	include/Experiments/HCUBE_CheckersCommon.h
	include/Experiments/HCUBE_CoCheckersExperiment.h
	include/Experiments/HCUBE_CheckersExperimentNoGeom.h
	include/Experiments/HCUBE_CheckersExperimentSubstrateGeom.h
	include/Experiments/HCUBE_ChessExperiment.h
	include/Experiments/HCUBE_FindClusterExperiment.h
	include/Experiments/HCUBE_FindClusterBPExperiment.h
	include/Experiments/HCUBE_FindClusterNoGeomExperiment.h
	include/Experiments/HCUBE_FindPointExperiment.h
	include/Experiments/HCUBE_RobotArmExperiment.h
	include/Experiments/HCUBE_RobotArmLatticeExperiment.h
	include/Experiments/HCUBE_SimpleImageExperiment.h
	include/Experiments/HCUBE_SpatialExperiment.h
	include/Experiments/HCUBE_SpatialCasinoExperiment.h
	include/Experiments/HCUBE_TicTacToeExperiment.h
	include/Experiments/HCUBE_TicTacToeGameExperiment.h
	include/Experiments/HCUBE_TicTacToeGameNoGeomExperiment.h
	include/Experiments/HCUBE_TicTacToeGameTreeSearch.h
	include/Experiments/HCUBE_TicTacToeShared.h
	include/Experiments/HCUBE_ImageCompressionExperiment.h
	include/Experiments/HCUBE_BinaryCompressionExperiment.h
	include/Experiments/HCUBE_LogicBuilderExperiment.h
	)

use_precompiled_header(
	Hypercube_NEAT_Base 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/HCUBE_Defines.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/HCUBE_Defines.cpp"
	)

ADD_DEPENDENCIES(Hypercube_NEAT_Base NEATLib CakeFixedDepthLib ClicheLib)

SET_TARGET_PROPERTIES(Hypercube_NEAT_Base PROPERTIES DEBUG_POSTFIX _d)

#ADD_EXECUTABLE(
# Hypercube_NEAT
# MACOSX_BUNDLE
#
# src/main.cpp
# )

ADD_EXECUTABLE( Hypercube_NEAT src/main.cpp )

SET_TARGET_PROPERTIES(Hypercube_NEAT PROPERTIES DEBUG_POSTFIX _d)

SET(
	BOOST_LIB_EXT_NAME_DEBUG
	"mt-d"
	CACHE
	STRING 
	"Boost debug extension"
	)
SET(
	BOOST_LIB_EXT_NAME_RELEASE
	"mt"
	CACHE
	STRING 
	"Boost Release extension"
	)

IF(WIN32)
SET(
	BOOST_LIB_PREFIX_NAME
	"lib"
	CACHE
	STRING 
	"Boost prefix"
	)
ELSE(WIN32)
SET(
	BOOST_LIB_PREFIX_NAME
	""
	CACHE
	STRING 
	"Boost prefix"
	)
ENDIF(WIN32)

TARGET_LINK_LIBRARIES(
	Hypercube_NEAT 

	debug
	Hypercube_NEAT_Base_d

	optimized
	Hypercube_NEAT_Base
		
	debug
	ClicheLib_d
		
	optimized
	ClicheLib

	debug
	CakeFixedDepthLib_d
	
	optimized
	CakeFixedDepthLib

	debug
	NEATLib_d

	optimized
	NEATLib
	
	debug
	tinyxmlpluslib_d

	optimized
	tinyxmlpluslib

	debug
	zlib_d
	
	optimized
	zlib

	debug
	board_d

	optimized
	board

	debug
	FuegoLib_d
	
	optimized
	FuegoLib

	debug
	${BOOST_LIB_PREFIX_NAME}boost_thread-${BOOST_LIB_EXT_NAME_DEBUG}

	optimized
	${BOOST_LIB_PREFIX_NAME}boost_thread-${BOOST_LIB_EXT_NAME_RELEASE}

	debug
	${BOOST_LIB_PREFIX_NAME}boost_filesystem-${BOOST_LIB_EXT_NAME_DEBUG}

	optimized
	${BOOST_LIB_PREFIX_NAME}boost_filesystem-${BOOST_LIB_EXT_NAME_RELEASE}

	debug
	${BOOST_LIB_PREFIX_NAME}boost_system-${BOOST_LIB_EXT_NAME_DEBUG}

	optimized
	${BOOST_LIB_PREFIX_NAME}boost_system-${BOOST_LIB_EXT_NAME_RELEASE}
)

IF(USE_GUI)
	IF(CYGWIN)
		TARGET_LINK_LIBRARIES(
			Hypercube_NEAT

			debug
			wx_based-2.8

			optimized
			wx_base-2.8

			debug
			wx_mswd_core-2.8

			optimized
			wx_msw_core-2.8

			debug
			wx_mswd_adv-2.8

			optimized
			wx_msw_adv-2.8
			)
	ELSE(CYGWIN)
		IF(WIN32)
			TARGET_LINK_LIBRARIES(
				Hypercube_NEAT 

				debug
				wxbase28d

				optimized
				wxbase28

				debug
				wxmsw28d_core

				optimized
				wxmsw28_core

				debug
				wxmsw28d_adv

				optimized
				wxmsw28_adv
				)
		ELSE(WIN32)
			IF(APPLE)
			TARGET_LINK_LIBRARIES(
				Hypercube_NEAT 

				general
				"-framework Carbon"
		
				debug
				wx_macud_adv-2.8

				optimized
				wx_macu_adv-2.8

				debug
				wx_macud_aui-2.8

				optimized
				wx_macu_aui-2.8

				debug
				wx_macud_qa-2.8

				optimized
				wx_macu_qa-2.8

				debug
				wx_macud_core-2.8

				optimized
				wx_macu_core-2.8

				debug
				wx_base_carbonud-2.8

				optimized
				wx_base_carbonu-2.8
				)
			ELSE(APPLE)
			TARGET_LINK_LIBRARIES(
				Hypercube_NEAT 

				debug
				wx_baseud-2.8

				optimized
				wx_baseu-2.8

				debug
				wx_gtk2ud_core-2.8

				optimized
				wx_gtk2u_core-2.8

				debug
				wx_gtk2ud_adv-2.8

				optimized
				wx_gtk2u_adv-2.8
				)
			ENDIF(APPLE)
		ENDIF(WIN32)
	ENDIF(CYGWIN)
ENDIF(USE_GUI)

IF(BUILD_GPU)
	IF(APPLE)
	TARGET_LINK_LIBRARIES(
		Hypercube_NEAT
	
		general
		"-framework OpenCL"
	)
	ELSE(APPLE)
	TARGET_LINK_LIBRARIES(
		Hypercube_NEAT
	
		general
		OpenCL
	)
	ENDIF(APPLE)
ENDIF(BUILD_GPU)

IF(NOT WIN32)
TARGET_LINK_LIBRARIES(
Hypercube_NEAT

general
pthread
)
ENDIF(NOT WIN32)

ADD_DEPENDENCIES(Hypercube_NEAT NEATLib Hypercube_NEAT_Base)

IF(BUILD_MPI)

	ADD_EXECUTABLE(
		Hypercube_NEAT_MPI

		src/HCUBE_MPIExperimentRun.cpp
		src/HCUBE_MPIEvaluationSet.cpp
		src/mpimain.cpp

		include/HCUBE_MPIExperimentRun.h
		include/HCUBE_MPIEvaluationSet.h

		)

	SET_TARGET_PROPERTIES(
		Hypercube_NEAT_MPI 
		PROPERTIES DEBUG_POSTFIX _d
		#COMPILE_FLAGS "-DHCUBE_NOGUI -DHCUBE_USEMPI -DUSE_STDARG -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDARG_H=1 -DUSE_STDARG=1 -DMALLOC_RET_VOID=1"
		COMPILE_FLAGS "-DHCUBE_USEMPI"
		)

		TARGET_LINK_LIBRARIES(
			Hypercube_NEAT_MPI

			debug
			Hypercube_NEAT_Base_d

			optimized
			Hypercube_NEAT_Base

			debug
			CakeFixedDepthLib_d
	
			optimized
			CakeFixedDepthLib
		
    		debug
		   ClicheLib_d
		
		   optimized
		   ClicheLib

			debug
			NEATLib_d

			optimized
			NEATLib

			debug
			tinyxmlpluslib_d

			optimized
			tinyxmlpluslib
			
			debug
			zlib_d
			
			optimized
			zlib

	debug
	board_d

	optimized
	board

	        debug
	        FuegoLib_d
        	
	        optimized
	        FuegoLib

			debug
			${BOOST_LIB_PREFIX_NAME}boost_thread-${BOOST_LIB_EXT_NAME_DEBUG}

			optimized
			${BOOST_LIB_PREFIX_NAME}boost_thread-${BOOST_LIB_EXT_NAME_RELEASE}

			debug
			${BOOST_LIB_PREFIX_NAME}boost_filesystem-${BOOST_LIB_EXT_NAME_DEBUG}

			optimized
			${BOOST_LIB_PREFIX_NAME}boost_filesystem-${BOOST_LIB_EXT_NAME_RELEASE}

			debug
			${BOOST_LIB_PREFIX_NAME}boost_system-${BOOST_LIB_EXT_NAME_DEBUG}

			optimized
			${BOOST_LIB_PREFIX_NAME}boost_system-${BOOST_LIB_EXT_NAME_RELEASE}
	    )

	IF(WIN32)
		TARGET_LINK_LIBRARIES(
			Hypercube_NEAT_MPI

			debug
			libmpid

			optimized
			libmpi
			
			debug
			libmpi_cxxd

			optimized
			libmpi_cxx

			debug
			libopen-pald

			optimized
			libopen-pal

			debug
			libopen-rted

			optimized
			libopen-rte
			
			general
			ws2_32
			)
	ELSE(WIN32)
		TARGET_LINK_LIBRARIES(
			Hypercube_NEAT_MPI

			general
			mpi_cxx

			general
			mpi
			)
	ENDIF(WIN32)

	IF(USE_GUI)
		IF(CYGWIN)
			TARGET_LINK_LIBRARIES(
				Hypercube_NEAT_MPI

				debug
				wx_based-2.8

				optimized
				wx_base-2.8

				debug
				wx_mswd_core-2.8

				optimized
				wx_msw_core-2.8

				debug
				wx_mswd_adv-2.8

				optimized
				wx_msw_adv-2.8
				)
		ELSE(CYGWIN)
			IF(WIN32)
				TARGET_LINK_LIBRARIES(
					Hypercube_NEAT_MPI

					debug
					wxbase28d

					optimized
					wxbase28

					debug
					wxmsw28d_core

					optimized
					wxmsw28_core

					debug
					wxmsw28d_adv

					optimized
					wxmsw28_adv
					)
			ELSE(WIN32)
			IF(APPLE)
			TARGET_LINK_LIBRARIES(
				Hypercube_NEAT_MPI

				general
				"-framework Carbon"
		
				debug
				wx_macud_adv-2.8

				optimized
				wx_macu_adv-2.8

				debug
				wx_macud_aui-2.8

				optimized
				wx_macu_aui-2.8

				debug
				wx_macud_qa-2.8

				optimized
				wx_macu_qa-2.8

				debug
				wx_macud_core-2.8

				optimized
				wx_macu_core-2.8

				debug
				wx_base_carbonud-2.8

				optimized
				wx_base_carbonu-2.8
				)
			ELSE(APPLE)
			TARGET_LINK_LIBRARIES(
				Hypercube_NEAT_MPI

				debug
				wx_baseud-2.8

				optimized
				wx_baseu-2.8

				debug
				wx_gtk2ud_core-2.8

				optimized
				wx_gtk2u_core-2.8

				debug
				wx_gtk2ud_adv-2.8

				optimized
				wx_gtk2u_adv-2.8
				)
			ENDIF(APPLE)
			ENDIF(WIN32)
		ENDIF(CYGWIN)
	ENDIF(USE_GUI)

	IF(BUILD_GPU)
		IF(APPLE)
		TARGET_LINK_LIBRARIES(
			Hypercube_NEAT_MPI
		
			general
			"-framework OpenCL"
		)
		ELSE(APPLE)
		TARGET_LINK_LIBRARIES(
			Hypercube_NEAT_MPI
		
			general
			OpenCL
		)
		ENDIF(APPLE)
	ENDIF(BUILD_GPU)

	IF(NOT WIN32)
		TARGET_LINK_LIBRARIES(
		Hypercube_NEAT_MPI
		
		general
		pthread
		)
	ENDIF(NOT WIN32)

	ADD_DEPENDENCIES(Hypercube_NEAT_MPI NEATLib Hypercube_NEAT_Base)

ENDIF(BUILD_MPI)
