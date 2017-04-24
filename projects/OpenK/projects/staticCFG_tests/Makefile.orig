## A sample Makefile to build a ROSE tool.
##
## Important: remember that Makefile recipes must contain tabs:
##
##     <target>: [ <dependency > ]*
##         [ <TAB> <command> <endl> ]+
## So you have to replace spaces with Tabs if you copy&paste this file from a browser!

LEVEL=../..

include $(LEVEL)/Makefile.common

## Configure
CC=g++
CCFLAGS = -I$(BOOST_INS)/include -I$(ROSE_INS)/include/rose -g
LIBS = -L$(ROSE_INS)/lib -lrose -L$(BOOST_INS)/lib -lboost_iostreams -lboost_system


## Your translator
TRANSLATOR = testStaticCFG
TRANSLATOR_EXE = $(TRANSLATOR).exe
DEP = $(TRANSLATOR).o 


## Input testcode for your translator
TESTDIR = .
TESTCODE = test2001_01.C
TESTFLAGS =

#-------------------------------------------------------------
# Makefile Targets
#-------------------------------------------------------------

all : $(TRANSLATOR).exe

# compile the translator and generate an executable
# -g is recommended to be used by default to enable debugging your code
# Note: depending on the version of boost, you may have to use something like -I $(BOOST_ROOT)/include/boost-1_40 instead. 
$(TRANSLATOR).exe : $(DEP)
	$(CC) -o $@ $^ $(CCFLAGS) $(LIBS) $(PLFLAGS)

$(TRANSLATOR).o : $(TRANSLATOR).C
	$(CC) -c -o $@ $< $(CCFLAGS) $(LIBS) $(PLFLAGS)

# test the translator
check:
	./$(TRANSLATOR).exe $(TESTFLAGS) -c $(TESTDIR)/$(TESTCODE) 

	
clean:
	rm -rf $(TRANSLATOR_EXE) *.exe *.o *.dot *.txt 

