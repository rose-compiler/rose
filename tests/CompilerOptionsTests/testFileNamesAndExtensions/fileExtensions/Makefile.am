include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = README

noinst_PROGRAMS = translator
translator_SOURCES = translator.C
translator_CPPFLAGS = $(ROSE_INCLUDES)
translator_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

########################################################################################################################
# Tests for various file name extensions.
########################################################################################################################

# Input file names with lower-case extensions
specimen_names_lc =				\
    suffix_01.c					\
    suffix_02.c++				\
    suffix_05.cc				\
    suffix_07.cp				\
    suffix_09.cpp				\
    suffix_11.cxx

# Input file names with upper-case extensions
specimen_names_uc =				\
    suffix_03.C

specimen_names = $(specimen_names_lc) $(specimen_names_uc)

# These names have been removed from the list because some compiler doesn't recognize them. The commented compiler
# and error message is not intended to be exhaustive, they're just the ones where we noticed the problem.
#   suffix_04.C++		gcc-4.8: file format not recognized; treating as linker script
#   suffix_06.CC		gcc-4.8: file format not recognized; treating as linker script
#   suffix_08.CP		gcc-4.8: file format not recognized; treating as linker script
#   suffix_10.CPP		ROSE: terminate called after throwing an instance of 'SgAsmExecutableFileFormat::FormatError'
#   suffix_12.CXX		gcc-4.8: file format not recognized; treating as linker script
#   suffix_13.i			ROSE: terminate called after throwing an instance of 'SgAsmExecutableFileFormat::FormatError'
#   suffix_14.I			gcc-4.8: file format not recognized; treating as linker script
#   suffix_15.ii		ROSE: terminate called after throwing an instance of 'SgAsmExecutableFileFormat::FormatError'
#   suffix_16.II		gcc-4.8: file format not recognized; treating as linker script
#   suffix_17.h			ROSE: terminate called after throwing an instance of 'SgAsmExecutableFileFormat::FormatError'
#   suffix_18.H			ROSE: terminate called after throwing an instance of 'SgAsmExecutableFileFormat::FormatError'
#   suffix_19.hh		ROSE: terminate called after throwing an instance of 'SgAsmExecutableFileFormat::FormatError'
#   suffix_20.HH		gcc-4.8: file format not recognized; treating as linker script

# Build each specimen
$(specimen_names):
	@echo "  GEN    $@"; echo "int main() {}" >$@

test_targets = $(addsuffix .passed, $(specimen_names))

test_script = $(srcdir)/runTest.conf
EXTRA_DIST += runTest.conf

$(test_targets): %.passed: % $(test_script)
	@$(RTH_RUN) CC="$(CC)" CXX="$(CXX)" SPECIMEN="$<" $(test_script) $@


########################################################################################################################
# Standard automake boilerplate
########################################################################################################################

check-local: $(test_targets)

clean-local:
	rm -f $(specimen_names)
	rm -f $(test_targets) $(test_targets:.passed=.failed)
