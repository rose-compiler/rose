include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 
AM_CPPFLAGS = 


bin_PROGRAMS = #tune
MANYCORE_LIB = $(top_srcdir)/projects/ManyCoreRuntime

path = $(top_builddir)/src/3rdPartyLibraries/POET/
debug = 
poet_lib = -L$(srcdir) -L$(top_srcdir)/src/3rdPartyLibraries/POET/lib
manycore_include = -I$(MANYCORE_LIB)


now:
	$(path)pcg $(poet_lib) $(debug) $(srcdir)/stencil-gen.pt < $(srcdir)/$(input).$(ext) > $(input)-new.$(ext)
	diff $(input)-new.$(ext) $(srcdir)/$(input)-save.$(ext)

# Liao, what is this? 1/16/2014
#compile:
#	$(CXX) $(input)-new.$(ext)

check-local:
	$(MAKE) now input=relax ext=cpp
	$(MAKE) now input=input ext=cpp
	$(MAKE) now input=testPermutations ext=C
	@echo "*******************************************************************************************"
	@echo "****** ROSE/projects/StencilManyCore: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************"

test: $(srcdir)/relax-main.cpp $(srcdir)/relax.cpp relax-new.cpp 
	rm -f optimized.cpp
	cat relax-new.cpp | $(SED) -e 's/high_level_relax/high_level_relax_optimized/' > optimized.cpp
	$(CXX) -g $(manycore_include) $(srcdir)/relax-main.cpp $(srcdir)/relax.cpp optimized.cpp  -o tune 
	bash -c 'for i in `seq 12 20` ; do \
		for c in  `seq 2 5` ; do \
		./tune $$i $$i $$i $$c $$c 1 ; \
	done ; \
	done'

clean:
	rm -f tune
	rm -f *.o
	rm -f optimized.cpp *-new.*



EXTRA_DIST = $(srcdir)/relax.cpp $(srcdir)/relax-main.cpp $(srcdir)/relax-save.cpp \
             $(srcdir)/input.cpp $(srcdir)/input-save.cpp \
             $(srcdir)/testPermutations.C $(srcdir)/testPermutations-save.C \
             $(srcdir)/input.code $(srcdir)/output.code \
             $(srcdir)/stencil_lib.pi $(srcdir)/stencil_lib.pt \
             $(srcdir)/stencil-gen.pt 

CLEANFILES = 

include_HEADERS =

