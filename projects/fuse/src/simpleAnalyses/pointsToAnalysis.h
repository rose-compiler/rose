#ifndef _POINTSTOANALYSIS_H
#define _POINTSTOANALYSIS_H

/*
 * Simple  PointsTo Analysis
 * Lattice - AbstractObjectSet
 * AbstractObjectMap (ProductLattice) stores an AbstractObjectSet for each MemLocObjectPtr
 * author: sriram@cs.utah.edu
 */

#include "compose.h"
#include "abstract_object_map.h"
#include "abstract_object_set.h"

namespace fuse
{
  extern int ptaDebugLevel;

  class PointsToAnalysis;
  // Transfer functions for the PointsTo analysis
  class PointsToAnalysisTransfer : public DFTransferVisitor
  {
    typedef boost::shared_ptr<AbstractObjectSet> AbstractObjectSetPtr;
    Composer* composer;
    PointsToAnalysis* analysis;
    // pointer to node state of the analysis at this part
    AbstractObjectMap* productLattice;
    // used by the analysis to determine if the states modified or not
    bool modified;
    int debugLevel;
  public:
    PointsToAnalysisTransfer(PartPtr part, CFGNode cn, NodeState& state,
                             std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo,
                             Composer* composer, PointsToAnalysis* analysis, const int& _debugLevel);

    // set the pointer of AbstractObjectMap at this PartEdge
    void setProductLattice();

    bool finish();

    // lattice access functions from the map (product lattice)
    // copied from VariableStateTransfer.h
    AbstractObjectSetPtr getLattice(SgExpression* sgexp);
    AbstractObjectSetPtr getLatticeOperand(SgNode* sgn, SgExpression* operand);
    AbstractObjectSetPtr getLatticeCommon(MemLocObjectPtr ml);
    AbstractObjectSetPtr getLattice(const AbstractObjectPtr o);
    void setLattice(SgExpression* sgexp, AbstractObjectSetPtr aos);
    void setLatticeOperand(SgNode* sgn, SgExpression* operand, AbstractObjectSetPtr aos);
    void setLatticeCommon(MemLocObjectPtr ml, AbstractObjectSetPtr aos);
    void setLattice(const AbstractObjectPtr o, AbstractObjectSetPtr aos);

    // Transfer functions
    void visit(SgAssignOp* sgn);
  };

  // See definition below
  class PointsToML;
  typedef boost::shared_ptr<PointsToML> PointsToMLPtr;

  class PointsToAnalysis : public virtual FWDataflow
  {
  public:
    PointsToAnalysis() { }
    
    // Returns a shared pointer to a freshly-allocated copy of this ComposedAnalysis object
    ComposedAnalysisPtr copy() { return boost::make_shared<PointsToAnalysis>(); }

    void genInitLattice(PartPtr part, PartEdgePtr pedge,
                        std::vector<Lattice*>& initLattices);

    bool transfer(PartPtr part, CFGNode cn, NodeState& state, 
                  std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo) { assert(false); return false; }

    boost::shared_ptr<DFTransferVisitor> 
      getTransferVisitor(PartPtr part, CFGNode cn, NodeState& state, 
                         std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo);
    
    // functions called by composer
    MemLocObjectPtr Expr2MemLoc(SgNode* sgn, PartEdgePtr pedge);

    std::string str(std::string indent); 

    friend class PointsToAnalysisTransfer;

    // helper function to copy elements from abstract object set
    void copyAbstractObjectSet(const AbstractObjectSet& aos, std::list<MemLocObjectPtr>& list);

    // get the pointsToSet from the given map
    boost::shared_ptr<AbstractObjectSet> getPointsToSet(SgNode* sgn, PartEdgePtr pedge, AbstractObjectMap *aom);

    // wrap the given set PointsToMLPtr
    PointsToMLPtr Expr2PointsToMLPtr(SgNode* sgn, PartEdgePtr pedge, boost::shared_ptr<AbstractObjectSet> aom);
  };

  // used to handle Expr2MemLoc queries by the composer
  // for various SgNode
  class Expr2MemLocTraversal : public ROSE_VisitorPatternDefaultBase
  {
    Composer* composer;
    PointsToAnalysis* analysis;
    PartEdgePtr pedge;
    AbstractObjectMap* aom;
    // returned by this class for a given SgNode*
    boost::shared_ptr<AbstractObjectSet> p_aos;
  public:
    Expr2MemLocTraversal(Composer* _composer, 
                         PointsToAnalysis* _analysis,
                         PartEdgePtr _pedge, 
                         AbstractObjectMap* _aom) : 
    composer(_composer), 
    analysis(_analysis), 
    pedge(_pedge), aom(_aom), 
    p_aos(boost::shared_ptr<AbstractObjectSet>()) { }
    void visit(SgPointerDerefExp* sgn);
    void visit(SgVarRefExp* sgn);
    void visit(SgAssignOp* sgn);
    boost::shared_ptr<AbstractObjectSet> getPointsToSet() { return p_aos; }
  };

  // Object returned by PointsToAnalysis::Expr2MemLoc
  // Wraps object returned by the composer
  class PointsToML : public UnionMemLocObject
  {
    // NOTE: UnionMemLocObject is useful to store list
    // of items a memory object may point to

  public:
    PointsToML(MemLocObjectPtr _mem) : MemLocObject(NULL), UnionMemLocObject(_mem) { }
    PointsToML(std::list<MemLocObjectPtr> _lml) : MemLocObject(NULL), UnionMemLocObject(_lml) { }       
        
    std::string str(std::string indent)
    {
      std::ostringstream oss;
      oss << "[PointsToML: " << UnionMemLocObject::str(indent) << "]";
      return oss.str();
    }
  };
};

#endif
