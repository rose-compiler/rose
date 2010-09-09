

#include <vector>
#include <string>
#include <algorithm>

#include <LoopUnroll.h>
#include <LoopInfoInterface.h>
#include <LoopTransformInterface.h>
#include <ProcessAstTree.h>
#include <SymbolicVal.h>
#include <assert.h>
#include <CommandOptions.h>

using namespace std;

std::string LoopUnrolling:: cmdline_help() 
   { 
    return "-unroll [locond] [nvar] <unrollsize> : unrolling innermost loops at <unrollsize>";
   }

bool LoopUnrolling:: cmdline_configure()
{
        opt = DEFAULT;
        vector<string>& opts = CmdOptions::GetInstance()->opts;
        // Must use -unroll to turn this on
        vector<string>::const_iterator i = std::find(opts.begin(), opts.end(), "-unroll");
        if (i == opts.end()) 
          return false;
        size_t idx = i - opts.begin();
        opts.erase(opts.begin() + idx);
        if (idx != opts.size() && opts[idx] == "locond") {
           opt = (LoopUnrolling::UnrollOpt)(opt | LoopUnrolling::COND_LEFTOVER);
           opts.erase(opts.begin() + idx);
        }
        if (idx != opts.size() && opts[idx] == "nvar") {
           opt = (LoopUnrolling::UnrollOpt)(opt | LoopUnrolling::USE_NEWVAR);
           opts.erase(opts.begin() + idx);
        }
        int localUnrollsize = 0;
        if (idx != opts.size()) {
          sscanf(opts[idx].c_str(), "%d",&localUnrollsize);
          if (localUnrollsize >= 1) {
            unrollsize = localUnrollsize;
            opts.erase(opts.begin() + idx);
          } else {
             std::cerr << "invalid unrolling size. Use default (4)\n";
             unrollsize = 4;
          }
        }
        return true;
}

bool LoopUnrolling::operator() ( AstInterface& fa, const AstNodePtr& s, AstNodePtr& r)
{
   bool isLoop = false;
   if (enclosingloop == s || (enclosingloop == AST_NULL && (isLoop = fa.IsLoop(s)))) 
   {
       // find the outer most enclosing loop of s 
       for (enclosingloop = fa.GetParent(s); 
            enclosingloop != AST_NULL && !fa.IsLoop(enclosingloop); 
            enclosingloop = fa.GetParent(enclosingloop));
       if (!isLoop)
          return false;
   }
   assert( la != 0);

   AstNodePtr body;
   SymbolicVal stepval, ubval, lbval;
   SymbolicVar ivar;
   // is Canonical loop?
   if (la->IsFortranLoop(s, &ivar, &lbval, &ubval, &stepval, &body)) { 
          AstNodePtr r = s;
          SymbolicVal nstepval = stepval * unrollsize;
          SymbolicVal nubval = ubval;

          bool hasleft = true, negativeStep = (stepval < 0);
          std::vector<AstNodePtr> bodylist;
          AstNodePtr leftbody, lefthead;

          int stepnum=0, loopnum = 0;
          SymbolicVal loopval = ubval - lbval + 1;
          if (stepval.isConstInt(stepnum) && loopval.isConstInt(loopnum) 
               && !(loopnum % stepnum)) {
            // Check if there are leftover iterations if loop step is not 1
             hasleft = false; 
          }
          else {
          // Create a loop for leftover  iterations when loopnum % stepnum !=0 
             nubval = ubval - SymbolicVal(unrollsize - 1);
             if (opt & COND_LEFTOVER) {
                 leftbody = fa.CreateBlock();
                 lefthead = leftbody;
             }
             else {
                 leftbody = fa.CopyAstTree(body);
                 lefthead = fa.CreateLoop( ivar.CodeGen(fa), 
                                           AstNodePtr(), 
                                           ubval.CodeGen(fa), 
                                           stepval.CodeGen(fa), leftbody,
                                           negativeStep);
             }
          }
          fa.RemoveStmt(body);
          AstNodePtr s1 = fa.CreateLoop(ivar.CodeGen(fa), lbval.CodeGen(fa),
                                          nubval.CodeGen(fa), 
                                          nstepval.CodeGen(fa), body,
                                           negativeStep);
          fa.ReplaceAst( s,s1);
          r = s1; 

          AstNodePtr origbody = fa.CopyAstTree(body);
          std::string nvarname = "";
          SymbolicVal nvar;
          if (opt & USE_NEWVAR) {
               nvarname = fa.NewVar(fa.GetType("int"),"",true,body, ivar.CodeGen(fa)); 
               nvar = SymbolicVar(nvarname,body);
          }
          bodylist.push_back(body);
          // Generate unrolled loop's body
          for (int i = 1; i < unrollsize; ++i) {
              AstNodePtr bodycopy = fa.CopyAstTree(origbody);
              if (opt & USE_NEWVAR) {
                 AstNodePtr nvarassign = 
                     fa.CreateAssignment(nvar.CodeGen(fa), (nvar+1).CodeGen(fa));
                 fa.BlockAppendStmt( body, nvarassign);
                 AstTreeReplaceVar repl(ivar, nvar);
                 repl( fa, bodycopy);
              }
              else {
                 AstTreeReplaceVar repl(ivar, ivar+i);
                 repl( fa, bodycopy);
              }
              fa.BlockAppendStmt( body, bodycopy);
              bodylist.push_back(bodycopy);
              if (hasleft && (opt & COND_LEFTOVER)) {
                 AstNodePtr cond = 
                      fa.CreateBinaryOP( AstInterface::BOP_LE, ivar.CodeGen(fa), (ubval-(i-1)).CodeGen(fa));
                 AstNodePtr body1 = fa.CopyAstTree(bodylist[i-1]);
                 AstNodePtr ifstmt =  fa.CreateIf( cond, body1);
                 fa.BlockAppendStmt( leftbody, ifstmt);
                 leftbody = body1;
              }
          }
          // Insert the loop for leftover iterations when iterationCount%step !=0
          if (hasleft) {
              fa.InsertStmt( r, lefthead, false, true);
          }
          r  = s;
          return true;
   }
   return false;
}
