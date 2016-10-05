/*  This is the stuff that must be included in both the target and the snippets if AST fixups are being performed. */
#ifndef errorSnippets_H
#define errorSnippets_H

#include <stdlib.h>
#include <string.h>

static int *savedInteger;

/* Declarations for all functions that might be copied to the target as a function. */
void readEnvironment(void);
void allocateSavedInteger(int *addr);
void rotateRight(int, int);
void rotateLeft(int, int);
void saveInteger(int);
void restoreInteger(void);
void arrayElementSwap(int*, int, int, int);

#endif
