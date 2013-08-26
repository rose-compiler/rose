#include "sage3basic.h"
#include "compose.h"
#include "const_prop_analysis.h"
#include <boost/enable_shared_from_this.hpp>
//#include "printAnalysisStates.h"
#include "saveDotAnalysis.h"
#include "stx_analysis.h"
#include "widgets.h"
#include <set>

using namespace std;
using namespace dbglog;
namespace fuse
{
int composerDebugLevel=1;

//--------------------
//----- Composer -----
//--------------------

Composer::Composer()
{
  //domInit = false;
}

// Returns whether the given pair of AbstractObjects are may-equal at the given PartEdge
// Wrapper for calling type-specific versions of isLive without forcing the caller to care about the type of objects.
bool Composer::mayEqual(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client)
{
  ValueObjectPtr val1 = boost::dynamic_pointer_cast<ValueObject>(ao1);
  if(val1) { 
    ValueObjectPtr val2 = boost::dynamic_pointer_cast<ValueObject>(ao2);
    assert(val2);
    return mayEqualV(val1, val2, pedge, client);
  }
  
  MemLocObjectPtr ml1 = boost::dynamic_pointer_cast<MemLocObject>(ao1);
  if(ml1) { 
    MemLocObjectPtr ml2 = boost::dynamic_pointer_cast<MemLocObject>(ao2);
    assert(ml2);
    return mayEqualML(ml1, ml2, pedge, client);
  }
  
  CodeLocObjectPtr cl1 = boost::dynamic_pointer_cast<CodeLocObject>(ao1);
  if(cl1) { 
    CodeLocObjectPtr cl2 = boost::dynamic_pointer_cast<CodeLocObject>(ao2);
    assert(cl2);
    return mayEqualCL(cl1, cl2, pedge, client);
  }
  
  assert(0);
}

// Returns whether the given pair of AbstractObjects are may-equal at the given PartEdge
// Wrapper for calling type-specific versions of isLive without forcing the caller to care about the type of objects.
bool Composer::mustEqual(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client)
{
  //dbg << "Composer::mustEqual"<<endl;
  ValueObjectPtr val1 = boost::dynamic_pointer_cast<ValueObject>(ao1);
  if(val1) { 
    ValueObjectPtr val2 = boost::dynamic_pointer_cast<ValueObject>(ao2);
    assert(val2);
    return mustEqualV(val1, val2, pedge, client);
  }
  
  MemLocObjectPtr ml1 = boost::dynamic_pointer_cast<MemLocObject>(ao1);
  if(ml1) { 
    MemLocObjectPtr ml2 = boost::dynamic_pointer_cast<MemLocObject>(ao2);
    assert(ml2);
    return mustEqualML(ml1, ml2, pedge, client);
  }
  
  CodeLocObjectPtr cl1 = boost::dynamic_pointer_cast<CodeLocObject>(ao1);
  if(cl1) { 
    CodeLocObjectPtr cl2 = boost::dynamic_pointer_cast<CodeLocObject>(ao2);
    assert(cl2);
    return mustEqualCL(cl1, cl2, pedge, client);
  }
  
  assert(0);
}

// Returns whether the given AbstractObject is live at the given PartEdge
// This version is a wrapper for calling type-specific versions of isLive without forcing the caller to
// care about the type of object
bool Composer::isLive(AbstractObjectPtr ao, PartEdgePtr pedge, ComposedAnalysis* client)
{
  ValueObjectPtr val = boost::dynamic_pointer_cast<ValueObject>(ao);
  if(val) return isLiveVal(val, pedge, client);
  
  MemLocObjectPtr ml = boost::dynamic_pointer_cast<MemLocObject>(ao);
  if(ml) return isLiveMemLoc(ml, pedge, client);
  
  CodeLocObjectPtr cl = boost::dynamic_pointer_cast<CodeLocObject>(ao);
  if(cl) return isLiveCodeLoc(cl, pedge, client);
  
  assert(0);
}

/* // Returns whether dom is a dominator of part
std::set<PartPtr> Composer::isDominator(PartPtr part, PartPtr dom, ComposedAnalysis* client)
{

}

// Initializes the dominator-related data structures
void Composer::initializeDominators(ComposedAnalysis* client) 
{
  if(domInit) return;
  
  set<PartPtr> startingStates = GetStartAStates(client);
  fw_dataflowPartEdgeIterator worklist;
  for(set<PartPtr>::iterator s=startingStates.begin(); s!=startingStates.end(); s++) {
    worklist->addStart(*s);
    
    set<PartPtr> onlyS; onlyS.insert(*s);
    dominators[s] = onlyS;
  }
  
  while(*worklist!=dataflowPartEdgeIterator::end()) {
    PartPtr part = worklist->getPart();
    allParts.insert(part);
    
    // There must be a dominator record for each part before we visit it
    assert(dominators.find(part) != dominators.end());
    
    // Intersect this part's dominator set with the dominator sets of all of its successors
    list<PartEdgePtr> descendants = part->outEdges();
    for(list<PartEdgePtr>::iterator de=descendants.begin(); de!=descendants.end(); de++) {
      // If the current descendant does not have a mapping in the graph, its current dominator set must be 
      // all the parts
      PartPtr target = (*de)->target();
      if(dominators.find(target) == dominators.end()) {
        // The intersection of all parts with dominators[part] is dominators[part]
        dominators[target] = dominators[part];
      } else {
        //set_intersection(dominators[target].begin(), dominators[target].end(), dominators[part].begin(), dominators[part].end(), .begin());
        set<PartPtr>::iterator partIt = dominators[part].begin(),
                               descIt = dominators[target].begin();
        while(partIt!=dominators[part].end() && descIt!=dominators[target].end()) {
          if(*partIt < *descIt) partIt++;
          else if(*partId > *descIt) descIt = dominators[target].erase(descIt);
          else if(*partId == *descIt) {
            partIt++;
            descIt++;
          }
        }
        while(descIt!=dominators[target].end()))
          descIt = dominators[target].erase(descIt);
      }
    } 
  }
}*/

// --------------------------
// ----- Chain Composer -----
// --------------------------
  
// stxAnalysis - Points to the analysis that ChainComposer should run before any other to take application 
//    information provided in its syntax and represent it using Fuse abstractions. If stxAnalysis==NULL,
//    SyntacticAnalysis is used.
ChainComposer::ChainComposer(const list<ComposedAnalysis*>& analyses, 
                             ComposedAnalysis* testAnalysis, bool verboseTest, 
                             ComposedAnalysis* stxAnalysis) : 
    allAnalyses(analyses), testAnalysis(testAnalysis), verboseTest(verboseTest)
{
  //cout << "#allAnalyses="<<allAnalyses.size()<<endl;
  // If we're provided a syntactic analysis to use, employ it
  if(stxAnalysis) {
    doneAnalyses.push_front(stxAnalysis);
  // If we're not provided with a syntactic analysis to use, use the default one
  } else {
    // Create an instance of the syntactic analysis and insert it at the front of the done list.
    // This analysis will be called last if matching functions do not exist in any other
    // analysis and does not need a round of fixed-point iteration to produce its results.
    SyntacticAnalysis::instance()->setComposer(this);
    doneAnalyses.push_front((ComposedAnalysis*) SyntacticAnalysis::instance());
  }
  
  currentAnalysis = NULL;
  
  // Inform each analysis of the composer's identity
  //cout << "#allAnalyses="<<allAnalyses.size()<<" #doneAnalyses="<<doneAnalyses.size()<<endl;
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++) {
    //cout << "ChainComposer::ChainComposer: "<<(*a)<<" : "<<(*a)->str("") << endl;
    cout.flush();
    (*a)->setComposer(this);
  }
  if(testAnalysis) testAnalysis->setComposer(this);

}

ChainComposer::ChainComposer(const ChainComposer& that) : 
  allAnalyses(allAnalyses), doneAnalyses(doneAnalyses), 
  currentAnalysis(currentAnalysis), testAnalysis(testAnalysis), verboseTest(verboseTest)
{} 

// Generic function that looks up the composition chain from the given client 
// analysis and returns the result produced by the first instance of the function 
// called by the caller object found along the way.
template<class RetObject, class FuncCallerArgs>
RetObject ChainComposer::callServerAnalysisFunc(FuncCallerArgs& args, PartEdgePtr pedge, ComposedAnalysis* client,
                                   FuncCaller<RetObject, FuncCallerArgs>& caller, bool verbose) {
  ostringstream label;
  scope reg(txt()<<"ChainComposer::callServerAnalysisFunc() "<<caller.funcName()<<" #doneAnalyses="<<doneAnalyses.size()<<" client="<<(client? client->str(): "NULL"), scope::medium,
            composerDebugLevel, (verbose? 1: composerDebugLevel+1));
  assert(doneAnalyses.size()>0);
  //assert(serverCache.find(client) != serverCache.end());
  list<ComposedAnalysis*> doneAnalyses_back;
  
  // If the ChainComposed has already found the server analysis that implements requests of this type
/*  map<reqType, ComposedAnalysis*>::iterator server = serverCache[client].find(type);
  if(server != serverCache.end()) {
    // Pop from doneAnalyses the client, the server and all the analyses that separate them
    doneAnalyses_back.splice(doneAnalyses_back.end(), doneAnalyses, doneAnalyses.rbegin(), doneAnalyses.rbegin() + serverCache[client][type]+2);
    
    // Now invoke the given caller routine on the appropriate PartEdge
    RetObject v(caller(args, pedge, *server, client));
    
    // Restore doneAnalyses by pushing back all the analyses that were removed for the sake of recursive
    // calls to callServerAnalysisFunc().
    doneAnalyses.splice(doneAnalyses.end(), doneAnalysesCache[client][type].second);
  }*/
  // Otherwise, call work through the composition chain to identify the server
  
  /*for(list<ComposedAnalysis*>::reverse_iterator a=doneAnalyses.rbegin(); a!=doneAnalyses.rend(); a++) {
      dbg << "&nbsp;&nbsp;&nbsp;&nbsp;"<<(*a)->str("")<<" : "<<(*a)<<endl;
  }*/
  
  // Iterate backwards looking for an analysis that implements caller() behind in the chain of completed analyses
  list<ComposedAnalysis*>::reverse_iterator a=doneAnalyses.rbegin();
  while(doneAnalyses.size() >= 0) {
    scope reg(txt()<<caller.funcName() << "  : " << (*a)->str(""), scope::medium, 
              composerDebugLevel, (verbose? 1: composerDebugLevel+1));
    if(composerDebugLevel >= (verbose? 1: composerDebugLevel+1)) {
      dbg << args.str() << endl;
      dbg << "pedge="<<(pedge? pedge->str(): "NULLPartEdgePtr")<<endl;
    }
    
    ComposedAnalysis* curAnalysis = *a;
    // Move the current analysis from doneAnalyses onto a backup list to ensure that in recursive calls
    // to callServerAnalysisFunc() doneAnalyses excludes the current analysis. doneAnalyses will be restored
    // at the end of this function.
    doneAnalyses_back.push_front(curAnalysis);
    doneAnalyses.pop_back();
    a=doneAnalyses.rbegin();
       
    // Now invoke the given caller routine on the appropriate PartEdge
    try {
      RetObject v(caller(args, pedge, curAnalysis, client));
      // If control reaches here, we know that the current analysis does 
      // implement this method, so reconstruct doneAnalyses and return its reply

      // Restore doneAnalyses by pushing back all the analyses that were removed for the sake of recursive
      // calls to callServerAnalysisFunc().
      doneAnalyses.splice(doneAnalyses.end(), doneAnalyses_back);
      
      /*dbg << "Final State of doneAnalysis="<<endl;
      for(list<ComposedAnalysis*>::iterator a=doneAnalyses.begin(); a!=doneAnalyses.end(); a++)
        dbg << "    "<<(*a)->str("        ")<<endl;*/
      if(composerDebugLevel>=1 && verbose) dbg << "Returning "<<caller.retStr(v)<<endl;
      return v;
    } catch (NotImplementedException exc) {
      if(composerDebugLevel>=1 && verbose) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;"<<caller.funcName()<<" Not Implemented by "<<curAnalysis->str()<<". Advancing to "<<(*a)->str("")<<endl;
      // If control reaches here then the current analysis must not implement 
      // this method so we keep looking further back in the chain
    }
    
    // If the current caller object is concerned with PartEdges and the current analysis implements partition graphs, 
    // convert the current PartEdge that it implemented to the corresponding PartEdge of its precessor on which the 
    // current PartEdge is based.
    if(composerDebugLevel>=1 && verbose) dbg << "pedge="<<pedge->str()<<" curAnalysis->implementsPartGraph()="<<curAnalysis->implementsPartGraph()<<endl;
    if(pedge && curAnalysis->implementsPartGraph())
    { pedge = curAnalysis->convertPEdge(pedge); 
      //pedge = pedge->getParent();
      if(composerDebugLevel>=1 && verbose) dbg << "Updated: pedge="<<pedge->str()<<endl;
    }
  }
  
  // The first analysis in the chain must implement every optional method so 
  // control should never reach this point
  cerr << "ERROR: no analysis implements method "<<caller.funcName()<<"(SgExpression)";
  assert(0);
}
 
// -------------------------------------
// ----- Expression Interpretation -----
// -------------------------------------

// ----------------------
// --- Calling Expr2* ---
// ----------------------

// Contains the arguments needed by Expr2* calls
class FuncCallerArgs_Expr2Any
{
  public:
  SgNode* n;
  
  FuncCallerArgs_Expr2Any(SgNode* n) : n(n){}
  
  std::string str(std::string indent="") {
    ostringstream oss;
    oss << "[n="<<SgNode2Str(n)<<"]";
    return oss.str();
  }
};

// --- Calling Expr2Val ---

// These classes wrap the functionality of calling a specific function within an 
// Analysis or a Part
class Expr2ValCaller : public FuncCaller<ValueObjectPtr, FuncCallerArgs_Expr2Any>
{
  public:
  // Calls the given analysis' implementation of Expr2Val within the given node
  ValueObjectPtr operator()(const FuncCallerArgs_Expr2Any& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client)
  { return server->Expr2Val(args.n, pedge); }
  // Returns a string representation of the returned object
  std::string retStr(ValueObjectPtr val) { return (val ? val->str() : "NULL"); }
  string funcName() const{ return "Expr2Val"; }
};

// #SA
// check if the MemLocObject is an ExprObj or not
bool isExprObj(MemLocObjectPtr p)
{ 
  return (boost::dynamic_pointer_cast<ExprObj>(p) != NULL); 
}

ValueObjectPtr ChainComposer::Expr2Val(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client) { 
  Expr2ValCaller c;
  FuncCallerArgs_Expr2Any args(n);
  return callServerAnalysisFunc<ValueObjectPtr, FuncCallerArgs_Expr2Any>(args, pedge, client, c, false);
}

// Variant of Expr2Val that inquires about the value of the memory location denoted by the operand of the 
// given node n, where the part denotes the set of prefixes that terminate at SgNode n.
ValueObjectPtr ChainComposer::OperandExpr2Val(SgNode* n, SgNode* operand, PartEdgePtr pedge, ComposedAnalysis* client) {
  scope reg("ChainComposer::OperandExpr2Val()", scope::medium, composerDebugLevel, 2);
  if(composerDebugLevel>=2) dbg << "n="<<SgNode2Str(n)<<endl << "operand("<<operand<<")="<<SgNode2Str(operand)<<endl << "pedge="<<pedge->str()<<endl;
  
  // Get the part edges of the execution prefixes that terminate at the operand before continuing directly 
  // to SgNode n in the given part edge
  list<PartEdgePtr> opPartEdges = pedge->getOperandPartEdge(n, operand);
  if(composerDebugLevel>=2) dbg << "opPartEdges(#"<<opPartEdges.size()<<")="<<endl;
  for(list<PartEdgePtr>::iterator opE=opPartEdges.begin(); opE!=opPartEdges.end(); opE++) {
    indent ind;
    if(composerDebugLevel>=2) dbg << (*opE)->str("    ")<<endl;
  }
  
  if(opPartEdges.size()>0) {
    // The ValueObjects that represent the operand within different Parts in opParts
    list<ValueObjectPtr> partVs; 
    
    // Iterate over all the parts to get the expression and memory MemLocObjects for operand within those parts
    for(list<PartEdgePtr>::iterator opE=opPartEdges.begin(); opE!=opPartEdges.end(); opE++)
      partVs.push_back(Expr2Val(operand, *opE, client));

    return boost::static_pointer_cast<ValueObject>(boost::make_shared<UnionValueObject>(partVs));
  } else
    return NULLValueObject;
}

// --- Calling Expr2MemLoc ---

class Expr2MemLocCaller : public FuncCaller<MemLocObjectPtr, FuncCallerArgs_Expr2Any>
{
  public:
  // Calls the given analysis' implementation of Expr2MemLoc within the given node
  MemLocObjectPtr operator()(const FuncCallerArgs_Expr2Any& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client)
  { return server->Expr2MemLoc(args.n, pedge); }
  // Returns a string representation of the returned object
  std::string retStr(MemLocObjectPtr ml) { return (ml ? ml->str() : "NULL"); }
  string funcName() const{ return "Expr2MemLoc"; }
};

MemLocObjectPtr ChainComposer::Expr2MemLoc(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client) {
  // Call Expr2MemLoc_ex() and wrap the results of the memory MemLocObject with a UnionMemLocObject
  MemLocObjectPtr p = Expr2MemLoc_ex(n, pedge, client);
  // create combined memlocobject for objects that correspond to actual memory
  if(!boost::dynamic_pointer_cast<ExprObj> (p))
    p = boost::static_pointer_cast<MemLocObject> (UnionMemLocObject::create(p));

  // if(p.mem)  p.mem  = boost::static_pointer_cast<MemLocObject>(UnionMemLocObject::create(p.mem));
  // //if(p.expr) p.expr = boost::static_pointer_cast<MemLocObject>(UnionMemLocObject::create(p.expr));
  // //#SA: if the expr is valid, it better be cast as ExprObj
  // if(p.expr)  assert(boost::dynamic_pointer_cast<ExprObj> (p.expr));
  return p;
}

// #SA: Variant of Expr2MemLoc for an analysis to call its own Expr2MemLoc method to interpret complex expressions
// Composer caches memory objects for the analysis
MemLocObjectPtr ChainComposer::Expr2MemLocSelf(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* self) {
  // call its own Expr2MemLoc
  // TODO: Implement caching
  return self->Expr2MemLoc(n, pedge);
}

MemLocObjectPtr ChainComposer::Expr2MemLoc_ex(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client) { 
  // Return the pair of <object that specifies the expression temporary of n, 
  //                     object that specifies the memory location that n corresponds to>
  Expr2MemLocCaller c;
  FuncCallerArgs_Expr2Any args(n);
  MemLocObjectPtr mem = callServerAnalysisFunc<MemLocObjectPtr, FuncCallerArgs_Expr2Any>(args, pedge, client, c, false);
  return mem; // #SA: return the object by server without any wrapping

  // If mem is an expression object returned by the syntactic analysis, there is no object that
  // specifies n's memory location
  // if(boost::dynamic_pointer_cast<ExprObj>(mem))
  //   // Return mem as n's expression object and do not return an object for n's memory location
  //   return MemLocObjectPtrPair(mem, NULLMemLocObject);
  // // If mem actually corresponds to a location in memory 
  // else
  //   // Generate a fresh object for n's expression temporary and return it along with mem
  //   return MemLocObjectPtrPair(
  //             isSgExpression(n) && !isSgVarRefExp(n) ? 
  //               createExpressionMemLocObject(isSgExpression(n), isSgExpression(n)->get_type(), pedge) :
  //               NULLMemLocObject,
  //             mem);
}

// Variant of Expr2MemLoc that inquires about the memory location denoted by the operand of the given node n, where
// the part denotes the set of prefixes that terminate at SgNode n.
MemLocObjectPtr ChainComposer::OperandExpr2MemLoc(SgNode* n, SgNode* operand, PartEdgePtr pedge, ComposedAnalysis* client)
{
  scope reg("ChainComposer::OperandExpr2MemLoc()", scope::medium, composerDebugLevel, 2);
  if(composerDebugLevel>=2) dbg << "n="<<SgNode2Str(n)<<endl << "operand("<<operand<<")="<<SgNode2Str(operand)<<endl << "pedge="<<pedge->str()<<endl;
  
  // Get the parts of the execution prefixes that terminate at the operand before continuing directly 
  // to SgNode n in the given part
  list<PartEdgePtr> opPartEdges = pedge->getOperandPartEdge(n, operand);
  if(composerDebugLevel>=2) {
    dbg << "opPartEdges(#"<<opPartEdges.size()<<")="<<endl;
    for(list<PartEdgePtr>::iterator opE=opPartEdges.begin(); opE!=opPartEdges.end(); opE++) {
      indent ind;
      dbg << (*opE)->str()<<endl;
    }
  }
  
  // The memory and expression MemLocObjects that represent the operand within different Parts in opParts
  //list<MemLocObjectPtr> partMLsExpr; 
  MemLocObjectPtr partMLsExpr = NULLMemLocObject;
  list<MemLocObjectPtr> partMLsMem;
  
  // Flags that indicate whether we have memory and expression objects from all/none of the sub-parts
  // Exactly One of these must be true
  bool expr4All=true, expr4None=true;
  bool mem4All=true,  mem4None=true;
  
  // Iterate over all the part edges to get the expression and memory MemLocObjects for operand within those parts
  for(list<PartEdgePtr>::iterator opE=opPartEdges.begin(); opE!=opPartEdges.end(); opE++) {
    MemLocObjectPtr p = Expr2MemLoc_ex(operand, *opE, client);
    if(composerDebugLevel>=2) {
      dbg << "opE="<<opE->get()->str()<<endl;
      dbg << "p(expr="<<isExprObj(p)<<")="<<p->str()<<endl;
    }
    // if(!p.expr) expr4All=false;
    // else        expr4None=false;
    
    // if(!p.mem) mem4All=false;
    // else       mem4None=false;
    // We must get either an expression or a memory object
    assert(p);
    if(isExprObj(p)) {
      expr4None = false;
      mem4All = false;
    }
    // p is a memory location
    else if(p){
      expr4All = false;
      mem4None = false;
    }
    
    //if(p.expr) partMLsExpr.push_back(p.expr);
    // if(p.expr) {
    //   // All expression objects must be the same and we record the first one we see in partMLsExpr
    //   if(partMLsExpr) assert(partMLsExpr == p.expr);
    //   else partMLsExpr = p.expr;
    // }
    if(isExprObj(p)) {
      // All expression objects must be the same and we record the first one we see in partMLsExpr
      if(partMLsExpr) assert(partMLsExpr == p);
      else partMLsExpr = p;
    }
    // if(p.mem)  partMLsMem.push_back(p.mem);
    // we have a MemLocObject
    else  partMLsMem.push_back(p);
  }
  
  // Either we got expression/memory MemLocObjects from all parts or none of them
  if(opPartEdges.size()>0) {
    assert((expr4All && !expr4None) || (!expr4All && expr4None));
    assert((mem4All  && !mem4None)  || (!mem4All  && mem4None));
  }
  
  // Create a MemLocObjectPtrPair that includes UnionMemLocObjects that combine all the expression and memory
  // MemLocObjects from all the Parts that terminate at operand, using the Null MemLocObjectPtr if either
  // the expression or the memory MemLocObjects were not provided.
  // if(expr4All && mem4All) 
  //   return MemLocObjectPtrPair(partMLsExpr, // boost::static_pointer_cast<MemLocObject>(UnionMemLocObject::create(partMLsExpr)),
  //                              boost::static_pointer_cast<MemLocObject>(UnionMemLocObject::create(partMLsMem)));
  // else if(expr4All)
  //   return MemLocObjectPtrPair(partMLsExpr, // boost::static_pointer_cast<MemLocObject>(UnionMemLocObject::create(partMLsExpr)),
  //                              NULLMemLocObject);
  // else if(mem4All)
  //   return MemLocObjectPtrPair(NULLMemLocObject,
  //                              boost::static_pointer_cast<MemLocObject>(UnionMemLocObject::create(partMLsMem)));
  // // We must get either an expression or a memory object
  // else
  //   assert(0);

  scope reg2("Returning", scope::low, composerDebugLevel, 1);
  if(opPartEdges.size()>0) {
    // Create a MemLocObjectPtr that includes UnionMemLocObjects that combine all the memory
    if(mem4All) {
      UnionMemLocObjectPtr ret = UnionMemLocObject::create(partMLsMem);
      if(composerDebugLevel>=1) dbg << ret->str()<<endl;
      return boost::static_pointer_cast<MemLocObject>(ret);
    }
    else {
      assert(expr4All);
      if(composerDebugLevel>=1) dbg << (partMLsExpr? partMLsExpr->str() : "NULL")<<endl;
      return partMLsExpr;
    }
  } else
    return NULLMemLocObject;
  /*// Find the Partition(s) that correspond to the given operand of n
  std::vector<PartEdgePtr> in=part->inEdges();
  list<PartPtr> opParts;
  //for(std::vector<PartEdgePtr>::iterator e=in.begin(); e!=in.end(); e++) {
  // Walk backwards through the partition graph until we reach the Part that includes the operand
  // GB 2012-09-24: Note that there may be multiple such parts and right now we're only reaching the first one.
  //                To fully support this we need to integrate partitions into SgNodes to create a fixed mapping 
  //                between SgNodes and their containing Parts.
  back_partIterator curPart(part); curPart++;
  for(; curPart!=back_partIterator::end(); curPart++) {
    indent ind;
    dbg << "curPart="<<(*curPart)->str()<<endl;
    std::vector<CFGNode> nodes = (*curPart)->CFGNodes();
    indent ind2;
    // Look to see if any of the CFGNodes within this source Part include this operand
    for(std::vector<CFGNode>::iterator node=nodes.begin(); node!=nodes.end(); node++) {
      dbg << "node("<<node->getNode()<<")="<<CFGNode2Str(*node)<<endl;
      // If so, record it in opParts
      if(node->getNode()==operand) opParts.push_back(*curPart);
    }
    // If we've reached the part that includes the operand, we're done with the search
    if(opParts.size()>0) { break; }
  }
  if(opParts.size()==0) { dbg << "Empty opParts."<<endl; }
  assert(opParts.size()>=1);
  // We currently can only deal with the case where the operand appears in one source Part. To support the general
  // case we need to implement support for intersections of analyses, where any query to multiple analyses comes back
  // with the tightest result returned by any of them.
  assert(opParts.size()==1);
  PartPtr opPart = *opParts.begin();
  
  return Expr2MemLoc(operand, opPart, client);*/
}

// --- CallingExpr2CodeLoc ---

class Expr2CodeLocCaller : public FuncCaller<CodeLocObjectPtr, FuncCallerArgs_Expr2Any>
{
  public:
  // Calls the given analysis' implementation of Expr2CodeLoc within the given node
  CodeLocObjectPtr operator()(const FuncCallerArgs_Expr2Any& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client)
  { return server->Expr2CodeLoc(args.n, pedge);}
  // Returns a string representation of the returned object
  std::string retStr(CodeLocObjectPtr cl) { return (cl ? cl->str() : "NULL"); }
  string funcName() const{ return "Expr2CodeLoc"; }
};


CodeLocObjectPtrPair ChainComposer::Expr2CodeLoc(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client) { 
  // Return the pair of <object that specifies the expression temporary of n, 
  //                     object that specifies the memory location that n corresponds to>
  // GB: !!! Right now we don't have a firm idea of how to manage CodeLocObjects and have not yet implemented
  //     !!! an ExprObj for them. When we have done so, this code will likely mirror the code for Expr2MemLoc.
  Expr2CodeLocCaller c;
  FuncCallerArgs_Expr2Any args(n);
  return CodeLocObjectPtrPair(boost::make_shared<StxCodeLocObject>(n, pedge),
                              callServerAnalysisFunc<CodeLocObjectPtr, FuncCallerArgs_Expr2Any>(args, pedge, client, c, false));
}

/*
// Calls the isLive() method of the given MemLocObject that denotes an operand of the given SgNode n within
// the context of its own PartEdges and returns true if it may be live within any of them
bool ChainComposer::OperandIsLive(SgNode* n, SgNode* operand, MemLocObjectPtr ml, PartEdgePtr pedge, ComposedAnalysis* client) {
  // Get the parts of the execution prefixes that terminate at the operand before continuing directly 
  // to SgNode n in the given part
  list<PartEdgePtr> opPartEdges = pedge->getOperandPartEdge(n, operand);
  for(list<PartEdgePtr>::iterator opE=opPartEdges.begin(); opE!=opPartEdges.end(); opE++) {
    if(ml->isLive(*opE)) return true;
  }
  return false;
}*/

// --- Calling mayEqual* or mustEqual* ---
// Contains the arguments needed by mayEqual* and mustEqual calls
template<class FocusObject>
class FuncCallerArgs_maymustEqual
{
  public:
  FocusObject obj1;
  FocusObject obj2;
  
  FuncCallerArgs_maymustEqual(FocusObject obj1, FocusObject obj2) : obj1(obj1), obj2(obj2){}
  
  std::string str(std::string indent="") {
    ostringstream oss;
    oss << "[obj1="<<obj1->str()<<", obj2="<<obj2->str()<<"]";
    return oss.str();
  }
};

// --- Calling mayEqual* ---

// Wrap the functionality of calling a mayEqualV within an Analysis or a Part
class mayEqualVCaller : public FuncCaller<bool, FuncCallerArgs_maymustEqual<ValueObjectPtr> >
{
  public:
  // Calls the given analysis' implementation of Expr2Val within the given node
  bool operator()(const FuncCallerArgs_maymustEqual<ValueObjectPtr>& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    //dbg << "mayEqualVCaller() client="<<client->str()<<" client->implementsExpr2Val()="<<client->implementsExpr2Val()<<endl;
    // If the current analysis implements Expr2Value, call these objects' mayEqualV method
    if(server->implementsExpr2Val()) { return args.obj1->mayEqualV(args.obj2, pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "mayEqualV"; }
};

// Returns whether the given pair of ValueObjects are may-equal at the given PartEdge
bool ChainComposer::mayEqualV (ValueObjectPtr  val1, ValueObjectPtr  val2, PartEdgePtr pedge, ComposedAnalysis* client) {
  mayEqualVCaller c;
  FuncCallerArgs_maymustEqual<ValueObjectPtr> args(val1, val2);
  return callServerAnalysisFunc<bool, FuncCallerArgs_maymustEqual<ValueObjectPtr> >(args, pedge, client, c, false);
}

// Wrap the functionality of calling a mayEqualML within an Analysis or a Part
class mayEqualMLCaller : public FuncCaller<bool, FuncCallerArgs_maymustEqual<MemLocObjectPtr> >
{
  public:
  // Calls the given analysis' implementation of Expr2Val within the given node
  bool operator()(const FuncCallerArgs_maymustEqual<MemLocObjectPtr>& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    //dbg << "ChainComposer::mayEqualMLCaller() server="<<server->str()<<" client="<<client->str()<<endl;
    // If the current analysis implements Expr2Value, call these objects' mayEqualML method
    if(server->implementsExpr2MemLoc()) { return args.obj1->mayEqualML(args.obj2, pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "mayEqualML"; }
};

// Returns whether the given pair of MemLocObjects are may-equal at the given PartEdge
bool ChainComposer::mayEqualML(MemLocObjectPtr  ml1, MemLocObjectPtr  ml2, PartEdgePtr pedge, ComposedAnalysis* client)  {
  //dbg << "ChainComposer::mayEqualML() #doneAnalyses="<<doneAnalyses.size()<<" last doneAnalysis="<<doneAnalyses.back()->str()<<" currentAnalysis="<<currentAnalysis->str()<<" client="<<client->str()<<endl;
  mayEqualMLCaller c;
  FuncCallerArgs_maymustEqual<MemLocObjectPtr> args(ml1, ml2);
  return callServerAnalysisFunc<bool, FuncCallerArgs_maymustEqual<MemLocObjectPtr> >(args, pedge, client, c, false);
}

// Wrap the functionality of calling a mayEqualCL within an Analysis or a Part
class mayEqualCLCaller : public FuncCaller<bool, FuncCallerArgs_maymustEqual<CodeLocObjectPtr> >
{
  public:
  // Calls the given analysis' implementation of Expr2Val within the given node
  bool operator()(const FuncCallerArgs_maymustEqual<CodeLocObjectPtr>& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If the current analysis implements Expr2CodeLoc, call these objects' mayEqualCL method
    if(server->implementsExpr2CodeLoc()) { return args.obj1->mayEqualCL(args.obj2, pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "mayEqualCL"; }
};

// Returns whether the given pair of CodeLocObjects are may-equal at the given PartEdge
bool ChainComposer::mayEqualCL(CodeLocObjectPtr cl1, CodeLocObjectPtr cl2, PartEdgePtr pedge, ComposedAnalysis* client) {
  mayEqualCLCaller c;
  FuncCallerArgs_maymustEqual<CodeLocObjectPtr> args(cl1, cl2);
  return callServerAnalysisFunc<bool, FuncCallerArgs_maymustEqual<CodeLocObjectPtr> >(args, pedge, client, c, false);
}

// --- Calling mustEqual* ---

// Wrap the functionality of calling a mustEqualV within an Analysis or a Part
class mustEqualVCaller : public FuncCaller<bool, FuncCallerArgs_maymustEqual<ValueObjectPtr> >
{
  public:
  // Calls the given analysis' implementation of Expr2Val within the given node
  bool operator()(const FuncCallerArgs_maymustEqual<ValueObjectPtr>& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If the current analysis implements Expr2Value, call these objects' mayEqualV method
    if(server->implementsExpr2Val()) { return args.obj1->mustEqualV(args.obj2, pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "mustEqualV"; }
};

// Returns whether the given pair of ValueObjects are may-equal at the given PartEdge
bool ChainComposer::mustEqualV (ValueObjectPtr  val1, ValueObjectPtr  val2, PartEdgePtr pedge, ComposedAnalysis* client) {
  mustEqualVCaller c;
  FuncCallerArgs_maymustEqual<ValueObjectPtr> args(val1, val2);
  return callServerAnalysisFunc<bool, FuncCallerArgs_maymustEqual<ValueObjectPtr> >(args, pedge, client, c, false);
}

// Wrap the functionality of calling a mustEqualML within an Analysis or a Part
class mustEqualMLCaller : public FuncCaller<bool, FuncCallerArgs_maymustEqual<MemLocObjectPtr> >
{
  public:
  // Calls the given analysis' implementation of Expr2Val within the given node
  bool operator()(const FuncCallerArgs_maymustEqual<MemLocObjectPtr>& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If the current analysis implements Expr2Value, call these objects' mayEqualML method
    if(server->implementsExpr2MemLoc()) { return args.obj1->mustEqualML(args.obj2, pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "mustEqualML"; }
};

// Returns whether the given pair of MemLocObjects are may-equal at the given PartEdge
bool ChainComposer::mustEqualML(MemLocObjectPtr  ml1, MemLocObjectPtr  ml2, PartEdgePtr pedge, ComposedAnalysis* client)  {
  //dbg << "ChainComposer::mustEqualML"<<endl;
  mustEqualMLCaller c;
  FuncCallerArgs_maymustEqual<MemLocObjectPtr> args(ml1, ml2);
  return callServerAnalysisFunc<bool, FuncCallerArgs_maymustEqual<MemLocObjectPtr> >(args, pedge, client, c, false);
}

// Wrap the functionality of calling a mustEqualCL within an Analysis or a Part
class mustEqualCLCaller : public FuncCaller<bool, FuncCallerArgs_maymustEqual<CodeLocObjectPtr> >
{
  public:
  // Calls the given analysis' implementation of Expr2Val within the given node
  bool operator()(const FuncCallerArgs_maymustEqual<CodeLocObjectPtr>& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If the current analysis implements Expr2CodeLoc, call these objects' mayEqualCL method
    if(server->implementsExpr2CodeLoc()) { return args.obj1->mustEqualCL(args.obj2, pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "mustEqualCL"; }
};

// Returns whether the given pair of CodeLocObjects are may-equal at the given PartEdge
bool ChainComposer::mustEqualCL(CodeLocObjectPtr cl1, CodeLocObjectPtr cl2, PartEdgePtr pedge, ComposedAnalysis* client) {
  mustEqualCLCaller c;
  FuncCallerArgs_maymustEqual<CodeLocObjectPtr> args(cl1, cl2);
  return callServerAnalysisFunc<bool, FuncCallerArgs_maymustEqual<CodeLocObjectPtr> >(args, pedge, client, c, false);
}

// ------------------------
// --- Calling equalSet ---
// ------------------------
// Contains the arguments needed by mayEqual, mustEqual, equalSet and subSet calls
class FuncCallerArgs_equality
{
  public:
  AbstractObjectPtr ao1;
  AbstractObjectPtr ao2;
  
  FuncCallerArgs_equality(AbstractObjectPtr ao1, AbstractObjectPtr ao2) : ao1(ao1), ao2(ao2){}
  
  std::string str(std::string indent="") {
    ostringstream oss;
    oss << "[ao1="<<ao1->str()<<", ao2="<<ao2->str()<<"]";
    return oss.str();
  }
};

// Wraps the functionality of calling a equalSet() within an Analysis or a Part
class equalSetCaller : public FuncCaller<bool, FuncCallerArgs_equality>
{
  public:
  // Calls the given analysis' implementation of meetUpdateV within the given node
  bool operator()(const FuncCallerArgs_equality& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If this server analysis implements the type of AbstractObject that ao is, call its isFull method
    if((args.ao1->isMemLocObject()  && args.ao2->isMemLocObject()  && server->implementsExpr2MemLoc()) ||
       (args.ao1->isCodeLocObject() && args.ao2->isCodeLocObject() && server->implementsExpr2MemLoc()) ||
       (args.ao1->isValueObject()   && args.ao2->isValueObject()   && server->implementsExpr2MemLoc()))
    { return args.ao1->equalSet(args.ao2, pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "equalSet"; }
};

// Returns whether the given AbstractObject corresponds to the set of all sub-executions or the empty set
bool ChainComposer::equalSet(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client)
{
  equalSetCaller c;
  FuncCallerArgs_equality args(ao1, ao2);
  return callServerAnalysisFunc<bool, FuncCallerArgs_equality>(args, pedge, client, c, false);
}


// Wraps the functionality of calling a subSet() within an Analysis or a Part
class subSetCaller : public FuncCaller<bool, FuncCallerArgs_equality>
{
  public:
  // Calls the given analysis' implementation of meetUpdateV within the given node
  bool operator()(const FuncCallerArgs_equality& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If this server analysis implements the type of AbstractObject that ao is, call its isFull method
    if((args.ao1->isMemLocObject()  && args.ao2->isMemLocObject()  && server->implementsExpr2MemLoc()) ||
       (args.ao1->isCodeLocObject() && args.ao2->isCodeLocObject() && server->implementsExpr2MemLoc()) ||
       (args.ao1->isValueObject()   && args.ao2->isValueObject()   && server->implementsExpr2MemLoc()))
    { return args.ao1->subSet(args.ao2, pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "subSet"; }
};

// Returns whether abstract object ao1 denotes a non-strict subset (the sets may be equal) of the set denoted
// by the abstract object ao2.
bool ChainComposer::subSet(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client)
{
  subSetCaller c;
  FuncCallerArgs_equality args(ao1, ao2);
  return callServerAnalysisFunc<bool, FuncCallerArgs_equality>(args, pedge, client, c, false);
}

// -----------------------
// --- Calling isLive* ---
// -----------------------

template<class FocusObject>
class FuncCallerArgs_isLiveAny
{
  public:
  FocusObject obj;
  
  FuncCallerArgs_isLiveAny(FocusObject obj, PartEdgePtr pedge) : obj(obj){}
  
  std::string str(std::string indent="") {
    ostringstream oss;
    oss << "[obj="<<obj->str()<<"]";
    return oss.str();
  }
};

// --- Calling isLiveVal ---

// These classes wrap the functionality of calling a specific function within an Analysis or a Part
class isLiveValCaller : public FuncCaller<bool, FuncCallerArgs_isLiveAny<ValueObjectPtr> >
{
  public:
  // Calls the given analysis' implementation of Expr2Val within the given node
  bool operator()(const FuncCallerArgs_isLiveAny<ValueObjectPtr>& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If the current analysis implements Expr2Value, call this objects' isLive method
    if(server->implementsExpr2Val()) { return args.obj->isLiveV(pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "isLiveVal"; }
};

// Returns whether the given AbstractObject is live at the given part edge
bool ChainComposer::isLiveVal(ValueObjectPtr val,  PartEdgePtr pedge, ComposedAnalysis* client) {
  isLiveValCaller c;
  FuncCallerArgs_isLiveAny<ValueObjectPtr> args(val, pedge);
  return callServerAnalysisFunc<bool, FuncCallerArgs_isLiveAny<ValueObjectPtr> >(args, pedge, client, c, false);
}

// Calls the isLive() method of the given AbstractObject that denotes an operand of the given SgNode n within
// the context of its own PartEdges and returns true if it may be live within any of them
bool ChainComposer::OperandIsLiveVal(SgNode* n, SgNode* operand, ValueObjectPtr val, PartEdgePtr pedge, ComposedAnalysis* client) {
  // Get the parts of the execution prefixes that terminate at the operand before continuing directly 
  // to SgNode n in the given part
  list<PartEdgePtr> opPartEdges = pedge->getOperandPartEdge(n, operand);
  for(list<PartEdgePtr>::iterator opE=opPartEdges.begin(); opE!=opPartEdges.end(); opE++) {
    if(isLiveVal(val, *opE, client)) {
      return true;
    }
  }
  return false;
}

// --- Calling isLiveMemLoc ---

// These classes wrap the functionality of calling a specific function within an Analysis or a Part
class isLiveMemLocCaller : public FuncCaller<bool, FuncCallerArgs_isLiveAny<MemLocObjectPtr> >
{
  public:
  // Calls the given analysis' implementation of Expr2Val within the given node
  bool operator()(const FuncCallerArgs_isLiveAny<MemLocObjectPtr>& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    //dbg << "ChainComposer::isLiveMemLocCaller() server="<<server->str()<<" client="<<client->str()<<endl;
    // If the current analysis implements Expr2MemLoc, call this objects' isLive method
    if(server->implementsExpr2MemLoc()) { return args.obj->isLiveML(pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "isLiveMemLoc"; }
};

// Returns whether the given AbstractObject is live at the given part edge
bool ChainComposer::isLiveMemLoc(MemLocObjectPtr ml, PartEdgePtr pedge, ComposedAnalysis* client) {
  //dbg << "ChainComposer::isLiveMemLoc() #doneAnalyses="<<doneAnalyses.size()<<" last doneAnalysis="<<doneAnalyses.back()->str()<<" currentAnalysis="<<currentAnalysis->str()<<" client="<<client->str()<<endl;
  isLiveMemLocCaller c;
  FuncCallerArgs_isLiveAny<MemLocObjectPtr> args(ml, pedge);
  return callServerAnalysisFunc<bool, FuncCallerArgs_isLiveAny<MemLocObjectPtr> >(args, pedge, client, c, false);
}

// Calls the isLive() method of the given AbstractObject that denotes an operand of the given SgNode n within
// the context of its own PartEdges and returns true if it may be live within any of them
bool ChainComposer::OperandIsLiveMemLoc(SgNode* n, SgNode* operand, MemLocObjectPtr ml, PartEdgePtr pedge, ComposedAnalysis* client) {
  //assert(0);
  // Get the parts of the execution prefixes that terminate at the operand before continuing directly 
  // to SgNode n in the given part
  scope reg(txt()<<"ChainComposer::OperandIsLiveMemLoc(n="<<SgNode2Str(n)<<", operand="<<SgNode2Str(operand)<<")", scope::medium);
  dbg << "pedge="<<pedge->str("    ")<<endl;
  list<PartEdgePtr> opPartEdges = pedge->getOperandPartEdge(n, operand);
  dbg << "#opPartEdges="<<opPartEdges.size()<<endl;
  for(list<PartEdgePtr>::iterator opE=opPartEdges.begin(); opE!=opPartEdges.end(); opE++) {
    indent ind;
    dbg << "opE="<<opE->get()->str("    ")<<endl;
    if(isLiveMemLoc(ml, *opE, client)) return true;
  }
  return false;
}

// --- Calling isLiveCodeLoc ---

// These classes wrap the functionality of calling a specific function within an Analysis or a Part
class isLiveCodeLocCaller : public FuncCaller<bool, FuncCallerArgs_isLiveAny<CodeLocObjectPtr> >
{
  public:
  // Calls the given analysis' implementation of Expr2Val within the given node
  bool operator()(const FuncCallerArgs_isLiveAny<CodeLocObjectPtr>& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If the current analysis implements Expr2CodeLoc, call this objects' isLive method
    if(server->implementsExpr2CodeLoc()) { return args.obj->isLiveCL(pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "isLiveCodeLoc"; }
};


// Returns whether the given AbstractObject is live at the given part edge
bool ChainComposer::isLiveCodeLoc(CodeLocObjectPtr cl, PartEdgePtr pedge, ComposedAnalysis* client) {
  isLiveCodeLocCaller c;
  FuncCallerArgs_isLiveAny<CodeLocObjectPtr> args(cl, pedge);
  return callServerAnalysisFunc<bool, FuncCallerArgs_isLiveAny<CodeLocObjectPtr> >(args, pedge, client, c, false);
}

// Calls the isLive() method of the given AbstractObject that denotes an operand of the given SgNode n within
// the context of its own PartEdges and returns true if it may be live within any of them
bool ChainComposer::OperandIsLiveCodeLoc(SgNode* n, SgNode* operand, CodeLocObjectPtr cl, PartEdgePtr pedge, ComposedAnalysis* client) {
  // Get the parts of the execution prefixes that terminate at the operand before continuing directly 
  // to SgNode n in the given part
  list<PartEdgePtr> opPartEdges = pedge->getOperandPartEdge(n, operand);
  for(list<PartEdgePtr>::iterator opE=opPartEdges.begin(); opE!=opPartEdges.end(); opE++) {
    if(isLiveCodeLoc(cl, *opE, client)) return true;
  }
  return false;
}

// ---------------------------
// --- Calling meetUpdate* ---
// ---------------------------
template<class FocusObject>
class FuncCallerArgs_meetUpdateAny
{
  public:
  FocusObject to;
  FocusObject from;
  
  FuncCallerArgs_meetUpdateAny(FocusObject to, FocusObject from) : to(to), from(from){}
  
  std::string str(std::string indent="") {
    ostringstream oss;
    oss << "[to="<<to->str()<<", from="<<from->str()<<"]";
    return oss.str();
  }
};

// --- Calling meetUpdateVal ---

// These classes wrap the functionality of calling a specific function within an Analysis or a Part
class meetUpdateValCaller : public FuncCaller<bool, FuncCallerArgs_meetUpdateAny<ValueObjectPtr> >
{
  public:
  // Calls the given analysis' implementation of meetUpdateV within the given node
  bool operator()(const FuncCallerArgs_meetUpdateAny<ValueObjectPtr>& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If the current analysis implements Expr2Value, call this objects' meetUpdateV method
    if(server->implementsExpr2Val()) { return args.to->meetUpdateV(args.from, pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "meetUpdateVal"; }
};

// Returns whether the given AbstractObject is live at the given part edge
bool ChainComposer::meetUpdateVal(ValueObjectPtr to, ValueObjectPtr from, PartEdgePtr pedge, ComposedAnalysis* client)
{
  meetUpdateValCaller c;
  FuncCallerArgs_meetUpdateAny<ValueObjectPtr> args(to, from);
  return callServerAnalysisFunc<bool, FuncCallerArgs_meetUpdateAny<ValueObjectPtr> >(args, pedge, client, c, false);
}

// --- Calling meetUpdateMemLoc ---

// These classes wrap the functionality of calling a specific function within an Analysis or a Part
class meetUpdateMemLocCaller : public FuncCaller<bool, FuncCallerArgs_meetUpdateAny<MemLocObjectPtr> >
{
  public:
  // Calls the given analysis' implementation of meetUpdateV within the given node
  bool operator()(const FuncCallerArgs_meetUpdateAny<MemLocObjectPtr>& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If the current analysis implements Expr2Value, call this objects' meetUpdateML method
    if(server->implementsExpr2MemLoc()) { return args.to->meetUpdateML(args.from, pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "meetUpdateML"; }
};

// Returns whether the given AbstractObject is live at the given part edge
bool ChainComposer::meetUpdateMemLoc(MemLocObjectPtr to, MemLocObjectPtr from, PartEdgePtr pedge, ComposedAnalysis* client)
{
  meetUpdateMemLocCaller c;
  FuncCallerArgs_meetUpdateAny<MemLocObjectPtr> args(to, from);
  return callServerAnalysisFunc<bool, FuncCallerArgs_meetUpdateAny<MemLocObjectPtr> >(args, pedge, client, c, false);
}

// --- Calling meetUpdateCodeLoc ---

// These classes wrap the functionality of calling a specific function within an Analysis or a Part
class meetUpdateCodeLocCaller : public FuncCaller<bool, FuncCallerArgs_meetUpdateAny<CodeLocObjectPtr> >
{
  public:
  // Calls the given analysis' implementation of meetUpdateV within the given node
  bool operator()(const FuncCallerArgs_meetUpdateAny<CodeLocObjectPtr>& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If the current analysis implements Expr2Value, call this objects' meetUpdateCL method
    if(server->implementsExpr2CodeLoc()) { return args.to->meetUpdateCL(args.from, pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "meetUpdateCL"; }
};

// Returns whether the given AbstractObject is live at the given part edge
bool ChainComposer::meetUpdateCodeLoc(CodeLocObjectPtr to, CodeLocObjectPtr from, PartEdgePtr pedge, ComposedAnalysis* client)
{
  meetUpdateCodeLocCaller c;
  FuncCallerArgs_meetUpdateAny<CodeLocObjectPtr> args(to, from);
  return callServerAnalysisFunc<bool, FuncCallerArgs_meetUpdateAny<CodeLocObjectPtr> >(args, pedge, client, c, false);
}

// ----------------------------------
// --- Calling isFull and isEmpty ---
// ----------------------------------
class FuncCallerArgs_isFullEmpty
{
  public:
  AbstractObjectPtr ao;
  FuncCallerArgs_isFullEmpty(AbstractObjectPtr ao) : ao(ao){}
  
  std::string str(std::string indent="") {
    ostringstream oss;
    oss << "[ao="<<ao->str()<<"]";
    return oss.str();
  }
};

// Wraps the functionality of calling a isFull() within an Analysis or a Part
class isFullCaller : public FuncCaller<bool, FuncCallerArgs_isFullEmpty>
{
  public:
  // Calls the given analysis' implementation of meetUpdateV within the given node
  bool operator()(const FuncCallerArgs_isFullEmpty& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If this server analysis implements the type of AbstractObject that ao is, call its isFull method
    if((args.ao->isMemLocObject()  && server->implementsExpr2MemLoc()) ||
       (args.ao->isCodeLocObject() && server->implementsExpr2MemLoc()) ||
       (args.ao->isValueObject()   && server->implementsExpr2MemLoc()))
    { return args.ao->isFull(pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "isFull"; }
};

// Returns whether the given AbstractObject corresponds to the set of all sub-executions or the empty set
bool ChainComposer::isFull(AbstractObjectPtr ao, PartEdgePtr pedge, ComposedAnalysis* client)
{
  isFullCaller c;
  FuncCallerArgs_isFullEmpty args(ao);
  return callServerAnalysisFunc<bool, FuncCallerArgs_isFullEmpty>(args, pedge, client, c, false);
}


// Wraps the functionality of calling a isEmpty() within an Analysis or a Part
class isEmptyCaller : public FuncCaller<bool, FuncCallerArgs_isFullEmpty>
{
  public:
  // Calls the given analysis' implementation of meetUpdateV within the given node
  bool operator()(const FuncCallerArgs_isFullEmpty& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client) {
    // If this server analysis implements the type of AbstractObject that ao is, call its isEmpty method
    if((args.ao->isMemLocObject()  && server->implementsExpr2MemLoc()) ||
       (args.ao->isCodeLocObject() && server->implementsExpr2MemLoc()) ||
       (args.ao->isValueObject()   && server->implementsExpr2MemLoc()))
    { return args.ao->isEmpty(pedge); }
    // Otherwise, throw an exception to indicate that this ComposedAnalysis cannot determine the equality of the given objects
    else { throw NotImplementedException(); }
  }
  // Returns a string representation of the returned object
  std::string retStr(bool r) { return (r ? "TRUE" : "FALSE"); }
  string funcName() const{ return "isEmpty"; }
};

bool ChainComposer::isEmpty(AbstractObjectPtr ao, PartEdgePtr pedge, ComposedAnalysis* client)
{
  isEmptyCaller c;
  FuncCallerArgs_isFullEmpty args(ao);
  return callServerAnalysisFunc<bool, FuncCallerArgs_isFullEmpty>(args, pedge, client, c, false);
}

// --------------------------------
// --- Calling GetFunction*Part ---
// --------------------------------

class dummyClass: public dbglog::printable
{ public: std::string str(std::string indent="") { return ""; } };

// --- Calling GetStartAStates ---
class GetStartAStatesCaller : public FuncCaller<std::set<PartPtr>, dummyClass>
{
  public:
  // Calls the given analysis' implementation of GetStartAStates within the given node
  std::set<PartPtr> operator()(const dummyClass& dummy, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client)
  { return server->GetStartAStates(); }
  // Returns a string representation of the returned object
  std::string retStr(std::set<PartPtr> parts) { 
    ostringstream oss;
    for(std::set<PartPtr>::iterator i=parts.begin(); i!=parts.end(); i++) {
      if(i==parts.begin()) oss << ", ";
      oss << i->get()->str();
    }
    return oss.str();
  }
  string funcName() const{ return "GetStartAStates"; }
};

std::set<PartPtr> ChainComposer::GetStartAStates(ComposedAnalysis* client) { 
  dummyClass dummy; GetStartAStatesCaller c;
  //dbg << "ChainComposer::GetStartAStates"<<endl;
  return callServerAnalysisFunc<set<PartPtr>, dummyClass>(dummy, NULLPartEdge, client, c, false);
}


// --- Calling GetEndAStates ---
class GetEndAStatesCaller : public FuncCaller<std::set<PartPtr>, dummyClass>
{
  public:
  // Calls the given analysis' implementation of GetEndAStates within the given node
  std::set<PartPtr> operator()(const dummyClass& dummy, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client)
  { return server->GetEndAStates(); }
  // Returns a string representation of the returned object
  std::string retStr(std::set<PartPtr> parts) { 
    ostringstream oss;
    for(std::set<PartPtr>::iterator i=parts.begin(); i!=parts.end(); i++) {
      if(i==parts.begin()) oss << ", ";
      oss << i->get()->str();
    }
    return oss.str();
  }
  string funcName() const{ return "GetEndAStates"; }
};

std::set<PartPtr> ChainComposer::GetEndAStates(ComposedAnalysis* client) { 
  dummyClass dummy; GetEndAStatesCaller c;
  return callServerAnalysisFunc<set<PartPtr>, dummyClass>(dummy, NULLPartEdge, client, c, false);
}

// -----------------------------------------
// ----- Methods from ComposedAnalysis -----
// -----------------------------------------

// Runs the analysis, combining the intra-analysis with the inter-analysis of its choice
// ChainComposer invokes the runAnalysis methods of all its constituent analyses in sequence
void ChainComposer::runAnalysis()
{
  /*int j=1;
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++, j++) {
    cout << "ChainComposer Analysis "<<j<<": "<<(*a)<<" : "<<(*a)->str("") << endl;
    cout.flush();
  }*/
  
  string lastAnalysisName = "";
  ComposedAnalysis* lastAnalysis=NULL;
  
  int i=1;
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++, i++) {
    if((lastAnalysis || doneAnalyses.size()>0) && composerDebugLevel>=1) {
      set<PartPtr> startStates = GetStartAStates(lastAnalysis);
      set<PartPtr> endStates   = GetEndAStates(*(allAnalyses.begin()));
      ostringstream fName; fName << "ats." << i << "." << lastAnalysisName;
      ats2dot(fName.str(), "ATS", startStates, endStates);
      /*ostringstream dotStr;
      ats2dot(dotStr, "ATS", startStates, endStates);
      dbglog::graph::genGraph(dotStr.str());*/
      //ats2dot_bw(fName.str()+".BW", "ATS", startState, endStates);
    }
    
    cout << "ChainComposer Analysis "<<i<<": "<<(*a)<<" : "<<(*a)->str("") << endl;
    ostringstream label; 
    if(composerDebugLevel>=1) {
      label << "ChainComposer Running Analysis "<<i<<": "<<(*a)<<" : "<<(*a)->str("");
      lastAnalysisName = (*a)->str();
      lastAnalysis = *a;
    }
    scope reg(label.str(), scope::high, composerDebugLevel, 1);
    
    /*
    // Initialize a map for this analysis in the serverCacheMap
    std::map<reqType, list<ComposedAnalysis*> > init;
    serverCache[*a] = init;*/
    
    currentAnalysis = *a;
    (*a)->runAnalysis();
    
    /*if(composerDebugLevel >= 3) {
      scope reg("Final State", scope::medium, composerDebugLevel, 3);
      std::vector<int> factNames;
      std::vector<int> latticeNames;
      latticeNames.push_back(0);
      printAnalysisStates paa(*a, factNames, latticeNames, printAnalysisStates::below);
      UnstructuredPassInterAnalysis up_paa(paa);
      up_paa.runAnalysis();
    }*/
    
    // Record that we've completed the given analysis
    doneAnalyses.push_back(*a);
    currentAnalysis = NULL;
  }

  if(lastAnalysis && composerDebugLevel>=1) {
    set<PartPtr> startStates = GetStartAStates(lastAnalysis);
    set<PartPtr> endStates   = GetEndAStates(*(allAnalyses.begin()));
    ostringstream fName; fName << "ats." << i << "." << lastAnalysisName;
    ats2dot(fName.str(), "ATS", startStates, endStates);
    /*ostringstream dotStr;
    ats2dot(dotStr, "ATS", startStates, endStates);
    dbglog::graph::genGraph(dotStr.str());*/
    //ats2dot_bw(fName.str()+".BW", "ATS", startState, endStates);
  }
  
  if(testAnalysis) {
    scope s("---", scope::high, composerDebugLevel, 1);
    //UnstructuredPassInterDataflow inter_up(testAnalysis);
    /*ContextInsensitiveInterProceduralDataflow inter_up(testAnalysis, getCallGraph());
    inter_up.runAnalysis();*/
    testAnalysis->runAnalysis();
  }
  
  return;
}

// The Expr2* and GetFunction*Part functions are implemented by calling the same functions the 
// most analysis in the composition chain that implements them.

// Abstract interpretation functions that return this analysis' abstractions that
// represent the outcome of the given SgExpression. The default implementations of
// these throw NotImplementedException so that if a derived class does not implement
// any of these functions, the Composer is informed.
//
// The objects returned by these functions are expected to be deallocated by their callers.
ValueObjectPtr   ChainComposer::Expr2Val(SgNode* n, PartEdgePtr pedge)
{ return Expr2Val(n, pedge, this); }

MemLocObjectPtr  ChainComposer::Expr2MemLoc (SgNode* n, PartEdgePtr pedge)
{ return Expr2MemLoc(n, pedge, this); }

CodeLocObjectPtr ChainComposer::Expr2CodeLoc(SgNode* n, PartEdgePtr pedge) {
  CodeLocObjectPtrPair p = Expr2CodeLoc(n, pedge, this);
  if(p.mem) return p.mem;
  else      return p.expr;
}

// Return true if the class implements Expr2* and false otherwise
bool ChainComposer::implementsExpr2Val()
{
  // If any analyses composed in series returns true, this function returns true
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++)
    if((*a)->implementsExpr2Val()) { return true; }
  return false;
}

bool ChainComposer::implementsExpr2MemLoc()
{
  // If any analyses composed in series returns true, this function returns true
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++)
    if((*a)->implementsExpr2MemLoc()) { return true; }
  return false;
}

bool ChainComposer::implementsExpr2CodeLoc()
{
  // If any analyses composed in series returns true, this function returns true
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++)
    if((*a)->implementsExpr2CodeLoc()) { return true; }
  return false;
}

// Return the anchor Parts of the application
set<PartPtr> ChainComposer::GetStartAStates_Spec()
{ return GetStartAStates(this); }

set<PartPtr> ChainComposer::GetEndAStates_Spec()
{ return GetEndAStates(this); }

string ChainComposer::str(string indent) {
  ostringstream oss;
  oss << "[ChainComposer: ";
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); ) {
    oss << (*a)->str();
    a++;
    if(a!=allAnalyses.end()) oss << ", ";
  }
  oss << "]";
  return oss.str();
}

// -----------------------------------
// ----- Loose Parallel Composer -----
// -----------------------------------
  
LooseParallelComposer::LooseParallelComposer(const list<ComposedAnalysis*>& analyses, 
                                             knowledgeT subAnalysesImplementPartitions) : 
    allAnalyses(analyses), subAnalysesImplementPartitions(subAnalysesImplementPartitions)
{
  //initAnalysis();
  
  // Inform each analysis of the composer's identity
  //dbg << "LPC: #allAnalyses="<<allAnalyses.size()<<endl;
  //indent ind;
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++) {
    (*a)->setComposer(this);
    //dbg << (*a)->str()<<endl;
  }
  
  subAnalysesImplementPartitions = Unknown;
}

// ---------------------------------
// ----- Methods from Composer -----
// ---------------------------------

// The Expr2* and GetFunction*Part functions are implemented by calling the same functions in the parent composer

// Abstract interpretation functions that return this analysis' abstractions that 
// represent the outcome of the given SgExpression. 
// The objects returned by these functions are expected to be deallocated by their callers.
ValueObjectPtr LooseParallelComposer::Expr2Val(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->Expr2Val(n, pedge, this); }

// Variant of Expr2Val that inquires about the value of the memory location denoted by the operand of the 
// given node n, where the part denotes the set of prefixes that terminate at SgNode n.
ValueObjectPtr LooseParallelComposer::OperandExpr2Val(SgNode* n, SgNode* operand, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->OperandExpr2Val(n, operand, pedge, this); }

MemLocObjectPtr LooseParallelComposer::Expr2MemLoc(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->Expr2MemLoc(n, pedge, this); }

// Variant of Expr2MemLoc that inquires about the memory location denoted by the operand of the given node n, where
// the part denotes the set of prefixes that terminate at SgNode n.
MemLocObjectPtr LooseParallelComposer::OperandExpr2MemLoc(SgNode* n, SgNode* operand, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->OperandExpr2MemLoc(n, operand, pedge, this); }

// #SA: Variant of Expr2MemLoc for an analysis to call its own Expr2MemLoc method to interpret complex expressions
// Composer caches memory objects for the analysis
MemLocObjectPtr LooseParallelComposer::Expr2MemLocSelf(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* self) {
  // call its own Expr2MemLoc
  // TODO: Implement caching
  return self->Expr2MemLoc(n, pedge);
}

CodeLocObjectPtrPair LooseParallelComposer::Expr2CodeLoc(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->Expr2CodeLoc(n, pedge, this); }

// Returns whether the given pair of AbstractObjects are may-equal at the given PartEdge
bool LooseParallelComposer::mayEqualV (ValueObjectPtr  val1, ValueObjectPtr  val2, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->mayEqualV(val1, val2, pedge, this); }
bool LooseParallelComposer::mayEqualML(MemLocObjectPtr  ml1, MemLocObjectPtr  ml2, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->mayEqualML(ml1, ml2, pedge, this); }
bool LooseParallelComposer::mayEqualCL(CodeLocObjectPtr cl1, CodeLocObjectPtr cl2, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->mayEqualCL(cl1, cl2, pedge, this); }

// Returns whether the given pair of AbstractObjects are must-equal at the given PartEdge
bool LooseParallelComposer::mustEqualV (ValueObjectPtr  val1, ValueObjectPtr  val2, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->mustEqualV(val1, val2, pedge, this); }
bool LooseParallelComposer::mustEqualML(MemLocObjectPtr  ml1, MemLocObjectPtr  ml2, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->mustEqualML(ml1, ml2, pedge, this); }
bool LooseParallelComposer::mustEqualCL(CodeLocObjectPtr cl1, CodeLocObjectPtr cl2, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->mustEqualCL(cl1, cl2, pedge, this); }

// Returns whether the two abstract objects denote the same set of concrete objects
bool LooseParallelComposer::equalSet(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->equalSet(ao1, ao2, pedge, this); }

// Returns whether abstract object ao1 denotes a non-strict subset (the sets may be equal) of the set denoted
// by the abstract object ao2.
bool LooseParallelComposer::subSet(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->subSet(ao1, ao2, pedge, this); }

// Returns whether the given AbstractObject is live at the given part edge
bool LooseParallelComposer::isLiveVal    (ValueObjectPtr val,  PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->isLiveVal(val, pedge, this); }
bool LooseParallelComposer::isLiveMemLoc (MemLocObjectPtr ml,  PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->isLiveMemLoc(ml, pedge, this); }
bool LooseParallelComposer::isLiveCodeLoc(CodeLocObjectPtr cl, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->isLiveCodeLoc(cl, pedge, this); }

// Calls the isLive() method of the given AbstractObject that denotes an operand of the given SgNode n within
// the context of its own PartEdges and returns true if it may be live within any of them
bool LooseParallelComposer::OperandIsLiveVal    (SgNode* n, SgNode* operand, ValueObjectPtr val,  PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->OperandIsLiveVal(n, operand, val, pedge, this); }
bool LooseParallelComposer::OperandIsLiveMemLoc (SgNode* n, SgNode* operand, MemLocObjectPtr ml,  PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->OperandIsLiveMemLoc(n, operand, ml, pedge, this); }
bool LooseParallelComposer::OperandIsLiveCodeLoc(SgNode* n, SgNode* operand, CodeLocObjectPtr cl, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->OperandIsLiveCodeLoc(n, operand, cl, pedge, this); }

// Computes the meet of from and to and saves the result in to.
// Returns true if this causes this to change and false otherwise.
bool LooseParallelComposer::meetUpdateVal    (ValueObjectPtr   to, ValueObjectPtr   from, PartEdgePtr pedge, ComposedAnalysis* analysis)
{ return getComposer()->meetUpdateVal(to, from, pedge, this); }
bool LooseParallelComposer::meetUpdateMemLoc (MemLocObjectPtr  to, MemLocObjectPtr  from, PartEdgePtr pedge, ComposedAnalysis* analysis)
{ return getComposer()->meetUpdateMemLoc(to, from, pedge, this); }
bool LooseParallelComposer::meetUpdateCodeLoc(CodeLocObjectPtr to, CodeLocObjectPtr from, PartEdgePtr pedge, ComposedAnalysis* analysis)
{ return getComposer()->meetUpdateCodeLoc(to, from, pedge, this); }

// Returns whether the given AbstractObject corresponds to the set of all sub-executions or the empty set
bool LooseParallelComposer::isFull (AbstractObjectPtr ao, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->isFull(ao, pedge, this); }
bool LooseParallelComposer::isEmpty(AbstractObjectPtr ao, PartEdgePtr pedge, ComposedAnalysis* client)
{ return getComposer()->isFull(ao, pedge, this); }

// Return the anchor Parts of a given function
set<PartPtr> LooseParallelComposer::GetStartAStates(ComposedAnalysis* client)
{ return getComposer()->GetStartAStates(this); }
set<PartPtr> LooseParallelComposer::GetEndAStates(ComposedAnalysis* client)
{ return getComposer()->GetEndAStates(this); }

// -----------------------------------------
// ----- Methods from ComposedAnalysis -----
// -----------------------------------------

// Runs the analysis, combining the intra-analysis with the inter-analysis of its choice
// LooseParallelComposer invokes the runAnalysis methods of all its constituent analyses in sequence
void LooseParallelComposer::runAnalysis()
{
  // Run all the analyses without any interactions between them
  int i=1;
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++, i++) {
    scope reg(txt()<< "LooseParallelComposer Running Analysis "<<i<<": "<<(*a)->str(""), scope::high, composerDebugLevel, 1);
    
    (*a)->runAnalysis();
    
    dbg << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Analysis finished" << endl;
    
    /*if(composerDebugLevel >= 3) {
      scope reg(Final State", scope::medium, composerDebugLevel, 3);
      std::vector<int> factNames;
      std::vector<int> latticeNames;
      latticeNames.push_back(0);
      printAnalysisStates paa(*a, factNames, latticeNames, printAnalysisStates::below);
      UnstructuredPassInterAnalysis up_paa(paa);
      up_paa.runAnalysis();
    }*/
  }
  
  return;
}

// The Expr2* and GetFunction*Part functions are implemented by calling the same functions in each of the 
// constituent analyses and returning an Intersection object that includes their responses

// Abstract interpretation functions that return this analysis' abstractions that 
// represent the outcome of the given SgExpression. The default implementations of 
// these throw NotImplementedException so that if a derived class does not implement 
// any of these functions, the Composer is informed.
//
// The objects returned by these functions are expected to be deallocated by their callers.
ValueObjectPtr   LooseParallelComposer::Expr2Val(SgNode* n, PartEdgePtr pedge)
{
  // List of values that will be intersected and returned
  list<ValueObjectPtr> vals;
  
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++) {
    scope reg(txt()<<"Expr2Val  : " << (*a)->str(""), scope::medium, composerDebugLevel, 1);
    
    try {
      ValueObjectPtr val = (*a)->Expr2Val(n, getEdgeForAnalysis(pedge, *a));
      dbg << "Returning "<<val->str("")<<endl;
      vals.push_back(val);
    } catch (NotImplementedException exc) {
      if(composerDebugLevel>=1) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;Expr2Val() Not Implemented."<<endl;
      // If control reaches here then the current analysis must not implement 
      // this method so we ask the remaining analyses
      continue;
    }
  }
  
  // If no sub-analysis implements this query, forward it to the composer
  if(vals.size()==0) {
    return getComposer()->Expr2Val(n, pedge, this);
  } else 
    return boost::make_shared<IntersectValueObject>(vals);
}

MemLocObjectPtr  LooseParallelComposer::Expr2MemLoc(SgNode* n, PartEdgePtr pedge)
{
  // List of memory location that will be intersected and returned
  list<MemLocObjectPtr> mls;
  
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++) {
    scope reg(txt()<<"Expr2MemLoc  : " << (*a)->str(""), scope::medium, composerDebugLevel, 1);
    
    try {
      MemLocObjectPtr ml = (*a)->Expr2MemLoc(n, getEdgeForAnalysis(pedge, *a));
      if(composerDebugLevel >= 1) { dbg << "Returning "<<ml->str("")<<endl; }
      mls.push_back(ml);
    } catch (NotImplementedException exc) {
      if(composerDebugLevel>=1) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;Expr2MemLoc() Not Implemented."<<endl;
      // If control reaches here then the current analysis must not implement 
      // this method so we ask the remaining analyses
      continue;
    }
  }
  
  // If no sub-analysis implements this query, forward it to the composer
  return IntersectMemLocObject::create(mls);
}

CodeLocObjectPtr LooseParallelComposer::Expr2CodeLoc(SgNode* n, PartEdgePtr pedge)
{
  // List of code location that will be intersected and returned
  list<CodeLocObjectPtr> cls;
  
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++) {
    scope reg(txt()<<"Expr2CodeLoc  : " << (*a)->str(""), scope::medium, composerDebugLevel, 1);

    try {
      CodeLocObjectPtr cl = (*a)->Expr2CodeLoc(n, getEdgeForAnalysis(pedge, *a));
      if(composerDebugLevel >= 1) dbg << "Returning "<<cl->str("")<<endl;
      cls.push_back(cl);
    } catch (NotImplementedException exc) {
      if(composerDebugLevel>=1) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;Expr2CodeLoc() Not Implemented."<<endl;
      // If control reaches here then the current analysis must not implement 
      // this method so we ask the remaining analyses
      continue;
    }
  }
  
  // If no sub-analysis implements this query, forward it to the composer
  if(cls.size()==0) {
    CodeLocObjectPtrPair p = getComposer()->Expr2CodeLoc(n, pedge, this);
    if(p.mem) return p.mem;
    else      return p.expr;
  } else 
    return boost::make_shared<IntersectCodeLocObject>(cls);
}

// Return true if the class implements Expr2* and false otherwise

bool LooseParallelComposer::implementsExpr2Val()
{
  // If any analyses composed in parallel returns true, this function returns true
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++)
    if((*a)->implementsExpr2Val()) { return true; }
  return false;
}

bool LooseParallelComposer::implementsExpr2MemLoc()
{
  // If any analyses composed in parallel returns true, this function returns true
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++)
    if((*a)->implementsExpr2MemLoc()) { return true; }
  return false;
}

bool LooseParallelComposer::implementsExpr2CodeLoc()
{
  // If any analyses composed in parallel returns true, this function returns true
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++)
    if((*a)->implementsExpr2CodeLoc()) { return true; }
  return false;
}

/*// Return the anchor Parts of the application
set<PartPtr> LooseParallelComposer::GetStartAState_Spec()
{
  // The parts that will be intersected and returned
  map<ComposedAnalysis*, PartPtr> parts;
  
  // Construct the intersection of the sub-analyses responses to the GetStartAState query if we know
  // that at least one implements or if we don't yet know if any do
  if(subAnalysesImplementPartitions==True || subAnalysesImplementPartitions==Unknown) {
    for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++) {
      scope reg(txt()<<"GetStartAState  : " << (*a)->str(""), scope::medium, composerDebugLevel, 1);

      try {
        PartPtr part = (*a)->GetStartAState();
        if(composerDebugLevel >= 1) dbg << "Returning "<<part->str("")<<endl;
        parts[*a] = part;
      } catch (NotImplementedException exc) {
        if(composerDebugLevel>=1) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;GetStartAState() Not Implemented."<<endl;
        // If control reaches here then the current analysis must not implement 
        // this method so we ask the remaining analyses
        continue;
      }
    }
  
    // Since analyses either always implement GetStartAState and GetEndAStates or they do not,
    // update our knowledge about partition implementations
    subAnalysesImplementPartitions = (parts.size()>0? True: False);
  }
  
  // If no sub-analysis implements this query, forward it to the composer
  if(subAnalysesImplementPartitions==False) {
    assert(parts.size()==0);
    return getComposer()->GetStartAState(this);
  } else {
    assert(parts.size()>0);
    return makePtr<IntersectionPart>(parts, getComposer()->GetStartAState(this), this);
  }
}*/


// Return the anchor Parts of the application
// ------------------------------------------

class callStartAStates : public LooseParallelComposer::callStartOrEndAStates {
  public:
  set<PartPtr> callGetStartOrEndAStates_ComposedAnalysis(ComposedAnalysis* analysis)
  { return analysis->GetStartAStates(); }
  set<PartPtr> callGetStartOrEndAStates_Composer(Composer* composer, ComposedAnalysis* analysis)
  { return composer->GetStartAStates(analysis); }
};

class callEndAStates : public LooseParallelComposer::callStartOrEndAStates {
  public:
  set<PartPtr> callGetStartOrEndAStates_ComposedAnalysis(ComposedAnalysis* analysis)
  { return analysis->GetEndAStates(); }
  set<PartPtr> callGetStartOrEndAStates_Composer(Composer* composer, ComposedAnalysis* analysis)
  { return composer->GetEndAStates(analysis); }
};

set<PartPtr> LooseParallelComposer::GetStartAStates_Spec()
{
  callStartAStates caller;
  return GetStartOrEndAStates_Spec(caller, "GetStartAStates");
}

set<PartPtr> LooseParallelComposer::GetEndAStates_Spec()
{
  callEndAStates caller;
  return GetStartOrEndAStates_Spec(caller, "GetEndAStates");
}

// Common functionality for GetStartAStates_Spec() and GetEndAStates_Spec()
set<PartPtr> LooseParallelComposer::GetStartOrEndAStates_Spec(callStartOrEndAStates& caller, string funcName)
{
  // Stores the Parts returned by the different analyses, indexing them by their parent Parts and the analyses 
  // that returned them. All the Parts associated with the same parent Part will be grouped into a single
  // IntersectionPartEdge
  map<PartPtr, map<ComposedAnalysis*, PartPtr> > intersection;
  
  // Construct the intersection of the sub-analyses responses to the GetEndAState query if we know
  // that at least one implements or if we don't yet know if any do
  if(subAnalysesImplementPartitions==True || subAnalysesImplementPartitions==Unknown) {
    for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++) {
      scope reg(txt()<<funcName<<"  : " << (*a)->str(""), scope::medium, composerDebugLevel, 1);

      try {
        //set<PartPtr> curParts = (*a)->GetEndAStates();
        set<PartPtr> curParts = caller.callGetStartOrEndAStates_ComposedAnalysis(*a);
        
        // If this is the first analysis, simply copy its curParts into intersection
        if(a==allAnalyses.begin()) {
          for(set<PartPtr>::iterator cur=curParts.begin(); cur!=curParts.end(); cur++)
            intersection[(*cur)->getParent()][*a] = *cur;
        // If this is not the first analysis, intersect curParts with intersection, storing the result in intersection
        } else {
          map<PartPtr, map<ComposedAnalysis*, PartPtr> >::iterator i=intersection.begin();
          set<PartPtr>::iterator curI=curParts.begin();
          while(i!=intersection.end() && curI!=curParts.end()) {
            // If i and curI have the same parent Part, it must be kept in the intersection of intersection and curParts
            if((*curI)->getParent() == i->first) {
              // A single analysis cannot return multiple Parts with the same parent Part
              assert(i->second.find(*a) == i->second.end());
              
              // Add the current Part from the current analysis to the intersection map
              (i->second)[*a] = *curI;
              
              // Advance both iterators
              i++;
              curI++;
            // if curI is ahead of i, all the Parts in intersection betwen i (inclusive) and curI exclusive
            // should be removed from intersection since they're not members of the intersection of intersection and curParts
            } else if(*curI > i->first) 
              intersection.erase(++i);
            // Finally, if i is ahead of i then it skipped some Parts in curParts, which will not be included
            // in the intersection if we simply advance curI
            else if(*curI < i->first)
              curI++;
          }
          
          // If curI has reached the end of curParts but i has not yet reached the end of intersection, the
          // Parts in intersection at and after i are not in the intersection and must thus be removed
          while(i!=intersection.end())
            intersection.erase(++i);
          
          // If i has reached the end of intersection but curI has not reached the end of curParts then the remaining
          // Parts in curParts will not be included in the intersection and we don't need to do anything to ensure this.
        }
      } catch (NotImplementedException exc) {
        if(composerDebugLevel>=1) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;"<<funcName<<"() Not Implemented."<<endl;
        // If control reaches here then the current analysis must not implement 
        // this method so we ask the remaining analyses
        continue;
      }
    }
    
    // Since analyses either always implement GetStartAState and GetEndAStates or they do not,
    // update our knowledge about partition implementations
    subAnalysesImplementPartitions = (intersection.size()>0? True: False);
  }
  
  // If no sub-analysis implements this query, forward it to the composer
  if(subAnalysesImplementPartitions==False) {
    assert(intersection.size()==0);
    //return getComposer()->GetEndAStates(this);
    return caller.callGetStartOrEndAStates_Composer(getComposer(), this);
  } else {
    assert(intersection.size()>0);
    
    if(composerDebugLevel >= 1) {
      dbg << "Returning ";
      for(map<PartPtr, map<ComposedAnalysis*, PartPtr> >::iterator i=intersection.begin(); i!=intersection.end(); i++) {
        indent ind();
        dbg << "Parent: "<<i->first.get()->str()<<endl;
        for(map<ComposedAnalysis*, PartPtr>::iterator j=i->second.begin(); j!=i->second.end(); j++) {
          indent ind();
          dbg << j->first->str() << " =&gt; " << j->second->str() << endl;
        }
      }
    }
    
    // Convert all the Parts in parts into IntersectionParts to match the result of 
    set<PartPtr> interParts;
    for(map<PartPtr, map<ComposedAnalysis*, PartPtr> >::iterator i=intersection.begin(); i!=intersection.end(); i++) {
      interParts.insert(makePtr<IntersectionPart>(i->second, i->first, this));
    }
    return interParts;
  }
}

/*set<PartPtr> LooseParallelComposer::GetEndAStates_Spec()
{
  // The intersection of the responses from the different analyses, which will be returned to the caller
  set<PartPtr> intersection;
  
  // Construct the intersection of the sub-analyses responses to the GetEndAState query if we know
  // that at least one implements or if we don't yet know if any do
  if(subAnalysesImplementPartitions==True || subAnalysesImplementPartitions==Unknown) {
    for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); a++) {
      scope reg(txt()<<"GetEndAState  : " << (*a)->str(""), scope::medium, composerDebugLevel, 1);

      try {
        set<PartPtr> curParts = (*a)->GetEndAStates();
        // If this is the first analysis, simply copy its curParts into intersection
        if(a==allAnalyses.begin()) {
          intersection = curParts;
        // If this is not the first analysis, intersect curParts with intersection, storing the result in intersection
        }else {
          set<PartPtr>::iterator i=intersection.begin(),
                              curI=curParts.begin();
          while(i!=intersection.end() && curI!=curParts.begin()) {
            // If i and curI refer to the same Part, it must be kept in the intersection of intersection and curParts
            if(*curI == *i) {
              i++;
              curI++;
            // if curI is ahead of i, all the Parts in intersection betwen i (inclusive) and curI exclusive
            // should be removed from intersection since they're not members of the intersection of intersection and curParts
            } else if(*curI > *i) 
              intersection.erase(++i);
            // Finally, if i is ahead of i then it skipped some Parts in curParts, which will not be included
            // in the intersection if we simply advance curI
            else if(*curI < *i)
              curI++;
          }
          
          // If curI has reached the end of curParts but i has not yet reached the end of intersection, the
          // Parts in intersection at and after i are not in the intersection and must thus be removed
          while(i!=intersection.end())
            intersection.erase(++i);
          
          // If i has reached the end of intersection but curI has not reached the end of curParts then the remaining
          // Parts in curParts will not be included in the intersection and we don't need to do anything to ensure this.
        }
      } catch (NotImplementedException exc) {
        if(composerDebugLevel>=1) dbg << "&nbsp;&nbsp;&nbsp;&nbsp;GetEndAState() Not Implemented."<<endl;
        // If control reaches here then the current analysis must not implement 
        // this method so we ask the remaining analyses
        continue;
      }
    }
    
    // Since analyses either always implement GetStartAState and GetEndAStates or they do not,
    // update our knowledge about partition implementations
    subAnalysesImplementPartitions = (intersection.size()>0? True: False);
  }
  
  // If no sub-analysis implements this query, forward it to the composer
  if(subAnalysesImplementPartitions==False) {
    assert(parts.size()==0);
    return getComposer()->GetEndAStates(this);
  } else {
    assert(parts.size()>0);
    
    if(composerDebugLevel >= 1) {
      dbg << "Returning ";
      for(set<PartPtr>::iterator i=parts.begin(); i!=parts.end(); i++) {
        if(i==parts.begin()) dbg << ", ";
        dbg << i->get()->str("");
      }
    }
    
    // Convert all the Parts in parts into IntersectionParts to match the result of 
    set<PartPtr> interParts;
    for(set<PartPtr>::iterator i=parts.begin(); i!=parts.end(); i++) {
      set<PartPtr> singleton; singleton.insert(*i);
      interParts.insert(makePtr<IntersectionPart>(singleton, (*i)->getParent(), this));
    }
    return interParts;
  }
}*/

// When Expr2* is queried for a particular analysis on edge pedge, exported by this LooseParallelComposer 
// this function translates from the pedge that the LooseParallelComposer::Expr2* is given to the PartEdge 
// that this particular sub-analysis runs on. If some of the analyses that were composed in parallel with 
// this analysis (may include this analysis) implement partition graphs, we know that 
// GetStartAState/GetEndAState wrapped them in IntersectionPartEdges. In this case this function
// converts pedge into an IntersectionPartEdge and queries its getPartEdge method. Otherwise, 
// GetStartAState/GetEndAState do no wrapping and thus, we can return pedge directly.
PartEdgePtr LooseParallelComposer::getEdgeForAnalysis(PartEdgePtr pedge, ComposedAnalysis* analysis)
{
  assert(subAnalysesImplementPartitions != Unknown);
  
  // If some sub-analyses of this composer do implement partition graphs, unwrap the IntersectionPartEdge
  // that combines their edges
  if(subAnalysesImplementPartitions==True) {
    IntersectionPartEdgePtr iEdge = dynamicPtrCast<IntersectionPartEdge>(pedge);
    assert(iEdge);
    //dbg << "getEdgeForAnalysis iEdge="<<iEdge->str()<<endl;
    return iEdge->getPartEdge(analysis);
  // Otherwise, pass back the raw edge that came from analyses that precede the composer
  } else {
    //dbg << "getEdgeForAnalysis pedge="<<pedge->str()<<endl;
    return pedge;
  }
}

string LooseParallelComposer::str(string indent) {
  ostringstream oss;
  oss << "[LooseParallelComposer: ";
  for(list<ComposedAnalysis*>::iterator a=allAnalyses.begin(); a!=allAnalyses.end(); ) {
    oss << (*a)->str();
    a++;
    if(a!=allAnalyses.end()) oss << ", ";
  }
  oss << "]";
  return oss.str();
}

}; //namespace fuse;
