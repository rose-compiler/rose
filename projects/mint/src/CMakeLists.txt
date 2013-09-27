
add_executable(mint
  main.C
  midend/MintCudaMidend.C
  midend/CudaOutliner.C
  midend/VariableHandling.C
  midend/LoweringToCuda.C
  midend/memoryManagement/CudaMemoryManagement.C
  midend/memoryManagement/DataTransferSizes.C
  midend/arrayProcessing/MintArrayInterface.C
  midend/mintTools/MintTools.C
  midend/mintTools/MintOptions.C
  midend/mintPragmas/MintPragmas.C
  optimizer/CudaOptimizer.C
  optimizer/KernelMergeInterface.C
  optimizer/OnChipMemoryOptimizer/OnChipMemoryOpt.C
  optimizer/OptimizerInterface/CudaOptimizerInterface.C
  optimizer/programAnalysis/StencilAnalysis.C
  optimizer/programAnalysis/MintConstantFolding.C
  optimizer/LoopUnroll/LoopUnrollOptimizer.C
  optimizer/OnChipMemoryOptimizer/GhostCellHandler.C
  optimizer/CudaOptimizer.h
  optimizer/KernelMergeInterface.h
  optimizer/LoopUnroll/LoopUnrollOptimizer.h
  optimizer/OptimizerInterface/CudaOptimizerInterface.h
  optimizer/OnChipMemoryOptimizer/GhostCellHandler.h
  optimizer/OnChipMemoryOptimizer/OnChipMemoryOpt.h
  optimizer/programAnalysis/MintConstantFolding.h
  optimizer/programAnalysis/StencilAnalysis.h
  midend/CudaOutliner.h
  midend/memoryManagement/CudaMemoryManagement.h
  midend/memoryManagement/DataTransferSizes.h
  midend/mintTools/MintTools.h
  midend/mintTools/MintOptions.h
  midend/LoweringToCuda.h
  midend/MintCudaMidend.h
  midend/rose_openmplowering/omp_lowering.h
  midend/mintPragmas/MintPragmas.h
  midend/arrayProcessing/MintArrayInterface.h
  types/MintTypes.h
)

target_link_libraries(mint ROSE_DLL ${Boost_LIBRARIES})

