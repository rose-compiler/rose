// From Thomas Neimann <thomasn@jps.net> at http://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_man.htm:
//     Permission to reproduce this document, in whole or in part, is given provided the original web site listed below is
//     referenced, and no additional restrictions apply. Source code, when part of a software project, may be used freely
//     without reference to the author.

/* insert sort */

namespace NAMESPACE {
namespace insertsort {

typedef int T;          /* type of item to be sorted */
typedef int tblIndex;   /* type of subscript */

#define compGT(a,b) (a > b)

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

} // namespace
} // namespace
