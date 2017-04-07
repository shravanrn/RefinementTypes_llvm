add_llvm_library(LLVMTransformLiquidTypes
  RefinementChecker.cpp
  FixpointConstraintBuilder.cpp
  RefinementUtils.cpp
  RefinementFunctionAnalysisPass.cpp
  RefinementMetadata.cpp
  RefinementMetadataParser.cpp
  RefinementConstraintGenerator.cpp
  VariablesEnvironment.cpp
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Transforms/LiquidTypes

  DEPENDS
  intrinsics_gen
)