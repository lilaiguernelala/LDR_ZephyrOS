# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(PeriodicTasks)

target_sources(app PRIVATE ../src/main.c)
target_sources_ifdef(CONFIG_TRACING_USER app PRIVATE ../src/tracing_user.c)
