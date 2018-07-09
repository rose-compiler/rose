# Example Makefile for ROSE users
# This makefile is provided as an example of how to use ROSE when ROSE is
# installed (using "make install").  This makefile is tested as part of the
# "make distcheck" rule (run as part of tests before any CVS checkin).
# The test of this makefile can also be run by using the "make installcheck"
# rule (run as part of "make distcheck").


# Location of include directory after "make install"

ifeq ($(BOOST_HOME),)
  echo "setting default BOOST_HOME to /usr/include"
  BOOST_HOME=/usr
endif

ifeq ($(ROSE_ROOT),)
  echo "setting default ROSE_ROOT to ./rose"
  ROSE_ROOT=./rose
endif

ifeq ($(ROSE_HOME),)
  echo "setting default ROSE_HOME to $(ROSE_ROOT)/installtree"
  ROSE_HOME=$(ROSE_ROOT)/installtree)
endif

ifeq ($(ROSE_BUILD),)
  echo "setting default ROSE_HOME to $(ROSE_ROOT)/buildtree"
  ROSE_BUILD=$(ROSE_ROOT)/buildtree)
endif

ifeq ($(ROSE_SRC),)
  echo "setting default ROSE_SRC to $(ROSE_ROOT)"
  ROSE_SRC=$(ROSE_ROOT)
endif



# Location of Boost include directory
#BOOST_CPPFLAGS = -pthread -I/usr/include
BOOST_CPPFLAGS = -pthread -I$(BOOST_HOME)/include -L$(BOOST_HOME)/lib/ -lboost_system -lboost_iostreams


# Location of Dwarf include and lib (if ROSE is configured to use Dwarf)
ROSE_INCLUDE_DIR = $(ROSE_HOME)/include


CC                    = gcc
CXX                   = g++
CPPFLAGS              =
#CXXCPPFLAGS           = @CXXCPPFLAGS@
CXXFLAGS              =  -ggdb -O0 -Wall -Wextra
LDFLAGS               =

# Location of library directory after "make install"
# ROSE_LIB_DIR = /opt/rose-edg4/install/lib

#ROSE_LIB_DIR = /opt/rose-edg4/build/src
ROSE_LIBS = $(ROSE_HOME)/lib/librose.la

ROSE_INCLUDES = \
     -I$(ROSE_INCLUDE_DIR) \
     -I$(ROSE_INCLUDE_DIR)/rose

SOURCES = \
			instrumenter/RTC.C \
			instrumenter/funcDecl.C \
			instrumenter/strDecl.C \
			instrumenter/util.C \
			instrumenter/preProcessAST.C \
			instrumenter/traverse.C \
			instrumenter/instr.C \
			instrumenter/instrBinaryOps.C \
			instrumenter/instrCallExps.C \
			instrumenter/instrDeclStmts.C \
			instrumenter/instrDeleteExps.C \
			instrumenter/instrNewExps.C \
			instrumenter/instrRefExps.C \
			instrumenter/instrRetStmts.C \
			instrumenter/instrScopeStmts.C \
			instrumenter/instrSizeOfOps.C \
			instrumenter/instrThisExps.C \
			instrumenter/instrUnaryOps.C \
			instrumenter/instrVarRefs.C \
			instrumenter/typechecking.C

HEADERS = \
			instrumenter/RTC.h \
			instrumenter/funcDecl.h \
			instrumenter/util.h \
			instrumenter/strDecl.h \
			instrumenter/preProcessAST.h \
			instrumenter/traverse.h \
			instrumenter/instr.h \
			instrumenter/typechecking.h

OBJECTS = $(SOURCES:instrumenter/%.C=%.lo)

.PHONY: metadatalib clean purge

# Default make rule to use
all: RTC
	@if [ x$${ROSE_IN_BUILD_TREE:+present} = xpresent ]; then echo "ROSE_IN_BUILD_TREE should not be set" >&2; exit 1; fi

check: $(RTED_TESTS)

# Example suffix rule for more experienced makefile users
# .C.o:
#	g++ -c -O0 -I$(ROSE_INCLUDE_DIR) -o $@ $(@:.o=.C)

# Compile the exampleTranslator using the file identityTranslator.C
%.lo: instrumenter/%.C $(HEADERS)
	/bin/bash $(ROSE_BUILD)/libtool --tag=CXX --mode=compile $(CXX) $(CXXFLAGS)  $(CPPFLAGS) $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -c -o $@ $<

RTC: $(OBJECTS)
	/bin/bash $(ROSE_BUILD)/libtool --tag=CXX --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(ROSE_LIBS) $(BOOST_CPPFLAGS)


#
# tests

# metadata lib - duplicates the target in ../tests/Makefile
metadatalib: metadata_alt.o metalib_alt.o

metadata_alt.o: metadata/metadata_alt.C metadata/metadata_alt.h metadata/LockMgr.h metadata/TrackingDB.h metadata/rtc-defines.h
	$(CXX) -Wall -Wextra -pedantic -c $<

metalib_alt.o: metadata/metalib_alt.C metadata/metalib_alt.h metadata/LockMgr.h metadata/TrackingDB.h metadata/rtc-defines.h
	$(CXX) -Wall -Wextra -pedantic -c $<

#
# clean

clean:
	rm -rf *.rtc.c *.rtc.o *.rtc.bin

purge: clean
	rm -rf .libs *.lo RTC *.o
