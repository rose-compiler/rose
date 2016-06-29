#include <rose_config.h>

#include "FileUtility.h"
#include "StringUtility.h"

using namespace std;
using namespace rose;


void stringTest ( string s )
   {
     string s_absolute = StringUtility::getAbsolutePathFromRelativePath(s);
     string s_filename = StringUtility::stripPathFromFileName(s);
     string s_path     = StringUtility::getPathFromFileName(s);
     string s_nosuffix = StringUtility::stripFileSuffixFromFileName(s);

     printf ("s = %s s_absolute = %s \n",s.c_str(),s_absolute.c_str());
     printf ("s = %s s_filename = %s \n",s.c_str(),s_filename.c_str());
     printf ("s = %s s_path     = %s \n",s.c_str(),s_path.c_str());
     printf ("s = %s s_nosuffix = %s \n",s.c_str(),s_nosuffix.c_str());
     printf ("\n");
   }

int
main ( int argc, char* argv[] )
   {
  // Test code for string utility library

     string currentExecutable = argv[0];
     printf ("currentExecutable = %s \n",currentExecutable.c_str());
     stringTest (currentExecutable);

     currentExecutable = "./" + currentExecutable;
     printf ("currentExecutable = %s \n",currentExecutable.c_str());
     stringTest (currentExecutable);

     currentExecutable = "././" + currentExecutable;
     printf ("currentExecutable = %s \n",currentExecutable.c_str());
     stringTest (currentExecutable);

#if 0
     stringTest ("foo.h");
     stringTest ("/foo.h");
     stringTest ("//foo.h");
     stringTest ("///foo.h");
     stringTest ("////foo.h");
     stringTest ("./foo.h");
     stringTest ("../foo.h");
     stringTest ("//foo.h");
     stringTest ("path/foo.h");
     stringTest ("/path/foo.h");
     stringTest ("/pathA/pathB/foo.h");
     stringTest ("foo");
     stringTest ("/path/foo");
#endif

     printf ("Program Terminated Normally! \n");
     return 0;
   }

