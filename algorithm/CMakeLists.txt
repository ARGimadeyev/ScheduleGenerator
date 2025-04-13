cmake_minimum_required(VERSION 3.30)
project(ScheduleGenerator)

set(CMAKE_CXX_STANDARD 26)

add_executable(ScheduleGenerator main.cpp
        lib/scheduler.h)
