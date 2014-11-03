## A sample Makefile to build a ROSE tool.
##
## Important: remember that Makefile rules must contain tabs:
##
##     <target>: [ <dependency > ]*
##         [ <TAB> <command> <endl> ]+
## 
## Please replace space with TAB if you copy & paste this file to create your Makefile!!

## ROSE installation contains
##   * libraries, e.g. "librose.la"
##   * headers, e.g. "rose.h"
ROSE_INSTALL=/home/lin32/Development/projects/rose-master/install

## ROSE uses the BOOST C++ libraries, the --prefix path
BOOST_INSTALL=/home/lin32/Development/opt/boost

## Your translator
TRANSLATOR=mlm
TRANSLATOR_SOURCE=$(TRANSLATOR).cpp

## Input testcode for your translator
TESTCODE=stream.c

#-------------------------------------------------------------
# Makefile Targets
# #-------------------------------------------------------------
#
all: $(TRANSLATOR)
#
# # compile the translator and generate an executable
# # -g is recommended to be used by default to enable debugging your code
$(TRANSLATOR): $(TRANSLATOR_SOURCE)
	g++ -g $(TRANSLATOR_SOURCE) -o $(TRANSLATOR) -I$(BOOST_INSTALL)/include -I$(ROSE_INSTALL)/include/rose -L$(ROSE_INSTALL)/lib -lrose
#
#         # test the translator
check: $(TRANSLATOR)
	./$(TRANSLATOR) -c -I. -I$(ROSE_INSTALL)/include/rose $(TESTCODE) 

clean:
	rm -rf $(TRANSLATOR) *.o rose_* *.dot

