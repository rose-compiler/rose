
#include <set>

#include "KeepAttribute.h"

#ifndef MARKALL_IS_DEFINED
#define MARKALL_IS_DEFINED
/*!
  This class traverses the AST with the given astNode as the root and marks all nodes in the subtree
  to be kept.
*/
//#include "MarkingNodes.h"
class MarkAll : public AstSimpleProcessing{
 public:
  virtual void visit(SgNode* astNode){
   if(!astNode->attribute.exists("keep")){ 
     astNode->attribute.add("keep", new KeepAttribute(true));
    } 
    // MarkingNodes::keepNode(astNode);  // would be nicer to reuse this func
  }
};
#endif

#ifndef MARKINGNODES_IS_DEFINED
#define MARKINGNODES_IS_DEFINED

/*!
  MarkingNodes is a subclass of AstTopDownBottomUpProcessing. In this traversal AST nodes are marked
  according to if the are to be or not to be in the slice: Each AST node is compared to the with the
  list of statements that are to be in the slice. If an AST node is found in the list of statements,
  then the subtree with this node as its root is marked with the attribute keep=true, and also a
  synthesized attribute is "sent" up in the tree, such that we mark all nodes "above" the current node
  with the attribute keep=true.
*/

class MarkingNodes : public AstTopDownBottomUpProcessing<bool, bool> {

  // private:
 protected:
  set<SgNode*> stmtlist;   //! The input list of statements that we are to have in our slice
  set<SgNode*> newStmtlist; //! Output list of statements that need to be included
  set<SgNode*> allreturnedstmts;
  SgFunctionDefinition* sliceThisFunc;
  list<SgFunctionDeclaration*> funclist;

 protected:
  /*!
    This function marks all nodes in the subtree with node as the root to be kept.
  */ 
  void keepAllNodes(SgNode* node);
  
  /*!
    This function adds the AstAttribute keep to the node and sets the value of the attribute to be true.
  */ 
  static void keepNode(SgNode* node);
  
  
  /*!
    This function inserts the node in the list which we return in get_newStatementList().
  */
  void insertInList(SgNode* node);
  
  /*!
    This function checks if the node is already in two sets: 1) the set of statements returning from the
    current travesal 2) the set of all statements ever returned from a traversal.
    The latter set is to avoid to include a statement that is already known to be kept, and thus avoid
    never-ending loops in the function FindStatementsForSlice().
    
    \param node - the node which is to be tested if it is in the set of new statements we are finding
    from the control structures and if it is in the set of all statements found for the slice ever found. 
    
    \return Returns true if is already in any of the sets, otherwise false is retured.
  */
  bool isAlreadyInSet(SgNode* node);
  
  /*! A variable that tells us if we are before the first pragma declaration in the code. */ 
  bool beforePragma;
  
 public:
  /*!
    This function sets the statement list. It also clears the list containing the "new"(i.e. the
    statements we collect in this traversal) statements, as well as the bool beforePragma to be true
    as a default.
  */
  void initialize(set<SgNode*> s, SgFunctionDefinition* func);
  
  /*!
    This function returns the list of new statements for which we need to find all statements that
    affect them.
  */
  set<SgNode*> get_newStatementList(){return newStmtlist;}

  /*!
    This function returns the list of function declarations which are to be in the slice. (Not complete
    from this marking traversal. We also need to find possible function calls within these functions recursively.)
  */
  list<SgFunctionDeclaration*> get_funclist(){return funclist;}
  
  
  /*!
    If the boolean keep is true, then this function checks that the SgNode node is declared/defined
    properly. This means the functions checks if the definition AST node of the variable which occurs
    at node has the AstAttribute "keep".
    
    If the boolean value keep is false, this SgNode is not to be included in the slice and thus does
    not need to be defined/declared properly.

    \param node - the node that is to be checked if is declared properly.
    \param keep - a boolean value that tells us whether this check is necessary or not.
  */  
  static void checkIfDeclared(SgNode* node, bool keep);

  
  
 protected:
  /*!
  This function implements the virtual function "evaluateInheritedAttribute" in the class AstTopDownBottomUpProcessing. In this function marking of statements which are to be kept for the slice is done. (more specific?)
  */
  bool virtual evaluateInheritedAttribute(SgNode* astNode, bool inherited);

 /*!
  This function implements the virtual function "evaluateSynthesizedAttribute" in the class AstTopDownBottomUpProcessing.
  */
  bool virtual evaluateSynthesizedAttribute(SgNode* astNode,
                                            bool inherited, vector<bool> synattri);

/*!
  This function implements the virtual function "defaultSynthesizedAttribute" in the class AstTopDownBottomUpProcessing. The default return value is "false", implying that the default answer to whether an AST node is to be kept for the slice or not, is not to keep.
  */
  bool defaultSynthesizedAttribute();
  
};

#endif

