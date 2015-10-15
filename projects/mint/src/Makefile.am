
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

# ------------------------------------------------------------------------------
#  mint
# ------------------------------------------------------------------------------

mint_API_VERSION=1.0.0
exec_prefix=${prefix}/mint-$(mint_API_VERSION)

AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD    = $(ROSE_LIBS)

if !OS_MACOSX

bin_PROGRAMS = mint

mint_SOURCES=\
	main.C \
	midend/MintCudaMidend.C \
	midend/CudaOutliner.C \
	midend/VariableHandling.C \
	midend/LoweringToCuda.C \
	midend/memoryManagement/CudaMemoryManagement.C \
	midend/memoryManagement/DataTransferSizes.C \
	midend/arrayProcessing/MintArrayInterface.C \
	midend/mintTools/MintTools.C \
	midend/mintTools/MintOptions.C \
	midend/mintPragmas/MintPragmas.C \
	optimizer/CudaOptimizer.C \
	optimizer/KernelMergeInterface.C \
	optimizer/OnChipMemoryOptimizer/OnChipMemoryOpt.C \
	optimizer/OptimizerInterface/CudaOptimizerInterface.C \
	optimizer/programAnalysis/StencilAnalysis.C \
	optimizer/programAnalysis/MintConstantFolding.C \
	optimizer/LoopUnroll/LoopUnrollOptimizer.C \
	optimizer/OnChipMemoryOptimizer/GhostCellHandler.C \
        optimizer/CudaOptimizer.h \
        optimizer/KernelMergeInterface.h \
        optimizer/LoopUnroll/LoopUnrollOptimizer.h \
        optimizer/OptimizerInterface/CudaOptimizerInterface.h \
        optimizer/OnChipMemoryOptimizer/GhostCellHandler.h \
        optimizer/OnChipMemoryOptimizer/OnChipMemoryOpt.h \
        optimizer/programAnalysis/MintConstantFolding.h \
        optimizer/programAnalysis/StencilAnalysis.h \
        midend/CudaOutliner.h \
        midend/memoryManagement/CudaMemoryManagement.h \
        midend/memoryManagement/DataTransferSizes.h \
        midend/mintTools/MintTools.h \
        midend/mintTools/MintOptions.h \
        midend/LoweringToCuda.h \
        midend/MintCudaMidend.h \
        midend/rose_openmplowering/omp_lowering.h \
        midend/mintPragmas/MintPragmas.h \
        midend/arrayProcessing/MintArrayInterface.h \
        types/MintTypes.h

endif



















