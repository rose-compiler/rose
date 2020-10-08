include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)

bin_PROGRAMS = defaultTranslator preprocessingInfoDumper mangledNameDumper qualifiedNameDumper KeepGoingTranslator fileInfoDumper textASTGenerator

defaultTranslator_SOURCES = defaultTranslator.C 
preprocessingInfoDumper_SOURCES = preprocessingInfoDumper.C
mangledNameDumper_SOURCES = mangledNameDumper.C
qualifiedNameDumper_SOURCES = qualifiedNameDumper.C
KeepGoingTranslator_SOURCES = KeepGoingTranslator.cpp

fileInfoDumper_SOURCES= fileInfoDumper.C
textASTGenerator_SOURCES= textASTGenerator.C

LDADD = $(ROSE_LIBS)

EXTRA_DIST = CMakeLists.txt  test_mangeldName.c test_qualifiedName.cpp
CLEANFILES = 

check-local:mangledNameDumper qualifiedNameDumper
if ROSE_BUILD_C_LANGUAGE_SUPPORT
	./mangledNameDumper -c $(srcdir)/test_mangeldName.c &>log.txt
	./qualifiedNameDumper -c $(srcdir)/test_qualifiedName.cpp &> log.txt
	./textASTGenerator -c $(srcdir)/test_qualifiedName.cpp
endif

clean-local:
	rm -rf Templates.DB ii_files ti_files rose_*.* log.txt

