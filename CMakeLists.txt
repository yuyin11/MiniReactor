CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(MiniReactor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(Threads REQUIRED)

add_subdirectory(net)

add_subdirectory(examples)

enable_testing()
add_subdirectory(tests)
