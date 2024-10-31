cmake_minimum_required(VERSION 3.26)
project(Weiji C)

set(CMAKE_C_STANDARD 11)

add_executable(Weiji main.c
        cpu.c
        cpu.h
        address.c
        address.h
        instruction.c
        instruction.h
        biu.c
        biu.h
        eu.c
        eu.h
        test5.c
        test5.h
        Chip2164.c
        Chip2164.h)
