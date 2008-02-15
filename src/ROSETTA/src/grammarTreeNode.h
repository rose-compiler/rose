#ifndef __GRAMMARTREENODE_H__
#define __GRAMMARTREENODE_H__

// #include <list.h>
// #include <list>
// #ifndef STL_LIST_IS_BROKEN
// #include STL_LIST_HEADER_FILE
// #endif

#include "ROSETTA_macros.h"
#include <string>

class Terminal;  // forward declaration
class NonTerminal;

class GrammarTreeNode
{
 public:

  Terminal* token;
  GrammarTreeNode*  parentTreeNode;
  std::list<GrammarTreeNode *> nodeList;

  ~GrammarTreeNode();
  GrammarTreeNode();
  GrammarTreeNode( Terminal & X );
  GrammarTreeNode( NonTerminal & X );
  GrammarTreeNode( const GrammarTreeNode & X );
  GrammarTreeNode & operator= ( const GrammarTreeNode & X );

  const std::string& getName() const;
  std::string getParentName() const;
  GrammarTreeNode & getParent() const;

  // static int isSameName ( const char* s1, const char* s2 );
  bool isLeafNode() const;
  bool isRootNode() const;

  // MS: 2002: these simple functions simplify the code generation and doc generation
  bool isTerminal() const;
  bool isNonTerminal() const;

  // Some treeNodes are associated with elements of grammar which are 
  // built from other grammar elements using the constraint mechanism.
  // They are considered children of the original grammar elements 
  // (terminals and nonterminals).  This function helps us recognise these
  // somewhat special sorts of terminals (since they don't contain parse 
  // member functions for example)
  bool isChildOfExistingGrammarElement() const;

  Terminal & getToken();
  bool hasParent() const;

  // void setIncludeInitializerInDataStrings ( bool X );
  // bool getIncludeInitializerInDataStrings ();

  void consistencyCheck() const;
 private:
  bool representsTerminal;
};

#endif
