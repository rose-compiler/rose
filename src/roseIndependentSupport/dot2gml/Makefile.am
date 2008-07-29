include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# "if" for DOT_TO_GML_TRANSLATOR (AutoMake conditional block)
if DOT_TO_GML_TRANSLATOR

bin_PROGRAMS = dot2gml

dot2gml_SOURCES = \
     main.C

DEPENDENCIES = lex.o bison.o

# Header files that need to be included in the distribution
include_HEADERS = parse_header.h

LDADD = lex.o bison.o

main.o: lex.o bison.o

lex.o: bison.o lex.c
		$(CXX) $(CFLAGS) -I$(srcdir) -c lex.c -o lex.o

lex.c: $(srcdir)/parseDot.lex 
		flex $(srcdir)/parseDot.lex
		cp lex.yy.c lex.c

bison.o:	bison.c
		$(CXX) $(CFLAGS) -I$(srcdir) -c bison.c -o bison.o

y.tab.h: bison.o

bison.c:	$(srcdir)/parseDot.y
	bison -d -v $(srcdir)/parseDot.y
	cp parseDot.tab.c bison.c
	cp parseDot.tab.h y.tab.h

lex.o yac.o main.o: parse_header.h
lex.o main.o		: y.tab.h

# parseDot.tab.h  y.tab.h

clean-local:
	rm -f *.o *~ lex.c lex.yy.c bison.c y.tab.h parseDot.tab.c parseDot.tab.h parseDot.output parseDot *.gml

# DQ (9/10/2005): This should be moved to the tests directory
#check:
#	./dot2gml $(srcdir)/onefileonly.C.dot

endif

EXTRA_DIST = parse_header.h parseDot.lex parseDot.y README main.C 
