#include <string>

/* Use the same header file to declare these variables as is used to reference them so that they will be globally available (and not local). */
#include "rose_paths.h"

/* These paths will be absolute or relative depending on how the configure script is called (called with an absolute or relative path). */
const std::string ROSE_GFORTRAN_PATH          = "/usr/local/bin/gfortran";
const std::string ROSE_AUTOMAKE_TOP_SRCDIR    = "/g/g15/bronevet/Compilers/ROSE";
const std::string ROSE_AUTOMAKE_TOP_BUILDDIR  = "/g/g15/bronevet/Compilers/ROSE/compile";
const std::string ROSE_AUTOMAKE_PREFIX        = "/g/g15/bronevet/Compilers/ROSE/compile";
const std::string ROSE_AUTOMAKE_DATADIR       = "/g/g15/bronevet/Compilers/ROSE/compile/share";
const std::string ROSE_AUTOMAKE_BINDIR        = "/g/g15/bronevet/Compilers/ROSE/compile/bin";
const std::string ROSE_AUTOMAKE_INCLUDEDIR    = "/g/g15/bronevet/Compilers/ROSE/compile/include";
const std::string ROSE_AUTOMAKE_INFODIR       = "/g/g15/bronevet/Compilers/ROSE/compile/info";
const std::string ROSE_AUTOMAKE_LIBDIR        = "/g/g15/bronevet/Compilers/ROSE/compile/lib";
const std::string ROSE_AUTOMAKE_LIBEXECDIR    = "/g/g15/bronevet/Compilers/ROSE/compile/libexec";
const std::string ROSE_AUTOMAKE_LOCALSTATEDIR = "/g/g15/bronevet/Compilers/ROSE/compile/var";
const std::string ROSE_AUTOMAKE_MANDIR        = "/g/g15/bronevet/Compilers/ROSE/compile/man";

/* This will always be an absolute path, while paths above are dependent on how the configure script is called (called with an absolute or relative path). */
const std::string ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR = "/g/g15/bronevet/Compilers/ROSE";

/* Additional interesting data to provide. */
const std::string ROSE_CONFIGURE_DATE     = "Thursday May  5 10:32:21 2011";
const std::string ROSE_AUTOMAKE_BUILD_OS  = "linux-gnu";
const std::string ROSE_AUTOMAKE_BUILD_CPU = "x86_64";

/* Numeric form of ROSE version -- assuming ROSE version x.y.zL (where */
/* x, y, and z are numbers, and L is a single lowercase letter from a to j), */
/* the numeric value is x * 1000000 + y * 10000 + z * 100 + (L - 'a'). */
const int ROSE_NUMERIC_VERSION = 90500;
