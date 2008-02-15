
#ifndef ___CPP_ANNOTATION_H
#define ___CPP_ANNOTATION_H

#include <TypeAnnotation.h>
#include <CPPAstInterface.h>
#include <SymbolicVal.h>

template <class Descriptor>
class CPPTypeCollection 
{
  TypeCollection<Descriptor>* cp;
 public:
  CPPTypeCollection(TypeCollection<Descriptor>* p) : cp(p) {}
  typedef typename TypeCollection<Descriptor>::const_iterator const_iterator;
  std::string is_known_member_function( CPPAstInterface& fa, const AstNodePtr& exp, 
                                   AstNodePtr* obj = 0, 
                                   AstInterface::AstNodeList* args = 0,
                                   Descriptor* desc = 0);
  std::string is_known_member_function( CPPAstInterface& fa, const SymbolicVal& exp,
                                   AstNodePtr* obj = 0,
                                   SymbolicFunction::Arguments* args = 0,
                                   Descriptor* desc = 0);
  SymbolicVal create_known_member_function( const AstNodePtr& obj, const std::string& memname,
                                     const SymbolicFunction::Arguments& args);
  AstNodePtr create_known_member_function( CPPAstInterface& fa, const AstNodePtr& obj, 
                                           const std::string& memname,
                                            const AstInterface::AstNodeList& args);
};

#endif
