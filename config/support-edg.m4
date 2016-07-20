
AC_DEFUN([ROSE_SUPPORT_EDG],
[

# DQ (2/2/2010): New code to control use of different versions of EDG with ROSE.
AC_ARG_ENABLE(edg-version,
[  --enable-edg_version     major.minor version number for EDG (e.g. 4.4, \[4.7\], 4.8, 4.9).],
[ echo "Setting up EDG version"
])

echo "enable_edg_version = $enable_edg_version"
if test "x$enable_edg_version" = "x"; then
# DQ (5/22/2016): Changed the default version of EDG to 4.9.
   echo "Default version of EDG used (4.9)"
   edg_major_version_number=4
   edg_minor_version_number=9
else
   echo "Specifying EDG version is not recommended"
   edg_major_version_number=`echo $enable_edg_version | cut -d\. -f1`
   edg_minor_version_number=`echo $enable_edg_version | cut -d\. -f2`
fi

echo "edg_major_version_number = $edg_major_version_number"
echo "edg_minor_version_number = $edg_minor_version_number"

if test "x$edg_major_version_number" = "x4"; then
  echo "Recognized an accepted major version number."
  if test "x$edg_minor_version_number" = "x4"; then
    echo "Recognized an accepted minor version number."
    enable_edg_version44=yes
    AC_DEFINE([ROSE_USE_EDG_VERSION_4_4], [], [Whether to use the new EDG version 4.4])
  else
    if test "x$edg_minor_version_number" = "x7"; then
      echo "Recognized an accepted minor version number."
      enable_edg_version47=yes
      AC_DEFINE([ROSE_USE_EDG_VERSION_4_7], [], [Whether to use the new EDG version 4.7])
    else
      if test "x$edg_minor_version_number" = "x8"; then
        echo "Recognized an accepted minor version number."
        enable_edg_version48=yes
        AC_DEFINE([ROSE_USE_EDG_VERSION_4_8], [], [Whether to use the new EDG version 4.8])
      else
        if test "x$edg_minor_version_number" = "x9"; then
          echo "Recognized an accepted minor version number."
          enable_edg_version49=yes
          AC_DEFINE([ROSE_USE_EDG_VERSION_4_9], [], [Whether to use the new EDG version 4.9])
        else
          echo "ERROR: Could not identify the EDG minor version number."
          exit 1
        fi
      fi
    fi
  fi
else
  echo "ERROR: Could not identify the EDG major version number."
  exit 1
fi

enable_edg_version4=yes
AC_DEFINE([ROSE_USE_EDG_VERSION_4], [], [Whether to use the new EDG version 4.x])

enable_new_edg_interface=yes
AC_DEFINE([ROSE_USE_NEW_EDG_INTERFACE], [], [Whether to use the new interface to EDG])

AC_DEFINE_UNQUOTED([ROSE_EDG_MAJOR_VERSION_NUMBER], $edg_major_version_number , [EDG major version number])
AC_DEFINE_UNQUOTED([ROSE_EDG_MINOR_VERSION_NUMBER], $edg_minor_version_number , [EDG minor version number])

ROSE_EDG_MAJOR_VERSION_NUMBER=$edg_major_version_number
ROSE_EDG_MINOR_VERSION_NUMBER=$edg_minor_version_number

AC_SUBST(ROSE_EDG_MAJOR_VERSION_NUMBER)
AC_SUBST(ROSE_EDG_MINOR_VERSION_NUMBER)


# DQ (2/3/2010): I would like to not have to use these and use the new
# ROSE_EDG_MAJOR_VERSION_NUMBER and ROSE_EDG_MINOR_VERSION_NUMBER instead.
AM_CONDITIONAL(ROSE_USE_EDG_VERSION_4_4, [test "x$enable_edg_version44" = xyes])
AM_CONDITIONAL(ROSE_USE_EDG_VERSION_4_7, [test "x$enable_edg_version47" = xyes])
AM_CONDITIONAL(ROSE_USE_EDG_VERSION_4_8, [test "x$enable_edg_version48" = xyes])
AM_CONDITIONAL(ROSE_USE_EDG_VERSION_4_9, [test "x$enable_edg_version49" = xyes])
]
)

