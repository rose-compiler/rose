##########################################################################################
# This makefile is unfinished and does not yet generate Doxygen documentation for Rosetta.
##########################################################################################

if ROSE_HAVE_DOXYGEN

# Regenerate the doxygen output if the dependent source code is newer
$(RosettaDoxygenDocumentation): $(RosettaDocsDependencies) $(ROSETTA_MAINPAGE_DOCUMENTATION)
	@echo "Generate documentation for Rosetta (used within ROSE to generate Sage III) (using Doxygen)"
	$(DOXYGEN) rosetta.cfg

else

$(RosettaDoxygenDocumentation): $(RosettaDocsDependencies) $(ROSETTA_MAINPAGE_DOCUMENTATION)
	@echo "Doxygen documentation not generated (configure with '--with-doxygen')" >&2
	@false

endif

# Makefile rules called by user (Makefile's public interface)
rosetta-docs: $(RosettaDoxygenDocumentation)

# Makefile rule called by automake (using dist or install rules)
docs:
	@echo "Skipping the creation of documentation within the docs rule (for now)."



EXTRA_DIST = CMakeLists.txt
