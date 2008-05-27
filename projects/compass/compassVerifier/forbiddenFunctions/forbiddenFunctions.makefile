# Set Variables ROSE_INSTALL to your ROSE_INSTALL installation and
# COMPASS_SUPPORT to your compassSupport directory like this:
#ROSE_INSTALL= /home/dquinlan/rose-install
#COMPASS_SUPPORT = /home/dquinlan/ROSE/NEW_ROSE/projects/compass/compassVerifier/compassSupport

CHECKER_NAME = forbiddenFunctionsTest

LINKER_FLAGS = -L$(ROSE_INSTALL)/lib -Wl,-rpath $(ROSE_INSTALL)/lib -lrose $(RT_LIBS) -L`pwd` -Wl,-rpath `pwd` -lcompass

all: forbiddenFunctionsTest

libcompass.so: $(COMPASS_SUPPORT)/compass.h $(COMPASS_SUPPORT)/compass.C
	g++ -fPIC -Wall -shared -o $@ $(COMPASS_SUPPORT)/compass.C -I$(ROSE_INSTALL)/include -I$(COMPASS_SUPPORT)

forbiddenFunctionsTest: forbiddenFunctions.C forbiddenFunctionsMain.C libcompass.so $(COMPASS_SUPPORT)/compassTestMain.C
	g++ -fPIC -Wall -o $@ forbiddenFunctions.C forbiddenFunctionsMain.C -I$(COMPASS_SUPPORT) -I$(ROSE_INSTALL)/include $(LINKER_FLAGS)

test: forbiddenFunctionsTest forbiddenFunctionsTest1.C
	./forbiddenFunctionsTest forbiddenFunctionsTest1.C

clean:
	rm -f libcompass.so "forbiddenFunctionsTest"

