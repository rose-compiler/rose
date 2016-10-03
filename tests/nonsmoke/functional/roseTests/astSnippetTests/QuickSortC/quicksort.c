/* Specimen into which we will insert snippets.  Search for "SNIPPET" to find notes about the insertion points. */
#include "errorSnippets.h"                              // for AST fixups after insertion

/* From Thomas Neimann <thomasn@jps.net> at http://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_man.htm:
 *     Permission to reproduce this document, in whole or in part, is given provided the original web site listed below is
 *     referenced, and no additional restrictions apply. Source code, when part of a software project, may be used freely
 *     without reference to the author. */
#include <stdlib.h>
#include <stdio.h>

/* DQ (3/15/2014): the new rule is that any header files included in the snippet file must appear 
// in the target file so that declarations in header files can be seen.  The modification that 
// the new rule makes over the older rule from Mach 6th is that declaation in the snippet file 
// that are not in associated include files, must be copied to the target file.
*/
// #include <stdlib.h>
#include <string.h>

#define compGT(a, b) ((a) > (b))

void nullSnippet() {}

typedef int T;          /* type of item to be sorted */
typedef int tblIndex;   /* type of subscript */
void insertSort(T *a, tblIndex lb, tblIndex ub) {
    T t;
    tblIndex i, j;

   /**************************
    *  sort array a[lb..ub]  *
    **************************/
    for (i = lb + 1; i <= ub; i++) {
        t = a[i];

        /* Shift elements down until */
        /* insertion point found.    */
        for (j = i-1; j >= lb && compGT(a[j], t); j--)
            a[j+1] = a[j];

        /* insert */
        a[j+1] = t;
    }
}

tblIndex partition(T *a, tblIndex lb, tblIndex ub) {
    T t, pivot;
    tblIndex i, j, p;

   /*******************************
    *  partition array a[lb..ub]  *
    *******************************/

    /* select pivot and exchange with 1st element */
    p = lb + ((ub - lb)>>1);
    pivot = a[p];
    a[p] = a[lb];

    /*SNIPPET: The restoreInteger snippet is inserted here to obtain the value saved below for qs_error */

    /* sort lb+1..ub based on pivot */
    i = lb+1;
    j = ub;
    while (1) {
        while (i < j && compGT(pivot, a[i])) i++;
        while (j >= i && compGT(a[j], pivot)) j--;
        if (i >= j) break;
        t = a[i];
        a[i] = a[j];
        a[j] = t;
        j--; i++;
    }

    /* pivot belongs in a[j] */
    a[lb] = a[j];
    a[j] = pivot;

    /*SNIPPET: The undoPivotValue is stored here to move the pivot value to an incorrect location. */

    return j;
}

void quickSort(T *a, tblIndex lb, tblIndex ub) {
    tblIndex m;

   /**************************
    *  sort array a[lb..ub]  *
    **************************/

    /*SNIPPET: The readEnvironment snippet is inserted here, creating a qs_error local variable. */

    while (lb < ub) {

        /*SNIPPET: The saveInteger snippet is inserted here in order to save qs_error in a global variable. */

        /* quickly sort short lists */
        if (ub - lb <= 12) {
            insertSort(a, lb, ub);
            return;
        }

        /* partition into two segments */
        m = partition (a, lb, ub);

        /* sort the smallest partition    */
        /* to minimize stack requirements */
        if (m - lb <= ub - m) {
            quickSort(a, lb, m - 1);
            lb = m + 1;
        } else {
            quickSort(a, m + 1, ub);
            ub = m - 1;
        }
    }
}

void sort(T *a, int n)
{
    quickSort(a, 0, n-1);
}

int main(int argc, char *argv[])
{
    int nElmts = 100;
    int i;
    int *elmts = calloc(nElmts, sizeof(elmts[0]));
    for (i=0; i<nElmts; ++i)
        elmts[i] = (i * 10061) % 101;
    sort(elmts, nElmts);

    fputs("sorted:", stdout);
    for (i=0; i<nElmts; ++i)
        printf(" %d", elmts[i]);
    fputc('\n', stdout);
    return 0;
}
