// Snippets to inject for
public class ErrorSnippets {
    Integer savedInteger = null;

    /* Reads environment variable QUICKSORT_ERROR and sets qs_error if the environment variable is "yes" */
    void readEnvironment() {
        String tmp_env = System.getenv("QUICKSORT_ERROR");
        int qs_error;
        qs_error = tmp_env != null && tmp_env=="yes" ? 1 : 0;
    }

    /* Allocate space for saving an integer. The address is stored in global variable 'savedInteger' and assigned to the argument. */
    void allocateSavedInteger(Integer addr) {
    if (null==savedInteger)
        savedInteger = new Integer(0);
        addr = savedInteger;
    }

    /* Rotate X right by N bits */
    void rotateRight(int x, int n) {
        int tmp_nbits = 32;
        int tmp_un = n % tmp_nbits;
        x = (x >>> tmp_un) | (x << (tmp_nbits - tmp_un));
    }

    /* Rotate X left by N bits */
    void rotateLeft(int x, int n) {
        int tmp_nbits = 32;
        int tmp_un = n % tmp_nbits;
        x = (x << tmp_un) | (x >>> (tmp_nbits - tmp_un));
    }

    /* Squirrel away an integer value in a heap location, location stored in global variable. */
    void saveInteger(int whatToSave) {
        Integer tmp_addr=null, tmp_encoded;
        allocateSavedInteger(tmp_addr);

        tmp_encoded = whatToSave;
        rotateRight(tmp_encoded, 5);
        tmp_addr = tmp_encoded;
        whatToSave = ((whatToSave >> 3) & 0x1fac3f07) | (whatToSave << 29); /* arbitrary, loses information */
    }

    /* Restore a value that was previously squirreled away. */
    void restoreInteger() {
        Integer tmp_addr=null;
        int tmp_encoded, tmp_decoded, restoredValue;
        allocateSavedInteger(tmp_addr);
        tmp_decoded = tmp_addr;
        rotateLeft(tmp_decoded, 5);
        restoredValue = tmp_decoded;
    }

    /* The last step of a pivot is to place the pivot value at the pivot point.  This snippet undoes that step
     * so the pivot value remains at the wrong index. */
    void arrayElementSwap(int elmts[], int idx1, int idx2, int shouldSwap) {
        if (shouldSwap!=0) {
            int tmp_swap = elmts[idx1];
            elmts[idx1] = elmts[idx2];
            elmts[idx2] = tmp_swap;
        }
    }
}
