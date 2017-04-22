include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/atn
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dfa
  ${CMAKE_CURRENT_SOURCE_DIR}/src/misc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/support
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tree
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/pattern
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/xpath
)

file(GLOB libantlrcpp_SRC
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/atn/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/dfa/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/misc/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/support/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/tree/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/tree/pattern/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/tree/xpath/*.cpp"
)

add_llvm_library(LLVMAntlrRuntime ${libantlrcpp_SRC}

  ${CMAKE_CURRENT_SOURCE_DIR}/src/
  ${CMAKE_CURRENT_SOURCE_DIR}/src/atn/
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dfa/
  ${CMAKE_CURRENT_SOURCE_DIR}/src/misc/
  ${CMAKE_CURRENT_SOURCE_DIR}/src/support/
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/pattern/
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tree/xpath/
)

set(compiler_flags_to_use "")
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(compiler_flags_to_use "${compiler_flags_to_use} -pedantic -Wno-overloaded-virtual -Wno-dollar-in-identifier-extension -Wno-four-char-constants -fexceptions")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  set(compiler_flags_to_use "${compiler_flags_to_use} -pedantic -Wno-overloaded-virtual -Wno-multichar -fexceptions")
elseif(MSVC)
  set(compiler_flags_to_use "/EHsc /wd4138")
endif()

set_target_properties(LLVMAntlrRuntime PROPERTIES CMAKE_CXX_FLAGS                "")
set_target_properties(LLVMAntlrRuntime PROPERTIES CMAKE_CXX_FLAGS_DEBUG          "")
set_target_properties(LLVMAntlrRuntime PROPERTIES CMAKE_CXX_FLAGS_MINSIZEREL     "")
set_target_properties(LLVMAntlrRuntime PROPERTIES CMAKE_CXX_FLAGS_RELEASE        "")
set_target_properties(LLVMAntlrRuntime PROPERTIES CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
set_target_properties(LLVMAntlrRuntime PROPERTIES COMPILE_FLAGS                  "${compiler_flags_to_use} ")
set_target_properties(LLVMAntlrRuntime PROPERTIES COMPILE_DEFINITIONS            "")

foreach(fn ${libantlrcpp_SRC})
    set_property(SOURCE ${fn} PROPERTY CMAKE_CXX_FLAGS                "")
    set_property(SOURCE ${fn} PROPERTY CMAKE_CXX_FLAGS_DEBUG          "")
    set_property(SOURCE ${fn} PROPERTY CMAKE_CXX_FLAGS_MINSIZEREL     "")
    set_property(SOURCE ${fn} PROPERTY CMAKE_CXX_FLAGS_RELEASE        "")
    set_property(SOURCE ${fn} PROPERTY CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
    set_property(SOURCE ${fn} PROPERTY COMPILE_FLAGS                  "")
    set_property(SOURCE ${fn} PROPERTY COMPILE_DEFINITIONS            "")
endforeach()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_link_libraries(LLVMAntlrRuntime ${UUID_LIBRARIES})
elseif(APPLE)
  target_link_libraries(LLVMAntlrRuntime ${COREFOUNDATION_LIBRARY})
endif()
