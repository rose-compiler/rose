#if 0
/* A piece of code to reproduce compilation errors from spec cpu2006
    Liao 8/6/2010
*/

// spec_cpu2006/benchspec/CPU2006/444.namd/ComputeNonbondedUtil.C
typedef double BigReal;
void foo(int lesFactor)
{
   BigReal* br;

   br = new BigReal [lesFactor];
   int n = 0;
   br = new BigReal [n];
}

// example code from spec_cpu2006/benchspec/CPU2006/453.povray/messageoutput.cpp)
void bar (int x)
{
   char * c;
   c= new char[x];
   int n =0;
   c = new char[n];

}
------------- symptom ----------------
[liao6@tux284:~]identityTranslator -c bugx.cpp
"/home/liao6/bugx.cpp", line 9: warning: variable "br" was set but never used
     BigReal* br;
              ^

"/home/liao6/bugx.cpp", line 19: warning: variable "c" was set but never used
     char * c;
            ^

rose_bugx.cpp: In function 'void foo(int)':
rose_bugx.cpp:10: error: 'n' was not declared in this scope
rose_bugx.cpp: In function 'void bar(int)':
rose_bugx.cpp:20: error: 'n' was not declared in this scope



------------Output: rose_bugx.cpp--------------------------------

/* A piece of code to reproduce compilation errors from spec cpu2006
    Liao 8/6/2010
*/
// spec_cpu2006/benchspec/CPU2006/444.namd/ComputeNonbondedUtil.C
typedef double BigReal;

void foo(int lesFactor)
{
   BigReal *br;
   br = (new BigReal [n]);
   int n = 0;
   br = (new BigReal [n]);
}

// example code from spec_cpu2006/benchspec/CPU2006/453.povray/messageoutput.cpp)

void bar(int x)
{
   char *c;
   c = (new char [n]);
   int n = 0;
   c = (new char [n]);
}
#endif

// example code from spec_cpu2006/benchspec/CPU2006/453.povray/messageoutput.cpp)
void bar (int x)
   {
     float* f = new float[42];

     unsigned long unsignedLong = 12;
     double* d;
     d = new double[unsignedLong];

     char * c;
     c = new char[x];
     int n =0;
     c = new char[n];
   }
