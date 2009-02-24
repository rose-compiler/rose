// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/**
 *  \file Recognizer.h
 *  \brief Implements a class that recognizes API signature entities in
 *  an AST.
 */
#include <rose.h>
#include <cassert>
#include <string>
#include "Recognizer.h"

//=======================================================================

using namespace std;

//=======================================================================
// Helper functions.

/*!
 *  \brief Given forward iterators into a container, returns the i-th
 *  element, where 'i' is a 1-based index (i.e., first element has
 *  index 'i == 1').
 */
template <typename FwdInIter_T>
static
typename FwdInIter_T::value_type
getElementAtIndex (size_t i, FwdInIter_T cur, FwdInIter_T end)
{
  assert (i);
  --i;
  for (; cur != end && i; ++cur, --i)
    ;
  assert (cur != end);
  return *cur;
}

//=======================================================================
namespace C_API
{
  //=======================================================================
  Recognizer::Recognizer (void)
    : sigs_ (0)
  {
  }

  //=======================================================================
  Recognizer::Recognizer (const SigMap_t* sigs)
    : sigs_ (sigs)
  {
  } // Recognizer::Recognizer ()

  //=======================================================================
  void
  Recognizer::setSigs (const SigMap_t* sigs)
  {
    sigs_ = sigs;
  } // Recognizer::setSigs ()

  //=======================================================================
  const SigBase *
  Recognizer::lookup (const string& name) const
  {
    if (sigs_)
      {
	SigMap_t::const_iterator sig = sigs_->find (name);
	if (sig != sigs_->end ())
          return sig->second;
      }
    return 0; // Default case: Not found.
  } // Recognizer::lookup (const string &)

  //=======================================================================
  const FuncSig *
  Recognizer::lookup (const SgFunctionCallExp* call, const string& name) const
  {
    if (call)
      {
	const SgFunctionRefExp* func_ref =
	  isSgFunctionRefExp (call->get_function ());
	if (func_ref)
	  return lookup (func_ref, name);
      }
    return 0; // Not found
  } // Recognizer::lookup (const SgFunctionCallExp *)

  const FuncSig *
  Recognizer::lookup (const SgFunctionRefExp* ref, const string& name) const
  {
    if (ref)
      {
	const SgFunctionSymbol* sym = ref->get_symbol ();
	return lookup (sym, name);
      }
    return 0; // Not found
  } // Recognizer::lookup (const SgFunctionRefExp *)

  const FuncSig *
  Recognizer::lookup (const SgFunctionSymbol* sym, const string& name) const
  {
    if (sym)
      {
	const SgFunctionDeclaration* decl = sym->get_declaration ();
	return lookup (decl, name);
      }
    return 0; // Not found
  } // Recognizer::lookup (const SgFunctionSymbol *)

  const FuncSig *
  Recognizer::lookup (const SgFunctionDeclaration* decl,
                      const string& name) const
  {
    if (decl)
      {
        string decl_name = decl->get_name ().getString ();

	// Matching criteria: 'decl' must be a global function with
	// the same name and number of arguments as some signature
	// entity. Note that name match is always 'yes' if 'name'
        // is the empty string.
	if (isSgGlobal (decl->get_scope ())
            && (name.empty () || (!name.empty () && name == decl_name)))
	  {
	    const SigBase* sig_base = lookup (decl_name);
	    const FuncSig* sig = dynamic_cast<const FuncSig *> (sig_base);
	    if (sig && sig->getNumArgs () == decl->get_args ().size ())
	      return sig;
	  }
      }
    return 0;
  } // Recognizer::lookup (const SgFunctionDeclaration *)

  //=======================================================================
  const ConstSig *
  Recognizer::lookup (const SgVarRefExp* ref, const string& name) const
  {
    if (ref)
      {
	const SgVariableSymbol* sym = ref->get_symbol ();
	return lookup (sym, name);
      }
    return 0;
  } // Recognizer::lookup (const SgVarRefExp *)

  const ConstSig *
  Recognizer::lookup (const SgVariableSymbol* sym, const string& name) const
  {
    if (sym)
      {
	const SgInitializedName* i_name = sym->get_declaration ();
	return lookup (i_name, name);
      }
    return 0;
  } // Recognizer::lookup (const SgVariableSymbol *)

  const ConstSig *
  Recognizer::lookup (const SgInitializedName* i_name,
                      const string& name) const
  {
    if (i_name && isSgGlobal (i_name->get_scope ()))
      {
        string i_name_str = i_name->get_name ().getString ();
        if (name.empty ()
            || (!name.empty () && name == i_name_str))
          {
            const SigBase* sig_base = lookup (i_name_str);
            const ConstSig* sig = dynamic_cast<const ConstSig *> (sig_base);
            return sig;
          }
      }
    return 0; // Not found
  } // Recognizer::lookup (const SgInitializedName *)

  //=======================================================================
  const SgFunctionCallExp *
  Recognizer::isCall (const SgNode* n, const string& name) const
  {
    const SgFunctionCallExp* call = isSgFunctionCallExp (n);
    return lookup (call, name) ? call : 0;
  } // Recognizer::isCall ()

  //=======================================================================
  const SgFunctionRefExp *
  Recognizer::isFuncRef (const SgNode* n, const string& name) const
  {
    const SgFunctionRefExp* ref = isSgFunctionRefExp (n);
    return lookup (ref, name) ? ref : 0;
  } // Recognizer::isFuncRef ()

  //=======================================================================
  const SgExpression *
  Recognizer::getArg (const SgFunctionCallExp* call,
		      const std::string& arg_name) const
  {
    const FuncSig* sig = lookup (call);
    if (sig)
      {
	size_t i = sig->getArg (arg_name);
	if (i)
	  {
	    const SgExprListExp* arg_exprs = call->get_args ();
	    ROSE_ASSERT (arg_exprs);
	    const SgExpressionPtrList& args = arg_exprs->get_expressions ();
	    ROSE_ASSERT (i <= args.size ());
	    return getElementAtIndex (i, args.begin (), args.end ());
	  }
      }
    return 0; // Not found
  } // Recognizer::getArg ()

  //=======================================================================
  bool
  Recognizer::isConst (const SgExpression* e,
                       const string& name) const
  {
    const SgVarRefExp* var = isSgVarRefExp (e);
    return lookup (var, name);
  } // Recognizer::isConst ()

} // namespace C_API

// eof
