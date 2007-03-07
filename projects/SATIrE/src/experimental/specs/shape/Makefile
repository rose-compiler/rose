ANALYSIS = shape

# PAGDIR must be set by user (directory where PAG is installed)

# this is the only variable that must be set (all other vars are set with this
# variable as rootdir)
SATIREMAINDIR = ../../..


SATIRELIBS = $(SATIREMAINDIR)/analyzer/
SATIREINCLUDES = -I $(SATIREMAINDIR)/analyzer/commandlineinterface -I $(SATIREMAINDIR)/analyzer/astaccess -I $(SATIREMAINDIR)/analyzer/icfgbuilder -I $(SATIREMAINDIR)/analyzer/annotation
COMMONDIR = $(SATIREMAINDIR)/analyzer/astaccess

PAG = $(PAGDIR)/bin/pag
FLAGS = -Wall -I $(ROSE_INCLUDE) -I $(PAGDIR)/include/pag -I $(PAGDIR)/include/pag/analyzer -I $(ANALYSIS)-out/src/ $(SATIREINCLUDES)
LPATH = -L $(ROSE_LIB) -L $(PAGDIR)/lib/pag -L $(SATIRELIBS)
LIBS = $(ANALYSIS).a -lsatire -lrose -ledg -lpag -lrt

TESTDIR = tests

all: $(ANALYSIS)

$(ANALYSIS).a: $(ANALYSIS).set $(ANALYSIS).optla
	mkdir -p $(ANALYSIS)-out
	if $(PAG) $(COMMONDIR)/ iterate1.t $(ANALYSIS) $(ANALYSIS)-out --prefix=$(PAGDIR) 2> pag-out; then cat pag-out; else cat pag-out; false; fi
	cp $(COMMONDIR)/syntree.o $(ANALYSIS)-out/src/
	$(MAKE) CFLAGS="-DANIM -DDFI_WRITE" -C $(ANALYSIS)-out/src/
	cp $(ANALYSIS)-out/src/$(ANALYSIS).a .

main.o: main.C
	g++ $(FLAGS) -I $(ANALYSIS)-out/src -DDFI_WRITE -DANALYSIS=$(ANALYSIS) -I $(SATIREINCLUDES) -DPAG -c main.C

$(ANALYSIS): $(ANALYSIS).a main.o
	g++ $(FLAGS) main.o -o $(ANALYSIS) $(LPATH) $(LIBS)
	@echo
	@echo 'PAG said:'
	@cat pag-out

.PHONY: clean
clean:
	rm -f *~ *.o *.a pig_temp* syntree* $(ANALYSIS) *.gdl pag-out
	rm -rf $(ANALYSIS)-out/
	rm -rf shapeanim

check:  $(ANALYSIS)
	mkdir -p shapeanim
	$(ANALYSIS) $(TESTDIR)/reverse1.C
	$(ANALYSIS) $(TESTDIR)/reverse2.C
	$(ANALYSIS) $(TESTDIR)/artificialListCreation.C
	$(ANALYSIS) $(TESTDIR)/artificialListCreationCyclic.C
	$(ANALYSIS) $(TESTDIR)/test1.C
	$(ANALYSIS) $(TESTDIR)/test2.C
	$(ANALYSIS) $(TESTDIR)/test3.C
	$(ANALYSIS) $(TESTDIR)/test4.C
	$(ANALYSIS) $(TESTDIR)/test5.C
	$(ANALYSIS) $(TESTDIR)/test6.C
	$(ANALYSIS) $(TESTDIR)/test7.C
	$(ANALYSIS) $(TESTDIR)/test8.C
	$(ANALYSIS) $(TESTDIR)/listTraversal1.C
	$(ANALYSIS) $(TESTDIR)/srw98reverse.C
	$(ANALYSIS) $(TESTDIR)/srw98insert.C
	$(ANALYSIS) $(TESTDIR)/OOsrw98insert1.C
	$(ANALYSIS) $(TESTDIR)/OOtest1a.C
	$(ANALYSIS) $(TESTDIR)/OOtest1b.C
	$(ANALYSIS) $(TESTDIR)/OOtest2.C
	$(ANALYSIS) $(TESTDIR)/OOtest3.C
	$(ANALYSIS) $(TESTDIR)/test9.C
	$(ANALYSIS) -I$(TESTDIR)/wcettests $(TESTDIR)/wcettests/bubble.c

fastcheck: $(ANALYSIS)
	mkdir -p shapeanim
	$(ANALYSIS) $(TESTDIR)/stack1.C
