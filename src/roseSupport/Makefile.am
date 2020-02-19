include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .


# The stringify.h and stringify.C files contain functions for converting an enum constant into a string. We once had
# this configured to rebuild these two files for every fresh build, but there's no good reason that end users need to
# run this command when developers can do it instead.  On a reasonable machine it takes around a minute; on a misconfigured
# Ubuntu 14.04 it takes >12 hours.
.PHONY: rebuild-stringify
rebuild-stringify:
	$(top_srcdir)/scripts/restringify $(top_srcdir) $(top_blddir)

noinst_LTLIBRARIES = libroseSupport.la

license.h: $(top_srcdir)/LicenseInformation/ROSE_BSD_License.txt
	perl $(top_srcdir)/scripts/to-c-string.pl < $< > $@

# All files mentiond in BUILT_SOURCES will be built before the normal compilation rules run.
BUILT_SOURCES = license.h

libroseSupport_la_SOURCES =			\
	Combinatorics.C				\
	CommandLine.C				\
	Diagnostics.C				\
	IncludeDirective.C			\
	SqlDatabase.C				\
	initialize.C				\
	memoryUsage.C				\
	optionDeclaration.C			\
	rangemap.C				\
	roseTranslators.C			\
	SourceLocation.C			\
	sourceLocationInheritedAttribute.C	\
	stringify.C				\
	transformationSupport.C			\
	utility_functions.C

pkginclude_HEADERS =				\
	Combinatorics.h				\
	CommandLine.h				\
	Diagnostics.h				\
	IncludeDirective.h			\
	RoseException.h				\
	SqlDatabase.h				\
	callbacks.h				\
	initialize.h				\
	optionDeclaration.h			\
	rangemap.h				\
	roseTranslators.h			\
	SourceLocation.h			\
	sourceLocationInheritedAttribute.h	\
	stringify.h				\
	transformationSupport.h			\
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
