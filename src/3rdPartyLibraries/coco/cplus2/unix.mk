# Makefile for building Coco/R C++ support library from its sources
# To use this file type "make -f unix.mk [entry-point]"
# or copy it to "Makefile" to avoid using the "-f unix.mk" option
# Entry Points:
# all           (Default) build Coco/R support library
# clean         Clean all object and executable files
# dos2unix      Fix source files that have any CR/LF -> LF

# CFLAGS
#       -O      optimization

#CC sets the name of the compiler to use (cc, gcc, etc)
#You will have to change this to suit your system
#CC             = g++

AR              = ar
CFLAGS          = -g
DOS2UNIX        = ../dos2unix.sh

%.o: %.cpp
		$(CC) -c $(CFLAGS) $< -o $@

%.o: %.cxx
		$(CC) -c $(CFLAGS) $< -o $@

all:            cr_lib.a

dos2unix:
		sh -c "$(DOS2UNIX) '*.c?? *.h?? *.mk'"

cr_lib.a:
		$(CC) $(CFLAGS) -c cr_abs.c?? cr_error.c?? \
                                   cr_scan.c?? cr_parse.c??
		$(AR) -rc cr_lib.a cr_abs.o cr_error.o \
                                   cr_scan.o cr_parse.o

fix_2_cpp:
		for i in cr_abs cr_error cr_parse cr_scan;\
		do (mv $$i.cxx $$i.cpp); done  

fix_2_cxx:
		for i in cr_abs cr_error cr_parse cr_scan;\
		do (mv $$i.cpp $$i.cxx); done  

clean:
		\rm -f *.o
		\rm -f *.a




