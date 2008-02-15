#if 0
Hi Dan,

Your fix for the last bug worked, thanks. I have another report
(although this may be an example of a case where gcc differs from the
ANSI C specs since the error it produces makes perfect sense).  Anyway
the behavior when compiling with rose is different than the behavior
when compiling with gcc so you should probably be aware of it:

$ cat test.c
static int gVar;

extern void foo();

__inline__ void foo() {
   gVar = 5;
}

int main(char argc, char *argv[]) {


   return 1;
}

stonea@monk ~/ma/rose/gcctest
$ gcc test.c

stonea@monk ~/ma/rose/gcctest
$ rosec -rose:C_only test.c
"/s/bach/l/under/stonea/ma/rose/gcctest/test.c", line 6: error: an entity with
         internal linkage cannot be referenced within an inline function with
         external linkage
     gVar = 5;
     ^

"/s/bach/l/under/stonea/ma/rose/gcctest/test.c", line 1: warning: variable
         "gVar" was set but never used
 static int gVar;
            ^

Errors in EDG Processing: (edg_errorLevel > 3)
Aborted


-Andy 

#endif


static int gVar;

extern void foo();

__inline__ void foo() {
   gVar = 5;
}

int main(char argc, char *argv[]) {
   return 1;
}
