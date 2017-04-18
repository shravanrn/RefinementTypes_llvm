include_directories(
  ${CMAKE_SOURCE_DIR}/lib/Transforms/antlr-cpp-rt/src
  ${CMAKE_SOURCE_DIR}/lib/Transforms/antlr-cpp-rt/src/atn
  ${CMAKE_SOURCE_DIR}/lib/Transforms/antlr-cpp-rt/src/dfa
  ${CMAKE_SOURCE_DIR}/lib/Transforms/antlr-cpp-rt/src/misc
  ${CMAKE_SOURCE_DIR}/lib/Transforms/antlr-cpp-rt/src/support
  ${CMAKE_SOURCE_DIR}/lib/Transforms/antlr-cpp-rt/src/tree
  ${CMAKE_SOURCE_DIR}/lib/Transforms/antlr-cpp-rt/src/tree/pattern
  ${CMAKE_SOURCE_DIR}/lib/Transforms/antlr-cpp-rt/src/tree/xpath
  ${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB librefinecpp_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/RefinementGrammarBaseListener.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/RefinementGrammarBaseVisitor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/RefinementGrammarLexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/RefinementGrammarListener.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/RefinementGrammarParser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/RefinementGrammarVisitor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/RefinementParser.cpp
)

add_llvm_library(LLVMTransformLiquidTypesParser 

  RefinementGrammarBaseListener.cpp
  RefinementGrammarBaseVisitor.cpp
  RefinementGrammarLexer.cpp
  RefinementGrammarListener.cpp
  RefinementGrammarParser.cpp
  RefinementGrammarVisitor.cpp
  RefinementParser.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(compiler_flags_to_use "")
set(linker_flags_to_use "")
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(compiler_flags_to_use "${compiler_flags_to_use} -pedantic -Wno-overloaded-virtual -Wno-dollar-in-identifier-extension -Wno-four-char-constants -fexceptions")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  set(compiler_flags_to_use "${compiler_flags_to_use} -pedantic -Wno-overloaded-virtual -Wno-multichar -fexceptions")
elseif(MSVC)
  set(compiler_flags_to_use "/EHsc")
endif()

set_target_properties(LLVMTransformLiquidTypesParser PROPERTIES CMAKE_CXX_FLAGS                "")
set_target_properties(LLVMTransformLiquidTypesParser PROPERTIES CMAKE_CXX_FLAGS_DEBUG          "")
set_target_properties(LLVMTransformLiquidTypesParser PROPERTIES CMAKE_CXX_FLAGS_MINSIZEREL     "")
set_target_properties(LLVMTransformLiquidTypesParser PROPERTIES CMAKE_CXX_FLAGS_RELEASE        "")
set_target_properties(LLVMTransformLiquidTypesParser PROPERTIES CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
set_target_properties(LLVMTransformLiquidTypesParser PROPERTIES COMPILE_FLAGS                  "${compiler_flags_to_use} ")
set_target_properties(LLVMTransformLiquidTypesParser PROPERTIES COMPILE_DEFINITIONS            "")

foreach(fn ${librefinecpp_SRC})
    set_property(SOURCE ${fn} PROPERTY CMAKE_CXX_FLAGS                "")
    set_property(SOURCE ${fn} PROPERTY CMAKE_CXX_FLAGS_DEBUG          "")
    set_property(SOURCE ${fn} PROPERTY CMAKE_CXX_FLAGS_MINSIZEREL     "")
    set_property(SOURCE ${fn} PROPERTY CMAKE_CXX_FLAGS_RELEASE        "")
    set_property(SOURCE ${fn} PROPERTY CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
    set_property(SOURCE ${fn} PROPERTY COMPILE_FLAGS                  "")
    set_property(SOURCE ${fn} PROPERTY COMPILE_DEFINITIONS            "")
endforeach()
