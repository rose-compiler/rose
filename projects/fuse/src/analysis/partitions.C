#include "sage3basic.h"
#include "partitions.h"
#include "abstract_object.h"
#include "compose.h"
using namespace std;
using namespace dbglog;
namespace fuse {

int partitionsDebugLevel=0;
  
/* #########################
   ##### Remap Functor #####
   ######################### */

MLMapping::MLMapping(MemLocObjectPtr from, MemLocObjectPtr to, bool replaceMapping) :
  from(from), to(to), replaceMapping(replaceMapping) { }

bool MLMapping::operator <(const MLMapping& that) const {
  return (from <  that.from) ||
         (from == that.from && to <  that.to) ||
         (from == that.from && to == that.to  && replaceMapping < that.replaceMapping);
}

string MLMapping::str(string indent) const {
  ostringstream oss;
  oss << "[MLMapping: replaceMapping="<<replaceMapping<<endl;
  oss << indent << "    from="<<from->str()<<endl;
  oss << indent << "    to="<<to->str()<<"]";
  return oss.str();
}

MLRemapper::MLRemapper() : initialized(false) {}

MLRemapper::MLRemapper(const MLRemapper& that) : 
      pedge(that.pedge), fwML2ML(that.fwML2ML), bwML2ML(that.bwML2ML), initialized(that.initialized)
{}

void MLRemapper::init(const PartEdgePtr pedge, ComposedAnalysis* analysis)
{
  // Initialize pedge, fwML2ML and bwML2ML only once
  if(initialized) return;
  initialized = true;
  
  /*scope reg(txt()<<"MLRemapper::init() pedge=" << pedge.get()->str(), scope::medium, 1, 1);*/
  this->pedge = pedge;
  if(pedge->source()) {
    set<CFGNode> srcNodes = pedge->source()->CFGNodes();
    for(set<CFGNode>::iterator n=srcNodes.begin(); n!=srcNodes.end(); n++) {
      // If this is a non-void return
      if(isSgReturnStmt(n->getNode()) && isSgReturnStmt(n->getNode())->get_expression()) {
        set<MLMapping> ml2ml;
        // Map the return value to a FunctionResultML
        ml2ml.insert(
            MLMapping(analysis->getComposer()->Expr2MemLoc(isSgReturnStmt(n->getNode())->get_expression(), pedge->source()->inEdgeFromAny(), analysis),
                      boost::make_shared<FuncResultMemLocObject>(Function(SageInterface::getEnclosingFunctionDeclaration(n->getNode()))),
                      false));
        fwML2ML.insert(ml2ml);
      // Else, if this is an edge out of a function's call (must be to a SgFunctionParameterList)
      } else if(isSgFunctionCallExp(n->getNode()) && n->getIndex()==2) {
        //dbg << "Function call out"<<endl;
        // Map the arguments of a function call <-> the function's parameters
        set<MLMapping> ml2ml_fw;
        setArgParamMap(pedge, isSgFunctionCallExp(n->getNode()), ml2ml_fw,
                       analysis->getComposer(),
                       analysis, true);
        fwML2ML.insert(ml2ml_fw);
        
        set<MLMapping> ml2ml_bw;
        setArgParamMap(pedge, isSgFunctionCallExp(n->getNode()), ml2ml_bw,
                       analysis->getComposer(),
                       analysis, false);
        bwML2ML.insert(invertArg2ParamMap(ml2ml_bw));
      }
    }
  }
  
  if(pedge->target()) {
    set<CFGNode> tgtNodes = pedge->target()->CFGNodes();
    for(set<CFGNode>::iterator n=tgtNodes.begin(); n!=tgtNodes.end(); n++) {
      // If this is an edge into a function's call (must be from a SgFuncDefinition)
      if(isSgFunctionCallExp(n->getNode()) && n->getIndex()==3) {
        // Mapping the function parameters that are passed by reference <-> the arguments in the function call, AND
        //         the function's return MemLocObject <-> the call's return MemLocObject
        set<MLMapping> ml2ml;
        setArgByRef2ParamMap(pedge, isSgFunctionCallExp(n->getNode()), ml2ml,
                             analysis->getComposer(),
                             analysis);
        bwML2ML.insert(ml2ml);
        fwML2ML.insert(invertArg2ParamMap(ml2ml));
      }
    }
  }
  dbg << "MLRemapper::init"<<endl<<str() <<endl;
}

// Given a lattice returns a freshly-allocated Lattice object that points to Lattice remapped in the forward direction
// Since the function is called for the scope change across some Part, it needs to account for the fact that
//    some MemLocs are in scope on one side of Part, while others are in scope on the other side. 
//    fromPEdge is the edge from which control is passing and the current PartEdge (same as the PartEdge of 
//    the Lattice) is the one to which control is passing.
Lattice* MLRemapper::forwardRemapML(Lattice* lat, PartEdgePtr fromPEdge) const {
  assert(initialized);
  assert(pedge == lat->getPartEdge());
  
  /*scope reg("MLRemapper::forwardRemapML()", scope::medium, 1, 1);
  dbg << "#fwML2ML="<<fwML2ML.size()<<" pedge="<<pedge.get()->str()<<endl;*/
  
  // Iterate through all the possible remappings, computing the Lattices that they produce
  Lattice* ret=NULL;
  for(set<set<MLMapping> >::iterator i=fwML2ML.begin(); i!=fwML2ML.end(); i++) {
    //dbg << "#i="<<i->size()<<endl;
    if(ret==NULL) ret = lat->remapML(*i, fromPEdge);
    else {
      // Merge the lattice that results from the current remapping into ret, deleting the intermediate Lattice object
      Lattice* tmp = lat->remapML(*i, fromPEdge);
      ret->meetUpdate(tmp);
      delete tmp;
    }
  }
  return ret;
}

// Given a lattice returns a freshly-allocated Lattice object that points to Lattice remapped in the forward direction
// Since the function is called for the scope change across some Part, it needs to account for the fact that
//    some MemLocs are in scope on one side of Part, while others are in scope on the other side. 
//    fromPEdge is the edge from which control is passing and the current PartEdge (same as the PartEdge of 
//    the Lattice) is the one to which control is passing.
Lattice* MLRemapper::backwardRemapML(Lattice* lat, PartEdgePtr fromPEdge) const {
  assert(initialized);
  assert(pedge == lat->getPartEdge());
  
  dbg << "MLRemapper::backwardRemapML() #bwML2ML="<<bwML2ML.size()<<" pedge="<<pedge.get()->str()<<endl;
  
  // Iterate through all the possible remappings, computing the Lattices that they produce
  Lattice* ret=NULL;
  for(set<set<MLMapping> >::iterator i=bwML2ML.begin(); i!=bwML2ML.end(); i++) {
    if(ret==NULL) ret = lat->remapML(*i, fromPEdge);
    else {
      // Merge the lattice that results from the current remapping into ret, deleting the intermediate Lattice object
      Lattice* tmp = lat->remapML(*i, fromPEdge);
      ret->meetUpdate(tmp);
      delete tmp;
    }
  }
  return ret;
}

// Returns whether if the two given ml2ml maps are equal.
bool MLRemapper::equalMaps(const set<set<MLMapping> >& ml2mlA,
                           const set<set<MLMapping> >& ml2mlB)
{
  if(ml2mlA.size() != ml2mlB.size()) return false;

  set<set<MLMapping> >::const_iterator iA=ml2mlA.begin();
  set<set<MLMapping> >::const_iterator iB=ml2mlB.begin();
  for(; iA!=ml2mlA.end() && iB!=ml2mlB.end(); iA++, iB++) {
    if(iA->size() != iB->size()) return false;
    std::set<MLMapping>::const_iterator jA=iA->begin();
    std::set<MLMapping>::const_iterator jB=iB->begin();
    for(; jA!=iA->end() && jB!=iB->end(); jA++, jB++)
      if(jA->from != jB->from || 
         jA->to != jB->to ||
         jA->replaceMapping != jB->replaceMapping)
        return false;
  }

  // If both sets have the same mappings, they're equal
  return true;
}

bool MLRemapper::operator==(const MLRemapper& that) const
{
  return equalMaps(fwML2ML, that.fwML2ML) && 
         equalMaps(bwML2ML, that.bwML2ML);
}

// String representation of object
std::string MLRemapper::map2Str(set<set<MLMapping> >& ml2ml, std::string indent) {
  ostringstream oss;

  bool first=true;
  for(set<set<MLMapping> >::iterator i=ml2ml.begin(); i!=ml2ml.end(); i++) {
    for(set<MLMapping>::iterator j=i->begin(); j!=i->end(); j++){
      if(first) first=false;
      else      oss << indent;
      oss << indent << (j->from? j->from->str(indent+"        "): "NULL") << " -&gt; " << endl;
      oss << indent << "    " << (j->to? j->to->str(indent+"        "): "NULL") << endl;
      oss << indent << "    replaceMapping=" <<j->replaceMapping<<endl;
    }
  }

  return oss.str();
}

std::string MLRemapper::str(std::string indent)
{
  ostringstream oss;

  oss << "[MLRemapper: fwML2ML="<<endl<<
                       map2Str(fwML2ML, indent+"                 ")<<endl<<
                       indent << "             bwML2ML="<<endl<<
                       map2Str(bwML2ML, indent+"                 ");
  return oss.str();
}

// Given a function call, sets argParamMap to map all arguments of this function call to their 
// corresponding parameters.
// Supports caller->callee transfers for forwards analyses and callee->caller transfers for backwards analyses
// (direction specified by the fw flag).
void setArgParamMap(PartEdgePtr callEdge, SgFunctionCallExp* call, 
                    std::set<MLMapping>& argParamMap,
                    Composer* composer, ComposedAnalysis* analysis,
                    bool fw)
{
  scope reg("setArgParamMap", scope::medium, analysisDebugLevel, 1);
  Function func(call);
  dbg << "callEdge="<<callEdge->str()<<endl;
  
  PartPtr callPart = callEdge->source();
  PartPtr funcStartPart = callEdge->target();
  
  // Part that corresponds to the function, which for now is set to be the start of its definition
  //PartPtr funcStartPart = analysis->getComposer()->GetFunctionStartPart(func, analysis);
  
  SgExpressionPtrList args = call->get_args()->get_expressions();
  //SgInitializedNamePtrList params = funcArgToParamByRef(call);
  SgInitializedNamePtrList* params = func.get_args();
  assert(params);
  /*cout << "callPart="<<callPart->str()<<endl;
  cout << "call="<<cfgUtils::SgNode2Str(call)<<endl;
  cout << "args="<<endl;
  for(SgExpressionPtrList::iterator a=args.begin(); a!=args.end(); a++)
    cout << "    "<<cfgUtils::SgNode2Str(*a)<<endl;
  cout << "params="<<endl;
  for(SgInitializedNamePtrList::iterator p=params->begin(); p!=params->end(); p++)
    cout << "    "<<cfgUtils::SgNode2Str(*p)<<", type="<<cfgUtils::SgNode2Str((*p)->get_type())<<endl;
  assert(args.size() == params->size());*/

  //dbg << "setArgParamMap() #args="<<args.size()<<" #params="<<params->size()<<"\n";
  // the state of the callee's variables at the call site
  SgExpressionPtrList::iterator itA;
  SgInitializedNamePtrList::iterator itP;
  for(itA = args.begin(), itP = params->begin(); 
      itA!=args.end() && itP!=params->end(); 
      itA++, itP++)
  {
    scope iter("iter", scope::low, analysisDebugLevel, 1);
    dbg << "itA="<<SgNode2Str(*itA)<<endl;
    dbg << "itP="<<SgNode2Str(*itP)<<endl;
    SgType* typeParam = (*itP)->get_type();
    
    // Skip "..." types, which are used to specify VarArgs.
    // NEED TO SUPPORT VAR ARGS MORE EFECTIVELY IN THE FUTURE
    if(isSgTypeEllipse(typeParam)) continue;
    // Skip variables with no names (possible in function declarations that have no definitions).
    // NEED BETTER SUPPORT FOR FUNCTIONS WITH NO BODIES
    //if((*itP)->get_name().getString() == "") continue;
    
    //cout << "itA="<<cfgUtils::SgNode2Str(*itA)<<", itP="<<cfgUtils::SgNode2Str(*itP)<<endl;
    // MemLocObjectPtrPair argP = composer->Expr2MemLoc(*itA, funcStartPart->inEdgeFromAny(), analysis);
    // The argument MemLoc is preferrably the argument expression but may be a memory location if the expression is not available
    MemLocObjectPtr arg = composer->Expr2MemLoc(*itA, callEdge->source()->outEdgeToAny(), analysis);
    // if(argP.expr) arg = argP.expr;
    // else    arg = argP.mem;
    //if(analysisDebugLevel>=1) dbg << "argParamMap["<<arg->str()<<"]="<< composer->Expr2MemLoc(*itP, funcStartPart->inEdgeFromAny(), analysis)->str()<<endl;
    /*dbg << "funcStartPart="<<funcStartPart->str()<<endl;
    dbg << "itP="<<SgNode2Str(*itP)<<endl;*/
    argParamMap.insert(MLMapping(arg,
                                 composer->Expr2MemLoc(*itP, funcStartPart->outEdgeToAny(), analysis),
                                 // In the forward direction only replace parameters passed by reference
                                 // In the backward direction replace everything
                                 !fw || (fw && isSgReferenceType(typeParam))));
  }
}

// Given a function call, sets paramArgByRef2ParamMap to map all arguments of this function call that are passed by 
// reference to their corresponding parameters and to map the call's SgFunctionCallExp expression to the MemLocObject 
// that denotes the function's declaration (associated with its return value).
// Supports callee->caller transfers for forwards analyses and caller->callee transfers for backwards analyses.
void setArgByRef2ParamMap(PartEdgePtr callEdge, SgFunctionCallExp* call, 
                          std::set<MLMapping>& paramArgByRef2ParamMap,
                          Composer* composer, ComposedAnalysis* analysis)
{
  scope reg("setArgByRef2ParamMap", scope::medium, analysisDebugLevel, 1);
  Function func(call);
  
  PartPtr callPart = callEdge->source();
  PartPtr funcStartPart = callEdge->target();
  dbg << "callEdge="<<callEdge->str()<<endl;
  
  // Part that corresponds to the function, which for now is set to be the start of its definition
  //PartPtr funcStartPart = analysis->getComposer()->GetFunctionStartPart(func, analysis);

  SgExpressionPtrList args = call->get_args()->get_expressions(); 
  SgInitializedNamePtrList* params = func.get_args();
  assert(params);

  SgExpressionPtrList::iterator itArgs;
  SgInitializedNamePtrList::iterator itParams;
  //cout << "      #params="<<params->size()<<" #args="<<args.size()<<"\n";
  for(itParams = params->begin(), itArgs = args.begin(); 
      itParams!=params->end() && itArgs!=args.end(); 
      itParams++, itArgs++)
  {
    scope iter("iter", scope::low, analysisDebugLevel, 1);
    dbg << "itArgs="<<SgNode2Str(*itArgs)<<endl;
    dbg << "itParams="<<SgNode2Str(*itParams)<<endl;
    SgType* typeParam = (*itParams)->get_type();
    
    // Skip "..." types, which are used to specify VarArgs.
    // NEED TO SUPPORT VAR ARGS MORE EFECTIVELY IN THE FUTURE
    if(isSgTypeEllipse(typeParam)) continue;
    // Skip variables with no names (possible in function declarations that have no definitions).
    // NEED BETTER SUPPORT FOR FUNCTIONS WITH NO BODIES
    //if((*itParams)->get_name().getString() == "") continue;
    
    if(isSgReferenceType(typeParam)) {
  // If the current argument expression corresponds to a real memory location, make its key the MemLocObject 
  // that corresponds to its memory location
  /*scope reg("setArgByRef2ParamMap", scope::medium, 1, 1);
  dbg << "itParams=["<<(*itParams)->unparseToString()<<" | "<<(*itParams)->class_name()<<"]"<<endl;
  dbg << "itParams MemLoc = "<<composer->Expr2MemLoc(*itParams, funcStartPart, analysis).strp(funcStartPart)<<endl;*/
  // if(isSgVarRefExp(*itArgs) || isSgPntrArrRefExp(*itArgs))
  //   paramArgByRef2ParamMap.insert(make_pair(composer->Expr2MemLoc(*itArgs, callPart->inEdgeFromAny(), analysis).mem,
  //             composer->Expr2MemLoc(*itParams, funcStartPart->outEdgeToAny(), analysis).mem));
  // // Otherwise, use the expression MemLocObject
  // else
  //   paramArgByRef2ParamMap.insert(make_pair(composer->Expr2MemLoc(*itArgs, callPart->inEdgeFromAny(), analysis).expr,
  //             composer->Expr2MemLoc(*itParams, funcStartPart->outEdgeToAny(), analysis).mem));
  //   }
      paramArgByRef2ParamMap.insert(MLMapping(composer->Expr2MemLoc(*itArgs,   callEdge->target()->inEdgeFromAny(), analysis),
                                              composer->Expr2MemLoc(*itParams, funcStartPart->outEdgeToAny(),       analysis), true));
    // Parameters that are not passed in by reference are mapped to the NULL MemLoc to indicate that their scope is 
    // purely inside the function and they should not be propagated across function boundaries
    } else
      paramArgByRef2ParamMap.insert(MLMapping(NULLMemLocObject,
                                              composer->Expr2MemLoc(*itParams, funcStartPart->outEdgeToAny(), analysis), true));
  }
  // Add the mapping from the FuncResultMemLocObject that denotes the return value to the function's call expression
  /*dbg << "declSymbol=["<<func.get_declaration()->search_for_symbol_from_symbol_table()->unparseToString()<<" | "<<func.get_declaration()->search_for_symbol_from_symbol_table()->class_name()<<"]"<<endl;
  dbg << "declSymbol MemLoc = "<<composer->Expr2MemLoc(func.get_declaration()->search_for_symbol_from_symbol_table(), funcStartPart, analysis).str()<<endl;
  dbg << "declSymbol MemLoc (funcStartPart)= "<<composer->Expr2MemLoc(func.get_declaration()->search_for_symbol_from_symbol_table(), funcStartPart, analysis).strp(funcStartPart)<<endl;
  dbg << "declSymbol MemLoc (callPart)= "<<composer->Expr2MemLoc(func.get_declaration()->search_for_symbol_from_symbol_table(), funcStartPart, analysis).strp(callPart)<<endl;*/

  // paramArgByRef2ParamMap.insert(make_pair(composer->Expr2MemLoc(call, callPart->inEdgeFromAny(), analysis).expr,
  //           composer->Expr2MemLoc(func.get_declaration()->search_for_symbol_from_symbol_table(), funcStartPart->outEdgeToAny(), analysis).mem));
  paramArgByRef2ParamMap.insert(MLMapping(composer->Expr2MemLoc(call, callEdge, analysis),
                                          //composer->Expr2MemLoc(func.get_declaration()->search_for_symbol_from_symbol_table(), funcStartPart->outEdgeToAny(), analysis)
                                          boost::make_shared<FuncResultMemLocObject>(func), true));
}

// Given a map produced by setArgParamMap or setArgByRef2ParamMap, return the same map but where the key->value 
// mappings are inverted to value->key
std::set<MLMapping> invertArg2ParamMap(std::set<MLMapping> ml2ml)
{
  std::set<MLMapping> ret;
  for(std::set<MLMapping>::iterator m=ml2ml.begin(); m!=ml2ml.end(); m++)
    ret.insert(MLMapping(m->to, m->from, m->replaceMapping));
  return ret;
}


/* ###################
   ##### Context #####
   ################### */

PartContextPtr NULLPartContextPtr;

bool PartContext::operator!=(const PartContextPtr& that) const { return !(*this==that); }
bool PartContext::operator>=(const PartContextPtr& that) const { return !(*this<that); }
bool PartContext::operator<=(const PartContextPtr& that) const { return (*this<that) || (*this == that); }
bool PartContext::operator> (const PartContextPtr& that) const { return !(*this<=that); }

Context::Context(PartPtr part)/*: part(part)*/ {
  // Initialize partContexts from this part and its ancestors
  PartPtr p = part;
  while(p) {
    if(p->getPartContext()) partContexts.push_back(p->getPartContext());
    p = p->getParent();
  }
}

// Returns the list of PartContexts that comprise this Context
const list<PartContextPtr>& Context::getPartContexts() const
{ return partContexts; }

// Returns a more detailed PartContexts lists that comprise this Context. This list contains one list for each
// PartContext implemented by some analysis and this list contains the PartContext(s) that contain a fully detailed
// description of the internal notion of context within the analysis.
list<list<PartContextPtr> > Context::getDetailedPartContexts() const {
  list<list<PartContextPtr> > subPartContexts;
  for(list<PartContextPtr>::const_iterator i=partContexts.begin(); i!=partContexts.end(); i++) {
    list<PartContextPtr> cur = (*i)->getSubPartContexts();
    
    // If there is no internal structure in this context, use the context itself
    if(cur.size()==0) cur.push_back(*i);
    
    //subPartContexts.splice(subPartContexts.end(), cur);
    subPartContexts.push_back(cur);
  }
  return subPartContexts;
}

/*
//bool Context::equalContext(ConstPartPtr a, ConstPartPtr b) { 
bool Context::equalContext(PartPtr a, PartPtr b) { 
  // If we're currently not at the root of the parent hierarchy
  if(a && b) {
           // Return whether the parent contexts are equal AND
    return equalContext(a->getParent(), b->getParent()) &&
           // This level's contexts are equal
           (a->getPartContext() && b->getPartContext() ? a->getPartContext() == b->getPartContext() :
                                                         true);
  // If we're at the root, all contexts are equal
  } else {
    // Make sure that a and b agree about their ancestry
    assert(!a); assert(!b);
    return true;
  }
}*/

bool Context::operator==(const ContextPtr& that) const { 
  //dbg << "Context::operator==  #partContexts="<<partContexts.size()<<" #that->partContexts="<<that->partContexts.size()<<endl;
  if(partContexts.size() != that->partContexts.size()) return false;
  
  list<PartContextPtr>::const_iterator thisI = partContexts.begin(),
                                       thatI = that->partContexts.begin();
  for(; thisI!=partContexts.end() && thatI!=that->partContexts.end(); thisI++, thatI++) {
    if(*thisI != *thatI) return false;
  }
  return true;
}

/*
//bool Context::lessContext(ConstPartPtr a, ConstPartPtr b) { 
bool Context::lessContext(PartPtr a, PartPtr b) { 
  // If we're currently not at the root of the parent hierarchy
  if(a && b) {
           // Return whether the parent of a is LESS-THAN the parent of b OR
    return lessContext(a->getParent(), b->getParent()) ||
           // The parents of a and b are EQUAL equal AND
           (equalContext(a->getParent(), b->getParent()) &&
           // This level's contexts are equal
            (a->getPartContext() && b->getPartContext() ? a->getPartContext() < b->getPartContext():
                                                          false));
  // If we're at the root, all contexts are equal
  } else {
    // Make sure that a and b agree about their ancestry
    assert(!a); assert(!b);
    return false;
  }
}*/
bool Context::operator<(const ContextPtr& that) const { 
  //return lessContext(part, that->part);
  dbg << "Context::operator<  #partContexts="<<partContexts.size()<<" #that->partContexts="<<that->partContexts.size()<<endl;
  if(partContexts.size() < that->partContexts.size()) return true;
  if(partContexts.size() > that->partContexts.size()) return false;
  
  list<PartContextPtr>::const_iterator thisI = partContexts.begin(),
                                       thatI = that->partContexts.begin();
  for(; thisI!=partContexts.end() && thatI!=that->partContexts.end(); thisI++, thatI++) {
    if(*thisI < *thatI) return true;
    if(*thisI > *thatI) return false;
  }
  // All contexts must be equal
  return false;
}

bool Context::operator!=(const ContextPtr& that) const { return !(*this==that); }
bool Context::operator>=(const ContextPtr& that) const { return !(*this<that); }
bool Context::operator<=(const ContextPtr& that) const { return (*this<that) || (*this == that); }
bool Context::operator> (const ContextPtr& that) const { return !(*this<=that); }

std::string Context::str(std::string indent)
{
  ostringstream oss;
  
  oss << "[Context: ";
  //oss << str_rec(part, indent);
  bool wroteAnyCtxts = false;
  for(list<PartContextPtr>::iterator p=partContexts.begin(); p!=partContexts.end(); p++) {
    string partStr = (*p)->str();
    if(partStr != "") {
      if(!wroteAnyCtxts) oss << endl << indent;
      wroteAnyCtxts = true;
      oss << partStr;
    }
  }
          
  oss << "]";
  return oss.str();
}

// Generates the string representation of this context by recursively 
// calling the string method of this parent's 
//std::string Context::str_rec(ConstPartPtr part, std::string indent)
/*std::string Context::str_rec(PartPtr part, std::string indent)
{
  if(part) {
    ostringstream oss;
    if(part->getParent()) {
      // Get the current part's context
      string partStr = str_rec(part->getParent(), indent);
      // If this part has a non-trivial context, add its string representation
      if(partStr != "") oss << str_rec(part->getParent(), indent) << endl << indent;
    }
    oss << part->getPartContext().str();
    return oss.str();
  } else
    return "";
}*/

/* ################
   ##### Part #####
   ################ */

PartPtr NULLPart;
PartEdgePtr NULLPartEdge;

Part::~Part()
{
  /*scope reg(txt()<<"Deleting Part "<<this, scope::medium, 1, 1);*/
}

// Returns the context that includes this Part and its ancestors.
ContextPtr Part::getContext() {
  //return makePtr<const Context>(makePtrFromThis(shared_from_this()));
  //return makePtr<Context>(this);
  boost::shared_ptr<Part> me_shr = shared_from_this();
  PartPtr me = makePtrFromThis(me_shr);
  return makePtr<Context>(me);
  //return NULLContextPtr;
}

// Returns the specific context of this Part. Can return the NULLPartContextPtr if this
// Part doesn't implement a non-trivial context.
PartContextPtr Part::getPartContext() const {
  return pContext;
}

// If the filter accepts (returns true) on any of the CFGNodes within this part, return true)
bool Part::filterAny(bool (*filter) (CFGNode cfgn))
{
  std::set<CFGNode> v=CFGNodes();
  for(std::set<CFGNode>::iterator i=v.begin(); i!=v.end(); i++) {
    if(filter(*i)) return true;
  }
  return false;
}

// If the filter accepts (returns true) on all of the CFGNodes within this part, return true)
bool Part::filterAll(bool (*filter) (CFGNode cfgn))
{
  std::set<CFGNode> v=CFGNodes();
  for(std::set<CFGNode>::iterator i=v.begin(); i!=v.end(); i++) {
    if(!filter(*i)) return false;
  }
  return true;
}

// Returns whether this node denotes the portion of a function call that targets the entry into another function
bool Part::isOutgoingFuncCall(CFGNode cfgn)
{ return isSgFunctionCallExp(cfgn.getNode()) && cfgn.getIndex()==2; }

// Returns whether all of this Part's CFGNodes denote the outgoing portion of a function call and
// return the relevant CFGNode(s)
bool Part::mustOutgoingFuncCall(set<CFGNode>& ret) {
  ret.clear();
  std::set<CFGNode> v=CFGNodes();
  for(std::set<CFGNode>::iterator i=v.begin(); i!=v.end(); i++) {
    if(!isOutgoingFuncCall(*i)) { ret.clear(); return false; }
    ret.insert(*i);
  }
  return true;
}

// Returns whether some of this Part's CFGNodes denote the outgoing portion of a function call and
// return the relevant CFGNode(s)
bool Part::mayOutgoingFuncCall(set<CFGNode>& ret) {
  ret.clear();
  std::set<CFGNode> v=CFGNodes();
  for(std::set<CFGNode>::iterator i=v.begin(); i!=v.end(); i++) {
    if(isOutgoingFuncCall(*i)) ret.insert(*i);
  }
  return ret.size() > 0;
}

// Returns whether this node denotes the portion of a function call to which control from the callee function
// flows after it terminates.
bool Part::isIncomingFuncCall(CFGNode cfgn)
{ return isSgFunctionCallExp(cfgn.getNode()) && cfgn.getIndex()==3; }

// Returns whether all of this Part's CFGNodes denote the incoming portion of a function call and
// return the relevant CFGNode(s)
bool Part::mustIncomingFuncCall(set<CFGNode>& ret) {
  ret.clear();
  std::set<CFGNode> v=CFGNodes();
  for(std::set<CFGNode>::iterator i=v.begin(); i!=v.end(); i++) {
    if(!isIncomingFuncCall(*i)) { ret.clear(); return false; }
    ret.insert(*i);
  }
  return true;
}

// Returns whether some of this Part's CFGNodes denote the incoming portion of a function call and
// return the relevant CFGNode(s)
bool Part::mayIncomingFuncCall(set<CFGNode>& ret) {
  ret.clear();
  std::set<CFGNode> v=CFGNodes();
  for(std::set<CFGNode>::iterator i=v.begin(); i!=v.end(); i++) {
    if(isIncomingFuncCall(*i)) ret.insert(*i);
  }
  return ret.size() > 0;
}

// Returns whether both this and that parts have the same context and their CFGNode lists consist
// exclusively of matching pairs of outgoing and incoming function calls (for each outgoing call in one
// list there's an incoming call in the other and vice versa).
bool Part::mustMatchFuncCall(PartPtr that) {
  //scope reg("Part::mustMatchFuncCall()", scope::medium, 1, 1);
  //dbg << "this = "<<str()<<endl;
  //dbg << "that = "<<that->str()<<endl;
  
  set<CFGNode> thisCN = CFGNodes();
  set<CFGNode> thatCN = that->CFGNodes();
  //dbg << "#thisCN="<<thisCN.size()<< " #thatCN="<<thatCN.size()<<endl;
  
  if(getContext() != that->getContext()) return false;
  if(thisCN.size() != thatCN.size()) return false;
  
  for(set<CFGNode>::iterator thisI=thisCN.begin(); thisI!=thisCN.end(); thisI++) {
    //dbg << "thisI="<<cfgUtils::CFGNode2Str(*thisI)<<" isIncomingFuncCall(*thisI)="<<isIncomingFuncCall(*thisI)<<endl;
    if(isIncomingFuncCall(*thisI)) {
      set<CFGNode>::iterator thatI;
      for(thatI = thatCN.begin(); thatI!=thatCN.end(); thatI++) {
        //dbg << "thatI="<<cfgUtils::CFGNode2Str(*thatI)<<" isIncomingFuncCall(*thatI)="<<isIncomingFuncCall(*thatI)<<endl;
        //dbg << "thisI->getNode() == thatI->getNode()="<<(thisI->getNode() == thatI->getNode())<<endl;
        // If thisI is an incoming call and thatI is an outgoing call
        if(isOutgoingFuncCall(*thatI) && 
           // and both correspond to the same function call
           thisI->getNode() == thatI->getNode()) break;
      }
      // If we couldn't find a match for *thisI
      if(thatI == thatCN.end()) return false;
      
    } else if(isOutgoingFuncCall(*thisI)) {
      set<CFGNode>::iterator thatI;
      for(thatI = thatCN.begin(); thatI!=thatCN.end(); thatI++) {
        // If thisI is an outgoing call and thatI is an incoming call
        if(isIncomingFuncCall(*thatI) && 
           // and both correspond to the same function call
           thisI->getNode() == thatI->getNode()) break;
      }
      // If we couldn't find a match for *thisI
      if(thatI == thatCN.end()) return false;
    }
  }
  
  // If all of thisCN has a match in all of thatCN and vice versa
  return true;
}

// Returns whether both this and that parts have the same context and their CFGNode lists include some 
// matching pairs of outgoing and incoming function calls.
bool Part::mayMatchFuncCall(PartPtr that) {
  set<CFGNode> thisCN = CFGNodes();
  set<CFGNode> thatCN = that->CFGNodes();
  
  if(getContext() != that->getContext()) return false;
  
  for(set<CFGNode>::iterator thisI=thisCN.begin(); thisI!=thisCN.end(); thisI++) {
    if(isIncomingFuncCall(*thisI)) {
      for(set<CFGNode>::iterator thatI = thatCN.begin(); thatI!=thatCN.end(); thatI++)
        // If thisI is an incoming call and thatI is an outgoing call
        if(isOutgoingFuncCall(*thatI) && 
           // and both correspond to the same function call
           thisI->getNode() == thatI->getNode()) return true;
    
    } else if(isOutgoingFuncCall(*thisI)) {
      for(set<CFGNode>::iterator thatI = thatCN.begin(); thatI!=thatCN.end(); thatI++)
        // If thisI is an outgoing call and thatI is an incoming call
        if(isIncomingFuncCall(*thatI) && 
           // and both correspond to the same function call
           thisI->getNode() == thatI->getNode()) return true;
    }
  }
  
  // If we couldn't find even one match
  return false;
}

// Returns whether this node denotes the entry into a function
bool Part::isFuncEntry(CFGNode cfgn)
{ return isSgFunctionParameterList(cfgn.getNode()); }

// Returns whether all of this Part's CFGNodes denote a function's entry node and return the relevant CFGNode(s)
bool Part::mustFuncEntry(set<CFGNode>& ret) {
  ret.clear();
  std::set<CFGNode> v=CFGNodes();
  for(std::set<CFGNode>::iterator i=v.begin(); i!=v.end(); i++) {
    if(!isFuncEntry(*i)) { ret.clear(); return false; }
    ret.insert(*i);
  }
  return true;
}

// Returns whether all of this Part's CFGNodes denote a function's entry node and return the relevant CFGNode(s)
bool Part::mayFuncEntry(set<CFGNode>& ret) {
  ret.clear();
  std::set<CFGNode> v=CFGNodes();
  for(std::set<CFGNode>::iterator i=v.begin(); i!=v.end(); i++) {
    if(isFuncEntry(*i)) ret.insert(*i);
  }
  return ret.size() > 0;
}

// Returns whether this node denotes the exit from a function
bool Part::isFuncExit(CFGNode cfgn)
{ return isSgFunctionDefinition(cfgn.getNode()); }

// Returns whether all of this Part's CFGNodes denote a function's exit node and return the relevant CFGNode(s)
bool Part::mustFuncExit(set<CFGNode>& ret) {
  ret.clear();
  std::set<CFGNode> v=CFGNodes();
  for(std::set<CFGNode>::iterator i=v.begin(); i!=v.end(); i++) {
    if(!isFuncExit(*i)) { ret.clear(); return false; }
    ret.insert(*i);
  }
  return true;
}

// Returns whether all of this Part's CFGNodes denote a function's exit node and return the relevant CFGNode(s)
bool Part::mayFuncExit(set<CFGNode>& ret) {
  ret.clear();
  std::set<CFGNode> v=CFGNodes();
  for(std::set<CFGNode>::iterator i=v.begin(); i!=v.end(); i++) {
    if(isFuncExit(*i)) ret.insert(*i);
  }
  return ret.size() > 0;
}

// If this and that come from the same analysis, call the type-specific equality test implemented
// in the derived class. Otherwise, these Parts are not equal.
bool Part::operator==(const PartPtr& that) const
{
  if(analysis == that->analysis) return equal(that);
  else                           return false;
}

// If this and that come from the same analysis, call the type-specific inequality test implemented
// in the derived class. Otherwise, determine inequality by comparing the analysis pointers.
bool Part::operator<(const PartPtr& that) const
{
  if(analysis == that->analysis) return less(that);
  else                           return analysis < that->analysis;
}

bool Part::operator!=(const PartPtr& that) const { return !(*this==that); }
bool Part::operator>=(const PartPtr& that) const { return !(*this<that); }
bool Part::operator<=(const PartPtr& that) const { return (*this<that) || (*this == that); }
bool Part::operator> (const PartPtr& that) const { return !(*this<=that); }

/* ####################
   ##### PartEdge #####
   #################### */

// Let A={ set of execution prefixes that terminate at the given anchor SgNode }
// Let O={ set of execution prefixes that terminate at anchor's operand SgNode }
// Since to reach a given SgNode an execution must first execute all of its operands it must
//    be true that there is a 1-1 mapping m() : O->A such that o in O is a prefix of m(o).
// This function is the inverse of m: given the anchor node and operand as well as the
//    PartEdge that denotes a subset of A (the function is called on this PartEdge), 
//    it returns a list of PartEdges that partition O.
std::list<PartEdgePtr> PartEdge::getOperandPartEdge(SgNode* anchor, SgNode* operand)
{
  // operand precedes anchor in the CFG, either immediately or at some distance. As such, the edge
  //   we're looking for is not necessarily the edge from operand to anchor but rather the first
  //   edge along the path from operand to anchor. Since operand is part of anchor's expression
  //   tree we're guaranteed that there is only one such path.
  // The implementor of the partition we're running on may have created multiple parts for 
  //   operand to provide path sensitivity and indeed, may have created additional outgoing edges
  //   from each of the operand's parts. Fortunately, since in the original program the original
  //   edge led from operand to anchor and the implementor of the partition could have only hierarchically 
  //   refined the original partition, all the new edges must also lead from operand to anchor.
  //   As such, the returned list contains all the outgoing edges from all the parts that correspond
  //   to operand.
  // Note: if the partitioning process is not hierarchical we may run into minor trouble since the 
  //   new edges from operand may lead to parts other than anchor. However, this is just an issue
  //   of precision since we'll account for paths that are actually infeasible.
  
  // The target of this edge identifies the termination point of all the execution prefixes
  // denoted by this edge. We thus use it to query for the parts of the operands and only both
  // if this part is itself live.
  scope reg("PartEdge::getOperandPartEdge()", scope::medium, partitionsDebugLevel, 2);
  if(partitionsDebugLevel>=2) {
    dbg << "anchor="<<SgNode2Str(anchor)<<" operand="<<SgNode2Str(operand)<<endl;
    dbg << "this PartEdge="<<str()<<endl;
  }
  
  std::list<PartEdgePtr> baseEdges = getParent()->getOperandPartEdge(anchor, operand);
  // Convert the list of edges into a set for easier/faster lookups
  set<PartEdgePtr> baseEdgesSet;
  for(list<PartEdgePtr>::iterator be=baseEdges.begin(); be!=baseEdges.end(); be++)
    baseEdgesSet.insert(*be);
  
  if(partitionsDebugLevel>=2) {
    scope regBE("baseOperandEdges", scope::medium, partitionsDebugLevel, 2);
    for(list<PartEdgePtr>::iterator be=baseEdges.begin(); be!=baseEdges.end(); be++)
      dbg << be->get()->str();
  }
    
  std::list<PartEdgePtr> ccsOperandEdges;
  bw_dataflowPartEdgeIterator it;
  it.add(makePtrFromThis(shared_from_this()));
  
  /* // There are scenarios where getOperandPartEdge() is called on edges that immediately precede
  // the desired operand->anchor edge (e.g. live-dead analysis performs its transfer function
  // on the incoming edge of the current part to correctly read live-dead decisions of any prior
  // live-dead analyses). As such, initialize the graph iterator with this PartEdge's successors.
  std::list<PartEdgePtr> out;
  if(target())      out = target()->outEdges();
  else if(source()) out = source()->outEdges();
  for(std::list<PartEdgePtr>::iterator o=out.begin(); o!=out.end(); o++)
    it.add(*o);*/
  
  if(partitionsDebugLevel>=2) dbg << "it="<<it.str()<<endl;
  scope regBE("Backward search", scope::medium, partitionsDebugLevel, 2);
  
  // Walk backwards through the CCS edges, looking for the most recent CCS edge the parent of which is in list baseEdges
  while(it!=bw_dataflowPartEdgeIterator::end()) {
    scope reg("Predecessor", scope::low, partitionsDebugLevel, 2);
    if(partitionsDebugLevel>=2) {
        dbg << it.getPartEdge().get()->str()<<endl;
        dbg << "pred-parent "<<it.getPartEdge()->getParent()->str()<<", "<<
               "source is "<<(it.getPartEdge()->getParent()->source()==NULLPart? "wildcard": "concrete")<<", "<<
               "target is "<<(it.getPartEdge()->getParent()->target()==NULLPart? "wildcard": "concrete")<<", "<<endl;
    }
    
    // If the parent of the current edge is one of the base edges
    bool isOperandEdge = false;
    
    // If the current edge has any wildcards (may occur in the first iteration, which touches this edge)
    if(it.getPartEdge()->getParent()->source()==NULLPart ||
       it.getPartEdge()->getParent()->target()==NULLPart) {
      // Look it up in baseEdges using a linear lookup that is sensitive to wildcards (this case should be 
      // rare enough that we don't optimize for it).
      for(list<PartEdgePtr>::iterator be=baseEdges.begin(); be!=baseEdges.end(); be++) {
        if(partitionsDebugLevel>=3) {
          scope sbe(txt()<<"baseEdge="<<be->get()->str(), scope::low, partitionsDebugLevel, 2);
          dbg << "it.getPartEdge()->getParent()->source()==NULLPart="<<(it.getPartEdge()->getParent()->source()==NULLPart)<<", "<<
                 "it.getPartEdge()->getParent()->target()==(*be)->target()="<<(it.getPartEdge()->getParent()->target()==(*be)->target())<<", "<<
                 "it.getPartEdge()->getParent()->target()==NULLPart="<<(it.getPartEdge()->getParent()->target()==NULLPart)<<", "<<
                 "it.getPartEdge()->getParent()->source()==(*be)->source()="<<(it.getPartEdge()->getParent()->source()==(*be)->source())<<endl;
          dbg << "it.getPartEdge()->getParent()->source()="<<it.getPartEdge()->getParent()->source()->str()<<endl;
          dbg << "(*be)->source()="<<(*be)->source()->str()<<endl;
        }
        
        if((it.getPartEdge()->getParent()->source()==NULLPart &&
            it.getPartEdge()->getParent()->target()==(*be)->target()) ||
           (it.getPartEdge()->getParent()->target()==NULLPart &&
            it.getPartEdge()->getParent()->source()==(*be)->source())) {
          isOperandEdge = true;
          break;
        }
      }
    // If the current edge is not a wildcard, use efficient lookups to search for edges that match it. 
    } else
      isOperandEdge = (baseEdgesSet.find(it.getPartEdge()->getParent()) != baseEdgesSet.end());
    
    if(isOperandEdge) {
      if(partitionsDebugLevel>=2) dbg << "    Predecessor is an Operand edge."<<endl;
      // Add it to the operand edges
      ccsOperandEdges.push_back(it.getPartEdge());
    // Otherwise, keep searching backward
    } else {
      if(partitionsDebugLevel>=2) dbg << "    Not an Operand edge. Moving on..."<<endl;
      it.pushAllDescendants();
    }
    it++;
  }

  return ccsOperandEdges;
}

// If this and that come from the same analysis, call the type-specific equality test implemented
// in the derived class. Otherwise, these Parts are not equal.
bool PartEdge::operator==(const PartEdgePtr& that) const
{
  if(analysis == that->analysis) return equal(that);
  else                           return false;
}

// If this and that come from the same analysis, call the type-specific inequality test implemented
// in the derived class. Otherwise, determine inequality by comparing the analysis pointers.
bool PartEdge::operator<(const PartEdgePtr& that) const
{
  if(analysis == that->analysis) return less(that);
  else                           return analysis < that->analysis;
}

bool PartEdge::operator!=(const PartEdgePtr& that) const { return !(*this==that); }
bool PartEdge::operator>=(const PartEdgePtr& that) const { return !(*this<that); }
bool PartEdge::operator<=(const PartEdgePtr& that) const { return (*this<that) || (*this == that); }
bool PartEdge::operator> (const PartEdgePtr& that) const { return !(*this<=that); }

// Remaps the given Lattice as needed to take into account any function call boundaries.
// Remapping is performed both in the forwards and backwards directions. 
// Returns the resulting Lattice object, which is freshly allocated.
// Since the function is called for the scope change across some Part, it needs to account for the fact that
//    some MemLocs are in scope on one side of Part, while others are in scope on the other side. 
//    fromPEdge is the edge from which control is passing and the current PartEdge (same as the PartEdge of 
//    the Lattice) is the one to which control is passing.
Lattice* PartEdge::forwardRemapML(Lattice* lat, PartEdgePtr fromPEdge) { 
  remap.init(makePtrFromThis(shared_from_this()), analysis);
  assert(makePtrFromThis(shared_from_this()) == lat->getPartEdge());
  return remap.forwardRemapML(lat, fromPEdge);
}
Lattice* PartEdge::backwardRemapML(Lattice* lat, PartEdgePtr fromPEdge) {
  remap.init(makePtrFromThis(shared_from_this()), analysis);
  assert(makePtrFromThis(shared_from_this()) == lat->getPartEdge());
  return remap.backwardRemapML(lat, fromPEdge);
}

/* ################################
   ##### ATS 2 DOT Conversion #####
   ################################ */

/* ###################################
   ##### IntersectionPartContext #####
   ################################### */

bool IntersectionPartContext::operator==(const PartContextPtr& that_arg) const {
  const IntersectionPartContext& that = dynamic_cast<const IntersectionPartContext&>(that_arg);
  
  if(pContexts.size() != that.pContexts.size()) return false;
  
  set<PartContextPtr>::const_iterator thisI=pContexts.begin();
  set<PartContextPtr>::const_iterator thatI=that.pContexts.begin();
  for(; thisI!=pContexts.end() && thatI!=that.pContexts.end(); thisI++, thatI++) {
    if(*thisI != *thatI) return false;
  }
  
  return true;
}

bool IntersectionPartContext::operator< (const PartContextPtr& that_arg) const {
  const IntersectionPartContext& that = dynamic_cast<const IntersectionPartContext&>(that_arg);
  
  if(pContexts.size() < that.pContexts.size()) return true;
  if(pContexts.size() > that.pContexts.size()) return false;
  
  set<PartContextPtr>::const_iterator thisI=pContexts.begin();
  set<PartContextPtr>::const_iterator thatI=that.pContexts.begin();
  for(; thisI!=pContexts.end() && thatI!=that.pContexts.end(); thisI++, thatI++) {
    if(*thisI <= *thatI) return true;
    if(*thisI > *thatI)  return false;
  }
  
  return false;
}
  
std::string IntersectionPartContext::str(std::string indent) {
  ostringstream oss;
  oss << "[IntersectionPartContext: ";
  bool anyValidCtxtStrs = false;
  for(set<PartContextPtr>::iterator i=pContexts.begin(); i!=pContexts.end(); i++) {
    string ctxStr = (*i).get()->str();
    if(ctxStr!="") {
      if(anyValidCtxtStrs) oss << endl << indent;
      oss << ctxStr;
      anyValidCtxtStrs = true;
    }
  }
  oss << "]"; 
  return oss.str();
}
    

/* ############################
   ##### IntersectionPart #####
   ############################ */

// Recursive computation of the cross-product of the edges in the range of analysis2Edges. Hierarchically 
// builds a recursion tree that contains more and more combinations of PartEdgePtrs from the analysis2Edges, 
// which are associated with the partition implementations of different analyses. When the recursion 
// tree reaches its full depth (one level per part in parts), it creates an intersection the current combination of 
// parent - the common PartEdge that is the parent of all the edges in the range of analysis2Edges
// curA - current iterator into analysis2Edges
// outPartEdges - the list of outgoing edges of the current combination of analysis2Edges's sub-edges, upto curA->first
// edges - vector that contains all the edges in the intersection
// analysisOfIntersection - the analysis associated with the Intersection edge
void intersectEdges(PartEdgePtr parent,
                    map<ComposedAnalysis*, set<PartEdgePtr> >::iterator curA,
                    map<ComposedAnalysis*, set<PartEdgePtr> >& analysis2Edges,
                    map<ComposedAnalysis*, PartEdgePtr> outPartEdges, 
                    list<PartEdgePtr>& edges,
                    ComposedAnalysis* analysisOfIntersection)
{
  ComposedAnalysis* curAnalysis = curA->first;

  // If we've reached the last part in parts and outEdgeParts contains all the outgoing PartEdges
  if(curA == analysis2Edges.end()) {
    PartEdgePtr newEdge = makePtr<IntersectionPartEdge>(outPartEdges, parent, analysisOfIntersection);
    //dbg << "analysisOfIntersection="<<analysisOfIntersection<<" newEdge="<<newEdge->str()<<endl;
    edges.push_back(newEdge);
  // If we haven't yet reached the end, recurse on all the outgoing edges of the current part
  } else {
    // Set nextA to follow curA
    map<ComposedAnalysis*, set<PartEdgePtr> >::iterator nextA = curA;
    nextA++;
    
    // Recurse on the cross product of the outgoing edges of this part and the outgoing edges of subsequent parts
    for(set<PartEdgePtr>::iterator e=curA->second.begin(); e!=curA->second.end(); e++){
      outPartEdges[curAnalysis] = *e;
      intersectEdges(parent, nextA, analysis2Edges, outPartEdges, edges, analysisOfIntersection);
      outPartEdges.erase(curAnalysis);
    }
  }
}

/*IntersectionPart::IntersectionPart(PartPtr part, ComposedAnalysis* analysis) : 
    Part(analysis)
{ parts.push_back(part); }*/

IntersectionPart::IntersectionPart(const std::map<ComposedAnalysis*, PartPtr>& parts, PartPtr parent, ComposedAnalysis* analysis) : 
    Part(analysis, parent), parts(parts)
{}

// Returns the list of outgoing IntersectionPartEdge of this Part, which are the cross-product of the outEdges()
// of its sub-parts.
std::list<PartEdgePtr> IntersectionPart::outEdges()
{
  /*scope reg("IntersectionPart::outEdges", scope::high, 1, 1);*/
  // For each part in parts, maps the parent part of each outgoing part to the set of parts that share this parent
  map<PartEdgePtr, map<ComposedAnalysis*, set<PartEdgePtr> > > parent2Out;
  for(map<ComposedAnalysis*, PartPtr>::iterator part=parts.begin(); part!=parts.end(); part++) {
    // Get this part's outgoing edges
    list<PartEdgePtr> out = part->second->outEdges();
    
    // Group these edges according to their common parent edge
    for(list<PartEdgePtr>::iterator e=out.begin(); e!=out.end(); e++)
      parent2Out[(*e)->getParent()][part->first].insert(*e);
  }
  
  /*for(map<PartEdgePtr, map<ComposedAnalysis*, set<PartEdgePtr> > >::iterator p=parent2Out.begin(); p!=parent2Out.end(); p++) {
    PartEdgePtr pf = p->first;
    dbg << "parent="<<pf->str()<<endl;
    indent ind;
    for(map<ComposedAnalysis*, set<PartEdgePtr> >::iterator a=p->second.begin(); a!=p->second.end(); a++) {
      indent ind;
      for(set<PartEdgePtr>::iterator e=a->second.begin(); e!=a->second.end(); e++) {
        PartEdgePtr ef = *e;
        dbg << ef->str()<<endl;
   } } }*/
  
  // Create a cross-product of the edges in parent2Out, one parent edge at a time
  std::list<PartEdgePtr> edges;
  for(map<PartEdgePtr, map<ComposedAnalysis*, set<PartEdgePtr> > >::iterator par=parent2Out.begin(); 
      par!=parent2Out.end(); par++) {
    map<ComposedAnalysis*, PartEdgePtr> outPartEdges;
    assert(par->second.size()!=0);
    intersectEdges(par->first, par->second.begin(), par->second, outPartEdges, edges, analysis);
  }
  
  /*dbg << "edges="<<endl;
  indent ind;
  for(list<PartEdgePtr>::iterator e=edges.begin(); e!=edges.end(); e++)
    dbg << (*e)->str()<<endl;*/
  
  return edges;
}

// Returns the list of incoming IntersectionPartEdge of this Part, which are the cross-product of the inEdges()
// of its sub-parts.
std::list<PartEdgePtr> IntersectionPart::inEdges()
{
  // For each part in parts, maps the parent part of each incoming part to the set of parts that share this parent
  map<PartEdgePtr, map<ComposedAnalysis*, set<PartEdgePtr> > > parent2In;
  for(map<ComposedAnalysis*, PartPtr>::iterator part=parts.begin(); part!=parts.end(); part++) {
    // Get this part's outgoing edges
    list<PartEdgePtr> in = part->second->inEdges();
    
    // Group these edges according to their common parent edge
    for(list<PartEdgePtr>::iterator e=in.begin(); e!=in.end(); e++)
      parent2In[(*e)->getParent()][part->first].insert(*e);
  }
  
  // Create a cross-product of the edges in parent2Out, one parent edge at a time
  std::list<PartEdgePtr> edges;
  for(map<PartEdgePtr, map<ComposedAnalysis*, set<PartEdgePtr> > >::iterator par=parent2In.begin(); 
      par!=parent2In.end(); par++) {
    map<ComposedAnalysis*, PartEdgePtr> inPartEdges;
    intersectEdges(par->first, par->second.begin(), par->second, inPartEdges, edges, analysis);
  }
  return edges;
}

/* // Recursive computation of the cross-product of the outEdges of all the sub-parts of this Intersection part.
// Hierarchically builds a recursion tree that contains more and more combinations of PartsPtr from the outEdges
// of different sub-parts. When the recursion tree reaches its full depth (one level per part in parts), it creates
// an intersection the current combination of 
// partI - refers to the current part in parts
// outPartEdges - the list of outgoing edges of the current combination of this IntersectionPart's sub-parts, 
//         upto partI
void IntersectionPart::outEdges_rec(std::map<ComposedAnalysis*, PartPtr>::iterator partI, 
                                    std::map<ComposedAnalysis*, PartPtr> outPartEdges, 
                                    std::vector<PartEdgePtr>& edges) {
  // If we've reached the last part in parts and outEdgeParts contains all the outgoing PartEdges
  if(partI == parts.end())
    edges.push_back(makePtr<IntersectionPartEdge>(outPartEdges, analysis));
  // If we haven't yet reached the end, recurse on all the outgoing edges of the current part
  else {
    // Get this part's outgoing edges
    vector<PartEdgePtr> out = (*partI)->outEdges();
    // Maps the parent part of each outgoing part to the set of parts that share this parent
    map<PartPrt, set<PartEdgePtr> > parent2Out;
    for(vector<PartEdgePtr>::iterator e=out.begin(); e!=out.end(); e++)
      parent2Out[(*e)->getParent()].insert(*e);
    
    ComposedAnalysis* curAnalysis = partI->first;
    
    // Advance to the next part in parts
    partI++;
    
    // Recurse on the cross product of the outgoing edges of this part and the outgoing edges of subsequent parts
    for(vector<PartEdgePtr>::iterator e=partOut.begin(); e!=partOut.end(); e++){
      outPartEdges[curAnalysis] = *e;
      outEdges_rec(partI, outPartEdges, edges);
      outPartEdges.pop_back();
    }
  }
}*/

/*// Returns the list of incoming IntersectionPartEdge of this Part, which are the cross-product of the inEdges()
// of its sub-parts.
std::vector<PartEdgePtr> IntersectionPart::inEdges() {
  list<PartEdgePtr> inPartEdges;
  vector<PartEdgePtr> edges;
  inEdges_rec(parts.begin(), inPartEdges, edges);
  return edges;
}
// Recursive computation of the cross-product of the inEdges of all the sub-parts of this Intersection part.
// Hierarchically builds a recursion tree that contains more and more combinations of PartsPtr from the inEdges
// of different sub-parts. When the recursion tree reaches its full depth (one level per part in parts), it creates
// an intersection the current combination of 
// partI - refers to the current part in parts
// inPartEdges - the list of incoming edges of the current combination of this IntersectionPart's sub-parts, 
//         upto partI
void IntersectionPart::inEdges_rec(list<PartPtr>::iterator partI, list<PartEdgePtr> inPartEdges, 
                                   vector<PartEdgePtr>& edges) {
  // If we've reached the last part in parts and inEdgeParts contains all the incoming PartEdges
  if(partI == parts.end())
    edges.push_back(makePtr<IntersectionPartEdge>(inPartEdges, analysis));
  // If we haven't yet reached the end, recurse on all the incoming edges of the current part
  else {
    // Get this part's incoming edges
    vector<PartEdgePtr> partIn = (*partI)->inEdges();
    
    // Advance to the next part in parts
    partI++;
    
    // Recurse on the cross product of the ingoing edges of this part and the incoming edges of subsequent parts
    for(vector<PartEdgePtr>::iterator e=partIn.begin(); e!=partIn.end(); e++){
      inPartEdges.push_back(*e);
      inEdges_rec(partI, inPartEdges, edges);
      inPartEdges.pop_back();
    }
  }
}*/

// Returns the intersection of the lists of CFGNodes returned by the Parts in parts
set<CFGNode> IntersectionPart::CFGNodes() const {
  set<CFGNode> nodes;
  bool initializedNodes=false; // Records whether nodes has been initialized from one of the parts
  for(map<ComposedAnalysis*, PartPtr>::const_iterator part=parts.begin(); part!=parts.end(); part++) {
    // If nodes has not yet been initialized, simply copy this Part's CFGNodes to nodes
    if(!initializedNodes) {
      // Make sure to only copy if part is not NULL (not a wildcard source or destination of an edge)
      if(part->second) {
        nodes = part->second->CFGNodes();
        initializedNodes=true;
      } 
    // Otherwise, remove any nodes from node that are not in (*part)->CFGNodes()
    } else {
      set<CFGNode> partNodes=part->second->CFGNodes();
      for(set<CFGNode>::const_iterator nI=nodes.begin(); nI!=nodes.end(); ) {
        bool found=false;
        for(set<CFGNode>::const_iterator pnI=partNodes.begin(); pnI!=partNodes.end(); pnI++) {  
          if(nI==pnI) { found=true; break; }
        }
        // If the current element in nodes was not found in partNodes, erase it and move on to the next one
        if(!found) { 
          set<CFGNode>::const_iterator nINext = nI; nINext++;
          nodes.erase(nI);
          nI = nINext;
        // If it was found, just move on to the next CFGNode in nodes
        } else
          nI++;
      }
    }
  }
  
  return nodes;
}

// If this Part corresponds to a function call/return, returns the set of Parts that contain
// its corresponding return/call, respectively.
set<PartPtr> IntersectionPart::matchingCallParts() const {
  set<PartPtr> matchParts;
  bool initializedMatchParts=false; // Records whether matchParts has been initialized from one of the parts
  for(map<ComposedAnalysis*, PartPtr>::const_iterator part=parts.begin(); part!=parts.end(); part++) {
    // If nodes has not yet been initialized, simply copy this Part's matchingCallParts to matchParts
    if(!initializedMatchParts) {
      // Make sure to only copy if part is not NULL (not a wildcard source or destination of an edge)
      if(part->second) {
        matchParts = part->second->matchingCallParts();
        initializedMatchParts = true;
      }
    // Otherwise, remove any nodes from node that are not in (*part)->CFGNodes()
    } else {
      set<PartPtr> partMatchParts=part->second->matchingCallParts();
      for(set<PartPtr>::const_iterator mpI=matchParts.begin(); mpI!=matchParts.end(); ) {
        bool found=false;
        for(set<PartPtr>::const_iterator pmpI=partMatchParts.begin(); pmpI!=partMatchParts.end(); pmpI++) {  
          if(mpI==pmpI) { found=true; break; }
        }
        // If the current element in nodes was not found in partNodes, erase it and move on to the next one
        if(!found) { 
          set<PartPtr>::const_iterator mpINext = mpI; mpINext++;
          matchParts.erase(mpI);
          mpI = mpINext;
        // If it was found, just move on to the next CFGNode in nodes
        } else
          mpI++;
      }
    }
  }
  return matchParts;
}

/*
// Let A={ set of execution prefixes that terminate at the given anchor SgNode }
// Let O={ set of execution prefixes that terminate at anchor's operand SgNode }
// Since to reach a given SgNode an execution must first execute all of its operands it must
//    be true that there is a 1-1 mapping m() : O->A such that o in O is a prefix of m(o).
// This function is the inverse of m: given the anchor node and operand as well as the
//    Part that denotes a subset of A (the function is called on this part), 
//    it returns a list of Parts that partition O.
std::list<PartPtr> IntersectionPart::getOperandPart(SgNode* anchor, SgNode* operand)
{
  list<PartPtr> accumOperandParts;
  list<PartPtr> allParts;
  getOperandPart_rec(anchor, operand, parts.begin(), accumOperandParts, allParts);
  return allParts;
}
// Recursive computation of the cross-product of the getOperandParts of all the sub-parts of this Intersection part.
// Hierarchically builds a recursion tree that contains more and more combinations of PartPtrs from the results of
// getOperandPart of different sub-parts. When the recursion tree reaches its full depth (one level per part in parts), 
// it creates an intersection the current combination of parts
// partI - refers to the current part in parts
// accumOperandParts - the list of incoming parts of the current combination of this IntersectionPart's sub-parts, 
//         upto partI
void IntersectionPart::getOperandPart_rec(SgNode* anchor, SgNode* operand,
                                          list<PartPtr>::iterator partI, list<PartPtr> accumOperandParts, 
                                          list<PartPtr>& allParts)
{
  // If we've reached the last part in parts and accumOperandParts contains all the parts for the current combination
  if(partI == parts.end())
    allParts.push_back(makePtr<IntersectionPart>(accumOperandParts));
  // If we haven't yet reached the end, recurse on all the incoming edges of the current part
  else {
    // Get this part's incoming edges
    list<PartPtr> operandParts = (*partI)->getOperandPart(anchor, operand);
    
    // Advance to the next part in parts
    partI++;
    
    // Recurse on the cross product of the ingoing edges of this part and the incoming edges of subsequent parts
    for(list<PartPtr>::iterator opP=operandParts.begin(); opP!=operandParts.end(); opP++){
      accumOperandParts.push_back(*opP);
      getOperandPart_rec(anchor, operand, partI, accumOperandParts, allParts);
      accumOperandParts.pop_back();
    }
  }
}*/

// Returns a PartEdgePtr, where the source is a wild-card part (NULLPart) and the target is this Part
PartEdgePtr IntersectionPart::inEdgeFromAny()
{
  // Collect the incoming edges from each sub-part and intersect them
  map<ComposedAnalysis*, PartEdgePtr> edges;
  for(map<ComposedAnalysis*, PartPtr>::iterator part=parts.begin(); part!=parts.end(); part++) {
    PartPtr ps = part->second;
    edges[part->first] = part->second->inEdgeFromAny();
  }
  
  return makePtr<IntersectionPartEdge>(edges, (getParent()? getParent()->inEdgeFromAny() : NULLPartEdge), analysis);
}

// Returns a PartEdgePtr, where the target is a wild-card part (NULLPart) and the source is this Part
PartEdgePtr IntersectionPart::outEdgeToAny()
{
  // Collect the outgoing edges from each sub-part and intersect them
  map<ComposedAnalysis*, PartEdgePtr> edges;
  for(map<ComposedAnalysis*, PartPtr>::iterator part=parts.begin(); part!=parts.end(); part++)
    edges[part->first] = part->second->outEdgeToAny();
  return makePtr<IntersectionPartEdge>(edges, (getParent()? getParent()->outEdgeToAny() : NULLPartEdge), analysis);
}

// Two IntersectionParts are equal if their parents and all their constituent sub-parts are equal
bool IntersectionPart::equal(const PartPtr& that_arg) const
{
  IntersectionPartPtr that = dynamicPtrCast<IntersectionPart>(that_arg);
  /*IntersectionPart copy(parts, getParent(), analysis);
  dbg << "IntersectionPart::equal("<<copy.str()<<", "<<that->str()<<")"<<endl;*/
  
  // Two intersection parts with different numbers of sub-parts are definitely not equal
  if(parts.size() != that->parts.size()) { /*dbg << "NOT EQUAL: size\n"; */return false; }
  
  // Two intersections with different parents are definitely not equal
  if(getParent() != that->getParent()) { /*dbg << "NOT EQUAL: parents\n"; */return false; }
  
  for(map<ComposedAnalysis*, PartPtr>::const_iterator thisIt=parts.begin(), thatIt=that->parts.begin();
      thisIt!=parts.end(); thisIt++) {
    assert(thisIt->first == thatIt->first);
    if(thisIt->second != thatIt->second) { /*dbg << "NOT EQUAL\n"; */return false; }
  }
  
  //dbg << "EQUAL: size\n";
  return true;
}

// Lexicographic ordering: This IntersectionPart is < that IntersectionPart if 
// - their parents are < ordered, OR
// - if this has fewer parts than that, OR
// - there exists an index i in this.parts and that.parts s.t. forall j<i. this.parts[j]==that.parts[j] and 
//   this.parts[i] < that.parts[i].
bool IntersectionPart::less(const PartPtr& that_arg) const
{
  IntersectionPartPtr that = dynamicPtrCast<IntersectionPart>(that_arg);
  /*IntersectionPart copy(parts, getParent(), analysis);
  dbg << "IntersectionPart::less("<<copy.str()<<", "<<that->str()<<")"<<endl;*/
  
  // If parents are properly ordered, use their ordering
  if(getParent() < that->getParent()) { /*dbg << "LESS-THAN: parent\n";*/ return true; }
  if(getParent() > that->getParent()) { /*dbg << "GREATER-THAN: parent\n";*/ return false; }
  
  // If this has fewer parts than that, it is ordered before it
  if(parts.size() < that->parts.size()) { /*dbg << "LESS-THAN: size\n";*/ return true; }
  // If greater number of parts, it is order afterwards
  if(parts.size() > that->parts.size()) { /*dbg << "GREATER-THAN: size\n";*/ return false; }
  
  // Keep iterating for as long as the sub-parts are equal and declare this < that if we find
  // a sub-part in this < the corresponding sub-part in that
  for(map<ComposedAnalysis*, PartPtr>::const_iterator thisIt=parts.begin(), thatIt=that->parts.begin();
      thisIt!=parts.end(); thisIt++) {
    assert(thisIt->first == thatIt->first);
    if(thisIt->second < thatIt->second) { /*dbg << "LESS-THAN\n";*/ return true; }
    else if(thisIt->second > thatIt->second) { /*dbg << "GREATER-THAN\n"; */return false; }
  }
  
  // If the lexicographic < condition was not met then this is not < than that
  //dbg << "NOT LESS THAN\n"; 
  return false;
}

std::string IntersectionPart::str(std::string indent)
{
  ostringstream oss;
  oss << "[IntersectionPart:";
  if(parts.size() > 1) oss << endl;
  for(map<ComposedAnalysis*, PartPtr>::iterator part=parts.begin(); part!=parts.end(); ) {
    if(parts.size() > 1) oss << indent << "&nbsp;&nbsp;&nbsp;&nbsp;";
    oss << (part->second? part->second->str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;") : "NULL");
    part++;
    if(part!=parts.end()) oss << endl;
  }
  oss << "]"; //", parent="<<(getParent()? getParent()->str(): "NULL")<<", analysis="<<analysis<<"]";
  return oss.str();
}

/* ################################
   ##### IntersectionPartEdge #####
   ################################ */
/*IntersectionPartEdge::IntersectionPartEdge(PartEdgePtr edge, ComposedAnalysis* analysis) :
    PartEdge(analysis)
{ edges.push_back(edge); }*/

IntersectionPartEdge::IntersectionPartEdge(const map<ComposedAnalysis*, PartEdgePtr>& edges, PartEdgePtr parent, ComposedAnalysis* analysis) : 
    PartEdge(analysis, parent), edges(edges) 
{}

// Returns the PartEdge associated with this analysis. If the analysis does not implement the partition graph
// (is not among the keys of parts), returns the parent PartEdge.
PartEdgePtr IntersectionPartEdge::getPartEdge(ComposedAnalysis* analysis)
{
  //dbg << "IntersectionPartEdge::getPartEdge(analysis="<<analysis<<" = "<<analysis->str()<<") #edges="<<edges.size()<<endl;
  for(map<ComposedAnalysis*, PartEdgePtr>::iterator e = edges.begin(); e!=edges.end(); e++)
  {
    PartEdgePtr es = e->second;
    //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;"<<e->first<< " : " << e->first->str() << " : " << es->str() << endl;
  }
  
  //for(map<ComposedAnalysis*, PartEdgePtr>::iterator edge=edges.begin(); edge!=edges.end(); edge
  //map<ComposedAnalysis*, PartEdgePtr>::iterator edge;
  if(edges.find(analysis)!=edges.end()) { /*dbg << "found edge "<<edges[analysis]->str()<<endl;*/ return edges[analysis]; }
  else { /*dbg << "not found. parent="<<getParent()->str()<<endl; */return getParent(); }
}

// Return the part that intersects the sources of all the sub-edges of this IntersectionPartEdge
PartPtr IntersectionPartEdge::source() const {
  map<ComposedAnalysis*, PartPtr> sourceParts;
  bool allNULL=true; // True if all the source parts of the sub-edges are NULL, false otherwise.
  PartPtr srcParent;
  for(map<ComposedAnalysis*, PartEdgePtr>::const_iterator e=edges.begin(); e!=edges.end(); e++) {
    PartPtr s = e->second->source();
    // Make sure that the parents of source parts are consistent
    if(e == edges.begin()) {
      if(s) {
        srcParent = s->getParent();
        allNULL=false;
      }
    } else {
      // Either all sources are NULL or none are
      assert((allNULL && !s) || (!allNULL && s));
      if(s)
        // All parents must be consistent
        assert(srcParent == s->getParent());
    }
    
    sourceParts[e->first] = s;
  }
  if(allNULL) return NULLPart;
  else        return makePtr<IntersectionPart>(sourceParts, srcParent, analysis);
}

// Return the part that intersects the targets of all the sub-edges of this IntersectionPartEdge
PartPtr IntersectionPartEdge::target() const {
  map<ComposedAnalysis*, PartPtr> targetParts;
  bool allNULL=true; // True if all the target parts of the sub-edges are NULL, false otherwise.
  PartPtr tgtParent;
  for(map<ComposedAnalysis*, PartEdgePtr>::const_iterator e=edges.begin(); e!=edges.end(); e++) {
    PartPtr t = e->second->target();
    // Make sure that the parents of source parts are consistent
    if(e == edges.begin()) {
      if(t) {
        tgtParent = t->getParent();
        allNULL=false;
      }
    } else {
      // Either all sources are NULL or none are
      assert((allNULL && !t) || (!allNULL && t));
      if(t)
        // All parents must be consistent
        assert(tgtParent == t->getParent());
    }
    
    targetParts[e->first] = t;
  }
  if(allNULL) return NULLPart;
  else        return makePtr<IntersectionPart>(targetParts, tgtParent, analysis);
}


// Let A={ set of execution prefixes that terminate at the given anchor SgNode }
// Let O={ set of execution prefixes that terminate at anchor's operand SgNode }
// Since to reach a given SgNode an execution must first execute all of its operands it must
//    be true that there is a 1-1 mapping m() : O->A such that o in O is a prefix of m(o).
// This function is the inverse of m: given the anchor node and operand as well as the
//    PartEdge that denotes a subset of A (the function is called on this PartEdge), 
//    it returns a list of PartEdges that partition O.
std::list<PartEdgePtr> IntersectionPartEdge::getOperandPartEdge(SgNode* anchor, SgNode* operand)
{
  // For each part in parts, maps the parent part of each operand part edge to the set of parts that share this parent
  map<PartEdgePtr, map<ComposedAnalysis*, set<PartEdgePtr> > > parent2OPE;
  for(map<ComposedAnalysis*, PartEdgePtr>::iterator edge=edges.begin(); edge!=edges.end(); edge++) {
    // Get this part's outgoing edges
    list<PartEdgePtr> ope = edge->second->getOperandPartEdge(anchor, operand);
    
    // Group these edges according to their common parent edge
    for(list<PartEdgePtr>::iterator e=ope.begin(); e!=ope.end(); e++)
      parent2OPE[(*e)->getParent()][edge->first].insert(*e);
  }
  
  // Create a cross-product of the edges in parent2Out, one parent edge at a time
  std::list<PartEdgePtr> edges;
  for(map<PartEdgePtr, map<ComposedAnalysis*, set<PartEdgePtr> > >::iterator par=parent2OPE.begin(); 
      par!=parent2OPE.end(); par++) {
    map<ComposedAnalysis*, PartEdgePtr> opePartEdges;
    intersectEdges(par->first, par->second.begin(), par->second, opePartEdges, edges, analysis);
  }
  return edges;
}


/*std::list<PartEdgePtr> IntersectionPartEdge::getOperandPartEdge(SgNode* anchor, SgNode* operand)
{
  list<PartEdgePtr> accumOperandPartEdges;
  list<PartEdgePtr> allPartEdges;
  getOperandPartEdge_rec(anchor, operand, edges.begin(), accumOperandPartEdges, allPartEdges);
  return allPartEdges;
}

// Recursive computation of the cross-product of the getOperandParts of all the sub-part edges of this Intersection part edge.
// Hierarchically builds a recursion tree that contains more and more combinations of PartEdgePtrs from the results of
// getOperandPart of different sub-part edges. When the recursion tree reaches its full depth (one level per edge in edges), 
// it creates an intersection the current combination of edges.
// edgeI - refers to the current edge in edges
// accumOperandPartEdges - the list of incoming edgesof the current combination of this IntersectionPartEdges's sub-Edges, 
//         upto edgeI
void IntersectionPartEdge::getOperandPartEdge_rec(SgNode* anchor, SgNode* operand,
                                                  list<PartEdgePtr>::iterator edgeI, list<PartEdgePtr> accumOperandPartEdges, 
                                                  list<PartEdgePtr>& allPartEdges)
{
  // If we've reached the last edge in edges and accumOperandPartEdges contains all the edges for the current combination
  if(edgeI == edges.end())
    allPartEdges.push_back(makePtr<IntersectionPartEdge>(accumOperandPartEdges, analysis));
  // If we haven't yet reached the end, recurse on all the incoming edges of the current edge
  else {
    // Get this edge's incoming edges
    list<PartEdgePtr> operandPartEdges = (*edgeI)->getOperandPartEdge(anchor, operand);
    
    // Advance to the next edge in edges
    edgeI++;
    
    // Recurse on the cross product of the ingoing edges of this edge and the incoming edges of subsequent edges
    for(list<PartEdgePtr>::iterator opP=operandPartEdges.begin(); opP!=operandPartEdges.end(); opP++){
      accumOperandPartEdges.push_back(*opP);
      getOperandPartEdge_rec(anchor, operand, edgeI, accumOperandPartEdges, allPartEdges);
      accumOperandPartEdges.pop_back();
    }
  }
}*/

// If the source Part corresponds to a conditional of some sort (if, switch, while test, etc.)
// it must evaluate some predicate and depending on its value continue, execution along one of the
// outgoing edges. The value associated with each outgoing edge is fixed and known statically.
// getPredicateValue() returns the value associated with this particular edge. Since a single 
// Part may correspond to multiple CFGNodes getPredicateValue() returns a map from each CFG node
// within its source part that corresponds to a conditional to the value of its predicate along 
// this edge. 
std::map<CFGNode, boost::shared_ptr<SgValueExp> > IntersectionPartEdge::getPredicateValue()
{
  if(source()) {
    // The set of CFGNodes for which we'll create a value mapping since these nodes exist
    // in all the sub-edges of this IntersectionPartEdge
    set<CFGNode> srcNodes = source()->CFGNodes();

    map<CFGNode, boost::shared_ptr<SgValueExp> > pv;
    // Consider the predicate->value mappings of all the sub-edges
    for(map<ComposedAnalysis*, PartEdgePtr>::iterator e=edges.begin(); e!=edges.end(); e++) {
      map<CFGNode, boost::shared_ptr<SgValueExp> > epv = e->second->getPredicateValue();
      // Consider the values mapped under all the CFG nodes of this sub-edge's source part
      for(map<CFGNode, boost::shared_ptr<SgValueExp> >::iterator v=epv.begin(); v!=epv.end(); v++) {
        // Skip CFGNodes that are not shared but all the sources of all the sub-edges
        if(srcNodes.find(v->first) == srcNodes.end()) { continue; }
        
        // If a value mapping for the current CFGNode of the current sub-edge has already 
        // been observed from another sub-edge, make sure that the mapped values are the same
        if(pv.find(v->first) != pv.end())
          assert(ValueObject::equalValueExp(pv[v->first].get(), v->second.get()));
        else
          pv[v->first] = v->second;
      }
    }
    
    return pv;
  } else {
    std::map<CFGNode, boost::shared_ptr<SgValueExp> > empty;
    return empty;
  }
}

// Two IntersectionPartEdges are equal of all their constituent sub-parts are equal
bool IntersectionPartEdge::equal(const PartEdgePtr& o) const
{
  IntersectionPartEdgePtr that = dynamicPtrCast<IntersectionPartEdge>(o);
  /*IntersectionPartEdge copy(edges, getParent(), analysis);
  dbg << "IntersectionPartEdge::equal("<<copy.str()<<", "<<that->str()<<")"<<endl;*/
  // Two intersection parts with different numbers of sub-parts are definitely not equal
  if(edges.size() != that->edges.size()) { /*dbg << "NOT EQUAL: size\n"; */return false; }
  
  // Two intersections with different parents are definitely not equal
  if(getParent() != that->getParent()) { /*dbg << "NOT EQUAL: parents\n"; */return false; }
  
  for(map<ComposedAnalysis*, PartEdgePtr>::const_iterator thisIt=edges.begin(), thatIt=that->edges.begin();
      thisIt!=edges.end(); thisIt++) {
    if(*thisIt != *thatIt) { /*dbg << "NOT EQUAL\n"; */return false; }
  }
  
  //dbg << "EQUAL\n";
  return true;
}

// Lexicographic ordering: This IntersectionPartEdge is < that IntersectionPartEdge if this has fewer edges than that or
// there exists an index i in this.edges and that.edges s.t. forall j<i. this.edges[j]==that.edges[j] and 
// this.edges[i] < that.edges[i].
bool IntersectionPartEdge::less(const PartEdgePtr& o) const
{
  IntersectionPartEdgePtr that = dynamicPtrCast<IntersectionPartEdge>(o);
  /*IntersectionPartEdge copy(edges, getParent(), analysis);
  dbg << "IntersectionPartEdge::less("<<copy.str()<<", "<<that->str()<<")"<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;getParent()="<<getParent()->str()<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;that->getParent()="<<that->getParent()->str()<<endl;
  dbg << "&nbsp;&nbsp;&nbsp;&nbsp;< "<<(getParent() < that->getParent())<<", == "<<(getParent() == that->getParent())<<", > "<<(getParent() > that->getParent())<<endl;
  */
  // If parents are properly ordered, use their ordering
  if(getParent() < that->getParent()) { /*dbg << "LESS-THAN: parent\n";*/ return true; }
  if(getParent() > that->getParent()) { /*dbg << "GREATER-THAN: parent\n";*/ return false; }
  
  // If this has fewer edges than that, it is ordered before it
  if(edges.size() < that->edges.size()) { /*dbg << "LESS-THAN: size\n";*/ return true; } 
  // If greater number of edges, it is order afterwards
  if(edges.size() > that->edges.size()) { /*dbg << "GREATER-THAN: size\n";*/ return false; }
  
  // Keep iterating for as long as the sub-edges are equal and declare this < that if we find
  // a sub-edge in this < the corresponding sub-edge in that
  for(map<ComposedAnalysis*, PartEdgePtr>::const_iterator thisIt=edges.begin(), thatIt=that->edges.begin();
      thisIt!=edges.end(); thisIt++) {
    if(*thisIt < *thatIt) { /*dbg << "LESS-THAN\n"; */return true; }
    else if(*thisIt > *thatIt) { /*dbg << "GREATER-THAN\n"; */return false; }
  }
  
  // If the lexicographic < condition was not met then this is not < than that
  /*dbg << "NOT LESS THAN\n"; */
  return false;
}

// Recursive function that uses a binary tree to check the equality of the remapping functors.
// Returns whether the functors between iterators start and end are equal to each other.
// numElts is the total number of map elements between start and end (not including end)
bool IntersectionPartEdge::isEqualRemap
                 (map<ComposedAnalysis*, PartEdgePtr>::const_iterator start,
                  map<ComposedAnalysis*, PartEdgePtr>::const_iterator end,
                  int numElts) const
{
  if(numElts<=1) return true;
  else if(numElts==2) {
    map<ComposedAnalysis*, PartEdgePtr>::const_iterator next = start; next++;
    return start->second->getRemap() == next->second->getRemap();
  } else {
    // Divide the current scope of the map into two almost equal halves
    map<ComposedAnalysis*, PartEdgePtr>::const_iterator cur = start;
    // Advance cur to the midpoint of the scope [start, end)
    for(int i=0; i<numElts/2; i++, cur++) {}

    return isEqualRemap(start, cur, numElts/2) &&
           isEqualRemap(cur, end, numElts-numElts/2);
  }
}

// Remaps the given Lattice as needed to take into account any function call boundaries.
// Remapping is performed both in the forwards and backwards directions. 
// Returns the resulting Lattice object, which is freshly allocated.
// Since the function is called for the scope change across some Part, it needs to account for the fact that
//    some MemLocs are in scope on one side of Part, while others are in scope on the other side. 
//    fromPEdge is the edge from which control is passing and the current PartEdge (same as the PartEdge of 
//    the Lattice) is the one to which control is passing.
Lattice* IntersectionPartEdge::forwardRemapML(Lattice* lat, PartEdgePtr fromPEdge) 
{
  assert(edges.size()>0);
  // Confirm that all the sub-edges use the same remapping functors
  assert(isEqualRemap(edges.begin(), edges.end(), edges.size()));
  assert(makePtrFromThis(shared_from_this()) == lat->getPartEdge());

  // Since all the edges have the same remapper, call the remapper of the first one
  return edges.begin()->second->forwardRemapML(lat, fromPEdge);
}

Lattice* IntersectionPartEdge::backwardRemapML(Lattice* lat, PartEdgePtr fromPEdge) 
{ 
  assert(edges.size()>0);
  // Confirm that all the sub-edges use the same remapping functors
  assert(isEqualRemap(edges.begin(), edges.end(), edges.size()));
  assert(makePtrFromThis(shared_from_this()) == lat->getPartEdge());
  
  // Since all the edges have the same remapper, call the remapper of the first one
  return edges.begin()->second->backwardRemapML(lat, fromPEdge);
}

std::string IntersectionPartEdge::str(std::string indent)
{
  ostringstream oss;
  oss << "[IntersectionPartEdge:";
  if(edges.size() > 1) oss << endl;
  for(map<ComposedAnalysis*, PartEdgePtr>::iterator edge=edges.begin(); edge!=edges.end(); ) {
    if(edges.size() > 1) oss << indent << "&nbsp;&nbsp;&nbsp;&nbsp;";
    oss << edge->second->str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;");
    edge++;
    if(edge!=edges.end()) oss << endl;
  }
  oss << "]"; //", parent="<<(getParent()? getParent()->str(): "NULL")<<", analysis="<<analysis<<"]";
  return oss.str();
}

}; // namespace fuse
