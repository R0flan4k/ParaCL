cmake_minimum_required(VERSION 3.11)
project(ParaCL)
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address -fsanitize=undefined")
endif()

find_package(FLEX  REQUIRED)
find_package(BISON REQUIRED)

#set(BISON_EXECUTABLE "/usr/bin/bison")

#if (BISON_FOUND)
#	MESSAGE(${BISON_EXECUTABLE} "lol")
#	MESSAGE(${BISON_VERSION})
#else()
#	MESSAGE(Bison not founded)
#endif()

set(BISON_VARS
    BISON_FOUND
    BISON_EXECUTABLE
    BISON_VERSION
)

foreach(BISON_VAR ${BISON_VARS})
    message("${BISON_VAR}: ${${BISON_VAR}}")
    if(NOT ${BISON_VAR})
        message(FATAL_ERROR "${BISON_VAR} NOT FOUND")
    endif()
endforeach()


if (NOT DEFINED GRAMMAR)
	set(GRAMMAR "${CMAKE_SOURCE_DIR}/ParaCL/src/parser.y" CACHE STRING "file with grammar" FORCE)
endif()

flex_target(scanner
	${CMAKE_SOURCE_DIR}/ParaCL/src/scanner.l
        ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

bison_target(parser
        ${GRAMMAR}
        ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
  	COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hh -Wnone"
)

add_flex_bison_dependency(scanner parser)

add_executable(ParaCL.x
	${CMAKE_SOURCE_DIR}/ParaCL/src/paracl.cpp
        ${FLEX_scanner_OUTPUTS}
        ${BISON_parser_OUTPUTS}
)

target_compile_features(ParaCL.x PUBLIC cxx_std_20)
if((NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
        target_compile_options(ParaCL.x PUBLIC -std=c++20 -Wall -g -O0)
endif()
target_sources(ParaCL.x PRIVATE ${SRCS})
target_include_directories(ParaCL.x PUBLIC "${CMAKE_SOURCE_DIR}/ParaCL/include" "${CMAKE_BINARY_DIR}")
# target_link_libraries(ParaCL.x PUBLIC bison::bison)

set(CLANG_FORMAT_SRCS
        ${SRCS}
	${CMAKE_SOURCE_DIR}/ParaCL/include/*.h
#        ${CMAKE_SOURCE_DIR}/unit_tests/*.cpp
)
add_custom_target(
        clangformat
        ALL
        COMMAND clang-format -style=file -i ${CLANG_FORMAT_SRCS}
)

# add_subdirectory(${CMAKE_SOURCE_DIR}/unit_tests)
add_subdirectory(${CMAKE_SOURCE_DIR}/e2e)

