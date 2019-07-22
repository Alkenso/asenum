cmake_minimum_required(VERSION 3.0.2) 

project(assenum DESCRIPTION "AssociatedEnum: header-only library for C++ for enumerations with associated values.")

if (NOT WIN32)
    add_compile_options(-std=c++11)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-stdlib=libc++)
    endif()
endif()

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9")
endif()

OPTION(ASSENUM_TESTING_ENABLE "Build AssEnum's unit-tests." OFF)

### assenum library ###

add_library(assenum INTERFACE)

target_include_directories(assenum INTERFACE "include")


### assenum unit-tests ###

if (ASSENUM_TESTING_ENABLE)
    set(TEST_SOURCES
        tests/AssEnumTest.cpp
    )
    add_executable(assenum_tests ${TEST_SOURCES})

    # setup 3rdParty
    add_subdirectory(3rdParty/googletest)
    set_target_properties(gmock PROPERTIES FOLDER 3rdParty)
    set_target_properties(gmock_main PROPERTIES FOLDER 3rdParty)
    set_target_properties(gtest PROPERTIES FOLDER 3rdParty)
    set_target_properties(gtest_main PROPERTIES FOLDER 3rdParty)

    target_link_libraries(assenum_tests assenum gtest gmock gmock_main)
endif()
