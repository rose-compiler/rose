
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TILEK_REL_PATH=../../..

#########################################

TILEK_INC=$(top_srcdir)/src/backend/KLT/include

TILEK=$(builddir)/$(TILEK_REL_PATH)/src/tilek-acc-cuda

TILEK_RTL=$(builddir)/$(TILEK_REL_PATH)/lib/libTileK-RTL-acc-cuda.la

KLT_RTL=$(top_builddir)/src/backend/KLT/lib/rtl/libKLT-RTL.la

check-local: check-test_1

clean-local:
	rm -f rose_*.c *-kernel.c *-data.c *.o

#########################################

$(builddir)/$(TILEK_REL_PATH)/tilek-acc-cuda:
	make -C $(builddir)/$(TILEK_REL_PATH) tilek-acc-cuda

#########################################

test_1-kernel.c: rose_test_1.c
test_1-data.c: rose_test_1.c
rose_test_1.c: $(srcdir)/../test_1.c $(TILEK)
	$(TILEK) -DSKIP_ROSE_BUILTIN_DECLARATIONS -I$(TILEK_INC) -c $(srcdir)/../test_1.c

rose_test_1.o: rose_test_1.c
	gcc -O0 -g -I$(TILEK_INC) -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA -c rose_test_1.c -o rose_test_1.o

test_1-kernel.o: test_1-kernel.c
	gcc -O0 -g -I$(TILEK_INC) -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA -c test_1-kernel.c -o test_1-kernel.o

test_1-data.o: test_1-data.c
	gcc -O0 -g -I$(TILEK_INC) -DTILEK_ACCELERATOR -DTILEK_TARGET_CUDA -c test_1-data.c -o test_1-data.o

test_1: rose_test_1.o test_1-kernel.o test_1-data.o $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc rose_test_1.o test_1-kernel.o test_1-data.o $(TILEK_RTL) $(KLT_RTL) -o test_1

check-test_1: test_1
	./test_1

#########################################
