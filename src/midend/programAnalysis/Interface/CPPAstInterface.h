#ifndef CPPAST_INTERFACE
#define CPPAST_INTERFACE

#include <outsideInterface/AstInterface.h>

class CPPAstInterface : public AstInterface
{
 public:
  static bool IsMemberFunctionCall( const AstNodePtr& n); 
  static AstNodePtr GetMemberFunctionCallObject( const AstNodePtr& n);
  
  static bool IsPointerVariable( const AstNodePtr& n); 
  static AstNodePtr GetVarScope( const AstNodePtr& n);

  static AstNodePtr GetLoopInit( const AstNodePtr& s);
  static AstNodePtr GetLoopTest( const AstNodePtr& s);
  static AstNodePtr GetLoopIncr( const AstNodePtr& s);

  static bool IsPlusPlus( const AstNodePtr& s);
  static AstNodePtr GetPlusPlusOpd( const AstNodePtr& s);
};

#endif
