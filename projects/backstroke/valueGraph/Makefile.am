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
libBackstrokeFunctionReverser_a_LIBADD = 

noinst_PROGRAMS= functionReverserTest
functionReverserTest_SOURCES = test.C
functionReverserTest_LDADD = libBackstrokeFunctionReverser.a \
		$(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.a \
		$(top_builddir)/projects/backstroke/normalizations/libBackstrokeNormalizations.a \
		$(LIBS_WITH_RPATH) $(ROSE_LIBS)


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
GTNETS_FILES = $(GTNETS_DIR)/RngStream.cc \
		$(GTNETS_DIR)/countermalice.cc \
		$(GTNETS_DIR)/image-Taclane.cc \
		$(GTNETS_DIR)/nixvector.cc \
		$(GTNETS_DIR)/scheduler-map.cc \
		$(GTNETS_DIR)/aiff.cc \
		$(GTNETS_DIR)/cq.cc \
		$(GTNETS_DIR)/image-UAV.cc \
		$(GTNETS_DIR)/node-blue-impl.cc \
		$(GTNETS_DIR)/scheduler.cc \
		$(GTNETS_DIR)/aodv-queue.cc \
		$(GTNETS_DIR)/datapdu.cc \
		$(GTNETS_DIR)/image-VoipPhone.cc \
		$(GTNETS_DIR)/node-blue.cc \
		$(GTNETS_DIR)/security_association.cc \
		$(GTNETS_DIR)/appAlert.cc \
		$(GTNETS_DIR)/daw.cc \
		$(GTNETS_DIR)/image-gb_taclane.cc \
		$(GTNETS_DIR)/node-firstbit-impl.cc \
		$(GTNETS_DIR)/seq.cc \
		$(GTNETS_DIR)/application-aodv.cc \
		$(GTNETS_DIR)/diffserv-queue.cc \
		$(GTNETS_DIR)/image-rb_taclane.cc \
		$(GTNETS_DIR)/node-firstbit.cc \
		$(GTNETS_DIR)/serializable.cc \
		$(GTNETS_DIR)/application-blue.cc \
		$(GTNETS_DIR)/distributed-simulator.cc \
		$(GTNETS_DIR)/image-rg_taclane.cc \
		$(GTNETS_DIR)/node-ghost.cc \
		$(GTNETS_DIR)/sha1.cc \
		$(GTNETS_DIR)/application-cbr.cc \
		$(GTNETS_DIR)/droppdu.cc \
		$(GTNETS_DIR)/interface-basic.cc \
		$(GTNETS_DIR)/node-impl.cc \
		$(GTNETS_DIR)/simulator.cc \
		$(GTNETS_DIR)/application-chord.cc \
		$(GTNETS_DIR)/droptail-filter.cc \
		$(GTNETS_DIR)/interface-ethernet.cc \
		$(GTNETS_DIR)/node-interconnect.cc \
		$(GTNETS_DIR)/star.cc \
		$(GTNETS_DIR)/application-gnutella.cc \
		$(GTNETS_DIR)/droptail-ipa-retx.cc \
		$(GTNETS_DIR)/interface-ghost.cc \
		$(GTNETS_DIR)/node-real.cc \
		$(GTNETS_DIR)/supercomputer-interconnect.cc \
		$(GTNETS_DIR)/application-kneighlev.cc \
		$(GTNETS_DIR)/droptail-ipa.cc \
		$(GTNETS_DIR)/interface-interconnect-remote.cc \
		$(GTNETS_DIR)/node-satellite-impl.cc \
		$(GTNETS_DIR)/tcp-newreno.cc \
		$(GTNETS_DIR)/application-mpi.cc \
		$(GTNETS_DIR)/droptail.cc \
		$(GTNETS_DIR)/interface-interconnect.cc \
		$(GTNETS_DIR)/node-satellite.cc \
		$(GTNETS_DIR)/tcp-reno.cc \
		$(GTNETS_DIR)/application-onoff.cc \
		$(GTNETS_DIR)/dualqueue.cc \
		$(GTNETS_DIR)/interface-link16.cc \
		$(GTNETS_DIR)/node-taclane-impl.cc \
		$(GTNETS_DIR)/tcp-tahoe.cc \
		$(GTNETS_DIR)/application-ospf.cc \
		$(GTNETS_DIR)/dumbbell.cc \
		$(GTNETS_DIR)/interface-real.cc \
		$(GTNETS_DIR)/node-taclane.cc \
		$(GTNETS_DIR)/tcp.cc \
		$(GTNETS_DIR)/application-ping.cc \
		$(GTNETS_DIR)/duplexlink.cc \
		$(GTNETS_DIR)/interface-wireless.cc \
		$(GTNETS_DIR)/node.cc \
		$(GTNETS_DIR)/tcpdemux.cc \
		$(GTNETS_DIR)/application-routing.cc \
		$(GTNETS_DIR)/eigrp.cc \
		$(GTNETS_DIR)/interface.cc \
		$(GTNETS_DIR)/object.cc \
		$(GTNETS_DIR)/tfstream.cc \
		$(GTNETS_DIR)/application-routingstatic.cc \
		$(GTNETS_DIR)/encryption.cc \
		$(GTNETS_DIR)/ipaddr.cc \
		$(GTNETS_DIR)/ospf.cc \
		$(GTNETS_DIR)/time-value-graph.cc \
		$(GTNETS_DIR)/application-synflood.cc \
		$(GTNETS_DIR)/ethernet.cc \
		$(GTNETS_DIR)/ipv4.cc \
		$(GTNETS_DIR)/packet-callbacks.cc \
		$(GTNETS_DIR)/time-value.cc \
		$(GTNETS_DIR)/application-tcp.cc \
		$(GTNETS_DIR)/eventcq.cc \
		$(GTNETS_DIR)/ipv6addr.cc \
		$(GTNETS_DIR)/packet.cc \
		$(GTNETS_DIR)/timer.cc \
		$(GTNETS_DIR)/application-tcpreceive.cc \
		$(GTNETS_DIR)/ftp-client.cc \
		$(GTNETS_DIR)/l2cap.cc \
		$(GTNETS_DIR)/peachqueue.cc \
		$(GTNETS_DIR)/timerbuckets.cc \
		$(GTNETS_DIR)/application-tcpsend.cc \
		$(GTNETS_DIR)/globalstats.cc \
		$(GTNETS_DIR)/l2proto-interconnect.cc \
		$(GTNETS_DIR)/portdemux.cc \
		$(GTNETS_DIR)/trace.cc \
		$(GTNETS_DIR)/application-tcpserver.cc \
		$(GTNETS_DIR)/grid-location.cc \
		$(GTNETS_DIR)/l2proto-satellite.cc \
		$(GTNETS_DIR)/portmatching.cc \
		$(GTNETS_DIR)/tree.cc \
		$(GTNETS_DIR)/application-traceroute.cc \
		$(GTNETS_DIR)/grid.cc \
		$(GTNETS_DIR)/l2proto.cc \
		$(GTNETS_DIR)/priqueue.cc \
		$(GTNETS_DIR)/ua-law.cc \
		$(GTNETS_DIR)/application-udpsink.cc \
		$(GTNETS_DIR)/handler.cc \
		$(GTNETS_DIR)/l2proto802.11.cc \
		$(GTNETS_DIR)/propagation.cc \
		$(GTNETS_DIR)/udp.cc \
		$(GTNETS_DIR)/application-udpstorm.cc \
		$(GTNETS_DIR)/hex.cc \
		$(GTNETS_DIR)/l2proto802.3.cc \
		$(GTNETS_DIR)/protograph.cc \
		$(GTNETS_DIR)/udpAlert.cc \
		$(GTNETS_DIR)/application-voip.cc \
		$(GTNETS_DIR)/histogram.cc \
		$(GTNETS_DIR)/l3protompi.cc \
		$(GTNETS_DIR)/qtchildwindow.cc \
		$(GTNETS_DIR)/udpdemux.cc \
		$(GTNETS_DIR)/application-webbrowser.cc \
		$(GTNETS_DIR)/histograph.cc \
		$(GTNETS_DIR)/l3prototaclane.cc \
		$(GTNETS_DIR)/qtwindow.cc \
		$(GTNETS_DIR)/validation.cc \
		$(GTNETS_DIR)/application-webserver.cc \
		$(GTNETS_DIR)/http-distributions.cc \
		$(GTNETS_DIR)/l4protocol.cc \
		$(GTNETS_DIR)/queue.cc \
		$(GTNETS_DIR)/virusthrottle.cc \
		$(GTNETS_DIR)/application-worm.cc \
		$(GTNETS_DIR)/icmp.cc \
		$(GTNETS_DIR)/link-ghost.cc \
		$(GTNETS_DIR)/randomtree.cc \
		$(GTNETS_DIR)/wireless-grid-manual.cc \
		$(GTNETS_DIR)/application.cc \
		$(GTNETS_DIR)/image-Aircraft.cc \
		$(GTNETS_DIR)/link-real.cc \
		$(GTNETS_DIR)/ratetimeparse.cc \
		$(GTNETS_DIR)/wireless-grid-polar.cc \
		$(GTNETS_DIR)/args.cc \
		$(GTNETS_DIR)/image-BlackRouter.cc \
		$(GTNETS_DIR)/link-rti.cc \
		$(GTNETS_DIR)/red.cc \
		$(GTNETS_DIR)/wireless-grid-rectangular.cc \
		$(GTNETS_DIR)/arp.cc \
		$(GTNETS_DIR)/image-ClientPC.cc \
		$(GTNETS_DIR)/link.cc \
		$(GTNETS_DIR)/ring.cc \
		$(GTNETS_DIR)/wireless-grid.cc \
		$(GTNETS_DIR)/average-min-max.cc \
		$(GTNETS_DIR)/image-DellDesktop.cc \
		$(GTNETS_DIR)/link16.cc \
		$(GTNETS_DIR)/rng.cc \
		$(GTNETS_DIR)/wlan.cc \
		$(GTNETS_DIR)/backplane.cc \
		$(GTNETS_DIR)/image-GreenRouter.cc \
		$(GTNETS_DIR)/linkmonitor.cc \
		$(GTNETS_DIR)/routing-aodv.cc \
		$(GTNETS_DIR)/worm-udp-cflash.cc \
		$(GTNETS_DIR)/baseband.cc \
		$(GTNETS_DIR)/image-Host.cc \
		$(GTNETS_DIR)/linkp2p.cc \
		$(GTNETS_DIR)/routing-dsr-options.cc \
		$(GTNETS_DIR)/worm-udp-flash.cc \
		$(GTNETS_DIR)/bfs.cc \
		$(GTNETS_DIR)/image-HostSmall.cc \
		$(GTNETS_DIR)/llcsnap.cc \
		$(GTNETS_DIR)/routing-dsr.cc \
		$(GTNETS_DIR)/worm.cc \
		$(GTNETS_DIR)/bitmap.cc \
		$(GTNETS_DIR)/image-RedRouter.cc \
		$(GTNETS_DIR)/lmp.cc \
		$(GTNETS_DIR)/routing-eigrp.cc \
		$(GTNETS_DIR)/wormhelper.cc \
		$(GTNETS_DIR)/bloom-filter.cc \
		$(GTNETS_DIR)/image-Router.cc \
		$(GTNETS_DIR)/macaddr.cc \
		$(GTNETS_DIR)/routing-manual.cc \
		$(GTNETS_DIR)/wormtargetvector.cc \
		$(GTNETS_DIR)/bnep.cc \
		$(GTNETS_DIR)/image-SatGroundLeft.cc \
		$(GTNETS_DIR)/mask.cc \
		$(GTNETS_DIR)/routing-nixvector.cc \
		$(GTNETS_DIR)/wormtcp.cc \
		$(GTNETS_DIR)/butterfly.cc \
		$(GTNETS_DIR)/image-SatGroundRight.cc \
		$(GTNETS_DIR)/memreuse.cc \
		$(GTNETS_DIR)/routing-nvr.cc \
		$(GTNETS_DIR)/wormudp.cc \
		$(GTNETS_DIR)/chord-stats.cc \
		$(GTNETS_DIR)/image-Satellite.cc \
		$(GTNETS_DIR)/mobility-random-waypoint.cc \
		$(GTNETS_DIR)/routing-static.cc \
		$(GTNETS_DIR)/chord-vis.cc \
		$(GTNETS_DIR)/image-Ship.cc \
		$(GTNETS_DIR)/mobility-specific-waypoint.cc \
		$(GTNETS_DIR)/routing.cc \
		$(GTNETS_DIR)/clique.cc \
		$(GTNETS_DIR)/image-ShipLarge.cc \
		$(GTNETS_DIR)/mobility.cc \
		$(GTNETS_DIR)/rtt-estimator.cc
	

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
		$(GTNETS_DIR)/interface-ghost.cc \
		$(GTNETS_DIR)/interface-ethernet.cc \
		$(GTNETS_FLAG)

wlan2:
	./functionReverserTest -c \
		$(GTNETS_DIR)/l2proto.cc \
		$(GTNETS_DIR)/l4protocol.cc \
		$(GTNETS_DIR)/l2proto802.11.cc \
		$(GTNETS_DIR)/l2proto802.3.cc \
		$(GTNETS_FLAG)


wlan3:
	./functionReverserTest -c \
		$(GTNETS_DIR)/timer.cc \
		$(GTNETS_DIR)/timerbuckets.cc \
		$(GTNETS_DIR)/node.cc \
		$(GTNETS_DIR)/node-real.cc \
		$(GTNETS_DIR)/node-impl.cc \
		$(GTNETS_DIR)/node-ghost.cc \
		$(GTNETS_FLAG)

wlan4:
	./functionReverserTest -c \
		$(GTNETS_DIR)/tcp.cc \
		$(GTNETS_FLAG)

wlan5:
	./functionReverserTest -c \
		$(GTNETS_DIR)/rng.cc \
		$(GTNETS_DIR)/queue.cc \
		$(GTNETS_DIR)/rtt-estimator.cc \
		$(GTNETS_DIR)/simulator.cc \
		$(GTNETS_DIR)/trace.cc \
		$(GTNETS_DIR)/datapdu.cc \
		$(GTNETS_FLAG)

wlan6:
	./functionReverserTest -c \
		$(GTNETS_DIR)/routing-aodv.cc \
		$(GTNETS_DIR)/routing-nixvector.cc \
		$(GTNETS_DIR)/routing-static.cc \
		$(GTNETS_FLAG)

wlan7:
	./functionReverserTest -c \
		$(GTNETS_DIR)/RngStream.cc \
		$(GTNETS_DIR)/tfstream.cc \
		$(GTNETS_DIR)/packet.cc \
		$(GTNETS_DIR)/routing.cc \
		$(GTNETS_FLAG)

wlan8:
	./functionReverserTest -c \
		$(GTNETS_DIR)/udp.cc \
		$(GTNETS_DIR)/link-rti.cc \
		$(GTNETS_DIR)/droptail.cc \
		$(GTNETS_DIR)/portdemux.cc \
		$(GTNETS_DIR)/linkp2p.cc \
		$(GTNETS_DIR)/mobility.cc \
		$(GTNETS_FLAG)

wlan9:
	./functionReverserTest -c \
		$(GTNETS_DIR)/ipv4.cc \
		$(GTNETS_DIR)/packet-callbacks.cc \
		$(GTNETS_DIR)/routing-nvr.cc \
		$(GTNETS_DIR)/routing-dsr.cc \
		$(GTNETS_FLAG)

wlan10:
	./functionReverserTest -c \
		$(GTNETS_DIR)/application.cc \
		$(GTNETS_DIR)/application-tcpsend.cc \
		$(GTNETS_DIR)/application-cbr.cc \
		$(GTNETS_DIR)/protograph.cc \
		$(GTNETS_DIR)/macaddr.cc \
		$(GTNETS_DIR)/ethernet.cc \
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
		$(GTNETS_DIR)/interface-ghost.cc \
		$(GTNETS_DIR)/interface-ethernet.cc \
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
		$(GTNETS_DIR)/macaddr.cc \
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
		$(GTNETS_DIR)/routing-nvr.cc \
		$(GTNETS_DIR)/routing-static.cc \
		$(GTNETS_DIR)/routing-nixvector.cc \
		$(GTNETS_DIR)/routing-dsr.cc \
		$(GTNETS_DIR)/udp.cc \
		$(GTNETS_DIR)/ethernet.cc \
		$(GTNETS_DIR)/link-rti.cc \
		$(GTNETS_DIR)/droptail.cc \
		$(GTNETS_DIR)/portdemux.cc \
		$(GTNETS_DIR)/linkp2p.cc \
		$(GTNETS_DIR)/routing-aodv.cc \
		$(GTNETS_DIR)/mobility.cc \
		$(GTNETS_DIR)/protograph.cc \
		$(GTNETS_DIR)/application.cc \
		$(GTNETS_DIR)/application-tcpsend.cc \
		$(GTNETS_DIR)/application-cbr.cc \
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


