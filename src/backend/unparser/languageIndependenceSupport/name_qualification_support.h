
/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_NAME_QUALIFICATION
#define UNPARSER_NAME_QUALIFICATION

class SgType;
class SgDeclarationStatement;
class SgName;
class SgInitializedName;

// #include "rose.h"
// #include "unparser_opt.h"
// #include "unparser.h"
// #include "unparse_type.h"

class Unparser;

class Unparser_Nameq
   {
  // This class has the low level support for name qualification.

     private:
       // DQ (3/28/2017): Eliminate warning about unused variable from Clang.
       // Unparser* unp;

     public:
       // DQ (3/28/2017): Eliminate warning about unused variable from Clang.
       // Unparser_Nameq(Unparser* unp):unp(unp){};
          Unparser_Nameq(Unparser*) { };
          virtual ~Unparser_Nameq() {};

       // DQ (6/2/2011): New name qualification support.
         SgName lookup_generated_qualified_name ( SgNode* n );

      // DQ (3/14/2019): Adding debugging support to output the map of names.
         void outputNameQualificationMap( const std::map<SgNode*,std::string> & qualifiedNameMap );

   };

#endif

