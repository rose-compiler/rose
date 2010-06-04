
CC=gcc
CFLAGS=-Wno-deprecated -g

LDFLAGS=-lstdc++ -lfada -lrose -lpiplib64 -lppl
LD_DIR=-L$(BOOST_HOME)/lib -L$(FADA_HOME)/lib -L$(ROSE_HOME)/lib -L$(PIP_HOME)/lib -L$(PPL_HOME)/lib

INCLUDE=-I$(BOOST_HOME)/include -I$(ROSE_HOME)/include -I$(PIP_HOME)/include -I$(FADA_HOME)/include -I$(PPL_HOME)/include 

all: TestRoseToFada TestRoseToPPL

# RoseToFadaCommon
	
RoseToFadaCommon.o: RoseToFadaCommon.cpp RoseToFadaCommon.hpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# RoseToFadaAST
	
RoseToFadaAST.o: RoseToFadaAST.cpp RoseToFadaAST.hpp RoseToFadaCommon.hpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	
# FadaToPPL

FadaToPPL.o: FadaToPPL.cpp FadaToPPL.hpp RoseToFadaCommon.hpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# *********************************************************************************************************

# TestRoseToFada

TestRoseToFada.o: TestRoseToFada.cpp RoseToFadaAST.hpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

TestRoseToFada: TestRoseToFada.o RoseToFadaCommon.o RoseToFadaAST.o
	$(CC) $^ $(LD_DIR) $(LDFLAGS) -o $@

# TestRoseToPPL

TestRoseToPPL.o: TestRoseToPPL.cpp FadaToPPL.hpp RoseToFadaAST.hpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	
TestRoseToPPL: TestRoseToPPL.o FadaToPPL.o RoseToFadaCommon.o RoseToFadaAST.o
	$(CC) $^ $(LD_DIR) $(LDFLAGS) -o $@


clean:
	rm -rf *.o
	
clean_all: clean
	rm -rf FadaTest 
