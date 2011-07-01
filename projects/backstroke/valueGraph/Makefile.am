include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = . headerUnparser

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
#LINK	 = $(CXXLINK)
#LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


# noinst_LIBRARIES lists all the librarlies that should be built, but not installed
noinst_LIBRARIES = libBackstrokeFunctionReverser.a

libBackstrokeFunctionReverser_a_DEPENDENCIES =
libBackstrokeFunctionReverser_a_SOURCES = valueGraphNode.C valueGraph.C graphSearch.C functionReverser.C \
		pathNumGenerator.C CFGFilter.C codeGeneration.C types.C 
libBackstrokeFunctionReverser_a_includedir = $(includedir)/backstroke
libBackstrokeFunctionReverser_a_include_HEADERS = valueGraph.h valueGraphNode.h pathNumGenerator.h functionReverser.h \
			CFGFilter.h types.h
libBackstrokeFunctionReverser_a_LIBADD = $(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.a \
		$(top_builddir)/projects/backstroke/ssa/libSSA.a

noinst_PROGRAMS= functionReverserTest
functionReverserTest_SOURCES = test.C
functionReverserTest_LDADD =$(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.a \
		$(top_builddir)/projects/backstroke/normalizations/libBackstrokeNormalizations.a \
		$(top_builddir)/projects/backstroke/ssa/libSSA.a \
		libBackstrokeFunctionReverser.a $(LIBS_WITH_RPATH) $(ROSE_LIBS)


# EXTRA_DIST are files that are not compiled or installed. These include readme's, internal header files, etc.
EXTRA_DIST = 

CLEANFILES = 

check-local:
	@echo "*********************************************************************************************************"
	@echo "******* ROSE/projects/backstroke/functionReverser: make check rule complete (terminated normally) *******"
	@echo "*********************************************************************************************************"

	
test:
	./functionReverserTest -c temp.C -I/home/hou1/opt/boost_1_36_0//include/boost-1_36
#./functionReverserTest -c temp1.C -I/opt/local/var/macports/software/boost/1.44.0_0/opt/local/include/
#./functionReverserTest -c temp2.C -I/opt/local/var/macports/software/boost/1.44.0_0/opt/local/include/

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

GTNETS_FLAG = -I$(GTNETS_DIR) -I/Users/hou1/MyApp/MacPorts/include/ -DNO_DEBUG

wlan:
	./functionReverserTest -c \
		$(GTNETS_DIR)/wlan.cc \
		$(GTNETS_FLAG)

wlan1:
	./functionReverserTest -c \
		$(GTNETS_DIR)/wlan.cc \
		$(GTNETS_DIR)/arp.cc \
		$(GTNETS_DIR)/link-real.cc \
		$(GTNETS_DIR)/interface.cc \
		$(GTNETS_DIR)/interface-basic.cc \
		$(GTNETS_DIR)/interface-wireless.cc \
		$(GTNETS_DIR)/interface-real.cc \
		$(GTNETS_FLAG)

wlan2:
	./functionReverserTest -c \
		$(GTNETS_DIR)/l2proto.cc \
		$(GTNETS_DIR)/l4protocol.cc \
		$(GTNETS_DIR)/l2proto802.11.cc \
		$(GTNETS_DIR)/l2proto802.3.cc \
		$(GTNETS_DIR)/timer.cc \
		$(GTNETS_DIR)/timerbuckets.cc \
		$(GTNETS_DIR)/node.cc \
		$(GTNETS_DIR)/node-real.cc \
		$(GTNETS_FLAG)

wlan3:
	./functionReverserTest -c \
		$(GTNETS_DIR)/node-impl.cc \
		$(GTNETS_DIR)/node-ghost.cc \
		$(GTNETS_DIR)/tcp.cc \
		$(GTNETS_DIR)/rng.cc \
		$(GTNETS_DIR)/queue.cc \
		$(GTNETS_DIR)/rtt-estimator.cc \
		$(GTNETS_DIR)/simulator.cc \
		$(GTNETS_DIR)/trace.cc \
		$(GTNETS_DIR)/ipv4.cc \
		$(GTNETS_DIR)/datapdu.cc \
		$(GTNETS_DIR)/RngStream.cc \
		$(GTNETS_DIR)/tfstream.cc \
		$(GTNETS_DIR)/packet-callbacks.cc \
		$(GTNETS_DIR)/packet.cc \
		$(GTNETS_DIR)/routing.cc \
		$(GTNETS_FLAG)

wlan-all:
	./functionReverserTest -c \
		$(GTNETS_DIR)/wlan.cc \
		$(GTNETS_DIR)/arp.cc \
		$(GTNETS_DIR)/link-real.cc \
		$(GTNETS_DIR)/interface.cc \
		$(GTNETS_DIR)/interface-basic.cc \
		$(GTNETS_DIR)/interface-wireless.cc \
		$(GTNETS_DIR)/interface-real.cc \
		$(GTNETS_DIR)/l2proto.cc \
		$(GTNETS_DIR)/l4protocol.cc \
		$(GTNETS_DIR)/l2proto802.11.cc \
		$(GTNETS_DIR)/l2proto802.3.cc \
		$(GTNETS_DIR)/timer.cc \
		$(GTNETS_DIR)/timerbuckets.cc \
		$(GTNETS_DIR)/node.cc \
		$(GTNETS_DIR)/node-real.cc \
		$(GTNETS_DIR)/node-impl.cc \
		$(GTNETS_DIR)/node-ghost.cc \
		$(GTNETS_DIR)/tcp.cc \
		$(GTNETS_DIR)/rng.cc \
		$(GTNETS_DIR)/queue.cc \
		$(GTNETS_DIR)/rtt-estimator.cc \
		$(GTNETS_DIR)/simulator.cc \
		$(GTNETS_DIR)/trace.cc \
		$(GTNETS_DIR)/ipv4.cc \
		$(GTNETS_DIR)/datapdu.cc \
		$(GTNETS_DIR)/RngStream.cc \
		$(GTNETS_DIR)/tfstream.cc \
		$(GTNETS_DIR)/packet-callbacks.cc \
		$(GTNETS_DIR)/packet.cc \
		$(GTNETS_DIR)/routing.cc \
		$(GTNETS_FLAG)

application:
	./functionReverserTest -c $(GTNETS_DIR)/application.cc $(GTNETS_FLAG)

link-real:
	./functionReverserTest -c $(GTNETS_DIR)/link-real.cc $(GTNETS_FLAG)

interface-real:
	./functionReverserTest -c $(GTNETS_DIR)/interface-real.cc $(GTNETS_DIR)/queue.cc $(GTNETS_FLAG)

l2proto802:
	./functionReverserTest -c $(GTNETS_DIR)/l2proto802.11.cc $(GTNETS_FLAG)

timer:
	./functionReverserTest -c $(GTNETS_DIR)/timer.cc $(GTNETS_DIR)/l2proto802.11.cc $(GTNETS_FLAG)

udpsink:
	./functionReverserTest -c $(GTNETS_DIR)/application-udpsink.cc $(GTNETS_FLAG)

udp:
	./functionReverserTest -c $(GTNETS_DIR)/udp.cc $(GTNETS_FLAG)

cbr:
	./functionReverserTest -c $(GTNETS_DIR)/application-cbr.cc $(GTNETS_FLAG)

interface-wireless:
	./functionReverserTest -c $(GTNETS_DIR)/interface-wireless.cc $(GTNETS_FLAG)

simulator:
	./functionReverserTest -c $(GTNETS_DIR)/simulator.cc $(GTNETS_FLAG) 

gtnets:
	./functionReverserTest -c $(GTNETS_FILES) $(GTNETS_FLAG) 


