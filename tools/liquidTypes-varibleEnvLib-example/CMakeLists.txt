set(LLVM_LINK_COMPONENTS
  Analysis
  BitWriter
  CodeGen
  Core
  IPO
  IRReader
  InstCombine
  Instrumentation
  Linker
  ObjCARCOpts
  ScalarOpts
  Support
  Target
  TransformUtils
  Vectorize
  )

# Support plugins.
set(LLVM_NO_DEAD_STRIP 1)

add_llvm_tool(liquidTypes-varibleEnvLib-example
  VariableEnvLib.cpp
  VariableEnvLib_IfExample.cpp
  VariableEnvLib_IfTwoExample.cpp
  VariableEnvLib_LoopExample.cpp
  )
export_executable_symbols(liquidTypes-varibleEnvLib-example)
