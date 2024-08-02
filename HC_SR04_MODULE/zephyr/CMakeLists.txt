# SPDX-License-Identifier: Apache-2.0
# Additional modules
set(ZEPHYR_EXTRA_MODULES "${CMAKE_SOURCE_DIR}/modules/hc_sr04")

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(LDR_ZephyrOS)

target_sources(app PRIVATE ../src/main.cpp)