cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES("../src")

# ADD_CUSTOM_COMMAND(TARGET copy PRE_BUILD
# 	COMMAND ${CMAKE_CMMAND} -E copy book.json ${CMAKE_CURRENT_BINARY_DIR}
# )

FILE(COPY book.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

SET(TEST_SOURCES
	test_json_assign_fail.cpp
	test_json_assign_value.cpp
	test_json_parser.cpp
	#test_book_json.cpp
)

SET(TEST_LIBS ${PROJECT_NAME})

CREATE_TEST_SOURCELIST( TEST_SRCS
	tests_jsonlite.cpp
	${TEST_SOURCES}
)

ADD_EXECUTABLE(tests_jsonlite ${TEST_SRCS})
TARGET_LINK_LIBRARIES(tests_jsonlite ${TEST_LIBS})

REMOVE(TEST_SRCS tests_jsonlite.cpp)

FOREACH(test ${TEST_SRCS})
	GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
	ADD_TEST(${TName} tests_jsonlite ${TName})
ENDFOREACH()
 