cmake_minimum_required(VERSION 3.17)
project(cpp_project)

set(CMAKE_CXX_STANDARD 14)

set(BOOST_ROOT "C:/boost_1_75_0")
set(BOOSTROOT "C:/boost_1_75_0")
find_package(Boost 1.75.0)
set(Boost_INCLUDE_DIR C:/boost_1_75_0/)
set(Boost_LIBRARY_DIR C:/boost_1_75_0/stage/lib)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})



add_executable(cpp_project th_main.cpp stb_image_write.h Task.cpp Task.h ThreadPool.cpp ThreadPool.h)