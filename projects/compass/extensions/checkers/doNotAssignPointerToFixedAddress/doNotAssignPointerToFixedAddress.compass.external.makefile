# Set Variables ROSE_INSTALL to your ROSE_INSTALL installation and
# COMPASS_SUPPORT to your compassSupport directory like this:
#ROSE_INSTALL=ROSE-INSTALL-DIRECTORY
#COMPASS_PROJECT=COMPASS-PROJECT-DIRECTORY
#COMPASS_SUPPORT=${COMPASS_PROJECT}/src/compassSupport

CHECKER_NAME = doNotAssignPointerToFixedAddressTest

LINKER_FLAGS = -L$(ROSE_INSTALL)/lib -Wl,-rpath $(ROSE_INSTALL)/lib -lrose $(RT_LIBS) -L`pwd` -Wl,-rpath `pwd` -lcompass

all: prerequisites.h instantiate_prerequisites.h doNotAssignPointerToFixedAddressTest

prerequisites.h: $(COMPASS_PROJECT)/extensions/prerequisites
	find $(COMPASS_PROJECT)/extensions/prerequisites -name "*.h" | awk -F/ '{print "#include \"" $$NF "\""}' > $@

instantiate_prerequisites.h: $(COMPASS_PROJECT)/extensions/prerequisites
	cat $(COMPASS_PROJECT)/extensions/prerequisites/*.h | grep "^extern" | sed -e 's@extern[\t\ ]*@Compass::@g' | awk '{print $$1 " Compass::" $$2}' > $@


libcompass.so: $(COMPASS_PROJECT)/src/compassSupport/compass.h $(COMPASS_PROJECT)/src/compassSupport/compass.C prerequisites.h instantiate_prerequisites.h
	g++ -fPIC -Wall -shared -o $@ $(COMPASS_PROJECT)/src/compassSupport/compass.C -I$(ROSE_INSTALL)/include -I$(COMPASS_PROJECT)/src/compassSupport -I$(COMPASS_PROJECT)/extensions/prerequisites -I.

# GMY: The order of source files during compilation is important due to the use
# of extern. The checker source code must follow the main source or else the
# executable is likely to seg. fault.
doNotAssignPointerToFixedAddressTest: doNotAssignPointerToFixedAddress.C doNotAssignPointerToFixedAddressMain.C libcompass.so $(COMPASS_PROJECT)/src/compassSupport/compassTestMain.C
	g++ -fPIC -Wall -o $@ doNotAssignPointerToFixedAddressMain.C doNotAssignPointerToFixedAddress.C -I$(COMPASS_PROJECT)/src/compassSupport -I$(ROSE_INSTALL)/include -I$(COMPASS_PROJECT)/extensions/prerequisites -I. $(LINKER_FLAGS)

test: doNotAssignPointerToFixedAddressTest doNotAssignPointerToFixedAddressTest1.C
	./doNotAssignPointerToFixedAddressTest doNotAssignPointerToFixedAddressTest1.C

clean:
	rm -f libcompass.so "doNotAssignPointerToFixedAddressTest"

