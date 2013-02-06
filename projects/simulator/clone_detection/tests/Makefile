# This makefile is not intended to be part of the ROSE build system.


CXX = g++
CXXFLAGS = -Wall -m32
OPT_LEVELS = 0 1 2 3 s
MOSTLYCLEANFILES =
CLEANFILES =

all: simple001 sorting


#------------------------------------------------------------------------------------------------------------------------
# Relatively simple specimens to help learn the basic capabilities of binary clone detection
simple001_OBJ = $(addprefix simple001-, $(addsuffix .o, $(OPT_LEVELS)))
$(simple001_OBJ): simple001-%.o: simple001.C
	$(CXX) $(CXXFLAGS) -O$* -DNAMESPACE=optimize_$* -o $@ -c $<
simple001: $(simple001_OBJ) main.o
	$(CXX) $(CXXFLAGS) -o $@ $^
MOSTLYCLEANFILES += $(simple001_OBJ) main.o
CLEANFILES += simple001

#------------------------------------------------------------------------------------------------------------------------
# Some complex examples of sorting algorithms
externalsort_OBJ = $(addprefix externalsort-, $(addsuffix .o, $(OPT_LEVELS)))
insertsort_OBJ   = $(addprefix   insertsort-, $(addsuffix .o, $(OPT_LEVELS)))
qsort_OBJ        = $(addprefix        qsort-, $(addsuffix .o, $(OPT_LEVELS)))
quicksort_OBJ    = $(addprefix    quicksort-, $(addsuffix .o, $(OPT_LEVELS)))
shellsort_OBJ    = $(addprefix    shellsort-, $(addsuffix .o, $(OPT_LEVELS)))
$(externalsort_OBJ): externalsort-%.o: externalsort.C
	$(CXX) $(CXXFLAGS) -O$* -DNAMESPACE=optimize_$* -o $@ -c $<
$(insertsort_OBJ):     insertsort-%.o:   insertsort.C
	$(CXX) $(CXXFLAGS) -O$* -DNAMESPACE=optimize_$* -o $@ -c $<
$(qsort_OBJ):               qsort-%.o:        qsort.C
	$(CXX) $(CXXFLAGS) -O$* -DNAMESPACE=optimize_$* -o $@ -c $<
$(quicksort_OBJ):       quicksort-%.o:    quicksort.C
	$(CXX) $(CXXFLAGS) -O$* -DNAMESPACE=optimize_$* -o $@ -c $<
$(shellsort_OBJ):       shellsort-%.o:    shellsort.C
	$(CXX) $(CXXFLAGS) -O$* -DNAMESPACE=optimize_$* -o $@ -c $<
sorting_OBJ = $(externalsort_OBJ) $(insertsort_OBJ) $(qsort_OBJ) $(quicksort_OBJ) $(shellsort_OBJ)
sorting: $(sorting_OBJ) main.o
	$(CXX) $(CXXFLAGS) -o $@ $^
MOSTLYCLEANFILES += $(sorting_OBJ)
CLEANFILES += sorting



#------------------------------------------------------------------------------------------------------------------------
# The usual suspects...

.PHONY: mostlyclean
mostlyclean:
	rm -f $(MOSTLYCLEANFILES)

.PHONY: clean
clean: mostlyclean
	rm -f $(CLEANFILES)
