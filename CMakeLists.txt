cmake_minimum_required(VERSION 3.21)
project(MemoryAssignment C)

set(CMAKE_C_STANDARD 99)

add_executable(MemoryAssignment mymem.c memorytests.c mymem.h testrunner.c testrunner.h memorytests.o)
