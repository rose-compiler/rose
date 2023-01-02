
#ifndef Rose_CodeGen_factory_functions_txx
#define Rose_CodeGen_factory_functions_txx

namespace Rose { namespace CodeGen {

template <typename CRT, typename API>
struct __factory_helper_t<CRT, API, Object::a_function> {
  static declaration_t<Object::a_function> * instantiate(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_function> * sym,
      SgNamedType * parent,
      tplargs_t & tpl_args
  );

  template <typename... Args>
  static declaration_t<Object::a_function> * instantiate(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_function> * sym,
      SgNamedType * parent,
      Args... args
  );

  template <typename... Args>
  static reference_t<Object::a_function> * reference(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_function> * sym,
      SgNamedType * parent,
      Args... args
  );
};

#define DEBUG___factory_helper_t__a_function__instantiate 0

template <typename CRT, typename API>
declaration_t<Object::a_function> * __factory_helper_t<CRT, API, Object::a_function>::instantiate(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_function> * sym,
  SgNamedType * parent,
  tplargs_t & tpl_args
) {
#if DEBUG___factory_helper_t__a_function__instantiate
  std::cout << "__factory_helper_t<CRT, API, Object::a_function>::instantiate" << std::endl;
  std::cout << "  sym    = " << std::hex << sym << " : " << ( sym ? sym->class_name() : "" ) << std::endl;
  std::cout << "  parent = " << std::hex << parent << " : " << ( parent ? parent->class_name() : "" ) << std::endl;
#endif

  SgName fname(sym->get_name().getString());
  SgName fname_tplargs = SageBuilder::appendTemplateArgumentsToName(fname, tpl_args);

  SgFunctionDeclaration * decl = isSgFunctionDeclaration(sym->get_declaration());
  ROSE_ASSERT(decl);
#if DEBUG___factory_helper_t__a_function__instantiate
  std::cout << "  decl    = " << std::hex << decl << " : " << ( decl ? decl->class_name() : "" ) << std::endl;
#endif

  SgTemplateFunctionDeclaration * tpl_fdecl = isSgTemplateFunctionDeclaration(decl);
  SgTemplateMemberFunctionDeclaration * tpl_mfdecl = isSgTemplateMemberFunctionDeclaration(decl);
  ROSE_ASSERT((tpl_fdecl != nullptr) xor (tpl_mfdecl != nullptr));
  ROSE_ASSERT(!tpl_fdecl || !parent);
  ROSE_ASSERT(!tpl_mfdecl || parent);

  SgType * rtype = decl->get_type()->get_return_type();
#if DEBUG___factory_helper_t__a_function__instantiate
  std::cout << "  rtype    = " << std::hex << rtype << " : " << ( rtype ? rtype->class_name() : "" ) << std::endl;
#endif
  if (tpl_fdecl) {
    rtype = Rose::Builder::Templates::instantiateNonrealTypes(rtype, tpl_fdecl->get_templateParameters(), tpl_args);
  } else if (tpl_mfdecl) {
    rtype = Rose::Builder::Templates::instantiateNonrealTypes(rtype, tpl_mfdecl->get_templateParameters(), tpl_args);
  }
  ROSE_ASSERT(rtype);
#if DEBUG___factory_helper_t__a_function__instantiate
  std::cout << "  rtype    = " << std::hex << rtype << " : " << ( rtype ? rtype->class_name() : "" ) << std::endl;
#endif

  SgFunctionParameterTypeList * ptypes = SageBuilder::buildFunctionParameterTypeList(); // TODO instantiate function parameter types
  SgFunctionType * ftype = SageBuilder::buildFunctionType(rtype, ptypes); // TODO add parameter types

  SgScopeStatement * defn_scope = nullptr;
  SgFunctionDeclaration * inst_decl = nullptr;
  if (tpl_fdecl) {
    defn_scope = decl->get_scope();
    SgTemplateInstantiationFunctionDecl * inst_fdecl = new SgTemplateInstantiationFunctionDecl(
        fname_tplargs, ftype, nullptr, tpl_fdecl, tpl_args
    );

    inst_fdecl->set_templateName(fname);
    inst_fdecl->set_template_argument_list_is_explicit(true);

    inst_decl = inst_fdecl;

  } else if (tpl_mfdecl) {

    while (isSgTypedefType(parent)) {
      parent = isSgNamedType(((SgTypedefType*)parent)->get_base_type());
      ROSE_ASSERT(parent);
    }

    SgTemplateInstantiationDecl * inst_pdecl = isSgTemplateInstantiationDecl(parent->get_declaration());
    ROSE_ASSERT(inst_pdecl);
#if DEBUG___factory_helper_t__a_function__instantiate
    std::cout << "  inst_pdecl    = " << std::hex << inst_pdecl << " : " << ( inst_pdecl ? inst_pdecl->class_name() : "" ) << std::endl;
#endif
    inst_pdecl = isSgTemplateInstantiationDecl(inst_pdecl->get_definingDeclaration());
    ROSE_ASSERT(inst_pdecl);
    defn_scope = inst_pdecl->get_definition();

    SgMemberFunctionDeclaration * inst_mfdecl = new SgTemplateInstantiationMemberFunctionDecl(
        fname_tplargs, ftype, nullptr, tpl_mfdecl, tpl_args
    );

    inst_decl = inst_mfdecl;
  }

  inst_decl->set_scope(defn_scope);
  inst_decl->set_parent(defn_scope);
  defn_scope->insert_symbol(fname_tplargs, new SgFunctionSymbol(inst_decl));

  SageInterface::setSourcePositionForTransformation(inst_decl);
  return inst_decl;
}

template <typename CRT, typename API>
template <typename... Args>
declaration_t<Object::a_function> * __factory_helper_t<CRT, API, Object::a_function>::instantiate(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_function> * sym,
  SgNamedType * parent,
  Args... args
) {
  std::vector<SgTemplateArgument *> tpl_args;
  Rose::Builder::Templates::fillTemplateArgumentList(tpl_args, args...);
  return instantiate(factory, sym, parent, tpl_args);
}


#define DEBUG___factory_helper_t__a_function__reference 0

template <typename CRT, typename API>
template <typename... Args>
reference_t<Object::a_function> * __factory_helper_t<CRT, API, Object::a_function>::reference(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_function> * sym,
  SgNamedType * parent,
  Args... args
) {
#if DEBUG___factory_helper_t__a_function__reference
  std::cout << "__factory_helper_t<CRT, API, Object::a_function>::reference" << std::endl;
  std::cout << "  sym    = " << std::hex << sym << " : " << ( sym ? sym->class_name() : "" ) << std::endl;
  std::cout << "  parent = " << std::hex << parent << " : " << ( parent ? parent->class_name() : "" ) << std::endl;
#endif
  reference_t<Object::a_function> *  res = SageBuilder::buildFunctionRefExp(sym);
  SageInterface::setSourcePositionForTransformation(res);
  return res;
}

} }

#endif /* Rose_CodeGen_factory_functions_txx */

