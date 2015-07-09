/* Wrappers around RSIM-specific system calls.  The system call numbers are defined in RSIM_Simulator::ctor(). */

#include "RSIM_Client.h"
#include <sys/syscall.h>
#include <unistd.h>

int
RSIM_is_present()
{
    return syscall(1000000);
}

int
RSIM_message(const char *mesg)
{
    return syscall(1000001, mesg);
}

int
RSIM_delay(unsigned sec, unsigned nsec)
{
    return syscall(1000002, sec, nsec);
}

int
RSIM_transaction_begin(RSIM_Transaction *transaction)
{
    return syscall(1000003, 0, transaction);
}

int
RSIM_transaction_rollback(RSIM_Transaction *transaction)
{
    return syscall(1000003, 1, transaction);
}
