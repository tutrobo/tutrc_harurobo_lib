cmake_minimum_required(VERSION 3.22)

project(tutrc_harurobo_lib)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE
  src/can.cpp
  src/gpio.cpp
  src/stdio.cpp
)

target_include_directories(${PROJECT_NAME} INTERFACE
  include
)
