include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = -I$(srcdir)/../compassSupport $(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = libStaticConstructorInitialization.la
else
noinst_LTLIBRARIES = libStaticConstructorInitialization.la
endif

libStaticConstructorInitialization_la_SOURCES = staticConstructorInitialization.C

bin_PROGRAMS = staticConstructorInitializationMain

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) ../compassSupport/libCompass.la libStaticConstructorInitialization.la

staticConstructorInitializationMain_SOURCES = libStaticConstructorInitialization.la staticConstructorInitializationMain.C
staticConstructorInitializationMain_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH) libStaticConstructorInitialization.la

test: libStaticConstructorInitialization.la staticConstructorInitializationMain 
	env COMPASS_PARAMETERS=$(srcdir)/compass_parameters ./staticConstructorInitializationMain $(srcdir)/../tests/staticConstructorInitializationTest.C

check-local:
	@$(MAKE) test
	@echo "*********************************************************************************************************************"
	@echo "*** ROSE/projects/compass/staticConstructorInitialization: make check rule complete (terminated normally) ***"
	@echo "*********************************************************************************************************************"

EXTRA_DIST = compass_parameters staticConstructorInitialization.h 

clean-local:
	rm -f *.ti staticConstructorInitializationTest

