# Copyright 2018 Pawel Bylica.
# Licensed under the Apache License, Version 2.0. See the LICENSE file.

hunter_add_package(benchmark)
find_package(benchmark CONFIG REQUIRED)

add_executable(bench benchmarks.cpp)
target_link_libraries(bench PRIVATE ethash benchmark::benchmark)
target_include_directories(bench PRIVATE ${ETHASH_PRIVATE_INCLUDE_DIR})