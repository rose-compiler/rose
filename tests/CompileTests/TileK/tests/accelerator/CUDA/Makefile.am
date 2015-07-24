
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TILEK_REL_PATH=../../..

#########################################

TILEK_INC=$(top_srcdir)/src/backend/KLT/include

TILEK=$(builddir)/$(TILEK_REL_PATH)/src/tilek-acc-cuda

TILEK_RTL=$(builddir)/$(TILEK_REL_PATH)/lib/libTileK-RTL-acc-cuda.la

KLT_RTL=$(top_builddir)/src/backend/KLT/lib/rtl/libKLT-RTL.la

#########################################

ROSE_FLAGS=-DSKIP_ROSE_BUILTIN_DECLARATIONS -I$(TILEK_INC) -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA
C_FLAGS=-O0 -g -I$(TILEK_INC) -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA
LD_FLAGS=-lrt $(TILEK_RTL) $(KLT_RTL) 

CHECK_TARGET=
if ROSE_HAVE_CUDA_INC
if ROSE_HAVE_CUDA_LIB
CHECK_TARGET+=check-test_1 check-test_2
ROSE_FLAGS+=-I@CUDA_INC_DIR@
C_FLAGS+=-I@CUDA_INC_DIR@
LD_FLAGS+=-L@CUDA_LIB_DIR@
endif
endif

check-local: $(CHECK_TARGET)

clean-local:
	rm -f rose_*.c *-kernel.cu *-data.c *.o
	rm -f test_1 test_2

#########################################

$(builddir)/$(TILEK_REL_PATH)/tilek-acc-cuda:
	make -C $(builddir)/$(TILEK_REL_PATH) tilek-acc-cuda

#########################################

test_1-kernel.cu: rose_test_1.c
test_1-data.c: rose_test_1.c
rose_test_1.c: $(srcdir)/../test_1.c $(TILEK)
	$(TILEK) $(ROSE_FLAGS) -c $(srcdir)/../test_1.c

rose_test_1.o: rose_test_1.c
	gcc $(C_FLAGS) -c rose_test_1.c -o rose_test_1.o

test_1-kernel.ptx: test_1-kernel.cu
#	gcc $(C_FLAGS) -c test_1-kernel.c -o test_1-kernel.o

test_1-data.o: test_1-data.c
	gcc $(C_FLAGS) -c test_1-data.c -o test_1-data.o

test_1: rose_test_1.o test_1-data.o $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc rose_test_1.o test_1-data.o $(TILEK_RTL) $(KLT_RTL) -o test_1

check-test_1: test_1
	./test_1

#########################################
