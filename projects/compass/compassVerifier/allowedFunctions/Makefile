#Set variable ROSE_INSTALL to your ROSE_INSTALL installation like this.
ROSE_INSTALL = /home/yuan5/ROSE/WORKING_COPY/INSTALL
COMPASS_PROJECT=/home/yuan5/ROSE/WORKING_COPY/SRC/projects/compass

CHECKER_NAME = allowedFunctions

INCLUDES=-I$(COMPASS_PROJECT)/compassSupport -I$(ROSE_INSTALL)/include

LINKER_FLAGS = -L$(ROSE_INSTALL)/lib -Wl,-rpath $(ROSE_INSTALL)/lib -lrose $(RT_LIBS) -L`pwd` -Wl,-rpath `pwd` -lcompass 

all: allowedFunctionsTest

libcompass.so: $(COMPASS_PROJECT)/compassSupport/compass.h $(COMPASS_PROJECT)/compassSupport/compass.C
	g++ -fPIC -Wall -shared -o libcompass.so $(INCLUDES) $(COMPASS_PROJECT)/compassSupport/compass.C

allowedFunctionsTest: allowedFunctions.C allowedFunctionsMain.C libcompass.so
	g++ -fPIC -Wall -o allowedFunctionsTest $(INCLUDES) allowedFunctions.C allowedFunctionsMain.C $(LINKER_FLAGS)

test: allowedFunctionsTest allowedFunctionsTest1.C
	./allowedFunctionsTest allowedFunctionsTest1.C

clean:
	-rm -f libcompass.so "allowedFunctionsTest"

