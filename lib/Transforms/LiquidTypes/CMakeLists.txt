add_llvm_library(LLVMTransformLiquidTypes
  RefinementChecker.cpp
  FixpointConstraintBuilder.cpp
  FixpointTypeConvertor.cpp
  RefinementUtils.cpp
  RefinementFunctionAnalysisPass.cpp
  RefinementMetadata_Raw.cpp
  RefinementMetadata.cpp
  RefinementConstraintGenerator.cpp
  RefinementInstructionConstraintGenerator.cpp
  VariablesEnvironment.cpp
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Transforms/LiquidTypes

  DEPENDS
  intrinsics_gen
)