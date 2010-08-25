#include "rose.h"
#ifdef HAVE_CONFIG_H
#include "rose_config.h"
//#include "config.h"
#endif

#include "FindStatements.h"

/*************
 * FIX THIS FILE SO IT IS "CLEANER"
 *****************/



/******************************************************************
 *   FindStatementsForSlice
 *****************************************************************/
 void FindStatements::FindStatementsForSlice( SgNode* func_defn, SgNode* slicing_criterion,
                                              set<SgNode*>& slice,
                                              AliasAnalysisInterface* alias){

   
  // Slice is the list of all statements we want for the program slice, with slicing_criterion
  // as the slicing criterion.
  // We start by inserting the input statement in the list.
  slice.insert(slicing_criterion);

  //Building the defusechain from the root (the function definition)
  DefaultDUchain defuse;
  defuse.build(func_defn);
#ifdef DEBUG_FINDSTATEMENTS
  cout << "The statement we are doing slicing on is: " << slicing_criterion->unparseToString() << endl;
  cout << "\n\nDumping the DefUseChain object:" << endl;
  cout << "--------------------------------" << endl;
  defuse.Dump();
  cout << "--------------------------------\n\n" << endl;
#endif

  // build a map from memory references to Def-use chain nodes
  map<SgNode*,  DefUseChainNode*> refmap;
  for (DefaultDUchain::NodeIterator nodes = defuse.GetNodeIterator();
       !nodes.ReachEnd(); ++nodes) {
    DefUseChainNode *cur = *nodes;
    SgNode* ref = cur->get_ref(); 
    refmap[ref] = cur;
  } 

  // mapping: statements to a set of all DefUseChainNode which the statements are occuring in.
  map<SgNode*, set<DefUseChainNode*> > stmtmap;// fjernet en stjerne her..
  for (DefaultDUchain::NodeIterator nodes = defuse.GetNodeIterator();
       !nodes.ReachEnd(); ++nodes) {
    DefUseChainNode *cur = *nodes;
    SgNode* statement = cur->get_stmt();
    stmtmap[statement].insert(cur); 
  } 
  
  
  // put all the statement in a list: The statements in the list are the ones I will be
  // going through to find the def use chain nodes which "belong" to all the relevant statements
  // and track backwards more statements. Or something like that.
  list<SgNode*> stmtlist;
  // add our input statement which we do slicing on, as the first element in the list
  stmtlist.push_front(slicing_criterion);
  
  // In case we have an EXPRESSION and not a statement, we do a special traversal to find the DefUseChainNodes associated to this expression
  if(isSgExpression(slicing_criterion)!=NULL){
    // cout << "Finding DefUseChainNodes for: " << slicing_criterion->unparseToString() << endl;
    FindDefUseChainNode *duTraverse = new FindDefUseChainNode(refmap);
    duTraverse->traverse(slicing_criterion, preorder);
    set<DefUseChainNode*> refs = duTraverse->get_refs();

    // copy the statements of the defusechain nodes to the stmtlist we want to traverse 
    for(set<DefUseChainNode*>::const_iterator it=refs.begin(); it!=refs.end(); it++){
      stmtlist.push_front((*it)->get_stmt());
    }
#ifdef DEBUG_FINDSTATEMENTS  
    writeDUnodes(refs,"The DUnodes after traversing with the expression as a root:");   
#endif
   }
  
  // While there are statements to go through/traverse
  while(!stmtlist.empty()){
    SgNode* s = *(stmtlist.begin());  // get the first statement in the list
    stmtlist.pop_front(); // remove the first element from the list (the one we have just used).
    // for this statemet we refer to the mapping from statment to defusechain nodes and find the def use chain nodes with "belong" to this statement
    set<DefUseChainNode*> duset = stmtmap[s];
#ifdef DEBUG_FINDSTATEMENTS      
    writeDUnodes(duset, "duset");
#endif
    // For all the DefUseChainNodes in the set we find the incoming edges and the node this
    // edge starts from, and the statement which is "attached" to this DefUseChainNode is
    // added to the list of statments to "traverse"
    set<DefUseChainNode*>::const_iterator it;
    for(it=duset.begin();it!=duset.end();++it){
      SgNode* ref =(*it)->get_ref();   
      DefUseChainNode *refnode = refmap[ref];
      
      // traversing edges into a node
      for (DefaultDUchain::EdgeIterator edges = 
             defuse.GetNodeEdgeIterator(refnode, DefaultDUchain::EdgeIn);
	   !edges.ReachEnd(); ++edges) {
        GraphEdge *e = *edges;
	DefUseChainNode* addnode = defuse.GetEdgeEndPoint( e, DefaultDUchain::EdgeOut);
	if(addnode->get_stmt() != NULL){
	  //This test is needed so that we don't try to include a NULL-pointer. The get_stmt() of a input parameter is NULL as it is not a statement and we include the input parameter in the AST later, so we don't need to collect it here.
	// insert only if not in slice. Since the defusechains graphs may be cyclic, we
	// need this testing so that we avoid neverending loop.
	if(!alreadyInSlice(slice, addnode->get_stmt())){
	  stmtlist.push_front(addnode->get_stmt());
	  slice.insert(addnode->get_stmt());
	  }
      }
      }
    }
    stmtlist.sort();
    stmtlist.unique(); // To remove duplicates
  } 
  
#ifdef DEBUG_FINDSTATEMENTS
  writeNodes(slice,"ProgramSlicing::What slice contains:");
#endif
}
 
bool FindStatements::alreadyInSlice(set<SgNode*> slice, SgNode* node){
  // This function checks if the slice already contains the node. If contains, then we don't insert it again. (And neither insert in temporary list, thus avoiding unending loops.)
  bool isThere = false;
  set<SgNode*>::const_iterator it;
  for(it=slice.begin();it!=slice.end();++it){
    if((*it)==node){
      isThere = true;
      break;
    }
  }
  return isThere;
} 

void FindStatements::writeNodes(set<SgNode*> stmts, string heading){
  cout<< heading << endl;
  set<SgNode*>::const_iterator s;
  for(s=stmts.begin();s!=stmts.end();++s){
    cout <<"    - " << (*s)->unparseToString() << endl;
  }
}

void FindStatements::writeDUnodes(set<DefUseChainNode*> stmts, string heading){
  cout<< heading << endl;
  set<DefUseChainNode*>::const_iterator s;
  for(s=stmts.begin();s!=stmts.end();++s){
    cout <<"    - " << (*s)->ToString() << endl;
  }
}
