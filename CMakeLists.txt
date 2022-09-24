cmake_minimum_required(VERSION 3.0)

project(brrUtils)

add_library(brr STATIC brrUtils.c)

add_executable(brrenc brrenc.c)
add_executable(brrdec brrdec.c)

target_link_libraries(brrenc brr sndfile)
target_link_libraries(brrdec brr sndfile)
