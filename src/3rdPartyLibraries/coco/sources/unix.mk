# Makefile for building Coco/R from its sources
# To use this file type "make -f unix.mk [entry-point]"
# or copy it to "Makefile" to avoid using the "-f unix.mk" option
# Entry Points:
# all           (Default) build Coco/R
# new           Clean and build Coco/R
# clean         Clean all object and executable files
# dos2unix      Fix source files that have any CR/LF -> LF

# CFLAGS
#	-O	optimization
# add -DDebug if required
# CFLAGS          = -O -DDebug

CFLAGS          = -O

#CC sets the name of the compiler to use (cc, gcc, etc)
#You will have to change this to suit your system
#CC             = cc

DOS2UNIX        = ../dos2unix.sh

SRCS = \
	cr.c \
	cra.c \
	crp.c \
	crs.c \
	crt.c \
	crx.c \
	crf.c \
	collect.c \
	set.c

OBJS = \
	cr.o \
	cra.o \
	crp.o \
	crs.o \
	crt.o \
	crx.o \
	crf.o \
	collect.o\
	set.o

all:            cocor

dos2unix:
		sh -c "$(DOS2UNIX) '*.c *.h *.mk *.sh *.frm *.atg'"

new:            clean cocor

cocor:          $(OBJS)
		$(CC) -o cocor $(CFLAGS) $(OBJS)
		cp cocor ..

clean:
		set -i; \
                rm	 *.o
		rm cocor





