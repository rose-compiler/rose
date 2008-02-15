# Set Variables ROSE_INSTALL to your ROSE_INSTALL installation and
# COMPASS_SUPPORT to your compassSupport directory like this:
#ROSE_INSTALL= ROSE-INSTALL-DIRECTORY
#COMPASS_PROJECT = COMPASS-PROJECT-DIRECTORY 
#COMPASS_SUPPORT = $\{COMPASS_PROJECT\}/compassSupport

CHECKER_NAME = forLoopCppIndexVariableDeclarationTest

LINKER_FLAGS = -L$(ROSE_INSTALL)/lib -Wl,-rpath $(ROSE_INSTALL)/lib -lrose -lrt -lfl -L`pwd` -Wl,-rpath `pwd` -lcompass

all: forLoopCppIndexVariableDeclarationTest

libcompass.so: $(COMPASS_SUPPORT)/compass.h $(COMPASS_SUPPORT)/compass.C
	g++ -fPIC -Wall -shared -o $@ $(COMPASS_SUPPORT)/compass.C -I$(ROSE_INSTALL)/include -I$(COMPASS_SUPPORT)

forLoopCppIndexVariableDeclarationTest: forLoopCppIndexVariableDeclaration.C forLoopCppIndexVariableDeclarationMain.C libcompass.so $(COMPASS_SUPPORT)/compassTestMain.C
	g++ -fPIC -Wall -o $@ forLoopCppIndexVariableDeclaration.C forLoopCppIndexVariableDeclarationMain.C -I$(COMPASS_SUPPORT) -I$(ROSE_INSTALL)/include $(LINKER_FLAGS)

test: forLoopCppIndexVariableDeclarationTest forLoopCppIndexVariableDeclarationTest1.C
	./forLoopCppIndexVariableDeclarationTest forLoopCppIndexVariableDeclarationTest1.C

clean:
	rm -f libcompass.so "forLoopCppIndexVariableDeclarationTest"

