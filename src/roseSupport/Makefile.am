include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .


# The stringify.h and stringify.C files contain functions for converting an enum constant into a string. We once had
# this configured to rebuild these two files for every fresh build, but there's no good reason that end users need to
# run this command when developers can do it instead.  On a reasonable machine it takes around a minute; on a misconfigured
# Ubuntu 14.04 it takes >12 hours.
.PHONY: rebuild-stringify
rebuild-stringify:
	$(AM_V_GEN) $(top_srcdir)/scripts/stringify.pl		\
	    --generic --header --output=$(srcdir)/stringify.C	\
	    $(top_srcdir)/src ../frontend/SageIII

noinst_LTLIBRARIES = libroseSupport.la

libroseSupport_la_SOURCES =			\
	transformationSupport.C			\
	optionDeclaration.C			\
	sourceLocationInheritedAttribute.C	\
	rangemap.C				\
	roseTranslators.C			\
	utility_functions.C			\
	memoryUsage.C				\
	threadSupport.C				\
	IncludeDirective.C			\
	SqlDatabase.C				\
	Diagnostics.C				\
	stringify.C

pkginclude_HEADERS =				\
	callbacks.h				\
	transformationSupport.h			\
	optionDeclaration.h			\
	sourceLocationInheritedAttribute.h	\
	rangemap.h				\
	roseTranslators.h			\
	stringify.h				\
	utility_functions.h			\
	threadSupport.h				\
	IncludeDirective.h			\
	SqlDatabase.h				\
	Diagnostics.h

# DQ (10/11/2007): This used to be part of the template instationation mechanism, but it was 
# based on nm and was not robust.  Instead we instantiate all templates and figure out which 
# are truely required.
# templateSupport.h

clean-local:
	rm -rf Templates.DB ii_files ti_files

distclean-local:
	rm -rf Templates.DB









EXTRA_DIST = CMakeLists.txt
