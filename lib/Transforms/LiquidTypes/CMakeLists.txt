add_llvm_library(LLVMTransformLiquidTypes
  RefinementChecker.cpp
  FixpointTypeConvertor.cpp
  FixpointManager.cpp
  RefinementFunctionAnalysisPass.cpp
  RefinementFunctionSignatureAnalysisPass.cpp
  RefinementMetadata_Raw.cpp
  RefinementMetadata.cpp
  RefinementConstraintGenerator.cpp
  RefinementInstructionConstraintGenerator.cpp
  AnalysisRetriever.cpp
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Transforms/LiquidTypes

  DEPENDS
  intrinsics_gen
)
