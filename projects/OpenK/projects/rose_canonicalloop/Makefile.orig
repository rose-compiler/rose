## A Makefile to build a ROSE tool.

# relative path to the top dir (openk)

LEVEL=../..

include $(LEVEL)/Makefile.common

## Configure
CC=g++
CCFLAGS = -I$(BOOST_INS)/include -I$(ROSE_INS)/include/rose 
LIBS = -L$(ROSE_INS)/lib -lrose -L$(BOOST_INS)/lib -lboost_iostreams


## Input testcode for your translator
TESTDIR = unittest
TESTCODE = is.c

ROSECL = roseNativeCl

#-------------------------------------------------------------
# Makefile Targets
#-------------------------------------------------------------

all: $(ROSECL).exe

$(ROSECL).exe : $(ROSECL).cpp
	$(CC) -g $(ROSECL).cpp $(CCFLAGS) $(LIBS) -o $(ROSECL).exe

# test
check:
	./$(ROSECL).exe -c $(TESTDIR)/$(TESTCODE) 

clean:
	rm -rf  *.exe *.o  *.dot *.report.txt

