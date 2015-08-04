
###################################################################################################

check: check-test_1

clean: clean-test_1

###################################################################################################

KLT_REL_PATH=src/backend/KLT
KLT_INC=$(top_srcdir)/$(KLT_REL_PATH)/include
KLT_RTL=$(top_builddir)/$(KLT_REL_PATH)/lib/rtl/libKLT-RTL.la

TILEK_REL_PATH=../..
TILEK_INC=$(srcdir)/$(TILEK_REL_PATH)/include
TILEK_RTL=$(builddir)/$(TILEK_REL_PATH)/lib/libTileK-RTL.la

###################################################################################################

C_FLAGS=-O0 -g -I$(KLT_INC) -I$(TILEK_INC) -DTILEK_BASIC
LD_FLAGS=-lrt $(TILEK_RTL) $(KLT_RTL)

###################################################################################################

test_1.o: $(srcdir)/test_1.c
	gcc $(C_FLAGS) -c $(srcdir)/test_1.c -o test_1.o

kernel_1.o: $(srcdir)/kernel_1.c
	gcc $(C_FLAGS) -c $(srcdir)/kernel_1.c -o kernel_1.o

static_1.o: $(srcdir)/static_1.c
	gcc $(C_FLAGS) -c $(srcdir)/static_1.c -o static_1.o

test_1: test_1.o kernel_1.o static_1.o $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc test_1.o kernel_1.o static_1.o $(LD_FLAGS) -o test_1

check-test_1: test_1
	./test_1

clean-test_1:
	rm -f test_1.o kernel_1.o static_1.o test_1

###################################################################################################

