include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = -I$(srcdir)/../compassSupport $(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = libAllowedFunctions.la
else
noinst_LTLIBRARIES = libAllowedFunctions.la
endif

libAllowedFunctions_la_SOURCES = allowedFunctions.C

bin_PROGRAMS = allowedFunctionsMain

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) ../compassSupport/libCompass.la libAllowedFunctions.la

allowedFunctionsMain_SOURCES = libAllowedFunctions.la allowedFunctionsMain.C
allowedFunctionsMain_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH) libAllowedFunctions.la

test: libAllowedFunctions.la allowedFunctionsMain 
	cp -f $(srcdir)/compass_parameters compass_parameters
	./allowedFunctionsMain $(srcdir)/allowedFunctions/allowedFunctionsTest1.C

check-local:
	@$(MAKE) test
	@echo "*********************************************************************************************************************"
	@echo "*** ROSE/projects/compass/allowedFunctions: make check rule complete (terminated normally) ***"
	@echo "*********************************************************************************************************************"

EXTRA_DIST = compass_parameters allowedFunctions.h 

clean-local:
	rm -f compass_parameters *.ti allowedFunctionsTest

