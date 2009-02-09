# ------------ You probably need to adjust these settings ------------------
# Source and build directories
ROSE_SRC=/home/matzke/GS-CAD/ROSE/sources/trunk
ROSE_BLD=/home/matzke/GS-CAD/ROSE/build/trunk

# Install point for BOOST. Use "/usr" if installed system-wide
BOOST=/home/matzke/GS-CAD/boost_1_35_0/installed

# Location of libjvm.so used by librose.so
JVM=/etc/java-config-2/current-system-vm/jre/lib/amd64/server
# --------------------------------------------------------------------------



# Comment out to see full compiler commands
SILENT=@

all: findConstants tokenizeLinuxMain

# *.h files are scattered all over the source and build trees. This command constructs the list of include directories.
# We need to exclude the "rose-0.9.4a (and similar) build directories that are created from "make distcheck"
# Also exclude headers from EDG since they conflict with system headers (e.g., getopt.h)
INCDIRS=$(shell find $(ROSE_SRC) $(ROSE_BLD) -name '*.h' -printf '%h\n' |sort |uniq |grep -v /rose- |grep -v /EDG_)

CXXFLAGS=-g -Wall $(patsubst %,-I%,$(INCDIRS)) -I$(BOOST)/include
LDFLAGS=-L$(ROSE_BLD)/src/.libs -lrose -Wl,-rpath,$(JVM) -Wl,-rpath,$(BOOST)/lib

tokenizeLinuxMain: tokenizeLinuxMain.o tokenizeLinux.o
	@echo "  LINK $@"
	$(SILENT)$(CXX) -o $@ $^ $(LDFLAGS)

findConstants: findConstants.o tokenizeLinux.o
	@echo "  LINK $@"
	$(SILENT)$(CXX) -o $@ $^ $(LDFLAGS)

.C.o:
	@echo "  COMPILE $@"
	$(SILENT)$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f findConstants.o tokenizeLinux.o tokenizeLinuxMain.o
	rm -f findConstants tokenizeLinuxMain
