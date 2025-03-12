# ctest --test-dir ./build/Debug/e2e 

cmake_minimum_required(VERSION 3.11)

project(Matrixe2e)

enable_testing()
set(MATRIX_TESTS "${CMAKE_CURRENT_SOURCE_DIR}/data")

file(GLOB testfiles "${MATRIX_TESTS}/*.dat")
message("${CMAKE_BINARY_DIR}")
foreach(file ${testfiles})
  add_test(
    NAME ${file}
    COMMAND bash -c "${CMAKE_CURRENT_SOURCE_DIR}/runtest.sh ${file} ./Matrix.x"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(${file} PROPERTIES DEPENDS Matrix.x)
endforeach()
