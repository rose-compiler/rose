
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TILEK_REL_PATH=../../..

#########################################

TILEK_INC=$(top_srcdir)/src/backend/KLT/include

TILEK=$(builddir)/$(TILEK_REL_PATH)/src/tilek-acc-ocl

TILEK_RTL=$(builddir)/$(TILEK_REL_PATH)/lib/libTileK-RTL-acc-ocl.la

KLT_RTL=$(top_builddir)/src/backend/KLT/lib/rtl/libKLT-RTL.la

#########################################

ROSE_FLAGS=-DSKIP_ROSE_BUILTIN_DECLARATIONS -I$(TILEK_INC)
C_FLAGS=-O0 -g -I$(TILEK_INC)
LD_FLAGS=-lrt $(TILEK_RTL) $(KLT_RTL) 

CHECK_TARGET=
if ROSE_HAVE_OPENCL_INC
if ROSE_HAVE_OPENCL_LIB
CHECK_TARGET+=check-test_1 check-test_2
ROSE_FLAGS+=-I@OPENCL_INC_DIR@ -DTILEK_ACCELERATOR -DTILEK_TARGET_OPENCL
C_FLAGS+=-I@OPENCL_INC_DIR@ -DTILEK_ACCELERATOR -DTILEK_TARGET_OPENCL
LD_FLAGS+=-L@OPENCL_LIB_DIR@ -lOpenCL
endif
endif

check-local: $(CHECK_TARGET)

clean-local:
	rm -f rose_*.c *-kernel.c *-data.c *.o

#########################################

$(builddir)/$(TILEK_REL_PATH)/tilek-acc-ocl:
	make -C $(builddir)/$(TILEK_REL_PATH) tilek-acc-ocl

#########################################

test_1-kernel.c: rose_test_1.c
test_1-data.c: rose_test_1.c
rose_test_1.c: $(srcdir)/../test_1.c $(TILEK)
	$(TILEK) $(ROSE_FLAGS) -c $(srcdir)/../test_1.c

rose_test_1.o: rose_test_1.c
	gcc $(C_FLAGS) -c rose_test_1.c -o rose_test_1.o

test_1-data.o: test_1-data.c
	gcc $(C_FLAGS) -c test_1-data.c -o test_1-data.o

test_1: rose_test_1.o test_1-data.o $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc rose_test_1.o test_1-data.o $(LD_FLAGS) -o test_1

check-test_1: test_1
	./test_1

#########################################

test_2-kernel.c: rose_test_2.c
test_2-data.c: rose_test_2.c
rose_test_2.c: $(srcdir)/../test_2.c $(TILEK)
	$(TILEK) $(ROSE_FLAGS) -c $(srcdir)/../test_2.c

rose_test_2.o: rose_test_2.c
	gcc $(C_FLAGS) -c rose_test_2.c -o rose_test_2.o

test_2-data.o: test_2-data.c
	gcc $(C_FLAGS) -c test_2-data.c -o test_2-data.o

test_2: rose_test_2.o test_2-data.o $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc rose_test_2.o test_2-data.o $(LD_FLAGS) -o test_2

check-test_2: test_2
	./test_2

#########################################

