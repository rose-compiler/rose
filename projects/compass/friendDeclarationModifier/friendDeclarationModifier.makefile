# Set Variables ROSE_INSTALL to your ROSE_INSTALL installation and
# COMPASS_SUPPORT to your compassSupport directory like this:
#ROSE_INSTALL= ROSE-INSTALL-DIRECTORY
#COMPASS_PROJECT = COMPASS-PROJECT-DIRECTORY 
#COMPASS_SUPPORT = $\{COMPASS_PROJECT\}/compassSupport

CHECKER_NAME = friendDeclarationModifierTest

LINKER_FLAGS = -L$(ROSE_INSTALL)/lib -Wl,-rpath $(ROSE_INSTALL)/lib -lrose $(RT_LIBS) -L`pwd` -Wl,-rpath `pwd` -lcompass

all: friendDeclarationModifierTest

libcompass.so: $(COMPASS_SUPPORT)/compass.h $(COMPASS_SUPPORT)/compass.C
	g++ -fPIC -Wall -shared -o $@ $(COMPASS_SUPPORT)/compass.C -I$(ROSE_INSTALL)/include -I$(COMPASS_SUPPORT)

friendDeclarationModifierTest: friendDeclarationModifier.C friendDeclarationModifierMain.C libcompass.so $(COMPASS_SUPPORT)/compassTestMain.C
	g++ -fPIC -Wall -o $@ friendDeclarationModifier.C friendDeclarationModifierMain.C -I$(COMPASS_SUPPORT) -I$(ROSE_INSTALL)/include $(LINKER_FLAGS)

test: friendDeclarationModifierTest friendDeclarationModifierTest1.C
	./friendDeclarationModifierTest friendDeclarationModifierTest1.C

clean:
	rm -f libcompass.so "friendDeclarationModifierTest"

