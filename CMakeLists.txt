cmake_minimum_required(VERSION 3.10)

#Project name
project(vPC-v4 VERSION 0.1)

#Include directories
include_directories(include)

#Source directories
file(GLOB SOURCES "source/*.cpp")

#Executable
add_executable(vPC-v4 ${SOURCES})
