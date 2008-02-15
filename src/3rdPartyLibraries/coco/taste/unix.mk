# Makefile for building Taste package (C version)
# To use this file type "make -f unix.mk [entry-point]"
# or copy it to "Makefile" to avoid using the "-f unix.mk" option
# Entry Points:
# all           (Default) build Taste examples
# clean         Clean all object and executable files
# dos2unix      Fix source files that have any CR/LF -> LF

#CFLAGS for debugging  (gdb)
#CFLAGS         = -ggdb
#CFLAGS for production
CFLAGS          = -O

#CC sets the name of the compiler to use (cc, gcc, etc)
#You will have to change this to suit your system
#CC             = cc

COCOR           = ../cocor
DOS2UNIX        = ../dos2unix.sh

all:            taste pretty xref

dos2unix:
		sh -c "$(DOS2UNIX) '*.c *.h *.mk *.frm *.atg *.tas'"

taste:          tastep.o taste.o tc.o tl.o
		$(CC) $(CFLAGS) -otaste taste.o tl.o tc.o\
                                        tastes.o tastep.o

tastep.o:       taste.atg
		$(COCOR) taste.atg
		$(CC) $(CFLAGS) -c taste.c tastep.c tastes.c

pretty:         prettyp.o pretty.o prettypr.o
		$(CC) $(CFLAGS) -opretty pretty.o prettypr.o \
                                         prettys.o prettyp.o

prettyp.o:      pretty.atg
		$(COCOR) pretty.atg
		$(CC) $(CFLAGS) -c pretty.c prettyp.c prettys.c

slang:          slangp.o slang.o prettypr.o
		$(CC) $(CFLAGS) -oslang slang.o prettypr.o \
                                         slangs.o slangp.o

slangp.o:       slang.atg
		$(COCOR) slang.atg
		$(CC) $(CFLAGS) -c slang.c slangp.c slangs.c

xref:           xrefp.o xref.o crossref.o
		$(CC) $(CFLAGS) -oxref xref.o crossref.o \
                                       xrefs.o xrefp.o

xrefp.o:        xref.atg
		$(COCOR) xref.atg
		$(CC) $(CFLAGS) -c xref.c xrefp.c xrefs.c

clean:
		set -i; \
                rm *.o 
		rm pretty 
		rm xref 
		rm taste
		rm taste?.c 
		rm taste?.h
		rm xref?.c
		rm xref?.h
		rm pretty?.c
		rm pretty?.h
		rm *.lst




