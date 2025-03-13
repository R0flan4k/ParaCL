# ctest --test-dir ./build/Debug/e2e 

cmake_minimum_required(VERSION 3.11)

project(ParaCLe2e)

enable_testing()
set(PARACL_TESTS "${CMAKE_CURRENT_SOURCE_DIR}/data")

file(GLOB testfiles "${PARACL_TESTS}/*.dat")
file(GLOB srcfiles  "${PARACL_TESTS}/*.pcl")
message("${CMAKE_BINARY_DIR}")
foreach(src_file ${srcfiles})
      	add_test(
    		NAME ${src_file}
    		COMMAND bash -c "${CMAKE_CURRENT_SOURCE_DIR}/runtest.sh ${src_file} ./ParaCL.x"
   		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
		set_tests_properties(${src_file} PROPERTIES DEPENDS ParaCL.x)
endforeach()

