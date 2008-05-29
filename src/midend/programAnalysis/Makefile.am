include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (5/3/2007): Old version before Christian's work was added.
#SUBDIRS = CallGraphAnalysis OAWrap CFG dataflowAnalysis
#          valuePropagation  dominatorTreesAndDominanceFrontiers
#          staticProgramSlicing proceduralSlicing

SUBDIRS = CallGraphAnalysis OAWrap CFG dataflowAnalysis \
          valuePropagation  defUseAnalysis dominanceAnalysis \
          staticInterproceduralSlicing pointerAnal \
	  annotationLanguageParser sideEffectAnalysis

# DQ (9/6/2005): Commented out these directories since they
# represent code that used the older graph interface and they
# no longer compile!  These will be updated later.
# staticProgramSlicing proceduralSlicing

# DQ (10/21/2004): Take thse out for now since I can't compile them (missing language.tab.h which should be generated)

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

#dist-hook:
#	ln -s ../STL-link ${distdir}

# AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = OpenAnalysis





