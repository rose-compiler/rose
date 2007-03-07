ANALYSIS = typeexpr
# you do not usually need to edit this file

# this is the only variable that must be set (all other vars are set with this
# variable as rootdir
SATIREMAINDIR = ../../..

SATIRELIBS = $(SATIREMAINDIR)/analyzer/
SATIREINCLUDES = -I $(SATIREMAINDIR)/analyzer/commandlineinterface -I $(SATIREMAINDIR)/analyzer/astaccess -I $(SATIREMAINDIR)/analyzer/icfgbuilder -I $(SATIREMAINDIR)/analyzer/annotation
COMMONDIR = $(SATIREMAINDIR)/analyzer/astaccess

PAG = $(PAGDIR)/bin/pag
FLAGS = -Wall -I $(ROSE_INCLUDE) -I $(PAGDIR)/include/pag -I $(PAGDIR)/include/pag/analyzer -I $(ANALYSIS)-out/src/ $(SATIREINCLUDES)
LPATH = -L $(ROSE_LIB) -L $(PAGDIR)/lib/pag -L $(SATIRELIBS)
LIBS = $(ANALYSIS).a -lsatire -lrose -ledg -lpag -lrt

all: $(ANALYSIS)

$(ANALYSIS).a: $(ANALYSIS).set $(ANALYSIS).optla
	mkdir -p $(ANALYSIS)-out
	if $(PAG) $(COMMONDIR)/ iterate1.t $(ANALYSIS) $(ANALYSIS)-out --prefix=$(PAGDIR) 2> pag-out; then cat pag-out; else cat pag-out; false; fi
	cp $(COMMONDIR)/syntree.o $(ANALYSIS)-out/src/
	$(MAKE) -C $(ANALYSIS)-out/src/
	cp $(ANALYSIS)-out/src/$(ANALYSIS).a .

main.o: main.C
	g++ $(FLAGS) -I $(ANALYSIS)-out/src -DANALYSIS=$(ANALYSIS) -DPAG -c main.C

$(ANALYSIS): $(ANALYSIS).a main.o
	g++ $(FLAGS) main.o -o $(ANALYSIS) $(LPATH) $(LIBS)
	@echo
	@echo 'PAG said:'
	@cat pag-out

.PHONY: clean
clean:
	rm -f *~ *.o *.a pig_temp* syntree* $(ANALYSIS) *.gdl pag-out
	rm -rf $(ANALYSIS)-out/
