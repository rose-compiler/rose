# This Makefile is responsible for building the libsatiredfa library that
# contains pre-built versions of some of the PAG data-flow analyzers that
# come with SATIrE. These analyzers become part of libsatire and can be
# called easily from programs using SATIrE.

# These are the analyzers we want to put into libsatiredfa. Add the analyzer
# name to this line, ignore the rest of this file, and try it out. If
# something doesn't work, you can still come back and mess around with the
# stuff below.
ANALYZERS = constprop sl2rd interval


# ----- no need to look beyond this line -----

# See if there is really anything to build.
if WITH_PAG
X_ANALYZERS = $(ANALYZERS)
else
X_ANALYZERS =
endif

# This is what we're producing here.
lib_LTLIBRARIES = libsatiredfa.la
nobase_include_HEADERS = \
	satire/satiredfa.h \
	$(foreach a, $(X_ANALYZERS), satire/$(a)_implementation.h)

# Explicit rule to override default (which assumes a certain C source file).
libsatiredfa_la_SOURCES = satiredfa.C

# All the objects and archives we want to go into libsatiredfa, based on
# $(ANALYZERS) specified above.
libsatiredfa_la_LIBADD = \
	$(foreach a, $(X_ANALYZERS), $(a)/$(a).a $(a)/$(a)_implementation.o)

# Configure the newanalysis script and Makefile template for use during the
# build process.
do_local_subst = \
	sed -e 's,[@]pkgdatadir[@],$(realpath .)/local_satire_skeletons,g' \
        -e 's,[@]bindir[@],$(bindir),g' \
        -e 's,[@]pagdir[@],$(PAGDIR),g' \
        -e 's,[@]roseinclude[@],$(ROSE_INCLUDE_DIR),g' \
        -e 's,[@]roselib[@],$(ROSE_LIB_DIR),g' \
        -e 's,[@]boostinclude[@],$(BOOST_INCLUDE_DIR),g' \
        -e 's,[@]boostlib[@],$(BOOST_LIB_DIR),g' \
        -e 's,[@]plinclude[@],$(SWI_PROLOG_INCLUDE_DIR),g' \
        -e 's|[@]plldadd[@]|$(SWI_PROLOG_LDADD)|g' \
        -e 's,[@]satireinclude[@],$(realpath .)/local_include_dir,g' \
        -e 's,[@]satirelib[@],$(realpath $(top_builddir))/src/analyzer/astaccess/satire/.libs,g' \
        -e 's,[@]cc[@],$(CC),g' \
        -e 's,[@]cxx[@],$(CXX),g'

# We manually instantiate newanalysis.in and Makefile-template.in according
# to our needs here; also, we copy all the SATIrE headers, and some other
# files to include, to a local directory here. Analyzer objects are built
# with -fPIC so we can put them into a shared library.
local_newanalysis_script: $(top_srcdir)/src/analyzer/skeletons/*
	rm -rf local_satire_skeletons
	cp -r $(top_srcdir)/src/analyzer/skeletons/ local_satire_skeletons
	chmod -R +w local_satire_skeletons
	$(do_local_subst) < local_satire_skeletons/newanalysis.in \
	                  > local_newanalysis_script
	$(do_local_subst) < local_satire_skeletons/Makefile-template.in \
	                  > local_satire_skeletons/Makefile-template
	echo "CUSTOM_CFLAGS = $(CUSTOM_CFLAGS) -fPIC" >> local_satire_skeletons/Makefile.custom
	chmod +x local_newanalysis_script
	mkdir -p local_include_dir
	chmod -R +w local_include_dir
	find $(top_srcdir)/src -wholename '*/satire/*.h' -exec cp '{}' ./local_include_dir \;
	d=$(top_srcdir)/src/analyzer/astaccess/satire ; \
	cp $$d/syn $$d/edges $$d/pagoptions* local_include_dir
	cp $(top_srcdir)/src/analyzer/commandlineinterface/satire/attributes \
	   local_include_dir
	chmod -R +w local_include_dir
	cp $(top_builddir)/src/analyzer/astaccess/satire/syntree.[ch] \
	   local_include_dir
	cp $(top_builddir)/src/termite/satire/termite.h \
	   local_include_dir

# Build an analyzer directory for each analysis and initialize it using the
# newanalysis script.
$(X_ANALYZERS): local_newanalysis_script $(top_srcdir)/examples/*/* $(top_srcdir)/tools/*/*
	@echo "----- making $@ -----"
	if [ -d $(top_srcdir)/examples/$@ ]; then \
		cp -r $(top_srcdir)/examples/$@ . ; \
	else \
		cp -r $(top_srcdir)/tools/$@ . ; \
	fi
	chmod -R +w $@
	prefix=`grep "prefix:" $@/$@.optla | awk '{ print $$2 }'` ; \
	./local_newanalysis_script -p $$prefix $@
	cat local_satire_skeletons/Makefile.custom >> $@/Makefile.custom

# Each foo/foo.a depends on the foo directory; each foo/foo_implementation.o
# depends on foo/foo.a (because building foo.a calls PAG and thus also
# generates the headers needed to build foo_implementation.o). Note that we
# can build these objects, but we would not be able to link a complete
# executable, as the SATIrE libraries do not exist yet.
# The .SECONDEXPANSION feature of GNU make allows us to use quoted special
# variables in the target list -- expanding these twice gives the desired
# result.
.SECONDEXPANSION:
$(foreach a, $(X_ANALYZERS), $(a)/$(a).a): $$(@D)
$(foreach a, $(X_ANALYZERS), $(a)/$(a)_implementation.o): $$(@D)/$$(@D).a

# The analyzer archive foo/foo.a is built by the default Makefile generated
# by SATIrE. That archive also contains a copy of syntree.o; however, since
# syntree.o is already contained in libsatire, we delete it from the
# archive.
$(foreach a, $(X_ANALYZERS), $(a)/$(a).a):
	$(MAKE) -C $(@D) $(@F)
	ar d $@ syntree.o

# The analyzer class implementation foo/foo_implementation.o is also built
# by calling the corresponding rule in the Makefile generated by SATIrE.
$(foreach a, $(X_ANALYZERS), $(a)/$(a)_implementation.o):
	$(MAKE) -C $(@D) $(@F)

# Each analyzer's foo_implementation.h header is generated by newanalysis
# when the analyzer directory is created above. In satiredfa.h, we collect
# include directives for each of these analyzers to make them easy to
# include elsewhere.
$(foreach a, $(X_ANALYZERS), satire/$(a)_implementation.h): $(X_ANALYZERS)

satire/satiredfa.h: $(foreach a, $(X_ANALYZERS), satire/$(a)_implementation.h)
	mkdir -p satire
	chmod -R +w satire
	rm -f satire/satiredfa.h
	touch satire/satiredfa.h
	for a in $(X_ANALYZERS); do \
		printf "#include \"%s_implementation.h\"\n" $$a >> satire/satiredfa.h; \
		cp `printf %s/%s_implementation.h $$a $$a` satire; \
	done

INCLUDES = -I$(top_srcdir)/src/satire \
       -I$(top_srcdir)/src/termite/satire \
       -I$(top_builddir)/src/termite/satire \
       -I$(top_srcdir)/src/analyzer/annotation/satire \
       -I$(top_srcdir)/src/analyzer/icfgbuilder/satire \
       -I$(top_srcdir)/src/analyzer/astaccess \
       -I$(top_srcdir)/src/analyzer/astaccess/satire \
       -I$(top_srcdir)/src/analyzer/astnumbering/satire \
       -I$(top_srcdir)/src/analyzer/annotation/satire \
       -I$(top_srcdir)/src/analyzer/commandlineinterface/satire \
       -I$(top_srcdir)/src/analyzer/support \
       -I$(top_srcdir)/src/analyzer/support/satire \
	   -I$(top_srcdir)/src/analyzer/provided/pointsto/satire \
       -I$(ROSE_INCLUDE_DIR) \
       -I$(BOOST_INCLUDE_DIR)

if WITH_PAG
INCLUDES += -I$(PAG_INCLUDE_DIR)/pag \
            -I$(PAG_INCLUDE_DIR)/pag/analyzer
endif

if WITH_SWI_PROLOG
INCLUDES += -I$(SWI_PROLOG_INCLUDE_DIR)
endif

satiredfa.C: satire/satiredfa.h
	rm -f satiredfa.C
	touch satiredfa.C
	echo "#include <satire.h>" >> satiredfa.C
	echo "#include \"satire/satiredfa.h\"" >> satiredfa.C
	echo "namespace SATIrE {" >> satiredfa.C
	echo "const char *provided_analyzers[] = {" >> satiredfa.C
	for a in $(X_ANALYZERS); do \
		echo "    \"$$a\"," >> satiredfa.C; \
	done
	echo "    (const char *) 0" >> satiredfa.C
	echo "};" >> satiredfa.C
	echo "DataFlowAnalysis* makeProvidedAnalyzer(const char *name) {" \
		>> satiredfa.C
	echo "    std::string analyzer = name;" >> satiredfa.C
	for a in $(X_ANALYZERS); do \
		echo "    if (analyzer == \"$$a\")" >> satiredfa.C; \
		echo "        return new DataFlowAnalysis(new $$a::Implementation());" \
			>> satiredfa.C; \
	done
	echo "    return NULL;" >> satiredfa.C
	echo "}" >> satiredfa.C
	echo "}" >> satiredfa.C

clean-local:
	rm -rf $(X_ANALYZERS)
	rm -rf satire/
	rm -rf local_satire_skeletons local_include_dir local_newanalysis_script
	rm -rf satiredfa.C
