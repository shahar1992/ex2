cmake_minimum_required(VERSION 3.7)
project(ex2-1)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        challenge.c
        challenge.h
        challenge_room_system_fields.h
        challenge_system.c
        challenge_system.h
        challenge_system_test_1.c
        constants.h
        system_additional_types.h
        visitor_room.c
        visitor_room.h)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic-errors -Werror -DNDEBUG")
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(ex2-1 ${SOURCE_FILES})