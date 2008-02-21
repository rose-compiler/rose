include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_BINARY_ANALYSIS_SUPPORT

SUBDIRS = analyses graphanalyses

INCLUDES = $(ROSE_INCLUDES) -I$(builddir)

bin_PROGRAMS = binCompassMain

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl

binCompassMain_SOURCES = binCompassMain.cpp 
binCompassMain_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)


binCompassMain.o: binCompassMain.cpp

clean:
	rm -f *.dot *.gml *.lo *.la *.s *.o
	rm -f analyses/*.la analyses/*.lo analyses/*.o graphanalyses/*.o graphanalyses/*.lo

endif
