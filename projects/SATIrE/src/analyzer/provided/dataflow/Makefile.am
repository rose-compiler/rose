# This Makefile is responsible for building the libDataFlow library that
# contains pre-built versions of some of the PAG data-flow analyzers that
# come with SATIrE. These analyzers become part of libsatire and can be
# called easily from programs using SATIrE.

# These are the analyzers we want to put into libDataFlow. Add the analyzer
# name to this line, ignore the rest of this file, and try it out. If
# something doesn't work, you can still come back and mess around with the
# stuff below.
ANALYZERS = constprop sl1rd


# ----- no need to look beyond this line -----

# This is what we're producing here.
noinst_LTLIBRARIES = libDataFlow.la

# Explicit rule to override default (which assumes a certain C source file).
libDataFlow_la_SOURCES =

# All the objects and archives we want to go into libDataFlow, based on
# $(ANALYZERS) specified above.
libDataFlow_la_LIBADD = \
	$(foreach a, $(ANALYZERS), $(a)/$(a).a $(a)/$(a)_implementation.o)

# Configure the newanalysis script and Makefile template for use during the
# build process.

do_local_subst = \
	sed -e 's,[@]pkgdatadir[@],$(realpath $(builddir))/local_satire_skeletons,g' \
        -e 's,[@]bindir[@],$(bindir),g' \
        -e 's,[@]pagdir[@],$(PAGDIR),g' \
        -e 's,[@]roseinclude[@],$(ROSE_INCLUDE_DIR),g' \
        -e 's,[@]roselib[@],$(ROSE_LIB_DIR),g' \
        -e 's,[@]boostinclude[@],$(BOOST_INCLUDE_DIR),g' \
        -e 's,[@]boostlib[@],$(BOOST_LIB_DIR),g' \
        -e 's,[@]plinclude[@],$(SWI_PROLOG_INCLUDE_DIR),g' \
        -e 's|[@]plldadd[@]|$(SWI_PROLOG_LDADD)|g' \
        -e 's,[@]satireinclude[@],$(realpath $(builddir))/local_include_dir,g' \
        -e 's,[@]satirelib[@],$(realpath $(top_builddir))/src/analyzer/astaccess/satire/.libs,g' \
        -e 's,[@]cxx[@],$(CXX),g'

local_newanalysis_script:
	cp -r $(top_srcdir)/src/analyzer/skeletons/ local_satire_skeletons
	$(do_local_subst) < local_satire_skeletons/newanalysis.in \
					  > local_newanalysis_script
	$(do_local_subst) < local_satire_skeletons/Makefile-template.in \
				 	  > local_satire_skeletons/Makefile-template
	echo "CUSTOM_CFLAGS = $(CUSTOM_CFLAGS) -fPIC" >> local_satire_skeletons/Makefile.custom
	chmod +x local_newanalysis_script
	:
	mkdir -p local_include_dir
# FIXME: only copy headers from satire/ directories!
	find $(top_srcdir)/src -name '*.h' -exec cp '{}' $(builddir)/local_include_dir \;
	d=$(top_srcdir)/src/analyzer/astaccess/satire ; \
	cp $$d/syn $$d/edges $$d/pagoptions* local_include_dir
	cp $(top_srcdir)/src/analyzer/commandlineinterface/satire/attributes \
	   local_include_dir
	cp $(top_builddir)/src/analyzer/astaccess/satire/syntree.[ch] \
	   local_include_dir
	cp $(top_builddir)/src/termite/satire/termite.h \
	   local_include_dir

# Each of the analyzers gets a section something like this. Note that we can
# build $(ANALYSIS).a here, but we cannot build a running executable because
# libsatire does not exist yet.
# FIXME: Fix this comment.

$(ANALYZERS): local_newanalysis_script
	@echo "----- making $@ -----"
	cp -r $(top_srcdir)/examples/$@ .
	prefix=`grep "prefix:" $@/$@.optla | awk '{ print $$2 }'` ; \
	./local_newanalysis_script -p $$prefix $@
	cat local_satire_skeletons/Makefile.custom >> $@/Makefile.custom

.SECONDEXPANSION:
$(foreach a, $(ANALYZERS), $(a)/$(a).a): $$(@D)
$(foreach a, $(ANALYZERS), $(a)/$(a)_implementation.o): $$(@D)/$$(@D).a

$(foreach a, $(ANALYZERS), $(a)/$(a).a):
	@echo "making $@, depends on $(@D)"
	$(MAKE) -C $(@D) $(@F)
	ar d $@ syntree.o

$(foreach a, $(ANALYZERS), $(a)/$(a)_implementation.o):
	@echo "@ = $@, @D = $(@D), @F = $(@F)"
	$(MAKE) -C $(@D) $(@F)

# TODO: clean rule!

#constprop: local_newanalysis_script
#	cp -r $(top_srcdir)/examples/constprop .
#	prefix=`grep "prefix:" constprop/constprop.optla | awk '{ print $$2 }'` ; \
#	./local_newanalysis_script -p $$prefix constprop
#	cat local_satire_skeletons/Makefile.custom >> constprop/Makefile.custom

#constprop/constprop.a: constprop
#	$(MAKE) -C constprop constprop.a
#	ar d constprop/constprop.a syntree.o
#	ar t constprop/constprop.a

#constprop/constprop_implementation.o: constprop/constprop.a
#	$(MAKE) -C constprop constprop_implementation.o
