#  LibBoard CMake file
#  Contribution of Jason Gauci, University of Central Florida, http://eplex.cs.ucf.edu/
#  Modified by Guillaume Nee, GREYC, http://www.greyc.fr

Project(LibBoard)

ADD_DEFINITIONS(-D_USE_MATH_DEFINES)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(MSVC)
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc /RTCsu /W3")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /EHsc /RTCsu /W3")

  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /EHsc")

  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /EHsc")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /EHsc")

  SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /EHsc")
  SET(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /EHsc")
ELSE(MSVC)
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -Wno-unused-variable")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -Wall -Wno-unused-variable")

  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} -pg")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_DEBUG} -pg")

  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -g")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -g")
ENDIF(MSVC)

IF(NOT MYPROJ_RAN_ONCE)
  SET(MYPROJ_RAN_ONCE 1 CACHE INTERNAL "")

  SET(
    CMAKE_INSTALL_PREFIX
    "${CMAKE_SOURCE_DIR}/out/"
    CACHE
    FILEPATH
    "Install path prefix, prepended onto install directories."
    FORCE
    )

  SET(
    EXECUTABLE_OUTPUT_PATH
    "${CMAKE_SOURCE_DIR}/out/"
    CACHE
    FILEPATH
    "Single output directory for building all executables."
    FORCE
    )

  SET(
    LIBRARY_OUTPUT_PATH
    "${CMAKE_SOURCE_DIR}/out/"
    CACHE
    FILEPATH
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

INCLUDE_DIRECTORIES( include/ )

LINK_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/out/ )

SET(lib_src 
  src/Board.cpp
  src/Color.cpp
  src/Rect.cpp
  src/Path.cpp
  src/Shapes.cpp
  src/ShapeList.cpp
  src/Transforms.cpp
  src/Tools.cpp
  src/PSFonts.cpp
  )

ADD_LIBRARY(board STATIC ${lib_src})
SET_TARGET_PROPERTIES(board PROPERTIES DEBUG_POSTFIX _d)

#ADD_LIBRARY(board-dynamic SHARED ${lib_src})
#SET_TARGET_PROPERTIES(board-dynamic PROPERTIES OUTPUT_NAME "board")
#SET_TARGET_PROPERTIES(board-dynamic PROPERTIES PREFIX "lib")

FOREACH( EXAMPLE logo example1 example2 example3 example4
    arithmetic ellipse graph arrows  ruler koch clipping
    flag scale_ellipse )
  ADD_EXECUTABLE(
    ${EXAMPLE}
    examples/${EXAMPLE}.cpp
    )
  TARGET_LINK_LIBRARIES(
    ${EXAMPLE}
    debug board_d
    optimized board
    )
  SET_TARGET_PROPERTIES(${EXAMPLE} PROPERTIES DEBUG_POSTFIX _d)
ENDFOREACH(EXAMPLE)

