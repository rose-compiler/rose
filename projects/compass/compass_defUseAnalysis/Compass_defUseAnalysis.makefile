# Set Variables ROSE_INSTALL to your ROSE_INSTALL installation and
# COMPASS_SUPPORT to your compassSupport directory like this:
#ROSE_INSTALL= ROSE-INSTALL-DIRECTORY
#COMPASS_PROJECT = COMPASS-PROJECT-DIRECTORY 
#COMPASS_SUPPORT = $\{COMPASS_PROJECT\}/compassSupport

CHECKER_NAME = Compass_defUseAnalysisTest

LINKER_FLAGS = -L$(ROSE_INSTALL)/lib -Wl,-rpath $(ROSE_INSTALL)/lib -lrose -lrt -lfl -L`pwd` -Wl,-rpath `pwd` -lcompass

all: Compass_defUseAnalysisTest

libcompass.so: $(COMPASS_SUPPORT)/compass.h $(COMPASS_SUPPORT)/compass.C
	g++ -fPIC -Wall -shared -o $@ $(COMPASS_SUPPORT)/compass.C -I$(ROSE_INSTALL)/include -I$(COMPASS_SUPPORT)

Compass_defUseAnalysisTest: Compass_defUseAnalysis.C Compass_defUseAnalysisMain.C libcompass.so $(COMPASS_SUPPORT)/compassTestMain.C
	g++ -fPIC -Wall -o $@ Compass_defUseAnalysis.C Compass_defUseAnalysisMain.C -I$(COMPASS_SUPPORT) -I$(ROSE_INSTALL)/include $(LINKER_FLAGS)

test: Compass_defUseAnalysisTest Compass_defUseAnalysisTest1.C
	./Compass_defUseAnalysisTest Compass_defUseAnalysisTest1.C

clean:
	rm -f libcompass.so "Compass_defUseAnalysisTest"

