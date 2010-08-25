#ifndef Slicing_IS_DEFINED
#define Slicing_IS_DEFINED


#include "SlicingCriterion.h"
#include "FindStatements.h"
#include "MarkingNodes.h"
#include "RemovalOfNodes.h"
#include "CheckWhichAreMarked.h"

//#define DEBUG_SLICING
/*!
\class Slicing
 This class controls the different parts of the slicing process: From the input of a SgProject object to the removal of nodes.

The slicing is divided into the following steps:
-# Identifying the slicing criterion, that is the statements we are doing slicing on. These statements are placed between pragma declarations.
\sa SlicingCriterion

-# Identify the function definition is which the slicing cirterion is. These two first steps are done in the function "init". \sa init.

-# Finding the statements which affect the slicing criterion, using the Definition Use Chain. This is done in the function "onlyStmts" which uses the class FindStatements.
\sa onlyStmts, FindStatements

-# Marking nodes in the AST is done in a traversal "MarkingNodes". If a node is in the set of statements necessary for the slice, then we mark the node with a AST attribute. At the same time we collect statements from the control statements which we might need to do a slicing on. Then we have to do the previous step one more time for these statements and then this traversal of marking as well. This is done in the function "addControlStmts". We also collect the functions that are called within the slice. Later the functions that are not needed for the slice are removed. This last part is done in the function "keepNeededFunc".
\sa MarkingNodes, addControlStmts, keepNeededFunc.

-# When we have found all statements necessary for the slice and also marked the nodes in the AST, then we are ready to remove the AST nodes which are not marked to be kept, and thus not needed for the slice.
\sa RemovalOfNodes


*/

class Slicing{

 public:
  
  /*!
    \brief Interface 1:
    Performs a complete slice, that is slices the input file and produces a compilable output file. 
  */
  static void completeSlice(SgProject* sgproject);
  /*! 
    \brief
    Interface 2: This function performs the same slicing as sliceOnlyStmts, however in addition this function includes statements connected to the control structure as well as return statements.
  */
  static void sliceOnlyStmtWithControl(SgProject* sgproject, set<SgNode*>& stmt);

  /*! 
    \brief
    Interface 3:
    This function finds only the statements that directly affect the slicing criterion. This function
    gives the same statements as the definition use associations gives. The protected function "onlyStmts" does the actual work, while this function is supposed to be the public one, which is called.
  */
  static void sliceOnlyStmts(SgProject* sgproject,set<SgNode*>&  stmt_in_slice);
 protected:

  /*!
 \brief
    This function initiates the slicing process by finding the slicing criterion and the function definition in which the slicing criterion is, from the SgProject.
  */

  static void init(SgProject* sgproject,
                   set<SgStatement*>& slicing_criterion, SgFunctionDefinition* &func_defn);

  /*!
\brief
    This function finds only the statements that directly affect the slicing criterion. This function gives the
    same statements as the definition use associations gives.
  */
  static void onlyStmts(set<SgStatement*> slicing_criterion, SgFunctionDefinition* func_defn,
			set<SgNode*>& stmt_in_slice);
 
  /*!
\brief
    This function, called after a call to "onlyStmts", adds the control structure around the statements from the call on "onlyStmts".
  */
 static void addControlStmts(SgProject* sgproject, MarkingNodes& v, SgFunctionDefinition* func_defn, 
			     set<SgNode*>& stmt_in_slice);
  

 /*!
\brief
This function finds all needed functions and "unmarks" (that is remove the keep attribute from) the AST function declaration nodes that are not needed for the slice. 
  */
 static void keepNeededFunc(SgProject* sgproject, MarkingNodes v, SgFunctionDefinition* func_defn);

 //  static set<SgNode*> return_stmt_in_slice;
 // static set<SgNode*> return_stmt_in_slice_wo_controlstmts; 
  
  /*!
\brief
    This function finds the statements between the pragma declarations. This is done using a traversal starting in the node "root" and collecting all the statements that are between the pragmas in the return variable " set<SgStatement*> stmts".
  */
  static void FindSlicingCriterion(SgGlobal* root, set<SgStatement*>& stmts);

  /*! ...
   */
  //static void SliceWithControlStatements(SgProject* sgproject,
  //					 set<SgNode*> slice, SgFunctionDefinition* defn);


  /*!
\brief
    Useful debugging function that writes the statements of a set to screen.
  */
  static void writeStatements(set<SgStatement*> stmts, string heading);
  
  /*!
\brief
    Useful debugging function that writes the nodes of a set to screen.
  */
  static void writeNodes(set<SgNode*> stmts, string heading);


  /*!
    \brief
    This function returns a boolean value according to whether a SgNode is in a list of SgFunctionDeclarations. This function is used for including functions that are called in the slice, and to not include functions not called.
  */
  static bool isInList(SgNode* node, list<SgFunctionDeclaration*> funclist);
};

#endif
