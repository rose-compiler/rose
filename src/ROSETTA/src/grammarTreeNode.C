// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammarTreeNode.h"
#include "terminal.h"
#include "nonterminal.h"
#include "grammarString.h"
#include <stdlib.h>

using namespace std;

// ################################################################
// #               GrammarTreeNode Member Functions               #
// ################################################################

GrammarTreeNode::~GrammarTreeNode()
   {
     token = NULL;
     parentTreeNode = NULL;
   }

GrammarTreeNode::GrammarTreeNode()
   : token(NULL), parentTreeNode(NULL), representsTerminal(false)
   {
     token          = NULL;
     parentTreeNode = NULL;
   }

GrammarTreeNode::GrammarTreeNode ( Terminal & X )
   : token(NULL), parentTreeNode(NULL), representsTerminal(true)
   {
  // printf ("Building a GrammarTreeNode from a Terminal \n");

     ROSE_ASSERT(this  != NULL);
     token = &X;

  // delete this before we make the assignement (to avoid Insure++ warming)
     delete X.grammarSubTree;
     if (X.grammarSubTree != NULL)
        {
          printf ("In GrammarTreeNode::GrammarTreeNode(Terminal): X.grammarSubTree != NULL name = %s \n",X.getName().c_str());
	  ROSE_ABORT();
        }

     X.grammarSubTree = this;

     parentTreeNode = NULL;
     ROSE_ASSERT(token != NULL);
   }

GrammarTreeNode::GrammarTreeNode( NonTerminal & X )
   : token(NULL), parentTreeNode(NULL), representsTerminal(false)
   {
     //printf ("Building a GrammarTreeNode from a NonTerminal \n");

     ROSE_ASSERT(this  != NULL);
     token = &X;
     parentTreeNode = NULL;

  // delete this before we make the assignement (to avoid Insure++ warming)
     delete X.grammarSubTree;
     X.grammarSubTree = this;

     ROSE_ASSERT(token != NULL);
   }

GrammarTreeNode::GrammarTreeNode( const GrammarTreeNode & X )
   : token(NULL), parentTreeNode(NULL), representsTerminal(false)
   {
    *this = X;
   }

GrammarTreeNode & GrammarTreeNode::operator= ( const GrammarTreeNode & X )
   {
     token              = X.token;
     parentTreeNode     = X.parentTreeNode;
     representsTerminal = X.representsTerminal;

     printf ("Exiting within GrammarTreeNode::operator=() \n");
     ROSE_ABORT();

     return *this;
   }

const string&
GrammarTreeNode::getName() const
   {
     ROSE_ASSERT (this  != NULL);
     ROSE_ASSERT (token != NULL);
     return token->name;
   }

// MS:2002
bool
GrammarTreeNode::isTerminal() const {
  ROSE_ASSERT(token!=NULL);
  return representsTerminal;
}

// MS:2002
bool
GrammarTreeNode::isNonTerminal() const {
  return !isTerminal();
}

bool
GrammarTreeNode::isLeafNode() const
   {
     return (nodeList.size() == 0) ? TRUE : FALSE;
   }

bool
GrammarTreeNode::isRootNode() const
   {
     return (parentTreeNode == NULL) ? TRUE : FALSE;
   }


Terminal & GrammarTreeNode::getToken()
   {
     ROSE_ASSERT (token != NULL);
     return *token;
   }


string
GrammarTreeNode::getParentName() const
   {
     string parentClassName = "";
     if (parentTreeNode != NULL)
        {
          parentClassName = parentTreeNode->getName();
	}

     return parentClassName;
   }

bool
GrammarTreeNode::hasParent() const
   {
     return (parentTreeNode != NULL) ? TRUE : FALSE;
   }


GrammarTreeNode &
GrammarTreeNode::getParent() const
   {
  // Return the parent node from teh current node
     ROSE_ASSERT (parentTreeNode != NULL);
     return *parentTreeNode;
   }

#if 0
void
GrammarTreeNode::setIncludeInitializerInDataStrings ( bool X )
   {
  // Set the value on the terminal directly
     getToken().setIncludeInitializerInDataStrings(X);
   }

bool
GrammarTreeNode::getIncludeInitializerInDataStrings ()
   {
  // Set the value on the terminal directly
     return getToken().getIncludeInitializerInDataStrings();
   }
#endif


bool
GrammarTreeNode::isChildOfExistingGrammarElement() const
   {
  // Since a NonTerminal is derived from a Terminal we can call the base class function directly

     ROSE_ASSERT (token != NULL);
     return token->isChild();
   }

