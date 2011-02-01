
#include <assert.h>
#include <upc.h>
#include <string.h>
#include <stdio.h>

#include "ParallelRTS.h"
#include "RuntimeSystem.h"

#ifndef __UPC__
#error File compiles with UPC
#endif

//
// types

enum rted_MsgKind
{
  mskFreeMemory,
  mskCreateHeapPtr,
  mskInitVariable
};

typedef enum rted_MsgKind rted_MsgKind;

struct rted_MsgMasterBlock
{
  int          unread_threads;
  upc_lock_t*  msg_lock;
};

typedef struct rted_MsgMasterBlock rted_MsgMasterBlock;

struct rted_MsgHeader
{
  shared struct rted_MsgHeader* next;

  rted_MsgKind                  kind;
  int                           threadno;
  size_t                        sz;
};

typedef struct rted_MsgHeader rted_MsgHeader;

struct rted_MsgSourceInfoHeader
{
  size_t       len;
  size_t       src_line;
  size_t       rted_line;
};

typedef struct rted_MsgSourceInfoHeader rted_MsgSourceInfoHeader;

struct rted_szTypeDesc
{
  size_t base_len;
  size_t name_len;
  size_t total;
};

typedef struct rted_szTypeDesc rted_szTypeDesc;

struct rted_MsgTypeDescHeader
{
  size_t           name_len;
  size_t           base_len;
  rted_AddressDesc desc;
};

typedef struct rted_MsgTypeDescHeader rted_MsgTypeDescHeader;


//
// Messaging Infrastructure - Single Reader/Multiple Writer Queue
//

struct MsgQSingleReadMultipleWrite
{
  shared rted_MsgHeader* head;
  shared rted_MsgHeader* tail;
  upc_lock_t*            tail_lock;
};

typedef struct MsgQSingleReadMultipleWrite MsgQSingleReadMultipleWrite;

shared[1] MsgQSingleReadMultipleWrite msgQueue[THREADS];

//
// Queue operations

void all_initMsgQueue(void)
{
  msgQueue[MYTHREAD].head = NULL;
  msgQueue[MYTHREAD].tail = NULL;
  msgQueue[MYTHREAD].tail_lock = upc_global_lock_alloc();
}

static
int msgQueueEmpty(void)
{
  return msgQueue[MYTHREAD].head == NULL;
}

/// \brief enqueues at the queue tid
static
void msgEnQueue(int tid, shared rted_MsgHeader* elem)
{
  upc_lock(msgQueue[tid].tail_lock);

  elem->next = msgQueue[tid].tail;
  msgQueue[tid].tail = elem;

  if (msgQueue[tid].head == NULL)
    msgQueue[tid].head = elem;

  upc_unlock(msgQueue[tid].tail_lock);
}

/// \brief dequeues from the local queue
static
shared rted_MsgHeader* msgDeQueue()
{
  shared rted_MsgHeader* elem = NULL;

  upc_lock(msgQueue[MYTHREAD].tail_lock);

  elem = msgQueue[MYTHREAD].head;
  msgQueue[MYTHREAD].head = elem->next;

  if (msgQueue[MYTHREAD].tail == elem)
    msgQueue[MYTHREAD].tail = NULL;

  upc_unlock(msgQueue[MYTHREAD].tail_lock);

  assert(elem);
  return elem;
}


//
// ** member function implementation :) **


// Header

static
rted_MsgHeader msgHeader(rted_MsgKind kind, int threadno, size_t sz)
{
  return (rted_MsgHeader) { NULL, kind, threadno, sz };
}

static
void msgw_Header(char* out, rted_MsgHeader head)
{
  *((rted_MsgHeader*) out) = head;
}


// Address

static
void msgw_Address(char* buf, rted_Address addr)
{
  *((rted_Address*) buf) = addr;
}

static
rted_Address msgr_Address(const char* buf)
{
  return *((const rted_Address*) buf);
}


// AllocKind

static
void msgw_AllocKind(char* buf, rted_AllocKind kind)
{
  *((rted_AllocKind*) buf) = kind;
}

static
rted_AllocKind msgr_AllocKind(const char* buf)
{
  return *((const rted_AllocKind*) buf);
}


// TypeDesc

static
rted_szTypeDesc msgsz_TypeDesc(rted_TypeDesc td)
{
  rted_szTypeDesc res;

  res.name_len = strlen(td.name) + 1;
  res.base_len = strlen(td.base) + 1;
  res.total    = sizeof(rted_MsgTypeDescHeader) + res.base_len + res.name_len;

  return res;
}

static
void msgw_TypeDesc(char* buf, rted_TypeDesc td, rted_szTypeDesc sz)
{
  rted_MsgTypeDescHeader head = { sz.name_len, sz.base_len, td.desc };
  char* const            name_loc = buf + sizeof(rted_MsgTypeDescHeader);
  char* const            base_loc = name_loc + sz.name_len;

  *((rted_MsgTypeDescHeader*) buf) = head;
  strncpy( name_loc, td.name, sz.name_len );
  strncpy( base_loc, td.base, sz.base_len );
}

static
rted_TypeDesc msgr_TypeDesc(const char* buf)
{
  rted_MsgTypeDescHeader head = *((const rted_MsgTypeDescHeader*)buf);
  const char* const      name_loc = buf + sizeof(rted_MsgTypeDescHeader);
  const char* const      base_loc = name_loc + head.name_len;

  // \note we do not copy the string, but just set the pointer to the buffer on
  //       the stack.
  return (rted_TypeDesc) { name_loc, base_loc, head.desc };
}

static
size_t msglen_Typedesc(const char* buf)
{
  rted_MsgTypeDescHeader head = *((const rted_MsgTypeDescHeader*)buf);

  return sizeof(rted_MsgTypeDescHeader) + head.name_len + head.base_len;
}

// SourceInfo

static
size_t msgsz_SourceInfo(rted_SourceInfo si)
{
  // rted_MsgSourceInfoHeader + file + '\0'
  return sizeof(rted_MsgSourceInfoHeader) +  strlen(si.file) + 1;
}

static
void msgw_SourceInfo(char* buf, rted_SourceInfo si, size_t len)
{
  const size_t             slen = len - sizeof(rted_MsgSourceInfoHeader);
  rted_MsgSourceInfoHeader head = { slen, si.src_line, si.rted_line };

  *((rted_MsgSourceInfoHeader*) buf) = head;
  strncpy( buf + sizeof(rted_MsgSourceInfoHeader), si.file, slen );
}

static
rted_SourceInfo msgr_SourceInfo(const char* buf)
{
  rted_MsgSourceInfoHeader head = *((rted_MsgSourceInfoHeader*) buf);

  // \note we do not copy the string, but just set the pointer to the buffer on
  //       the stack.
  return (rted_SourceInfo) { buf + sizeof(rted_MsgSourceInfoHeader), head.src_line, head.rted_line };
}


// built-in types

static
void msgw_SizeOf(char* buf, size_t s)
{
  *((size_t*)buf) = s;
}

static
size_t msgr_SizeOf(const char* buf)
{
  return *((const size_t*)buf);
}

static
void msgw_String(char* buf, const char* s, size_t len)
{
  strncpy( buf, s, len );
}

static
const char* msgr_String(const char* buf)
{
  return buf + sizeof(size_t);
}

static
size_t msglen_String(const char* buf)
{
  return *((const size_t*)buf);
}



//
// communication function


static
void msgBroadcast(const char* msg, const size_t len);

//
//
//


static
void rcv_FreeMemory( const rted_MsgHeader* msg )
{
  const char*          buf = (const char*) msg;
  const size_t         ad_ofs = sizeof(rted_MsgHeader);
  const size_t         ak_ofs = ad_ofs + sizeof(rted_Address);
  const size_t         si_ofs = ak_ofs + sizeof(rted_AllocKind);

  const rted_AllocKind freeKind = msgr_AllocKind(buf + ak_ofs);
  assert(freeKind == akUpcSharedHeap);

  _rted_FreeMemory( msgr_Address(buf+ad_ofs), freeKind, msgr_SourceInfo(buf+si_ofs) );
}

void snd_FreeMemory( rted_Address addr, rted_AllocKind freeKind, rted_SourceInfo si )
{
  // nothing to communicate on local frees (even if they were erroneous)
  if ((freeKind & akUpcSharedHeap) != akUpcSharedHeap) return;

  const size_t ad_ofs = sizeof(rted_MsgHeader);
  const size_t ak_ofs = ad_ofs + sizeof(rted_Address);
  const size_t si_ofs = ak_ofs + sizeof(rted_AllocKind);
  const size_t si_len = msgsz_SourceInfo(si);
  const size_t blocksz  = si_ofs + si_len;
  char         msg[blocksz];

  msgw_Header    (msg,          msgHeader(mskFreeMemory, MYTHREAD, blocksz));
  msgw_Address   (msg + ad_ofs, addr);
  msgw_AllocKind (msg + ak_ofs, freeKind);
  msgw_SourceInfo(msg + si_ofs, si, si_len);

  msgBroadcast(msg, blocksz);
}

static
int shareHeapAllocInfo(rted_AllocKind allocKind, rted_TypeDesc td)
{
  // the other upc-threads know about upc_all_alloc already
  return (  (allocKind == akStack && td.desc.shared_mask != 0) // shared array
         || (allocKind == akUpcAlloc)
         || (allocKind == akUpcGlobalAlloc)
         );
}


void rcv_CreateHeapPtr( const rted_MsgHeader* msg )
{
  const char*  buf = (const char*) msg;
  const size_t td_ofs = sizeof(rted_MsgHeader);
  const size_t ad_ofs = td_ofs + msglen_Typedesc(buf + td_ofs);
  const size_t sz_ofs = ad_ofs + sizeof(rted_Address);
  const size_t ma_ofs = sz_ofs + sizeof(size_t);
  const size_t ak_ofs = ma_ofs + sizeof(size_t);
  const size_t cn_ofs = ak_ofs + sizeof(rted_AllocKind);
  const size_t si_ofs = cn_ofs + msglen_String(buf + cn_ofs);

  _rted_CreateHeapPtr( msgr_TypeDesc  (buf + td_ofs),
                       msgr_Address   (buf + ad_ofs),
                       msgr_SizeOf    (buf + sz_ofs),
                       msgr_SizeOf    (buf + ma_ofs),
                       msgr_AllocKind (buf + ak_ofs),
                       msgr_String    (buf + cn_ofs),
                       msgr_SourceInfo(buf + si_ofs)
                     );
}

void rcv_InitVariable( const rted_MsgHeader* msg )
{
  const char*  buf = (const char*) msg;
  const size_t td_ofs = sizeof(rted_MsgHeader);
  const size_t ad_ofs = td_ofs + msglen_Typedesc(buf + td_ofs);
  const size_t sz_ofs = ad_ofs + sizeof(rted_Address);
  const size_t pc_ofs = sz_ofs + sizeof(size_t);
  const size_t ak_ofs = pc_ofs + sizeof(size_t);
  const size_t cn_ofs = ak_ofs + sizeof(rted_AllocKind);
  const size_t si_ofs = cn_ofs + msglen_String(buf + cn_ofs);

  _rted_InitVariable( msgr_TypeDesc  (buf + td_ofs),
                      msgr_Address   (buf + ad_ofs),
                      msgr_SizeOf    (buf + sz_ofs),
                      msgr_Int       (buf + pc_ofs),
                      msgr_AllocKind (buf + ak_ofs),
                      msgr_String    (buf + cn_ofs),
                      msgr_SourceInfo(buf + si_ofs)
                    );
}

void snd_CreateHeapPtr( rted_TypeDesc    td,
                        rted_Address     address,
                        size_t           size,
                        size_t           mallocSize,
                        rted_AllocKind   allocKind,
                        const char*      class_name,
                        rted_SourceInfo  si
                      )
{
  if (!shareHeapAllocInfo(allocKind, td)) return;

  const rted_szTypeDesc td_len = msgsz_TypeDesc(td);
  const size_t          td_ofs = sizeof(rted_MsgHeader);
  const size_t          ad_ofs = td_ofs + td_len.total;
  const size_t          sz_ofs = ad_ofs + sizeof(rted_Address);
  const size_t          ma_ofs = sz_ofs + sizeof(size_t);
  const size_t          ak_ofs = ma_ofs + sizeof(size_t);
  const size_t          cn_ofs = ak_ofs + sizeof(rted_AllocKind);
  const size_t          cn_len = strlen(class_name) + 1;
  const size_t          si_ofs = cn_ofs + cn_len;
  const size_t          si_len = msgsz_SourceInfo(si);
  const size_t          blocksz = si_ofs + si_len;
  char                  msg[blocksz];

  msgw_Header    (msg,          msgHeader(mskCreateHeapPtr, MYTHREAD, blocksz));
  msgw_TypeDesc  (msg + td_ofs, td, td_len);
  msgw_Address   (msg + ad_ofs, address);
  msgw_SizeOf    (msg + sz_ofs, size);
  msgw_SizeOf    (msg + ma_ofs, mallocSize);
  msgw_AllocKind (msg + ak_ofs, allocKind);
  msgw_String    (msg + cn_ofs, class_name, cn_len);
  msgw_SourceInfo(msg + si_ofs, si, si_len);

  msgBroadcast(msg, blocksz);
}


void snd_InitVariable( rted_TypeDesc   td,
                       rted_Address    address,
                       size_t          size,
                       int             pointer_changed,
                       rted_AllocKind  allocKind,
                       const char*     class_name,
                       rted_SourceInfo si
                     )
{
  // \pp \todo can we eliminate unneeded messages?
  // if (!shareHeapAllocInfo(allocKind, td)) return;

  // note, the following code is a clone from snd_CreateHeapPtr, with
  //       pointer_changed replacing mallocSize.
  const rted_szTypeDesc td_len = msgsz_TypeDesc(td);
  const size_t          td_ofs = sizeof(rted_MsgHeader);
  const size_t          ad_ofs = td_ofs + td_len.total;
  const size_t          sz_ofs = ad_ofs + sizeof(rted_Address);
  const size_t          pc_ofs = sz_ofs + sizeof(size_t);
  const size_t          ak_ofs = pc_ofs + sizeof(int);
  const size_t          cn_ofs = ak_ofs + sizeof(rted_AllocKind);
  const size_t          cn_len = strlen(class_name) + 1;
  const size_t          si_ofs = cn_ofs + cn_len;
  const size_t          si_len = msgsz_SourceInfo(si);
  const size_t          blocksz = si_ofs + si_len;
  char                  msg[blocksz];

  msgw_Header    (msg,          msgHeader(mskInitVariable, MYTHREAD, blocksz));
  msgw_TypeDesc  (msg + td_ofs, td, td_len);
  msgw_Address   (msg + ad_ofs, address);
  msgw_SizeOf    (msg + sz_ofs, size);
  msgw_Int       (msg + ma_ofs, pointer_changed);
  msgw_AllocKind (msg + ak_ofs, allocKind);
  msgw_String    (msg + cn_ofs, class_name, cn_len);
  msgw_SourceInfo(msg + si_ofs, si, si_len);

  msgBroadcast(msg, blocksz);
}


static
shared rted_MsgMasterBlock* msgMasterBlock(shared rted_MsgHeader* base, rted_MsgHeader msg)
{
  assert(base);

  shared char* _base = (shared char*) base;

  return (shared rted_MsgMasterBlock*) (_base + msg.threadno);
}

static
shared rted_MsgMasterBlock* msgMasterBlockRaw(shared char* base, const char* buf)
{
  assert(base && buf);

  return msgMasterBlock((shared rted_MsgHeader*)base, * ((rted_MsgHeader*)buf));
}

static
shared rted_MsgHeader* msgFirstBlock(shared rted_MsgHeader* myblock)
{
  shared char* myblockraw = (shared char*) myblock;

  return (shared rted_MsgHeader*) (myblockraw - MYTHREAD);
}

/// \brief marks the msg read; the last reading thread frees the memory
static
void msgRetire(shared rted_MsgHeader* m)
{
  shared rted_MsgHeader*      fb = msgFirstBlock(m);
  shared rted_MsgMasterBlock* mb = msgMasterBlock(fb, *m);

  upc_lock(mb->msg_lock);
  const size_t                early_reader = --(mb->unread_threads);
  upc_unlock(mb->msg_lock);

  // done, if this is not the last reader
  if (early_reader) return;

  // last reader frees it all
  upc_lock_free( mb->msg_lock );
  upc_free( fb );
}


// declared static by preceding declaration
void msgBroadcast(const char* msg, const size_t len)
{
  // to be freed by the last dequeuer in retire
  shared char*                tgtblock = upc_global_alloc(len, sizeof(char[len]));
  assert(tgtblock);

  // create a master block entry
  //   the lock is to be freed together with the block by retire
  shared rted_MsgMasterBlock* mb = msgMasterBlockRaw(tgtblock, msg);

  mb->unread_threads = THREADS - 1;
  mb->msg_lock = upc_global_lock_alloc();

  for (int i = 0; i < THREADS; ++i)
  {
    if (i != MYTHREAD)
    {
      assert(i - upc_threadof(tgtblock) == 0);

      upc_memput(tgtblock, msg, len);
      msgEnQueue(i, (shared rted_MsgHeader*) tgtblock);
    }

    // next thread
    ++tgtblock;
  }
}


static
void msgReceive()
{
  shared rted_MsgHeader* m = msgDeQueue();
  rted_MsgHeader*        msg = (rted_MsgHeader*)m;

  switch (msg->kind)
  {
    case mskFreeMemory:
      rcv_FreeMemory(msg);
      break;

    case mskCreateHeapPtr:
      rcv_CreateHeapPtr(msg);
      break;

    case mskInitVariable:
      rcv_InitVariable(msg);
      break;

    default:
      assert(0);
  }

  msgRetire(m);
}

void rted_ProcessMsg()
{
  while (!msgQueueEmpty())
  {
    msgReceive();
  }
}

void rted_UpcInitialize()
{
  msgQueue[MYTHREAD].head = NULL;
  msgQueue[MYTHREAD].tail = NULL;
  msgQueue[MYTHREAD].tail_lock = upc_global_lock_alloc();
}



#if OBSOLETE_CODE
static
size_t msgAlignBlockSize(size_t i)
{
  const size_t allignment = 4;
  const size_t a = allignment - 1;
  const size_t newsz = (i + a) & ~a;

  assert(newsz >= i);
  return newsz;
}
#endif /* OBSOLETE_CODE */
