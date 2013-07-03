
#include "scsm-sage.hpp"

#include "sage3basic.h"

namespace SCSM {

template <>
template <>
Sage<SgFunctionDeclaration>::build_result_t Driver<Sage>::build<SgFunctionDeclaration>(const Sage<SgFunctionDeclaration>::object_desc_t & desc) {
  Sage<SgFunctionDeclaration>::build_result_t result;

  std::pair<SgScopeStatement *, SgScopeStatement *> scopes = getScopes<SgFunctionDeclaration>(desc);

  SgScopeStatement * decl_scope = scopes.first;
  SgScopeStatement * defn_scope = scopes.second;

  SgFunctionDeclaration * func_decl = SageBuilder::buildNondefiningFunctionDeclaration(desc.name, desc.return_type, desc.params, decl_scope, NULL, false, NULL);
  SageInterface::appendStatement(func_decl, decl_scope);
  
  assert(func_decl->get_definition() == NULL);
  
  result.symbol = decl_scope->lookup_function_symbol(desc.name);
  assert(result.symbol != NULL);
  
  SgFunctionDeclaration * func_defn = SageBuilder::buildDefiningFunctionDeclaration(desc.name, desc.return_type, desc.params, defn_scope, NULL, false, func_decl, NULL);
  SageInterface::appendStatement(func_defn, defn_scope);
  
  result.definition = func_defn->get_definition();
  assert(result.definition != NULL);
  
  func_decl->set_definingDeclaration(func_defn);
  func_decl->set_firstNondefiningDeclaration(func_decl);
  
  func_defn->set_definingDeclaration(func_defn);
  func_defn->set_firstNondefiningDeclaration(func_decl);

  return result;
}

template <>
template <>
std::pair<SgScopeStatement *, SgScopeStatement *> Driver<Sage>::getScopes<SgFunctionDeclaration>(const Sage<SgFunctionDeclaration>::object_desc_t & desc) {
  std::pair<SgScopeStatement *, SgScopeStatement *> result;

  assert(source_files.size() == 1);
  result.first = (*header_files.begin())->get_globalScope();

  assert(header_files.size() == 1);
  result.second = (*source_files.begin())->get_globalScope();

  return result;
}

template <>
template <>
Sage<SgClassDeclaration>::build_result_t Driver<Sage>::build<SgClassDeclaration>(const Sage<SgClassDeclaration>::object_desc_t & desc) {
  Sage<SgClassDeclaration>::build_result_t result;

  std::pair<SgScopeStatement *, SgScopeStatement *> scopes = getScopes<SgClassDeclaration>(desc);

  SgScopeStatement * decl_scope = scopes.first;
  assert(scopes.second == NULL);

  SgClassDeclaration * class_decl = SageBuilder::buildNondefiningClassDeclaration_nfi(desc.name, (SgClassDeclaration::class_types)desc.kind, decl_scope, false, NULL);

  result.symbol = decl_scope->lookup_class_symbol(desc.name);
  assert(result.symbol != NULL);

  SgClassDeclaration * class_defn = SageBuilder::buildNondefiningClassDeclaration_nfi(desc.name, (SgClassDeclaration::class_types)desc.kind, decl_scope, false, NULL);
  SageInterface::appendStatement(class_defn, decl_scope);

  result.definition = SageBuilder::buildClassDefinition_nfi(class_defn, false);
  class_defn->set_definition(result.definition);
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

template <>
template <>
std::pair<SgScopeStatement *, SgScopeStatement *> Driver<Sage>::getScopes<SgClassDeclaration>(const Sage<SgClassDeclaration>::object_desc_t & desc) {
  std::pair<SgScopeStatement *, SgScopeStatement *> result;

  assert(source_files.size() == 1);
  result.first = (*header_files.begin())->get_globalScope();

  result.second = NULL;

  return result;
}

// SgMemberFunctionDeclaration

template <>
template <>
Sage<SgMemberFunctionDeclaration>::build_result_t Driver<Sage>::build<SgMemberFunctionDeclaration>(const Sage<SgMemberFunctionDeclaration>::object_desc_t & desc) {
  Sage<SgMemberFunctionDeclaration>::build_result_t result;

  std::pair<SgScopeStatement *, SgScopeStatement *> scopes = getScopes<SgMemberFunctionDeclaration>(desc);
  
  SgScopeStatement * decl_scope = scopes.first;
  SgScopeStatement * defn_scope = scopes.second;

  assert(isSgClassDefinition(decl_scope));

  SgMemberFunctionDeclaration * mfunc_decl = SageBuilder::buildNondefiningMemberFunctionDeclaration(desc.name, desc.return_type, desc.params, decl_scope, NULL, 0, false, NULL);
  SageInterface::appendStatement(mfunc_decl, decl_scope);

  assert(mfunc_decl->get_definition() == NULL);
  assert(mfunc_decl->get_associatedClassDeclaration() != NULL);

  result.symbol = isSgMemberFunctionSymbol(decl_scope->lookup_function_symbol(desc.name));
  assert(result.symbol != NULL);

  SgMemberFunctionDeclaration * mfunc_defn = SageBuilder::buildDefiningMemberFunctionDeclaration(desc.name, desc.return_type, desc.params, decl_scope, NULL, 0, false, mfunc_decl, NULL);
  SageInterface::appendStatement(mfunc_defn, defn_scope);

  assert(mfunc_defn->get_associatedClassDeclaration() != NULL);

  result.definition = mfunc_defn->get_definition();
  assert(result.definition != NULL);

  mfunc_decl->set_definingDeclaration(mfunc_defn);
  mfunc_decl->set_firstNondefiningDeclaration(mfunc_decl);

  mfunc_defn->set_definingDeclaration(mfunc_defn);
  mfunc_defn->set_firstNondefiningDeclaration(mfunc_decl);

  return result;
}

template <>
template <>
std::pair<SgScopeStatement *, SgScopeStatement *> Driver<Sage>::getScopes<SgMemberFunctionDeclaration>(const Sage<SgMemberFunctionDeclaration>::object_desc_t & desc) {
  std::pair<SgScopeStatement *, SgScopeStatement *> result;

  assert(desc.parent_class != NULL);
  SgClassDeclaration * class_decl = desc.parent_class->get_declaration();
  assert(class_decl != NULL);
  class_decl = isSgClassDeclaration(class_decl->get_definingDeclaration());
  assert(class_decl != NULL);
  SgClassDefinition * class_defn = isSgClassDefinition(class_decl->get_definition());
  assert(class_defn != NULL);
  result.first = class_defn;

  assert(source_files.size() == 1);
  result.second = (*source_files.begin())->get_globalScope();

  return result;
}

// Driver Ctor for Sage model

template <>
Driver<Sage>::Driver(const std::string & filename_, SgProject * project_) :
  source_files(),
  header_files(),
  project(project_)
{
  if (!CommandlineProcessing::isCppFileNameSuffix("hpp"))
    CommandlineProcessing::extraCppSourceFileSuffixes.push_back("hpp");

  std::string filename;

  filename = filename_ + ".hpp";
  SgSourceFile * decl_file = isSgSourceFile(SageBuilder::buildFile(filename, filename, project));
  SageInterface::attachComment(decl_file, "/* File generated by SCSM::Driver<Sage> */");
  source_files.insert(decl_file);

  filename = filename_ + ".cpp";
  SgSourceFile * defn_file = isSgSourceFile(SageBuilder::buildFile(filename, filename, project));
  SageInterface::attachComment(defn_file, "/* File generated by SCSM::Driver<Sage> */");
  SageInterface::insertHeader(defn_file, filename_ + ".hpp");
  header_files.insert(defn_file);
}

}

