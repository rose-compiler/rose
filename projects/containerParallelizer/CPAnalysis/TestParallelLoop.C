// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

#include <general.h>
#include <CPPAstInterface.h>
#include <TestParallelLoop.h>
#include <StmtInfoCollect.h>
#include <CommandOptions.h>

#include <assert.h>
#include <fstream>
#include <sstream>

bool DebugCPTest()
{
  static int r = 0;
    if (r == 0) {
      if (CmdOptions::GetInstance()->HasOption("-debugcptest"))
          r = 1;
      else
          r = -1;
    }
    return r == 1;
}


TestParallelLoop:: TestParallelLoop( int argc, char** argv)
{
   ReadAnnotation* op = ReadAnnotation::get_inst();
   op->add_TypeCollection( &containers);
   op->add_OperatorCollection(&modInfo);

 for (int i = 0; i < argc; ++i) {
    if (!strcmp(argv[i], "-annot")) {
       ++i;
       ifstream is(argv[i]);
       op->read(is);
     }
 }
}

class ContainerLoopBodyParallelizable : StmtInfoCollect
{
  AstNodePtr ivar;
  AstNodePtr body;
  string ivarTypeName;
  TestParallelLoop& funcInfo;
  bool succ;
 public:
   ContainerLoopBodyParallelizable( TestParallelLoop& f) 
      : funcInfo(f), body(0), succ(true) {}
   virtual void AppendReadLoc( AstInterface& fa, const AstNodePtr& loc)
    {}
   virtual void AppendModLoc( AstInterface& fa, const AstNodePtr& loc, const AstNodePtr& stmt)
    {
       if (!succ)
          return;
       if (fa.IsVarRef(loc)) {
            AstNodePtr scope = CPPAstInterface::GetVarScope(loc);
            while (scope != body && scope != 0) {
                scope = fa.GetParent(scope);
            }
            if (scope != body) {
		if (DebugCPTest()) 
                   cerr << "not parallelizable because non-local variable is modified: " << fa.AstToString(loc) << " in " << fa.AstToString(body) << endl;
                succ = false; 
                return;
            }
       }
       else {
             AstNodePtr var, func;
             if (CPPAstInterface::IsMemberFunctionCall(loc, &var, &func)) {
                if (fa.GetFunctionName(func) != "operator*") {
		   if (DebugCPTest()) 
                    cerr << "not parallelizable because of seeing member function " << fa.AstToString(func) <<  " in " << fa.AstToString(body) << endl;
                    succ = false;
                    return;
                }
                if (!fa.IsSameVarRef(ivar, var)) {
		   if (DebugCPTest()) 
                     cerr << "not parallelizable because of applying * to non-induction variable " << fa.AstToString(var) <<  " in " << fa.AstToString(body) << endl;
                   succ = false;
                   return;
                }   
             }         
             else {
		if (DebugCPTest()) 
                   cerr << "not parallelizable because of seeing unknown side effect: " << fa.AstToString(loc) <<  " in " << fa.AstToString(body) << endl;
                 succ = false;
                return;
             }
       }
    }
   virtual void AppendFuncCall( AstInterface& fa, const AstNodePtr& loc)
    {
           AstNodePtr func;
           if (!fa.IsFunctionCall(loc, &func))
              assert(false);
           string fn = OperatorDeclaration::get_signiture(func);
           AstNodePtr cur;
           OperatorSideEffectDescriptor fInfo;
           if (!funcInfo.known_operator( fn, fInfo)) {
		if (DebugCPTest()) 
                   cerr << "not parallelizable because of seeing unknown function " << fn <<  " in " << fa.AstToString(body) << endl;
                succ = false;
                return;
           }
              
           if ( fInfo.contain_global()) {
		if (DebugCPTest()) 
                   cerr << "not parallelizable because of seeing modify-global function " << fn <<  " in " << fa.AstToString(body) << endl;
                succ = false;
                return;
           }
           int index = 0;
           if (cur != 0) {
              if (fInfo.contain_parameter(0))
                   AppendModLoc(fa, cur, 0);
              ++index;
           }
           AstInterface::AstNodeList args;
           if (!fa.IsFunctionCall( loc, 0, &args)) 
                assert(false);
           for (AstInterface::AstNodeListIterator p 
                     = fa.GetAstNodeListIterator(args); 
                !p.ReachEnd(); ++p, ++index) {
              if (fInfo.contain_parameter(index))
                   AppendModLoc(fa, *p, 0);
           }
    }
   bool Check( const AstNodePtr& _ivar, const string& _ivarTypeName,  
               const AstNodePtr& _body)
    {
      succ = true;
      ivar = _ivar;
      body = _body;
      ivarTypeName = _ivarTypeName;
      AstInterface fa(body);
      StmtInfoCollect::operator()(fa, body);
      return succ;
    }
};

bool TestParallelLoop:: LoopParallelizable( const AstNodePtr& loop)
{
   AstNodePtr init, cond, incr, body;
   bool looptest =  CPPAstInterface::IsLoop(loop, &init, &cond, &incr, &body);
   assert(looptest);
   if (init == 0) {
     if (DebugCPTest()) 
        cerr << "not parallelizable because loop has no init" <<  AstInterface::AstToString(loop) << "\n";
        return false;
   }
   if (AstInterface::IsBasicBlock(init)) {
     if (AstInterface::GetBasicBlockSize(init) != 1) { 
        if (DebugCPTest()) 
           cerr << "not parallelizable because loop init has more than one statements" <<  AstInterface::AstToString(loop) << "\n";
        return false;
     }
     init = AstInterface::GetBasicBlockFirstStmt(init);
   }
      
   AstNodePtr ivar, rhs1;
   if (!CPPAstInterface::IsAssignment(init, &ivar, &rhs1)) { 
       AstInterface::AstNodeList ivarlist, rhs1list;
      if (!CPPAstInterface::IsVariableDecl(init,&ivarlist,&rhs1list) ||
           ivarlist.size() != 1 || rhs1list.size() != 1)  {
        if (DebugCPTest()) 
          cerr << "not parallelizable because loop init is neither assignment nor single initialized variable declaration " <<  AstInterface::AstToString(loop) << "\n";
         return false;
      }
      ivar = *AstInterface::GetAstNodeListIterator(ivarlist);
      rhs1 = *AstInterface::GetAstNodeListIterator(rhs1list);
   }

   // type of ivar should be container iterator 
   AstNodeType ivarType; 
   if (!AstInterface::IsVarRef(ivar, &ivarType)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because induction exp is not variable: " <<  AstInterface::AstToString(loop) << "\n";
         return false;
   }
   string ivarTypeName;
   AstInterface::GetTypeInfo(ivarType, &ivarTypeName);

   // rhs of should be container::begin() and ivarTypeName == containerType::iterator
   AstNodePtr container, func;
   if (!CPPAstInterface::IsMemberFunctionCall(rhs1, &container, &func)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because init rhs is not member function call: " << AstInterface::AstToString(rhs1) << " in " <<  AstInterface::AstToString(loop) << endl;
      return false;
   }
   AstNodeType containerType;
   if (!AstInterface::IsVarRef(container, &containerType)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because container in init rhs is not variable: " <<   AstInterface::AstToString(container) << ":" << AstInterface::AstToString(loop) << endl;
       return false;
   }
   string containerTypeName;
   AstInterface::GetTypeInfo(containerType, 0, &containerTypeName);
   if (containerTypeName[containerTypeName.size()-1] == '*')
      containerTypeName.erase(containerTypeName.size()-1,1); 
   if (!containers.known_type(containerTypeName) 
        || ivarTypeName != containerTypeName + "::iterator") {
     if (DebugCPTest()) 
      cerr << "not parallelizable because of container type :" << containerTypeName << " or induction var type : " << ivarTypeName << " from " <<  AstInterface::AstToString(loop) << endl;
      return false;
   }
   string funcName = AstInterface::GetFunctionName(func);
   if (funcName != "begin") {
     if (DebugCPTest()) 
      cerr << "not parallelizable because begin func name is " << funcName << endl; 
      return false;
   }
   

   // cond should be "ivar != container.end()"
   AstNodePtr condopd1, condopd2;
   if (!AstInterface::IsRelNE(cond, &condopd1, &condopd2)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because relational op is not '!=' in cond : " << AstInterface::AstToString(cond) << " from " <<  AstInterface::AstToString(loop) << "\n";
         return false;
   }
   if (! AstInterface::IsSameVarRef( ivar, condopd1) ) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because cond is not applied to induction var " <<  AstInterface::AstToString(loop) << "\n";
        return false;
   }
   AstNodePtr container2, func2;
   if (!CPPAstInterface::IsMemberFunctionCall(condopd2, &container2, &func2)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because cond rhs is not member function call : " << AstInterface::AstToString(condopd2) << " from " <<  AstInterface::AstToString(loop) << "\n";
       return false;
   }
   if (!AstInterface::IsSameVarRef( container, container2)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because cond rhs is not member function call of the same container "  <<  AstInterface::AstToString(loop) << "\n";
     return false;
   }
   string endName = AstInterface::GetFunctionName(func2);
   if (endName != "end") {
     if (DebugCPTest()) 
      cerr << "not parallelizable because end func name is " << funcName  << " from " <<  AstInterface::AstToString(loop) << endl; 
      return false;
   }

   AstNodePtr incropd;
   if (!CPPAstInterface::IsPlusPlus(incr, &incropd)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because incr is not ++ operator : " << AstInterface::AstToString(incr) << " in " <<  AstInterface::AstToString(loop) << "\n"; 
     return false;
   }
   if (! AstInterface::IsSameVarRef(incropd, ivar)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because incr operand is not equal to ivar : " << AstInterface::AstToString(incropd) << " from " <<  AstInterface::AstToString(loop) << endl;
     return false;
   }
   ContainerLoopBodyParallelizable modCheck(*this);
   return modCheck.Check(ivar, ivarTypeName, body);
}

void TestParallelLoop :: DumpOptions( ostream& os)
{
  os << "-annot <filename> " << endl;
}
