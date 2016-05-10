#our makefile variables.   Good to place commonly changed variables
# at the top of your makefile. Then follow with your rules.
DIM = 2
MINBOXSIZE_EXP=2
MAXBLOCKSIZE_EXP=6
MYDEBUGFLAG=true
HOME = .
VPATH= . $(HOME)/../timer $(HOME)/../StencilTools $(HOME)/../VisitWriter
system := $(shell uname)
#LIBS:=   -L../lib
#CFLAGS = -g 
CFLAGS = -O3 -DNDEBUG 

CFLAGS += -I. -I../timer -I$(HOME)/../StencilTools -I$(HOME)/../VisitWriter -std=c++11
CPPFLAGS = -D DIM=$(DIM) -D MINBOXSIZE_EXP=$(MINBOXSIZE_EXP) -D MAXBLOCKSIZE_EXP=$(MAXBLOCKSIZE_EXP) -D MYDEBUGFLAG=$(MYDEBUGFLAG)
#CXX=clang++
CXX=g++
SRCFILES:= $(wildcard $(HOME)/../timer/*.cpp $(HOME)/Multigrid.cpp , $(HOME)/../StencilTools/*.cpp $(HOME)/../VisitWriter/*.cpp)
OBJS:=$(patsubst %.cpp,%.o, $(SRCFILES))  


%.o: %.cpp GNUmakefile
	$(CXX) -c $(CPPFLAGS) $(CFLAGS) $< -o $@
	$(CXX) -MM $(CFLAGS) $< > $*.d


multigrid: GNUmakefile  $(SRCFILES) $(OBJS) multigridTest.cpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) multigridTest.cpp $(OBJS)  $(LIBS) -o multigrid.exe

copytest: GNUmakefile  $(SRCFILES) $(OBJS) multigridTest.cpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) copyTest.cpp $(OBJS)  $(LIBS) -o copytest.exe

clean:
	rm  *.o *.exe *.d ../StencilTools/*.{o,d}

-include $(OBJS:.o=.d)
