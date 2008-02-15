#include <CPPAstInterface.h>
#include <TestParallelLoop.h>
#include <StmtInfoCollect.h>
#include <CommandOptions.h>

#include <assert.h>
#include <fstream>
#include <sstream>

using namespace std;

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
      : funcInfo(f), body(), succ(true) {}
   virtual void AppendReadLoc( AstInterface& fa, const AstNodePtr& loc)
    {}
   virtual void AppendModLoc( AstInterface& _fa, const AstNodePtr& loc, const AstNodePtr& stmt)
    {
       if (!succ)
          return;
       CPPAstInterface& fa = static_cast<CPPAstInterface&>(_fa);
       if (fa.IsVarRef(loc)) {
            AstNodePtr scope = fa.GetVarScope(loc);
            while (scope != body && scope != 0) {
                scope = fa.GetParent(scope);
            }
            if (scope != body) {
		if (DebugCPTest()) 
                   cerr << "not parallelizable because non-local variable is modified: " << AstToString(loc) << " in " << AstToString(body) << endl;
                succ = false; 
                return;
            }
       }
       else {
             AstNodePtr var;
             STD string funcname;
             if (fa.IsMemberFunctionCall(loc, &var, &funcname)) {
                if (funcname != "operator*") {
		   if (DebugCPTest()) 
                    cerr << "not parallelizable because of seeing member function " << funcname <<  " in " << AstToString(body) << endl;
                    succ = false;
                    return;
                }
                if (!fa.IsSameVarRef(ivar, var)) {
		   if (DebugCPTest()) 
                     cerr << "not parallelizable because of applying * to non-induction variable " << AstToString(var) <<  " in " << AstToString(body) << endl;
                   succ = false;
                   return;
                }   
             }         
             else {
		if (DebugCPTest()) 
                   cerr << "not parallelizable because of seeing unknown side effect: " << AstToString(loc) <<  " in " << AstToString(body) << endl;
                 succ = false;
                return;
             }
       }
    }
   virtual void AppendFuncCall( AstInterface& fa, const AstNodePtr& loc)
    {
           AstNodePtr cur;
           OperatorSideEffectDescriptor fInfo;
           if (!funcInfo.known_operator( fa, loc, fInfo)) {
		if (DebugCPTest()) 
                   cerr << "not parallelizable because of seeing unknown function call" << AstToString(loc) <<  " in " << AstToString(body) << endl;
                succ = false;
                return;
           }
              
           if ( fInfo.contain_global()) {
		if (DebugCPTest()) 
                   cerr << "not parallelizable because of seeing modify-global function call" << AstToString(loc) <<  " in " << AstToString(body) << endl;
                succ = false;
                return;
           }
           int index = 0;
           if (cur != 0) {
              if (fInfo.contain_parameter(0))
                   AppendModLoc(fa, cur, AST_NULL);
              ++index;
           }
           AstInterface::AstNodeList args;
           if (!fa.IsFunctionCall( loc, 0, &args)) 
                assert(false);
           for (AstInterface::AstNodeList::iterator p = args.begin();
                p != args.end(); ++p, ++index) {
              if (fInfo.contain_parameter(index))
                   AppendModLoc(fa, *p, AST_NULL);
           }
    }
   bool Check( AstInterface& fa, const AstNodePtr& _ivar, 
                const string& _ivarTypeName,  const AstNodePtr& _body)
    {
      succ = true;
      ivar = _ivar;
      body = _body;
      ivarTypeName = _ivarTypeName;
      StmtInfoCollect::operator()(fa, body);
      return succ;
    }
};

bool TestParallelLoop:: LoopParallelizable( CPPAstInterface& fa, const AstNodePtr& loop)
{
   AstNodePtr init, cond, incr, body;
   bool looptest =  fa.IsLoop(loop, &init, &cond, &incr, &body);
   assert(looptest);
   if (init == 0) {
     if (DebugCPTest()) 
        cerr << "not parallelizable because loop has no init" <<  AstToString(loop) << "\n";
        return false;
   }
   if (fa.IsBlock(init)) {
     if (fa.GetBlockSize(init) != 1) { 
        if (DebugCPTest()) 
           cerr << "not parallelizable because loop init has more than one statements" <<  AstToString(loop) << "\n";
        return false;
     }
     init = fa.GetBlockFirstStmt(init);
   }
      
   AstNodePtr ivar, rhs1;
   if (!fa.IsAssignment(init, &ivar, &rhs1)) { 
       AstInterface::AstNodeList ivarlist, rhs1list;
      if (!fa.IsVariableDecl(init,&ivarlist,&rhs1list) ||
           ivarlist.size() != 1 || rhs1list.size() != 1)  {
        if (DebugCPTest()) 
          cerr << "not parallelizable because loop init is neither assignment nor single initialized variable declaration " <<  AstToString(loop) << "\n";
         return false;
      }
      ivar = *ivarlist.begin();
      rhs1 = *rhs1list.begin();
   }

   // type of ivar should be container iterator 
   AstNodeType ivarType; 
   if (!fa.IsVarRef(ivar, &ivarType)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because induction exp is not variable: " <<  AstToString(loop) << "\n";
         return false;
   }
   string ivarTypeName;
   fa.GetTypeInfo(ivarType, &ivarTypeName);

   // rhs of should be container::begin() and ivarTypeName == containerType::iterator
   AstNodePtr func, container;
   string funcName;
   if (!fa.IsMemberFunctionCall(rhs1, &container, &funcName)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because init rhs is not member function call: " << AstToString(rhs1) << " in " <<  AstToString(loop) << endl;
      return false;
   }
   AstNodeType containerType;
   if (!fa.IsVarRef(container, &containerType)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because container in init rhs is not variable: " <<   AstToString(container) << ":" << AstToString(loop) << endl;
       return false;
   }
   string containerTypeName;
   fa.GetTypeInfo(containerType, 0, &containerTypeName);
   if (containerTypeName[containerTypeName.size()-1] == '*')
      containerTypeName.erase(containerTypeName.size()-1,1); 
   if (!containers.known_type(containerTypeName) 
        || ivarTypeName != containerTypeName + "::iterator") {
     if (DebugCPTest()) 
      cerr << "not parallelizable because of container type :" << containerTypeName << " or induction var type : " << ivarTypeName << " from " <<  AstToString(loop) << endl;
      return false;
   }
   if (funcName != "begin") {
     if (DebugCPTest()) 
      cerr << "not parallelizable because begin func name is " << funcName << endl; 
      return false;
   }
   

   // cond should be "ivar != container.end()"
   AstNodePtr condopd1, condopd2;
   AstInterface::OperatorEnum opr;
   if (!fa.IsBinaryOp(cond, &opr, &condopd1, &condopd2) && opr != AstInterface::BOP_NE)    {
     AstInterface::AstNodeList args; 
     if (!fa.IsFunctionCall(cond, &func, &args) || !fa.IsVarRef(func,0,&funcName) || strstr(funcName.c_str(), "!=") == 0) {
        if (DebugCPTest()) 
           cerr << "not parallelizable because relational op is not '!=' in cond : " << AstToString(cond) << " from " <<  AstToString(loop) << "\n";
         return false;
     }
     condopd1 = args.front(); condopd2 = args.back();
   }
   if (! fa.IsSameVarRef( ivar, condopd1) ) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because cond is not applied to induction var " <<  AstToString(loop) << "\n";
        return false;
   }
   AstNodePtr container2;
   string endName;
   if (!fa.IsMemberFunctionCall(condopd2, &container2, &endName)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because cond rhs is not member function call : " << AstToString(condopd2) << " from " <<  AstToString(loop) << "\n";
       return false;
   }
   if (!fa.IsSameVarRef( container, container2)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because cond rhs is not member function call of the same container "  <<  AstToString(loop) << "\n";
     return false;
   }
   if (endName != "end") {
     if (DebugCPTest()) 
      cerr << "not parallelizable because end func name is " << funcName  << " from " <<  AstToString(loop) << endl; 
      return false;
   }

   AstNodePtr incropd;
   if (!fa.IsPlusPlus(incr, &incropd)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because incr is not ++ operator : " << AstToString(incr) << " in " <<  AstToString(loop) << "\n"; 
     return false;
   }
   if (! fa.IsSameVarRef(incropd, ivar)) {
     if (DebugCPTest()) 
      cerr << "not parallelizable because incr operand is not equal to ivar : " << AstToString(incropd) << " from " <<  AstToString(loop) << endl;
     return false;
   }
   ContainerLoopBodyParallelizable modCheck(*this);
   return modCheck.Check(fa, ivar, ivarTypeName, body);
}

void TestParallelLoop :: DumpOptions( ostream& os)
{
  os << "-annot <filename> " << endl;
}
