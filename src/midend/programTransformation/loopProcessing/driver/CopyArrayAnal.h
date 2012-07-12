#ifndef COPY_ARRAY_ANAL
#define COPY_ARRAY_ANAL

#include <DepCompTransform.h>
#include <LoopTransformOptions.h>

class CopyArrayOperator : public DepCompCopyArrayToBuffer
{
 /*QY: modify the collected array copy specifications in (collect)  */
  virtual void ModifyCopyArrayCollect( DepCompCopyArrayCollect& collect, 
                DepCompAstRefGraphCreate& g) {}
  DepCompCopyArrayToBuffer::ApplyXform;
public:
  virtual ~CopyArrayOperator() {}
  virtual LoopTransformOptions::OptType GetOptimizationType()  = 0;

  /* QY: Modifies (unit.root) to Ensure at most (copydim) loop dimensions are copied for the given (unit) */
  int EnforceCopyDimension( DepCompCopyArrayCollect::CopyArrayUnit& unit, 
             DepCompAstRefGraphCreate& refDep, int copydim,
             DepCompCopyArrayCollect::CopyArrayUnit::NodeSet* cuts = 0);
  bool SplitDisconnectedUnit( DepCompCopyArrayCollect& collect,
             DepCompCopyArrayCollect::CopyArrayUnit& unit,
             DepCompAstRefGraphCreate& g,                        
             DepCompCopyArrayCollect::CopyArrayUnit::NodeSet& cuts);
  bool IsRedundantCopy(DepCompCopyArrayCollect::CopyArrayUnit& unit, int copydim);
  int OutmostReuseLevel ( DepCompCopyArrayCollect::CopyArrayUnit& unit, 
             DepCompAstRefGraphCreate& refDep);

  void operator()(LoopTreeLocalityAnal& tc, 
                  LoopTreeNode* root);
};

class NoCopyArrayOperator : public CopyArrayOperator
{
  virtual void ApplyXform( DepCompCopyArrayCollect::CopyArrayUnit& curarray,
                CopyArrayConfig& config, LoopTreeNode* replRoot,
                LoopTreeNode* initStmt, LoopTreeNode* saveStmt) {}
 public:
  virtual LoopTransformOptions::OptType GetOptimizationType() 
       { return LoopTransformOptions::NO_OPT; }
};


class CopyArrayUnderSizeLimit : public CopyArrayOperator
{
  protected:
   unsigned copydim;
  public:
   CopyArrayUnderSizeLimit(unsigned sz) : copydim(sz) {}
   virtual void ModifyCopyArrayCollect( DepCompCopyArrayCollect& collect, DepCompAstRefGraphCreate& g);
   virtual LoopTransformOptions::OptType GetOptimizationType() 
       { return LoopTransformOptions::DATA_OPT; }
};

class ParameterizeCopyArray : public CopyArrayUnderSizeLimit
{
/*
  struct LoopInfo {
     std::vector<LoopTreeNode*> loops;
     bool canbeBlocked;
     LoopInfo() : canbeBlocked(false) {}
  }
  std::map<DepCompCopyArrayCollect::CopyArrayUnit*,LoopInfo> copyInfo; 
*/
  /* set of copied arrays that can be blocked*/ 
/*
  std::set<DepCompCopyArrayCollect::CopyArrayUnit*> blockInfo;
*/

  virtual void ModifyCopyArrayCollect( DepCompCopyArrayCollect& collect, DepCompAstRefGraphCreate& g); 
  virtual void ApplyXform( DepCompCopyArrayCollect::CopyArrayUnit& curarray,
                CopyArrayConfig& config, LoopTreeNode* replRoot,
                LoopTreeNode* initStmt, LoopTreeNode* saveStmt); 
  bool CanBeBlocked( DepCompCopyArrayCollect::CopyArrayUnit& unit, int copydim);
  public:
  ParameterizeCopyArray(unsigned sz) : CopyArrayUnderSizeLimit(sz) {}
  virtual LoopTransformOptions::OptType GetOptimizationType() 
       { return LoopTransformOptions::DATA_OPT; }
};
                                                                                                

#endif
