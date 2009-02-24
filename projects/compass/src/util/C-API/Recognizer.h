// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/**
 *  \file Recognizer.h
 *  \brief Declares a class that recognizes API signature entities in
 *  an AST.
 */

#if !defined (INC_C_API_RECOGNIZER_H)
#define INC_C_API_RECOGNIZER_H //!< Recognizer.h included.

#include <C-API/C-API.h>


namespace C_API
{
  /*!
   *  \brief A generic API recognizer.
   *
   *  \note The 'isXXX' routines accept optional string arguments. If
   *  these names are blank (default), then the routine will look for
   *  any match to the given node.
   */
  class Recognizer
  {
  public:
    Recognizer (const SigMap_t* sigs);
    virtual ~Recognizer (void) {}

    //! Returns non-NULL iff the node is a named API call.
    const SgFunctionCallExp* isCall (const SgNode* n,
                                     const std::string& name = std::string ("")) const;

    //! Returns non-NULL iff the node is a named API function reference.
    const SgFunctionRefExp* isFuncRef (const SgNode* n,
                                       const std::string& name = std::string ("")) const;

    //! Returns non-NULL iff the node is a named API declaration.
    const SgFunctionDeclaration* isFuncDecl (const SgNode* n,
                                             const std::string& name = std::string ("")) const;

    //! Returns the named argument for an API call, 'call'.
    const SgExpression* getArg (const SgFunctionCallExp* call,
                                const std::string& arg_name) const;

    //! Returns true iff expression is a named API constant.
    bool isConst (const SgExpression* e,
                  const std::string& name = std::string ("")) const;

    //! \name Look-up an entity by name.
    //@{
    const SigBase* lookup (const std::string& name) const;
    const FuncSig* lookup (const SgFunctionCallExp* call,
                           const std::string& name = std::string ("")) const;
    const FuncSig* lookup (const SgFunctionRefExp* ref,
                           const std::string& name = std::string ("")) const;
    const FuncSig* lookup (const SgFunctionSymbol* sym,
                           const std::string& name = std::string ("")) const;
    const FuncSig* lookup (const SgFunctionDeclaration* decl,
                           const std::string& name = std::string ("")) const;
    const ConstSig* lookup (const SgVarRefExp* ref,
                            const std::string& name = std::string ("")) const;
    const ConstSig* lookup (const SgVariableSymbol* sym,
                            const std::string& name = std::string ("")) const;
    const ConstSig* lookup (const SgInitializedName* i_name,
                            const std::string& name = std::string ("")) const;
    //@}

  protected:
    Recognizer (void);

    //! Shallow-copies the map of signatures to recognize.
    void setSigs (const SigMap_t* sigs);

  private:
    const SigMap_t* sigs_; //!< Signatures this recognizer understands.
  };
} // namespace C_API

#endif // !defined (INC_C_API_RECOGNIZER_H)

// eof
