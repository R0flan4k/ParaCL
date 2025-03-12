cmake_minimum_required(VERSION 3.11)

project(ParaCLTests)

find_package(GTest REQUIRED)

set(SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
)

add_executable(
	TestSerialization ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp)

target_compile_features(TestSerialization PUBLIC cxx_std_20)
target_include_directories(TestSerialization PUBLIC
	${CMAKE_SOURCE_DIR}/ParaCL/include
	${GTEST_INCLUDE_DIRS}
)
target_link_libraries(
	TestSerialization PUBLIC ${GTEST_LIBRARIES} GTest::gtest GTest::gtest_main)

enable_testing()
add_test(TestSerialization "./TestSerialization")
