cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(dfhooks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES dfhooks.cpp LibWrapper.cpp ChainLoader.cpp)

add_library(dfhooks SHARED ${SOURCES})
