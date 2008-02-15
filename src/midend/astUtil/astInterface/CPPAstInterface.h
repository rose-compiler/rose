#ifndef CPPAST_INTERFACE
#define CPPAST_INTERFACE

#include <AstInterface.h>

class CPPAstInterface : public AstInterface
{
 public:
  CPPAstInterface(AstInterfaceImpl* _impl) : AstInterface(_impl) {}
  bool IsMemberAccess( const AstNodePtr& n, AstNodePtr* obj = 0,
                                         STD string* fieldname = 0);

  bool IsMemberFunctionCall( const AstNodePtr& n, 
                                       AstNodePtr* obj = 0,
	                               STD string* funcname = 0, 
                                       AstNodePtr* access = 0,
                                       AstInterface::AstNodeList* args = 0);
  AstNodePtr CreateFunctionCall( const AstNodePtr& func, 
                          const AstInterface::AstNodeList& args);
  bool IsPointerVariable( const AstNodePtr& n); 
  AstNodePtr GetVarScope( const AstNodePtr& n);

  bool IsPlusPlus( const AstNodePtr& s, AstNodePtr* opd = 0);
};

#endif
