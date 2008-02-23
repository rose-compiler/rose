
DQ: THIS PROGRAM IS NOT USED!!!

Instead there is a script of the same name which is used!


#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <string>

using namespace std;

bool TestFile( string name, string srcdir)
   {
     string compare = "diff -U5 " + srcdir + "/" + name + " " + name + " > ./diffout2";
     cerr << compare << endl;
     bool  fail = system(compare.c_str());
     if (fail)
        {
          printf ("Failing Test: compare = %s \n",compare.c_str());
          return false;
        }

  // DQ: What is this code about!
     ifstream in2("diffout2");
     char c;
     in2 >> c;
     if (in2.eof())
        {
          string cleanup = "rm " + name + "; rm diffout2;";
          system(cleanup.c_str());
          return true;
        }
       else
        {
          printf ("Failing Test of in2 \n");
          return false;
        }
   }

int main( int argc, char* argv[])
{
  assert(argc == 2);
  string srcdir = string(argv[1]);

// DQ (4/9/2005): Added "./" to permit use of ROSE without "." in $PATH (suggested by Rich)
  string proc1 = "./CFGTest -I" + srcdir + " " + srcdir + "/testfile1.c" ;
  cerr << proc1 << endl;
  if (system(proc1.c_str()) )
     return -1;
  system("mv testfile1.c.out testfile1.c.cfg");
  if (!TestFile("testfile1.c.cfg", srcdir))
      return -1;

// DQ (4/9/2005): Added "./" to permit use of ROSE without "." in $PATH (suggested by Rich)
  string proc2 = "./DataFlowTest -I" + srcdir + " " + srcdir + "/testfile1.c" ;
  cerr << proc2 << endl;
  if (system(proc2.c_str()) )
     return -1;
  system("mv testfile1.c.out testfile1.c.du");
  if (!TestFile("testfile1.c.du", srcdir))
      return -1;

// DQ (4/9/2005): Added "./" to permit use of ROSE without "." in $PATH (suggested by Rich)
  string proc3 = "./StmtRefTest -I" + srcdir + " " + srcdir + "/testfile1.c" ;
  cerr << proc3 << endl;
  if (system(proc3.c_str()) )
     return -1;
  system("mv testfile1.c.out testfile1.c.ref");
  if (!TestFile("testfile1.c.ref", srcdir))
      return -1;

  return 0;
} 
