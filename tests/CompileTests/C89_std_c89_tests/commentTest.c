#if 0
Bug report from Chadd:

Also, I have found that gnu-3.x-headerfile-fixup.pl places
C++ style comments (//) in the g++_HEADERS header files, which 
causes problems if those header files are included by a C file.
This is a problem I noticed with stddef.h

/*****fileThatIncludesStddef.c*********/
#include <stddef.h>
/************************************/
#endif

#include <stddef.h>

