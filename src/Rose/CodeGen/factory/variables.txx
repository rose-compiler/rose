
#ifndef Rose_CodeGen_factory_variables_txx
#define Rose_CodeGen_factory_variables_txx

namespace Rose { namespace CodeGen {

template <typename CRT, typename API>
struct __factory_helper_t<CRT, API, Object::a_variable> {
  static declaration_t<Object::a_variable> * instantiate(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_variable> * sym,
      SgNamedType * parent,
      tplargs_t & tpl_args
  );

  template <typename... Args>
  static declaration_t<Object::a_variable> * instantiate(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_variable> * sym,
      SgNamedType * parent,
      Args... args
  );

  template <typename... Args>
  static reference_t<Object::a_variable> * reference(
      Factory<CRT, API> const & factory,
      symbol_t<Object::a_variable> * sym,
      SgNamedType * parent,
      Args... args
  );
};

#define DEBUG___factory_helper_t__a_variable__instantiate 0

template <typename CRT, typename API>
declaration_t<Object::a_variable> * __factory_helper_t<CRT, API, Object::a_variable>::instantiate(
  Factory<CRT, API> const & /*factory*/,
  symbol_t<Object::a_variable> * sym,
  SgNamedType * parent,
  tplargs_t & tpl_args
) {
#if DEBUG___factory_helper_t__a_variable__instantiate
  std::cout << "__factory_helper_t<CRT, API, Object::a_variable>::instantiate" << std::endl;
  std::cout << "  sym = " << std::hex << sym << " : " << ( sym ? sym->class_name() : "" ) << std::endl;
  std::cout << "  parent = " << std::hex << parent << " : " << ( parent ? parent->class_name() : "" ) << std::endl;
#endif
  SgInitializedName * iname = sym->get_declaration();
  ROSE_ASSERT(iname);
#if DEBUG___factory_helper_t__a_variable__instantiate
  std::cout << "  iname = " << std::hex << iname << " : " << ( iname ? iname->class_name() : "" ) << std::endl;
  std::cout << "    ->get_qualified_name() = " << iname->get_qualified_name().getString() << std::endl;
#endif

  SgName vname(sym->get_name().getString());
  SgName vname_tplargs = SageBuilder::appendTemplateArgumentsToName(vname, tpl_args);

  SgTemplateVariableDeclaration * tpl_decl = isSgTemplateVariableDeclaration(sym->get_declaration()->get_parent());
#if DEBUG___factory_helper_t__a_variable__instantiate
  std::cout << "  tpl_decl = " << std::hex << tpl_decl << " : " << ( tpl_decl ? tpl_decl->class_name() : "" ) << std::endl;
  std::cout << "    ->get_parent() = " << std::hex << tpl_decl->get_parent() << " : " << ( tpl_decl->get_parent() ? tpl_decl->get_parent()->class_name() : "" ) << std::endl;
#endif

  SgScopeStatement * defn_scope = iname->get_scope();
  if (parent) {
    while (isSgTypedefType(parent)) {
      parent = isSgNamedType(((SgTypedefType*)parent)->get_base_type());
    }
    ROSE_ASSERT(parent);
#if DEBUG___factory_helper_t__a_variable__instantiate
    std::cout << "  parent = " << std::hex << parent << " : " << ( parent ? parent->class_name() : "" ) << std::endl;
#endif
    SgClassType * xtype = isSgClassType(parent);
    ROSE_ASSERT(xtype);
    SgClassDeclaration * xdecl = isSgClassDeclaration(xtype->get_declaration());
    ROSE_ASSERT(xdecl);
#if DEBUG___factory_helper_t__a_variable__instantiate
    std::cout << "  xdecl = " << std::hex << xdecl << " : " << ( xdecl ? xdecl->class_name() : "" ) << std::endl;
#endif
    xdecl = isSgClassDeclaration(xdecl->get_definingDeclaration());
    ROSE_ASSERT(xdecl);
    defn_scope = xdecl->get_definition();

    // TODO tpl_decl should also be updated to point to the partial instantiation inside parent
  }
  ROSE_ASSERT(defn_scope);

  SgType * iname_type = iname->get_type();
  ROSE_ASSERT(iname_type);
#if DEBUG___factory_helper_t__a_variable__instantiate
  std::cout << "  iname_type = " << std::hex << iname_type << " : " << ( iname_type ? iname_type->class_name() : "" ) << std::endl;
#endif
  if (tpl_decl) iname_type = Rose::Builder::Templates::instantiateNonrealTypes(iname_type, tpl_decl->get_templateParameters(), tpl_args);
  ROSE_ASSERT(iname_type);
#if DEBUG___factory_helper_t__a_variable__instantiate
  std::cout << "  iname_type = " << std::hex << iname_type << " : " << ( iname_type ? iname_type->class_name() : "" ) << std::endl;
#endif

  SgVariableDeclaration * vdecl = nullptr;
  if (tpl_decl) {
    vdecl = SageBuilder::buildTemplateVariableInstantiation(vname_tplargs, iname_type, nullptr, defn_scope, tpl_decl, tpl_args);
#if DEBUG___factory_helper_t__a_variable__instantiate
    std::cout << "  vdecl = " << std::hex << vdecl << " : " << ( vdecl ? vdecl->class_name() : "" ) << std::endl;
#endif
  } else {
    vdecl = SageBuilder::buildVariableDeclaration(vname, iname_type, nullptr, defn_scope);
  }
  vdecl->set_parent(defn_scope);
  defn_scope->insert_symbol(vname_tplargs, new SgVariableSymbol(vdecl->get_variables()[0]));

  for (auto tpl_arg: tpl_args) {
    tpl_arg->set_parent(vdecl);
  }

  SageInterface::setSourcePositionForTransformation(vdecl);
  return vdecl;
}

template <typename CRT, typename API>
template <typename... Args>
declaration_t<Object::a_variable> * __factory_helper_t<CRT, API, Object::a_variable>::instantiate(
  Factory<CRT, API> const & factory,
  symbol_t<Object::a_variable> * sym,
  SgNamedType * parent,
  Args... args
) {
  std::vector<SgTemplateArgument *> tpl_args;
  Rose::Builder::Templates::fillTemplateArgumentList(tpl_args, args...);
  return instantiate(factory, sym, parent, tpl_args);
}

template <typename CRT, typename API>
template <typename... Args>
reference_t<Object::a_variable> * __factory_helper_t<CRT, API, Object::a_variable>::reference(
  Factory<CRT, API> const & /*factory*/,
  symbol_t<Object::a_variable> * sym,
  SgNamedType * /*parent*/,
  Args... /*args*/
) {
  return SageBuilder::buildVarRefExp(sym);
}

} }

#endif /* Rose_CodeGen_factory_variables_txx */

