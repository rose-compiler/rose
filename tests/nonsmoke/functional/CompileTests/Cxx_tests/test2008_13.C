// from /home/liao6/opt/spec_cpu2006/benchspec/CPU2006/400.perlbench/build/build_base_i386-m32-gcc42-nn.0001/doio.c
int foo(int i)
{
#ifdef VMS
if (i==0)
{
// DQ (12/16/2008): This directive has a tab after the '#' which is a bug now fixed.
#	     include <starlet.h>
  i++;
}
#endif

return i;
}
