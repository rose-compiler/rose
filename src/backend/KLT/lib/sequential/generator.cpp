
#include "KLT/Sequential/generator.hpp"
#include "KLT/Sequential/kernel.hpp"

#include "KLT/Sequential/mfb-klt.hpp"

#include "KLT/Core/data.hpp"

#include "sage3basic.h"

#include <sstream>

#include <cassert>

namespace KLT {

namespace Sequential {

#if 0
/*
template <typename Symbol, typename Defn>
std::pair<Symbol *, Defn *> buildDeclAndDefn(
  const std::string & name,
  SgScopeStatement * decl_scope,
  SgScopeStatement * defn_scope
);
*/

std::pair<SgClassSymbol *, SgClassDefinition *> buildClassDecl(
  const std::string & name,
  SgClassDeclaration::class_types kind,
  SgScopeStatement * decl_scope
) {
  assert(decl_scope != NULL);

  std::pair<SgClassSymbol *, SgClassDefinition *> result(NULL, NULL);

  SgClassDeclaration * class_decl = SageBuilder::buildNondefiningClassDeclaration_nfi(name, kind, decl_scope, false, NULL);
//SageInterface::appendStatement(class_decl, decl_scope);

  result.first = decl_scope->lookup_class_symbol(name);
  assert(result.first != NULL);
 
  SgClassDeclaration * class_defn = SageBuilder::buildNondefiningClassDeclaration_nfi(name, kind, decl_scope, false, NULL);
  SageInterface::appendStatement(class_defn, decl_scope);

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
  assert(decl_scope != NULL && defn_scope != NULL && decl_scope != defn_scope);
 
  std::pair<SgFunctionSymbol *, SgFunctionDefinition *> result(NULL, NULL);
 
  SgFunctionDeclaration * func_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, return_type, params, decl_scope, NULL, false, NULL);
  SageInterface::appendStatement(func_decl, decl_scope);
 
  assert(func_decl->get_definition() == NULL);
 
  result.first = decl_scope->lookup_function_symbol(name);
  assert(result.first != NULL);
 
  SgFunctionDeclaration * func_defn = SageBuilder::buildDefiningFunctionDeclaration(name, return_type, params, defn_scope, NULL, false, func_decl, NULL);
  SageInterface::appendStatement(func_defn, defn_scope);
 
  result.second = func_defn->get_definition();
  assert(result.second != NULL);
 
  func_decl->set_definingDeclaration(func_defn);
  func_decl->set_firstNondefiningDeclaration(func_decl);
 
  func_defn->set_definingDeclaration(func_defn);
  func_defn->set_firstNondefiningDeclaration(func_decl);
 
  return result;
}

std::pair<SgMemberFunctionSymbol *, SgFunctionDefinition *> buildMemberFunctionDeclAndDefn(
  const std::string & name,
  SgType * return_type,
  SgFunctionParameterList * params,
  const std::pair<SgClassSymbol *, SgClassDefinition *> & class_desc,
  SgScopeStatement * defn_scope
) {
  assert(defn_scope != NULL);
  assert(class_desc.first != NULL && class_desc.second != NULL);

  SgScopeStatement * decl_scope = class_desc.second;

  std::pair<SgMemberFunctionSymbol *, SgFunctionDefinition *> result(NULL, NULL);

  SgMemberFunctionDeclaration * mfunc_decl = SageBuilder::buildNondefiningMemberFunctionDeclaration(name, return_type, params, decl_scope, NULL, 0, false, NULL);
  SageInterface::appendStatement(mfunc_decl, decl_scope);

  assert(mfunc_decl->get_definition() == NULL);
  assert(mfunc_decl->get_associatedClassDeclaration() != NULL);

  result.first = isSgMemberFunctionSymbol(decl_scope->lookup_function_symbol(name));
  assert(result.first != NULL);

  SgMemberFunctionDeclaration * mfunc_defn = SageBuilder::buildDefiningMemberFunctionDeclaration(name, return_type, params, decl_scope, NULL, 0, false, mfunc_decl, NULL);
  SageInterface::appendStatement(mfunc_defn, defn_scope);

  assert(mfunc_defn->get_associatedClassDeclaration() != NULL);

  result.second = mfunc_defn->get_definition();
  assert(result.second != NULL);

  mfunc_decl->set_definingDeclaration(mfunc_defn);
  mfunc_decl->set_firstNondefiningDeclaration(mfunc_decl);

  mfunc_defn->set_definingDeclaration(mfunc_defn);
  mfunc_defn->set_firstNondefiningDeclaration(mfunc_decl);

  return result;
}

#endif

Core::Kernel * Generator::makeKernel() const {
  return new Kernel();
}

void Generator::doCodeGeneration(Core::Kernel * kernel_, const Core::CG_Config & cg_config) {
  Kernel * kernel = dynamic_cast<Kernel *>(kernel_);
  assert(kernel != NULL);

  assert(kernel->isDataflowDone());
  assert(kernel->isArgumentDone());
  assert(kernel->isContentDone());

  ::MultiFileBuilder::KLT<Kernel>::object_desc_t kernel_desc(kernel, p_file_id);

  ::MultiFileBuilder::KLT<Kernel>::build_result_t result = p_klt_driver.build<Kernel>(kernel_desc);

  kernel->setKernelSymbol(result.kernel);
  kernel->setArgumentPacker(result.arguments_packer);
}

Generator::Generator(SgProject * project, const std::string & filename_) :
  Core::Generator(project),
  p_filename(filename_),
  p_file_id(p_sage_driver.createPairOfFiles(p_filename))
{}

Generator::~Generator() {}

}

}

