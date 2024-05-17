
#include "OperatorAnnotation.h"
#include <ROSE_ASSERT.h>
#include "CommandOptions.h"

DebugLog DebugOperatorAnnotation("-debugopa");

OperatorInlineAnnotation* OperatorInlineAnnotation::inst = 0;
OperatorSideEffectAnnotation* OperatorSideEffectAnnotation::inst = 0;
OperatorAliasAnnotation* OperatorAliasAnnotation::inst = 0;

bool OperatorInlineAnnotation::
known_operator( AstInterface& fa, const AstNodePtr& arrayExp, SymbolicVal* val) 
{
  typedef OperatorAnnotCollection<OperatorInlineDescriptor> BaseClass;
  if (val == 0)
     return BaseClass::known_operator( fa, arrayExp);
  AstInterface::AstNodeList args;
  OperatorInlineDescriptor desc;
  if (!BaseClass::known_operator( fa, arrayExp, &args, &desc, true))
    return false;
  *val = desc.get_val();
  return true;
}

bool OperatorInlineAnnotation::
get_inline( AstInterface& fa, const AstNodePtr& h, SymbolicVal* val)
{
  return known_operator( fa, h, val);
}

static bool
AliasAnnotAnal(AstInterface& fa,
               OperatorAnnotCollection <OperatorAliasDescriptor>& aliasInfo,
               const AstNodePtr& fc, const AstNodePtr& result,
               CollectObject< std::pair<AstNodePtr, int> >& collectalias)
{
  AstInterface::AstNodeList args;
  OperatorAliasDescriptor desc;
  if (!aliasInfo.known_operator( fa, fc, &args, &desc, false) )
    return false;
  if (desc.get_param_decl().get_params().size() != args.size()) {
    DebugOperatorAnnotation("AliasAnnotationAnalysis: Parameter and argument sizes are different. Return false.");
    return false;
  }
  ReplaceParams paramMap( desc.get_param_decl(), args);
  paramMap.add("result", result);
  int index = 0;
  for (OperatorAliasDescriptor::const_iterator p1 = desc.begin();
       p1 != desc.end(); ++p1, ++index) {
    const NameGroup& cur = *p1;
    for (NameGroup::const_iterator p2 = cur.begin();
         p2 != cur.end(); ++p2) {
      std::string varname = *p2;
      AstNodePtr arg = paramMap.find(varname).get_ast();
      if (arg != AST_NULL) {
        collectalias( std::pair<AstNodePtr, int>(arg, index));
      }
      else {
        AstNodePtr var = fa.CreateVarRef(varname);
        collectalias( std::pair<AstNodePtr, int>(var, index));
      }
    }
  }
  return true;
}

bool OperatorAliasAnnotation::
may_alias(AstInterface& fa, const AstNodePtr& fc, const AstNodePtr& result,
               CollectObject< std::pair<AstNodePtr, int> >& collectalias)
{
  return AliasAnnotAnal( fa, aliasInfo, fc, result, collectalias);
}

bool OperatorAliasAnnotation::
allow_alias(AstInterface& fa, const AstNodePtr& fc,
            CollectObject< std::pair<AstNodePtr, int> >& collectalias)
{
  return AliasAnnotAnal( fa, allowaliasInfo, fc, AST_NULL, collectalias);
}

#define TEMPLATE_ONLY
#include <TypeAnnotation.C>
template class ReadAnnotCollection<OperatorDeclaration, '{', ';','}'>;
template class TypeCollection<BoolDescriptor>;
template class TypeAnnotCollection<BoolDescriptor>;
template class TypeCollection<OperatorInlineDescriptor>;
template class TypeCollection<OperatorAliasDescriptor>;
template class TypeCollection<OperatorSideEffectDescriptor>;
template class OperatorAnnotCollection<OperatorSideEffectDescriptor>;
template class OperatorAnnotCollection<OperatorAliasDescriptor>;
template class OperatorAnnotCollection<OperatorInlineDescriptor>;


#include <AnnotDescriptors.C>
template class WriteContainer<set<NameDescriptor>, ',', '(', ')'>;
template class WriteContainer<vector<NameDescriptor>, ',', '(', ')'>;
template class ReadContainer<SetDescriptor<NameDescriptor, ',', '{', '}'>,
                             NameDescriptor, ',','{','}'>;
template class WriteContainer<vector<CollectPair<TypeDescriptor, NameDescriptor, 0> >, ',', '(', ')'>;

// DQ (1/8/2006): force instantiation of this template so that the "read" member function will be available (required for g++ 4.0.2)
// This is reported as a duplicate template instantiation directive if the previousl on before the AnnotDescriptors.C is include is not commented out.
template class ReadContainer<ParameterDeclaration, CollectPair<TypeDescriptor, NameDescriptor,0>, ',', '(', ')'>;
