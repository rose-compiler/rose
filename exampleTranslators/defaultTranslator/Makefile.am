include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = defaultTranslator preprocessingInfoDumper mangledNameDumper qualifiedNameDumper

defaultTranslator_SOURCES = defaultTranslator.C 
preprocessingInfoDumper_SOURCES = preprocessingInfoDumper.C
mangledNameDumper_SOURCES = mangledNameDumper.C
qualifiedNameDumper_SOURCES = qualifiedNameDumper.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = CMakeLists.txt  test_mangeldName.c test_qualifiedName.cpp
CLEANFILES = 

check-local:mangledNameDumper qualifiedNameDumper
	./mangledNameDumper -c $(srcdir)/test_mangeldName.c
	./qualifiedNameDumper -c $(srcdir)/test_qualifiedName.cpp
clean-local:
	rm -rf Templates.DB ii_files ti_files

