# SPDX-License-Identifier: Apache-2.0
set(ZEPHYR_EXTRA_MODULES "${CMAKE_SOURCE_DIR}/modules/hc_sr04")

cmake_minimum_required(VERSION 3.20.0)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(GUI_BME)

target_sources(app PRIVATE ../src/main.cpp)
