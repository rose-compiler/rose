#ifndef ROSE_PATHS_H
#define ROSE_PATHS_H

extern const char ROSE_GFORTRAN_PATH[];
extern const char ROSE_AUTOMAKE_TOP_SRCDIR[];
extern const char ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR[];
extern const char ROSE_AUTOMAKE_TOP_BUILDDIR[];
extern const char ROSE_AUTOMAKE_PREFIX[];
extern const char ROSE_AUTOMAKE_DATADIR[];
extern const char ROSE_AUTOMAKE_BINDIR[];
extern const char ROSE_AUTOMAKE_INCLUDEDIR[];
extern const char ROSE_AUTOMAKE_INFODIR[];
extern const char ROSE_AUTOMAKE_LIBDIR[];
extern const char ROSE_AUTOMAKE_LIBEXECDIR[];
extern const char ROSE_AUTOMAKE_LOCALSTATEDIR[];
extern const char ROSE_AUTOMAKE_MANDIR[];

/* Additional interesting data to provide */
extern const char ROSE_CONFIGURE_DATE[];
extern const char ROSE_AUTOMAKE_BUILD_OS[];
extern const char ROSE_AUTOMAKE_BUILD_CPU[];

/* Numeric form of ROSE version -- assuming ROSE version x.y.zL (where */
/* x, y, and z are numbers, and L is a single lowercase letter from a to j), */
/* the numeric value is x * 1000000 + y * 10000 + z * 100 + (L - 'a') */
extern const int ROSE_NUMERIC_VERSION;

#endif /* ROSE_PATHS_H */
