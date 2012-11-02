ADD_SUBDIRECTORY( gtest )

APPEND_SOURCE_FILES(TEST_SOURCES)
APPEND_SOURCE_FILES(TEST_SOURCES BaseLib)
APPEND_SOURCE_FILES(TEST_SOURCES GeoLib)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/Tests/gtest
	${CMAKE_SOURCE_DIR}/BaseLib
	${CMAKE_SOURCE_DIR}/GeoLib
)

ADD_EXECUTABLE (testrunner testrunner.cpp ${TEST_SOURCES})
SET_TARGET_PROPERTIES(testrunner PROPERTIES FOLDER Testing)

TARGET_LINK_LIBRARIES(testrunner
	GTest
	BaseLib
	GeoLib
)

# Add make-target test which runs the testrunner
# This should override CTest's predefined test-target but it does not
ADD_CUSTOM_TARGET(test
	${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/testrunner
	DEPENDS testrunner
)

# Testing
#ENABLE_TESTING()

# Creates one ctest entry for every googletest
#ADD_GOOGLE_TESTS ( ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/testrunner ${TEST_SOURCES})