#include <stdio.h>

/* This is a C header file for a small library that defines some wrappers around RSIM-specific system calls. */
#include "RSIM_Client.h"

#define TABLE_SIZE 5

int
main(int argc, char *argv[])
{
    RSIM_Transaction transaction;
    int i, value[TABLE_SIZE];

    if (0==RSIM_is_present()) {
        printf("%s: running under the ROSE simulator (RSIM).\n", argv[0]);
    } else {
        printf("%s: running natively; expect transactions to fail\n", argv[0]);
    }

    /* Initialize the array.  We'll modify the array elements within transactions so that if the transaction is canceled we
     * should see the array initial value. */
    for (i=0; i<TABLE_SIZE; ++i)
        value[i] = -1;

    for (i=0; i<TABLE_SIZE; ++i) {
        /* Start a transaction.  Possible return values are:
         *   -1 w/errno ENOSYS -- we're probably running natively, in which case no transaction is started
         *   -1 other errno    -- simulator-detected error; no transaction started
         *   positive          -- transaction started; return value is number of outstanding transactions
         *   zero              -- rollback occurred
         */
        printf("  iteration %d\n", i);
        if (0!=RSIM_transaction_begin(&transaction)) {

            /* Do something that has some effect on memory... */
            value[i] = i>0 ? value[i-1]+1 : 0;

            /* Cancel the transaction (except on the last time through the loop, for demo purposes).  If this returns then the
             * transaction was not canceled.  If the transaction is canceled, then the corresponding RSIM_transaction_begin()
             * will return zero instead.  This is sort of like a longjmp, except it restores memory as well as registers. */
            if (i+1<TABLE_SIZE)
                RSIM_transaction_rollback(&transaction);
            printf("    transaction was not canceled\n");
        }
        printf("    value[%d] = %d\n", i, value[i]);
    }

    /* Show the results.  If transactions worked, then the array will have its initial value (except the last element since we
     * didn't ever roll back the last transaction).  If the transactions didn't work then they'll have values like 0, 1, 2, ... */
    printf("  final results:\n");
    for (i=0; i<TABLE_SIZE; ++i)
        printf("    value[%d] = %d\n", i, value[i]);

    return 0;
}
