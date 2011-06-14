#ifndef CPPAST_INTERFACE
#define CPPAST_INTERFACE

#include "AstInterface.h"

class CPPAstInterface : public AstInterface
{
 public:
  CPPAstInterface(AstInterfaceImpl* _impl) : AstInterface(_impl) {}

  //! Check if $n$ is a data member access operator; If yes, grab the object and the field name
  bool IsMemberAccess( const AstNodePtr& n, AstNodePtr* obj = 0,
                                         std::string* fieldname = 0);

  //Check if $_s$ is a method call; if yes, grab relevant info.
  bool IsMemberFunctionCall( const AstNodePtr& n, 
                                       AstNodePtr* obj = 0,
	                               std::string* funcname = 0, 
                                       AstNodePtr* access = 0,
                                       AstInterface::AstNodeList* args = 0);
  AstNodePtr CreateFunctionCall( const AstNodePtr& func, 
                          const AstInterface::AstNodeList& args);
  //! Check whether $n$ is a pointer or reference variable reference.
  bool IsPointerVariable( const AstNodePtr& n); 
  AstNodePtr GetVarScope( const AstNodePtr& n);

  bool IsPlusPlus( const AstNodePtr& s, AstNodePtr* opd = 0);
};

#endif
