#ifndef FINDSTATEMENTS_IS_DEFINED
#define FINDSTATEMENTS_IS_DEFINED


#include <AstInterface.h>
#include <StmtInfoCollect.h>
#include <ReachingDefinition.h>
#include <DefUseChain.h>

#include <iostream>
#include <iterator>
#include <list>
#include <set>

using namespace std;

//#define DEBUG_FINDSTATEMENTS

/*!
  \class FindDefUseChainNode
  
  This class traverses a subtree of the AST with an expression (not statement) as the root.
  The traversal collects all references of definition-use chain nodes to which the ast expression
  node corresponds to/appears in.
*/
class FindDefUseChainNode : public AstSimpleProcessing {
  
  map<SgNode*,  DefUseChainNode*> &refmap; // copy of the map from memory ref to def use chain nodes
  set<DefUseChainNode*> refs;
  
 public:
  FindDefUseChainNode( map<SgNode*,  DefUseChainNode*> &r) : refmap(r) {}

  virtual void visit( SgNode* cur){
    map<SgNode*,  DefUseChainNode*>::const_iterator p = refmap.find(cur);
    if ( p  != refmap.end()) {
      refs.insert((*p).second);
    } 
  }
  set<DefUseChainNode*> get_refs(){return refs;}
};

/*!
  \class FindStatements
  This class finds all the statements that affect the given statement in the slicing criterion
  (SgNode* slicing_criterion). These statements are found using Definition-Use associations. A special
  traversal for finding statements affecting expressions (not statements), is provided in the class
  FindDefUseChainNode. 
  \sa FindDefUseChainNode
*/
class FindStatements {
  
 public:
  /*!
    \brief This function returns the set "slice" with all statements that affect the statement
    "slicing_criterion". These statements are found using Definition Use assosications. "func_defn" is
    the function definition of the function where the statements "slicing_criterion" appears in.
    \param func_defn - the function definition from which the definition-use associations are built.
    \param slicing_criterion - the statement that we want to find all affecting statements to.
    \param slice - all the statements needed for the slice with repect to the slicing_criterion is
    stored here
    \param alias -
  */
  static void FindStatementsForSlice(SgNode* func_defn, SgNode* slicing_criterion, set<SgNode*>& slice,
                                     AliasAnalysisInterface* alias);
 protected:
  /*!
    Function to check if "node" is alrady in the set "slice".
   \param slice - a set of SgNodes
   \param node - the node that we want to check if is the set "slice".
   \return A boolean value that is true if "node" is in "slice". Otherwise false.
  */
  static bool alreadyInSlice(set<SgNode*> slice, SgNode* node);
  
  /*!
    Output function used for testing code: outputs the contents of the set.
    \param nodes - the SgNodes that will be outputted to screen.
    \param heading - the heading prior to the listing of the SgNode in the set "nodes"
  */
  static void writeNodes(set<SgNode*> nodes, string heading);
  
  /*!
    Output function used for testing code: outputs the contents of the set.
    \param nodes - the SgNodes that will be outputted to screen.
    \param heading - the heading prior to the listing of the SgNode in the set "nodes"
    
  */
  static void writeDUnodes(set<DefUseChainNode*> nodes, string heading);
};

#endif
