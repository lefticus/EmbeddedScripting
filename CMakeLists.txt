cmake_minimum_required (VERSION 2.8.6)

project (EmbeddedScripting CXX)

find_package(Ruby 1.9 REQUIRED)
find_package(SWIG 3.0.8 REQUIRED)

include(UseSWIG)


set(BUILD_SHARED_LIBS OFF)

set_source_files_properties(EmbeddedScripting.i
  PROPERTIES CPLUSPLUS true
  )

if (UNIX)
  set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

include_directories(${RUBY_INCLUDE_DIR} ${RUBY_CONFIG_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

if (MSVC)
  set_target_properties(${swig_target} 
    PROPERTIES COMPILE_FLAGS "/DSTATIC /bigobj /DBUILDING_NODE_EXTENSION /wd4996")  ## /wd4996 suppresses deprecated warnings
elseif(UNIX)
  set_source_files_properties( EmbeddedScriptingRUBY_wrap.cxx
    PROPERTIES COMPILE_FLAGS "-DSTATIC -Wno-deprecated-declarations")
endif()


add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/SWIGRubyRuntime.hxx"
  COMMAND "${SWIG_EXECUTABLE}"
          "-ruby"
          -external-runtime "${CMAKE_CURRENT_BINARY_DIR}/SWIGRubyRuntime.hxx"
)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/EmbeddedScriptingRUBY_wrap.cxx"
  COMMAND "${SWIG_EXECUTABLE}"
          "-ruby"
          "-c++"
          -o "${CMAKE_CURRENT_BINARY_DIR}/EmbeddedScriptingRUBY_wrap.cxx"
          "${CMAKE_CURRENT_SOURCE_DIR}/EmbeddedScripting.i"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/EmbeddedScripting.i"
)

add_library(EmbeddedScriptingRUBY
  OBJECT
  "${CMAKE_CURRENT_BINARY_DIR}/EmbeddedScriptingRUBY_wrap.cxx"
  )

add_executable(EmbeddedScripting
 "${CMAKE_CURRENT_BINARY_DIR}/SWIGRubyRuntime.hxx"
 $<TARGET_OBJECTS:EmbeddedScriptingRUBY>
 main.cpp
 )

target_link_libraries(EmbeddedScripting ${RUBY_LIBRARY})



