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
set(CXX_STANDARD_REQUIRED ON)

add_library(std STATIC)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
# Clang requires a horrible hack using invalid C++ syntax
  set(pmif std.cppm)
# libstdc++ is required because libc++
#  - is not even at C++17
#  - is lacking essential C++20 features
  set(std-library libstdc++)
# libstdc++ has a buggy implementation of std::ranges in C++23 mode
  set(CMAKE_CXX_STANDARD 20)
  target_compile_features(std PUBLIC cxx_std_20)
# I do mean C++20
  set(std-lang-additional -fsized-deallocation)
# the TBB of libstdc++ is not importable
  set(std-lib-additional -D_PSTL_PAR_BACKEND_SERIAL)
# libstdc++ has no support for a C++23 module 'std'
# fall back to emulation, don't define _BUILD_STD_MODULE

else()
# these are the desired defaults
  set(pmif std.ixx)
  target_compile_features(std PUBLIC cxx_std_23)
  target_compile_definitions(std PRIVATE -D_BUILD_STD_MODULE)
endif()

target_sources(std
  PUBLIC
    FILE_SET modules TYPE CXX_MODULES
      FILES ${pmif}
)

if (MSVC)

# right, but please don't complain about #includes in the module purview
target_compile_options(std PRIVATE /wd5244)
# enable using a custom standard library
target_include_directories(std BEFORE PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

set(allstd-hu ${CMAKE_CURRENT_SOURCE_DIR}/allstd.hpp)
set(allstd-bmi ${CMAKE_CURRENT_BINARY_DIR}/allstd.pcm)

add_library(allstd INTERFACE ${allstd-bmi})
target_compile_options(allstd INTERFACE -stdlib=${std-library} ${std-lang-additional})

target_compile_options(std
  PUBLIC ${std-lang-additional}
  PRIVATE -fmodule-file=${allstd-bmi} -fmodule-output
)
target_link_libraries(std PRIVATE allstd)

get_target_property(std-standard std CXX_STANDARD)
add_custom_command(OUTPUT ${allstd-bmi}
  COMMAND ${CMAKE_CXX_COMPILER} -std=c++${std-standard} -stdlib=${std-library}
    ${std-lang-additional} ${std-lib-additional}
    -fmodule-header ${allstd-hu} -o ${allstd-bmi}
)

else()
  message(FATAL_ERROR "Sorry, unsupported compiler!")
endif()

add_executable(std-test test.cpp)
target_compile_features(std-test PRIVATE cxx_std_20)
target_link_libraries(std-test PRIVATE std)
