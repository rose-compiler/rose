// This is the ROSE verson of Sawyer's Database.C file. We're basically wrapping Database.C so that we can turn off some
// features if they're not configured in ROSE.

#include <featureTests.h>

#if !defined(SAWYER_NO_SQLITE) && !defined(ROSE_HAVE_SQLITE3)
#define SAWYER_NO_SQLITE
#endif

#if !defined(SAWYER_NO_POSTGRESQL) && !defined(ROSE_HAVE_LIBPQXX)
#define SAWYER_NO_POSTGRESQL
#endif

#include "Database.C"
