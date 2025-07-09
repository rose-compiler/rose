
#ifndef Rose_SourceCode_CodeGen_factory_factory_txx
#define Rose_SourceCode_CodeGen_factory_factory_txx

namespace Rose { namespace SourceCode { namespace CodeGen {

template <typename CRT, typename API>
Factory<CRT, API>::Factory(Driver & driver_) :
  driver(driver_),
  api()
{
  api.load(driver);
}

template <typename CRT, typename API>
template <Object otag, typename... Args>
declaration_t<otag> * Factory<CRT, API>::instantiate(symbol_t<otag> * API::* obj, SgNamedType * parent, Args... args) const {
  symbol_t<otag> * sym = api.*obj;
  ROSE_ASSERT(sym);
  return __factory_helper_t<CRT, API, otag>::instantiate(*this, sym, parent, args...);
}

#define DEBUG__Factory__reference 0

template <typename CRT, typename API>
template <Object otag, typename... Args>
reference_t<otag> * Factory<CRT, API>::reference(symbol_t<otag> * API::* obj, SgNamedType * parent, Args... args) const {
#if DEBUG__Factory__reference
  std::cout << "Factory<CRT, API>::reference" << std::endl;
  std::cout << "  obj    = " << std::hex << obj << std::endl;
  std::cout << "  parent = " << std::hex << parent << " : " << ( parent ? parent->class_name() : "" ) << std::endl;
#endif
  symbol_t<otag> * sym = api.*obj;
  ROSE_ASSERT(sym);
#if DEBUG__Factory__reference
  std::cout << "  sym        = " << std::hex << sym << " : " << ( sym ? sym->class_name() : "" ) << std::endl;
  std::cout << "    ->name() = " << std::hex << sym->get_name().getString() << std::endl;
#endif

  bool unk_ptype = false; // This bypass is for generated type-alias where `int` is used as the base type
                          // Happens when I don't go properly construct template instantiations in old codegen

  SgType * pp = parent;
  while (pp && !isSgClassType(pp)) {
#if DEBUG__Factory__reference
    std::cout << "  pp        = " << std::hex << pp << " : " << ( pp ? pp->class_name() : "" ) << std::endl;
    std::cout << "            = " << ( pp ? pp->unparseToString() : "" ) << std::endl;
#endif
    if (isSgTypedefType(pp)) pp = ((SgTypedefType*)pp)->get_base_type();
    else if (isSgModifierType(pp)) pp = ((SgModifierType*)pp)->get_base_type();
    else if (isSgTypeInt(pp)) { pp = nullptr; unk_ptype = true; }
    else {
      std::cerr << "FATAL: pp = " << std::hex << pp << " ( " << (pp ? pp->class_name() : "") << " )" << std::endl;
      ROSE_ABORT();
    }
  }
  ROSE_ASSERT(!(parent && !unk_ptype) || pp);
  SgClassType * xp = isSgClassType(pp);
  ROSE_ASSERT(!(parent && !unk_ptype) || xp);

  SgScopeStatement * scope_of_symbol = sym->get_scope();
#if DEBUG__Factory__reference
  std::cout << "  scope_of_symbol = " << std::hex << scope_of_symbol << " : " << ( scope_of_symbol ? scope_of_symbol->class_name() : "" ) << std::endl;
#endif
  SgTemplateClassDefinition * xdefn_of_symbol = isSgTemplateClassDefinition(scope_of_symbol);
  ROSE_ASSERT(!parent || xdefn_of_symbol);

  SgDeclarationStatement * decl_from_symbol = parent ? xdefn_of_symbol->get_declaration() : nullptr;
  SgDeclarationStatement * decl_from_parent = (parent && !unk_ptype) ? xp->get_declaration() : nullptr;
#if DEBUG__Factory__reference
  std::cout << "  decl_from_symbol = " << std::hex << decl_from_symbol << " : " << ( decl_from_symbol ? decl_from_symbol->class_name() : "" ) << std::endl;
  std::cout << "  decl_from_parent = " << std::hex << decl_from_parent << " : " << ( decl_from_parent ? decl_from_parent->class_name() : "" ) << std::endl;
#endif

  // The rhs of the OR will be true when unk_ptype is true which should only occur when it is a template
  if (is_template_symbol_variant<otag>(sym->variantT()) || (parent && decl_from_symbol != decl_from_parent) ) {
    declaration_t<otag> * decl = instantiate<otag>(obj, parent, args...);
    ROSE_ASSERT(decl != nullptr);
    sym = dynamic_cast<symbol_t<otag> *>(search_for_symbol_from_symbol_table<otag>(decl));
    ROSE_ASSERT(sym != nullptr);
  }

  reference_t<otag> * res = __factory_helper_t<CRT, API, otag>::reference(*this, sym, parent, args...);
  SageInterface::setSourcePositionForTransformation(res);
  return res;
}

#define DEBUG__Factory__access_expr 0

template <typename CRT, typename API>
template <Object otag, typename... Args>
SgExpression * Factory<CRT, API>::access(symbol_t<otag> * API::* obj, SgExpression * parent, Args... args) const {
#if DEBUG__Factory__access_expr
  std::cout << "Factory<CRT, API>::access (expr)" << std::endl;
  std::cout << "  obj    = " << std::hex << obj << std::endl;
  std::cout << "  parent = " << std::hex << parent << " : " << ( parent ? parent->class_name() : "" ) << std::endl;
  std::cout << "         = " << ( parent ? parent->unparseToString() : "" ) << std::endl;
#endif

  SgType * ptype = parent->get_type(); // TODO strip type modifiers and references
#if DEBUG__Factory__access_expr
  std::cout << "  ptype = " << std::hex << ptype << " : " << ( ptype ? ptype->class_name() : "" ) << std::endl;
  std::cout << "        = " << ( ptype ? ptype->unparseToString() : "" ) << std::endl;
#endif
  // Handling use case where `array->operator()(index)` is generated as `array(index)`
  // 
  if (isSgTypeUnknown(ptype)) {
    ROSE_ASSERT(isSgFunctionCallExp(parent));
    SgExpression * fnc = ((SgFunctionCallExp*)parent)->get_function();
#if DEBUG__Factory__access_expr
    std::cout << "  fnc = " << std::hex << fnc << " : " << ( fnc ? fnc->class_name() : "" ) << std::endl;
    std::cout << "      = " << ( fnc ? fnc->unparseToString() : "" ) << std::endl;
#endif
    SgType * ftype = fnc->get_type();
#if DEBUG__Factory__access_expr
    std::cout << "  ftype = " << std::hex << ftype << " : " << ( ftype ? ftype->class_name() : "" ) << std::endl;
    std::cout << "        = " << ( ftype ? ftype->unparseToString() : "" ) << std::endl;
#endif
    while (isSgTypedefType(ftype)) ftype = ((SgTypedefType*)ftype)->get_base_type();
#if DEBUG__Factory__access_expr
    std::cout << "  ftype = " << std::hex << ftype << " : " << ( ftype ? ftype->class_name() : "" ) << std::endl;
    std::cout << "        = " << ( ftype ? ftype->unparseToString() : "" ) << std::endl;
#endif
    ROSE_ASSERT(isSgClassType(ftype));
    SgTemplateInstantiationDecl * tidecl = isSgTemplateInstantiationDecl(((SgClassType*)ftype)->get_declaration());
    ROSE_ASSERT(tidecl != nullptr);
    auto tplargs = tidecl->get_templateArguments();
    ROSE_ASSERT(tplargs.size() > 0);
    ptype = tplargs[0]->get_type();
#if DEBUG__Factory__access_expr
    std::cout << "  ptype = " << std::hex << ptype << " : " << ( ptype ? ptype->class_name() : "" ) << std::endl;
    std::cout << "        = " << ( ptype ? ptype->unparseToString() : "" ) << std::endl;
#endif
  }

  while (isSgModifierType(ptype)) { ptype = ((SgModifierType*)ptype)->get_base_type(); }
#if DEBUG__Factory__access_expr
  std::cout << "  ptype = " << std::hex << ptype << " : " << ( ptype ? ptype->class_name() : "" ) << std::endl;
  std::cout << "        = " << ( ptype ? ptype->unparseToString() : "" ) << std::endl;
#endif

  bool lhs_has_ptr_type = isSgPointerType(parent->get_type());
  if (lhs_has_ptr_type) {
    ptype = ((SgPointerType*)ptype)->get_base_type(); // TODO strip type modifiers and references
  }
#if DEBUG__Factory__access_expr
  std::cout << "  ptype = " << std::hex << ptype << " : " << ( ptype ? ptype->class_name() : "" ) << std::endl;
  std::cout << "        = " << ( ptype ? ptype->unparseToString() : "" ) << std::endl;
#endif

  while (isSgModifierType(ptype)) { ptype = ((SgModifierType*)ptype)->get_base_type(); }
#if DEBUG__Factory__access_expr
  std::cout << "  ptype = " << std::hex << ptype << " : " << ( ptype ? ptype->class_name() : "" ) << std::endl;
  std::cout << "        = " << ( ptype ? ptype->unparseToString() : "" ) << std::endl;
#endif

  while (isSgReferenceType(ptype)) { ptype = ((SgReferenceType*)ptype)->get_base_type(); }
#if DEBUG__Factory__access_expr
  std::cout << "  ptype = " << std::hex << ptype << " : " << ( ptype ? ptype->class_name() : "" ) << std::endl;
  std::cout << "        = " << ( ptype ? ptype->unparseToString() : "" ) << std::endl;
#endif

  SgNamedType * ntype = isSgNamedType(ptype);
  ROSE_ASSERT(ntype != nullptr);

  reference_t<otag> * rhs = reference<otag>(obj, ntype, args...);
  ROSE_ASSERT(rhs != nullptr);

  SgExpression * res = nullptr;
  if (lhs_has_ptr_type) {
    res = SageBuilder::buildArrowExp(parent, rhs);
  } else {
    res = SageBuilder::buildDotExp(parent, rhs);
  }
  SageInterface::setSourcePositionForTransformation(res);
  return res;
}

template <typename CRT, typename API>
template <Object otag, std::enable_if_t<otag == Object::a_class || otag == Object::a_typedef> *>
SgScopedType * Factory<CRT, API>::build_scoped_ref(SgNamedType * lhs, reference_t<otag> * rhs) {
  return new SgScopedType(lhs, rhs);
}

template <typename CRT, typename API>
template <Object otag, std::enable_if_t<otag != Object::a_class && otag != Object::a_typedef> *>
SgScopedRefExp * Factory<CRT, API>::build_scoped_ref(SgNamedType * lhs, reference_t<otag> * rhs) {
  SgTypeRefExp * lhs_e = new SgTypeRefExp(lhs);
  SgScopedRefExp * res = new SgScopedRefExp(lhs_e, rhs);
  lhs_e->set_parent(res);
  rhs->set_parent(res);
  return res;
}

#define DEBUG__Factory__access_type 0

template <typename CRT, typename API>
template <Object otag, typename... Args>
typename Factory<CRT, API>::template access_return_t<otag> * Factory<CRT, API>::access(symbol_t<otag> * API::* obj, SgNamedType * parent, Args... args) const {
#if DEBUG__Factory__access_type
  std::cout << "Factory<CRT, API>::access (type)" << std::endl;
  std::cout << "  obj    = " << std::hex << obj << std::endl;
  std::cout << "  parent = " << std::hex << parent << " : " << ( parent ? parent->class_name() : "" ) << std::endl;
#endif
  reference_t<otag> * rhs = reference<otag>(obj, parent, args...);
  access_return_t<otag> * res =  parent ? (access_return_t<otag> *)build_scoped_ref<otag>(parent, rhs) : (access_return_t<otag> *)rhs;
  SageInterface::setSourcePositionForTransformation(res);
  return res;
}

} } }

#endif /* Rose_SourceCode_CodeGen_factory_factory_txx */

