cmake_minimum_required(VERSION 3.8)
project(__YAML_BDD2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        resources/arguments.c
        resources/arguments.h
        resources/Callback.c
        resources/Callback.h
        resources/dependancies.h
        resources/Parser.c
        resources/Parser.h
        main.c)

add_executable(__YAML_BDD2 ${SOURCE_FILES})