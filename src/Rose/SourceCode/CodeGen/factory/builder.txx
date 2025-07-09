
#error "TODO file is not used..."

#ifndef Rose_SourceCode_CodeGen_factory_builder_txx
#define Rose_SourceCode_CodeGen_factory_builder_txx

template <typename CRT, typename API>
SgNamespaceDeclarationStatement * Factory<CRT,API>::buildNamespace(SgScopeStatement * scope, std::string const & name) const {
  SgNamespaceDeclarationStatement * decl = SageBuilder::buildNamespaceDeclaration(name, scope);
  ROSE_ASSERT(decl->get_definition() != nullptr);
  return decl;
}

template <typename CRT, typename API>
SgClassDeclaration * Factory<CRT,API>::buildType(SgScopeStatement * scope, std::string const & name, bool is_prototype) const {  
  SgClassDeclaration * decl = nullptr;
  if (is_prototype) {
    decl = SageBuilder::buildNondefiningClassDeclaration_nfi(name, SgClassDeclaration::e_struct, scope, false, nullptr);
    ROSE_ASSERT(decl->get_definition() == nullptr);
  } else {
    decl = SageBuilder::buildClassDeclaration_nfi(name, SgClassDeclaration::e_struct, scope, nullptr, false, nullptr);
    ROSE_ASSERT(decl->get_definition() != nullptr);
  }
  return decl;
}

template <typename CRT, typename API>
SgTypedefDeclaration * Factory<CRT,API>::buildType(SgScopeStatement * scope, std::string const & name, SgType * type) const {
  return SageBuilder::buildTypedefDeclaration(name, type, scope);
}

template <typename CRT, typename API>
SgEnumDeclaration * Factory<CRT,API>::buildEnumType(SgScopeStatement * scope, std::string const & name) const {
  return SageBuilder::buildEnumDeclaration(name, scope);
}

template <typename CRT, typename API>
SgVariableDeclaration * Factory<CRT,API>::buildVariable(SgScopeStatement * scope, std::string const & name, SgType * type) const {
  return SageBuilder::buildVariableDeclaration(name, type, nullptr, scope);
}

template <typename CRT, typename API>
SgFunctionDeclaration * Factory<CRT,API>::buildFunction(SgScopeStatement * scope, std::string const & name, SgType * rtype, std::vector<SgInitializedName *> const & params, bool is_prototype) const {
  SgFunctionParameterList * param_lst = SageBuilder::buildFunctionParameterList();
  auto & args = param_lst->get_args();
  for (auto p: params) {
    args.push_back(p);
    p->set_parent(param_lst);
  }
  if (is_prototype) {
    return SageBuilder::buildNondefiningFunctionDeclaration(name, rtype, param_lst, scope);
  } else {
    return SageBuilder::buildDefiningFunctionDeclaration(name, rtype, param_lst, scope);
  }
}

#endif /* Rose_SourceCode_CodeGen_factory_builder_txx */

