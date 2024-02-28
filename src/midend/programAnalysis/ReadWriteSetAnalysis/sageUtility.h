#ifndef SAGE_UTIL_H
#define SAGE_UTIL_H 1

/// copied from XPlacer project

#include <string>

#include <boost/lexical_cast.hpp>

#include <sage3basic.h>

namespace SageUtil
{
namespace
{
  inline
  std::string str(const SgNode* n)
  {
    return (n? n->unparseToString() : std::string("<null>"));
  }

  inline
  std::string str(const Sg_File_Info* n)
  {
    if (!n) return "?";

    return ( n->get_filenameString()
           + " : "
           + boost::lexical_cast<std::string>(n->get_line())
           );
  }

  template <class ErrorClass = std::runtime_error>
  inline
  void chk(bool success, const std::string& a, std::string b = "", std::string c = "")
  {
    if (!success) throw ErrorClass(a + b + c);
  }

  template <class ErrorClass = std::runtime_error>
  inline
  void chk(bool success, const std::string& a, const SgNode& n)
  {
    if (!success) throw ErrorClass(a + n.unparseToString());
  }


  /// returns the parameter list of a function call
  inline
  const SgInitializedNamePtrList& paramlist(const SgFunctionDeclaration& fndcl)
  {
    return fndcl.get_args();
  }

  /// returns the parameter list of a function call
  inline
  const SgInitializedNamePtrList& paramlist(const SgFunctionDefinition& fndef)
  {
    return paramlist(SG_DEREF(fndef.get_declaration()));
  }

  /// returns the argument list of a function call
  inline
  SgExpressionPtrList& arglist(const SgCallExpression& call)
  {
    SgExprListExp& args = sg::deref(call.get_args());

    return args.get_expressions();
  }

  /// returns the argument list of a function call
  inline
  SgExpressionPtrList& arglist(const SgCallExpression* call)
  {
    return arglist(sg::deref(call));
  }

  /// returns the n-th argument of a function call
  inline
  SgExpression& argN(const SgCallExpression& call, size_t n)
  {
    return sg::deref(arglist(call).at(n));
  }

  /// returns the n-th argument of a function call
  inline
  SgExpression& argN(const SgCallExpression* call, size_t n)
  {
    return argN(sg::deref(call), n);
  }

  /// returns the declaration associated with a variable reference
  inline
  SgInitializedName& decl(const SgVarRefExp& n)
  {
    SgVariableSymbol& sym = SG_DEREF(n.get_symbol());

    return SG_DEREF(sym.get_declaration());
  }

  /// returns the declaration associated with a function reference
  /// \note assers that the declaration is available
  inline
  SgFunctionDeclaration& decl(const SgFunctionRefExp& n)
  {
    return SG_DEREF(n.getAssociatedFunctionDeclaration());
  }


  /// Returns a translation unit's key declaration of a given declaration.
  /// \tparam a type that is a SgDeclarationStatement or inherited from it.
  /// \param  dcl a valid sage declaration (&dcl must not be nullptr).
  /// \details
  ///    a key declaration is the defining declaration (if available),
  ///    or the first non-defining declaration (if a definition is not available).
  template <class SageDecl>
  inline
  SageDecl& keyDecl(const SageDecl& dcl)
  {
    SgDeclarationStatement* keydcl = dcl.get_definingDeclaration();
    if (keydcl) return SG_ASSERT_TYPE(SageDecl, *keydcl);

    keydcl = dcl.get_firstNondefiningDeclaration();
    return SG_ASSERT_TYPE(SageDecl, SG_DEREF(keydcl));
  }

  /// returns the key declaration associated with a function reference
  /// \note asserts that the function is available
  inline
  SgFunctionDeclaration& keyDecl(const SgFunctionRefExp& n)
  {
    return keyDecl(decl(n));
  }

  /// returns the initialized name of a variable declaration
  /// \note asserts that a variable declaration only initializes
  ///       a single variable.
  inline
  SgInitializedName& initName(const SgVariableDeclaration& n)
  {
    const SgInitializedNamePtrList& varlst = n.get_variables();
    ROSE_ASSERT(varlst.size() == 1);

    return SG_DEREF(varlst.at(0));
  }

  /// returns the key declaration of a variable declaration
  /// \note the key declaration of a variable is the initialized
  ///       name (SgInitializedName) of the defining declaration.
  ///       If no defining declaration is available, the initialized
  ///       name of the first non-defining declaration is used.
  inline
  SgInitializedName& keyDecl(const SgVariableDeclaration& n)
  {
    SgDeclarationStatement* defdcl   = n.get_definingDeclaration();

    if (defdcl)
      return initName(SG_ASSERT_TYPE(SgVariableDeclaration, *defdcl));

    defdcl = n.get_firstNondefiningDeclaration();
    return initName(SG_ASSERT_TYPE(SgVariableDeclaration, SG_DEREF(defdcl)));
  }

  /// auxiliary struct for finding key variable declarations
  struct KeyDeclFinder : sg::DispatchHandler<SgInitializedName*>
  {
      explicit
      KeyDeclFinder(const SgInitializedName& dcl)
      : var(dcl)
      {}

      void handle(SgNode& n)                { SG_UNEXPECTED_NODE(n); }
      void handle(SgVariableDeclaration& n) { res = &keyDecl(n); }
      void handle(SgFunctionParameterList&)
      {
        // \todo should this be the parameter in a key-declaration
        //       or the declaration that defines the default argument.
        res = &const_cast<SgInitializedName&>(var);
      }

    private:
      const SgInitializedName& var;
  };

  /// returns the key declaration of an initializd name
  inline
  SgInitializedName& keyDecl(const SgInitializedName& n)
  {
    SgInitializedName* res = sg::dispatch(KeyDeclFinder(n), n.get_declaration());

    return SG_DEREF(res);
  }

  /// returns the key declaration of a variable reference
  inline
  SgInitializedName& keyDecl(const SgVarRefExp& n)
  {
    return keyDecl(decl(n));
  }

  /// gives key declarations a distinct type
  ///   (e.g., for use inside a map, or not to have to reacquire the key decl from a decl...)
  template <class SageDecl>
  struct KeyDecl
  {
      // implicit
      template <class SageNode>
      KeyDecl(SageNode& n)
      : keydcl(keyDecl(n))
      {}

      KeyDecl(const KeyDecl&) = default;
      KeyDecl(KeyDecl&&)      = default;

      SageDecl& decl()       const { return keydcl;  }
      SageDecl* operator->() const { return &keydcl; }
      
      template <class SageKeyNode>
      friend
      bool operator==(const KeyDecl<SageKeyNode>& lhs, const KeyDecl<SageKeyNode>& rhs)
      {
        return &lhs.keydcl == &rhs.keydcl;
      }
      
      template <class SageKeyNode>
      friend
      bool operator<(const KeyDecl<SageKeyNode>& lhs, const KeyDecl<SageKeyNode>& rhs)
      {
        return &lhs.keydcl < &rhs.keydcl;
      }
      
    private:
      SageDecl& keydcl;

      KeyDecl()                          = delete;
      KeyDecl& operator=(KeyDecl&&)      = delete;
      KeyDecl& operator=(const KeyDecl&) = delete;
  };

  enum TypeSkipMode : size_t
  {
    modifiers  = 1,
    lvaluerefs = 2,
    rvaluerefs = 4,
    references = lvaluerefs + rvaluerefs,
    typedefs   = 8,
    arrays     = 16,
    pointers   = 32
  };

  struct TypeSkipper : sg::DispatchHandler<SgType*>
  {
    typedef sg::DispatchHandler<SgType*> base;

    explicit
    TypeSkipper(TypeSkipMode m)
    : base(), mode(m)
    {}

    SgType* recurse(SgNode* ty) { return sg::dispatch(TypeSkipper(mode), ty); }

    void handle(SgNode& n)      { SG_UNEXPECTED_NODE(n); }

    void handle(SgType& n)      { res = &n; }

    void handle(SgModifierType& n)
    {
      res = (mode & modifiers) ? recurse(n.get_base_type()) : &n;
    }

    void handle(SgReferenceType& n)
    {
      res = (mode & lvaluerefs) ? recurse(n.get_base_type()) : &n;
    }

    void handle(SgRvalueReferenceType& n)
    {
      res = (mode & rvaluerefs) ? recurse(n.get_base_type()) : &n;
    }

    void handle(SgPointerType& n)
    {
      res = (mode & pointers) ? recurse(n.get_base_type()) : &n;
    }

    void handle(SgArrayType& n)
    {
      res = (mode & arrays) ? recurse(n.get_base_type()) : &n;
    }

    void handle(SgTypedefType& n)
    {
      res = (mode & typedefs) ? recurse(n.get_base_type()) : &n;
    }

    TypeSkipMode mode;
  };

  inline
  SgType& skipTypes(SgType& ty, size_t mode)
  {
    SgType* res = sg::dispatch(TypeSkipper(TypeSkipMode(mode)), &ty);
    return SG_DEREF(res);
  }

  /// returns a type without modifiers
  inline
  SgType& skipTypeModifier(SgType& t)
  {
    return skipTypes(t, modifiers);
  }

  inline
  SgType& type(SgExpression& n)
  {
    return SG_DEREF(n.get_type());
  }

  /// returns name of symbol
  inline
  SgName nameOf(const SgSymbol& varsy)
  {
    return varsy.get_name();
  }

  inline
  SgName nameOf(const SgVarRefExp& var_ref)
  {
    return nameOf(SG_DEREF(var_ref.get_symbol()));
  }
} // anonymous namespace
} // namespace SageUtil
#endif /* SAGE_UTIL_H */
