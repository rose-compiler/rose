SRC = correlation covariance
SUBDIRS = stencils/jacobi-1d-imper \
	stencils/jacobi-2d-imper \
	stencils/fdtd-2d \
	stencils/seidel-2d \
	stencils/fdtd-apml \
	stencils/adi \
	datamining/correlation \
	datamining/covariance \
	linear-algebra/kernels/2mm \
	linear-algebra/kernels/3mm \
	linear-algebra/kernels/atax \
	linear-algebra/kernels/bicg \
	linear-algebra/kernels/cholesky \
	linear-algebra/kernels/doitgen \
	linear-algebra/kernels/gemm \
	linear-algebra/kernels/gemver \
	linear-algebra/kernels/gesummv \
	linear-algebra/kernels/mvt \
	linear-algebra/kernels/symm \
	linear-algebra/kernels/syr2k \
	linear-algebra/kernels/syrk \
	linear-algebra/kernels/trisolv \
	linear-algebra/kernels/trmm \
	linear-algebra/solvers/durbin \
	linear-algebra/solvers/dynprog \
	linear-algebra/solvers/gramschmidt \
	linear-algebra/solvers/lu \
	linear-algebra/solvers/ludcmp 

export ARRAYSIZE := 16
export STEPSIZE := 2
export CC := /home/lin32/Development/projects/polyopt-c-0.2.1/src/PolyOpt
#export CFLAGS := -O3 --polyopt-verbose --polyopt-scop-extractor-verbose=3 --polyopt-pluto-fuse-maxfuse --polyopt-fixed-tiling
export CMAXFUSEFLAGS := -O3 --polyopt-pluto-fuse-maxfuse --polyopt-quiet -DARRAYSIZE=$(ARRAYSIZE) -DSTEPSIZE=$(STEPSIZE)
export CSMARTFUSEFLAGS := -O3 --polyopt-pluto-fuse-smartfuse --polyopt-quiet -DARRAYSIZE=$(ARRAYSIZE) -DSTEPSIZE=$(STEPSIZE) 
export CTILEFLAGS := -O3 --polyopt-fixed-tiling --polyopt-quiet -DARRAYSIZE=$(ARRAYSIZE) -DSTEPSIZE=$(STEPSIZE)
export INCLUDES := -I.
export 1STTILE := echo "13 1 1" > tile.sizes 
export 2NDTILE := echo "11 1 1" > tile.sizes 
export NESTTILE := echo "13 11 1" > tile.sizes
.PHONY: all clean cleanROSE nest_tile fuse_tile

all clean nest_tile fuse_tile cleanROSE: $(SUBDIRS)
	for dir in $(SUBDIRS); do \
	$(MAKE) -C $$dir $@; \
	done
