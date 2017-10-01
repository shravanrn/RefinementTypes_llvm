add_llvm_library(LLVMTransformLiquidFixpointBuilder
  FixpointConstraintBuilder.cpp
  VariablesEnvironment.cpp
  FunctionBlockGraph.cpp
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Transforms/LiquidFixpointBuilder
)
