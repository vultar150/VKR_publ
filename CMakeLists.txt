cmake_minimum_required(VERSION 3.14)

project(VKR_publ LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(MYCOMPILE_FLAGS "-O3")

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${MYCOMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${MYCOMPILE_FLAGS}")

include_directories(/src)
include_directories(/data)

add_executable(VKR_publ
  src/main.cpp
  src/classes.cpp
  src/classes.h
  src/functions.cpp
  src/functions.h
  src/tinyxml2.cpp
  src/tinyxml2.h
)

add_executable(generator
  src/generator.cpp
  src/tinyxml2.cpp
  src/tinyxml2.h
)

add_executable(generator_lb
  src/generator_lb.cpp
  src/tinyxml2.cpp
  src/tinyxml2.h
)
