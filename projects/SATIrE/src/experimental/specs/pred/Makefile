ANALYSIS = pred
CARRIER_TYPE = CarrierType

#

SATIRE_SRC_DIR = ../../..
SATIRE_INCLUDE = -I $(SATIRE_SRC_DIR)/analyzer/commandlineinterface -I $(SATIRE_SRC_DIR)/analyzer/astaccess -I $(SATIRE_SRC_DIR)/analyzer/icfgbuilder -I $(SATIRE_SRC_DIR)/analyzer/annotation
SATIRE_LIB = $(SATIRE_SRC_DIR)/analyzer
SATIRE_FRONTENDDIR = $(SATIRE_SRC_DIR)/analyzer/astaccess
#PAGDIR

# you do not usually need to edit this file
PAG = $(PAGDIR)/bin/pag
FLAGS = -Wall -I $(ROSE_INCLUDE) -I $(PAGDIR)/include/pag -I $(PAGDIR)/include/pag/analyzer -I $(ANALYSIS)-out/src/ $(SATIRE_INCLUDE)
LPATH = -L $(ROSE_LIB) -L $(PAGDIR)/lib/pag -L $(SATIRE_LIB)
LIB = $(ANALYSIS).a -lsatire -lrose -lrt -lpag

all: $(ANALYSIS)

$(ANALYSIS).a: $(ANALYSIS).set $(ANALYSIS).optla
	mkdir -p $(ANALYSIS)-out/src
	mkdir -p anim-out
	if $(PAG) $(SATIRE_FRONTENDDIR)/ iterate1.t $(ANALYSIS) $(ANALYSIS)-out --prefix=$(PAGDIR) 2> pag-out; then cat pag-out; else cat pag-out; false; fi
	cp $(SATIRE_LIB)/syntree.o $(ANALYSIS)-out/src/
	$(MAKE) -C $(ANALYSIS)-out/src/
	cp $(ANALYSIS)-out/src/$(ANALYSIS).a .

main.o: main.C
	g++ $(FLAGS) -DANALYSIS=$(ANALYSIS) -DCARRIER_TYPE=$(CARRIER_TYPE) -c main.C

main-support.o: main-support.C
	g++ $(FLAGS) -DANALYSIS=$(ANALYSIS) -DCARRIER_TYPE=$(CARRIER_TYPE) -c main-support.C

$(ANALYSIS): $(ANALYSIS).a main.o main-support.o
	g++ $(FLAGS) -DCARRIER_TYPE=$(CARRIER_TYPE) -DANALYSIS=$(ANALYSIS) main.o main-support.o -o $(ANALYSIS) $(LPATH) $(LIB) 
	@echo
	@echo 'PAG said:'
	@cat pag-out
	@echo "nothing, because it wasn't run (the analysis specification has not changed)" > pag-out

check:	
	pred tests/test3.C
	pred --textoutput tests/test3.C
	pred --sourceoutput tests/test4.C
	pred --sourceoutput tests/test1.C
	pred --sourceoutput tests/test3.C
	pred --sourceoutput reverse1.C
	pred --sourceoutput tests/test8.C

test:	pred
	generatedot graphtest1
	generatedot graphtest2
	generatedot graphtest3
	generatedot graphtest4

.PHONY: clean
clean:
	rm -f *~ *.o *.a pig_temp* syntree* $(ANALYSIS) *.gdl pag-out
	rm -rf $(ANALYSIS)-out/
	rm -f anim-out/*.gdl
