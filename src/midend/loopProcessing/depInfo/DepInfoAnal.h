
#ifndef DEP_INFO_ANAL
#define DEP_INFO_ANAL

#include <map>
#include <DepInfo.h>
#include <DomainInfo.h>
#include <FunctionObject.h>
#include <StmtInfoCollect.h>
#include <LoopTransformInterface.h>
#include <SymbolicBound.h>

class LoopTransformInterface;

class DepInfoAnal 
{
 public:
  struct LoopDepInfo { 
      DomainCond domain; 
      std::vector<SymbolicVar> ivars;
      std::vector<SymbolicBound> ivarbounds;
      bool IsTop() const { return domain.IsTop(); }
   };
   struct StmtRefInfo { 
     AstNodePtr stmt, ref;
     StmtRefInfo( const AstNodePtr& s, const AstNodePtr& r) 
        : stmt(s), ref(r) {}
   };
   struct StmtRefDep { 
     StmtRefInfo r1, r2;
     AstNodePtr commLoop;
     int commLevel;
     StmtRefDep( const StmtRefInfo& _r1, const StmtRefInfo& _r2,
                 const AstNodePtr& l, int level) 
        : r1(_r1), r2(_r2), commLoop(l), commLevel(level) {} 
   };
  class SelectLoop {
    public:
      bool operator()(AstInterface& ai, const AstNodePtr& s)
       { return ai.IsLoop(s); }
  };
  typedef ModifyVariableMap<SelectLoop> ModifyVariableInfo;
  typedef CollectObject<DepInfo> DepInfoCollect;

  DepInfoAnal(LoopTransformInterface& la);

  const ModifyVariableInfo& GetModifyVariableInfo() const { return varmodInfo;}
  StmtRefDep GetStmtRefDep(LoopTransformInterface& ai, 
                           const AstNodePtr& s1, const AstNodePtr& r1,
                           const AstNodePtr& s2, const AstNodePtr& r2);
  const LoopDepInfo& GetLoopInfo(LoopTransformInterface &la, const AstNodePtr& s); 
  const LoopDepInfo& GetStmtInfo(LoopTransformInterface &la, const AstNodePtr& s);
  void ComputePrivateScalarDep( LoopTransformInterface &la, const StmtRefDep& ref,
                              DepInfoCollect &outDeps, DepInfoCollect &inDeps); 
  void ComputeGlobalScalarDep(LoopTransformInterface &la, const StmtRefDep& ref,
                              DepInfoCollect &outDeps, DepInfoCollect &inDeps); 
  void ComputeArrayDep( LoopTransformInterface &la, const StmtRefDep& ref,
                         DepType deptype, DepInfoCollect &outDeps, 
                         DepInfoCollect &inDeps) ;

  void ComputeIODep(LoopTransformInterface &la,  
                    const AstNodePtr& s1,  const AstNodePtr& s2,
                    DepInfoCollect &outDeps, DepInfoCollect &inDeps, 
                    DepType t = DEPTYPE_IO);
  void ComputeCtrlDep(LoopTransformInterface &la,  
                      const AstNodePtr& s1,  const AstNodePtr& s2,
                      DepInfoCollect &outDeps, DepInfoCollect &inDeps, 
                      DepType t = DEPTYPE_CTRL);
  void ComputeDataDep( LoopTransformInterface &la, 
                      const AstNodePtr& n1,  const AstNodePtr& n2,
                      DepInfoCollect &outDeps, DepInfoCollect &inDeps, 
                      int deptype = DEPTYPE_DATA);

 private:
  std::map <AstNodePtr, LoopDepInfo, std::less <AstNodePtr> > stmtInfo;
  ModifyVariableInfo varmodInfo;
};

bool AnalyzeStmtRefs(LoopTransformInterface &la, const AstNodePtr& n,
                      CollectObject<AstNodePtr> &wRefs, 
                      CollectObject<AstNodePtr> &rRefs);

#endif
