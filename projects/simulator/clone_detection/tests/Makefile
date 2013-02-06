# This makefile is not intended to be part of the ROSE build system.

all: simple001

CXX = g++
CXXFLAGS = -Wall -m32
OPT_LEVELS = 0 1 2 3 s

simple001_OBJ = $(addprefix simple001-, $(addsuffix .o, $(OPT_LEVELS)))
$(simple001_OBJ): simple001-%.o: simple001.C
	$(CXX) $(CXXFLAGS) -O$* -DNAMESPACE=optimize_$* -o $@ -c $<
simple001: $(simple001_OBJ) main.o
	$(CXX) $(CXXFLAGS) -o $@ $^


.PHONY: mostlyclean
mostlyclean:
	rm -f $(simple001_OBJ) main.o

.PHONY: clean
clean: mostlyclean
	rm -f simple001
