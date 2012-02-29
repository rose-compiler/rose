SATIREMAINDIR = ../../../../src
SATIRE_LIB = $(SATIREMAINDIR)/analyzer/
# we deliberately do not use "-I" for the first dir because a libname is
# expected in the gerated include file
SATIRE_INCLUDE = $(SATIREMAINDIR)/analyzer/commandlineinterface -I $(SATIREMAINDIR)/analyzer/astaccess -I $(SATIREMAINDIR)/analyzer/icfgbuilder -I $(SATIREMAINDIR)/analyzer/annotation
ANALYSIS = nielson
CARRIER_TYPE = dfi
SATIRE_INCLUDE_DIR=$(SATIREMAINDIR)/analyzer/astaccess
# you do not usually need to edit this file
PAG = $(PAGDIR)/bin/pag
FLAGS = -Wall -I $(ROSE_INCLUDE) -I $(PAGDIR)/include/pag -I $(PAGDIR)/include/pag/analyzer -I $(ANALYSIS)-out/src/ -I $(SATIRE_INCLUDE)
LPATH = -L $(ROSE_LIB) -L $(PAGDIR)/lib/pag -L $(SATIRE_LIB)
LIBS = $(ANALYSIS).a -lsatire -lrose -lrt -lpag

TESTDIR = tests

all: $(ANALYSIS)

$(ANALYSIS).a: $(ANALYSIS).set $(ANALYSIS).optla
	mkdir -p anim-out/
	mkdir -p $(ANALYSIS)-out/src
	if $(PAG) $(SATIRE_INCLUDE_DIR)/ iterate1.t $(ANALYSIS) $(ANALYSIS)-out --prefix=$(PAGDIR) 2> pag-out; then cat pag-out; else cat pag-out; false; fi
	cp $(SATIRE_LIB)/syntree.o $(ANALYSIS)-out/src/
	$(MAKE) CFLAGS="-DANIM -DDFI_WRITE" -C $(ANALYSIS)-out/src/
	cp $(ANALYSIS)-out/src/$(ANALYSIS).a .

main.o: main.C
	g++ $(FLAGS) -DPAG -DANALYSIS=$(ANALYSIS) -DCARRIER_TYPE=$(CARRIER_TYPE) -DDFI_WRITE -c main.C

main-support.o: main-support.C
	g++ $(FLAGS) -DPAG -DANALYSIS=$(ANALYSIS) -DCARRIER_TYPE=$(CARRIER_TYPE) -c main-support.C

$(ANALYSIS): $(ANALYSIS).a main.o main-support.o
	g++ $(FLAGS) -DCARRIER_TYPE=$(CARRIER_TYPE) -DANALYSIS=$(ANALYSIS) main.o main-support.o -o $(ANALYSIS) $(LPATH) $(LIBS) 
	@echo
	@echo 'PAG said:'
	@cat pag-out
	@echo "nothing, because it wasn't run (the analysis specification has not changed)" > pag-out


.PHONY: clean
clean:
	rm -f *~ *.o *.a pig_temp* syntree* $(ANALYSIS) *.gdl pag-out
	rm -rf $(ANALYSIS)-out/
	rm -rf anim-out

check:  $(ANALYSIS)
	mkdir -p anim-out
	$(ANALYSIS) $(TESTDIR)/reverse1.C
