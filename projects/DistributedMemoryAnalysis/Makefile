# Makefile used by Gergo on Zeus

ROSEDIR = /home/panas2/development/ROSE-64bit/install-gcc4-svn

INCLUDES = -I$(ROSEDIR)/include/ -I.
LPATH = -L$(ROSEDIR)/lib/ -Wl,-rpath $(ROSEDIR)/lib/
LIBS = -lrose -lrt -lfl

CXX = mpicxx
CXXFLAGS = -Wall -g $(INCLUDES)

OBJS = coloredAST.o DistributedMemoryAnalysis.o

all: coloredAST functionNames

coloredAST: coloredAST.o DistributedMemoryAnalysis.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LPATH) $(LIBS)

coloredAST.o: coloredAST.C DistributedMemoryAnalysis.h DistributedMemoryAnalysisImplementation.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

functionNames: functionNames.o DistributedMemoryAnalysis.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LPATH) $(LIBS)

functionNames.o: functionNames.C DistributedMemoryAnalysis.h DistributedMemoryAnalysisImplementation.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

check: coloredASTcheck functionNamesCheck

coloredASTcheck:
#	srun -N4 -ppdebug coloredAST -edg:w exampleInputs/*
	mpirun -np 4 coloredAST -edg:w exampleInputs/*

functionNamesCheck:
#	srun -N4 -ppdebug functionNames -edg:w exampleInputs/*
	mpirun -np 4 coloredAST -edg:w exampleInputs/*

clean:
	rm -f *.o coloredAST functionNames *.dot *~ gmon.out *.core
