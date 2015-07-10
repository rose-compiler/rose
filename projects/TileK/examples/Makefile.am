
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#########################################

TILEK_INC=$(srcdir)/../include
TILEK_LIB=$(builddir)/../lib/tilek

all-local: example_1 example_1-tilek

check-local: check-example_1 check-example_1-tilek

clean-local:
	rm -rf example_1 example_1-tilek *.o

#########################################

# Main

main-example_1.o: $(srcdir)/main-example_1.c
	gcc -g -O0 -c $(srcdir)/main-example_1.c -o main-example_1.o

# Original

example_1.o: $(srcdir)/example_1.c
	gcc -g -O0 -I$(TILEK_INC) -c $(srcdir)/example_1.c -o example_1.o

example_1: example_1.o main-example_1.o
	gcc example_1.o main-example_1.o -o example_1

check-example_1: example_1
	./example_1

# TileK

example_1-tilek.o: $(srcdir)/rose_example_1.c
	gcc -g -O0 -I$(TILEK_INC) -c $(srcdir)/rose_example_1.c -o example_1-tilek.o

example_1-data.o: $(srcdir)/example_1-data.c
	gcc -g -O0 -I$(TILEK_INC) -c $(srcdir)/example_1-data.c -o example_1-data.o

example_1-kernel.o: $(srcdir)/example_1-kernel.c
	gcc -g -O0 -I$(TILEK_INC) -c $(srcdir)/example_1-kernel.c -o example_1-kernel.o

example_1-tilek: example_1-tilek.o example_1-data.o example_1-kernel.o $(TILEK_LIB)/libTileK.la main-example_1.o
	libtool --mode=link gcc main-example_1.o example_1-tilek.o example_1-data.o example_1-kernel.o $(TILEK_LIB)/libTileK.la -o example_1-tilek

check-example_1-tilek: example_1-tilek
	./example_1-tilek

# Libraries

$(TILEK_LIB)/libTileK.la:
	make -C $(TILEK_LIB) libTileK.la

