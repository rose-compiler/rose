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
     memoryUsage.C

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
   utility_functions.h

# DQ (10/11/2007): This used to be part of the template instationation mechanism, but it was 
# based on nm and was not robust.  Instead we instantiate all templates and figure out which 
# are truely required.
# templateSupport.h

clean-local:
	rm -rf Templates.DB ii_files ti_files

distclean-local:
	rm -rf Templates.DB









EXTRA_DIST = CMakeLists.txt
