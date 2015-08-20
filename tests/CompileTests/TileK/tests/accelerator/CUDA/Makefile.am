
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TILEK_REL_PATH=../../..

#########################################

TILEK_INC=$(top_srcdir)/src/midend/KLT/include

TILEK=$(builddir)/$(TILEK_REL_PATH)/src/tilek-cuda

TILEK_RTL=$(builddir)/$(TILEK_REL_PATH)/lib/libTileK-RTL-cuda.la

KLT_RTL=$(top_builddir)/src/midend/KLT/lib/rtl/libKLT-RTL.la

#########################################

ROSE_FLAGS=-DSKIP_ROSE_BUILTIN_DECLARATIONS -I$(TILEK_INC) -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA
C_FLAGS=-O0 -g -I$(TILEK_INC) -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA
NVCC_FLAGS=-I$(TILEK_INC) -DCOMPILE_FOR_KERNEL=1 -DSTORAGE_MODIFIER="" -DDEVICE_FUNCTION_MODIFIER=__device__
LD_FLAGS=-lrt $(TILEK_RTL) $(KLT_RTL) 

klt_context=$(top_srcdir)/src/midend/KLT/lib/rtl/context.c

CHECK_TARGET=
if ROSE_HAVE_CUDA_INC
if ROSE_HAVE_CUDA_LIB
if ROSE_HAVE_CUDA_BIN
CHECK_TARGET+=check-test_1 check-test_2

C_FLAGS+=-I@CUDA_INC_DIR@
LD_FLAGS+=-L@CUDA_LIB_DIR@ -lcuda -lcudart

NVCC=@CUDA_BIN_DIR@/nvcc
#NVCC_FLAGS+=--compiler-bindir=@CUDA_BIN_DIR@/gcc-4.4
else
CHECK_TARGET+=rose_test_1.c rose_test_2.c
endif
else
CHECK_TARGET+=rose_test_1.c rose_test_2.c
endif
else
CHECK_TARGET+=rose_test_1.c rose_test_2.c
endif

check-local: $(CHECK_TARGET)

clean-local:
	rm -f rose_*.c *-kernel.cu *-data.c *.o
	rm -f test_1 test_2

#########################################

$(builddir)/$(TILEK_REL_PATH)/tilek-cuda:
	make -C $(builddir)/$(TILEK_REL_PATH) tilek-cuda

#########################################

test_1-kernel.cu: rose_test_1.c
test_1-static.c: rose_test_1.c
rose_test_1.c: $(srcdir)/../test_1.c $(TILEK)
	$(TILEK) $(ROSE_FLAGS) -c $(srcdir)/../test_1.c

rose_test_1.o: rose_test_1.c
	gcc $(C_FLAGS) -c rose_test_1.c -o rose_test_1.o

test_1-kernel.clean.cu: test_1-kernel.cu
	cat test_1-kernel.cu $(klt_context) | grep -v "preinclude-cuda.h" > test_1-kernel.clean.cu

test_1-kernel.cu.ptx: test_1-kernel.clean.cu
	$(NVCC) $(NVCC_FLAGS) -ptx test_1-kernel.clean.cu -o test_1-kernel.cu.ptx

test_1-static.o: test_1-static.c
	gcc $(C_FLAGS) -c test_1-static.c -o test_1-static.o

test_1: rose_test_1.o test_1-static.o test_1-kernel.cu.ptx $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc $(LD_FLAGS) rose_test_1.o test_1-static.o -o test_1

check-test_1: test_1
	./test_1

#########################################

test_2-kernel.cu: rose_test_2.c
test_2-static.c: rose_test_2.c
rose_test_2.c: $(srcdir)/../test_2.c $(TILEK)
	$(TILEK) $(ROSE_FLAGS) -c $(srcdir)/../test_2.c

rose_test_2.o: rose_test_2.c
	gcc $(C_FLAGS) -c rose_test_2.c -o rose_test_2.o

test_2-kernel.clean.cu: test_2-kernel.cu
	cat test_2-kernel.cu $(klt_context) | grep -v "preinclude-cuda.h" > test_2-kernel.clean.cu

test_2-kernel.cu.ptx: test_2-kernel.cu
	$(NVCC) $(NVCC_FLAGS) -ptx test_2-kernel.cu -o test_2-kernel.cu.ptx

test_2-static.o: test_2-static.c
	gcc $(C_FLAGS) -c test_2-static.c -o test_2-static.o

test_2: rose_test_2.o test_2-static.o test_2-kernel.cu.ptx $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc rose_test_2.o test_2-static.o $(TILEK_RTL) $(KLT_RTL) -o test_2

check-test_2: test_2
	./test_2

#########################################
