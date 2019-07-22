cmake_minimum_required(VERSION 3.0.2) 

project(asenum DESCRIPTION "AssociatedEnum: header-only library for C++ for enumerations with associated values.")

if (NOT WIN32)
    add_compile_options(-std=c++11)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-stdlib=libc++)
    endif()
endif()

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9")
endif()

OPTION(ASENUM_TESTING_ENABLE "Build AssEnum's unit-tests." OFF)

### asenum library ###

add_library(asenum INTERFACE)

target_include_directories(asenum INTERFACE "include")


### asenum unit-tests ###

if (ASENUM_TESTING_ENABLE)
    set(TEST_SOURCES
        tests/AsEnumTest.cpp
    )
    add_executable(asenum_tests ${TEST_SOURCES})

    # setup 3rdParty
    add_subdirectory(3rdParty/googletest)
    set_target_properties(gmock PROPERTIES FOLDER 3rdParty)
    set_target_properties(gmock_main PROPERTIES FOLDER 3rdParty)
    set_target_properties(gtest PROPERTIES FOLDER 3rdParty)
    set_target_properties(gtest_main PROPERTIES FOLDER 3rdParty)

    target_link_libraries(asenum_tests asenum gtest gmock gmock_main)
endif()
