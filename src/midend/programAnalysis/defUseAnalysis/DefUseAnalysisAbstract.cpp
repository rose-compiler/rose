/******************************************                                           
  * Category: DFA                                                                      
  * DefUse Analysis Declaration (per Function)                                         
  * created by tps in Feb 2007                                                         
  *****************************************/   
// tps : Switching from rose.h to sage3 changed size from 17,5 MB to 7,2MB
#include "sage3basic.h"
#include "DefUseAnalysisAbstract.h"

using namespace std;


/**********************************************************
 *  find element in vector
 *********************************************************/
bool DefUseAnalysisAbstract::searchVector(vector <filteredCFGNodeType> done, 
                                    filteredCFGNodeType cfgNode) {
  bool found = false;
  for (vector<filteredCFGNodeType >::const_iterator i = done.begin(); i != done.end(); ++i) {
    filteredCFGNodeType node = *i;
    if (node==cfgNode)
      found=true;
  }
  return found;
}




/**********************************************************
 *  Search for the value and key in the multimap
 *********************************************************/
bool DefUseAnalysisAbstract::isDoubleExactEntry(const multitype* multi, 
                                          SgInitializedName* name, SgNode* sgNode) {
  bool isCurrentValueContained=false;
  multitype::const_iterator i = multi->begin();    
  //SgNode* sgNodeMM = NULL;
  for (; i != multi->end(); ++i) {
    SgInitializedName* initNameMM = (*i).first;
    SgNode* sgnodeMM = (*i).second;
    if (initNameMM==name && sgnodeMM==sgNode)
      isCurrentValueContained=true;
    // cout << " comparing -  map: " << initNameMM << " vs. search: " << initName <<
    //  "   result euqal? " << resBool(isCurrentValueContained) << endl;
  } 
  return isCurrentValueContained;
}

/**********************************************************
 *  Search for the value for a certain key in the multimap
 *********************************************************/
bool DefUseAnalysisAbstract::searchMulti(const multitype* multi, SgInitializedName* initName) {
  bool isCurrentValueContained=false;
  multitype::const_iterator i = multi->begin();    
  //SgNode* sgNodeMM = NULL;
  for (; i != multi->end(); ++i) {
    SgInitializedName* initNameMM = (*i).first;
    if (initNameMM==initName)
      isCurrentValueContained=true;
    // cout << " comparing -  map: " << initNameMM << " vs. search: " << initName <<
    //  "   result euqal? " << resBool(isCurrentValueContained) << endl;
  } 
  return isCurrentValueContained;
}


/**********************************************************
 *  check if two multimaps are equal
 *********************************************************/
bool DefUseAnalysisAbstract::checkElementsForEquality(const multitype* t1, const multitype* t2) {
  // if every element of t2 is contained in t1, then no change
  // occured in the map
  
  typedef set<pair<SgInitializedName*, SgNode*> > st;
  st s1(t1->begin(), t1->end());
  st s2(t2->begin(), t2->end());
  assert (s1.size() == t1->size());

  //  if (s2.size() != t2->size())
  //    printMultiMap(t2);
  assert (s2.size() == t2->size());
  return s1 != s2;
  /*


  if (t1->size()!=t2->size())
    return true;
  bool changed=false;
  for (multitype::const_iterator it = t1->begin(); it != t1->end(); ++it) {
    SgInitializedName* init1 = (*it).first;
    SgNode* node1 = (*it).second;
    bool identical=false;
    for (multitype::const_iterator it2 = t2->begin(); it2 != t2->end(); ++it2) {
      SgInitializedName* init2 = (*it2).first;
      SgNode* node2 = (*it2).second;
      if (init1==init2 && node1==node2)
        identical=true;
    }
    if (identical==false)
      changed = true;
  }  
  return changed;
  */
}



/**********************************************************
 * return the initializedName
 * we need this if we have a VarRefExp on the left hand
 * side of an assignment. We want to know which variable it is.
 * The variable may be hiding down the hirarchy in e.g. an array
 *********************************************************/
SgInitializedName* DefUseAnalysisAbstract::getInitName(SgNode* l_expr) {
  SgInitializedName* retName = NULL;
  if (isSgPntrArrRefExp(l_expr)) {
    SgPntrArrRefExp* varArrRefExp = isSgPntrArrRefExp(l_expr);
    if (DEBUG_MODE)
      cout << " **********  BINARY OP : " << l_expr->class_name() << endl;
    ROSE_ASSERT(varArrRefExp);
    SgExpression* l_expr = varArrRefExp->get_lhs_operand();
    ROSE_ASSERT(l_expr);
    if (isSgVarRefExp(l_expr)) {
      retName = isSgVarRefExp(l_expr)->get_symbol()->get_declaration();
    } 
  } else if (isSgPointerDerefExp(l_expr)) {
    SgPointerDerefExp* ptr = isSgPointerDerefExp(l_expr);
    if (DEBUG_MODE)
      cout << " **********  BINARY OP : " << l_expr->class_name() << endl;
    ROSE_ASSERT(ptr);
    SgExpression* l_expr = ptr->get_operand();
    ROSE_ASSERT(l_expr);
    if (isSgVarRefExp(l_expr)) {
      retName = isSgVarRefExp(l_expr)->get_symbol()->get_declaration();
    } 
  } else {
    if (DEBUG_MODE)
      cout << " GETINITNAME:: could not resolve initName " << l_expr->class_name() << endl;
  }
  return retName;
}

/**********************************************************
 * Resolve a CAST
 *********************************************************/
SgExpression* DefUseAnalysisAbstract::resolveCast(SgExpression* expr) {
  SgCastExp* cast = isSgCastExp(expr);
  SgExpression* retExpr = cast->get_operand();
  ROSE_ASSERT(retExpr);
  return retExpr;
}
