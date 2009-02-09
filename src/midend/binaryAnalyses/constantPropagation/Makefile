all: findConstants tokenizeLinux

ROSE_INSTALL = /home/willcock2/rose-svn-checkout/install
BOOST_INSTALL = /home/willcock2/boost-1.35-install64

findConstants: findConstants.C flowEquations.h tokenizeLinux.C tokenizeLinux.h $(ROSE_INSTALL)/include/x86InstructionSemantics.h
	g++ -g -Wall -o findConstants findConstants.C tokenizeLinux.C -I$(ROSE_INSTALL)/include -I$(BOOST_INSTALL)/include/boost-1_35 -L$(ROSE_INSTALL)/lib -lrose

tokenizeLinux: tokenizeLinux.C tokenizeLinuxMain.C tokenizeLinux.h
	g++ -g -Wall -o tokenizeLinux tokenizeLinux.C tokenizeLinuxMain.C -I$(ROSE_INSTALL)/include -I$(BOOST_INSTALL)/include/boost-1_35

clean:
	-rm -f findConstants
