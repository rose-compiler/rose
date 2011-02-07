include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# . is necessary to ensure building the translator before building the tests
SUBDIRS = . tests doc
ROSEHPCT_SRC_ROOT = $(top_srcdir)/src/roseExtensions/roseHPCToolkit
ROSEHPCT_BUILD_ROOT = $(top_builddir)/src/roseExtensions/roseHPCToolkit

# ROSEHPCT_DEP_INCLUDES contains include paths to libxml2
INCLUDES = $(ROSE_INCLUDES) \
	$(ROSEHPCT_DEP_INCLUDES) \
	-I$(ROSEHPCT_SRC_ROOT)/include

ROSEHPCT_LIBS = $(ROSEHPCT_BUILD_ROOT)/src/librosehpct.la \
                $(ROSE_LIBS) $(RT_LIBS)

# ROSEHPCT_DEP_LIBS: for libxml2 		
LDADD = $(ROSEHPCT_DEP_LIBS)

# this lib contains supporting functions for dlopen(), checkpointing, etc
# the transformation will generated calls to them and insert their header.
AM_CFLAGS=$(CXX_DEBUG)  #we share CXXs debug flags for C compiler 
lib_LIBRARIES = libautoTuning.a
libautoTuning_a_SOURCES = autotuning_lib.c
include_HEADERS = autotuning_lib.h uthash.h

EXTRA_DIST =  README makefile-external autoTuningSupport.h autotuning_lib.h uthash.h
#--------------------------------------
if ROSE_BUILD_ROSEHPCT

bin_PROGRAMS = autoTuning

autoTuning_SOURCES = autoTuning.C autoTuningSupport.C
autoTuning_LDFLAGS = $(ROSEHPCT_LIBS)

#noinst_HEADERS = autoTuningSupport.h

check-local: 
	@echo "**************************************************************************************"
	@echo "****** ROSE/projects/autoTuning: make check rule complete (terminated normally) ******"
	@echo "**************************************************************************************"

clean-local:
	rm -rf autoTuning
clean-local:
	rm -rf Templates.DB ii_files ti_files *.bin
	rm -f *.pdf
else #  !ROSE_BUILD_ROSEHPCT

EXTRA_DIST +=autoTuning.C autoTuningSupport.C 

endif # ROSE_BUILD_ROSEHPCT



