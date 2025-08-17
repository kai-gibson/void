cmake_minimum_required(VERSION 3.5)

project(void)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES 
  src/void.cxx
  src/error.cxx
  src/types.cxx
  src/utils.cxx
  src/parser.cxx
)

add_executable(void ${SOURCES})
target_include_directories(void PRIVATE include)

add_custom_command(
    TARGET void
    POST_BUILD
    DEPENDS void
    COMMAND void
    ARGS ${CMAKE_SOURCE_DIR}/test.void
)
