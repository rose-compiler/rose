#Set variable ROSE_INSTALL to your ROSE_INSTALL installation like this.
ROSE_INSTALL = /home/yuan5/ROSE/WORKING_COPY/INSTALL

CHECKER_NAME = allowedFunctions

LINKER_FLAGS = -L$(ROSE_INSTALL)/lib -Wl,-rpath $(ROSE_INSTALL)/lib -lrose $(RT_LIBS) -L`pwd` -Wl,-rpath `pwd` -lcompass 

all: allowedFunctionsTest

libcompass.so: compass.h compass.C
	g++ -fPIC -Wall -shared -o libcompass.so compass.C -I$(ROSE_INSTALL)/include

allowedFunctionsTest: allowedFunctions.C allowedFunctionsMain.C libcompass.so
	g++ -fPIC -Wall -o allowedFunctionsTest allowedFunctions.C allowedFunctionsMain.C -I$(ROSE_INSTALL)/include $(LINKER_FLAGS)

test: allowedFunctionsTest allowedFunctionsTest1.C
	./allowedFunctionsTest allowedFunctionsTest1.C

clean:
	-rm -f libcompass.so "allowedFunctionsTest"

