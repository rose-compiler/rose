#include "hclient.h"
#include "hsockutil.h"
static int har_registration = 0;
static int *unroll =NULL;

int
hypre_SMGResidual(..)
{
  // initialize the search engine for the first call 
  if(har_registration == 0)
  {
    printf("Starting Harmony...\n");
    harmony_startup(0);
    printf("Sending setup file!\n");
    harmony_application_setup_file("harmony.tcl");
    printf("Adding a harmony variable ....");
    unroll=(int *)harmony_add_variable("smg2000Unroll","unroll",VAR_INT);
    har_registration++;
  }

  //Load the .so files
  char nextUnroll[255];
  if (g_execution_flag == 0)
  {
    /* Only load the lib for the first time, reuse it later on */
    printf("Opening the .so file ...\n");
    FunctionLib = dlopen("./unrolled_versions.so",RTLD_LAZY);
    dlError = dlerror();
    if( dlError ) {
      printf("cannot open .so file!\n");
      exit(1);
    }
  } // end if (flag ==0)

  //Use the value provided Harmony to find a kernel variant
  sprintf(nextUnroll, "OUT__1__6755__%d",*unroll);
  printf("Trying to find: %s ...\n",nextUnroll);
  OUT__1__6755__ = (void (*)(void**)) dlsym( FunctionLib, nextUnroll);

  // timing the execution of a variant
  time1=time_stamp();
  //.. parameter wrapping is omitted here
  (*OUT__1__6755__)(__out_argv1__1527__);
  time2=time_stamp();
  int perf = (int) ((time2-time1)  * 100000);
  printf("Trying to send performance information:%d...\n",perf);
  // send performance feedback to Harmony server
  harmony_performance_update(perf);
  // get the next point of unroll from the server
  harmony_request_all();
  printf("done with one iteration :\n");


  //...
}
