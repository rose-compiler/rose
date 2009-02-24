
/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_DEBUG
#define UNPARSER_DEBUG


#include "unparser.h"

class Unparser;

class Unparse_Debug {
 private:
  Unparser* unp;
 public:
  Unparse_Debug(Unparser* unp):unp(unp){};
  virtual ~Unparse_Debug() {};

          void printDebugInfo(int, bool);
          void printDebugInfo(const std::string&, bool);

};

#endif


