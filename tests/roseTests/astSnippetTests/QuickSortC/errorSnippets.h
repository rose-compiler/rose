/*  This is the stuff that must be included in both the target and the snippets if AST fixups are being performed. */
#ifndef errorSnippets_H
#define errorSnippets_H

#ifdef DECLARE_RELATED_THINGS

#include <stdlib.h>
#include <string.h>

static int *savedInteger;

/* Declarations for snippets that are inserted recursively. The target AST will only have transient calls to these
 * functions since they'll be expanded as snippets, but they must be declared nonetheless when AST fixups are enabled
 * because the fixups run at each level of the recursion. */
void allocateSavedInteger(int *addr);
void rotateRight(int, int);
void rotateLeft(int, int);

#endif
#endif
