
###################################################################################################

check: check-test_1 check-test_2

clean: clean-test_1 clean-test_2

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

test_%.o: $(srcdir)/test_%.c
	gcc $(C_FLAGS) -c $< -o $@

kernel_%.o: $(srcdir)/kernel_%.c
	gcc $(C_FLAGS) -c $< -o $@

static_%.o: $(srcdir)/static_%.c
	gcc $(C_FLAGS) -c $< -o $@

###################################################################################################

test_1: test_1.o kernel_1.o static_1.o $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc test_1.o kernel_1.o static_1.o $(LD_FLAGS) -o test_1

check-test_1: test_1
	./test_1

clean-test_1:
	rm -f test_1.o kernel_1.o static_1.o test_1

###################################################################################################

test_2: test_2.o kernel_2.o static_2.o $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc test_2.o kernel_2.o static_2.o $(LD_FLAGS) -o test_2

check-test_2: test_2
	./test_2

clean-test_2:
	rm -f test_2.o kernel_2.o static_2.o test_2

###################################################################################################

