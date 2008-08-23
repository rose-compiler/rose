// SATIrE wrapper header file for <rose.h> to suppress warnings related to
// redefinition of autoconf macros.

#ifndef SATIRE_ROSE_H
#define SATIRE_ROSE_H

// Copy existing definitions.
#define SATIRE_PACKAGE PACKAGE
#define SATIRE_PACKAGE_BUGREPORT PACKAGE_BUGREPORT
#define SATIRE_PACKAGE_NAME PACKAGE_NAME
#define SATIRE_PACKAGE_STRING PACKAGE_STRING
#define SATIRE_PACKAGE_TARNAME PACKAGE_TARNAME
#define SATIRE_PACKAGE_VERSION PACKAGE_VERSION
#define SATIRE_VERSION VERSION

// Undefine existing definitions.
#undef PACKAGE
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef VERSION

// Include ROSE's definitions.
#include <rose.h>

// GB (2008-08-21): We will undefine ROSE's definitions below. However,
// rather than throwing them away, let's keep copies around, because they
// might come in handy some time.
#define ROSE_PACKAGE PACKAGE
#define ROSE_PACKAGE_BUGREPORT PACKAGE_BUGREPORT
#define ROSE_PACKAGE_NAME PACKAGE_NAME
#define ROSE_PACKAGE_STRING PACKAGE_STRING
#define ROSE_PACKAGE_TARNAME PACKAGE_TARNAME
#define ROSE_PACKAGE_VERSION PACKAGE_VERSION
#define ROSE_VERSION VERSION

// Undefine ROSE's definitions.
#undef PACKAGE
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef VERSION

// Finally, redefine our copied definitions.
#define PACKAGE SATIRE_PACKAGE
#define PACKAGE_BUGREPORT SATIRE_PACKAGE_BUGREPORT
#define PACKAGE_NAME SATIRE_PACKAGE_NAME
#define PACKAGE_STRING SATIRE_PACKAGE_STRING
#define PACKAGE_TARNAME SATIRE_PACKAGE_TARNAME
#define PACKAGE_VERSION SATIRE_PACKAGE_VERSION
#define VERSION SATIRE_VERSION

#endif
