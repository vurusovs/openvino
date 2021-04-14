# Copyright (C) 2018-2021 Intel Corporation
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.13)

set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type")

project(time_tests_reproducer)

find_package(InferenceEngine)
if (NOT InferenceEngine_FOUND)
    set (HAVE_SYS_STAT_H 1)
    set (HAVE_INTTYPES_H 1)
    set (INTTYPES_FORMAT C99)
    find_package(InferenceEngineDeveloperPackage REQUIRED)
endif()

set (TARGET_NAME "time_tests_reproducer")
file (GLOB HDR *.h)
file (GLOB SRC *.cpp)
add_executable(${TARGET_NAME} ${HDR} ${SRC})

include(FetchContent)
FetchContent_Declare(
    gflags
    GIT_REPOSITORY "https://github.com/gflags/gflags.git"
    GIT_TAG "v2.2.2"
)
FetchContent_GetProperties(gflags)
if(NOT gflags_POPULATED)
    FetchContent_Populate(gflags)
    add_subdirectory(${gflags_SOURCE_DIR} ${gflags_BINARY_DIR})
endif()

target_link_libraries(${TARGET_NAME}
    gflags
    IE::inference_engine
    #${InferenceEngine_LIBRARIES}
    )