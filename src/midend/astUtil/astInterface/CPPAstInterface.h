#ifndef CPPAST_INTERFACE
#define CPPAST_INTERFACE

#include <AstInterface.h>

class CPPAstInterface : public AstInterfaceBase
{
 public:
//static Boolean IsMemberAccess( const AstNodePtr& n, AstNodePtr* obj = 0,
  static int IsMemberAccess( const AstNodePtr& n, AstNodePtr* obj = 0,
                                         AstNodePtr* func = 0);

//static Boolean IsMemberFunctionCall( const AstNodePtr& n, 
  static int IsMemberFunctionCall( const AstNodePtr& n, 
                                       AstNodePtr* obj = 0,
	                               AstNodePtr* func = 0, 
                                       AstNodePtr* access = 0,
                                       AstInterface::AstNodeList* args = 0);
  
//static Boolean IsPointerVariable( const AstNodePtr& n); 
  static int IsPointerVariable( const AstNodePtr& n); 
  static AstNodePtr GetVarScope( const AstNodePtr& n);

//static Boolean IsPlusPlus( const AstNodePtr& s, AstNodePtr* opd = 0);
  static int IsPlusPlus( const AstNodePtr& s, AstNodePtr* opd = 0);
};

#endif
