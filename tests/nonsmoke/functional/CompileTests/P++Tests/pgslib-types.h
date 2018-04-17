/* These are the structs needed for the communication buffers */

/* $Id: pgslib-types.h,v 1.1 2004/07/07 10:27:17 dquinlan Exp $ */

/* The terminology:
       Requestor:    Requests the communication.  Typically the elements are the requestors.
       Complior :    Complies with the request.  Typically the nodes are the compliors.
   Abbreviations:
       Req = Requestor
       Cmpl = Complior
*/

#ifndef PGSLIB_TYPES_GUARD
#define PGSLIB_TYPES_GUARD

#ifndef HASH_INCLUDE_FILE
#include "pgshash.h"
#endif

#ifndef SEND_RCV_INCLUDE_FILE
#include "send-rcv.h"
/* include <send-rcv.h> */
#endif

typedef int CMPL_NUM;
typedef int CMPL_TYPE;

typedef struct {
  int PE;
} ADDRESS;


typedef struct {
    ADDRESS PE;
    int     Tag;
    int     Offset;
    int     ncmpls;
} CmplOwnerPE;

typedef struct {
    ADDRESS PE;
    int     Tag;
    int     Offset;
    int     ncmpls;
} CmplReceiverPE;

typedef struct {
    int            N_CmplOwnerPEs;
    int            N_CmplReceiverPEs;
    int            *CmplOwnerPEOrder;
    int            *CmplReceiverPEOrder;
    CmplOwnerPE    *CmplOwnerPEs;
    CmplReceiverPE *CmplReceiverPEs;
    int            N_Supplement;
    CMPL_NUM       *Supplements;
    ADDRESS        *SupplementsPEs;
    int            N_Duplicate;
    int            *Duplicates;
    HASH_TABLE_ENTRY *Hash_Table;
    COMM_SEND	   *GatherSBuf, *ScatterSBuf;
    COMM_RCV	   *GatherRBuf, *ScatterRBuf;
} GS_TRACE_STRUCT;  /*Gather Scatter - Trace - Structure */

#endif
