#SYNOBJS = ../grato/synparser.o ../grato/synscanner.o ../grato/GrammarIr.o
SYNOBJS = ../grato/libgrato.a
OBJECTS = y.tab.o lex.pig.o Rule.o spec.o generate.o $(SYNOBJS)

all: objs
	g++ $(OBJECTS) -o pig -lfl

$(SYNOBJS):
	$(MAKE) -C ../grato

y.tab.o: spec.y
	yacc -d -ppig spec.y
	g++ -c y.tab.c

lex.pig.o: spec.l y.tab.o
	lex -Ppig spec.l
	g++ -c lex.pig.c

%.o: %.c++ %.h
	g++ -c $<

objs: $(OBJECTS)

clean:
	rm -f *~ *.o lex.pig.c y.tab.c y.tab.h pig

