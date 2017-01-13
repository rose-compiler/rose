/* Header file for utility routines */

/* $Id: utility-c.h,v 1.1 2004/07/07 10:27:17 dquinlan Exp $ */

#define DEFAULT_IO_ROOT_PE 0

/* Global variables */
int PGSLib_IO_ROOT_PE;		/* Root PE used during this particular run */

/* Prototypes for utility routines in utility-c.c */
void pgslib_initialize_c(int *, int *, int *, char *);
void pgslib_finalize_c();
void pgslib_error_c(char *);
void pgslib_fatal_error_c(char *);
void pgslib_abort_c();
void pgslib_output_c(char *);
void pgslib_close_output_c();
void pgslib_check_malloc_c(void *, char *);
