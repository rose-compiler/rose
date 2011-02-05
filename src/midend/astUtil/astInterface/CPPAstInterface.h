#ifndef CPPAST_INTERFACE
#define CPPAST_INTERFACE

#include "AstInterface.h"

class CPPAstInterface : public AstInterface
{
 public:
  CPPAstInterface(AstInterfaceImpl* _impl) : AstInterface(_impl) {}
  //! Check if a node is a data member access function for an object, 
  // such as dot (object.a) or arrow (objectptr->a)expression
  // If yest, return the object, and the data field name
  bool IsMemberAccess( const AstNodePtr& n, AstNodePtr* obj = 0,
                                         std::string* fieldname = 0);

  //Check if a node '_s' is a member function call of an object
  // If yes, store the object into 'obj', member function name into 'func', function arguments into '_args'
  // and the dot or arrow expressions nodes into 'access'
  bool IsMemberFunctionCall( const AstNodePtr& n, 
                                       AstNodePtr* obj = 0,
                                       std::string* funcname = 0, 
                                       AstNodePtr* access = 0,
                                       AstInterface::AstNodeList* args = 0);
  AstNodePtr CreateFunctionCall( const AstNodePtr& func, 
                          const AstInterface::AstNodeList& args);
  //! Check if a node is a variable reference to pointer or C++ reference type variables
  bool IsPointerVariable( const AstNodePtr& n); 
  AstNodePtr GetVarScope( const AstNodePtr& n);

  bool IsPlusPlus( const AstNodePtr& s, AstNodePtr* opd = 0);
};

#endif
