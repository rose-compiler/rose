SUBDIRS = analysis \
         arrIndexLabeler \
         cfgUtils \
         chkptRangeAnalysis \
         common \
         external \
         lattice \
         mpiAnal \
         ompAnal \
         rwAccessLabeler \
         simpleAnalyses \
         state \
         unionFind \
         varBitVector \
         variables \
         varLatticeVector 
# there are inter dependencies among sub directories, parallel build is a bad idea for now
#.NOTPARALLEL:
#	echo "parallel build is disabled in this directory to ease complex inter-directory dependencies"

# Saf stands for symbolic analysis framework
noinst_LTLIBRARIES = libSafall.la
libSafall_la_SOURCES = dummyCppFileForLibSafall.C

libSafall_la_LIBADD = \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/analysis/libSafanalysis.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/arrIndexLabeler/libSafarrIndexLabeler.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/cfgUtils/libSafcfgUtils.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/chkptRangeAnalysis/libSafchkptRangeAnalysis.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/common/libSafcommon.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/external/libSafexternal.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/lattice/libSaflattice.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/mpiAnal/libSafmpiAnal.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/ompAnal/libSafompAnal.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/rwAccessLabeler/libSafrwAccessLabeler.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/simpleAnalyses/libSafsimpleAnalyses.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/state/libSafstate.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/unionFind/libSafunionFind.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/varBitVector/libSafvarBitVector.la \
          $(top_builddir)/projects/symbolicAnalysisFramework/src/variables/libSafvariables.la
# Liao, not yet compilable 	  
#          $(top_builddir)/projects/symbolicAnalysisFramework/src/varLatticeVector/libSafvarLatticeVector.la
