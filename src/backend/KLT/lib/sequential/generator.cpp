
#include "KLT/Sequential/generator.hpp"
#include "KLT/Sequential/kernel.hpp"

#include "sage3basic.h"

#include <sstream>

#include <cassert>

namespace KLT {

namespace Sequential {

/*
template <typename Symbol, typename Defn>
std::pair<Symbol *, Defn *> buildDeclAndDefn(
  const std::string & name,
  SgScopeStatement * decl_scope,
  SgScopeStatement * defn_scope
);
*/

std::pair<SgClassSymbol *, SgClassDefinition *> buildClassDeclAndDefn(
  const std::string & name,
  SgClassDeclaration::class_types kind,
  SgScopeStatement * decl_scope,
  SgScopeStatement * defn_scope
) {
  assert(decl_scope != NULL && defn_scope != NULL);

  std::pair<SgClassSymbol *, SgClassDefinition *> result(NULL, NULL);

  SgClassDeclaration * class_decl = SageBuilder::buildNondefiningClassDeclaration_nfi(name, kind, decl_scope, false, NULL);
  SageInterface::appendStatement(class_decl, decl_scope);

  result.first = decl_scope->lookup_class_symbol(name);
  assert(result.first != NULL);
 
  SgClassDeclaration * class_defn = SageBuilder::buildNondefiningClassDeclaration_nfi(name, kind, defn_scope, false, NULL);
  SageInterface::appendStatement(class_defn, defn_scope);

  result.second = SageBuilder::buildClassDefinition_nfi(class_defn, false);
  class_defn->set_definition(result.second);
  class_defn->unsetForward();
 
  assert(class_decl->get_definition() == NULL);
  assert(class_defn->get_definition() != NULL);
  assert(!class_defn->isForward());

  class_decl->set_definingDeclaration(class_defn);
  class_decl->set_firstNondefiningDeclaration(class_decl);
 
  class_defn->set_definingDeclaration(class_defn);
  class_defn->set_firstNondefiningDeclaration(class_decl);

  return result;
}

std::pair<SgFunctionSymbol *, SgFunctionDefinition *> buildFunctionDeclAndDefn(
  const std::string & name,
  SgType * return_type,
  SgFunctionParameterList * params,
  SgScopeStatement * decl_scope,
  SgScopeStatement * defn_scope
) {
  assert(decl_scope != NULL && defn_scope != NULL);
 
  std::pair<SgFunctionSymbol *, SgFunctionDefinition *> result(NULL, NULL);
 
  SgFunctionDeclaration * func_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, return_type, params, decl_scope, NULL, false, NULL);
  SageInterface::appendStatement(func_decl, decl_scope);
 
  assert(func_decl->get_definition() == NULL);
 
  // result.first = SageInterface::lookupClassSymbolInParentScopes(name, decl_scope);
  result.first = decl_scope->lookup_function_symbol(name);
  assert(result.first != NULL);
 
  SgFunctionDeclaration * func_defn = SageBuilder::buildDefiningFunctionDeclaration(
    name, return_type, params, defn_scope, NULL, false, func_decl, NULL
  );
  SageInterface::appendStatement(func_defn, defn_scope);
 
  result.second = func_defn->get_definition();
  assert(result.second != NULL);
 
  func_decl->set_definingDeclaration(func_defn);
  func_decl->set_firstNondefiningDeclaration(func_decl);
 
  func_defn->set_definingDeclaration(func_defn);
  func_defn->set_firstNondefiningDeclaration(func_decl);
 
  return result;
}

Kernel * Generator::makeKernel() const {
  return new Kernel();
}

void Generator::doCodeGeneration(Core::Kernel * kernel_, const Core::CG_Config & cg_config) {
  Kernel * kernel = dynamic_cast<Kernel *>(kernel_);
  assert(kernel != NULL);

  assert(kernel->isDataflowDone());
  assert(kernel->isArgumentDone());
  assert(kernel->isContentDone());

  SgScopeStatement * decl_scope = p_decl_file->get_globalScope();
  SgScopeStatement * defn_scope = p_defn_file->get_globalScope();

  std::ostringstream arguments_packer_name;
  arguments_packer_name << "arguments_packer_" << kernel->id << "_" << kernel;

  std::pair<SgClassSymbol *, SgClassDefinition *> arguments_packer =
    buildClassDeclAndDefn(arguments_packer_name.str(), SgClassDeclaration::e_class, decl_scope, defn_scope);

  // TODO fill arguments_packer.second

  kernel->setArgumentPacker(arguments_packer.first);

  std::ostringstream kernel_function_name;
  kernel_function_name << "kernel_function_" << kernel->id << "_" << kernel;

  SgType * return_type = SageBuilder::buildVoidType();

  SgFunctionParameterList * params = SageBuilder::buildFunctionParameterList(
    SageBuilder::buildInitializedName("arg_", arguments_packer.first->get_type())
  );

  std::pair<SgFunctionSymbol *, SgFunctionDefinition *> kernel_function =
    buildFunctionDeclAndDefn(kernel_function_name.str(), return_type, params, decl_scope, defn_scope);

  // TODO fill kernel_function.second

  kernel->setKernelSymbol(kernel_function.first);
}

Generator::Generator(SgProject * project, std::string filename_) :
  Core::Generator(project),
  p_decl_file(NULL),
  p_defn_file(NULL)
{
  if (!CommandlineProcessing::isCppFileNameSuffix("hpp"))
    CommandlineProcessing::extraCppSourceFileSuffixes.push_back("hpp");

  std::string filename;

  filename = filename_ + ".hpp";
  p_decl_file = isSgSourceFile(SageBuilder::buildFile(filename, filename, p_project));
  SageInterface::attachComment(p_decl_file, "/* File generated by KLT::Sequential::Generator */");

  filename = filename_ + ".cpp";
  p_defn_file = isSgSourceFile(SageBuilder::buildFile(filename, filename, p_project));
  SageInterface::attachComment(p_defn_file, "/* File generated by KLT::Sequential::Generator */");
  SageInterface::insertHeader(p_defn_file, filename_ + ".hpp");
}

Generator::~Generator() {}

void Generator::unparse() {
  assert(p_decl_file != NULL && p_defn_file != NULL);

//SageInterface::setSourcePositionForTransformation(p_decl_file);

  p_decl_file->unparse();  
  p_defn_file->unparse();
}

}

}

