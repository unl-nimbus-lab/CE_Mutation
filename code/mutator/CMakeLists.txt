cmake_minimum_required(VERSION 3.6)
project(mutator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    b747cl.c
    b747cl.h
    b747cl_data.c
    b747cl_private.h
    b747cl_types.h
    builtin_typeid_types.h
    lib.cpp
    lib.h
    Makefile
    multiword_types.h
    mutator.cpp
    mutator_b1.cpp
    mutator_base.cpp
    rt_defines.h
    rt_nonfinite.c
    rt_nonfinite.h
    rtGetInf.c
    rtGetInf.h
    rtGetNaN.c
    rtGetNaN.h
    rtmodel.h
    rtwtypes.h
    runconfiguration.cpp
    sum.cpp
    sum.h
    temp.cpp
    test.cpp
    zero_crossing_types.h)

add_executable(mutator ${SOURCE_FILES})