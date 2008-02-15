# Set Variables ROSE_INSTALL to your ROSE_INSTALL installation and
# COMPASS_SUPPORT to your compassSupport directory like this:
#ROSE_INSTALL= /home/dquinlan/rose-install
#COMPASS_SUPPORT = /home/dquinlan/ROSE/NEW_ROSE/projects/compass/compassVerifier/compassSupport

CHECKER_NAME = fileReadOnlyAccessTest

LINKER_FLAGS = -L$(ROSE_INSTALL)/lib -Wl,-rpath $(ROSE_INSTALL)/lib -lrose -lrt -lfl -L`pwd` -Wl,-rpath `pwd` -lcompass

all: fileReadOnlyAccessTest

libcompass.so: $(COMPASS_SUPPORT)/compass.h $(COMPASS_SUPPORT)/compass.C
	g++ -fPIC -Wall -shared -o $@ $(COMPASS_SUPPORT)/compass.C -I$(ROSE_INSTALL)/include -I$(COMPASS_SUPPORT)

fileReadOnlyAccessTest: fileReadOnlyAccess.C fileReadOnlyAccessMain.C libcompass.so $(COMPASS_SUPPORT)/compassTestMain.C
	g++ -fPIC -Wall -o $@ fileReadOnlyAccess.C fileReadOnlyAccessMain.C -I$(COMPASS_SUPPORT) -I$(ROSE_INSTALL)/include $(LINKER_FLAGS)

test: fileReadOnlyAccessTest fileReadOnlyAccessTest1.C
	./fileReadOnlyAccessTest fileReadOnlyAccessTest1.C

clean:
	rm -f libcompass.so "fileReadOnlyAccessTest"

