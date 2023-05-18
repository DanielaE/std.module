cmake_minimum_required(VERSION 3.26)

if (CMAKE_CXX_STANDARD LESS 20)
  message(FATAL_ERROR "At least C++20 required but have ${CMAKE_CXX_STANDARD}")
endif()

project(std
        DESCRIPTION "A modularized standard library for C++23"
        LANGUAGES CXX
)

set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(std STATIC)
target_compile_features(std PUBLIC cxx_std_20)

if (MSVC)

target_sources(std
  PUBLIC
    FILE_SET modules TYPE CXX_MODULES
      FILES std.ixx
)
target_compile_options(std PRIVATE /wd5244)
target_compile_definitions(std PRIVATE -D_BUILD_STD_MODULE)
target_include_directories(std BEFORE PRIVATE ../include)

else()

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/allstd.pcm
  COMMAND ${CMAKE_CXX_COMPILER} -std=c++20 -stdlib=libstdc++ -fsized-deallocation -D_PSTL_PAR_BACKEND_SERIAL
    -fmodule-header ${CMAKE_SOURCE_DIR}/allstd.hpp -o ${CMAKE_CURRENT_BINARY_DIR}/allstd.pcm
)
add_library(allstd INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/allstd.pcm)
target_compile_features(allstd INTERFACE cxx_std_20)
target_compile_options(allstd INTERFACE -stdlib=libstdc++ -fsized-deallocation)

target_sources(std
  PUBLIC
    FILE_SET modules TYPE CXX_MODULES
      FILES std.cppm
    FILE_SET headerunits TYPE CXX_MODULE_HEADER_UNITS
      FILES allstd.hpp
)
target_compile_options(std PUBLIC -stdlib=libstdc++ -fsized-deallocation PRIVATE -fmodule-file=allstd.pcm -fmodule-output)
target_link_libraries(std PRIVATE allstd)

endif()

add_executable(test test.cpp)
target_compile_features(test PRIVATE cxx_std_20)
target_link_libraries(test PRIVATE std)