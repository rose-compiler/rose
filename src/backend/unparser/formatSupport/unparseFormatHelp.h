
/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_FHELP
#define UNPARSER_FHELP

// #include "rose.h"
#include "unparser.h"
class SgUnparse_Info;
class SgLocatedNode;
class Unparser;

class ROSE_DLL_API UnparseFormatHelp
   {
  // This class provides low level functions to control how we get line number 
  // and column number information.  All formating information comes
  // from this class and the user can then control the formatting of source 
  // code by deriving their own class from this one.  Our goal is not particularly
  // to represent the most complex pretty printing by to allow the output to be
  // easily tailored separately from the implementation of the code generation.

     public:
          virtual ~UnparseFormatHelp();

       // return: > 0: start new lines; == 0: use same line; < 0:default
          virtual int getLine( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt);

       // return starting column. if < 0, use default
          virtual int getCol( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt);

      // return the value for indentation of code (part of control over style)
      // virtual int tabIndent (SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt);
         virtual int tabIndent();

      // return the value for indentation of code (part of control over style)
      // virtual int maxLineLength (SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt);
         virtual int maxLineLength ();
   };


#endif

