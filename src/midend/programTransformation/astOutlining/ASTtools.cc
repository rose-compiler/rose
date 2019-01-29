/**
 *  \file ASTtools.hh
 *
 *  \brief Higher-level AST manipulation support routines.
 *
 *  \author Chunhua Liao <liaoch@cs.uh.edu>, Richard Vuduc
 *  <richie@llnl.gov>
 *
 *  This implementation was extracted by Liao from his ROSE
 *  OpenMP_Translator project.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <iostream>
#include <sstream>
#include <list>
#include <algorithm>


#include "ASTtools.hh"

// =====================================================================

using namespace std;

// =====================================================================

bool
ASTtools::isConstMemFunc (const SgFunctionDeclaration* decl)
{
  const SgMemberFunctionDeclaration* mf_decl =
    isSgMemberFunctionDeclaration (decl);
  if (mf_decl)
    {
      const SgMemberFunctionType* func_type =
        isSgMemberFunctionType (mf_decl->get_type ());
      if (func_type)
        return func_type->isConstFunc ();
    }
  return false; // Default: assume it isn't a const member function.
}

bool
ASTtools::isConstMemFunc (const SgFunctionDefinition* def)
{
  if (def)
    {
      SgFunctionDeclaration* decl = def->get_declaration ();
      return isConstMemFunc (decl);
    }
  return false; // Default: assume it isn't a const member function.
}

bool
ASTtools::isConstObj (const SgType* type)
{
  if (!type) return false;
  switch (type->variantT ())
    {
    case V_SgModifierType:
      {
        const SgModifierType* mod = isSgModifierType (type);
        return mod->get_typeModifier ().get_constVolatileModifier ().isConst ();
      }
      break;
    case V_SgPointerType:
      {
        const SgPointerType* p = isSgPointerType (type);
        return isConstObj (isSgModifierType (p->get_base_type ()));
      }
    case V_SgReferenceType:
      {
        const SgReferenceType* r = isSgReferenceType (type);
        return isConstObj (isSgModifierType (r->get_base_type ()));
      }
    case V_SgArrayType:
      {
        const SgArrayType* a = isSgArrayType (type);
        return isConstObj (isSgModifierType (a->get_base_type ()));
      }
    default:
      break;
    }
  // Default: Assume it's not
  return false;
}

// ========================================================================
#if 0
const SgClassDefinition *
ASTtools::getEnclosingClassDef (const SgNode* n)
{
  if (!n)
    return 0; // not found

  switch (n->variantT ())
    {
    case V_SgGlobal:
      return 0; // not found
    case V_SgClassDefinition:
      return isSgClassDefinition (n); // found
    default:
      break;
    }

  // Recursively search parents.
  return getEnclosingClassDef (n->get_parent ());
}
#endif
// ========================================================================
#if 0
/*!
 *  \brief
 *
 *  \returns The next statement, or NULL if none could be found.
 */
const SgStatement *
ASTtools::findNextStatement (const SgStatement* s)
{
  const SgStatement* s_next = NULL;
  if (s)
    {
      const SgScopeStatement* scope_s = s->get_scope ();
      ROSE_ASSERT (scope_s);
      const SgStatementPtrList& statementList =
        const_cast<SgScopeStatement *> (scope_s)->getStatementList ();

      SgStatementPtrList::const_iterator i =
        find (statementList.begin (), statementList.end (), s);
      ROSE_ASSERT (i != statementList.end ());
      ++i;
      if (i != statementList.end ())
        s_next = *i;
    }
  return s_next;
}
#endif
// ========================================================================

/*!
 *  \brief
 *  \returns A scope statement, or NULL if not found.
 */
const SgScopeStatement *
ASTtools::findFirstFuncDefScope (const SgStatement* s)
{
  if (s)
    {
      const SgScopeStatement* scope = s->get_scope ();
      if (scope)
        {
          switch (scope->variantT ())
            {
              // The following may contain a function definition:
            case V_SgGlobal:
            case V_SgClassDefinition:
            case V_SgNamespaceDefinitionStatement:
              return scope;

              // Otherwise, keep looking.
            default:
              return findFirstFuncDefScope (scope);
            }
        }
    }
  return NULL; // Can't find?
}

const SgFunctionDefinition *
ASTtools::findFirstFuncDef (const SgStatement* s)
{
  if (s)
    {
      const SgScopeStatement* scope = s->get_scope ();
      const SgFunctionDefinition* def = isSgFunctionDefinition (scope);
      if (def)
        return def;
      else
        return findFirstFuncDef (scope);
    }
  return 0; // Can't find?
}

// ========================================================================

bool
ASTtools::isSwitchCond (const SgStatement* s)
{
  if (s)
    {
      const SgSwitchStatement* s_switch =
        isSgSwitchStatement (s->get_parent ());
      return s_switch && s_switch->get_item_selector () == s;
    }
  return false;
}

bool
ASTtools::isIfCond (const SgStatement* s)
{
  if (s)
    {
      const SgIfStmt* s_if = isSgIfStmt (s->get_parent ());
      return s_if && s_if->get_conditional () == s;
    }
  return false;
}

bool
ASTtools::isWhileCond (const SgStatement* s)
{
  if (s)
    {
      const SgNode* s_par = s->get_parent ();
      if (isSgWhileStmt (s_par))
        {
          const SgWhileStmt* s_while = isSgWhileStmt (s_par);
          ROSE_ASSERT (s_while);
          return s_while && s_while->get_condition () == s;
        }
      else if (isSgDoWhileStmt (s_par))
        {
          const SgDoWhileStmt* s_while = isSgDoWhileStmt (s_par);
          ROSE_ASSERT (s_while);
          return s_while && s_while->get_condition () == s;
        }
    }
  return false;
}

// ========================================================================

bool
ASTtools::isC99 (const SgNode* n)
{
  if (n)
    switch (n->variantT ())
      {
      case V_SgProject:
        {
          const SgProject* proj = isSgProject (n);
          ROSE_ASSERT (proj);
          return proj->get_C99_only ();
        }
        break;
   // case V_SgFile:
      case V_SgSourceFile:
      case V_SgBinaryComposite:
        {
          const SgFile* file = isSgFile (n);
          ROSE_ASSERT (file);
          return file->get_C99_only ();
        }
        break;
      default:
        return ASTtools::isC99 (n->get_parent ());
      }
  return false;
}

// ========================================================================

bool
ASTtools::isStdNamespace (const SgScopeStatement* scope)
{
  const SgNamespaceDefinitionStatement* ns =
    isSgNamespaceDefinitionStatement (scope);
  if (ns && isSgGlobal (ns->get_scope ()))
    {
      const SgNamespaceDeclarationStatement* decl =
        ns->get_namespaceDeclaration ();
      ROSE_ASSERT (decl);
      return decl->get_name ().getString () == "std";
    }
  // Default: No.
  return false;
}

// ========================================================================

bool
ASTtools::isMain (const SgFunctionDeclaration* func)
{
  return isFuncName (func, "main") && isSgGlobal (func->get_scope ());
}

// ========================================================================

bool
ASTtools::isTemplateInst (const SgDeclarationStatement* decl)
{
  return isSgTemplateInstantiationDecl (decl)
    || isSgTemplateInstantiationFunctionDecl (decl)
    || isSgTemplateInstantiationMemberFunctionDecl (decl);
}

bool
ASTtools::isTemplateInst (const SgScopeStatement* scope)
{
  if (isSgTemplateInstantiationDefn (scope))
    return true;

  const SgFunctionDefinition* func_def = isSgFunctionDefinition (scope);
  if (func_def && isTemplateInst (func_def->get_declaration ()))
    return true;

  return false;
}

// ========================================================================

bool
ASTtools::isExternC (const SgFunctionDeclaration* func)
{
  return func
    && const_cast<SgFunctionDeclaration *> (func)->get_declarationModifier ().get_storageModifier ().isExtern ()
    && func->get_linkage() == string ("C");
}

// ========================================================================

bool
ASTtools::isFuncName (const SgFunctionDeclaration* func,
            const string& name)
{
  return func && func->get_name ().getString () == name;
}

bool
ASTtools::isFuncNamePrefix (const SgFunctionDeclaration* func,
                  const string& name)
{
  if (func)
    return func->get_name ().getString ().find (name) == 0;
  return false;
}

// ========================================================================

size_t
ASTtools::getNumArgs (const SgFunctionDeclaration* func)
{
  if (func)
    {
      if (isSgMemberFunctionDeclaration (func) && !isStaticFunc (func))
        return func->get_args ().size () + 1;
      else
        return func->get_args ().size ();
    }
  // Default: Assume none.
  return 0;
}

// ========================================================================

bool
ASTtools::isCtor (const SgFunctionDeclaration* func)
{
  const SgMemberFunctionDeclaration* mem_func =
    isSgMemberFunctionDeclaration (func);
  return mem_func
    && getMemFuncName (mem_func) == getClassName (mem_func);
}

bool
ASTtools::isDtor (const SgFunctionDeclaration* func)
{
  const SgMemberFunctionDeclaration* mem_func =
    isSgMemberFunctionDeclaration (func);
  return mem_func
    && getMemFuncName (mem_func) == ("~" + getClassName (mem_func));
}

// ========================================================================

bool
ASTtools::isStaticFunc (const SgFunctionDeclaration* func)
{
  return func
    && const_cast<SgFunctionDeclaration *> (func)->get_declarationModifier ().get_storageModifier ().isStatic ();
}

// ========================================================================

string
ASTtools::getUnqualUntmplName (const SgDeclarationStatement* d)
{
  if (isTemplateInst (d))
    {
      ROSE_ASSERT (d);
      string name;
      switch (d->variantT ())
        {
        case V_SgTemplateInstantiationDecl:
          {
            const SgTemplateInstantiationDecl* t =
              isSgTemplateInstantiationDecl (d);
            ROSE_ASSERT (t);
            name = t->get_templateName ();
          }
          break;
        case V_SgTemplateInstantiationFunctionDecl:
          {
            const SgTemplateInstantiationFunctionDecl* t =
              isSgTemplateInstantiationFunctionDecl (d);
            ROSE_ASSERT (t);
            name = t->get_templateName ();
          }
          break;
        case V_SgTemplateInstantiationMemberFunctionDecl:
          {
            const SgTemplateInstantiationMemberFunctionDecl* t =
              isSgTemplateInstantiationMemberFunctionDecl (d);
            ROSE_ASSERT (t);
            name = t->get_templateName ();
          }
          break;
        default:
          ROSE_ASSERT (!"*** Unhandled case in getUnqualUntmplName () ***");
          break;
        }
      return name;
    }
  else if (isSgClassDeclaration (d))
    return isSgClassDeclaration (d)->get_name ().getString ();
  else if (isSgFunctionDeclaration (d))
    return isSgFunctionDeclaration (d)->get_name ().getString ();
  return string (""); // No name found!
}

// ========================================================================

string
ASTtools::getClassName (const SgMemberFunctionDeclaration* mem_func)
{
  ROSE_ASSERT (mem_func);
  const SgScopeStatement* scope = mem_func->get_class_scope ();
  ROSE_ASSERT (scope);

  const SgClassDefinition* cdefn = isSgClassDefinition(scope);
  const SgDeclarationScope* nrscope = isSgDeclarationScope(scope);
  if (cdefn != NULL) {
    const SgClassDeclaration* cls_decl = cdefn->get_declaration ();
    ROSE_ASSERT (cls_decl);
    return cls_decl->get_name ();
  } else if (nrscope != NULL) {
    const SgNode* parent = nrscope->get_parent();
    ROSE_ASSERT (parent != NULL);
    const SgClassDeclaration * xdecl = isSgClassDeclaration(parent);
    const SgNonrealDecl * nrdecl = isSgNonrealDecl(parent);
    if (xdecl != NULL) {
      return xdecl->get_name ();
    } else if (nrdecl != NULL) {
      return nrdecl->get_name ();
    } else {
      ROSE_ASSERT(false);
    }
  } else {
    ROSE_ASSERT(false);
  }
}

string
ASTtools::getMemFuncName (const SgMemberFunctionDeclaration* mem_func)
{
  ROSE_ASSERT (mem_func);
  return mem_func->get_name ().getString ();
}

/* =====================================================================
 *  Debugging aids.
 */

void
ASTtools::dumpSymTab (const SgScopeStatement* scope, const string& tag,
                      ostream& o)
{
  if (scope)
    {
      char* c_tag = const_cast<char *> (tag.c_str ());
      ROSE_ASSERT (c_tag);
      const_cast<SgScopeStatement *> (scope)->print_symboltable (c_tag, o);
    }
}

string
ASTtools::toStringFileLoc (const SgLocatedNode* n)
{
  stringstream s;
  if (n)
    {
      string sep (" ");
      switch (n->variantT ())
        {
        case V_SgVarRefExp:
          {
            const SgVarRefExp* ref = isSgVarRefExp (n);
            ROSE_ASSERT (ref);
            const SgVariableSymbol* sym = ref->get_symbol ();
            ROSE_ASSERT (sym);
            s << "'" << sym->get_name ().getString () << '\'';
          }
          break;
        case V_SgFunctionRefExp:
          {
            const SgFunctionRefExp* ref = isSgFunctionRefExp (n);
            ROSE_ASSERT (ref);
            const SgFunctionSymbol* sym = ref->get_symbol ();
            ROSE_ASSERT (sym);
            s << "'" << sym->get_name ().getString () << '\'';
          }
        default:
          sep = "";
          break;
        }

      const Sg_File_Info* start = n->get_startOfConstruct ();
      s << sep
        << (const void *)n << ":<" << n->class_name () << ">"
        << " [" << start->get_raw_filename () << ':'
        << start->get_raw_line ();
      const Sg_File_Info* end = n->get_endOfConstruct ();
      if (end && end->get_raw_line () > start->get_raw_line ())
        s << "--" << end->get_raw_line ();
      s << "]";
    }
  return s.str ();
}

Sg_File_Info *
ASTtools::newFileInfo (void)
{
  return Sg_File_Info::generateDefaultFileInfoForTransformationNode();
}
//TODO  move to SageInterface when thoroughly tested, especially how to free previously allocated memory for file info nodes
void ASTtools::setSourcePositionAsTransformation(SgNode* node)
{
  ROSE_ASSERT(node != NULL);

  SgLocatedNode*     locatedNode = isSgLocatedNode(node);
  SgExpression*      expression  = isSgExpression(node);
  SgInitializedName* initName    = isSgInitializedName(node);
  SgPragma*          pragma      = isSgPragma(node); // missed this one!! Liao, 1/30/2008
  SgGlobal*          global      = isSgGlobal(node); // SgGlobal should have NULL endOfConstruct()

  if  (locatedNode != NULL)
  {
    locatedNode->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
    locatedNode->get_startOfConstruct()->set_parent(locatedNode);

    if (global==NULL)
    {  
      locatedNode->set_endOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
      locatedNode->get_endOfConstruct  ()->set_parent(locatedNode); 
    }

    // Only SgExpression IR nodes have a 3rd source position data structure.
    if (expression!=NULL)
    {
      expression->set_operatorPosition(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
      expression->get_operatorPosition()->set_parent(expression);
    }
  }
  else
  {
    if (initName != NULL) 
    {
      //  no endOfConstruct for SgInitializedName
      initName->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());   
      initName->get_startOfConstruct()->set_parent(initName);
    }
    else 
    {
      if (pragma != NULL) 
      {
        pragma->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
        pragma->get_startOfConstruct()->set_parent(pragma);
      }
    }
  }
}




void ASTtools::setSourcePositionAtRootAndAllChildrenAsTransformation(SgNode* root)
{
  Rose_STL_Container <SgNode*> nodeList= NodeQuery::querySubTree(root,V_SgNode);
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i!=nodeList.end(); i++ )
  {
   setSourcePositionAsTransformation(*i);   
  }
}

// eof
