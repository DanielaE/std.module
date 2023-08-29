cmake_minimum_required(VERSION 3.26)

if (CMAKE_CXX_STANDARD LESS 20)
  message(FATAL_ERROR "At least C++20 required but have ${CMAKE_CXX_STANDARD}")
endif()

project(std
        DESCRIPTION "A modularized standard library for C++23"
        LANGUAGES CXX
)

if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.27)
  set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "aa1f7df0-828a-4fcd-9afc-2dc80491aca7")
else ()
  set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
endif()
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CXX_STANDARD_REQUIRED ON)

option(USE_EMULATION "use emulation of module 'std' instead of the library-provided one" OFF)

add_library(std STATIC)
# the default PMIF is 'std.ixx'
set(pmif std.ixx)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
set(std-library libc++)
# I do mean C++20
  set(std-lang-additional -fsized-deallocation -faligned-allocation -fchar8_t)
# libc++ has no support for a C++23 module 'std'
# fall back to emulation, don't define _BUILD_STD_MODULE
  set(USE_EMULATION ON)  
elseif (MSVC)
# I do mean C++23
  set(std-lang-additional /utf-8 /Zc:__cplusplus /Zc:throwingNew /Zc:inline /Zc:externConstexpr /Zc:templateScope /Zc:checkGwOdr /Zc:enumTypes)
  set(USE_EMULATION ON)  
endif()

target_sources(std
  PUBLIC
    FILE_SET modules TYPE CXX_MODULES
      FILES ${pmif}
)
target_compile_features(std PUBLIC cxx_std_23)

if (USE_EMULATION)
  set(allstd-hu ${CMAKE_CURRENT_SOURCE_DIR}/allstd.cpp)
  if (MSVC)
    set(allstd-bmi ${CMAKE_CURRENT_BINARY_DIR}/allstd.ifc)
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(allstd-bmi ${CMAKE_CURRENT_BINARY_DIR}/allstd.pcm)
  else()
    message(FATAL_ERROR "Sorry, unsupported compiler!")
  endif()

  add_subdirectory(allstd)

  if (MSVC)
    target_link_libraries(std PUBLIC allstd)
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(std
      PUBLIC ${std-lang-additional} -stdlib=${std-library}
      PRIVATE  -Wno-reserved-module-identifier -Wno-experimental-header-units -fmodule-file=${allstd-bmi}
    )
    set_source_files_properties(${pmif} PROPERTIES OBJECT_DEPENDS ${allstd-bmi})
  endif()

else() # not emulated, true named module
  target_compile_definitions(std PRIVATE _BUILD_STD_MODULE)
  if (MSVC)
  # right, but please don't complain about #includes in the module purview
    target_compile_options(std PRIVATE /wd5244)
  endif()

endif() #emulated

add_executable(std-test test.cpp)
target_compile_features(std-test PRIVATE cxx_std_20)
target_link_libraries(std-test PRIVATE std c++)
