## A sample Makefile to build a ROSE tool.
##
## Important: remember that Makefile recipes must contain tabs:
##
##     <target>: [ <dependency > ]*
##         [ <TAB> <command> <endl> ]+
## So you have to replace spaces with Tabs if you copy&paste this file from a browser!

## ROSE installation contains
##   * libraries, e.g. "librose.la"
##   * headers, e.g. "rose.h"
ROSE_INSTALL=/home/liao6/workspace/raja/rose/install

## ROSE uses the BOOST C++ libraries
BOOST_INSTALL=/nfs/casc/overture/ROSE/opt/rhel7/x86_64/boost/1_54_0/gcc/4.8.3

## Your translator
TRANSLATOR=xgenTranslator
TRANSLATOR_SOURCE=$(TRANSLATOR).cpp

## Input testcode for your translator
TESTCODE=test1.cpp

#-------------------------------------------------------------
# Makefile Targets
#-------------------------------------------------------------

all: $(TRANSLATOR)

# compile the translator and generate an executable
# -g is recommended to be used by default to enable debugging your code
# Note: depending on the version of boost, you may have to use something like -I $(BOOST_ROOT)/include/boost-1_40 instead. 
$(TRANSLATOR): $(TRANSLATOR_SOURCE)
	g++ -g $(TRANSLATOR_SOURCE) -I$(BOOST_INSTALL)/include -I$(ROSE_INSTALL)/include/rose -L$(ROSE_INSTALL)/lib -lrose -L$(BOOST_INSTALL)/lib -lboost_iostreams -lboost_system -o $(TRANSLATOR)

# test the translator
check: $(TRANSLATOR)
	source ./set.rose ; ./$(TRANSLATOR) -c -rose:Cxx11_only -I. -I$(ROSE_INSTALL)/include $(TESTCODE) 

clean:
	rm -rf $(TRANSLATOR) *.o rose_* *.dot

