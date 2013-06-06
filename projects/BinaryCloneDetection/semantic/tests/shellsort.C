// From Thomas Neimann <thomasn@jps.net> at http://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_man.htm:
//     Permission to reproduce this document, in whole or in part, is given provided the original web site listed below is
//     referenced, and no additional restrictions apply. Source code, when part of a software project, may be used freely
//     without reference to the author.

/* shell sort */

namespace NAMESPACE {
namespace shellsort {

template<typename T>
static bool compGT(T a, T b) { return a > b; }

typedef int T;          /* type of item to be sorted */
typedef int tblIndex;   /* type of subscript */


void shellSort(T *a, tblIndex lb, tblIndex ub) {
    tblIndex n, h, i, j;
    T t;

   /**************************
    *  sort array a[lb..ub]  *
    **************************/

    /* compute largest increment */
    n = ub - lb + 1;
    h = 1;
    if (n < 14)
        h = 1;
    else if (sizeof(tblIndex) == 2 && n > 29524)
        h = 3280;
    else {
        while (h < n) h = 3*h + 1;
        h /= 3;
        h /= 3;
    }

    while (h > 0) {

        /* sort-by-insertion in increments of h */
        for (i = lb + h; i <= ub; i++) {
            t = a[i];
            for (j = i-h; j >= lb && compGT(a[j], t); j -= h)
                a[j+h] = a[j];
            a[j+h] = t;
        }

        /* compute next increment */
        h /= 3;
    }
}

} // namespace
} // namespace
