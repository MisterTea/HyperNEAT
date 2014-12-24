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
      -DBOOST_FILESYSTEM_DEPRECATED
			-DNOMINMAX
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
      -DBOOST_FILESYSTEM_DEPRECATED
			-DNOMINMAX
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
      -DBOOST_FILESYSTEM_DEPRECATED
			-DNOMINMAX
			)
		ENDIF(APPLE)
	endif(WIN32)
endif(CYGWIN)

IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	ADD_DEFINITIONS(-fPIC)
ENDIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

SET(
	USE_GUI 
	ON 
	CACHE 
	BOOL
	"Should we add support for WxWidgets?"
	)

SET(
	BUILD_PYTHON
	ON 
	CACHE 
	BOOL
	"Build the python extension?"
	)

SET(
	BUILD_MPI
	ON 
	CACHE 
	BOOL
	"Build the MPI extension?"
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
	../Hypercube_NEAT/include
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

ADD_EXECUTABLE(
	HyperNEATGoPlayer

	include/FuegoMainEngine.h
	include/FuegoMainUtil.h

	src/FuegoMain.cpp
	src/FuegoMainEngine.cpp
	src/FuegoMainUtil.cpp
	)

ADD_DEPENDENCIES(HyperNEATGoPlayer Hypercube_NEAT_Base NEATLib CakeFixedDepthLib ClicheLib)

SET_TARGET_PROPERTIES(HyperNEATGoPlayer PROPERTIES DEBUG_POSTFIX _d)

SET(
	BOOST_LIB_EXT_NAME_DEBUG
	"mt-gd"
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
	HyperNEATGoPlayer 

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
			HyperNEATGoPlayer

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
				HyperNEATGoPlayer 

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
				HyperNEATGoPlayer 

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
				HyperNEATGoPlayer 

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
		HyperNEATGoPlayer
	
		general
		"-framework OpenCL"
	)
	ELSE(APPLE)
	TARGET_LINK_LIBRARIES(
		HyperNEATGoPlayer
	
		general
		OpenCL
	)
	ENDIF(APPLE)
ENDIF(BUILD_GPU)

IF(NOT WIN32)
TARGET_LINK_LIBRARIES(
HyperNEATGoPlayer

general
pthread
)
ENDIF(NOT WIN32)

