
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TILEK_REL_PATH=../..

#########################################

TILEK_INC=$(top_srcdir)/src/backend/KLT/include

TILEK=$(builddir)/$(TILEK_REL_PATH)/src/tilek

TILEK_RTL=$(builddir)/$(TILEK_REL_PATH)/lib/libTileK-RTL.la

KLT_RTL=$(top_builddir)/src/backend/KLT/lib/rtl/libKLT-RTL.la

#########################################

ROSE_FLAGS=-DSKIP_ROSE_BUILTIN_DECLARATIONS -I$(TILEK_INC)
C_FLAGS=-O0 -g -I$(TILEK_INC)
LD_FLAGS=-lrt $(TILEK_RTL) $(KLT_RTL)

CHECK_TARGET=check-test_1 check-test_2 check-test_3 check-test_4 check-test_5
C_FLAGS+=-DTILEK_BASIC

check-local: $(CHECK_TARGET) 

clean-local:
	rm -f rose_*.c *-kernel.c *-data.c *.o

#########################################

$(builddir)/$(TILEK_REL_PATH)/tilek:
	make -C $(builddir)/$(TILEK_REL_PATH) tilek

#########################################

test_1-kernel.c: rose_test_1.c
test_1-data.c: rose_test_1.c
rose_test_1.c: $(srcdir)/test_1.c $(TILEK)
	$(TILEK) $(ROSE_FLAGS) -c $(srcdir)/test_1.c

rose_test_1.o: rose_test_1.c
	gcc $(C_FLAGS) -c rose_test_1.c -o rose_test_1.o

test_1-kernel.o: test_1-kernel.c
	gcc $(C_FLAGS) -c test_1-kernel.c -o test_1-kernel.o

test_1-data.o: test_1-data.c
	gcc $(C_FLAGS) -c test_1-data.c -o test_1-data.o

test_1: rose_test_1.o test_1-kernel.o test_1-data.o $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc rose_test_1.o test_1-kernel.o test_1-data.o $(LD_FLAGS) -o test_1

check-test_1: test_1
	./test_1

#########################################

test_2-kernel.c: rose_test_2.c
test_2-data.c: rose_test_2.c
rose_test_2.c: $(srcdir)/test_2.c $(TILEK)
	$(TILEK) $(ROSE_FLAGS) -c $(srcdir)/test_2.c

rose_test_2.o: rose_test_2.c
	gcc $(C_FLAGS) -c rose_test_2.c -o rose_test_2.o

test_2-kernel.o: test_2-kernel.c
	gcc $(C_FLAGS) -c test_2-kernel.c -o test_2-kernel.o

test_2-data.o: test_2-data.c
	gcc $(C_FLAGS) -c test_2-data.c -o test_2-data.o

test_2: rose_test_2.o test_2-kernel.o test_2-data.o $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc rose_test_2.o test_2-kernel.o test_2-data.o $(LD_FLAGS) -o test_2

check-test_2: test_2
	./test_2

#########################################

test_3-kernel.c: rose_test_3.c
test_3-data.c: rose_test_3.c
rose_test_3.c: $(srcdir)/test_3.c $(TILEK)
	$(TILEK) $(ROSE_FLAGS) -c $(srcdir)/test_3.c

rose_test_3.o: rose_test_3.c
	gcc $(C_FLAGS) -c rose_test_3.c -o rose_test_3.o

test_3-kernel.o: test_3-kernel.c
	gcc $(C_FLAGS) -c test_3-kernel.c -o test_3-kernel.o

test_3-data.o: test_3-data.c
	gcc $(C_FLAGS) -c test_3-data.c -o test_3-data.o

test_3: rose_test_3.o test_3-kernel.o test_3-data.o $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc rose_test_3.o test_3-kernel.o test_3-data.o $(LD_FLAGS) -o test_3

check-test_3: test_3
	./test_3

#########################################

test_4-kernel.c: rose_test_4.c
test_4-data.c: rose_test_4.c
rose_test_4.c: $(srcdir)/test_4.c $(TILEK)
	$(TILEK) $(ROSE_FLAGS) -c $(srcdir)/test_4.c

rose_test_4.o: rose_test_4.c
	gcc $(C_FLAGS) -c rose_test_4.c -o rose_test_4.o

test_4-kernel.o: test_4-kernel.c
	gcc $(C_FLAGS) -c test_4-kernel.c -o test_4-kernel.o

test_4-data.o: test_4-data.c
	gcc $(C_FLAGS) -c test_4-data.c -o test_4-data.o

test_4: rose_test_4.o test_4-kernel.o test_4-data.o $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc rose_test_4.o test_4-kernel.o test_4-data.o $(LD_FLAGS) -o test_4

check-test_4: test_4
	./test_4

#########################################

test_5-kernel.c: rose_test_5.c
test_5-data.c: rose_test_5.c
rose_test_5.c: $(srcdir)/test_5.c $(TILEK)
	$(TILEK) $(ROSE_FLAGS) -c $(srcdir)/test_5.c

rose_test_5.o: rose_test_5.c
	gcc $(C_FLAGS) -c rose_test_5.c -o rose_test_5.o

test_5-kernel.o: test_5-kernel.c
	gcc $(C_FLAGS) -c test_5-kernel.c -o test_5-kernel.o

test_5-data.o: test_5-data.c
	gcc $(C_FLAGS) -c test_5-data.c -o test_5-data.o

test_5: rose_test_5.o test_5-kernel.o test_5-data.o $(TILEK_RTL) $(KLT_RTL)
	libtool --mode=link gcc rose_test_5.o test_5-kernel.o test_5-data.o $(LD_FLAGS) -o test_5

check-test_5: test_5
	./test_5

#########################################

