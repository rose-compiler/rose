/**
 * Java-ized version of ../QuickSortC/quicksort.c
 */
public class QuickSort {

    void nullSnippet () {}

    boolean compGT(int a, int b) {
        return a > b;
    }

    void insertSort(int a[], int lb, int ub) {
        int t;
        int i, j;

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

    int partition(int a[], int lb, int ub) {
        int t, pivot;
        int i, j, p;

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
        while (true) {
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

    void quickSort(int a[], int lb, int ub) {
        int m;

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

    void sort(int a[], int n) {
        quickSort(a, 0, n-1);
    }

    public static void main(String argv[]) {
        int nElmts = 100;
        int i;
        int elmts[] = new int[nElmts];
        for (i=0; i<nElmts; ++i)
            elmts[i] = (i * 10061) % 101;
        QuickSort sorter = new QuickSort();
        sorter.sort(elmts, nElmts);

        System.out.print("sorted:");
        for (i=0; i<nElmts; ++i) {
            System.out.print(" ");
            System.out.print(elmts[i]);
        }
        System.out.println();
    }
}
