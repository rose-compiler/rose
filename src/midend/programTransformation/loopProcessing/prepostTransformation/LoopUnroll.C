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
#include <AutoTuningInterface.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

unsigned LoopUnrolling::unrollsize = 0;
LoopUnrolling::UnrollOpt LoopUnrolling::opt = DEFAULT;
std::string LoopUnrolling:: cmdline_help()
   {
    return "-unroll [-locond] [-nvar] [poet] <-unrollsize> : unrolling innermost loops at <unrollsize>";
   }

void LoopUnrolling::
cmdline_configure(const std::vector<std::string>& argv,
                                std::vector<std::string>* unknown_args)
{
     unsigned index=0;
     if (!cmdline_find(argv, index, "-unroll", unknown_args)) return;
     ++index;
     if (index < argv.size() && argv[index] == "-locond") {
        opt = (LoopUnrolling::UnrollOpt)(opt | LoopUnrolling::COND_LEFTOVER);
        ++index;
     }
     else if (index < argv.size() && argv[index] == "-lo_skip") { ++index; }
     else opt = (LoopUnrolling::UnrollOpt)(opt | LoopUnrolling::COND_LEFTOVER);
     if (index < argv.size() && argv[index] == "-nvar") {
        opt = (LoopUnrolling::UnrollOpt)(opt | LoopUnrolling::USE_NEWVAR);
        ++index;
     }
     if (index < argv.size() && argv[index] == "poet") {
          opt = (LoopUnrolling::UnrollOpt)(opt | LoopUnrolling::POET_TUNING);
          ++index;
     }
     if (index < argv.size() && ((unrollsize = atoi(argv[index].c_str())) > 0))
         ++index;
     else
        {
             std::cerr << "invalid unrolling size. Use default (4)\n";
             unrollsize = 4;
        }
     if (unknown_args != 0)
         append_args(argv,index,*unknown_args);
}

bool LoopUnrolling::operator() ( AstInterface& fa, const AstNodePtr& s, AstNodePtr& r)
{
   bool isLoop = false;
   if (enclosingloop == s || (enclosingloop == AST_NULL && (isLoop = fa.IsLoop(s)))) {
       for (enclosingloop = fa.GetParent(s);
            enclosingloop != AST_NULL && !fa.IsLoop(enclosingloop);
            enclosingloop = fa.GetParent(enclosingloop));
       if (!isLoop)
          return false;
   }

   AstNodePtr body;
   SymbolicVal stepval, ubval, lbval;
   SymbolicVar ivar;
   if (!SymbolicValGenerator::IsFortranLoop(fa, s, &ivar, &lbval, &ubval, &stepval, &body))
      return false;

   if (opt & POET_TUNING) {
     AutoTuningInterface* tune = LoopTransformInterface::getAutoTuningInterface();
     if (tune == 0) {
        std::cerr << "ERROR: AutoTuning Interface not defined!\n";
        assert(0);
     }
     tune->UnrollLoop(fa,s, unrollsize);
   }
   else {
          AstNodePtr r = s;
          SymbolicVal nstepval = stepval * unrollsize;
          SymbolicVal nubval = ubval;

          bool hasleft = true, negativeStep = (stepval < 0);
          std::vector<AstNodePtr> bodylist;
          AstNodePtr leftbody, lefthead;

          int stepnum=0, loopnum = 0;
          SymbolicVal loopval = ubval - lbval + 1;
          if (stepval.isConstInt(stepnum) && loopval.isConstInt(loopnum)
               && !(loopnum % (stepnum * unrollsize))) {
std::cerr << "loop val = " << loopnum << "; step: " << stepnum * unrollsize << "\n";
             hasleft = false;
          }
          else {
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
               nvarname = fa.NewVar(fa.GetType("int"),"",true,false,body, ivar.CodeGen(fa));
               nvar = SymbolicVar(nvarname,body);
          }
          bodylist.push_back(body);
          for (unsigned i = 1; i < unrollsize; ++i) {
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
                 AstNodePtr body1 = fa.CopyAstTree(bodylist[i]);
                 AstNodePtr ifstmt =  fa.CreateIf( cond, body1);
                 fa.BlockAppendStmt( leftbody, ifstmt);
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
