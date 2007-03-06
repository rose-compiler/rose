
CC = g++ -g -Wall
LD = gcc -g
YACC = bison
LEX = flex

SYNSCANNER = synscanner
SYNPARSER = synparser

GRATO = grato

CPPFLAGS = -Wall

LIBGRATO = lib$(GRATO).a

HEADERFILES = GrammarIr.h GBackEnd.h GTextBackEnd.h GTextTransformation.h GDotBackEnd.h GProperty.h GSequenceVisitor.h GCocoBackEnd.h

FRONTENDSOURCEFILES = $(SYNSCANNER).C $(SYNPARSER).C
IRSOURCEFILES = GrammarIr.C
BACKENDSOURCEFILES = GBackEnd.C GTextBackEnd.C GDotBackEnd.C GCocoBackEnd.C

LIBSOURCEFILES = $(FRONTENDSOURCEFILES) $(IRSOURCEFILES) $(BACKENDSOURCEFILES)
LIBOBJECTFILES := $(patsubst %.C,%.o,$(LIBSOURCEFILES))

SOURCEFILES = GTextTransformation.C GProperty.C GSequenceVisitor.C

TESTS = tests
EXAMPLES = examples

#objects := $(patsubst %.C,%.o,$(wildcard *.C))
# remove grato.o
#objects := $(patsubst grato.o, ,$(objects))

all: $(GRATO) $(SYNSCANNER).o $(SYNPARSER).o $(LIBGRATO)

$(SYNPARSER).H $(SYNPARSER).C: $(SYNPARSER).y parser_decls.h
	${YACC} -g -d --name-prefix=syn --file-prefix=syn -o $(SYNPARSER).C $(SYNPARSER).y

$(SYNSCANNER).C: $(SYNSCANNER).l $(SYNPARSER).H
	flex --prefix=syn -o$(SYNSCANNER).C $(SYNSCANNER).l 

#$(GRATO):	$(LIBSOURCEFILES) $(HEADERFILES) $(SOURCEFILES) $(GRATO).C
#	$(CC) $(SOURCEFILES) $(LIBSOURCEFILES) $(GRATO).C -lfl -o $(GRATO) 

$(GRATO):	$(LIBGRATO) $(GRATO).C $(HEADERFILES) $(SOURCEFILES)
	$(CC) $(SOURCEFILES) $(GRATO).C $(LIBGRATO) -lfl -o $(GRATO)

$(LIBGRATO): $(LIBOBJECTFILES)
	rm -f $(LIBGRATO)
	ar -crsv $(LIBGRATO) $(LIBOBJECTFILES)

check:	$(GRATO) $(TESTS)/WHILE.syn $(TESTS)/SAGE3.syn $(TESTS)/OCE.syn
	$(GRATO) -t $(TESTS)/WHILE.syn | $(GRATO) -t
	$(GRATO) -d $(TESTS)/WHILE.syn > $(TESTS)/WHILE.dot
	dot -Tps $(TESTS)/WHILE.dot -o $(TESTS)/WHILE.ps
	rm $(TESTS)/WHILE.dot; rm $(TESTS)/WHILE.ps
	$(GRATO) -t $(TESTS)/OCE.syn | $(GRATO) -t
	$(GRATO) -d $(TESTS)/OCE.syn > $(TESTS)/OCE.dot
	dot -Tps $(TESTS)/OCE.dot -o $(TESTS)/OCE.ps
	rm $(TESTS)/OCE.dot; rm $(TESTS)/OCE.ps
	$(GRATO) -t $(TESTS)/SAGE3.syn | $(GRATO) -t
	$(GRATO) -d $(TESTS)/SAGE3.syn > $(TESTS)/SAGE3.dot
	dot -Tps $(TESTS)/SAGE3.dot -o $(TESTS)/SAGE3.ps
	rm $(TESTS)/SAGE3.dot; rm $(TESTS)/SAGE3.ps

	$(GRATO) -p $(TESTS)/SAGE3.syn
	$(GRATO) -r1 $(TESTS)/SAGE3.syn | $(GRATO) -p
	$(GRATO) -r2 $(TESTS)/SAGE3.syn | $(GRATO) -p

$(EXAMPLES): $(GRATO) $(TESTS)/SAGE3.syn $(TESTS)/OCE.syn $(TESTS)/SETL2.syn
	mkdir -p examples
	$(GRATO) -r1 $(TESTS)/SAGE3.syn > $(EXAMPLES)/SAGE3_r1.syn
	$(GRATO) -r2 $(TESTS)/SAGE3.syn > $(EXAMPLES)/SAGE3_r2.syn
	$(GRATO) -r1 $(TESTS)/OCE.syn > $(EXAMPLES)/OCE_r1.syn
	$(GRATO) -r2 $(TESTS)/OCE.syn > $(EXAMPLES)/OCE_r2.syn
	$(GRATO) -r1 $(TESTS)/SETL2.syn > $(EXAMPLES)/SETL2_r1.syn
	$(GRATO) -r2 $(TESTS)/SETL2.syn > $(EXAMPLES)/SETL2_r2.syn
	$(GRATO) -p $(EXAMPLES)/SAGE3_r1.syn
	$(GRATO) -p $(EXAMPLES)/SAGE3_r2.syn
	$(GRATO) -p $(EXAMPLES)/OCE_r1.syn
	$(GRATO) -p $(EXAMPLES)/OCE_r2.syn
	$(GRATO) -p $(EXAMPLES)/SETL2_r1.syn
	$(GRATO) -p $(EXAMPLES)/SETL2_r2.syn

docs:	Doxyfile
	doxygen Doxyfile

clean:
	rm -f $(SYNSCANNER).[HhCc] $(SYNPARSER).[HhCc]
	rm -f $(SYNPARSER).vcg
	rm -f $(GRATO)
	rm -f *.o
	rm -f OCE.dot OCE.ps WHILE.dot WHILE.ps
	rm -rf docs
	rm -f $(LIBGRATO)

distclean: clean
	rm -f *~
	rm -rf docs
	rm -rf $(EXAMPLES)
