/*!
 *  \file ASTtools/VarSym.hh
 *
 *  \brief Implements routines to assist in variable symbol analysis
 *  and manipulation.
 */
#include <rose.h>
#include <algorithm>

#include "VarSym.hh"

// ========================================================================

using namespace std;

// ========================================================================

//! Converts a set of variable symbols into a string for debugging.
string
ASTtools::toString (const VarSymSet_t& syms)
{
  stringstream s;
  ASTtools::VarSymSet_t::const_iterator v = syms.begin ();
  bool is_first = true;
  while (v != syms.end ())
    {
      if (is_first)
        is_first = false;
      else
        s << ", ";
      const SgVariableSymbol* sym = *v;
      if (sym)
        {
          const SgInitializedName* n = sym->get_declaration ();
          ROSE_ASSERT (n);
          s << n->get_name ().str ();
        }
      else // !sym
        s << "(nil?)";
      ++v;
    }
  return s.str ();
}

// ========================================================================

/*!
 *  \brief Return an existing variable symbol for the given
 *  initialized name.
 *
 *  This routine checks various scopes in trying to find a suitable
 *  variable symbol for the given initialized name.
 */
static
const SgVariableSymbol *
getVarSymFromName_const (const SgInitializedName* name)
{
  SgVariableSymbol* v_sym = 0;
  if (name)
    {
      SgScopeStatement* s = name->get_scope ();
      ROSE_ASSERT (s);
      v_sym = s->lookup_var_symbol (name->get_name ());

      if (!v_sym) // E.g., might be part of an 'extern' declaration.
        {
          // Try the declaration's scope.
          SgDeclarationStatement* decl = name->get_declaration ();
          ROSE_ASSERT (decl);

          SgScopeStatement* decl_scope = decl->get_scope ();
          if (decl_scope)
            v_sym = decl_scope->lookup_var_symbol (name->get_name ());

          if (!v_sym)
            cerr << "\t\t*** WARNING: Can't seem to find a symbol for '"
                 << name->get_name ().str ()
                 << "' ***" << endl;
        }
    }
  return v_sym;
}

/*!
 *  \brief Return an existing variable symbol for the given
 *  initialized name.
 *
 *  This routine checks various scopes in trying to find a suitable
 *  variable symbol for the given initialized name.
 */
static
SgVariableSymbol *
getVarSymFromName (SgInitializedName* name)
{
  const SgVariableSymbol* v_sym = getVarSymFromName_const (name);
  return const_cast<SgVariableSymbol *> (v_sym);
}

/*!
 *  \brief Returns the SgVariableSymbol associated with an SgVarRefExp
 *  or SgInitializedName, or 0 if none.
 */
static
const SgVariableSymbol *
getVarSym_const (const SgNode* n)
{
  const SgVariableSymbol* v_sym = 0;
  switch (n->variantT ())
    {
    case V_SgVarRefExp:
      v_sym = isSgVarRefExp (n)->get_symbol ();
      break;
    case V_SgInitializedName:
      v_sym = getVarSymFromName_const (isSgInitializedName (n));
      break;
    default:
      break;
    }
  return v_sym;
}

/*!
 *  \brief Returns the SgVariableSymbol associated with an SgVarRefExp
 *  or SgInitializedName, or 0 if none.
 */
static
SgVariableSymbol *
getVarSym (SgNode* n)
{
  const SgVariableSymbol* v_sym = getVarSym_const (n);
  return const_cast<SgVariableSymbol *> (v_sym);
}

/*!
 *  Collect all SgVariableSymbols associated with an SgVarRefExp node
 *  a SgVariableDeclaration node,  or a SgInitializedName (function parameters)
 */
static
void
getVarSyms (SgNode* n, ASTtools::VarSymSet_t* p_syms)
{
  if (!p_syms || !n) return;

  ASTtools::VarSymSet_t& syms = *p_syms;

  switch (n->variantT ())
    {
    case V_SgVariableSymbol:
      {
        SgVariableSymbol* v_sym = isSgVariableSymbol (n);
        ROSE_ASSERT (v_sym);
        syms.insert (v_sym);
      }
      break;
    case V_SgVariableDeclaration:
      {
        SgVariableDeclaration* v_decl = isSgVariableDeclaration (n);
        ROSE_ASSERT (v_decl);
        SgInitializedNamePtrList& names = v_decl->get_variables ();
        transform (names.begin (), names.end (),
                   inserter (syms, syms.begin ()),
                   getVarSymFromName);
      }
      break;
    case V_SgInitializedName:
      {
        SgInitializedName* name = isSgInitializedName (n);
        ROSE_ASSERT (name);
        getVarSyms (getVarSym (name), p_syms);
      }
    default:
      break;
    }
}

// ========================================================================

static
const SgVariableSymbol *
getFirstVarSym_const (const SgVariableDeclaration* decl)
{
  if (!decl) return 0;
  const SgInitializedNamePtrList& names = decl->get_variables ();
  if (names.begin () != names.end ())
    {
      const SgInitializedName* name = *(names.begin ());
      return getVarSym_const (name);
    }
  else
    return 0;
}


SgVariableSymbol *
ASTtools::getFirstVarSym (SgVariableDeclaration* decl)
{
  const SgVariableSymbol* sym = getFirstVarSym_const (decl);
  return const_cast<SgVariableSymbol *> (sym);
}

void
ASTtools::collectRefdVarSyms (const SgStatement* s, VarSymSet_t& syms)
{
  // First, collect all variable reference expressions, {e}
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t var_refs = NodeQuery::querySubTree (const_cast<SgStatement *> (s), V_SgVarRefExp);

  // Next, insert the variable symbol for each e into syms.
  transform (var_refs.begin (),
             var_refs.end (),
             inserter (syms, syms.begin ()),
             getVarSym);
}

// ========================================================================

void
ASTtools::collectDefdVarSyms (const SgStatement* s, VarSymSet_t& syms)
{
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t vars_local = NodeQuery::querySubTree (const_cast<SgStatement *> (s), V_SgVariableDeclaration);
  for_each (vars_local.begin (), vars_local.end (), bind2nd (ptr_fun (getVarSyms), &syms));
}

void
ASTtools::collectLocalVisibleVarSyms (const SgStatement* root,
                                      const SgStatement* target,
                                      VarSymSet_t& syms)
{
  //! Traversal to collect variable symbols, with early stopping.
  class Collector : public AstSimpleProcessing
  {
  public:
    Collector (const SgStatement* target, VarSymSet_t& syms)
      : target_ (target), syms_ (syms)
    {
    }

    virtual void visit (SgNode* n)
    { 
      //Stop the traversal once target node is met.
      if (isSgStatement (n) == target_)
        throw string ("done");
      getVarSyms (n, &syms_);
#if 1
      // Liao, 12/18/2007
      // for Fortran, variables without declarations are legal,but easy to miss
      // grab them all from symbol tables
      SgScopeStatement* scope = isSgScopeStatement(n);
      if(scope) {
        SgSymbolTable * table = scope->get_symbol_table();
        std::set<SgNode*> nodeset = table->get_symbolSet();
        for (std::set<SgNode*>::iterator i=nodeset.begin();i!=nodeset.end();i++)
        {
            SgVariableSymbol* varsymbol = isSgVariableSymbol (*i);
            if(varsymbol) getVarSyms (varsymbol, &syms_);
        }
      }// end if scope
#endif
    }

  private:
    const SgStatement* target_; //!< Node at which to stop search.
    VarSymSet_t& syms_; //!< Container in which to collect symbols.
  };

  // Do collection
  Collector collector (target, syms);
  try
    {
      collector.traverse (const_cast<SgStatement *> (root), preorder);
    }
  catch (string& stopped_early)
    {
      ROSE_ASSERT (stopped_early == "done");
    }
}

// eof
