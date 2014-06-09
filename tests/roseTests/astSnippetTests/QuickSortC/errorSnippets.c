#define DECLARE_RELATED_THINGS                          /* used by errorSnippets.h */
#include "errorSnippets.h"

/* Reads environment variable QUICKSORT_ERROR and sets qs_error if the environment variable is "yes" */
void readEnvironment(void) {
    const char *tmp_env;
    int qs_error;
    tmp_env = getenv("QUICKSORT_ERROR");
    qs_error = tmp_env != NULL && 0 == strcmp(tmp_env, "yes");
}

/* Allocate space for saving an integer. The address is stored in global variable 'savedInteger' and assigned to the argument. */
void allocateSavedInteger(int *addr) {
    if (!savedInteger)
        savedInteger = malloc(sizeof savedInteger);
    addr = savedInteger;
}

/* Rotate X right by N bits */
void rotateRight(int x, int n) {
    unsigned tmp_un, tmp_nbits;
    tmp_nbits = 8 * sizeof(x);
    tmp_un = (unsigned)n % tmp_nbits;
    if (tmp_un!=0) {
        unsigned tmp_lo_mask = (unsigned)(-1) >> tmp_un;
        x = ((x >> tmp_un) & tmp_lo_mask) | (x << (tmp_nbits - tmp_un));
    }
}

/* Rotate X left by N bits */
void rotateLeft(int x, int n) {
    unsigned tmp_un, tmp_nbits;
    tmp_nbits = 8 * sizeof(x);
    tmp_un = (unsigned)n % tmp_nbits;
    if (tmp_un!=0) {
        unsigned tmp_lo_mask = (unsigned)(-1) >> (tmp_nbits - tmp_un);
        x = (x << tmp_un) | ((x >> (tmp_nbits - tmp_un)) & tmp_lo_mask);
    }
}

/* Squirrel away an integer value in a heap location, location stored in global variable. */
void saveInteger(int whatToSave) {
    int *tmp_addr, tmp_encoded;
    allocateSavedInteger(tmp_addr);

    tmp_encoded = whatToSave;
    rotateRight(tmp_encoded, 5);
    *tmp_addr = tmp_encoded;
    whatToSave = ((whatToSave >> 3) & 0x1fac3f07) | (whatToSave << 29); /* arbitrary, loses information */
}

/* Restore a value that was previously squirreled away. */
void restoreInteger(void) {
    int *tmp_addr, tmp_encoded, tmp_decoded, restoredValue;
    allocateSavedInteger(tmp_addr);
    tmp_decoded = *tmp_addr;
    rotateLeft(tmp_decoded, 5);
    restoredValue = tmp_decoded;
}

/* The last step of a pivot is to place the pivot value at the pivot point.  This snippet undoes that step
 * so the pivot value remains at the wrong index. */
void arrayElementSwap(int *elmts, int idx1, int idx2, int shouldSwap) {
    if (shouldSwap) {
        int tmp_swap = elmts[idx1];
        elmts[idx1] = elmts[idx2];
        elmts[idx2] = tmp_swap;
    }
}
