
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#########################################

all-local: foo foo-tilek

check-local: check-foo check-foo-tilek

clean-local:
	rm -rf foo foo-tilek *.o

TILEK_INC=$(srcdir)/../include
TILEK_LIB=$(builddir)/../lib/tilek

#########################################

# Main

main-foo.o: $(srcdir)/main-foo.c
	gcc -g -O0 -c $(srcdir)/main-foo.c -o main-foo.o

# Original

foo.o: $(srcdir)/foo.c
	gcc -g -O0 -I$(TILEK_INC) -c $(srcdir)/foo.c -o foo.o

foo: foo.o main-foo.o
	gcc foo.o main-foo.o -o foo

check-foo: foo
	./foo

# TileK

foo-tilek.o: $(srcdir)/rose_foo.c
	gcc -g -O0 -I$(TILEK_INC) -c $(srcdir)/rose_foo.c -o foo-tilek.o

foo-data.o: $(srcdir)/foo-data.c
	gcc -g -O0 -I$(TILEK_INC) -c $(srcdir)/foo-data.c -o foo-data.o

foo-kernel.o: $(srcdir)/foo-kernel.c
	gcc -g -O0 -I$(TILEK_INC) -c $(srcdir)/foo-kernel.c -o foo-kernel.o

foo-tilek: foo-tilek.o foo-data.o foo-kernel.o $(TILEK_LIB)/libTileK.la main-foo.o
	libtool --mode=link gcc foo-tilek.o foo-data.o foo-kernel.o $(TILEK_LIB)/libTileK.la main-foo.o -o foo-tilek

check-foo-tilek: foo-tilek
	./foo-tilek

# Libraries

$(TILEK_LIB)/libTileK.la:
	make -C $(TILEK_LIB) libTileK.la

