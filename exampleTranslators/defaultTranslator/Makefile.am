include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)

bin_PROGRAMS = defaultTranslator preprocessingInfoDumper mangledNameDumper qualifiedNameDumper KeepGoingTranslator

defaultTranslator_SOURCES = defaultTranslator.C 
preprocessingInfoDumper_SOURCES = preprocessingInfoDumper.C
mangledNameDumper_SOURCES = mangledNameDumper.C
qualifiedNameDumper_SOURCES = qualifiedNameDumper.C
KeepGoingTranslator_SOURCES = KeepGoingTranslator.cpp

LDADD = $(ROSE_LIBS)

EXTRA_DIST = CMakeLists.txt  test_mangeldName.c test_qualifiedName.cpp
CLEANFILES = 

check-local:mangledNameDumper qualifiedNameDumper
if ROSE_BUILD_C_LANGUAGE_SUPPORT
	./mangledNameDumper -c $(srcdir)/test_mangeldName.c
	./qualifiedNameDumper -c $(srcdir)/test_qualifiedName.cpp
endif

clean-local:
	rm -rf Templates.DB ii_files ti_files rose_*.*

