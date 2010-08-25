#if BLCR_CHECKPOINTING
//#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libcr.h"
#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

static int g_checkpoint_flag = 0;
extern cr_client_id_t cr;

#endif

int
hypre_SMGResidual (void *residual_vdata,
                   hypre_StructMatrix * A,
		   hypre_StructVector * x,
		   hypre_StructVector * b, hypre_StructVector * r)
{
  //.............
#if BLCR_CHECKPOINTING
  int err;
  cr_checkpoint_args_t cr_args;
  cr_checkpoint_handle_t cr_handle;
  cr_initialize_checkpoint_args_t (&cr_args);
  cr_args.cr_scope = CR_SCOPE_PROC;	// checkpoint an entire process
  cr_args.cr_target = 0;	//checkpoint self
  cr_args.cr_signal = SIGKILL;	// kill self after checkpointing
  cr_args.cr_fd = open ("dump.yy", O_WRONLY | O_CREAT | O_LARGEFILE, 0400);
  if (cr_args.cr_fd < 0)
  {
    printf ("Error: cannot open file for checkpoiting context\n");
    abort ();
  }

  printf ("Checkpointing: stopping here ..\n");

  err = cr_request_checkpoint (&cr_args, &cr_handle);
  if (err < 0)
  {
    printf ("cannot request checkpoining! err=%d\n", err);
    abort ();
  }
  // block self until the request is served
  cr_enter_cs (cr);
  cr_leave_cs (cr);

  printf ("Checkpointing: restarting here ..\n");
  FunctionLib =dlopen("OUT__1__6755__.so", RTLD_LAZY);
  // ignore the code to find the outlined function and to time the execution here
  // ...
  (*OUT__1__6755__)(__out_argv1__1527__);
  // timing code ignored here
  // ...
  // stop after the target finishes its execution   
  exit(0);

#endif

  // .........
}
