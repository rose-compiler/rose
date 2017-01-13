/* This is part of the netCDF package.
   Copyright 2005 University Corporation for Atmospheric Research/Unidata
   See COPYRIGHT file for conditions of use.

   Common includes, defines, etc., for test code in the nc_test4
   directory.

   $Id: tests.h,v 1.5 2007/11/05 16:22:43 ed Exp $
*/
#ifndef _NC_TEST4_TESTS_
#define _NC_TEST4_TESTS_

#include <config.h>
#include <netcdf.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef USE_PARALLEL 
#include <mpi.h>
#ifndef MPI_FILE_NULL /*MPIO may be defined in mpi.h already */
#include <mpio.h>
#endif
#endif

int total_err = 0, err = 0;

/* This is handy for print statements. */
char *format_name[] = {"", "classic", "64-bit offset", "netCDF-4", "netCDF-4 strict NC3"};

#define BAIL(e) do { \
printf ("Bailing out in file %s, line %d, error:%s.\n", __FILE__, __LINE__, nc_strerror(e)); \
return -1; \
} while (0) 

#define ERR do { \
fflush(stdout); /* Make sure our stdout is synced with stderr. */ \
err++; \
fprintf(stderr, "unexpected result, %s, line: %d\n", __FILE__, __LINE__); \
} while (0)

#define SUMMARIZE_ERR do { \
   if (err) \
   { \
      printf("%d failures\n", err); \
      total_err += err; \
      err = 0; \
   } \
   else \
      printf("ok.\n"); \
} while (0)

/* Print out our number of errors, if any, and exit badly. */
#define FINAL_RESULTS do { \
   if (total_err) \
   { \
      printf("%d errors detected! Sorry!\n", total_err); \
      return 2; \
   } \
   printf("*** Tests successful!\n"); \
   return 0; \
} while (0)

#endif /* _NC_TEST4_TESTS_ */
