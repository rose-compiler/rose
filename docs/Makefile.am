
# DQ (7/24/2008): Only the developer version needs the test directory specific to Doxygen support
if ROSE_USE_EDG_SOURCE
    DEVELOPER_DISTRIBUTION_DIRECTORY = testDoxygen
endif

# Later we will add ROSETTA documentation here ...
# SUBDIRS = Rose Rosetta testDoxygen

# DQ (9/11/2006): Removed Rosetta directory, an internal tool not documented 
# within ROSE for use externally.
# SUBDIRS = Rose Rosetta testDoxygen
# SUBDIRS = Rose testDoxygen
SUBDIRS = Rose $(DEVELOPER_DISTRIBUTION_DIRECTORY)

# New rule to simplify generation of documentation (we want to avoid using the
# automake generated "docs" rule because the generation of documentation is
# dependent upon separate tools which the user might not have available
# (true for bothe the LaTeX and html documentation).
# documentation:
docs:
	@echo "    Generate all possible documentation cd Rose; make documentation;"
	@cd Rose; $(MAKE) docs;

EXTRA_DIST =
