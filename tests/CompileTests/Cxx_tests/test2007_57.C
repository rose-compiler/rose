#if 0
Hi Dan,

I've found a case of something that will compile in gcc but not in
ROSE.  This is preventing me from compiling one of the SPEC benchmarks
(400.perlbench).  Here's the example:

stonea@monk ~/ma/rose/perltestcase
$ cat test.cpp
int main(char argc, char *argv[]) {
   char wbuf[0xFF];

   if(*wbuf) {

   }
}

stonea@monk ~/ma/rose/perltestcase
$ g++ test.cpp

stonea@monk ~/ma/rose/perltestcase
$ rosec test.cpp
"/s/bach/l/under/stonea/ma/rose/perltestcase/test.cpp", line 4: warning:
         variable "wbuf" is used before its value is set
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


int main(char argc, char *argv[]) {
   char wbuf[0xFF];

   if(*wbuf) {

   }
}

