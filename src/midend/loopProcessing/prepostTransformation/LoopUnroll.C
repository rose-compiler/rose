#include <general.h>
#include <LoopUnroll.h>
#include <LoopInfoInterface.h>
#include <LoopTransformInterface.h>
#include <ProcessAstTree.h>
#include <SymbolicVal.h>
#include <assert.h>
#include <CommandOptions.h>


// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a header file it is OK here!
#define Boolean int

string LoopUnrolling:: cmdline_help() 
   { 
    return "-unroll [locond] [nvar] <unrollsize> : unrolling innermost loops at <unrollsize>"; }
Boolean LoopUnrolling:: cmdline_configure()
{
        opt = DEFAULT;
	const vector<string>& opts = CmdOptions::GetInstance()->GetOptions();
        unsigned int config = CmdOptions::GetInstance()->HasOption("-unroll");
        if (config == 0)
            return false;
	// config now points to the NEXT argument after -unroll
        if (config != opts.size() && opts[config] == "locond") {
           opt = (LoopUnrolling::UnrollOpt)(opt | LoopUnrolling::COND_LEFTOVER);
           ++config;
        }
        if (config != opts.size() && opts[config] == "nvar") {
           opt = (LoopUnrolling::UnrollOpt)(opt | LoopUnrolling::USE_NEWVAR);
           ++config;
        }
	if (config == opts.size()) return false;
	unrollsize = atoi(opts[config].c_str());
        if (unrollsize <= 0) {
           cerr << "invalid unrolling size. Use default (4)\n";
           unrollsize = 4;
        }
       return true;
}

bool LoopUnrolling::operator() ( AstInterface& fa, const AstNodePtr& s, AstNodePtr& r)
{
   if (enclosingloop == s) {
       enclosingloop = GetEnclosingLoop(s, fa);
        return false;
   }

   if (fa.IsLoop(s))
      enclosingloop = GetEnclosingLoop(s, fa);
   assert( la != 0);

   AstNodePtr body;
   SymbolicVal stepval, ubval, lbval;
   SymbolicVar ivar;
   if (la->IsFortranLoop(s, &ivar, &lbval, &ubval, &stepval, &body)) { 
          AstNodePtr r = s;
          SymbolicVal nstepval = stepval * unrollsize;
          SymbolicVal nubval = ubval;

          bool hasleft = true;
          vector<AstNodePtr> bodylist;
          AstNodePtr leftbody, lefthead;

          int stepnum=0, loopnum = 0;
          SymbolicVal loopval = ubval - lbval + 1;
          if (stepval.ToInt(stepnum) && loopval.ToInt(loopnum) 
               && !(loopnum % stepnum)) {
             hasleft = false; 
          }
          else {
             nubval = ubval - SymbolicVal(unrollsize - 1);
             if (opt & COND_LEFTOVER) {
                 leftbody = fa.CreateBasicBlock();
                 lefthead = leftbody;
             }
             else {
                 leftbody = fa.CopyAstTree(body);
                 lefthead = fa.CreateLoop( ivar.CodeGen(fa), 
                                           AstNodePtr(), 
                                           ubval.CodeGen(fa), 
                                           stepval.CodeGen(fa), leftbody);
             }
          }
          fa.RemoveStmt(body);
          AstNodePtr s1 = fa.CreateLoop(ivar.CodeGen(fa), lbval.CodeGen(fa),
                                          nubval.CodeGen(fa), 
                                          nstepval.CodeGen(fa), body);
          fa.ReplaceAst( s,s1);
          r = s1; 

          AstNodePtr origbody = fa.CopyAstTree(body);
          string nvarname = "";
          SymbolicVal nvar;
          if (opt & USE_NEWVAR) {
               nvarname = fa.NewVar(fa.GetType("int"),"",true,body, ivar.CodeGen(fa)); 
               nvar = SymbolicVar(nvarname,body);
          }
          bodylist.push_back(body);
          for (int i = 1; i < unrollsize; ++i) {
              AstNodePtr bodycopy = fa.CopyAstTree(origbody);
              if (opt & USE_NEWVAR) {
                 AstNodePtr nvarassign = 
                     fa.CreateAssignment(nvar.CodeGen(fa), (nvar+1).CodeGen(fa));
                 fa.BasicBlockAppendStmt( body, nvarassign);
                 AstTreeReplaceVar repl(ivar, nvar);
                 repl( fa, bodycopy);
              }
              else {
                 AstTreeReplaceVar repl(ivar, ivar+i);
                 repl( fa, bodycopy);
              }
              fa.BasicBlockAppendStmt( body, bodycopy);
              bodylist.push_back(bodycopy);
              if (hasleft && (opt & COND_LEFTOVER)) {
                 AstNodePtr cond = 
                      fa.CreateRelLE( ivar.CodeGen(fa), (ubval-(i-1)).CodeGen(fa));
                 AstNodePtr body1 = fa.CopyAstTree(bodylist[i-1]);
                 AstNodePtr ifstmt =  fa.CreateIf( cond, body1);
                 fa.BasicBlockAppendStmt( leftbody, ifstmt);
                 leftbody = body1;
              }
          }
          if (hasleft) {
              fa.InsertStmt( r, lefthead, false, true);
          }
          r  = s;
          return true;
   }

   return false;
}
