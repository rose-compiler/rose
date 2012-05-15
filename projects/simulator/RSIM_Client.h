/* C library to be included by specimens that want to manipulate themselves via special system calls that will be intercepted
 * by RSIM. */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ROSE_RSIM_Client_H
#define ROSE_RSIM_Client_H

#include <sys/syscall.h>
#include <unistd.h>

/******************************************************************************************************************************
 *                                      Transactions
 ******************************************************************************************************************************/

/* Size must match pt_regs_32 defined in RSIM_Common.h */
typedef struct { unsigned char x[17*4]; } RSIM_Transaction;

/** Begin a new transaction.
 *
 *  A new transaction is started by pushing a new copy of the machine memory onto the transaction stack using
 *  RSIM_Process::mem_transaction_start().  The caller-supplied buffer is initialized with additional information (such as the
 *  registers) and must be the same object that is subsequently given as an argument to rollback or commit the transaction.
 *
 *  This function returns a negative error number if a new transaction could not be started, and it returns a positive value
 *  if the transaction was successfully started.  The success return value is the number of outstanding transactions, including
 *  the new transaction.  This value includes transactions initiated by means other than this system call, such as those
 *  initiated internally by the RSIM simulator itself.
 *
 *  A zero return value indicates that the transaction has been rolled back, or canceled. */
int RSIM_transaction_start(RSIM_Transaction *transaction);

/** Cancels a transaction.
 *
 *  Cancels the transaction specified by the argument, which must have the same address as the argument to the corresponding
 *  RSIM_transaction_start().  Any transactions that were started after the specified transaction are also terminated.  The
 *  machine memory and registers (with the exception of the EAX register) are returned to the values they had when the
 *  transaction was started.
 *
 *  This function returns a negative error number on failure.  On success, this function does not return, but rather causes the
 *  corresponding RSIM_transaction_start() to return again but with a value of zero.  In other words, RSIM_transaction_start()
 *  and RSIM_transaction_rollback() operate similarly to setjmp() and longjmp() with all the caveats that go with them. */
int RSIM_transaction_rollback(RSIM_Transaction *transaction);

#ifdef __cplusplus
} // extern
#endif
#endif
