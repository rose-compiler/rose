#include "AstUtilInterface.h"
#include "StmtInfoCollect.h"
#include "AstInterface_ROSE.h"
#include "annotation/OperatorAnnotation.h"

bool DebugAnnot();

bool AstUtilInterface::ComputeAstSideEffects(SgNode* ast, SgNode* scope,
              std::function<bool(SgNode*, SgNode*, AstUtilInterface::OperatorSideEffect)>& collect) {
    AstInterfaceImpl astImpl(scope);
    AstInterface fa(&astImpl);

    OperatorSideEffectAnnotation* funcAnnot=OperatorSideEffectAnnotation::get_inst();
    assert(funcAnnot != 0);

    StmtSideEffectCollect<SgNode*> collect_operator(fa, funcAnnot);
    std::function<bool(SgNode*, SgNode*)> save_mod = [&collect] (SgNode* first, SgNode* second) {
      return collect(first, second, OperatorSideEffect::Modify);
      };
    std::function<bool(SgNode*, SgNode*)> save_read = [&collect] (SgNode* first, SgNode* second) {
      return collect(first, second, OperatorSideEffect::Read);
      };
    std::function<bool(SgNode*, SgNode*)> save_kill = [&collect] (SgNode* first, SgNode* second) {
      return collect(first, second, OperatorSideEffect::Kill);
      };
    std::function<bool(SgNode*, SgNode*)> save_call = [&collect] (SgNode* first, SgNode* second) {
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
  if (DebugAnnot()) {
    std::cerr << "Adding operator annotation: " << relation << ":";
    std::cerr << "var is : " << AstInterface::AstToString(var) << "\n";
  }
  std::string op_name;
  AstInterface::AstNodeList op_params;
  if (!AstInterface::IsFunctionDefinition(op_ast, &op_name, &op_params)) {
     std::stringstream msg;
     msg  << "Expecting an operator but getting " << AstInterface::AstToString(op_ast);
     std::cerr  << msg.str() << "\n";
     throw ReadError(msg.str()); 
  }
  AstInterfaceImpl astImpl(op_ast);
  AstInterface fa(&astImpl);

  OperatorSideEffectAnnotation* funcAnnot=OperatorSideEffectAnnotation::get_inst();
  OperatorDeclaration op_decl(fa, op_ast);
  OperatorSideEffectDescriptor desc;
  std::string varname = (relation == OperatorSideEffect::Call)? 
    OperatorDeclaration::operator_signature(var) : GetVariableSignature(var);
  if (DebugAnnot()) {
    std::cerr << "Variable name is :" << varname << "\n";
  }
  if (varname == "") {
    std::cerr << "Do not store empty variable name as part of operator size effect annotation for: " << op_decl.get_signiture() << "\n Maybe there is something wrong?"; 
  } else {
     varname = NameDescriptor::get_signature(varname);
     desc.push_back(varname);
     switch (relation) {
        case OperatorSideEffect::Modify:
           funcAnnot->add_modify(op_decl, desc);
             break;
        case OperatorSideEffect::Read:
           funcAnnot->add_read(op_decl, desc);
             break;
        case OperatorSideEffect::Call:
           funcAnnot->add_call(op_decl, desc);
             break;
        default: break;
     }
  }
  return std::pair<std::string, std::string>(op_decl.get_signiture(), varname);
} 

std::string AstUtilInterface:: GetVariableSignature(SgNode* variable) {
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
    return AstInterface::GetVarName(variable, /*use_global_unique_name=*/true);
}

bool AstUtilInterface::IsLocalRef(SgNode* ref, SgNode* scope) {
   AstNodePtr _cur_scope;
   if (!AstInterface::IsVarRef(ref, 0, 0, &_cur_scope)) {
      return false;
   }  
   std::cerr << "scope is " << scope->class_name() << "\n";
   SgNode* cur_scope = AstNodePtrImpl(_cur_scope).get_ptr(); 
   while (cur_scope != 0 && cur_scope != scope) {
    std::cerr << "current scope:" << cur_scope->class_name() << "\n";
         SgNode* n = AstInterfaceImpl::GetScope(cur_scope);
         if (n == cur_scope) {
            break;
         }
         cur_scope = n;
   }   
   return cur_scope == scope;
}
