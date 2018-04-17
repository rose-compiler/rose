/* #include "dlfcn2.h" */

typedef struct
{
  void *dli_fbase;		/* Load address of that object.  */
} Dl_info;

void DumpStackToFile()
{
    void *pc;
    Dl_info info;
    int foff = (char*)pc - (char*)info.dli_fbase;

}
