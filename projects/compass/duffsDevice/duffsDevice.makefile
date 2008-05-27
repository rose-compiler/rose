# Set Variables ROSE_INSTALL to your ROSE_INSTALL installation and
# COMPASS_SUPPORT to your compassSupport directory like this:
#ROSE_INSTALL= ROSE-INSTALL-DIRECTORY
#COMPASS_PROJECT = COMPASS-PROJECT-DIRECTORY 
#COMPASS_SUPPORT = $\{COMPASS_PROJECT\}/compassSupport

CHECKER_NAME = duffsDeviceTest

LINKER_FLAGS = -L$(ROSE_INSTALL)/lib -Wl,-rpath $(ROSE_INSTALL)/lib -lrose $(RT_LIBS) -L`pwd` -Wl,-rpath `pwd` -lcompass

all: duffsDeviceTest

libcompass.so: $(COMPASS_SUPPORT)/compass.h $(COMPASS_SUPPORT)/compass.C
	g++ -fPIC -Wall -shared -o $@ $(COMPASS_SUPPORT)/compass.C -I$(ROSE_INSTALL)/include -I$(COMPASS_SUPPORT)

duffsDeviceTest: duffsDevice.C duffsDeviceMain.C libcompass.so $(COMPASS_SUPPORT)/compassTestMain.C
	g++ -fPIC -Wall -o $@ duffsDevice.C duffsDeviceMain.C -I$(COMPASS_SUPPORT) -I$(ROSE_INSTALL)/include $(LINKER_FLAGS)

test: duffsDeviceTest duffsDeviceTest1.C
	./duffsDeviceTest duffsDeviceTest1.C

clean:
	rm -f libcompass.so "duffsDeviceTest"

