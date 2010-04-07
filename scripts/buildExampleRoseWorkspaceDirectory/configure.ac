# This is the build specification file for a ROSE workspace.
AC_INIT([Internal-ROSE-Workspace],[0.1],[dquinlan@llnl.gov])

# require autoconf version 2.59 or greater
AC_PREREQ([2.59])

# Autoconf and Automake specific macro calls.
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([README])

# Autoheaders need to have this in palce else it is an error.
# we would like to have one of these, but coudl also test
# to make sure that the entries are the same for this and the
# generated "rose_config.h".
AC_CONFIG_HEADER([rose_workspace_config.h])

# These macros should be moved into ROSE directly so that any
# application workspace configuration can easily use them. They
# could maybe also be called within the ROSE SUPPORT ROSE macro.
# AX_ROSE
# AX_ROSE_LIB

# echo "ac_rose_path     = $ac_rose_path"
# echo "ax_rose_user_lib = $ax_rose_user_lib"
# echo "ac_rose_lib_path = $ac_rose_lib_path"

# These macros are to force all of the ROSE specific support
# to be called to define all macros as required for arbitrary
# ROSE projects.  The configure commandline should be the same
# for both ROSE/configure and the ROSE workspace's call to configure.
ROSE_SUPPORT_ROSE_PART_1
ROSE_SUPPORT_ROSE_PART_2

# We don't want to call this macro.
# ROSE_SUPPORT_ROSE_PART_3

ROSE_SUPPORT_ROSE_PART_4

# Specify the files to generate (Makefile.in's and any other required files).
AC_CONFIG_FILES([
Makefile
])

# Generate files required build (Makefiles, etc.)
AC_OUTPUT

echo ""
echo "******************************"
echo "Configure Terminated Normally!"
echo "******************************"
echo ""
