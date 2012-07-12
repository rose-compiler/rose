/* C library to be included by specimens that want to manipulate themselves via special system calls that will be intercepted
 * by RSIM. */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ROSE_RSIM_Client_H
#define ROSE_RSIM_Client_H

/******************************************************************************************************************************
 *                                      Transactions
 ******************************************************************************************************************************/

/** Determine if we're running under the simulator.
 *
 *  Returns -1 with errno set to ENOSYS if we're running natively, but returns zero if run under the simulator.  All of the
 *  simulator-specific system calls will return -1 with errno set to ENOSYS when running natively, but this function provides a
 *  nice, clean way to check. */
int RSIM_is_present();

/** Show a message in the syscall tracing output.
 *
 *  This system call does nothing except return zero. However, since its argument is a NUL-terminated string, the string will
 *  show up in the simulators syscall trace.  Thus, this provides a clean way to get a message into the log files. */
int RSIM_message(const char *mesg);

/** Pause for time.
 *
 *  This function triggers a nanosleep in the simulator.  It is useful as a simple way to pause the calling thread without
 *  generating specimen-level signals that would complicate the analysis output. */
int RSIM_delay(unsigned seconds, unsigned nanoseconds);

/** Data type for transactions.
 *
 *  This is intended to be an opaque type.  Its contents will change in the future if RSIM supports more sophisticated forms of
 *  transactions. */
typedef struct { unsigned char x[17*4]; } RSIM_Transaction; /* Size must match pt_regs_32 defined in RSIM_Common.h */

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
