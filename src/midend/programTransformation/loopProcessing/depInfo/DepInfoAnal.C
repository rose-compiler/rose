#include <sstream>
#include <DoublyLinkedList.h>
#include <DomainInfo.h>
#include <DepInfo.h>
#include <DepInfoAnal.h>
#include <SymbolicVal.h>
#include <StmtInfoCollect.h>
#include <StmtDepAnal.h>
#include <LoopInfoInterface.h>
#include <LoopTransformInterface.h>

#include <iostream>
#include <CommandOptions.h>
#include <fstream>

#ifdef BD_OMEGA
#include <PlatoOmegaInterface.h>
#include <DepTestStatistics.h>

extern DepTestStatistics DepStats;
#endif


extern bool DebugDep();

void PrintResults(const std::string buffer) {
   std::string filename;
        filename = "roseResults";
   if (CmdOptions::GetInstance()->HasOption("-depAnalOnlyPrintF"))
        {
      std::fstream outFile;
      outFile.open(filename.c_str(),std::fstream::out | std::fstream::app);
      if (outFile) {
         outFile << buffer;
         outFile.close();
      }
   }
   else if (CmdOptions::GetInstance()->HasOption("-depAnalOnlyPrintS")) {
      std::cerr << buffer;
   }
}

class AccuAstRefs : public CollectObject<std::pair<AstNodePtr,AstNodePtr> >
{
   CollectObject<AstNodePtr> &col;
  public:
   AccuAstRefs( CollectObject<AstNodePtr> &_col) 
      :  col(_col) {}
   bool operator()(const std::pair<AstNodePtr,AstNodePtr>& n)
   {
      return col(n.first);
   }
};
bool AnalyzeStmtRefs( AstInterface& fa, const AstNodePtr& n, 
                      CollectObject<AstNodePtr> &wRefs, 
                      CollectObject<AstNodePtr> &rRefs)
{
  AccuAstRefs colw(wRefs);
  AccuAstRefs colr(rRefs);
  return StmtSideEffectCollect(LoopTransformInterface::getSideEffectInterface())(fa,n,&colw,&colr);
}

std::string toString( std::vector<SymbolicVal> & analvec)
{
  std::stringstream out;
          for (size_t j = 0; j < analvec.size(); ++j)
             out << analvec[j].toString();
  return out.str();
}
std::string toString( std::vector< std::vector<SymbolicVal> > & analMatrix)
{
   std::string result;
      for (size_t i = 0; i < analMatrix.size(); ++i) {
         result = result + toString(analMatrix[i]) + "\n";
      }
  return result;
}

DepInfo ComputePrivateDep( DepInfoAnal& anal,
                          const DepInfoAnal::StmtRefDep& ref, 
                          DepType t, int pos)
  {
     const DepInfoAnal::LoopDepInfo& info1 = anal.GetStmtInfo(ref.r1.stmt);
     const DepInfoAnal::LoopDepInfo& info2 = anal.GetStmtInfo(ref.r2.stmt);
     int dim1 = info1.domain.NumOfLoops(), dim2 = info2.domain.NumOfLoops();

     if (! ref.commLevel && pos <= 0)
       return DepInfo();
     DepInfo result=DepInfoGenerator::GetDepInfo( dim1, dim2, t, 
                                   ref.r1.ref, ref.r2.ref, false,ref.commLevel);
     int i;
     for ( i = 0; i < ref.commLevel-1; i++) {
        result.Entry( i,i) = DepRel(DEPDIR_EQ, 0);
     }
     if (ref.commLevel > 0) {
       if (pos >= 0) 
         result.Entry( i,i) = DepRel(DEPDIR_EQ, 0);
       else 
         result.Entry( i,i) = DepRel(DEPDIR_LE, -1);
     }
     info2.domain.RestrictDepInfo( result, DEP_SINK);
     info1.domain.RestrictDepInfo( result, DEP_SRC);
     return result;
  }
      
DepInfo ComputeGlobalDep( DepInfoAnal& anal,
                          const DepInfoAnal::StmtRefDep& ref, 
                          DepType t, int pos)
  {
     const DepInfoAnal::LoopDepInfo& info1 = anal.GetStmtInfo(ref.r1.stmt);
     const DepInfoAnal::LoopDepInfo& info2 = anal.GetStmtInfo(ref.r2.stmt);
     int dim1 = info1.domain.NumOfLoops();
     int  dim2 = info2.domain.NumOfLoops();
     if (! ref.commLevel && pos <= 0)
       return DepInfo();
     DepInfo result =  DepInfoGenerator::GetDepInfo(dim1, dim2, t, 
                                 ref.r1.ref, ref.r2.ref, false, ref.commLevel );
     if (ref.commLevel > 0) {
       if (pos >= 0)
         result.Entry( ref.commLevel,ref.commLevel) = DepRel(DEPDIR_LE, 0);
       else
         result.Entry( ref.commLevel,ref.commLevel) = DepRel(DEPDIR_LE, -1);
     }
     return result;
  }

const DepInfoAnal::LoopDepInfo& DepInfoAnal::
GetStmtInfo( const AstNodePtr& _s)
{
    AstInterface& fa = get_astInterface();
    AstNodePtr s = fa.IsFortranLoop(_s)? _s : GetEnclosingLoop(_s, fa);
    const LoopDepInfo& info = GetLoopInfo(s);
    return info;
}

const DepInfoAnal::LoopDepInfo& DepInfoAnal::
GetLoopInfo( const AstNodePtr& s)
{
    AstInterface& ai = get_astInterface();
    SymbolicVar ivar;
    SymbolicVal lb, ub;
    if (s == AST_NULL || !SymbolicValGenerator::IsFortranLoop(ai,s, &ivar, &lb, &ub))
       return stmtInfo[AST_NULL];
    LoopDepInfo& info= stmtInfo[s];
    if (info.IsTop()) {
       AstNodePtr l = GetEnclosingLoop(s,ai);
       const LoopDepInfo& info1 = GetLoopInfo(l);
       int dim1 = info1.domain.NumOfLoops();
       info.domain = DomainCond(dim1+1);
       for (int j = 0; j < dim1; ++j) {
         info.ivars.push_back(info1.ivars[j]);
         info.ivarbounds.push_back(info1.ivarbounds[j]);
         for (int k = j+1; k < dim1; ++k)
            info.domain.SetLoopRel(j,k,info1.domain.Entry(j,k));
       }
       info.ivars.push_back(ivar); 
       SymbolicConstBoundAnalysis<AstNodePtr,DepInfoAnalInterface> 
            boundop( DepInfoAnalInterface(*this), s, AST_NULL);
       info.ivarbounds.push_back(boundop.GetConstBound(ivar));
       std::vector<SymbolicVal>  lbvec, ubvec;
       SymbolicVal lbleft = 
         DecomposeAffineExpression(lb,info1.ivars,lbvec,dim1);
       lbvec.push_back(-1);
       lbvec.push_back(-lbleft);
       SetDep op(info.domain, DomainCond(), 0);
       if (!AnalyzeEquation(lbvec, info.ivarbounds, boundop, op, DepRel(DEPDIR_LE, 0)))
         if (DebugDep())
            std::cerr << "unable to analyze equation: " << toString(lbvec) << std::endl;
       SymbolicVal ubleft = 
         DecomposeAffineExpression(ub,info1.ivars,ubvec,dim1);
       ubvec.push_back(-1);
       ubvec.push_back(-ubleft);
       if (!AnalyzeEquation(ubvec, info.ivarbounds, boundop, op, DepRel(DEPDIR_GE, 0))) 
          if (DebugDep())
             std::cerr << "unable to analyze equation: " << toString(ubvec) << std::endl;
       info.domain = op.get_domain1();
       info.domain.ClosureCond();
       if (DebugDep())
         std::cerr << "domain of statement " << AstInterface::AstToString(s) << " is : " << info.domain.toString() << std::endl;
    }
    assert(!info.IsTop());
    return info;
}

void DepInfoAnal :: 
ComputePrivateScalarDep( const StmtRefDep& ref,
                             DepInfoCollect &outDeps, DepInfoCollect &inDeps)
{
   DepType t1 = DEPTYPE_SCALAR, t2 = DEPTYPE_BACKSCALAR;
   if (ref.commLevel > 0 || ref.r1.ref != ref.r2.ref) {
     DepInfo d = ComputePrivateDep(*this, ref, t1, 1);
     assert(!d.IsTop());
     outDeps(d);
   }
   if ( ref.commLevel > 0 && ref.r1.ref != ref.r2.ref) {
      StmtRefDep ref2(ref.r2, ref.r1, ref.commLoop, ref.commLevel);
      DepInfo d1 = ComputePrivateDep(*this, ref2, t2, -1);
      assert(!d1.IsTop());
      inDeps(d1);
  }
}

void DepInfoAnal :: 
ComputeGlobalScalarDep( const StmtRefDep& ref,
                             DepInfoCollect &outDeps, DepInfoCollect &inDeps)
{
   DepType t1 = DEPTYPE_SCALAR, t2 = DEPTYPE_BACKSCALAR;
   if (ref.commLevel > 0 || ref.r1.ref != ref.r2.ref) {
     DepInfo d = ComputeGlobalDep(*this, ref, t1, 1);
     assert( !d.IsTop());
     outDeps(d);
   }
   if ( ref.commLevel > 0) {
      StmtRefDep ref2(ref.r2, ref.r1, ref.commLoop, ref.commLevel);
      DepInfo d1 = ComputeGlobalDep(*this, ref2, t2, -1);
      assert(!d1.IsTop());
      inDeps(d1); 
   }
}

AdhocDependenceTesting AdhocTest;
#ifdef OMEGA
PlatoOmegaInterface::PlatoOmegaDepTesting OmegaTest;
PlatoOmegaInterface::PlatoOmegaDepTesting PlatoTest;
#endif

DepInfoAnal :: DepInfoAnal(AstInterface& fa)
  : handle(AdhocTest), varmodInfo(fa, SelectLoop(),
              LoopTransformInterface::getSideEffectInterface())
{

#ifdef OMEGA
        if (CmdOptions::GetInstance()->HasOption("-plato"))
        {
      handle = PlatoTest;
                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::PLATO);
        }
        else if (CmdOptions::GetInstance()->HasOption("-omega"))
        {
                handle = OmegaTest;
                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::OMEGA);
        }
        else
        {
                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::ADHOC);
        }
#endif
        AstNodePtr root = fa.GetRoot();
        varmodInfo.Collect(root);
}

DepInfoAnal :: DepInfoAnal( AstInterface& fa, DependenceTesting& h)
  : handle(h), 
   varmodInfo(fa, SelectLoop(),
              LoopTransformInterface::getSideEffectInterface())
{
  AstNodePtr root = fa.GetRoot();
  varmodInfo.Collect(root);
}

void DepInfoAnal :: ComputeArrayDep( const StmtRefDep& ref,
                           DepType deptype, 
                           DepInfoCollect &outDeps, DepInfoCollect &inDeps) 
{
        DepInfo d;

#ifdef OMEGA
        DepInfo plato_d,omega_d;
        unsigned int test = 0;
        if (CmdOptions::GetInstance()->HasOption("-omega"))
                test |= PlatoOmegaInterface::OMEGA;
        if (CmdOptions::GetInstance()->HasOption("-plato"))
                test |= PlatoOmegaInterface::PLATO;
        if (CmdOptions::GetInstance()->HasOption("-adhoc") || !test)
                test |= PlatoOmegaInterface::ADHOC;
#endif

        if (ref.commLevel > 0 || ref.r1.ref != ref.r2.ref)
        {
                /*AstInterface& ai  = fa;
                AstNodePtr root = ai.GetRoot();
                ai.get_fileInfo(ref.r1.stmt,&fileName,&lineNo1);
                ai.get_fileInfo(ref.r2.stmt,&fileName,&lineNo2);
                ai.get_fileInfo(root,&fileName,&dummy);
                */
                /** Due to the time they take, do only the tests that are
                *         called for
                **/
#ifdef OMEGA
                switch(test)
                {
                        case PlatoOmegaInterface::ADHOC :
                        {
                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::ADHOC);
#endif

                                handle = AdhocTest;
                                d = handle.ComputeArrayDep(*this, ref, deptype);

#ifdef OMEGA
                        }
                        break;
                        case PlatoOmegaInterface::OMEGA :
                        {
                                handle = OmegaTest;
                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::OMEGA);
                                d = omega_d = OmegaTest.ComputeArrayDep(*this, ref, deptype);
                        }
                        break;
                        case PlatoOmegaInterface::PLATO :
                        {
                                handle = PlatoTest;
                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::PLATO);
                                d = plato_d = PlatoTest.ComputeArrayDep(*this, ref, deptype);
                        }
                        break;
                        case PlatoOmegaInterface::ADHOC | PlatoOmegaInterface::OMEGA :
                        {
                                handle = AdhocTest;
                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::ADHOC);
                                d = handle.ComputeArrayDep(fa, *this, ref, deptype);
                                
                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::OMEGA);
                                omega_d = OmegaTest.ComputeArrayDep(fa, *this, ref, deptype);
                        }
                        break;
                        case PlatoOmegaInterface::ADHOC | PlatoOmegaInterface::PLATO :
                        {
                                handle = AdhocTest;
                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::ADHOC);
                                d = handle.ComputeArrayDep(fa, *this, ref, deptype);                                

                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::PLATO);
                                plato_d = PlatoTest.ComputeArrayDep(fa, *this, ref, deptype);
                        }
                        break;
                        case PlatoOmegaInterface::OMEGA | PlatoOmegaInterface::PLATO :
                        {
                                handle = OmegaTest;
                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::OMEGA);
                                d = omega_d=OmegaTest.ComputeArrayDep(fa, *this, ref, deptype);                

                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::PLATO);
                                plato_d = PlatoTest.ComputeArrayDep(fa, *this, ref, deptype);
                        }
                        break;
                        case PlatoOmegaInterface::ADHOC | PlatoOmegaInterface::OMEGA | PlatoOmegaInterface::PLATO :
                        {
                                handle = AdhocTest;
                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::ADHOC);
                                d = handle.ComputeArrayDep(fa, *this, ref, deptype);

                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::OMEGA);
                                omega_d = OmegaTest.ComputeArrayDep(fa, *this, ref, deptype);
                                
                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::PLATO);
                                plato_d = PlatoTest.ComputeArrayDep(fa, *this, ref, deptype);

                                DepStats.SetDepChoice(test);
                                if (ref.commLevel > 0)
                                {
                                        DepStats.CompareDepTests(test, d, plato_d, omega_d);
                                }
                        }
                        break;
                        default :
                        {
                                handle = AdhocTest;
                                PlatoOmegaInterface::SetDepChoice(PlatoOmegaInterface::ADHOC);
                                d = handle.ComputeArrayDep(fa, *this, ref, deptype);
                        }
                        break;
                }
#endif

                if ( !d.IsTop())
                {
                   if (ref.commLevel > 0) {
                      DepInfo d1 = Reverse(d);
                      SetDepDirection( d1, ref.commLevel, inDeps);
                   }
                   if (ref.commLevel > 0 || ref.r1.ref != ref.r2.ref) {
                      int carryLevel = SetDepDirection( d, ref.commLevel, outDeps);
                      if ( ! d.IsTop() &&
                         !(carryLevel > ref.commLevel && ref.r1.ref == ref.r2.ref) )
                         outDeps( d );
                   }
                }
        }
}

int adhocProbNum = 0;

DepInfo AdhocDependenceTesting::ComputeArrayDep( DepInfoAnal& anal,
                       const DepInfoAnal::StmtRefDep& ref, DepType deptype)
{
  if (DebugDep())
     std::cerr << "compute array dep between " << AstInterface::AstToString(ref.r1.ref) << " and " << AstInterface::AstToString(ref.r2.ref) << std::endl;

  const DepInfoAnal::LoopDepInfo& info1 = anal.GetStmtInfo(ref.r1.stmt);
  const DepInfoAnal::LoopDepInfo& info2 = anal.GetStmtInfo(ref.r2.stmt);
  size_t dim1 = info1.domain.NumOfLoops(), dim2 = info2.domain.NumOfLoops();
  size_t dim = dim1+dim2, i;
  // int lineNo1, lineNo2;
  std::string filename;
  // double t0, adhocTime;
  std::stringstream buffer;
  
  std::vector<SymbolicBound> bounds;
  for (i = 0; i < dim1; ++i) 
     bounds.push_back(info1.ivarbounds[i]);
  for (i = 0 ; i < dim2; ++i)
     bounds.push_back(info2.ivarbounds[i]);

  MakeUniqueVar::ReverseRecMap varmap;
  MakeUniqueVar varop(anal.GetModifyVariableInfo(),varmap);
  MakeUniqueVarGetBound boundop(varmap, anal);

  AstInterface::AstNodeList sub1, sub2;
#ifndef NDEBUG
  bool succ1 =  LoopTransformInterface::IsArrayAccess(ref.r1.ref, 0, &sub1);
  bool succ2 = LoopTransformInterface::IsArrayAccess(ref.r2.ref, 0, &sub2);
  assert(succ1 && succ2);
#endif

  AstInterface::AstNodeList::const_iterator iter1 = sub1.begin();
  AstInterface::AstNodeList::const_iterator iter2 = sub2.begin();

  int postfix = 0;
  std::stringstream varpostfix1, varpostfix2;
  ++postfix;
  varpostfix1 << "___depanal_" << postfix;
  ++postfix;
  varpostfix2 << "___depanal_" << postfix;

  bool precise = true;
  AstNodePtr s1, s2;
  std::vector <std::vector<SymbolicVal> > analMatrix;

  AstInterface& fa = anal.get_astInterface();
  for ( ; iter1 != sub1.end() && iter2 != sub2.end(); ++iter1, ++iter2) {
    s1 = *iter1; s2 = *iter2;
    SymbolicVal val1 = SymbolicValGenerator::GetSymbolicVal(fa, s1);
    SymbolicVal val2 = SymbolicValGenerator::GetSymbolicVal(fa, s2);
      /* here try to handle a special case */ 
    if (val1 == val2 && 
        fa.IsArrayAccess(s1, &s2) && LoopTransformInterface::IsUniqueArray(s2)) {
          std::cerr << "Skipping unique array dependence!\n";
          return false;
    }
    std::vector<SymbolicVal> cur;
    SymbolicVal left1 = DecomposeAffineExpression(val1, info1.ivars, cur,dim1); 
    SymbolicVal left2 = DecomposeAffineExpression(-val2, info2.ivars,cur,dim2); 
    if (left1.IsNIL() || left2.IsNIL()) {
         precise = false;
         continue;
    }
    assert (cur.size() == dim);
    for (i = 0; i < dim1;  ++i) {
       cur[i] = varop(ref.commLoop, ref.r1.ref, cur[i], varpostfix1.str()); 
    }
    for (; i < dim; ++i) {
       cur[i] = varop(ref.commLoop, ref.r2.ref, cur[i], varpostfix2.str()); 
    }
    if (DebugDep()) {
       std::cerr << "analyzing array subscripts: " << val1.toString() << " .vs. " << val2.toString() << "\n";
       std::cerr << "remaining value assumed to be loop invarient: " << left1.toString() << " + " << left2.toString() << "\n";
    }
    left1 = varop(ref.commLoop, ref.r1.ref, left1, varpostfix1.str());
    left2 = varop(ref.commLoop, ref.r2.ref, left2, varpostfix2.str());
    SymbolicVal leftVal = -left2 - left1;
    cur.push_back(leftVal);  
    assert(dim+1 == cur.size());
    if (DebugDep()) {
       std::cerr << "coefficients for induction variables (" << dim1 << " + " << dim2 << "+ 1)\n";
       for (size_t i = 0; i < dim; ++i) 
         std::cerr << cur[i].toString() << bounds[i].toString() << "\n" ;
       std::cerr << cur[dim].toString() << std::endl;
    }

    for ( size_t i = 0; i < dim; ++i) {
        SymbolicVal cut = cur[i];
        if (cut == 1 || cut == 0 || cut == -1)
             continue;
        std::vector<SymbolicVal> split;
        if (SplitEquation(cur, cut, bounds, boundop, split)) 
             analMatrix.push_back(split);
    }
    analMatrix.push_back(cur);
  }
  if (DebugDep()) 
      std::cerr << "analyzing relation matrix : \n" <<  toString(analMatrix) << std::endl;

#ifdef OMEGA
  DepStats.InitAdhocTime();
#endif

  if (! NormalizeMatrix(analMatrix, analMatrix.size(), dim+1) )
  {  
        return false;
  }
  if (DebugDep()) 
      std::cerr << "after normalization, relation matrix = \n" << toString(analMatrix) << std::endl;
   DepInfo result=DepInfoGenerator::GetDepInfo(dim1, dim2, deptype, ref.r1.ref, ref.r2.ref, false, ref.commLevel);
  SetDep setdep( info1.domain, info2.domain, &result);
  for (size_t k = 0; setdep && k < analMatrix.size(); ++k) {
       size_t j = 0;
       for (; j < dim+1; ++j) {
          if (analMatrix[k][j] != 0)
              break;
       }
       if (j == dim+1) // equation has only 0
          continue;
       if (j == dim && analMatrix[k][j].GetValType() == VAL_CONST && analMatrix[k][j]!=0)
          return DepInfo();
       if (!AnalyzeEquation( analMatrix[k], bounds, boundop,setdep, DepRel(DEPDIR_EQ,0)))
                 {
           precise = false;
           if (DebugDep())
              std::cerr << "unable to analyze equation " << k  << std::endl;
       }
  }

#ifdef OMEGA
  DepStats.SetAdhocTime();  

  AstInterface *temp = (AstInterface*) &fa;
  std::string adhocDV;
  temp->get_fileInfo(ref.r1.ref,&filename,&lineNo1);
  temp->get_fileInfo(ref.r2.ref,&filename,&lineNo2);
  if (ref.commLevel > 0)
  {
          adhocProbNum++;
          //adhocDV = PlatoOmegaInterface::DirVector(result);
                  //buffer << "Prob\t" << adhocProbNum << " between " << lineNo1 << " and " << lineNo2 << "\tAdhoc\t" << DepType2String(result.GetDepType()) << "\tTime\t" << adhocTime << std::endl;
          //buffer << "Prob\t" << adhocProbNum << "\tAdhoc\t" << DepType2String(result.GetDepType()) << "\tDV\t" << adhocDV << "\tTime\t" << adhocTime <<  std::endl;
                //PrintResults(buffer.str());
  }
#endif

  if (!setdep)
      return DepInfo();
  if (precise) 
      result.set_precise(); 
  if (DebugDep()) 
       std::cerr << "after analyzing relation matrix, result =: \n" << result.toString() << std::endl;
  setdep.finalize();
  if (DebugDep())
       std::cerr << "after restrictions from stmt domain, result =: \n" << result.toString() << std::endl;
  return result;
}

DepInfoAnal::StmtRefDep DepInfoAnal::
GetStmtRefDep( const AstNodePtr& s1,  const AstNodePtr& r1,
               const AstNodePtr& s2, const AstNodePtr& r2)
{
  StmtRefDep r( StmtRefInfo(s1,r1), StmtRefInfo(s2,r2),AST_NULL,0);
  AstInterface& fa = get_astInterface();
  r.commLoop = GetCommonLoop(fa, s1, GetStmtInfo(s1).domain.NumOfLoops(),
                             s2, GetStmtInfo(s2).domain.NumOfLoops(),
                             &r.commLevel);
  return r;
}

void DepInfoAnal ::
ComputeIODep( const AstNodePtr& s1,  const AstNodePtr& s2,
               DepInfoCollect &outDeps, DepInfoCollect &inDeps, DepType t)
{
  StmtRefDep ref = GetStmtRefDep( s1, s1, s2, s2);
  if (ref.commLevel > 0 || s1 != s2) {
    DepInfo d = ComputeGlobalDep( *this, ref, t, 1);
    assert(!d.IsTop());
    outDeps(d);
  }
  if (ref.commLevel > 0 && s1 != s2) {
       StmtRefDep ref2( ref.r2, ref.r1, ref.commLoop, ref.commLevel);
       DepInfo d1 = ComputeGlobalDep(*this, ref2, t, -1);
       assert(!d1.IsTop());
       inDeps(d1);
  }
}

void DepInfoAnal::
ComputeCtrlDep( const AstNodePtr& s1,  const AstNodePtr& s2,
               DepInfoCollect &outDeps, DepInfoCollect &inDeps, DepType t)
{
  StmtRefDep ref = GetStmtRefDep(s1, s1, s2, s2);
  if (ref.commLevel > 0 || s1 != s2) {
     if (AstInterface::IsIf(s1)) {
        DepInfo d = ComputePrivateDep( *this, ref, t, 1);
        assert(!d.IsTop());
        outDeps(d);
        StmtRefDep ref2 = GetStmtRefDep(s2, s2, s1, s1);
        d = ComputePrivateDep( *this, ref2, t, 1);
        assert(!d.IsTop());
        inDeps(d);
     }
     else {
        DepInfo d = ComputePrivateDep( *this, ref, t, 1);
        assert(!d.IsTop());
        outDeps(d);
     }
  }
}


void ComputeRefSetDep( DepInfoAnal& anal, 
                       DepInfoAnal::StmtRefDep& ref,
                       DoublyLinkedListWrap<AstNodePtr> *rs1, 
                       DoublyLinkedListWrap<AstNodePtr> *rs2,
                       DepType t, CollectObject<DepInfo> &outDeps, 
                       CollectObject<DepInfo> &inDeps)
{
  AstInterface& fa = anal.get_astInterface();
  for (DoublyLinkedListWrap<AstNodePtr>::iterator iter1 = rs1->begin(); 
      iter1 != rs1->end(); ++iter1) {
    AstNodePtr r1 = *iter1, array1;
    bool b1 = LoopTransformInterface::IsArrayAccess(r1, &array1);
    if (!b1)
       array1 = r1; 

    ref.r1.ref = r1;
    for (DoublyLinkedListWrap<AstNodePtr>::iterator  iter2 = 
              (rs1 == rs2)? iter1 : rs2->begin();
         iter2 != rs2->end(); ++iter2) {
       AstNodePtr r2 = *iter2, array2;
       bool b2 = LoopTransformInterface::IsArrayAccess(r2, &array2);
       if (!b2)
          array2 = r2;

       ref.r2.ref = r2;
       if ( fa.IsSameVarRef( array1, array2) ) {
           if (b1 && b2) 
               anal.ComputeArrayDep( ref, t, outDeps, inDeps);
           else if (b1 || b2) 
               anal.ComputeGlobalScalarDep( ref, outDeps, inDeps);
           else 
               anal.ComputePrivateScalarDep( ref, outDeps, inDeps);
       }
       else if ( LoopTransformInterface::IsAliasedRef( r1, r2)) {
          anal.ComputeGlobalScalarDep( ref, outDeps, inDeps); 
       }
    }
  }
}

void RemoveIvars( AstInterface& ai, DoublyLinkedListWrap<AstNodePtr>& refs,
                  const std::vector<SymbolicVar>& ignore)
{
  for (DoublyLinkedEntryWrap<AstNodePtr>* p = refs.First(); p != 0; ) {
     DoublyLinkedEntryWrap<AstNodePtr>* p1 = p;
     p = refs.Next(p);
     AstNodePtr cur = p1->GetEntry();
     std::string name;
     AstNodePtr scope;
     if (cur != AST_NULL && ai.IsVarRef(cur, 0,&name, &scope)) {
         SymbolicVar curvar(name, scope);
         for (size_t i = 0; i < ignore.size(); ++i) 
             if (ignore[i] == curvar) {
                refs.Delete(p1);
                break;
             }
     } 
   }
}

void DepInfoAnal ::
ComputeDataDep( const AstNodePtr& s1,  const AstNodePtr& s2,
               DepInfoCollect &outDeps, DepInfoCollect &inDeps, int t)
{
  AstInterface& fa = get_astInterface();
  DoublyLinkedListWrap<AstNodePtr> rRef1, wRef1, rRef2, wRef2;
  CollectDoublyLinkedList<AstNodePtr> crRef1(rRef1),cwRef1(wRef1),crRef2(rRef2),cwRef2(wRef2);
  if (!AnalyzeStmtRefs( fa, s1, cwRef1, crRef1) || 
        (s1 != s2 && !AnalyzeStmtRefs( fa, s2, cwRef2, crRef2))) {
       if (DebugDep())
          std::cerr << "cannot determine side effects of statements: " << AstInterface::AstToString(s1) << "; or " << AstInterface::AstToString(s2) << std::endl;
       ComputeIODep( s1, s2, outDeps, inDeps, DEPTYPE_IO);
  }
  StmtRefDep ref = GetStmtRefDep( s1, AST_NULL, s2, AST_NULL);
  if (s1 == s2 && ref.commLevel == 0)
        return;

  if (s1 == s2) {
     RemoveIvars( fa, rRef1, GetStmtInfo(s1).ivars);
     RemoveIvars( fa, wRef1, GetStmtInfo(s1).ivars);
     if (t & DEPTYPE_OUTPUT)
          ComputeRefSetDep( *this, ref, &wRef1, &wRef1, 
                            DEPTYPE_OUTPUT, outDeps, inDeps);
     if (t & DEPTYPE_TRUE || t & DEPTYPE_ANTI)
          ComputeRefSetDep( *this, ref, &rRef1, &wRef1, 
                            DEPTYPE_ANTI, outDeps, inDeps);
     if (t & DEPTYPE_INPUT)
          ComputeRefSetDep( *this, ref, &rRef1, &rRef1, 
                            DEPTYPE_INPUT, outDeps, inDeps);
  }
  else {
     RemoveIvars(fa, rRef1, GetStmtInfo(s1).ivars);
     RemoveIvars(fa, rRef2, GetStmtInfo(s2).ivars);
     RemoveIvars(fa, wRef1, GetStmtInfo(s1).ivars);
     RemoveIvars(fa, wRef2, GetStmtInfo(s2).ivars);
     if (t & DEPTYPE_OUTPUT)
          ComputeRefSetDep( *this, ref, &wRef1, &wRef2, 
                            DEPTYPE_OUTPUT, outDeps, inDeps);
     if (t & DEPTYPE_TRUE)
          ComputeRefSetDep( *this, ref, &wRef1, &rRef2, 
                            DEPTYPE_TRUE, outDeps, inDeps);
     if (t & DEPTYPE_ANTI)
          ComputeRefSetDep( *this, ref, &rRef1, &wRef2, 
                            DEPTYPE_ANTI, outDeps, inDeps);
     if (t & DEPTYPE_INPUT)
          ComputeRefSetDep( *this, ref, &rRef1, &rRef2, 
                            DEPTYPE_INPUT, outDeps, inDeps);
   }
}

