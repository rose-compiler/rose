
#include "OperatorAnnotation.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

OperatorInlineAnnotation* OperatorInlineAnnotation::inst = 0;
OperatorSideEffectAnnotation* OperatorSideEffectAnnotation::inst = 0;
OperatorAliasAnnotation* OperatorAliasAnnotation::inst = 0;

bool OperatorInlineAnnotation::
known_operator( AstInterface& fa, const AstNodePtr& arrayExp, SymbolicVal* val) const
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

bool OperatorSideEffectAnnotation::
get_modify( AstInterface& fa, const AstNodePtr& fc,
                CollectObject< AstNodePtr >* collect)
{
  AstInterface::AstNodeList args;
  OperatorSideEffectDescriptor mod;
  if (modInfo.known_operator( fa, fc, &args, &mod)) {
    if (collect != 0)
      mod.get_side_effect(fa, args, *collect);
    return true;
  }
  return false;
}

bool OperatorSideEffectAnnotation::
get_read( AstInterface& fa, const AstNodePtr& fc,
                CollectObject< AstNodePtr >* collect)
{
  AstInterface::AstNodeList args;
  OperatorSideEffectDescriptor read;
  if  (readInfo.known_operator( fa, fc, &args, &read)) {
       if (collect != 0)
         read.get_side_effect(fa, args, *collect);
       return true;
   }
  return false;
}

static bool
AliasAnnotAnal(AstInterface& fa,
               OperatorAnnotCollection <OperatorAliasDescriptor>& aliasInfo,
               const AstNodePtr& fc, const AstNodePtr& result,
               CollectObject< pair<AstNodePtr, int> >& collectalias)
{
  AstInterface::AstNodeList args;
  OperatorAliasDescriptor desc;
  if (!aliasInfo.known_operator( fa, fc, &args, &desc, false) )
    return false;
  ReplaceParams paramMap( desc.get_param_decl().get_params(), args);
  paramMap.add("result", result);
  int index = 0;
  for (OperatorAliasDescriptor::const_iterator p1 = desc.begin();
       p1 != desc.end(); ++p1, ++index) {
    const NameGroup& cur = *p1;
    for (NameGroup::const_iterator p2 = cur.begin();
         p2 != cur.end(); ++p2) {
      string varname = *p2;
      AstNodePtr arg = paramMap.find(varname).get_ast();
      if (arg != AST_NULL) {
        collectalias( pair<AstNodePtr, int>(arg, index));
      }
      else {
        AstNodePtr var = fa.CreateVarRef(varname);
        collectalias( pair<AstNodePtr, int>(var, index));
      }
    }
  }
  return true;
}

bool OperatorAliasAnnotation::
may_alias(AstInterface& fa, const AstNodePtr& fc, const AstNodePtr& result,
               CollectObject< pair<AstNodePtr, int> >& collectalias)
{
  return AliasAnnotAnal( fa, aliasInfo, fc, result, collectalias);
}

bool OperatorAliasAnnotation::
allow_alias(AstInterface& fa, const AstNodePtr& fc,
            CollectObject< pair<AstNodePtr, int> >& collectalias)
{
  return AliasAnnotAnal( fa, allowaliasInfo, fc, AST_NULL, collectalias);
}

#define TEMPLATE_ONLY
#include <TypeAnnotation.C>
template class ReadAnnotCollection<OperatorDeclaration, '{', ';','}'>;
#if 0
// DQ (1/8/2006): This needs to be placed AFTER AnnotDescriptors.C is included (fix appears specific to g++ 4.0.2)
template class ReadContainer<ParameterDeclaration,
                             CollectPair<TypeDescriptor, NameDescriptor,0>,
                             ',', '(', ')'>;
#endif
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
#if 0
// DQ (1/8/2006): This does not appear to be required (unless it is type equivalant to the one using ParameterDeclaration
template class ReadContainer<vector<CollectPair<TypeDescriptor, NameDescriptor, 0> >,
                              CollectPair<TypeDescriptor, NameDescriptor, 0>,
                              ',', '(', ')'>;
#endif
template class WriteContainer<vector<CollectPair<TypeDescriptor, NameDescriptor, 0> >, ',', '(', ')'>;

// DQ (1/8/2006): force instantiation of this template so that the "read" member function will be available (required for g++ 4.0.2)
// This is reported as a duplicate template instantiation directive if the previousl on before the AnnotDescriptors.C is include is not commented out.
template class ReadContainer<ParameterDeclaration, CollectPair<TypeDescriptor, NameDescriptor,0>, ',', '(', ')'>;
