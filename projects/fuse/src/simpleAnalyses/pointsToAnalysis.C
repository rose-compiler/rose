#include "sage3basic.h"
#include "pointsToAnalysis.h"

using namespace dbglog;
namespace fuse
{
  int ptaDebugLevel = 2;

  PointsToAnalysisTransfer::PointsToAnalysisTransfer(PartPtr part,
                                                     CFGNode cn, NodeState& state,
                                                     std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo,
                                                     Composer* _composer, PointsToAnalysis* _analysis, const int& _debugLevel)
    :DFTransferVisitor(part, cn, state, dfInfo),
     composer(_composer),
     analysis(_analysis),
     modified(false),
     debugLevel(_debugLevel)
  {
    // set the pointer of the map for this PartEdge
    setProductLattice();
  }

  bool PointsToAnalysisTransfer::finish()
  {
    return modified;
  }

  void PointsToAnalysisTransfer::setProductLattice()
  { 
    // copied from VariableStateTransfer.h
    assert(dfInfo.size()==1);
    assert(dfInfo[NULLPartEdge].size()==1);
    assert(*dfInfo[NULLPartEdge].begin());
    Lattice *l = *dfInfo[NULLPartEdge].begin();
    productLattice = (dynamic_cast<AbstractObjectMap*>(l));
    assert(productLattice);
  }

  // NOTE: Should we use pedge instead of part->inEdgeFromAny()
  PointsToAnalysisTransfer::AbstractObjectSetPtr 
  PointsToAnalysisTransfer::getLattice(SgExpression* sgexp) 
  {
    MemLocObjectPtr ml= composer->Expr2MemLoc(sgexp, part->inEdgeFromAny(), analysis);
    return getLatticeCommon(ml);
  }

  // NOTE: Should we use pedge instead of part->inEdgeFromAny()
  PointsToAnalysisTransfer::AbstractObjectSetPtr 
  PointsToAnalysisTransfer::getLatticeOperand(SgNode* sgn, SgExpression* operand) 
  { 
    MemLocObjectPtr oml = composer->OperandExpr2MemLoc(sgn, operand, part->inEdgeFromAny(), analysis); 
    return getLatticeCommon(oml);
  }

  PointsToAnalysisTransfer::AbstractObjectSetPtr 
  PointsToAnalysisTransfer::getLatticeCommon(MemLocObjectPtr p)
  {
    return getLattice(AbstractObjectPtr(p));
  }

  PointsToAnalysisTransfer::AbstractObjectSetPtr 
  PointsToAnalysisTransfer::getLattice(const AbstractObjectPtr o)
  {
    AbstractObjectSetPtr _aos = boost::dynamic_pointer_cast<AbstractObjectSet> (productLattice->get(o));
    assert(_aos);
    dbg << "getLattice(o): o=" << o->strp(part->inEdgeFromAny()) << ", lattice=" << _aos->strp(part->inEdgeFromAny()) << "\n";
    return _aos;
  }

  void PointsToAnalysisTransfer::setLattice(SgExpression* sgexp,
                                            PointsToAnalysisTransfer::AbstractObjectSetPtr aos)
  {
    MemLocObjectPtr ml = composer->Expr2MemLoc(sgexp, part->inEdgeFromAny(), analysis);
    setLatticeCommon(ml, aos);
  }

  void PointsToAnalysisTransfer::setLatticeOperand(SgNode* sgn, SgExpression* operand,
                                                   PointsToAnalysisTransfer::AbstractObjectSetPtr aos)
  {
    MemLocObjectPtr ml = composer->OperandExpr2MemLoc(sgn, operand, part->inEdgeFromAny(), analysis);
    setLatticeCommon(ml, aos);
  }

  void PointsToAnalysisTransfer::setLatticeCommon(MemLocObjectPtr ml,
                                                  PointsToAnalysisTransfer::AbstractObjectSetPtr aos)
  {
    setLattice(AbstractObjectPtr(ml), aos);
  }

  void PointsToAnalysisTransfer::setLattice(const AbstractObjectPtr o,
                                            PointsToAnalysisTransfer::AbstractObjectSetPtr aos)
  {
    modified = modified || productLattice->insert(o, aos);
    if(debugLevel >= 2) {
      dbg << productLattice->strp(part->inEdgeFromAny());
    }
  }

  // very trivial transfer function
  // NOTE: requires extension for a full blown analysis
  void PointsToAnalysisTransfer::visit(SgAssignOp* sgn)
  {
    SgExpression* rhs_operand = sgn->get_rhs_operand();
    SgExpression* lhs_operand = sgn->get_lhs_operand();
    // handle p = &x
    // NOTE: rhs can be a complex expression but code below only handles the trivial case
    if(isSgAddressOfOp(rhs_operand)) 
    {
      // operand of SgAddressOfOp should be a variable
      SgVarRefExp* sgvexp = isSgVarRefExp(isSgAddressOfOp(rhs_operand)->get_operand()); assert(sgvexp);    
      MemLocObjectPtr _ml = composer->OperandExpr2MemLoc(rhs_operand, sgvexp, part->inEdgeFromAny(), analysis); assert(_ml);

      // get the lattice for lhs_operand
      // insert memory object for rhs_operand into this lattice
      AbstractObjectSetPtr aos = getLatticeOperand(sgn, lhs_operand);
      aos->insert(_ml);
      // update the product lattice
      setLatticeOperand(sgn, lhs_operand, aos);
      setLattice(sgn, aos);
      modified = true;
    }
    //TODO: handle p = q
    else if(isSgPointerType(lhs_operand->get_type()) &&
            isSgPointerType(rhs_operand->get_type()))
    {
      AbstractObjectSetPtr l_aos = getLatticeOperand(sgn, lhs_operand); 
      AbstractObjectSetPtr r_aos = getLatticeOperand(sgn, rhs_operand); 
      // union the information
      // NOTE: points to information can be NULL
      // merge pointsToSet from rhs if available
      if(l_aos && r_aos) l_aos->meetUpdate(dynamic_cast<Lattice*>(r_aos.get()));
      // pointsToSet is empty for lhs, populate it with rhs information
      if(!l_aos && r_aos) l_aos = boost::make_shared<AbstractObjectSet>(*r_aos);
      // set the map with this pointsToSet
      setLatticeOperand(sgn, lhs_operand, l_aos);
      setLattice(sgn, l_aos);
      modified = true;
    }
  }

  void PointsToAnalysis::genInitLattice(PartPtr part, PartEdgePtr pedge,
                                        std::vector<Lattice*>& initLattices)
  {
    AbstractObjectMap* productlattice = new AbstractObjectMap(boost::make_shared<AbstractObjectSet>(pedge, 
                                                                                                    getComposer(), 
                                                                                                    this, 
                                                                                                    AbstractObjectSet::may),
                                                              pedge,
                                                              getComposer(),
                                                              this);
    initLattices.push_back(productlattice);                                                                                                  
  }


  boost::shared_ptr<DFTransferVisitor>
  PointsToAnalysis::getTransferVisitor(PartPtr _part, CFGNode cn, NodeState& state, 
                                       std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo)                                     
  {
    PointsToAnalysisTransfer* idftv = new PointsToAnalysisTransfer(_part, cn, state, dfInfo, getComposer(), this, ptaDebugLevel);
    return boost::shared_ptr<DFTransferVisitor>(idftv);
  }

  std::string PointsToAnalysis::str(std::string indent="")
  { 
    return "PointsToAnalysis"; 
  }

  void PointsToAnalysis::copyAbstractObjectSet(const AbstractObjectSet& aos, std::list<MemLocObjectPtr>& list)
  {
    for(AbstractObjectSet::const_iterator it = aos.begin(); it != aos.end(); it++)
    {
      list.push_back(boost::dynamic_pointer_cast<MemLocObject> (*it));
    }
  }

  boost::shared_ptr<AbstractObjectSet> 
  PointsToAnalysis::getPointsToSet(SgNode* sgn, PartEdgePtr pedge, AbstractObjectMap* aom)
  {
    // use the visitor pattern to get pointsToSet for arbitrary sgn
    Expr2MemLocTraversal e2mlt(composer, this, pedge, aom);
    sgn->accept(e2mlt);
    boost::shared_ptr<AbstractObjectSet> aos = e2mlt.getPointsToSet();
    return aos;
  }

  PointsToMLPtr PointsToAnalysis::Expr2PointsToMLPtr(SgNode* sgn, PartEdgePtr pedge, boost::shared_ptr<AbstractObjectSet> aos)
  {
    // we have points to set for the current MemLocObject
    // wrap it up by PointsToML
    std::list<MemLocObjectPtr> pointsToSet;
    //NOTE: It can be empty if no entry was found in AbstractObjectMap
    if(aos.get())
    {
      // copy the elements of aos into pointsToSet which will be wrapped by PoinsToML
      copyAbstractObjectSet(*aos, pointsToSet);
    }
    else
    {
      // wrap other memory objects that don't point to any memory object
      // push the ml for sgn itself into the pointsToSet as it does not points to any other object
      MemLocObjectPtr ml = composer->Expr2MemLoc(sgn, pedge, this);
      pointsToSet.push_back(ml);
    }
    PointsToMLPtr ptmlp = boost::make_shared<PointsToML> (pointsToSet);
    return ptmlp;
  }

  MemLocObjectPtr PointsToAnalysis::Expr2MemLoc(SgNode* sgn, PartEdgePtr pedge)
  {
    scope reg(txt()<<"PointsToAnalysis::Expr2MemLoc(sgn=" << SgNode2Str(sgn) << ")", scope::medium, ptaDebugLevel, 1);
    if(ptaDebugLevel>=1) dbg << "pedge=" << pedge->str() << endl;

    // NOTE: source and target of edge are not wildcard
    if(pedge->source() && pedge->target())
    {
      NodeState* state = NodeState::getNodeState(this, pedge->source());
      if(ptaDebugLevel>=1) dbg << "state="<<state->str(this)<<endl;
      AbstractObjectMap* aom = dynamic_cast<AbstractObjectMap*>(state->getLatticeBelow(this, pedge, 0));
      assert(aom);
      boost::shared_ptr<AbstractObjectSet> aos = getPointsToSet(sgn, pedge, aom);
      return boost::dynamic_pointer_cast<MemLocObject>(Expr2PointsToMLPtr(sgn, pedge, aos));
    }    
    // NOTE: merge information across all outgoing edges
    // target of this edge is wildcard
    else if(pedge->source()) 
    { 
      NodeState* state = NodeState::getNodeState(this, pedge->source());
      //dbg << "state="<<state->str(this)<<endl;
    
      // Merge the lattices along all the outgoing edges
      map<PartEdgePtr, std::vector<Lattice*> >& e2lats = state->getLatticeBelowAllMod(this);
      assert(e2lats.size()>=1);
      boost::shared_ptr<AbstractObjectSet> mergedSet = boost::make_shared<AbstractObjectSet>(pedge, getComposer(), this, AbstractObjectSet::may);
      for(map<PartEdgePtr, std::vector<Lattice*> >::iterator lats=e2lats.begin(); lats!=e2lats.end(); lats++) 
      {
        PartEdge* edgePtr = lats->first.get();
        assert(edgePtr->source() == pedge.get()->source());
      
        AbstractObjectMap* aom = dynamic_cast<AbstractObjectMap*>(state->getLatticeBelow(this, lats->first, 0));
        assert(aom);
        if(ptaDebugLevel>=1) dbg << "aom="<<aom->str()<<endl;
        boost::shared_ptr<AbstractObjectSet> aos = getPointsToSet(sgn, pedge, aom);
        // NOTE: It can be empty if no entry was found in AbstractObjectMap
        // Safe approximation : merge only if it contains pointsTo information along this particular edge
        // 
        if(aos.get())
          mergedSet->meetUpdate(aos.get());
      }
      return boost::dynamic_pointer_cast<MemLocObject>(Expr2PointsToMLPtr(sgn, pedge, mergedSet));
    }
    // source of this edge is a wildcard
    else if(pedge->target()) 
    {
      NodeState* state = NodeState::getNodeState(this, pedge->target());
      if(ptaDebugLevel>=1) dbg << "state="<<state->str()<<endl;
      AbstractObjectMap* aom = dynamic_cast<AbstractObjectMap*>(state->getLatticeAbove(this, NULLPartEdge, 0));
      assert(aom);
      boost::shared_ptr<AbstractObjectSet> aos = getPointsToSet(sgn, pedge, aom);
      return boost::dynamic_pointer_cast<MemLocObject>(Expr2PointsToMLPtr(sgn, pedge, aos));
    }
    else { assert(false); return PointsToMLPtr(); }
  }

  void Expr2MemLocTraversal::visit(SgPointerDerefExp* sgn)
  {
    scope regvis("Expr2MemLocTraversal::visit(SgPointerDerefExp* sgn)", scope::medium, ptaDebugLevel, 1);
    SgExpression* operand = sgn->get_operand();
    operand->accept(*this);
    boost::shared_ptr<AbstractObjectSet> new_p_aos = 
            boost::make_shared<AbstractObjectSet>(pedge, composer, analysis, AbstractObjectSet::may);
    for(AbstractObjectSet::const_iterator i=p_aos->begin(); i!=p_aos->end(); i++) {
      boost::shared_ptr<AbstractObjectSet> ao = boost::dynamic_pointer_cast<AbstractObjectSet>(aom->get(*i));
      assert(ao);
      new_p_aos->meetUpdate(ao.get());
    }
    p_aos = new_p_aos;
  }

  void Expr2MemLocTraversal::visit(SgVarRefExp* sgn)
  {
    scope regvis("Expr2MemLocTraversal::visit(SgVarRefExp* sgn)", scope::medium, ptaDebugLevel, 1);
    dbg << "isSgPointerType(sgn->get_type())="<<isSgPointerType(sgn->get_type())<<endl;
    // return points to set only for pointer types
    /*if(isSgPointerType(sgn->get_type()))
    {
      MemLocObjectPtr ml = composer->Expr2MemLoc(sgn, pedge, analysis);
      p_aos = boost::dynamic_pointer_cast<AbstractObjectSet>(aom->get(ml));
    }*/
    p_aos = boost::make_shared<AbstractObjectSet>(pedge, composer, analysis, AbstractObjectSet::may);
    p_aos->insert(composer->Expr2MemLoc(sgn, pedge, analysis));
    if(ptaDebugLevel>=1) dbg << "p_aos="<<p_aos->str()<<endl;
  }

  void Expr2MemLocTraversal::visit(SgAssignOp* sgn)
  {
    // handle p = q where p, q are pointer types    
  }
};
