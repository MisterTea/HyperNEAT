Project(Fuego)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(MSVC)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc /RTCsu /W3")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /EHsc /RTCsu /W3")

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc /DRELEASE_MODE /fp:fast /arch:SSE2 ")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /EHsc /DRELEASE_MODE /fp:fast /arch:SSE2 ")

SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /EHsc /fp:fast /arch:SSE2")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /EHsc /fp:fast /arch:SSE2")

SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /EHsc")
SET(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /EHsc")

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

ADD_DEFINITIONS(
    -D_USE_MATH_DEFINES
    -DNOMINMAX
    -DBOOST_ALL_NO_LIB
    -DBOOST_TEST_ALTERNATIVE_INIT_API
    -DBOOST_TEST_NO_MAIN
    -DBOOST_FILESYSTEM_DEPRECATED
)

SET(
BOOST_ROOT
"${CMAKE_CURRENT_SOURCE_DIR}/../boost_1_57_0"
CACHE
PATH 
"Boost Base Directory"
)

INCLUDE_DIRECTORIES(
fuegomain
fuegotest
go
gouct
gtpengine
simpleplayers
smartgame
unittestmain
${BOOST_ROOT}
${JGTL_INCLUDE}
)

SET(
	BOOST_STAGE
	"${CMAKE_CURRENT_SOURCE_DIR}/../boost_1_57_0/stage/lib"
	CACHE
	PATH 
	"Boost Stage Directory"
)

LINK_DIRECTORIES(
	${BOOST_STAGE}
	${LIBRARY_OUTPUT_PATH}
)

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

SOURCE_GROUP("Go" REGULAR_EXPRESSION go/)
SOURCE_GROUP("GoUCT" REGULAR_EXPRESSION gouct/)
SOURCE_GROUP("GTPEngine" REGULAR_EXPRESSION gtpengine/)
SOURCE_GROUP("SimplePlayers" REGULAR_EXPRESSION simpleplayers/)
SOURCE_GROUP("SmartGame" REGULAR_EXPRESSION smartgame/)
SOURCE_GROUP("FuegoMain" REGULAR_EXPRESSION fuegomain/)
SOURCE_GROUP("UnitTestMain" REGULAR_EXPRESSION unittestmain/)
SOURCE_GROUP("FuegoTest" REGULAR_EXPRESSION fuegotest/)

ADD_LIBRARY(
FuegoLib
STATIC

./go/GoAssertBoardRestored.h
./go/GoAutoBook.cpp
./go/GoAutoBook.h
./go/GoBensonSolver.cpp
./go/GoBensonSolver.h
./go/GoBlock.cpp
./go/GoBlock.h
./go/GoBoard.cpp
./go/GoBoard.h
./go/GoBoardCheckPerformance.cpp
./go/GoBoardCheckPerformance.h
./go/GoBoardHistory.cpp
./go/GoBoardHistory.h
./go/GoBoardRestorer.cpp
./go/GoBoardRestorer.h
./go/GoBoardSynchronizer.cpp
./go/GoBoardSynchronizer.h
./go/GoBoardUpdater.cpp
./go/GoBoardUpdater.h
./go/GoBoardUtil.cpp
./go/GoBoardUtil.h
./go/GoBook.cpp
./go/GoBook.h
./go/GoChain.cpp
./go/GoChain.h
./go/GoEyeCount.cpp
./go/GoEyeCount.h
./go/GoEyeUtil.cpp
./go/GoEyeUtil.h
./go/GoGame.cpp
./go/GoGame.h
./go/GoGtpCommandUtil.cpp
./go/GoGtpCommandUtil.h
./go/GoGtpEngine.cpp
./go/GoGtpEngine.h
./go/GoGtpExtraCommands.cpp
./go/GoGtpExtraCommands.h
./go/GoGtpTimeSettings.cpp
./go/GoGtpTimeSettings.h
./go/GoInfluence.cpp
./go/GoInfluence.h
./go/GoInit.cpp
./go/GoInit.h
./go/GoKomi.cpp
./go/GoKomi.h
./go/GoLadder.cpp
./go/GoLadder.h
./go/GoModBoard.h
./go/GoMotive.cpp
./go/GoMotive.h
./go/GoMoveExecutor.h
./go/GoNodeUtil.cpp
./go/GoNodeUtil.h
./go/GoPlayer.cpp
./go/GoPlayer.h
./go/GoPlayerMove.cpp
./go/GoPlayerMove.h
./go/GoRegion.cpp
./go/GoRegion.h
./go/GoRegionBoard.cpp
./go/GoRegionBoard.h
./go/GoRegionUtil.cpp
./go/GoRegionUtil.h
./go/GoRules.cpp
./go/GoRules.h
./go/GoSafetyCommands.cpp
./go/GoSafetyCommands.h
./go/GoSafetySolver.cpp
./go/GoSafetySolver.h
./go/GoSafetyUtil.cpp
./go/GoSafetyUtil.h
./go/GoSearch.cpp
./go/GoSearch.h
./go/GoSetup.h
./go/GoSetupUtil.cpp
./go/GoSetupUtil.h
./go/GoSortedMoves.h
./go/GoStaticLadder.cpp
./go/GoStaticLadder.h
./go/GoStaticSafetySolver.cpp
./go/GoStaticSafetySolver.h
./go/GoTimeControl.cpp
./go/GoTimeControl.h
./go/GoUtil.cpp
./go/GoUtil.h
./gouct/GoUctBoard.cpp
./gouct/GoUctBoard.h
./gouct/GoUctBookBuilder.h
./gouct/GoUctBookBuilderCommands.h
./gouct/GoUctCommands.cpp
./gouct/GoUctCommands.h
./gouct/GoUctDefaultPriorKnowledge.cpp
./gouct/GoUctDefaultPriorKnowledge.h
./gouct/GoUctDefaultRootFilter.cpp
./gouct/GoUctDefaultRootFilter.h
./gouct/GoUctEstimatorStat.cpp
./gouct/GoUctEstimatorStat.h
./gouct/GoUctGlobalSearch.cpp
./gouct/GoUctGlobalSearch.h
./gouct/GoUctObjectWithSearch.cpp
./gouct/GoUctObjectWithSearch.h
./gouct/GoUctPatterns.h
./gouct/GoUctPlayer.h
./gouct/GoUctPlayoutPolicy.cpp
./gouct/GoUctPlayoutPolicy.h
./gouct/GoUctPureRandomGenerator.h
./gouct/GoUctRootFilter.cpp
./gouct/GoUctRootFilter.h
./gouct/GoUctSearch.cpp
./gouct/GoUctSearch.h
./gouct/GoUctUtil.cpp
./gouct/GoUctUtil.h
./gouct/test/GoUctBoardTest.cpp
./gouct/test/GoUctUtilTest.cpp
./gtpengine/GtpEngine.cpp
./gtpengine/GtpEngine.h
./gtpengine/GtpInputStream.cpp
./gtpengine/GtpInputStream.h
./gtpengine/GtpOutputStream.cpp
./gtpengine/GtpOutputStream.h
./gtpengine/test/GtpEngineTest.cpp
./simpleplayers/SpAveragePlayer.cpp
./simpleplayers/SpAveragePlayer.h
./simpleplayers/SpCapturePlayer.cpp
./simpleplayers/SpCapturePlayer.h
./simpleplayers/SpDumbTacticalPlayer.cpp
./simpleplayers/SpDumbTacticalPlayer.h
./simpleplayers/SpGreedyPlayer.cpp
./simpleplayers/SpGreedyPlayer.h
./simpleplayers/SpInfluencePlayer.cpp
./simpleplayers/SpInfluencePlayer.h
./simpleplayers/SpLadderPlayer.cpp
./simpleplayers/SpLadderPlayer.h
./simpleplayers/SpLibertyPlayer.cpp
./simpleplayers/SpLibertyPlayer.h
./simpleplayers/SpMaxEyePlayer.cpp
./simpleplayers/SpMaxEyePlayer.h
./simpleplayers/SpMinLibPlayer.cpp
./simpleplayers/SpMinLibPlayer.h
./simpleplayers/SpMoveGenerator.cpp
./simpleplayers/SpMoveGenerator.h
./simpleplayers/SpRandomPlayer.cpp
./simpleplayers/SpRandomPlayer.h
./simpleplayers/SpSafePlayer.cpp
./simpleplayers/SpSafePlayer.h
./simpleplayers/SpSimplePlayer.cpp
./simpleplayers/SpSimplePlayer.h
./simpleplayers/SpUtil.cpp
./simpleplayers/SpUtil.h
./smartgame/SgArray.h
./smartgame/SgBlackWhite.h
./smartgame/SgBoardColor.h
./smartgame/SgBoardConst.cpp
./smartgame/SgBoardConst.h
./smartgame/SgBookBuilder.cpp
./smartgame/SgBookBuilder.h
./smartgame/SgBWArray.h
./smartgame/SgBWSet.h
./smartgame/SgCmdLineOpt.cpp
./smartgame/SgCmdLineOpt.h
./smartgame/SgConnCompIterator.cpp
./smartgame/SgConnCompIterator.h
./smartgame/SgDebug.cpp
./smartgame/SgDebug.h
./smartgame/SgEBWArray.h
./smartgame/SgEvaluatedMoves.cpp
./smartgame/SgEvaluatedMoves.h
./smartgame/SgException.cpp
./smartgame/SgException.h
./smartgame/SgFastLog.cpp
./smartgame/SgFastLog.h
./smartgame/SgGameReader.cpp
./smartgame/SgGameReader.h
./smartgame/SgGameWriter.cpp
./smartgame/SgGameWriter.h
./smartgame/SgGtpClient.cpp
./smartgame/SgGtpClient.h
./smartgame/SgGtpCommands.cpp
./smartgame/SgGtpCommands.h
./smartgame/SgGtpUtil.cpp
./smartgame/SgGtpUtil.h
./smartgame/SgHash.h
./smartgame/SgHashTable.h
./smartgame/SgIncrementalStack.cpp
./smartgame/SgIncrementalStack.h
./smartgame/SgInit.cpp
./smartgame/SgInit.h
./smartgame/SgMarker.h
./smartgame/SgMath.h
./smartgame/SgMemCheck.cpp
./smartgame/SgMemCheck.h
./smartgame/SgMiaiMap.cpp
./smartgame/SgMiaiMap.h
./smartgame/SgMiaiStrategy.cpp
./smartgame/SgMiaiStrategy.h
./smartgame/SgMove.h
./smartgame/SgMpiSynchronizer.cpp
./smartgame/SgMpiSynchronizer.h
./smartgame/SgNbIterator.cpp
./smartgame/SgNbIterator.h
./smartgame/SgNode.cpp
./smartgame/SgNode.h
./smartgame/SgNodeUtil.cpp
./smartgame/SgNodeUtil.h
./smartgame/SgPoint.cpp
./smartgame/SgPoint.h
./smartgame/SgPointArray.h
./smartgame/SgPointIterator.h
./smartgame/SgPointSet.cpp
./smartgame/SgPointSet.h
./smartgame/SgPointSetUtil.cpp
./smartgame/SgPointSetUtil.h
./smartgame/SgProbCut.cpp
./smartgame/SgProbCut.h
./smartgame/SgProcess.cpp
./smartgame/SgProcess.h
./smartgame/SgProp.cpp
./smartgame/SgProp.h
./smartgame/SgRandom.cpp
./smartgame/SgRandom.h
./smartgame/SgRect.cpp
./smartgame/SgRect.h
./smartgame/SgRestorer.h
./smartgame/SgSearch.cpp
./smartgame/SgSearch.h
./smartgame/SgSearchControl.cpp
./smartgame/SgSearchControl.h
./smartgame/SgSearchStatistics.cpp
./smartgame/SgSearchStatistics.h
./smartgame/SgSearchTracer.cpp
./smartgame/SgSearchTracer.h
./smartgame/SgSearchValue.cpp
./smartgame/SgSearchValue.h
./smartgame/SgSList.h
./smartgame/SgSortedArray.h
./smartgame/SgSortedMoves.h
./smartgame/SgStack.h
./smartgame/SgStatistics.h
./smartgame/SgStrategy.cpp
./smartgame/SgStrategy.h
./smartgame/SgStringUtil.cpp
./smartgame/SgStringUtil.h
./smartgame/SgSystem.cpp
./smartgame/SgSystem.h
./smartgame/SgThreadedWorker.h
./smartgame/SgTime.cpp
./smartgame/SgTime.h
./smartgame/SgTimeControl.cpp
./smartgame/SgTimeControl.h
./smartgame/SgTimer.h
./smartgame/SgTimeRecord.cpp
./smartgame/SgTimeRecord.h
./smartgame/SgUctSearch.cpp
./smartgame/SgUctSearch.h
./smartgame/SgUctTree.cpp
./smartgame/SgUctTree.h
./smartgame/SgUctTreeUtil.cpp
./smartgame/SgUctTreeUtil.h
./smartgame/SgUtil.cpp
./smartgame/SgUtil.h
./smartgame/SgVector.h
./smartgame/SgVectorUtility.cpp
./smartgame/SgVectorUtility.h
./smartgame/SgWrite.cpp
./smartgame/SgWrite.h

)

SET_TARGET_PROPERTIES(FuegoLib PROPERTIES DEBUG_POSTFIX _d)

ADD_EXECUTABLE(
FuegoMain

./fuegomain/FuegoMainEngine.h
./fuegomain/FuegoMainUtil.h

./fuegomain/FuegoMain.cpp
./fuegomain/FuegoMainEngine.cpp
./fuegomain/FuegoMainUtil.cpp
)

SET_TARGET_PROPERTIES(FuegoMain PROPERTIES DEBUG_POSTFIX _d)

TARGET_LINK_LIBRARIES(
	FuegoMain

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

	debug
	${BOOST_LIB_PREFIX_NAME}boost_program_options-${BOOST_LIB_EXT_NAME_DEBUG}

	optimized
	${BOOST_LIB_PREFIX_NAME}boost_program_options-${BOOST_LIB_EXT_NAME_RELEASE}
)

IF(NOT WIN32)
TARGET_LINK_LIBRARIES(
	FuegoMain

    general
    pthread

	general
	pthread
)
ENDIF(NOT WIN32)

ADD_DEPENDENCIES(FuegoMain FuegoLib)

ADD_EXECUTABLE(
FuegoTest

./fuegotest/FuegoTestEngine.h

./fuegotest/FuegoTestEngine.cpp
./fuegotest/FuegoTestMain.cpp
)

TARGET_LINK_LIBRARIES(
	FuegoTest

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

	debug
	${BOOST_LIB_PREFIX_NAME}boost_program_options-${BOOST_LIB_EXT_NAME_DEBUG}

	optimized
	${BOOST_LIB_PREFIX_NAME}boost_program_options-${BOOST_LIB_EXT_NAME_RELEASE}
)

IF(NOT WIN32)
TARGET_LINK_LIBRARIES(
	FuegoTest

    general
    pthread
)
ENDIF(NOT WIN32)


ADD_DEPENDENCIES(FuegoTest FuegoLib)

ADD_EXECUTABLE(
FuegoUnitTest

./go/test/GoBoardSynchronizerTest.cpp
./go/test/GoBoardTest.cpp
./go/test/GoBoardUpdaterTest.cpp
./go/test/GoBoardUtilTest.cpp
./go/test/GoBookTest.cpp
./go/test/GoEyeUtilTest.cpp
./go/test/GoGameRecordTest.cpp
./go/test/GoGameTest.cpp
./go/test/GoGtpCommandUtilTest.cpp
./go/test/GoGtpTimeSettingsTest.cpp
./go/test/GoKomiTest.cpp
./go/test/GoLadderTest.cpp
./go/test/GoRegionBoardTest.cpp
./go/test/GoRegionTest.cpp
./go/test/GoSetupUtilTest.cpp
./go/test/GoTimeControlTest.cpp
./go/test/GoUtilTest.cpp

./smartgame/test/SgArrayTest.cpp
./smartgame/test/SgBlackWhiteTest.cpp
./smartgame/test/SgBoardColorTest.cpp
./smartgame/test/SgBoardConstTest.cpp
./smartgame/test/SgBWArrayTest.cpp
./smartgame/test/SgBWSetTest.cpp
./smartgame/test/SgCmdLineOptTest.cpp
./smartgame/test/SgConnCompIteratorTest.cpp
./smartgame/test/SgEBWArrayTest.cpp
./smartgame/test/SgEvaluatedMovesTest.cpp
./smartgame/test/SgFastLogTest.cpp
./smartgame/test/SgGameReaderTest.cpp
./smartgame/test/SgGtpUtilTest.cpp
./smartgame/test/SgHashTest.cpp
./smartgame/test/SgMarkerTest.cpp
./smartgame/test/SgMathTest.cpp
./smartgame/test/SgMiaiMapTest.cpp
./smartgame/test/SgMiaiStrategyTest.cpp
./smartgame/test/SgNbIteratorTest.cpp
./smartgame/test/SgNodeTest.cpp
./smartgame/test/SgPointArrayTest.cpp
./smartgame/test/SgPointSetTest.cpp
./smartgame/test/SgPointTest.cpp
./smartgame/test/SgPropTest.cpp
./smartgame/test/SgRectTest.cpp
./smartgame/test/SgRestorerTest.cpp
./smartgame/test/SgSearchTest.cpp
./smartgame/test/SgSListTest.cpp
./smartgame/test/SgSortedArrayTest.cpp
./smartgame/test/SgStackTest.cpp
./smartgame/test/SgStatisticsTest.cpp
./smartgame/test/SgStringUtilTest.cpp
./smartgame/test/SgSystemTest.cpp
./smartgame/test/SgTimeControlTest.cpp
./smartgame/test/SgUctSearchTest.cpp
./smartgame/test/SgUctTreeTest.cpp
./smartgame/test/SgUctTreeUtilTest.cpp
./smartgame/test/SgUtilTest.cpp
./smartgame/test/SgVectorTest.cpp
./smartgame/test/SgVectorUtilityTest.cpp
./smartgame/test/SgWriteTest.cpp

./unittestmain/UnitTestMain.cpp

)

TARGET_LINK_LIBRARIES(
	FuegoUnitTest

	debug
	FuegoLib_d

	optimized
	FuegoLib

	debug
	${BOOST_LIB_PREFIX_NAME}boost_prg_exec_monitor-${BOOST_LIB_EXT_NAME_DEBUG}

	optimized
	${BOOST_LIB_PREFIX_NAME}boost_prg_exec_monitor-${BOOST_LIB_EXT_NAME_RELEASE}

	debug
	${BOOST_LIB_PREFIX_NAME}boost_unit_test_framework-${BOOST_LIB_EXT_NAME_DEBUG}

	optimized
	${BOOST_LIB_PREFIX_NAME}boost_unit_test_framework-${BOOST_LIB_EXT_NAME_RELEASE}

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

	debug
	${BOOST_LIB_PREFIX_NAME}boost_program_options-${BOOST_LIB_EXT_NAME_DEBUG}

	optimized
	${BOOST_LIB_PREFIX_NAME}boost_program_options-${BOOST_LIB_EXT_NAME_RELEASE}
)

IF(NOT WIN32)
TARGET_LINK_LIBRARIES(
	FuegoUnitTest

    general
    pthread
)
ENDIF(NOT WIN32)


ADD_DEPENDENCIES(FuegoUnitTest FuegoLib)

