Project(NEAT)

ADD_DEFINITIONS(
    -DBUILD_NEAT_DLL 
    -DTIXML_USE_STL
	-DNOMINMAX
	-DBOOST_ALL_NO_LIB
	-DBOOST_PYTHON_STATIC_LIB
	-D_USE_MATH_DEFINES
)

if(CYGWIN)
	ADD_DEFINITIONS(
		-DWIN32
		-D_CONSOLE
		-D__WXMSW__
		-DWXUSINGDLL
		-DNOPCH
		)
ELSE(CYGWIN)
	if(WIN32)
		ADD_DEFINITIONS(
			-DWIN32
			-D_CONSOLE
			-D__WXMSW__
			-DWXUSINGDLL
			-DNOPCH
			)
	else(WIN32)
		IF(APPLE)
		ADD_DEFINITIONS(
			-D__WXMAC__
			-DWXUSINGDLL
			-DNOPCH
			)
		ELSE(APPLE)
		ADD_DEFINITIONS(
			-D__WXGTK__
			-DWXUSINGDLL
			-DNOPCH
			)
		ENDIF(APPLE)
	endif(WIN32)
endif(CYGWIN)


INCLUDE_REGULAR_EXPRESSION( "^.*[Nn][Ee][Aa][Tt][_].*$" )

#GET_CMAKE_PROPERTY(CMAKETESTPROP CACHE_VARIABLES)

SET(
TINYXMLDLL_INCLUDE
"../../../tinyxmldll/include"
CACHE
PATH 
"TinyXMLDLL include directory"
) 

SET(
BOOST_ROOT
"../../../../Libraries/boost-trunk"
CACHE
PATH 
"Boost Base Directory"
)

SET(
JGTL_INCLUDE
"../../../JGTL/include"
CACHE
PATH 
"JGTL include directory"
) 

SET(
ZLIB_INCLUDE
"../../../zlib"
CACHE
PATH 
"Zlib include directory"
) 

INCLUDE_DIRECTORIES(
    include
    ../NEAT/include
    ../Hypercube_NEAT/include
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

SET(
	BOOST_STAGE
	"${CMAKE_CURRENT_SOURCE_DIR}/../../../../Libraries/boost-trunk/stage/lib"
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

ADD_LIBRARY(
PyHyperNEAT
SHARED 

src/PyHyperNEAT.cpp
)

SET_TARGET_PROPERTIES(PyHyperNEAT PROPERTIES DEBUG_POSTFIX _d)

SET_TARGET_PROPERTIES(PyHyperNEAT PROPERTIES DEBUG_POSTFIX _d)

IF(WIN32)
    SET_TARGET_PROPERTIES(PyHyperNEAT PROPERTIES SUFFIX .pyd)
ELSE(WIN32)
IF(APPLE)
    SET_TARGET_PROPERTIES(PyHyperNEAT PROPERTIES SUFFIX .so)
ENDIF(APPLE)
    SET_TARGET_PROPERTIES(PyHyperNEAT PROPERTIES PREFIX "")
ENDIF(WIN32)

TARGET_LINK_LIBRARIES(
PyHyperNEAT

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
    ${BOOST_LIB_PREFIX_NAME}boost_python-${BOOST_LIB_EXT_NAME_DEBUG}-py26

    optimized
    ${BOOST_LIB_PREFIX_NAME}boost_python-${BOOST_LIB_EXT_NAME_RELEASE}-py26

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
			PyHyperNEAT

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
				PyHyperNEAT 

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
				PyHyperNEAT 

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
				PyHyperNEAT 

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

IF(WIN32)
TARGET_LINK_LIBRARIES(
	PyHyperNEAT
	
	general
	python26
	)
ELSE(WIN32)
	IF(APPLE)
	TARGET_LINK_LIBRARIES(
		PyHyperNEAT
	
		general
		"-framework Python"
		)
	ELSE(APPLE)
	TARGET_LINK_LIBRARIES(
		PyHyperNEAT
	
		general
		python2.6
		)
	ENDIF(APPLE)
ENDIF(WIN32)

IF(BUILD_GPU)
	IF(APPLE)
	TARGET_LINK_LIBRARIES(
		PyHyperNEAT
	
		general
		"-framework OpenCL"
	)
	ELSE(APPLE)
	TARGET_LINK_LIBRARIES(
		PyHyperNEAT
	
		general
		OpenCL
	)
	ENDIF(APPLE)
ENDIF(BUILD_GPU)

ADD_DEPENDENCIES(PyHyperNEAT NEATLib Hypercube_NEAT_Base)

