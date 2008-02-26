include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

all: exampleMakefile

bin_PROGRAMS = identityTranslator

identityTranslator_SOURCES = identityTranslator.C
identityTranslator_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#EXTRA_DIST = testCode.C AST-dot1.ps AST-pdf2.pdf
EXTRA_DIST = testCode.C AST-pdf2.pdf
CLEANFILES = 

clean-local:
	rm -rf Templates.DB ii_files ti_files exampleTranslator exampleTranslatorMultiLib rose_* a.out

# Compile everything with -g (compiler's debug flag)
# AM_CPPFLAGS = -g

installcheck:
	rm -f identityTranslator identityTranslator.o
	@echo "Testing build using installed libraries ..."
	env -u ROSE_IN_BUILD_TREE $(MAKE) -f exampleMakefile
	@echo "Testing executable built using installed libraries ..."
	env -u ROSE_IN_BUILD_TREE $(MAKE) -f exampleMakefile check
