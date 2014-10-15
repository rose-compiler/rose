
#our makefile variables.   Good to place commonly changed variables
# at the top of your makefile. Then follow with your rules.
HOME = .
VPATH= . $(HOME)/.. 
BLOCKSIZE=8
DOMAINSIZE=8

DIM = 2
system := $(shell uname)
osuffix:=$(DIM)D.o
dsuffix:=$(DIM)D.d
CFLAGS = -g
#CFLAGS = -O3 -DNDEBUG

CFLAGS += -I. -I$(HOME)/.. -DDIM=$(DIM) -DBLOCKSIZE=$(BLOCKSIZE) -DDOMAINSIZE=$(DOMAINSIZE) 

CXX=g++
SRCFILES:= $(wildcard $(HOME)/*.cpp)
OBJS:=$(patsubst %.cpp,%.o, $(SRCFILES))  


%.o: %.cpp GNUmakefile
	$(CXX) -c $(CFLAGS) $< -o $@
	$(CXX) -MM $(CFLAGS) $< > $*.d

%$(osuffix): %.cpp GNUmakefile
	$(CXX) -c $(CFLAGS) $< -o $@
	$(CXX) -MM $(CFLAGS) $< > $*$(dsuffix)

identity: GNUmakefile tests/identity.cpp $(SRCFILES) $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) $(LIBS) -o identity tests/identity.cpp

laplacian: GNUmakefile tests/laplacian.cpp $(SRCFILES) $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) $(LIBS) -o laplacian tests/laplacian.cpp

clean:
	rm  *.o *.d laplacian identity
realclean:
	rm -Rf *.dSYM *.o *.d laplacian identity

-include $(OBJS:.o=.d)
