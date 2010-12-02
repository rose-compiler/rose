/**
 *  \file Transform/Insert.cc
 *
 *  \brief Inserts the outlined function declarations (i.e., including
 *  prototypes) and function calls.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include <iostream>
#include <list>
#include <string>
#include <sstream>


#include "Outliner.hh"
#include "ASTtools.hh"
#include "PreprocessingInfo.hh"
#include "StmtRewrite.hh"
#include "Copy.hh"

// =====================================================================

typedef std::vector<SgFunctionDeclaration *> FuncDeclList_t;

// =====================================================================

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

// =====================================================================

//! Creates a 'prototype' (forward declaration) for a function.
static
SgFunctionDeclaration *
generatePrototype (const SgFunctionDeclaration* full_decl, SgScopeStatement* scope)
   {
     if (!full_decl) return 0; // nothing to do

#if 0
  // Temporarily "hide" definition.
     SgFunctionDeclaration* tmp = const_cast<SgFunctionDeclaration *> (full_decl);
     SgFunctionDefinition* def = full_decl->get_definition();
     ROSE_ASSERT (tmp != NULL);
     tmp->set_definition (NULL);

  // Clone the declaration with the hidden definition.
     SgFunctionDeclaration* proto = isSgFunctionDeclaration (ASTtools::deepCopy (tmp));
     ROSE_ASSERT (proto != NULL);

     ROSE_ASSERT (proto != full_decl);

  // Restore the definition.
     tmp->set_definition (def);
     def->set_parent (tmp);

     proto->set_forward (true);
     proto->set_definingDeclaration (tmp);
#else
// DQ (2/23/2009): Use this code instead.
     SgFunctionDeclaration* proto = SageBuilder::buildNondefiningFunctionDeclaration(full_decl,scope);
     ROSE_ASSERT(proto != NULL);

     if (Outliner::useNewFile == true)
        {
          ROSE_ASSERT(proto->get_definingDeclaration() == NULL);
        }

  // This should be the defining declaration (check it).
     ROSE_ASSERT(full_decl->get_definition() != NULL);

  // printf ("full_decl                            = %p = %s \n",full_decl,full_decl->class_name().c_str());
  // printf ("full_decl->get_definingDeclaration() = %p = %s \n",full_decl->get_definingDeclaration(),full_decl->get_definingDeclaration()->class_name().c_str());
     ROSE_ASSERT(full_decl->get_definingDeclaration() == full_decl);

  // DQ (2/23/2009): This will result in a cross file edge if we have outlined to a separate file.
  // SgFunctionDeclaration* tmp = const_cast<SgFunctionDeclaration *> (full_decl);
  // proto->set_definingDeclaration(tmp);
  // ROSE_ASSERT(proto->get_definingDeclaration() != NULL);
  // ROSE_ASSERT(proto->get_definingDeclaration() == NULL);
#endif

  // printf ("In generateFriendPrototype(): proto->get_definingDeclaration() = %p \n",proto->get_definingDeclaration());
  // printf ("In generateFriendPrototype(): full_decl = %p returning SgFunctionDeclaration prototype = %p \n",full_decl,proto);

  // Make sure that internal referneces are to the same file (else the symbol table information will not be consistant).
     ROSE_ASSERT(proto != NULL);
     ROSE_ASSERT(proto->get_firstNondefiningDeclaration() != NULL);

  // Note that the function prototype has not been inserted into the AST, so it does not have a path to SgSourceFile.
  // ROSE_ASSERT(TransformationSupport::getSourceFile(proto) == NULL);
     ROSE_ASSERT(TransformationSupport::getSourceFile(proto) != NULL);
     ROSE_ASSERT(TransformationSupport::getSourceFile(scope) != NULL);

     ROSE_ASSERT(TransformationSupport::getSourceFile(proto->get_firstNondefiningDeclaration()) != NULL);
  // printf ("TransformationSupport::getSourceFile(proto->get_firstNondefiningDeclaration())->getFileName() = %s \n",TransformationSupport::getSourceFile(proto->get_firstNondefiningDeclaration())->getFileName().c_str());

     ROSE_ASSERT(TransformationSupport::getSourceFile(scope) == TransformationSupport::getSourceFile(proto->get_firstNondefiningDeclaration()));
     ROSE_ASSERT(TransformationSupport::getSourceFile(proto->get_scope()) == TransformationSupport::getSourceFile(proto->get_firstNondefiningDeclaration()));

     return proto;
   }

//! Generates a 'friend' declaration from a given function declaration.
//For a friend declaration, two scopes are involved.
//'scope' is the class definition within which the friend declaration is inserted. 
//'class_scope' is the class definition's SgClassDeclaration's scope in which the function symbol should be created, if not exist.
static
SgFunctionDeclaration *
generateFriendPrototype (const SgFunctionDeclaration* full_decl, SgScopeStatement* scope, SgScopeStatement* class_scope)
   {
     ROSE_ASSERT (class_scope != NULL);
     SgFunctionDeclaration* proto = generatePrototype (full_decl,scope);
     ROSE_ASSERT (proto != NULL);

  // Remove any 'extern' modifiers
     proto->get_declarationModifier ().get_storageModifier ().reset ();

  // Set the 'friend' modifier
     proto->get_declarationModifier ().setFriend ();

  // DQ (2/26/2009): Remove the SgFunctionSymbol since this is a "friend" function.
  // Since this is a friend we don't want to have the SgFunctionSymbol in "scope" 
  // so remove the symbol.  The SageBuilder function generated the SgFunctionSymbol
  // and at this point we need to remove it.  Friend function don't have symbols
  // in the class scope where they may appear as a declaration.  The SageBuilder
  // function could be provided a parameter to indicate that a friend function is
  // required, this would then suppress the construction of the symbol in the scope's
  // symbol table (and the scope of the function is not the same as the class scope 
  // in this case as well.
     SgFunctionSymbol* friendFunctionSymbol = isSgFunctionSymbol(scope->lookup_symbol(full_decl->get_name()));
     ROSE_ASSERT(friendFunctionSymbol != NULL);
  // printf ("@@@@@@@@@@@@ In generateFriendPrototype(): removing SgFunctionSymbol = %p with friendFunctionSymbol->get_declaration() = %p \n",friendFunctionSymbol,friendFunctionSymbol->get_declaration());
     scope->remove_symbol(friendFunctionSymbol);

#if 0
    // Liao, 10/26/2009, patch up the symbol in the class's scope, if does not exist
    SgFunctionSymbol* symbol2 = isSgFunctionSymbol(class_scope->lookup_symbol(full_decl->get_name())); 
    if (symbol2 == NULL)
      class_scope->insert_symbol(full_decl->get_name(), isSgSymbol(friendFunctionSymbol));
    else
#endif      
    {
      delete friendFunctionSymbol;
      friendFunctionSymbol = NULL;
    }

  // printf ("In generatePrototype(): Returning SgFunctionDeclaration prototype = %p \n",proto);

  // ROSE_ASSERT(copyDeclarationStatement->get_firstNondefiningDeclaration()->get_definingDeclaration() != NULL);

     ROSE_ASSERT(proto->get_definingDeclaration() == NULL);
  // proto->set_definingDeclaration(full_decl);

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
    
    SgDeclarationStatement* insert_point = findClosestGlobalInsertPoint (target);
    ROSE_ASSERT (insert_point);

    ASTtools::moveBeforePreprocInfo (insert_point, proto);
#if 1
 // ROSE_ASSERT(insert_point->get_scope() == scope);
    ROSE_ASSERT(find(scope->getDeclarationList().begin(),scope->getDeclarationList().end(),insert_point) != scope->getDeclarationList().end());

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
                       FuncDeclList_t & friendFunctionPrototypeList,
                       SgGlobal* scope,
                       SgDeclarationStatement* default_target) // The enclosing function for the outlining target
{
  SgFunctionDeclaration* prototype = NULL;


  if (def && scope)
  {
 // DQ (3/3/2009): Why does this code use try .. catch blocks (exception handling)?
    try
    {
      GlobalProtoInserter ins (def, scope);
      ins.traverse (scope, preorder);
      prototype = ins.getProto();
    }
    catch (string & s) { ROSE_ASSERT (s == "done"); }

    if (!prototype && default_target) // No declaration found
       {

      //Liao, 5/19/2009
      //The prototype has to be inserted to the very first class having a friend declaration to the outlined function
      //to avoid conflicting type info. for extern "C" functions.
      //The reason is that there is no way to use friend and extern "C" together within a class.
            if (friendFunctionPrototypeList.size()!=0)
      {
        vector<SgDeclarationStatement*> origFriends;
        for (FuncDeclList_t::iterator i=friendFunctionPrototypeList.begin(); i!=friendFunctionPrototypeList.end(); i++) 
        {
          SgDeclarationStatement* decl = isSgDeclarationStatement(*i);
          ROSE_ASSERT(decl!=NULL);
          origFriends.push_back(decl);
        }
        vector<SgDeclarationStatement*> sortedFriends = SageInterface::sortSgNodeListBasedOnAppearanceOrderInSource(origFriends); 
        prototype = GlobalProtoInserter::insertManually (def,scope,sortedFriends[0]);
      }
      else
         prototype = GlobalProtoInserter::insertManually (def,scope,default_target);
      // printf ("In insertGlobalPrototype(): Calling GlobalProtoInserter::insertManually(): prototype = %p = %s \n",prototype,prototype->class_name().c_str());
         if (Outliner::useNewFile == true)
            {
              ROSE_ASSERT(prototype->get_definingDeclaration() == NULL);
            }
       }
  }

      // The friend function declarations are linked to the global declarations via first non-defining declaration links.
  // Fix-up remaining prototypes.
     if (prototype != NULL)
        {
       // printf ("In insertGlobalPrototype(): proto = %p protos.size() = %zu \n",prototype,friendFunctionPrototypeList.size());
          for (FuncDeclList_t::iterator i = friendFunctionPrototypeList.begin (); i != friendFunctionPrototypeList.end (); ++i)
             {
               SgFunctionDeclaration* proto_i = *i;
               ROSE_ASSERT (proto_i);
               proto_i->set_firstNondefiningDeclaration (prototype);
               ROSE_ASSERT (proto_i->get_declaration_associated_with_symbol() != NULL);     

            // Only set the friend function prototype to reference the defining declaration
            // of we will NOT be moving the defining declaration to a separate file.
               if (Outliner::useNewFile == false)
                    proto_i->set_definingDeclaration(def);
             }

       // DQ (2/20/2009): Set the non-defining declaration
          if (def->get_firstNondefiningDeclaration() == NULL)
             {
               prototype->set_firstNondefiningDeclaration(prototype);
             }

       // DQ (2/20/2009): Added assertions.
          ROSE_ASSERT(prototype->get_parent() != NULL);
          ROSE_ASSERT(prototype->get_firstNondefiningDeclaration() != NULL);

       // DQ (2/23/2009): After change in generatePrototype() to use build functions this is now NULL (OK for now)
       // ROSE_ASSERT(proto->get_definingDeclaration() != NULL);
          if (Outliner::useNewFile == true)
             {
               ROSE_ASSERT(prototype->get_definingDeclaration() == NULL);
             }
        }

  // printf ("In insertGlobalPrototype(): Returning global SgFunctionDeclaration prototype = %p \n",prototype);

     return prototype;
   }

/*!
 *  \brief Given a 'friend' declaration, insert it into the given
 *  class definition.
 */
static
SgFunctionDeclaration *
insertFriendDecl (const SgFunctionDeclaration* func,
                  SgGlobal* scope,
                  SgClassDefinition* cls_def)
{
  SgFunctionDeclaration* friend_proto = 0;
  if (func && scope && cls_def)
    {
      // Determine insertion point, i.
      SgDeclarationStatementPtrList& mems = cls_def->get_members ();
      SgDeclarationStatementPtrList::iterator i = mems.begin ();

      // Create the friend declaration.
      friend_proto = generateFriendPrototype (func,cls_def, scope);
      ROSE_ASSERT (friend_proto != NULL);
      ROSE_ASSERT(friend_proto->get_definingDeclaration() == NULL);

      // Insert it into the class.
      if (i != mems.end ())
        ASTtools::moveBeforePreprocInfo ((*i), friend_proto);
      cls_def->get_members().insert(i, friend_proto);
      friend_proto->set_parent (cls_def);
      friend_proto->set_scope (scope);
    }

//  printf ("In insertFriendDecl(): Returning SgFunctionDeclaration prototype = %p \n",friend_proto);
//  We should not try to unparse the friend declaration here. Since its first-non definining declaration 
//  has not yet been inserted. So it has no declaration associated with a symbol
//  cout<<friend_proto->unparseToString()<<endl; 

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

  return NULL; // default: is not
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
 *  func: the generated outlined function
 */
static
void
insertFriendDecls (SgFunctionDeclaration* func,
                   SgGlobal* scope,
                   FuncDeclList_t& friends)
{
  if (func && scope)
    {
   // printf ("In insertFriendDecls(): friends list size = %zu \n",friends.size());

   // Collect a list of all classes that need a 'friend' decl.
   // The outlining target has accesses to those classes' private/protected members 
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
      NodeList_t func_refs = NodeQuery::querySubTree (func,V_SgMemberFunctionRefExp);
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
          ROSE_ASSERT (friend_decl != NULL);
#if 0
          printf ("+++++++++++++++++++ friend_decl = %p = %s \n",friend_decl,friend_decl->class_name().c_str());
#endif
       // DQ (2/23/2009): Added assertion.
          ROSE_ASSERT(friend_decl->get_definingDeclaration() == NULL);

          ROSE_ASSERT(friend_decl->get_scope() == scope);

       // DQ (2/27/2009): If we are outlining to a separate file, then we don't want to attach
       // a reference to the defining declaration which will be moved to the different file 
       // (violates file consistency rules that are not well enforced).
          ROSE_ASSERT(friend_decl->get_definingDeclaration() == NULL);
       // printf ("friend_decl = %p friend_decl->get_definingDeclaration() = %p \n",friend_decl,friend_decl->get_definingDeclaration());

          friends.push_back (friend_decl);
        }
    }
}


// =====================================================================
//! Insert func into scope (could be either original scope or the new scope from a new file), 
//  and insert necessary declarations into the global scope of
//  target's original enclosing function). 
void
Outliner::insert (SgFunctionDeclaration* func,
                             SgGlobal* scope,
                             SgBasicBlock* target_outlined_code )
   {
  // Scope is the global scope of the outlined location (could be in a separate file).
     ROSE_ASSERT (func != NULL && scope != NULL );
     ROSE_ASSERT(target_outlined_code != NULL);
     SgFunctionDeclaration* target_func = const_cast<SgFunctionDeclaration *> 
       (SageInterface::getEnclosingFunctionDeclaration (target_outlined_code));
     ROSE_ASSERT(target_func!= NULL);

  // This is the global scope of the original file
     SgGlobal* src_global = SageInterface::getGlobalScope(target_func);
     ROSE_ASSERT(src_global != NULL);

  // The scopes are the same only if this the outlining is NOT being output to a separate file.
     ROSE_ASSERT( (Outliner::useNewFile == true && scope != src_global) || (Outliner::useNewFile == false || scope == src_global) );

  // Make sure this is a defining function
     ROSE_ASSERT(func->get_definition() != NULL);
     ROSE_ASSERT(func->get_definingDeclaration() != NULL);
     ROSE_ASSERT(func->get_definingDeclaration() == func);

#if 0
     printf ("******************************************************************************************** \n");
     printf ("Outliner::insert(): input function func = %p func->get_definingDeclaration() = %p \n",func,func->get_definingDeclaration());
     printf ("******************************************************************************************** \n");
#endif

     ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());
      // The defining function was generated by SageBuilder function 
      // with the right target scope, so its symbol exists. 
      // But we will insert prototypes for C/C++ ( not for Fortran) later and   
      // the function symbol will be re-generated when the function prototypes are generated
      // So we need to remove the symbol for C/C++ and keep it for Fortran 
      // Liao, 3/11/2009
     if (SageInterface::is_Fortran_language() != true)
     {
       // The constructed defining declaration should have a symbol in its scope, remove it.
       SgFunctionSymbol* definingFunctionSymbol = isSgFunctionSymbol(scope->lookup_symbol(func->get_name()));
       ROSE_ASSERT(definingFunctionSymbol != NULL);
       scope->remove_symbol(definingFunctionSymbol);
       delete definingFunctionSymbol;
       definingFunctionSymbol = NULL;
       ROSE_ASSERT(scope->lookup_symbol(func->get_name()) == NULL);
     }
  // Put the input function into the target scope
     scope->append_declaration (func);
     func->set_scope (scope);
     func->set_parent (scope);

  // Error checking...
     if (Outliner::useNewFile == false)
        {
          ROSE_ASSERT(func->get_scope() == src_global);
          ROSE_ASSERT(func->get_scope() == scope);
          ROSE_ASSERT(scope == src_global);
        }
       else
        {
          ROSE_ASSERT(scope != src_global);
          ROSE_ASSERT(func->get_scope() == scope);
          ROSE_ASSERT(func->get_scope() != src_global);
        }
    // no need to build nondefining function prototype for Fortran, Liao, 3/11/2009    
    if (SageInterface::is_Fortran_language() == true)
      return;
#if 0
     printf ("************************************************************ \n");
     printf ("Building the outline function prototype in the ORIGINAL file \n");
     printf ("************************************************************ \n");
#endif
     
  // I don't understand what this is (appears to be a list of oulined function prototypes (non-defining declarations)).
  // It is used by both the insertGlobalPrototype() and 
     FuncDeclList_t friendFunctionPrototypeList;

//     if (SageInterface::is_Fortran_language() == false)
//        {
#if 1
       // Insert all necessary 'friend' declarations. This step will not build symbols for the symbol
       // table (although the build functions will they are removed in the insertFriendDecls() function).
          insertFriendDecls (func, src_global, friendFunctionPrototypeList);
#else
          printf ("Skipping the insertion of friend function declarations (testing only) \n");
#endif
//        }
   SgFunctionDeclaration* sourceFileFunctionPrototype = NULL;
   // insert a pointer to function declaration if use_dlopen is true
   // insert it into the original global scope
   if (use_dlopen) 
   {
    // void (*OUT_xxx__p) (void**);
     SgFunctionParameterTypeList * tlist = buildFunctionParameterTypeList();
     (tlist->get_arguments()).push_back(buildPointerType(buildPointerType(buildVoidType())));

     SgFunctionType *ftype = buildFunctionType(buildVoidType(), tlist);//func->get_type();
     string var_name = func->get_name().getString()+"p";
     SgVariableDeclaration * ptofunc = buildVariableDeclaration(var_name,buildPointerType(ftype), NULL, src_global);
     prependStatement(ptofunc,src_global);
   }
//   else 
//   Liao, 5/1/2009
//   We still generate the prototype even they are not needed if dlopen() is used. 
//   since SageInterface::appendStatementWithDependentDeclaration() depends on it
   {
     // This is done in the original file (does not effect the separate file if we outline the function there)
     // Insert a single, global prototype (i.e., a first non-defining
     // declaration), which specifies the linkage property of 'func'.
     // insertGlobalPrototype (func, protos, src_global, target_func);
     sourceFileFunctionPrototype = insertGlobalPrototype (func, friendFunctionPrototypeList, src_global, target_func);

     SgFunctionSymbol* sourceFileFunctionPrototypeSymbol = isSgFunctionSymbol(src_global->lookup_symbol(func->get_name()));
     ROSE_ASSERT(sourceFileFunctionPrototypeSymbol != NULL);
     ROSE_ASSERT(sourceFileFunctionPrototypeSymbol->get_declaration() == sourceFileFunctionPrototype);
     ROSE_ASSERT(sourceFileFunctionPrototype->get_firstNondefiningDeclaration() == sourceFileFunctionPrototype);
     // DQ (2/27/2009): Assert this as a test!
     ROSE_ASSERT(sourceFileFunctionPrototype->get_definingDeclaration() == NULL);
   }
  // This is the outlined function prototype that is put into the separate file (when outlining is done to a separate file).
     SgFunctionDeclaration* outlinedFileFunctionPrototype = NULL;
     if (Outliner::useNewFile == true)
        {
#if 0
          printf ("************************************************************ \n");
          printf ("Building the outline function prototype in the SEPARATE file \n");
          printf ("************************************************************ \n");

          printf ("Before: Number of symbols in scope = %p symbol table = %d \n",scope,scope->get_symbol_table()->size());
          printf ("Output the symbol table: \n");
          scope->get_symbol_table()->print("Building the outline function prototype in the SEPARATE file");
#endif

       // Build a function prototype and insert it first (will be at the top of the generated file).
          outlinedFileFunctionPrototype = SageBuilder::buildNondefiningFunctionDeclaration (func,scope);
       // scope->append_declaration (outlinedFileFunctionPrototype);
          scope->prepend_declaration (outlinedFileFunctionPrototype);

#if 0
          printf ("After: Number of symbols in scope = %p symbol table = %d \n",scope,scope->get_symbol_table()->size());
          printf ("In Outliner::insert(): outlinedFileFunctionPrototype = %p \n",outlinedFileFunctionPrototype);
#endif

       // The build function should have build symbol for the symbol table.
          SgFunctionSymbol* outlinedFileFunctionPrototypeSymbol = isSgFunctionSymbol(scope->lookup_symbol(func->get_name()));
          ROSE_ASSERT(outlinedFileFunctionPrototypeSymbol != NULL);
          ROSE_ASSERT(outlinedFileFunctionPrototypeSymbol->get_declaration() == outlinedFileFunctionPrototype);

       // DQ (2/27/2009): Assert this as a test!
          ROSE_ASSERT(outlinedFileFunctionPrototype->get_definingDeclaration() == NULL);

       // DQ (2/20/2009): ASK LIAO: If func is a defining declaration then shouldn't the 
       // SageBuilder::buildNondefiningFunctionDeclaration() set the definingDeclaration?
          outlinedFileFunctionPrototype->set_definingDeclaration(func);
          outlinedFileFunctionPrototype->set_parent(scope);
          outlinedFileFunctionPrototype->set_scope(scope);

       // Set the func_prototype as the first non-defining declaration.
          func->set_firstNondefiningDeclaration(outlinedFileFunctionPrototype);

          ROSE_ASSERT(outlinedFileFunctionPrototype->get_parent() != NULL);
          ROSE_ASSERT(outlinedFileFunctionPrototype->get_firstNondefiningDeclaration() != NULL);
          ROSE_ASSERT(outlinedFileFunctionPrototype->get_definingDeclaration() != NULL);

       // Since the outlined function has been moved to a new file we can't have a pointer to the defining declaration.
      //  if (!use_dlopen) // no source function prototype if dlopen is used
          ROSE_ASSERT(sourceFileFunctionPrototype->get_definingDeclaration() == NULL);

       // Add a message to the top of the outlined function that has been added
          SageInterface::addMessageStatement(outlinedFileFunctionPrototype,"/* OUTLINED FUNCTION PROTOTYPE */");

       // Make sure that internal referneces are to the same file (else the symbol table information will not be consistant).
          ROSE_ASSERT(func->get_firstNondefiningDeclaration() != NULL);
          ROSE_ASSERT(TransformationSupport::getSourceFile(func) == TransformationSupport::getSourceFile(func->get_firstNondefiningDeclaration()));
          ROSE_ASSERT(TransformationSupport::getSourceFile(func->get_scope()) == TransformationSupport::getSourceFile(func->get_firstNondefiningDeclaration()));
        }
       else
       {
         //if (!use_dlopen)
         {
           // Since the outlined function has been kept in the same file we can have a pointer to the defining declaration.
           sourceFileFunctionPrototype->set_definingDeclaration(func);
           ROSE_ASSERT(sourceFileFunctionPrototype->get_definingDeclaration() != NULL);
         }
       }

     ROSE_ASSERT(func->get_definition()->get_body()->get_parent() == func->get_definition());
     // No forward declaration is needed for Fortran functions, Liao, 3/11/2009
     if (SageInterface::is_Fortran_language() != true)
       ROSE_ASSERT(func->get_firstNondefiningDeclaration() != NULL);
   }


// eof
