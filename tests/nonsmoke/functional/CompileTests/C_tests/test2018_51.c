/* This is a problem for C99, but works find for C90 using -std=iso9899:1990 */
/* ROSE does not support the -std=iso9899:1990 options, so ROSE flags "unix" and "asm" as reserved words */

int near, far, cdecl, unix, vax;       /* non-reserved names */
int globaldef, fortran, pascal, entry; /* non-reserved names */
int asm;                               /* non-reserved names */
int i386, i387, exception;             /* non-reserved names */

