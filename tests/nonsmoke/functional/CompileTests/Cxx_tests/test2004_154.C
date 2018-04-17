#if 0
// DQ (8/14/2006):
// The problem with this program is that I don't 
// have a good way to test this sort of problem yet.

Dan,

On an unrelated note, I have found that if I have a C file that 
includes a file "heap.h" (ie #include "heap.h") the heap.h that
rose/edg pulls in is the heap.h under g++_HEADERS/hdrs3, even if you 
have something like this:

mytool -c -rose:C --edg:c99 -I../include fileThatIncludesHeap.c 

where heap.h exists in the directory ../include.

/*****fileThatIncludesHeap.c*********/
#include "heap.h"
/************************************/
/*****************heap.h*************/
int x;
/************************************/

//If I change the include line to be specific, ie '#include "../include/heap.h"',
//the correct file is found.  I am not sure where this error originates. 


Also, I have found that gnu-3.x-headerfile-fixup.pl places
C++ style comments (//) in the g++_HEADERS header files, which 
causes problems if those header files are included by a C file.
This is a problem I noticed with stddef.h

/*****fileThatIncludesStddef.c*********/
#include <stddef.h>
/************************************/

mytool -c -rose:C fileThatIncludesStddef.c

And rose-g++-headerfilefixup.h has C++ style comments.

Note that using the --edg:c99 command line option seems
to allow C code to contain C++ style comments.

thanks,
chadd
#endif

