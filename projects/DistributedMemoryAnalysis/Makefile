# Makefile used by Gergo on Zeus

ROSEDIR = $(HOME)/sep28/ROSE_build

INCLUDES = -I$(ROSEDIR)/include/ -I.
LPATH = -L$(ROSEDIR)/lib/ -Wl,-rpath $(ROSEDIR)/lib/
LIBS = -lrose -lrt -lfl

CXX = mpiCC
CXXFLAGS = -Wall $(INCLUDES)

OBJS = coloredAST.o DistributedMemoryAnalysis.o

all: coloredAST functionNames

coloredAST: coloredAST.o DistributedMemoryAnalysis.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LPATH) $(LIBS)

coloredAST.o: coloredAST.C DistributedMemoryAnalysis.h DistributedMemoryAnalysisTemplateDefs.C
	$(CXX) $(CXXFLAGS) -c $< -o $@

functionNames: functionNames.o DistributedMemoryAnalysis.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LPATH) $(LIBS)

functionNames.o: functionNames.C DistributedMemoryAnalysis.h DistributedMemoryAnalysisTemplateDefs.C
	$(CXX) $(CXXFLAGS) -c $< -o $@

check: coloredASTcheck functionNamesCheck

coloredASTcheck:
	srun -N4 -ppdebug coloredAST -edg:w exampleInputs/*

functionNamesCheck:
	srun -N4 -ppdebug functionNames -edg:w exampleInputs/*

clean:
	rm -f *.o coloredAST functionNames *.dot *~ gmon.out *.core
