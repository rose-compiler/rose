/**
 *  \file Transform/Insert.cc
 *
 *  \brief Inserts the outlined function declarations (i.e., including
 *  prototypes) and function calls.
 */

#include <iostream>
#include <list>
#include <string>
#include <sstream>

#include <rose.h>
#include "Transform.hh"
#include "ASTtools.hh"
#include "PreprocessingInfo.hh"
#include "StmtRewrite.hh"
#include "Copy.hh"

// =====================================================================

typedef std::list<SgFunctionDeclaration *> FuncDeclList_t;

// =====================================================================

using namespace std;

// =====================================================================

//! Creates a 'prototype' (forward declaration) for a function.
static
SgFunctionDeclaration *
generatePrototype (const SgFunctionDeclaration* full_decl, SgScopeStatement* scope)
{
  if (!full_decl) return 0; // nothing to do

#if 1
  // Temporarily "hide" definition.
  SgFunctionDeclaration* tmp = const_cast<SgFunctionDeclaration *> (full_decl);
  SgFunctionDefinition* def = full_decl->get_definition ();
  ROSE_ASSERT (tmp);
  tmp->set_definition (0);

  // Clone the declaration with the hidden definition.
  SgFunctionDeclaration* proto =
    isSgFunctionDeclaration (ASTtools::deepCopy (tmp));
  ROSE_ASSERT (proto);

  // Restore the definition.
  tmp->set_definition (def);
  def->set_parent (tmp);

  proto->set_forward (true);
  proto->set_definingDeclaration (tmp);
#else
  SgFunctionDeclaration* proto = SageBuilder::buildNondefiningFunctionDeclaration(full_decl,scope);
  ROSE_ASSERT(proto!=NULL);
#endif  
  return proto;
}

//! Generates a 'friend' declaration from a given function declaration.
static
SgFunctionDeclaration *
generateFriendPrototype (const SgFunctionDeclaration* full_decl, SgScopeStatement* scope)
{
  SgFunctionDeclaration* proto = generatePrototype (full_decl,scope);
  ROSE_ASSERT (proto);

  // Remove any 'extern' modifiers
  proto->get_declarationModifier ().get_storageModifier ().reset ();

  // Set the 'friend' modifier
  proto->get_declarationModifier ().setFriend ();
  return proto;
}

/*!
 *  \brief Beginning at the given declaration statement, this routine
 *  searches for first declaration in global scope that appears before
 *  this one.
 */
static
SgDeclarationStatement *
findClosestGlobalInsertPoint (SgDeclarationStatement* f)
{
  ROSE_ASSERT (f);
  SgDeclarationStatement* closest = f;
  SgNode* cur_parent = f->get_parent ();
  while (cur_parent && !isSgGlobal (cur_parent))
    {
      if (isSgDeclarationStatement (cur_parent))
        closest = isSgDeclarationStatement (cur_parent);
      cur_parent = cur_parent->get_parent ();
    }
  return isSgGlobal (cur_parent) ? closest : 0;
}

/*!
 *  Traversal to insert a new global prototype.
 *
 *  This traversal searches for the first non-defining declaration for
 *  a given function definition, and inserts a new prototype of that
 *  function into global scope. In addition, it fixes up the
 *  definition's first non-defining declaration field to point to the
 *  new global prototype.
 *
 *  The traversal terminates as soon as the first matching declaration
 *  is found by throwing an exception as a string encoded with the
 *  word "done". The caller must ensure that any other matching
 *  declarations have their first non-defining declaration fields
 *  fixed up as well.
 */
class GlobalProtoInserter : public AstSimpleProcessing
{
public:
  GlobalProtoInserter (SgFunctionDeclaration* def, SgGlobal* scope)
    : def_decl_ (def), glob_scope_ (scope), proto_ (0)
  {
  }

  virtual void visit (SgNode* cur_node)
  {
    SgFunctionDeclaration* cur_decl = isSgFunctionDeclaration (cur_node);
    if (cur_decl && cur_decl->get_definingDeclaration () == def_decl_
        && !isSgGlobal (cur_decl->get_parent ()))
//        && isSgGlobal (cur_decl->get_parent ()) != glob_scope_)
      {
        proto_ = insertManually (def_decl_, glob_scope_, cur_decl);
        throw string ("done");
      }
  }

  SgFunctionDeclaration* getProto (void) { return proto_; }
  const SgFunctionDeclaration* getProto (void) const { return proto_; }

  static SgFunctionDeclaration*

  insertManually (SgFunctionDeclaration* def_decl,
                  SgGlobal* scope,
                  SgDeclarationStatement* target)
  {
    SgFunctionDeclaration* proto = generatePrototype (def_decl,scope);
    ROSE_ASSERT (proto);
    
    SgDeclarationStatement* insert_point =
      findClosestGlobalInsertPoint (target);
    ROSE_ASSERT (insert_point);

    ASTtools::moveBeforePreprocInfo (insert_point, proto);    
#if 1    
    scope->insert_statement (insert_point, proto, true);
    proto->set_parent (scope);
    proto->set_scope (scope);
#else
    // this only insert it under a parent node,not a scope node
    //SageInterface::insertStatementBefore(insert_point,proto);
    SageInterface::prependStatement(proto,scope);
#endif    
    if (!Outliner::useNewFile)
      def_decl->set_firstNondefiningDeclaration (proto);

    return proto;
  }

private:
  //! Defining declaration.
  SgFunctionDeclaration* def_decl_;

  //! Global scope.
  SgGlobal* glob_scope_;

  //! New global prototype (i.e., new first non-defining declaration).
  SgFunctionDeclaration* proto_;
};

//! Inserts a prototype into the original global scope of the outline target
static
SgFunctionDeclaration *
insertGlobalPrototype (SgFunctionDeclaration* def,
                       FuncDeclList_t& protos,
                       SgGlobal* scope,
                       SgDeclarationStatement* default_target)
{
  SgFunctionDeclaration* proto = 0;
  if (def && scope)
  {
    try
    {
      GlobalProtoInserter ins (def, scope);
      ins.traverse (scope, preorder);
      proto = ins.getProto ();
    }
    catch (string& s) { ROSE_ASSERT (s == "done"); }

    if (!proto && default_target) // No declaration found
      proto = GlobalProtoInserter::insertManually (def,
          scope,
          default_target);
  }

  // Fix-up remaining prototypes.
  if (proto)
    for (FuncDeclList_t::iterator i = protos.begin (); i != protos.end (); ++i)
    {
      SgFunctionDeclaration* proto_i = *i;
      ROSE_ASSERT (proto_i);
      proto_i->set_firstNondefiningDeclaration (proto);
      proto_i->set_definingDeclaration (def);
    }

  return proto;
}

/*!
 *  \brief Given a 'friend' declaration, insert it into the given
 *  class definition.
 */
static
SgFunctionDeclaration *
insertFriendDecl (const SgFunctionDeclaration* func,
                  SgGlobal* scope,
                  SgClassDefinition* cls_scope)
{
  SgFunctionDeclaration* friend_proto = 0;
  if (func && scope && cls_scope)
    {
      // Determine insertion point, i.
      SgDeclarationStatementPtrList& mems = cls_scope->get_members ();
      SgDeclarationStatementPtrList::iterator i = mems.begin ();

      // Create the friend declaration.
      friend_proto = generateFriendPrototype (func,cls_scope);
      ROSE_ASSERT (friend_proto);

      // Insert it into the class.
      if (i != mems.end ())
        ASTtools::moveBeforePreprocInfo ((*i), friend_proto);
      cls_scope->get_members().insert(i, friend_proto);
      friend_proto->set_parent (cls_scope);
      friend_proto->set_scope (scope);
    }
  return friend_proto;
}

/*!
 *  \brief Returns 'true' if the given declaration statement is marked
 *  as 'private' or 'protected'.
 */
static
bool
isProtPriv (const SgDeclarationStatement* decl)
{
  if (decl)
    {
      SgDeclarationStatement* decl_tmp =
        const_cast<SgDeclarationStatement *> (decl);
      ROSE_ASSERT (decl_tmp);
      const SgAccessModifier& decl_access_mod =
        decl_tmp->get_declarationModifier ().get_accessModifier ();
      return decl && (decl_access_mod.isPrivate ()
                      || decl_access_mod.isProtected ());
    }
  return false;
}

/*!
 *  \brief Returns 'true' if the given variable use is a 'protected'
 *  or 'private' class member.
 */
static
SgClassDefinition *
isProtPrivMember (SgVarRefExp* v)
{
  if (v)
    {
      SgVariableSymbol* sym = v->get_symbol ();
      if (sym)
        {
          SgInitializedName* name = sym->get_declaration ();
          ROSE_ASSERT (name);
          SgClassDefinition* cl_def =
            isSgClassDefinition (name->get_scope ());
          if (cl_def && isProtPriv (name->get_declaration ()))
            return cl_def;
        }
    }
  return 0; // default: is not
}

/*!
 *  \brief Returns 'true' if the given type was declared as a
 *  'protected' or 'private' class member.
 */
static
SgClassDefinition *
isProtPrivType (SgType* t)
{
  if (t)
    {
      SgType* base = t->findBaseType ();
      if (base && isSgNamedType (base))
        {
          SgNamedType* named = isSgNamedType (base);
          ROSE_ASSERT (named);
          SgDeclarationStatement* decl = named->get_declaration ();
          if (decl)
            if (decl->get_definingDeclaration ())
              decl = decl->get_definingDeclaration ();
          if (isProtPriv (decl))
            return isSgClassDefinition (decl->get_parent ());
        }
    }
  return false;
}

/*!
 *  \brief Returns 'true' if the given member function is 'protected'
 *  or 'private'.
 */
static
SgClassDefinition *
isProtPrivMember (SgMemberFunctionRefExp* f)
{
  if (f)
    {
      SgMemberFunctionSymbol* sym = f->get_symbol ();
      if (sym)
        {
          SgMemberFunctionDeclaration* f_decl = sym->get_declaration ();
          ROSE_ASSERT (f_decl);
          SgClassDefinition* cl_def = sym->get_scope ();
          if (cl_def && isProtPriv (f_decl))
            return cl_def;
        }
    }
  return 0; // default: is not
}

/*!
 *  \brief Inserts all necessary friend declarations.
 *
 *  \returns A list, 'friends', of all generated friend declarations.
 */
static
void
insertFriendDecls (SgFunctionDeclaration* func,
                   SgGlobal* scope,
                   FuncDeclList_t& friends)
{
  if (func && scope)
    {
      // Collect a list of all classes that need a 'friend' decl.
      typedef set<SgClassDefinition *> ClassDefSet_t;
      ClassDefSet_t classes;
      
      // First, look for references to private variables.
      typedef Rose_STL_Container<SgNode *> NodeList_t;
      NodeList_t var_refs = NodeQuery::querySubTree (func, V_SgVarRefExp);
      for (NodeList_t::iterator v = var_refs.begin (); v != var_refs.end (); ++v)
        {
          SgVarRefExp* v_ref = isSgVarRefExp (*v);
          SgClassDefinition* cl_def = isProtPrivMember (v_ref);
          if (!cl_def)
            cl_def = isProtPrivType (v_ref->get_type ());
          
          if (cl_def)
            classes.insert (cl_def);
        }
      
      // Get a list of all function reference expressions.
      NodeList_t func_refs = NodeQuery::querySubTree (func,
                                                      V_SgMemberFunctionRefExp);
      for (NodeList_t::iterator f = func_refs.begin (); f != func_refs.end ();
           ++f)
        {
          SgMemberFunctionRefExp* f_ref = isSgMemberFunctionRefExp (*f);
          SgClassDefinition* cl_def = isProtPrivMember (f_ref);
          if (cl_def)
            classes.insert (cl_def);
        }

      // Insert 'em
      for (ClassDefSet_t::iterator c = classes.begin (); c != classes.end (); ++c)
        {
          ROSE_ASSERT (*c);
          SgFunctionDeclaration* friend_decl = insertFriendDecl (func, scope, *c);
          ROSE_ASSERT (friend_decl);
          friends.push_back (friend_decl);
        }
    }
}

// =====================================================================
//! Insert func into scope (could be either original scope or the new scope from a new file), 
//  and insert necessary declarations into the global scope of
//  target's original enclosing function). 
void
Outliner::Transform::insert (SgFunctionDeclaration* func,
                                SgGlobal* scope,
                                SgFunctionDeclaration* target_func)
{
  ROSE_ASSERT (func && scope && target_func);

  // x. Insert the defining function 
  // Put the actual defining declaration at the end of global scope.
#if 1 
  scope->append_declaration (func);
  func->set_scope (scope);
  func->set_parent (scope);
#else
  SageInterface::appendStatement(func,scope);
#endif  

  // x. Insert the defining function's prototype right before target_func
  //
  // the scope parameter may be from the new source file 
  // so we grab the original global scope from the target_func (original enclosing function)
  SgGlobal* src_global = SageInterface::getGlobalScope(target_func);
  ROSE_ASSERT(src_global != NULL);
  if (!Outliner::useNewFile)
    ROSE_ASSERT(scope == src_global);

  // no forward declaration is needed for Fortran 77 
  // Liao, 12/13/2007
  if (!SageInterface::is_Fortran_language())
  {
    // Insert all necessary 'friend' declarations.
    FuncDeclList_t protos;
    insertFriendDecls (func, src_global, protos);

    // Insert a single, global prototype (i.e., a first non-defining
    // declaration), which specifies the linkage property of 'func'.
    insertGlobalPrototype (func, protos, src_global, target_func);
  }
  else
  {
    func->set_firstNondefiningDeclaration (func);
    //     func->set_definingDeclaration (def);
  }

#if 1 //Should be removed once SageBuilder is used
  // DQ (9/7/2007): Need to add function symbol to global scope!
  //   printf ("Fixing up the symbol table in scope = %p = %s for function = %p = %s \n",glob_scope,glob_scope->class_name().c_str(),func,func->get_name().str());
  if (src_global->lookup_function_symbol(func->get_name()) == NULL)  
  {
    SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(func);
    src_global->insert_symbol(func->get_name(),functionSymbol);
    ROSE_ASSERT(src_global->lookup_function_symbol(func->get_name()) != NULL);
  }

// Fixup the symbol in the newly generated file for the inserted function definition
  if (Outliner::useNewFile &&
      (scope->lookup_function_symbol(func->get_name()) == NULL) ) 
  {
    SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(func);
    scope->insert_symbol(func->get_name(),functionSymbol);
    ROSE_ASSERT(scope->lookup_function_symbol(func->get_name()) != NULL);
  }
#endif 
  ROSE_ASSERT(func->get_definingDeclaration()         != NULL);
  if  (!Outliner::useNewFile) 
  {
    // there can be NULL nondefining declaration if it is inserted into a brand new source file
    ROSE_ASSERT(func->get_firstNondefiningDeclaration() != NULL);
  }
}

// eof
