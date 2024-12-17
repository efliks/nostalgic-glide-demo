cmake_minimum_required(VERSION 3.13)

project(dumydemo.exe)

set(GCC_COVERAGE_COMPILE_FLAGS "-W -Wall -pedantic")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

add_executable(dumydemo.exe bumpmain.c bumpobj.c bumptri.c main.c math3d.c raster.c template.c texture.c)
add_subdirectory("dummy")

target_link_libraries(dumydemo.exe PUBLIC m)
