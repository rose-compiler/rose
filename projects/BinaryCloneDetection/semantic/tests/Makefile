# This makefile is not intended to be part of the ROSE build system.


CXX = g++
CXXFLAGS = -Wall -m32 -g
OPT_LEVELS = 0 1 2 3 s
MOSTLYCLEANFILES =
CLEANFILES =

all: combined

# Binaries where all optimization levels are combined into a single executable
.PHONY: combined
combined: simple001 simple002 sorting

# Binaries where each optimization level is its own executable
simple001_progs = $(addprefix simple001-, $(OPT_LEVELS))
simple002_progs = $(addprefix simple002-, $(OPT_LEVELS))
simple003_progs = $(addprefix simple003-, $(OPT_LEVELS))
externalsort_progs = $(addprefix externalsort-, $(OPT_LEVELS))
insertsort_progs   = $(addprefix   insertsort-, $(OPT_LEVELS))
qsort_progs        = $(addprefix        qsort-, $(OPT_LEVELS))
quicksort_progs    = $(addprefix    quicksort-, $(OPT_LEVELS))
shellsort_progs    = $(addprefix    shellsort-, $(OPT_LEVELS))
sort_progs = $(externalsort_progs) $(insertsort_progs) $(qsort_progs) $(quicksort_progs) $(shellsort_progs)
.PHONY: separate
separate: $(simple001_progs) $(simple002_progs) $(simple003_progs) $(sort_progs)


#------------------------------------------------------------------------------------------------------------------------
# Relatively simple specimens to help learn the basic capabilities of binary clone detection from Andreas
simple001_OBJ = $(addprefix simple001-, $(addsuffix .o, $(OPT_LEVELS)))
$(simple001_OBJ): simple001-%.o: simple001.C
	$(CXX) $(CXXFLAGS) -O$* -DNAMESPACE=optimize_$* -o $@ -c $<
simple001: $(simple001_OBJ) main.o
	$(CXX) $(CXXFLAGS) -o $@ $^
MOSTLYCLEANFILES += $(simple001_OBJ) main.o
CLEANFILES += simple001

# Same again, but this time create multiple executables
$(simple001_progs): simple001-%: simple001.C main.o
	$(CXX) $(CXXFLAGS) -o $@ -UNAMESPACE -O$* $^
MOSTLYCLEANFILES += $(simple001_progs)

#------------------------------------------------------------------------------------------------------------------------
# More simple specimens to help learn the basic capabilities of binary clone detection from Robb
simple002_OBJ = $(addprefix simple002-, $(addsuffix .o, $(OPT_LEVELS)))
$(simple002_OBJ): simple002-%.o: simple002.C
	$(CXX) $(CXXFLAGS) -O$* -DNAMESPACE=optimize_$* -o $@ -c $<
simple002: $(simple002_OBJ) main.o
	$(CXX) $(CXXFLAGS) -o $@ $^
MOSTLYCLEANFILES += $(simple002_OBJ) main.o
CLEANFILES += simple002

# Same again, but this time create multiple executables
$(simple002_progs): simple002-%: simple002.C main.o
	$(CXX) $(CXXFLAGS) -o $@ -UNAMESPACE -O$* $^
MOSTLYCLEANFILES += $(simple002_progs)

#------------------------------------------------------------------------------------------------------------------------
# More simple specimens that gcc -O3 does a good job of optimizing.
# These are always compiled separately
$(simple003_progs): simple003-%: simple003.c
	$(CC) $(CFLAGS) -o $@ -O$* $^
MOSTLYCLEANFILSE += $(simple003_progs)

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

# Same again, but this time create multiple executables
$(externalsort_progs): externalsort-%: externalsort.C main.o
	$(CXX) $(CXXFLAGS) -o $@ -UNAMESPACE -O$* $^
$(insertsort_progs): insertsort-%: insertsort.C main.o
	$(CXX) $(CXXFLAGS) -o $@ -UNAMESPACE -O$* $^
$(qsort_progs): qsort-%: qsort.C main.o
	$(CXX) $(CXXFLAGS) -o $@ -UNAMESPACE -O$* $^
$(quicksort_progs): quicksort-%: quicksort.C main.o
	$(CXX) $(CXXFLAGS) -o $@ -UNAMESPACE -O$* $^
$(shellsort_progs): shellsort-%: shellsort.C main.o
	$(CXX) $(CXXFLAGS) -o $@ -UNAMESPACE -O$* $^



#------------------------------------------------------------------------------------------------------------------------
# The usual suspects...

.PHONY: mostlyclean
mostlyclean:
	rm -f $(MOSTLYCLEANFILES)

.PHONY: clean
clean: mostlyclean
	rm -f $(CLEANFILES)
