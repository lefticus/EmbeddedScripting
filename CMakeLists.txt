cmake_minimum_required (VERSION 2.8.6)

project (EmbeddedScripting CXX)

 find_package(Ruby 1.9 REQUIRED)
find_package(SWIG 3.0.8 REQUIRED)

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
  "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" "main.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/RubyInterpreter.hpp" "some/new/loc/RubyInterpreter.hpp"
  
  )


set(BUILD_SHARED_LIBS OFF)

set_source_files_properties(EmbeddedScripting.i
  PROPERTIES CPLUSPLUS true
  )

if (UNIX)
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pthread ")
endif()

include_directories(${RUBY_INCLUDE_DIR} ${RUBY_CONFIG_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

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


add_executable(EmbeddedScripting
 "${CMAKE_CURRENT_BINARY_DIR}/SWIGRubyRuntime.hxx"
 main.cpp
 "${CMAKE_CURRENT_BINARY_DIR}/embedded_files.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/EmbeddedScriptingRUBY_wrap.cxx"
 )

target_link_libraries(EmbeddedScripting ${RUBY_LIBRARY} "dl" "crypt")



