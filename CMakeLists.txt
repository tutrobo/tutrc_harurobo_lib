cmake_minimum_required(VERSION 3.22)

project(tutrc_harurobo_lib)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE
  src/can.cpp
  src/encoder.cpp
  src/fdcan.cpp
  src/gpio.cpp
  src/pwm.cpp
  src/uart.cpp
)

target_include_directories(${PROJECT_NAME} INTERFACE
  include
)

find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN_OUTPUT_LANGUAGE Japanese)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
  doxygen_add_docs(docs
    README.md
    ${CMAKE_CURRENT_SOURCE_DIR}
  )
endif()
