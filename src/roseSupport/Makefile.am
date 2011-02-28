include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

libroseSupportSources = \
     transformationSupport.C \
     optionDeclaration.C \
     sourceLocationInheritedAttribute.C \
     roseTranslators.C \
     utility_functions.C \
     memoryUsage.C \
     stringify.C

# The stringify.h and stringify.C files contain functions for converting an enum constant into a string.
# This should probably also depend on all the source files that contain enum definitions.
stringify.h: stringify.C
stringify.C: $(top_srcdir)/scripts/stringify.pl
	@echo "  GEN     $@"
	@$(top_srcdir)/scripts/stringify.pl --generic --header --output=$@ $(top_srcdir)/src ../frontend/SageIII


# DQ (10/11/2007): This used to be part of the template instationation mechanism, but it was 
# based on nm and was not robust.  Instead we instantiate all templates and figure out which 
# are truely required.
# templateSupport.C

# lib_LIBRARIES = libroseSupport.a
# libroseSupport_a_SOURCES  = $(libroseSupportSources)

noinst_LTLIBRARIES = libroseSupport.la

libroseSupport_la_SOURCES = $(libroseSupportSources)

include_HEADERS = \
   transformationSupport.h \
   optionDeclaration.h \
   sourceLocationInheritedAttribute.h \
   roseTranslators.h \
   utility_functions.h \
   stringify.h \
   threadSupport.h

# DQ (10/11/2007): This used to be part of the template instationation mechanism, but it was 
# based on nm and was not robust.  Instead we instantiate all templates and figure out which 
# are truely required.
# templateSupport.h

clean-local:
	rm -rf Templates.DB ii_files ti_files stringify.C stringify.h

distclean-local:
	rm -rf Templates.DB









EXTRA_DIST = CMakeLists.txt
