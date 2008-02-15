# Makefile for building Taste package (C++ version)
# To use this file type "make -f unix.mk [entry-point]"
# or copy it to "Makefile" to avoid using the "-f unix.mk" option
# Entry Points:
# all           (Default) build Taste examples
# clean         Clean all object and executable files
# dos2unix      Fix source files that have any CR/LF -> LF

#CC sets the name of the compiler to use (cc, gcc, etc)
#You will have to change this to suit your system
CC             = g++

CFLAGS          = -g -I../cplus2
COCOR           = ../cocor
DOS2UNIX        = ../dos2unix.sh
LIB             = ../cplus2/cr_lib.a

%.o: %.cpp
		$(CC) -c $(CFLAGS) $< -o $@

%.o: %.cxx
		$(CC) -c $(CFLAGS) $< -o $@

all:            taste pretty xref

dos2unix:
		sh -c "$(DOS2UNIX) '*.c?? *.h?? *.h *.mk *.frm *.atg *.tas'"

fix_2_cpp:
		for i in crossref prettypr tc tl;\
		do (mv $$i.cxx $$i.cpp); done  

fix_2_cxx:
		for i in crossref prettypr tc tl;\
		do (mv $$i.cpp $$i.cxx); done  

taste:          tastep.o  taste.o tc.o tl.o
		$(CC) $(CFLAGS) -otaste taste.o tl.o tc.o\
                                        tastes.o tastep.o $(LIB)

tastep.o:       taste.atg
		$(COCOR) taste.atg
		$(CC) $(CFLAGS) -c taste.c?? tastep.c?? tastes.c??

pretty:         prettyp.o pretty.o prettypr.o 
		$(CC) $(CFLAGS) -opretty pretty.o prettypr.o \
                                         prettys.o prettyp.o $(LIB)

prettyp.o:      pretty.atg
		$(COCOR) pretty.atg
		$(CC) $(CFLAGS) -c pretty.c?? prettyp.c?? prettys.c??

slang:		slangp.o myslang.o prettypr.o
		$(CC) $(CFLAGS) -omyslang myslang.o prettypr.o \
		                  slangs.o slangp.o $(LIB)
slangp.o:	slang.atg
		$(COCOR) slang.atg
		$(CC) $(CFLAGS) -c myslang.c?? slangp.c?? slangs.c??

xref:           xrefp.o xref.o crossref.o 
		$(CC) $(CFLAGS) -oxref xref.o crossref.o \
                                       xrefs.o xrefp.o $(LIB)

xrefp.o:        xref.atg
		$(COCOR) xref.atg
		$(CC) $(CFLAGS) -c xref.c?? xrefp.c?? xrefs.c??

clean:
		set -i; \
                rm *.o
		rm pretty 
		rm xref 
		rm taste 
		rm pretty.c?? 
		rm xref.c?? 
		rm taste.c?? 
		rm taste?.c?? 
		rm taste?.hpp
		rm xref?.c??
		rm xref?.hpp
		rm pretty?.c??
		rm pretty?.hpp
		rm *.h
		rm *.lst



