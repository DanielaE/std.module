cmake_minimum_required(VERSION 3.26)

unset(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API)
unset(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP)

add_library(allstd OBJECT ${allstd-hu})
get_target_property(standard std CXX_STANDARD)
set_target_properties(allstd PROPERTIES CXX_STANDARD ${standard})
target_compile_features(allstd PUBLIC cxx_std_20)
if (MSVC)
  target_compile_options(allstd
    PUBLIC ${std-lang-additional}
    PRIVATE /exportHeader /ifcOutput ${allstd-bmi}
    INTERFACE /headerUnit ${allstd-hu}=${allstd-bmi} #/wd5050
  )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(allstd
    PUBLIC ${std-lang-additional}
    PRIVATE ${std-lib-additional} -Wno-builtin-macro-redefined -xc++-system-header --precompile
  )
  add_custom_target(copyBMI
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_OBJECTS:allstd> ${allstd-bmi}
    COMMAND_EXPAND_LISTS
    DEPENDS $<TARGET_OBJECTS:allstd>
    BYPRODUCTS ${allstd-bmi}
  )
endif()

