#if 0
Hi Dan

Ive found a case of something that will compile in gcc but not in
ROSE.  This is preventing me from compiling one of the SPEC benchmarks
(400.perlbench).  Here is the example:

/ma/rose/perltestcase
cat test.cpp
int main(char argc, char *argv[]) {
   char wbuf[0xFF];

   if(*wbuf) {

   }
}

/ma/rose/perltestcase
g++ test.cpp

/ma/rose/perltestcase
rosec test.cpp
/s/bach/l/under/stonea/ma/rose/perltestcase/test.cpp, line 4: warning: variable wbuf is used before its value is set
     if(*wbuf) {
         ^

is_array_decay_cast(node) == true, handling case of cast that does
array-to-pointer decay
rosec: Cxx_Grammar.C:21860: void
SgLocatedNode::setCompilerGenerated(): Assertion
get_startOfConstruct() != __null failed.
Aborted

Thanks,
-Andy

#endif

// DQ (9/12/2009): GNU G++ version 4.3 and 4.4 enforce that first argument of main be an "int" (if there are non-zero arguments).
// int main(char argc, char *argv[])
int
main(int argc, char *argv[])
   {
     char wbuf[0xFF];

     if(*wbuf) {

     }
   }

