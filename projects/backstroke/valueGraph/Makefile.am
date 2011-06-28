include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = headerUnparser

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LINK	 = $(CXXLINK)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


# noinst_LIBRARIES lists all the librarlies that should be built, but not installed
#noinst_LIBRARIES =  libBackstrokeValueGraph.a
noinst_PROGRAMS = valueGraph

valueGraph_SOURCES = valueGraphNode.C valueGraph.C graphSearch.C functionReverser.C \
		pathNumGenerator.C CFGFilter.C codeGeneration.C types.C test.C
valueGraph_LDADD = $(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.a \
		$(top_builddir)/projects/backstroke/normalizations/libBackstrokeNormalizations.a \
		$(top_builddir)/projects/backstroke/ssa/libSSA.a \
		$(LIBS_WITH_RPATH) $(ROSE_LIBS)
valueGraph_include_HEADERS = valueGraph.h valueGraphNode.h pathNumGenerator.h functionReverser.h \
			CFGFilter.h types.h
valueGraph_includedir = $(includedir)/backstroke


# EXTRA_DIST are files that are not compiled or installed. These include readme's, internal header files, etc.
EXTRA_DIST = 

CLEANFILES = 


check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/valueGraph: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

test:
	./valueGraph -c temp.C -I/home/hou1/opt/boost_1_36_0//include/boost-1_36
#./valueGraph -c temp1.C -I/opt/local/var/macports/software/boost/1.44.0_0/opt/local/include/
#./valueGraph -c temp2.C -I/opt/local/var/macports/software/boost/1.44.0_0/opt/local/include/

#GTNETS_DIR = /home/hou1/MyWork/GTNetS/SRC
GTNETS_DIR = /Users/hou1/MyWork/GTNetS/SRC_original

#$(GTNETS_DIR)/application.cc 
#GTNETS_FILES = $(GTNETS_DIR)/application-udpsink.cc $(GTNETS_DIR)/link-real.cc
GTNETS_FILES = $(GTNETS_DIR)/wlan.cc \
$(GTNETS_DIR)/application.cc \
$(GTNETS_DIR)/link-real.cc \
$(GTNETS_DIR)/interface-real.cc \
$(GTNETS_DIR)/interface-wireless.cc \
$(GTNETS_DIR)/l2proto802.11.cc  \
$(GTNETS_DIR)/timer.cc \
$(GTNETS_DIR)/application-udpsink.cc \
$(GTNETS_DIR)/application-cbr.cc \
$(GTNETS_DIR)/simulator.cc

GTNETS_FLAG = -I$(GTNETS_DIR) -DNO_DEBUG


wlan:
	./valueGraph -c $(GTNETS_DIR)/wlan.cc $(GTNETS_FLAG)

application:
	./valueGraph -c $(GTNETS_DIR)/application.cc $(GTNETS_FLAG)

link-real:
	./valueGraph -c $(GTNETS_DIR)/link-real.cc $(GTNETS_FLAG)

interface-real:
	./valueGraph -c $(GTNETS_DIR)/interface-real.cc $(GTNETS_FLAG)

l2proto802:
	./valueGraph -c $(GTNETS_DIR)/l2proto802.11.cc $(GTNETS_FLAG)

timer:
	./valueGraph -c $(GTNETS_DIR)/timer.cc $(GTNETS_DIR)/l2proto802.11.cc $(GTNETS_FLAG)

udpsink:
	./valueGraph -c $(GTNETS_DIR)/application-udpsink.cc $(GTNETS_FLAG)

udp:
	./valueGraph -c $(GTNETS_DIR)/udp.cc $(GTNETS_FLAG)

cbr:
	./valueGraph -c $(GTNETS_DIR)/application-cbr.cc $(GTNETS_FLAG)

interface-wireless:
	./valueGraph -c $(GTNETS_DIR)/interface-wireless.cc $(GTNETS_FLAG)

simulator:
	./valueGraph -c $(GTNETS_DIR)/simulator.cc $(GTNETS_FLAG) 

gtnets:
	./valueGraph -c $(GTNETS_FILES) $(GTNETS_FLAG) 

