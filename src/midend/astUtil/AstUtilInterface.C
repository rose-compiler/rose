#include "AstUtilInterface.h"
#include "StmtInfoCollect.h"
#include "AstInterface_ROSE.h"
#include "annotation/OperatorAnnotation.h"
#include "CommandOptions.h"
#include "SymbolicVal.h"

DebugLog DebugAstUtil("-debugastutil");

bool AstUtilInterface::ComputeAstSideEffects(SgNode* ast, SgNode* scope,
              std::function<bool(SgNode*, SgNode*, AstUtilInterface::OperatorSideEffect)>& collect) {
    AstInterfaceImpl astImpl(scope);
    AstInterface fa(&astImpl);

    OperatorSideEffectAnnotation* funcAnnot=OperatorSideEffectAnnotation::get_inst();
    assert(funcAnnot != 0);
    DebugAstUtil([&ast](){ return "ComputeAstSideEffect: " + AstInterface::AstToString(ast); });

    StmtSideEffectCollect<SgNode*> collect_operator(fa, funcAnnot);
    std::function<bool(SgNode*, SgNode*)> save_mod = [&collect, &ast] (SgNode* first, SgNode* second) {
      if (AstInterface::IsFunctionDefinition(ast) && !IsLocalRef(first, ast)) {
         AddOperatorSideEffectAnnotation(ast, first, OperatorSideEffect::Modify);
      }
      DebugAstUtil([&first](){ return "save modify:" + AstInterface::AstToString(first); });
      return collect(first, second, OperatorSideEffect::Modify);
      };
    std::function<bool(SgNode*, SgNode*)> save_read = [&collect,&ast] (SgNode* first, SgNode* second) {
      if (AstInterface::IsFunctionDefinition(ast) && !IsLocalRef(first, ast)) {
         AddOperatorSideEffectAnnotation(ast, first, OperatorSideEffect::Read);
      }
      DebugAstUtil([&first](){ return "save read:" + AstInterface::AstToString(first); });
      return collect(first, second, OperatorSideEffect::Read);
      };
    std::function<bool(SgNode*, SgNode*)> save_kill = [&collect,&ast] (SgNode* first, SgNode* second) {
      if (AstInterface::IsFunctionDefinition(ast) && !IsLocalRef(first, ast)) {
         AddOperatorSideEffectAnnotation(ast, first, OperatorSideEffect::Kill);
      }
      DebugAstUtil([&first](){ return "save kill:" + AstInterface::AstToString(first); });
      return collect(first, second, OperatorSideEffect::Kill);
      };
    std::function<bool(SgNode*, SgNode*)> save_call = [&collect,&ast] (SgNode* first, SgNode* second) {
      if (AstInterface::IsFunctionDefinition(ast) && !IsLocalRef(first, ast)) {
         AddOperatorSideEffectAnnotation(ast, first, OperatorSideEffect::Call);
      }
      DebugAstUtil([&first](){ return "save call:" + AstInterface::AstToString(first); });
      return collect(first, second, OperatorSideEffect::Call);
      };

    return collect_operator(ast, &save_mod, &save_read, &save_kill, &save_call);
}

void AstUtilInterface::ReadAnnotations(std::istream& input) {
  ReadAnnotation::get_inst()->read(input);
}

void AstUtilInterface::OutputOperatorSideEffectAnnotations(std::ostream& output) {
    OperatorSideEffectAnnotation* funcAnnot=OperatorSideEffectAnnotation::get_inst();
    funcAnnot->write(output);
}

void AstUtilInterface::RegisterOperatorSideEffectAnnotation() {
  OperatorSideEffectAnnotation* funcAnnot=OperatorSideEffectAnnotation::get_inst();
  funcAnnot->register_annot();
};

std::pair<std::string, std::string>
AstUtilInterface::AddOperatorSideEffectAnnotation(
              SgNode* op_ast, SgNode* var, AstUtilInterface::OperatorSideEffect relation)
{
  DebugAstUtil([&relation, &var](){ return "Adding operator annotation: " + OperatorSideEffectName(relation) + ":" + "var is : " + AstInterface::AstToString(var); });
  std::string op_name;
  AstInterface::AstNodeList op_params;
  if (!AstInterface::IsFunctionDefinition(op_ast, &op_name, &op_params)) {
     DebugAstUtil([&op_ast](){ return "Expecting an operator but getting " + AstInterface::AstToString(op_ast);});
     return std::pair<std::string, std::string>("","");
  }
  AstInterfaceImpl astImpl(op_ast);
  AstInterface fa(&astImpl);

  OperatorSideEffectAnnotation* funcAnnot=OperatorSideEffectAnnotation::get_inst();
  OperatorSideEffectDescriptor *desc = 0;
  switch (relation) {
     case OperatorSideEffect::Modify:
         desc = funcAnnot->get_modify_descriptor(fa, op_ast, true);
          break;
     case OperatorSideEffect::Read:
        desc = funcAnnot->get_read_descriptor(fa, op_ast, true);
          break;
     case OperatorSideEffect::Call:
        desc = funcAnnot->get_call_descriptor(fa, op_ast, true);
          break;
     case OperatorSideEffect::Kill:
        desc = funcAnnot->get_kill_descriptor(fa, op_ast, true);
          break;
     default: 
        std::cerr << "Unexpected relation: " << relation << "\n";
        assert(0);
  }
  assert(desc != 0);
  std::string varname = GetVariableSignature(var);
  DebugAstUtil([&varname](){ return "Variable name is :" + varname; });
  if (varname == "_UNKNOWN_" || varname == "") {
    desc->set_has_unknown(true);  
  } else {
    desc->push_back(SymbolicValDescriptor(SymbolicValGenerator::GetSymbolicVal(fa, AstNodePtrImpl(var)), varname));
  }
  return std::pair<std::string, std::string>(GetVariableSignature(op_ast), varname);
} 

std::string AstUtilInterface:: GetVariableSignature(SgNode* variable) {
    if (variable == 0) return "_UNKNOWN_";
    switch (variable->variantT()) {
     case V_SgNamespaceDeclarationStatement:
          return isSgNamespaceDeclarationStatement(variable)->get_name().getString();
     case V_SgUsingDirectiveStatement:
          return "using_" + isSgUsingDirectiveStatement(variable)->get_namespaceDeclaration()->get_name().getString();
     case V_SgTypedefDeclaration:
     case V_SgTemplateTypedefDeclaration:
          return "typedef_" + AstInterface::GetGlobalUniqueName(variable->get_parent(), isSgTypedefDeclaration(variable)->get_name().getString());
     case V_SgStaticAssertionDeclaration:
          return OperatorDeclaration::operator_signature(variable);
     default: break;
    }
    if (AstInterface::IsFunctionDefinition(variable)) {
        return OperatorDeclaration::operator_signature(variable);
    } 
    AstNodePtr f;
    if (AstInterface::IsFunctionCall(variable, &f)) {
       variable = AstNodePtrImpl(f).get_ptr(); 
    }
    // An empty string will be returned AstInterface::IsVarRef(variable) returns false.
    std::string name = AstInterface::GetVarName(variable, /*use_global_unique_name=*/true);
    if (name == "") {
        name = "_UNKNOWN_";
    }
    return name;
}

bool AstUtilInterface::IsLocalRef(SgNode* ref, SgNode* scope) {
   std::string scope_name;
   if (! AstInterface::IsBlock(scope, &scope_name)) {
     return false;
   }
   DebugAstUtil([&ref,&scope_name](){ return "IsLocalRef invoked: var is " + AstInterface::AstToString(ref) + "; scope is " + scope_name; });
   AstNodePtr _cur_scope;
   if (!AstInterface::IsVarRef(ref, 0, 0, &_cur_scope)) {
      return false;
   }  
   SgNode* cur_scope = AstNodePtrImpl(_cur_scope).get_ptr(); 
   std::string cur_scope_name;
   while (cur_scope != 0 && cur_scope->variantT() != V_SgGlobal) {
         if (AstInterface::IsBlock(cur_scope, &cur_scope_name) &&  cur_scope_name == scope_name) {
             return true;
         }
         DebugAstUtil([&cur_scope](){ return "IsLocalRef current scope:" + cur_scope->class_name(); });
         SgNode* n = AstInterfaceImpl::GetScope(cur_scope);
         cur_scope = n;
   }   
   return false;
}
