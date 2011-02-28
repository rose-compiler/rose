
#ifndef DEP_INFO_ANAL
#define DEP_INFO_ANAL

#include <map>
//#include <vector>
#include <DepInfo.h>
#include <DomainInfo.h>
#include <FunctionObject.h>
#include <StmtInfoCollect.h>
#include <LoopTransformInterface.h>
#include <SymbolicBound.h>

extern bool DebugDep();

class LoopTransformInterface;
class DependenceTesting;
class DepInfoAnal 
{
 public:
   //! Store information for a loop (could be nested): 
   //   domain, loop invariant variables, bound information for each loop nest
  struct LoopDepInfo { 
      DomainCond domain; 
      std::vector<SymbolicVar> ivars;
      std::vector<SymbolicBound> ivarbounds;
      bool IsTop() const { return domain.IsTop(); }
   };
   //!Store a single varaible reference within a statement
   struct StmtRefInfo { 
     AstNodePtr stmt, ref;
     StmtRefInfo( const AstNodePtr& s, const AstNodePtr& r) 
        : stmt(s), ref(r) {}
   };
   //!Store common loops and levels for two references from two statements (may be the same statement)
   struct StmtRefDep { 
     StmtRefInfo r1, r2;
     AstNodePtr commLoop;
     int commLevel;
     StmtRefDep( const StmtRefInfo& _r1, const StmtRefInfo& _r2,
                 const AstNodePtr& l, int level) 
        : r1(_r1), r2(_r2), commLoop(l), commLevel(level) {} 
   };
  // Provide an operator to check if a node is a loop node 
  class SelectLoop {
    public:
      bool operator()(AstInterface& ai, const AstNodePtr& s)
       { return ai.IsLoop(s); }
  };
  typedef ModifyVariableMap<SelectLoop> ModifyVariableInfo;
  typedef CollectObject<DepInfo> DepInfoCollect;
  // Constructor
  DepInfoAnal(LoopTransformInterface& la, DependenceTesting& h);
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
  //! Compute all possible data dependencies among two references n1, n2 
  //Store n1->n2 dependences in outDeps, n2->n1 oens int inDeps
  void ComputeDataDep( LoopTransformInterface &la, 
                      const AstNodePtr& n1,  const AstNodePtr& n2,
                      DepInfoCollect &outDeps, DepInfoCollect &inDeps, 
                      int deptype = DEPTYPE_DATA);
 private:
        DependenceTesting& handle;
        // A map store AST nodes and loop nest information
        std::map <AstNodePtr, LoopDepInfo, std::less <AstNodePtr> > stmtInfo;
        // A map store modified variables and the set of loops modifying them.
        ModifyVariableInfo varmodInfo;                
};

class DependenceTesting{
 public:
  virtual DepInfo ComputeArrayDep(LoopTransformInterface &fa, DepInfoAnal& anal,
                       const DepInfoAnal::StmtRefDep& ref, DepType deptype) =0;
  virtual ~DependenceTesting() {}
};

class AdhocDependenceTesting : public DependenceTesting {
 public:
    DepInfo ComputeArrayDep(LoopTransformInterface &fa, DepInfoAnal& anal,
                       const DepInfoAnal::StmtRefDep& ref, DepType deptype);
};
//! Analyze variable references within a statement n, put read and write references
// into two sets (rRefs and wRefs)
bool AnalyzeStmtRefs(LoopTransformInterface &la, const AstNodePtr& n,
                      CollectObject<AstNodePtr> &wRefs, 
                      CollectObject<AstNodePtr> &rRefs);

class SetDepEntry
{
  DepInfo* dep;
  DomainCond &domain1, &domain2;
  int l1, l2;
  bool& succ;
 public:
  SetDepEntry(DomainCond& _d1, DomainCond& _d2, DepInfo* _d, 
             int _l1, bool& _succ)
    : dep(_d), domain1(_d1), domain2(_d2), l1(_l1),l2(-1), succ(_succ) {}
  SetDepEntry& operator[](int i)
  { l2 = i; assert( l1 < l2); return *this; }
  void operator = (const DepRel& e)
  {
     if (!e.IsBottom()) {
       int dim1 = domain1.NumOfLoops();
       if (l1 >= dim1) {
         SetDomainRel(domain2, e, l1-dim1, l2-dim1);
         if (DebugDep())
            std::cerr << "setting domain2 entry(" << l1-dim1 << ", " << l2-dim1 << ") = " << e.toString() << "\n";
       }
       else if ( l2 >= dim1) {
         assert(dep != 0);
         SetDepRel( *dep, e, l1, l2-dim1);
         if (DebugDep())
            std::cerr << "setting dep entry(" << l1 << ", " << l2-dim1 << ") = " << e.toString() << "\n";
       }
       else  {
         SetDomainRel( domain1, e, l1, l2);
         if (DebugDep())
            std::cerr << "setting domain1 entry(" << l1 << ", " << l2 << ") = " << e.toString() << "\n";
       }
     }
  }
  void SetDepRel(DepInfo& d, const DepRel& e, int i1, int i2)
  {
    DepRel e1 = e & d.Entry(i1,i2);
    if (e1.IsTop())
       succ = false;
    d.Entry(i1,i2) = e1;
  }
  void SetDomainRel (DomainCond& d, const DepRel& e, int i1, int i2)
  {
    DepRel e1 = e & d.Entry(i1,i2);
    if (e1.IsTop())
       succ = false;
    d.SetLoopRel( i1,i2,e1);
  }
};

class SetDep
{
  DomainCond domain1, domain2;
  DepInfo* dep;
  bool succ;
 public:
  SetDep(const DomainCond& _d1, const DomainCond& _d2, DepInfo* _d)
    : domain1(_d1), domain2(_d2), dep(_d), succ(true) {}
    
  SetDepEntry operator [](int i)
   { return SetDepEntry(domain1, domain2, dep, i, succ); }

  operator bool() { return succ; }
  void finalize()
   { 
     if (DebugDep())
        std::cerr << "finalizing: domain1 = " << domain1.toString() << std::endl << "domain2 = " << domain2.toString() << std::endl;
     domain1.RestrictDepInfo( *dep, DEP_SRC);
     domain2.RestrictDepInfo( *dep, DEP_SINK);
   }
   const DomainCond& get_domain1() const { return domain1; }
   const DomainCond& get_domain2() const { return domain2; }
};
//Make variable unique by  adding scope information
class MakeUniqueVar : public MapObject<SymbolicVal, SymbolicVal>, 
                     public SymbolicVisitor
{
public:
  typedef std::pair<std::string,AstNodePtr> ReverseRec;
  typedef std::map <std::string,ReverseRec, std::less<std::string> > ReverseRecMap;
private:
  std::string postfix;
  SymbolicVal res;
  AstNodePtr curloop, curref;
  const DepInfoAnal::ModifyVariableInfo &varmodInfo;
  ReverseRecMap &reverse;

 void VisitVar( const SymbolicVar &v) 
     {  
        std::string name = v.GetVarName();
        if (varmodInfo.Modify(curloop,name)) {
            res = new SymbolicVar(name + postfix, v.GetVarScope());
            reverse[name+postfix] = ReverseRec(name,curref);
            if (DebugDep())
                std::cerr << "replacing var " << v.toString() << " => " << res.toString() << "postfix = " << postfix << std::endl;
        }
        else
           reverse[name] = ReverseRec(name,curref);
     }
 
 public:
  MakeUniqueVar(const DepInfoAnal::ModifyVariableInfo& r, ReverseRecMap& r1)  
      : varmodInfo(r), reverse(r1) {}
  SymbolicVal operator()(const SymbolicVal& v) 
  {
    res = SymbolicVal();
    v.Visit(this); 
    return res;
  }
  SymbolicVal operator()(const AstNodePtr& l, const AstNodePtr& r,
                         const SymbolicVal& v, const std::string& p) 
  {
    postfix = p;
    curloop = l;
    curref = r;
    return ReplaceVal(v, *this);
  }
};

class DepInfoAnalInterface {
  DepInfoAnal& anal;
  LoopTransformInterface& la;
 public:
  DepInfoAnalInterface(DepInfoAnal& a, LoopTransformInterface& _la)
    : anal(a), la(_la) {}
  AstNodePtr GetParent( const AstNodePtr& s)
     { AstInterface& fa = la;
       return fa.GetParent(s); }
  VarInfo GetVarInfo(const AstNodePtr& s) 
  { 
    SymbolicVar ivar;
    SymbolicVal lb,ub,step;
    if (la.IsFortranLoop(s, &ivar, &lb, &ub,&step))  
       return (step <= 0)? VarInfo(ivar,SymbolicBound(ub,lb)) 
                         : VarInfo(ivar, SymbolicBound(lb,ub));
    return VarInfo();
 }
};

class MakeUniqueVarGetBound 
  : public SymbolicConstBoundAnalysis<AstNodePtr, DepInfoAnalInterface>
{
  typedef MakeUniqueVar::ReverseRecMap ReverseRecMap;
  typedef MakeUniqueVar::ReverseRec ReverseRec;
  ReverseRecMap &reverse;
  void VisitVar( const SymbolicVar& var)
  {
          result = GetBound(var);
  }
 public:
   MakeUniqueVarGetBound( ReverseRecMap& r1, LoopTransformInterface& ai, DepInfoAnal& a) 
     : SymbolicConstBoundAnalysis<AstNodePtr, DepInfoAnalInterface>(DepInfoAnalInterface(a, ai),AST_NULL,AST_NULL),
       reverse(r1) {} 
  SymbolicBound GetBound(const SymbolicVar& var)
   {
      ReverseRec& entry = reverse[var.GetVarName()];
      node = entry.second;
      assert(node != AST_NULL);
      SymbolicBound r = SymbolicConstBoundAnalysis<AstNodePtr, DepInfoAnalInterface>::GetConstBound(SymbolicVar(entry.first,var.GetVarScope()));
      if (DebugDep())
         std::cerr << "bound of " << var.toString() << " is " << r.toString() << std::endl;
      return r;
   }
};


std::string ToString( std::vector< std::vector<SymbolicVal> > & analMatrix);

void PrintResults(const std::string buffer);

                      
#endif
