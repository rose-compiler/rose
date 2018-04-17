# -*- makefile -*-

# Lists of specimens go here. All make variables in this file should have a common prefix so they won't conflict with
# similar variables for other lists of specimens included from other make files in other directories.

# There are only a few smoke specimens, so unlike the non-smoke side, we don't take the trouble to build a comprehensive
# set of variables for specimen names.

smoke_specimens_binary_directory = $(abspath $(top_srcdir))/tests/smoke/specimens/binary

smoke_specimens_binary_list = x86-elf-exe
