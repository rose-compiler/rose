/* -*- C++ -*-
  Copyright 2009 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#ifndef _AST_JANITOR_H
#define _AST_JANITOR_H
#include "satire_rose.h"

/**
 * Perform various tasks to generate a valid AST
 * including the setting of parent scopes an pointers
 */
class InheritedAttribute
{
public:
  SgScopeStatement* scope;
  SgNode* parent;
  InheritedAttribute() : scope(NULL), parent(NULL) {};
  // Specific constructors are required
  InheritedAttribute(SgScopeStatement* s,  SgNode* p)
    :scope(s), parent(p) 
  {};
  InheritedAttribute(const InheritedAttribute& X)
    :scope(X.scope), parent(X.parent) 
  {};
};

class AstJanitor : public AstTopDownProcessing<InheritedAttribute>
{
public:
  //virtual function must be defined
  virtual InheritedAttribute 
  evaluateInheritedAttribute(SgNode *n, InheritedAttribute attr)
  {
    // FileInfo
    if (SgLocatedNode* ln = isSgLocatedNode(n)) {
      // Set the CompilerGenerated Flag
      //Sg_File_Info* fi = ln->get_file_info();
      //fi->set_classificationBitField(fi->get_classificationBitField() 
      //				   | Sg_File_Info::e_compiler_generated 
      /*| Sg_File_Info::e_output_in_code_generation*/
      //);
      // Set EndOfConstruct
      ln->set_endOfConstruct(ln->get_startOfConstruct());
    }

    // Scope
    SgScopeStatement* scope = isSgScopeStatement(n);
    if (scope == NULL) scope = attr.scope;
    //    else {
      // rebuild the symbol table
      /// scope->set_symbol_table(NULL);
      //  SageInterface::rebuildSymbolTable(scope);
      //}

    if (SgDeclarationStatement* decl = isSgDeclarationStatement(n)) {
      // These nodes don't have a scope associated
      if (!isSgVariableDeclaration(decl) 
	  && !isSgFunctionParameterList(decl)
	  ) {
	ROSE_ASSERT(scope != NULL);
	decl->set_scope(scope);
      } 
      PrologToRose::addSymbol(scope, decl);
    }

    // Parent
    if ((isSgInitializedName(n) && isSgEnumDeclaration(attr.parent))
	|| (isSgVariableDeclaration(n) && isSgForInitStatement(attr.parent))
	){
      n->set_parent(attr.parent->get_parent());
    } else {
      ROSE_ASSERT(n->get_parent() == attr.parent);
      //n->set_parent(attr.parent);
    }

    return InheritedAttribute(scope, n);
  };
};


#endif
