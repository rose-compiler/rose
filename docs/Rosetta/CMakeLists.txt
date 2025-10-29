
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})


########### install files ###############




#original Makefile.am contents follow:

###########################################################################################
## This makefile is unfinished and does not yet generate Doxygen documentation for Rosetta.
###########################################################################################
#
## Regenerate the doxygen output if the dependent source code is newer
#$(RosettaDoxygenDocumentation): $(RosettaDocsDependencies) $(ROSETTA_MAINPAGE_DOCUMENTATION)
#	@echo "Generate documentation for Rosetta (used within ROSE to generate Sage III) (using Doxygen)"
#	doxygen rosetta.cfg
#
## Makefile rules called by user (Makefile's public interface)
#rosetta-docs: $(RosettaDoxygenDocumentation)
#
## Makefile rule called by automake (using dist or install rules)
#docs:
#	@echo "Skipping the creation of documentation within the docs rule (for now)."
#
#
