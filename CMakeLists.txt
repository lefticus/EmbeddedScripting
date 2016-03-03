cmake_minimum_required (VERSION 2.8.6)

project (EmbeddedScripting CXX)

find_package(Ruby 1.9 REQUIRED)
find_package(SWIG REQUIRED)

# boost is only needed for the extraction stuff. It's easier that way

if (APPLE OR WIN32)
  set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(Boost COMPONENTS filesystem system)

include(UseSWIG)




function(embed_files )
  set(OUTFILE "${CMAKE_BINARY_DIR}/embedded_files.cpp")
  file(WRITE  ${OUTFILE}  "#include <embedded_files.hpp>\n ")
  file(APPEND ${OUTFILE}  "namespace embedded_files {\n ")

  math(EXPR ARGCEND "${ARGC} - 1")
  math(EXPR NUMFILES "${ARGC} / 2")
  math(EXPR MAXFILECOUNT "${NUMFILES} - 1")

  file(APPEND ${OUTFILE} "  static const size_t embedded_file_count = ${NUMFILES};\n" )

  foreach (arg RANGE 0 ${ARGCEND} 2)
    list(GET ARGV ${arg} INFILE)
    math(EXPR argplus1 "${arg} + 1")
    math(EXPR filenum "${arg} / 2")
    list(GET ARGV ${argplus1} OUTNAME)

    message(STATUS "Embedding '${INFILE}' as '${OUTNAME}'")

    file(READ ${INFILE} FILEDATA HEX)
    string(LENGTH ${FILEDATA} STRLEN)
    math(EXPR filelen "${STRLEN} / 2")

    math(EXPR STRLENEND "${STRLEN} - 1")
    set(FILEDATASTR "")
    foreach (byte RANGE 0 ${STRLENEND} 2)
      string(SUBSTRING ${FILEDATA} ${byte} 2 bytedata)
      set(FILEDATASTR "${FILEDATASTR} 0x${bytedata},")
    endforeach()
    math(EXPR EXPECTEDLENGTH "(${filelen} * 6) - 1")
    string(SUBSTRING ${FILEDATASTR} 0 ${EXPECTEDLENGTH} FILEDATASTR)

    file(APPEND ${OUTFILE} "  static const uint8_t embedded_file_${filenum}[] = {\n    ${FILEDATASTR}\n  };\n" )
    file(APPEND ${OUTFILE} "  static const char *embedded_file_name_${filenum} = \"${OUTNAME}\";\n" )
    file(APPEND ${OUTFILE} "  static const size_t embedded_file_len_${filenum} = ${filelen};\n" )

  endforeach()

  # file names
  file(APPEND ${OUTFILE} "  static const char *embedded_file_names[] = {\n")
  foreach (arg RANGE 0 ${MAXFILECOUNT})
    file(APPEND ${OUTFILE} "    embedded_file_name_${arg}")
    if (NOT arg EQUAL MAXFILECOUNT)
      file(APPEND ${OUTFILE} ",")
    endif()
    file(APPEND ${OUTFILE} "\n")
  endforeach()
  file(APPEND ${OUTFILE} "  };\n")

  # lengths
  file(APPEND ${OUTFILE} "  static const size_t embedded_file_lens[] = {\n")
  foreach (arg RANGE 0 ${MAXFILECOUNT})
    file(APPEND ${OUTFILE} "    embedded_file_len_${arg}")
    if (NOT arg EQUAL MAXFILECOUNT)
      file(APPEND ${OUTFILE} ",")
    endif()
    file(APPEND ${OUTFILE} "\n")
  endforeach()
  file(APPEND ${OUTFILE} "  };\n")

  # datas
  file(APPEND ${OUTFILE} "  static const uint8_t *embedded_files[] = {\n")
  foreach (arg RANGE 0 ${MAXFILECOUNT})
    file(APPEND ${OUTFILE} "    embedded_file_${arg}")
    if (NOT arg EQUAL MAXFILECOUNT)
      file(APPEND ${OUTFILE} ",")
    endif()
    file(APPEND ${OUTFILE} "\n")
  endforeach()
  file(APPEND ${OUTFILE} "  };\n")

  file(APPEND ${OUTFILE} "    std::map<std::string, std::pair<size_t, const uint8_t *>> files()\n")
  file(APPEND ${OUTFILE} "    {\n")
  file(APPEND ${OUTFILE} "      std::map<std::string, std::pair<size_t, const uint8_t *>> fs;\n")
  file(APPEND ${OUTFILE} "      for (size_t i = 0; i < embedded_file_count; ++i) {\n")
  file(APPEND ${OUTFILE} "        fs.insert(std::make_pair(std::string(embedded_file_names[i]), \n")
  file(APPEND ${OUTFILE} "                                 std::make_pair(embedded_file_lens[i],\n")
  file(APPEND ${OUTFILE} "                                                embedded_files[i])));\n")
  file(APPEND ${OUTFILE} "      }\n")
  file(APPEND ${OUTFILE} "      return fs;\n")
  file(APPEND ${OUTFILE} "    }\n")


  file(APPEND ${CMAKE_BINARY_DIR}/embedded_files.cpp "} //close namespace")
endfunction()




embed_files(
  "${CMAKE_CURRENT_SOURCE_DIR}/test.rb" "myvfs/test.rb"
  "${CMAKE_CURRENT_SOURCE_DIR}/test2.rb" "myvfs/test2.rb"
  "${CMAKE_CURRENT_SOURCE_DIR}/test3.rb" "extracted/test3.rb"
  
  )


set(BUILD_SHARED_LIBS OFF)

set_source_files_properties(EmbeddedScripting.i
  PROPERTIES CPLUSPLUS true
  )

if (UNIX)
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pthread ")

  option(ENABLE_ADDRESS_SANITIZER "Enable address sanitizer testing in gcc/clang" FALSE)
  if(ENABLE_ADDRESS_SANITIZER)
    add_definitions(-fsanitize=address -g)
    set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=address")
  endif()

  option(ENABLE_MEMORY_SANITIZER "Enable memory sanitizer testing in gcc/clang" FALSE)
  if(ENABLE_MEMORY_SANITIZER)
    add_definitions(-fsanitize=memory -g)
    set(LINKER_FLAGS "${LINKER_FLAGS} -fsanitize=memory")
  endif()
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKER_FLAGS}")

include_directories(${RUBY_INCLUDE_DIR} ${RUBY_CONFIG_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIR})


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
          "embedded_files.hpp"
)

#if (MSVC)
#  set_source_files_properties( EmbeddedScriptingRUBY_wrap.cxx 
#    PROPERTIES COMPILE_FLAGS "/DSTATIC /bigobj /wd4996")  ## /wd4996 suppresses deprecated warnings
#elseif(UNIX)
#  set_source_files_properties( EmbeddedScriptingRUBY_wrap.cxx
#    PROPERTIES COMPILE_FLAGS "-DSTATIC -Wno-deprecated-declarations")
#endif()



add_executable(EmbeddedScripting
 "${CMAKE_CURRENT_BINARY_DIR}/SWIGRubyRuntime.hxx"
 main.cpp
 RubyException.hpp
 RubyInterpreter.hpp
 embedded_files.hpp
 "${CMAKE_CURRENT_BINARY_DIR}/embedded_files.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/EmbeddedScriptingRUBY_wrap.cxx"
 )


if (NOT MSVC)
  target_link_libraries(EmbeddedScripting ${Boost_LIBRARIES} ${RUBY_LIBRARY}  "dl" "crypt")
else()
  target_link_libraries(EmbeddedScripting ${Boost_LIBRARIES} ${RUBY_LIBRARY} )
endif()



if(MSVC)
  get_filename_component(RUBY_EXECUTABLE_DIR "${RUBY_EXECUTABLE}" DIRECTORY)
  if(CMAKE_CL_64)
    set(RUBY_DLL x64-msvcrt-ruby200.dll)
    set(RUBY_DLLPATH "${RUBY_EXECUTABLE_DIR}/${RUBY_DLL}")
    set(RUBY_LIBRARY "${RUBY_POSSIBLE_LIB_DIR}/libx64-msvcrt-ruby200.dll.a")
  else()
    set(RUBY_DLL msvcrt-ruby200.dll)
    set(RUBY_DLLPATH "${RUBY_EXECUTABLE_DIR}/${RUBY_DLL}")
    set(RUBY_LIBRARY "${RUBY_POSSIBLE_LIB_DIR}/libmsvcrt-ruby200.dll.a")
  endif()




  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release)
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Debug)
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/RelWithDebInfo)
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/MinSizeRel)

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${RUBY_DLLPATH}" "${CMAKE_BINARY_DIR}/Release/${RUBY_DLL}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${RUBY_DLLPATH}" "${CMAKE_BINARY_DIR}/Debug/${RUBY_DLL}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${RUBY_DLLPATH}" "${CMAKE_BINARY_DIR}/RelWithDebInfo/${RUBY_DLL}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${RUBY_DLLPATH}" "${CMAKE_BINARY_DIR}/MinSizeRel/${RUBY_DLL}")
endif()
