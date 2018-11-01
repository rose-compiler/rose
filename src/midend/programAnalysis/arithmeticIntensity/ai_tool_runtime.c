#include "ai_tool_runtime.h"

int * _loop_counters;
int _loop_count;
char* _ofilename;

static void allocAndCopyStr(char** dest, const char* src)
{
  *dest = (char*) malloc((strlen(src)+1)*sizeof(char));
  strcpy (*dest, src);
  assert (strlen(src) == strlen (*dest));
}

void ai_runtime_init(char* outputfile_name, int loop_count)
{
  // allocate and initialize counters
  assert (loop_count>0);
  _loop_count = loop_count;
  int _i;
  _loop_counters = (int*) malloc (loop_count *sizeof(int));
  for ( _i =0; _i< loop_count ; _i++)
  {
    _loop_counters[_i] =  0;
  }
  // copy the file name
  allocAndCopyStr (&_ofilename, outputfile_name);
}

void ai_runtime_terminate()
{
  // trans
  // write results into a file
  FILE* o_file;
  o_file = fopen (_ofilename, "a+");
  if(o_file != NULL)
  {
    int ii;
    for (ii=0; ii< _loop_count; ii++)
    {
      fprintf(o_file, "loop %d : iteration count: %d\n", ii, _loop_counters[ii]);
    }
  }
  else
    assert (0);
  fclose (o_file);
  // terminate the monitoring
  free (_loop_counters);
}
