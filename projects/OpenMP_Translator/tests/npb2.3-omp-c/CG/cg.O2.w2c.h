/* Include builtin types and operators */
#include <whirl2c.h>

/* Types */
/* File-level vars and routines */
struct _IO_FILE_plus {
  _UINT8 dummy;
};
struct _IO_FILE {
  _INT32 _flags;
  _INT32 _fileno;
  _INT32 _flags2;
  _INT64 _old_offset;
  _UINT16 _cur_column;
  _INT8 _vtable_offset;
  _INT8 _shortbuf[1LL];
  _INT64 _offset;
  _INT32 _mode;
  _INT8 _unused2[44LL];
};
static _INT32 naa;

static _INT32 nzz;

static _INT32 firstrow;

static _INT32 lastrow;

static _INT32 firstcol;

static _INT32 lastcol;

static _INT32 colidx[2198001LL];

static _INT32 rowstr[14002LL];

static _INT32 iv[28002LL];

static _INT32 arow[2198001LL];

static _INT32 acol[2198001LL];

static _IEEE64 v[14002LL];

static _IEEE64 aelt[2198001LL];

static _IEEE64 a[2198001LL];

static _IEEE64 x[14003LL];

static _IEEE64 z[14003LL];

static _IEEE64 p[14003LL];

static _IEEE64 q[14003LL];

static _IEEE64 r[14003LL];

static _IEEE64 w[14003LL];

static _IEEE64 amult;

static _IEEE64 tran;

extern _INT32 main(_INT32, _INT8 **);

extern _INT32 printf(const _INT8 *, ...);

extern _IEEE64 randlc(_IEEE64 *, _IEEE64);

static void makea(_INT32, _INT32, _IEEE64 *, _INT32 *, _INT32 *, _INT32, _INT32, _INT32, _INT32, _INT32, _IEEE64, _INT32 *, _INT32 *, _IEEE64 *, _IEEE64 *, _INT32 *, _IEEE64);

static void conj_grad(_INT32 *, _INT32 *, _IEEE64 *, _IEEE64 *, _IEEE64 *, _IEEE64 *, _IEEE64 *, _IEEE64 *, _IEEE64 *, _IEEE64 *);

extern _IEEE64 sqrt(_IEEE64);

extern void timer_clear(_INT32);

extern void timer_start(_INT32);

extern _INT32 omp_get_num_threads(void);

extern void timer_stop(_INT32);

extern _IEEE64 timer_read(_INT32);

extern void c_print_results(_INT8 *, _INT8, _INT32, _INT32, _INT32, _INT32, _INT32, _IEEE64, _IEEE64, _INT8 *, _INT32, _INT8 *, _INT8 *, _INT8 *, _INT8 *, _INT8 *, _INT8 *, _INT8 *, _INT8 *, _INT8 *);

static void sprnvc(_INT32, _INT32, _IEEE64 *, _INT32 *, _INT32 *, _INT32 *);

static void vecset(_INT32, _IEEE64 *, _INT32 *, _INT32 *, _INT32, _IEEE64);

extern void exit(_INT32);

static void sparse(_IEEE64 *, _INT32 *, _INT32 *, _INT32, _INT32 *, _INT32 *, _IEEE64 *, _INT32, _INT32, _IEEE64 *, _INT32 *, _INT32 *, _INT32);

static _INT32 icnvrt(_IEEE64, _INT32);

static void __ompregion_main1();

extern _INT32 __ompc_static_init_4();

extern _INT32 __ompc_single();

extern _INT32 __ompc_end_single();

extern _INT32 __ompc_barrier();

_INT64 __mplock_1;

extern _INT32 __ompc_critical();

extern _INT32 __ompc_end_critical();

extern _INT32 __ompc_fork();

extern _INT32 __ompc_get_local_thread_num();

extern _INT32 __ompc_serialized_parallel();

extern _INT32 __ompc_end_serialized_parallel();

extern _INT32 __ompc_can_fork();

static void __ompregion_main2();

_INT64 __mplock_2;

extern _INT32 __ompc_master();

_INT64 __mplock_;

_INT64 __mplock_0;

_INT64 __mplock_3;

_INT64 __mplock_4;

static void __ompdo_makea1();

static void __ompdo_sparse1();

static void __ompdo_sparse2();

