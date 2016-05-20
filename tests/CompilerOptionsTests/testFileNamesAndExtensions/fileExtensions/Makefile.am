include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = README

noinst_PROGRAMS = translator
translator_SOURCES = translator.C
translator_CPPFLAGS = $(ROSE_INCLUDES)
translator_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

########################################################################################################################
# Tests for various file name extensions.
########################################################################################################################

# These names have been removed from the following lists for various reasons given here. The reasons are not meant to be
# exhaustive, they're just where we first noticed the problem now that the tests have been rewritten so they actually
# test something (previously the test ran but we never considered the result). [Matzke 2016-05-19]
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
#   dot.			gcc-4.8: file format not recognized; treating as linker script
#   no_suffix			gcc-4.8: file format not recognized; treating as linker script
#   test_empty_cplusplus_file.C removed: this is not testing file names but rather content, so doesn't belong here
#   test_empty_c_file.c         removed: this is not testing file names but rather content, so doesn't belong here
#   most long name tests	removed: these exceeded OS name length limits and some tested file content

# Input file names with lower-case extensions
specimen_lc_extensions =			\
    suffix_01.c					\
    suffix_02.c++				\
    suffix_05.cc				\
    suffix_07.cp				\
    suffix_09.cpp				\
    suffix_11.cxx

# Input file names with upper-case extensions
specimen_uc_extensions =			\
    suffix_03.C

# Strange names. Rather than play escaping tricks with make and the shell, we replace the special characters with
# non-special characters and then insert the special characters at the very last second in the translator.
specimen_special_chars =			\
	quote+SQUOTE+.C				\
	quote+DQUOTE+.C				\
	dollar+DOLLAR+.C			\
	star+STAR+.C				\
	hash+HASH+.C				\
	space+SPACE+.C				\
	backslash+BSLASH+.C

specimen_strange_paths =						\
	.+SLASH+x.c							\
	.+SLASH++SLASH+x.c						\
	.+SLASH+.+SLASH+x.c					\
	.+SLASH+.+SLASH++SLASH+.+SLASH++SLASH++SLASH+x.c

# These should be shorter than the filesystem name and path length limits. Remember to also include the length of
# generated names, such as adding "rose_" to the front of a name.  Some limits are:
#   Filesystem		Max name length			Max path length (*1)
#  ------------------- ------------------------------  ----------------------------
#   NTFS (Windows)	255 characters			32767 unicode characters
#   HFS Plus (OS X)	255 UTF-16 characters		unlimited
#   UFS1 & 2 (BSDs)	255 bytes			not defined
#   ext 1-4 (Linux)	255 bytes			not defined
#   Lustre (SCF/OCF)	255 bytes			not defined
#   GPFS (IBM)		255 UTF-8 codepoints		not defined
#   GFS (Linux)		255				not defined
#   ReiserFS (Linux)	255 characters			not defined
#   Reiser4		3976 bytes			not defined
#   ZFS			255 bytes			not defined
#   Btrfs (Linux)	255 bytes			not defined
#  ------------------- ------------------------------  ----------------------------
#  Footnotes:
#    *1: The on-disk representation often supports paths that can be longer than the limit imposed by the operating system.
#        For instance, the path length limit on Linux is 4096 characters.

specimen_long_names = \
	aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.c \
	+DOT++SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+.+SLASH+x.c

specimen_names = \
	$(specimen_lc_extensions) \
	$(specimen_uc_extensions) \
	$(specimen_special_chars) \
	$(specimen_strange_paths) \
	$(specimen_long_names)


test_targets = $(addsuffix .passed, $(specimen_names))

test_script = $(srcdir)/runTest.conf
EXTRA_DIST += runTest.conf

$(test_targets): %.passed: translator
	@$(RTH_RUN) CC="$(CC)" CXX="$(CXX)" SPECIMEN="$*" $(test_script) "$@"


########################################################################################################################
# Standard automake boilerplate
########################################################################################################################

check-local: $(test_targets)

# Watch out for strange characters in these names.
clean-local:
	rm -f $(specimen_names)
	rm -f $(test_targets) $(test_targets:.passed=.failed)
