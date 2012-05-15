#include "RSIM_Client.h"

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
