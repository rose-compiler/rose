/*
Dan,

I sent you a few bugs a few bugs a couple weeks ago - here are 1.5
more (one seems to be AST construction, the second farther down is
OpenMP related).  Both of these are against the latest up on SciDAC
(0.9.4a-multiplatform-7810).

Compiling the following code snippet (also attached) using even the
identity unparser:

---------------------------------------------
/* wait.c */
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
  int err;
  wait(&err);

  return 0;
}
---------------------------------------------

unparses to something fairly odd:
---------------------------------------------
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc,char **argv)
{
  int err;
  wait(.__iptr = (&err));
  return 0;
}
---------------------------------------------


I wasn't sure what to make of that - I modified the affected code to
use waitpid for now, but thought you would want to know.



The other bug relates to the OpenMP translation code.  Specifically
-rose:openmp:lowering (thanks for that mode by the way, it makes my
life a lot easier).  Compiling with that flag was giving me linking
errors pretty consistently due to multiply defined names when I had
OpenMP code in multiple compilation units.

My fix was to force the outlined portions of functions (from parallel
regions) to be declared static, and I've attached the patch I've been
using (apply with patch -p1 < rose_static_omp.patch from the rose root
directory)..

It only modifies src/midend/ompLowering/omp_lowering.cpp since I don't
know what other contexts the outlining code is used in, and if making
all outlined functions static would be good or bad.

Thanks,
--
Tristan Ravitch
travitch@cs.wisc.edu
http://pages.cs.wisc.edu/~travitch
*/


#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
  int err;
  wait(&err);

  return 0;
}
