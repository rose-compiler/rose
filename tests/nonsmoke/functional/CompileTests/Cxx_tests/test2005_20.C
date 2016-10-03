/*
Hi Dan,

As you asked for on Tuesday, here is a small program (main.cpp) calling the frontend twice to make two copies of the same sgproject.

Everything works fine when the input file ends in .c or .C (test with testfile.c).

If the input file ends in .cpp (test with testfile.cpp) the the following error message is displayed before aborting:

myexe: /home/hauge2/ROSE/src/frontend/EDG/EDG_3.3/src/cmd_line.c:2811: void proc_command_line(int, char**): Assertion `fileNameCounter == 1' failed.
Abort

Executing the code with the input file testfile.cpp demonstrates this error message.

The error message gives the file and the approximate line number where an if-test should be changed as to allow .cpp files.

main.cpp and the two testfiles are attached.

Vera

*/
#if 0
#include "rose.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



int main(int argc, char *argv[]){

  // making a copy of sgproject by calling the frontend twice
  SgProject* sgproject_original = frontend(argc,argv);
  SgProject* sgproject = frontend(argc, argv);

  return 0;
}


/*

If the input file ends in .c or .C everything works fine.
However, if the input file ends in .cpp then the execution aborts and the following error message is displayed:

myexe: /home/hauge2/ROSE/src/frontend/EDG/EDG_3.3/src/cmd_line.c:2811: void proc_command_line(int, char**): Assertion `fileNameCounter == 1' failed.
Abort

This error message gives the file and (approximate) line number where an if-test should be modified to allow calling frontend twice for a .cpp input file.

*/



/*
This test file is OK when calling frontend twice since it ends in .c.
*/

int main()
{
  int x;
  int y;
  int z;
  int w;
 
  return 0;
}



/*
This testfile is not OK when calling frontend twice since it ends in .cpp.
*/

int main()
{
  int x;
  int y;
  int z;
  int w;
 
  return 0;
}
#endif
