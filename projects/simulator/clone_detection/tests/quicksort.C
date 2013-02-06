// From Thomas Neimann <thomasn@jps.net> at http://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_man.htm:
//     Permission to reproduce this document, in whole or in part, is given provided the original web site listed below is
//     referenced, and no additional restrictions apply. Source code, when part of a software project, may be used freely
//     without reference to the author.

/* quicksort */

namespace NAMESPACE {
namespace quicksort {

template<typename T>
bool compGT(T a, T b) { return a > b; }

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

    return j;
}

void quickSort(T *a, tblIndex lb, tblIndex ub) {
    tblIndex m;

   /**************************
    *  sort array a[lb..ub]  *
    **************************/

    while (lb < ub) {

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

} // namespace
} // namespace
