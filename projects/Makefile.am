include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Add Projects that are associated with capabilities that belong to ROSE here
SUBDIRS = \
     compass \
     binCompass \
     CloneDetection \
     DatalogAnalysis \
     DocumentationGenerator \
     OpenMP_Translator \
     BabelPreprocessor \
     FiniteStateModelChecker \
     programModeling \
     arrayOptimization \
     highLevelGrammars \
     dataStructureGraphing \
     checkPointExample \
     simpleCallGraphAnalysis \
     NameConsistancyChecker \
     C_to_Promela \
     MPICodeMotion \
     javaport \
     palette \
     reverseComputation

# DQ (1/18/2008): This is removed because it has dependence on xml libraries 
# that may not be available for users.
#     BinaryContextLookup

# DQ (1/2/2007): Removed because it is a dated example (using Coco, circa 2004).
# Also the file names are too long and this would have to be fixed before 
# using GNU tar (posix standard enforces 100 character limit).
#    containerParallelizer

# DQ (9/3/2006): Removed in preference to the newer work on OpenMP.
# Removed the older incomplete (initial attempt) at an OpenMP processor.
#    OpenMPPreprocessor

# Removed Bable from tests since it is currently broken and needs to 
# be fixed (also fails to compile with Intel C++ 9.0 compiler.
#    BabelPreprocessor 

#     AtermTranslation      \ MS10/26/04: deactivated (didn't compile)

# The dataStructureGraphing directory represents Andreas's work 
# on the visualization of complex heap based data structures


# DQ (8/24/2004): This has been moved to the ROSE/src/roseExtensions/databaseConnection
# dataBase

check-local:
	@echo "***************************************************************************"
	@echo "****** ROSE/projects: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************"

