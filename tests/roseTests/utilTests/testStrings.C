#include <rose_config.h>

#include "string_functions.h"

using namespace std;


void stringTest ( string s )
   {
     string s_filename = StringUtility::stripPathFromFileName(s);
     string s_path     = StringUtility::getPathFromFileName(s);
     string s_nosuffix = StringUtility::stripFileSuffixFromFileName(s);
     printf ("s = %s s_filename = %s \n",s.c_str(),s_filename.c_str());
     printf ("s = %s s_path     = %s \n",s.c_str(),s_path.c_str());
     printf ("s = %s s_nosuffix = %s \n",s.c_str(),s_nosuffix.c_str());
     printf ("\n");
   }

int
main()
   {
  // Test code for string utility library

  // string X = "XXX1Y XXX1Y XXX2Y XXX1Y";
  // string X = "\nXXX1Y\nXXX1Y\nXXX2Y\nXXX1Y";
  // string X = "XXX1Y\nXXX1Y\nXXX2Y\nXXX1Y\n";
#if 0
     string X = "\
ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D1(_A)\n\
ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D6(_A)\n\
ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D1(_A)\n\
ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D1(_A_I)\n\
ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D6(_A)\n\
LOOP_INDEX_VARIABLES_DECLARATION_MACRO_D1();\n\
LOOP_INDEX_VARIABLES_DECLARATION_MACRO_D6();\n\
";
#else

#if 0
     string X = "\
   ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D6(A); \
ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D6(A); \
   _A_pointer = A.getAdjustedDataPointer(); \
   \"ROSE-TRANSFORMATION-MACRO:#define SC(x1,x2,x3,x4,x5,x6) /* case UniformSizeUnitStride */ (x1)+(x2)*_size1+(x3)*_size2+(x4)*_size3+(x5)*_size4+(x6)*_size5\"; \
";
#else
     string X = "ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D6(A);";
#endif
#endif

     printf ("X = \n%s\n",X.c_str());

     string Y = StringUtility::removePseudoRedundentSubstrings ( X );

     printf ("Y = \n%s\n",Y.c_str());

     
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

     printf ("Program Terminated Normally! \n");
     return 0;
   }





