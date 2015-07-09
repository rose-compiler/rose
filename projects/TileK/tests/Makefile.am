
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#########################################

TILEK_INC=$(srcdir)/../include
TILEK_LIB=$(builddir)/../lib/tilek
TILEK_BIN=$(builddir)/../src

check-local: check-test_1 check-test_2 check-test_3

clean-local:
	rm -f rose_*.c *-kernel.c *-data.c *.o

#########################################

test_1-kernel.c: rose_test_1.c
test_1-data.c: rose_test_1.c
rose_test_1.c: $(srcdir)/test_1.c $(TILEK_BIN)/tilek
	$(TILEK_BIN)/tilek -DSKIP_ROSE_BUILTIN_DECLARATIONS -I$(srcdir)/../include -c $(srcdir)/test_1.c

rose_test_1.o: rose_test_1.c
	gcc -O0 -g -I$(TILEK_INC) -c rose_test_1.c -o rose_test_1.o

test_1-kernel.o: test_1-kernel.c
	gcc -O0 -g -I$(TILEK_INC) -c test_1-kernel.c -o test_1-kernel.o

test_1-data.o: test_1-data.c
	gcc -O0 -g -I$(TILEK_INC) -c test_1-data.c -o test_1-data.o

test_1: rose_test_1.o test_1-kernel.o test_1-data.o $(TILEK_LIB)/libTileK.la
	libtool --mode=link gcc rose_test_1.o test_1-kernel.o test_1-data.o $(TILEK_LIB)/libTileK.la -o test_1

check-test_1: test_1
	./test_1

#########################################

test_2-kernel.c: rose_test_2.c
test_2-data.c: rose_test_2.c
rose_test_2.c: $(srcdir)/test_2.c $(TILEK_BIN)/tilek
	$(TILEK_BIN)/tilek -DSKIP_ROSE_BUILTIN_DECLARATIONS -I$(srcdir)/../include -c $(srcdir)/test_2.c

rose_test_2.o: rose_test_2.c
	gcc -O0 -g -I$(TILEK_INC) -c rose_test_2.c -o rose_test_2.o

test_2-kernel.o: test_2-kernel.c
	gcc -O0 -g -I$(TILEK_INC) -c test_2-kernel.c -o test_2-kernel.o

test_2-data.o: test_2-data.c
	gcc -O0 -g -I$(TILEK_INC) -c test_2-data.c -o test_2-data.o

test_2: rose_test_2.o test_2-kernel.o test_2-data.o $(TILEK_LIB)/libTileK.la
	libtool --mode=link gcc rose_test_2.o test_2-kernel.o test_2-data.o $(TILEK_LIB)/libTileK.la -o test_2

check-test_2: test_2
	./test_2

#########################################

test_3-kernel.c: rose_test_3.c
test_3-data.c: rose_test_3.c
rose_test_3.c: $(srcdir)/test_3.c $(TILEK_BIN)/tilek
	$(TILEK_BIN)/tilek -DSKIP_ROSE_BUILTIN_DECLARATIONS -I$(srcdir)/../include -c $(srcdir)/test_3.c

rose_test_3.o: rose_test_3.c
	gcc -O0 -g -I$(TILEK_INC) -c rose_test_3.c -o rose_test_3.o

test_3-kernel.o: test_3-kernel.c
	gcc -O0 -g -I$(TILEK_INC) -c test_3-kernel.c -o test_3-kernel.o

test_3-data.o: test_3-data.c
	gcc -O0 -g -I$(TILEK_INC) -c test_3-data.c -o test_3-data.o

test_3: rose_test_3.o test_3-kernel.o test_3-data.o $(TILEK_LIB)/libTileK.la
	libtool --mode=link gcc rose_test_3.o test_3-kernel.o test_3-data.o $(TILEK_LIB)/libTileK.la -o test_3

check-test_3: test_3
	./test_3

#########################################

# Compiler

$(TILEK_BIN)/tilek:
	make -C $(TILEK_BIN) tilek

# Libraries

$(TILEK_LIB)/libTileK.la:
	make -C $(TILEK_LIB) libTileK.la

#########################################

