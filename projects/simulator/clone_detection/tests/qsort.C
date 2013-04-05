// From Thomas Neimann <thomasn@jps.net> at http://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_man.htm:
//     Permission to reproduce this document, in whole or in part, is given provided the original web site listed below is
//     referenced, and no additional restrictions apply. Source code, when part of a software project, may be used freely
//     without reference to the author.

/* qsort() */

namespace NAMESPACE {
namespace qsort {

typedef unsigned size_t;
typedef int T;          /* type of item to be sorted */

static const size_t MAXSTACK = sizeof(size_t) * 8;

template<typename T>
void swap(T &a, T&b)
{
    T t = a;
    a = b;
    b = t;
}

static void exchange(char *a, char *b, size_t size) {
    size_t i;

    /******************
     *  exchange a,b  *
     ******************/

    for (i = sizeof(int); i <= size; i += sizeof(int)) {
        swap(*(int*)a, *(int*)b);
        a += sizeof(int);
        b += sizeof(int);
    }
    for (i = i - sizeof(int) + 1; i <= size; i++) {
        swap(*a, *b);
        a++;
        b++;
    }
}

void qsort(void *base, size_t nmemb, size_t size,
        int (*compar)(const void *, const void *)) {
    char *lbStack[MAXSTACK], *ubStack[MAXSTACK];
    int sp;
    unsigned int offset;

    /********************
     *  ANSI-C qsort()  *
     ********************/

    lbStack[0] = (char *)base;
    ubStack[0] = (char *)base + (nmemb-1)*size;
    for (sp = 0; sp >= 0; sp--) {
        char *lb, *ub, *m;
        char *P, *i, *j;

        lb = lbStack[sp];
        ub = ubStack[sp];

        while (lb < ub) {

            /* select pivot and exchange with 1st element */
            offset = (ub - lb) >> 1;
            P = lb + offset - offset % size;
            exchange (lb, P, size);

            /* partition into two segments */
            i = lb + size;
            j = ub;
            while (1) {
                while (i < j && compar(lb, i) > 0) i += size;
                while (j >= i && compar(j, lb) > 0) j -= size;
                if (i >= j) break;
                exchange (i, j, size);
                j -= size;
                i += size;
            }

            /* pivot belongs in A[j] */
            exchange (lb, j, size);
            m = j;

            /* keep processing smallest segment, and stack largest */
            if (m - lb <= ub - m) {
                if (m + size < ub) {
                    lbStack[sp] = m + size;
                    ubStack[sp++] = ub;
                }
                ub = m - size;
            } else {
                if (m - size > lb) {
                    lbStack[sp] = lb; 
                    ubStack[sp++] = m - size;
                }
                lb = m + size;
            }
        }
    }
}
} // namespace
} // namespace
