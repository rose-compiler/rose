#ifndef COPY_ARRAY_ANAL
#define COPY_ARRAY_ANAL

#include <DepCompTransform.h>
#include <LoopTransformOptions.h>

class CopyArrayOperator
{
  virtual void ModifyCopyArrayCollect(LoopTransformInterface& li,
                               DepCompCopyArrayCollect& collect, DepCompAstRefGraphCreate& g) 
        {}
public:

  int EnforceCopyDimension( DepCompCopyArrayCollect::CopyArrayUnit& unit, 
                               DepCompAstRefGraphCreate& refDep, int copydim,
                               DepCompCopyArrayCollect::CopyArrayUnit::NodeSet* cuts = 0);
  bool SplitDisconnectedUnit( DepCompCopyArrayCollect& collect,
                              DepCompCopyArrayCollect::CopyArrayUnit& unit,
                              DepCompAstRefGraphCreate& g,                        
                               DepCompCopyArrayCollect::CopyArrayUnit::NodeSet& cuts);
  bool IsRedundantCopy( LoopTransformInterface& li, 
                             DepCompCopyArrayCollect::CopyArrayUnit& unit, int copydim);
  int OutmostReuseLevel ( DepCompCopyArrayCollect::CopyArrayUnit& unit, 
                            DepCompAstRefGraphCreate& refDep);

  virtual LoopTransformOptions::OptType GetOptimizationType()  = 0;
  virtual void operator()(LoopTransformInterface& la, LoopTreeLocalityAnal& tc, LoopTreeNode* root);
  virtual ~CopyArrayOperator() {}
};

class NoCopyArrayOperator : public CopyArrayOperator
{
 public:
  void operator()(LoopTransformInterface& la, LoopTreeLocalityAnal& tc) {}
  virtual LoopTransformOptions::OptType GetOptimizationType() 
       { return LoopTransformOptions::NO_OPT; }
};


class CopyArrayUnderSizeLimit : public CopyArrayOperator
{
   unsigned copydim;
  public:
   CopyArrayUnderSizeLimit(unsigned sz) : copydim(sz) {}
   void ModifyCopyArrayCollect(LoopTransformInterface& li,
                               DepCompCopyArrayCollect& collect, DepCompAstRefGraphCreate& g);
  virtual LoopTransformOptions::OptType GetOptimizationType() 
       { return LoopTransformOptions::DATA_OPT; }
};
                                                                                                

#endif
