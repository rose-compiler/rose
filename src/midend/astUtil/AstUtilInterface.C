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
    OperatorDeclaration::operator_signature(fa, var) : AstInterface::GetVarName(var, /*use_global_unique_name=*/true);
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
    }
    if (AstInterface::IsFunctionDefinition(variable)) {
        AstInterfaceImpl astImpl(variable);
        AstInterface fa(&astImpl);
        return OperatorDeclaration::operator_signature(fa, variable);
    } 
    return AstInterface::GetVarName(variable, /*use_global_unique_name=*/true);
}

