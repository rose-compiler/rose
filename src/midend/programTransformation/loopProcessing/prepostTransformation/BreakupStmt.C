#include <BreakupStmt.h>
#include <DepInfoAnal.h>
#include <SinglyLinkedList.h>
#include <LoopTransformInterface.h>
#include <assert.h>
#include <CommandOptions.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

size_t BreakupStatement::breaksize = 0;

std::string BreakupStatement:: cmdline_help()
   {
    return "-bs <stmtsize> : break up statements in loops at <stmtsize>";
   }

void BreakupStatement:: cmdline_configure(const std::vector<std::string>& argv,
                                std::vector<std::string>* unknown_args)
{
   unsigned index=0;
   if (!cmdline_find(argv,index,"-bs", unknown_args)) return;
   ++index;
   if (index < argv.size())
       breaksize = atoi( argv[index].c_str());
   if (breaksize <= 0) {
         std::cerr << "invalid breaking size. Use default (12)\n";
         breaksize = 12;
   }
   else {
       ++index;
       std::cerr << "breaking size = " << breaksize << "\n";
   }
   if (unknown_args != 0)
         append_args(argv,index,*unknown_args);
}

bool BreakupStatement::operator() ( AstInterface& fa, const AstNodePtr& s,
                                    AstNodePtr &res)
{
    AstNodePtr lhs, rhs;
    AstInterface::OperatorEnum opr;
    if (fa.IsAssignment(s, &lhs, &rhs)) {
           AstNodeType lhstype;
           if ( fa.IsExpression( lhs, &lhstype) == AST_NULL)
                 return(false);
           AstNodePtr  exp = rhs;
           SinglyLinkedListWrap<AstNodePtr> refList;
           AstNodePtr opd2, opd1;
           std::list<AstNodePtr> stmtlist;
           while ( fa.IsBinaryOp( exp, &opr, &opd1, &opd2 ) && opr == AstInterface::BOP_PLUS) {
             CollectSinglyLinkedList<AstNodePtr> colref(refList);
             AnalyzeStmtRefs(fa, opd2, colref, colref);
             if (refList.size() >= breaksize) {
                bool alias = false;
                for (SinglyLinkedListWrap<AstNodePtr>::Iterator p(refList); !p.ReachEnd(); ++p) {
                    AstNodePtr r = p.Current();
                    if (LoopTransformInterface::IsAliasedRef(lhs, r) ) {
                        alias = true;
                        break;
                    }
                }
                AstNodePtr newvar = (alias)?
                      fa.CreateVarRef( fa.NewVar( lhstype)) : fa.CopyAstTree(lhs);
                fa.ReplaceAst( opd1, newvar);
                AstNodePtr s1 = fa.CreateAssignment( fa.CopyAstTree(newvar), opd1);
                stmtlist.push_back(s1);
                refList.DeleteAll();
             }
             exp = opd1;
           }
           for (std::list<AstNodePtr>::reverse_iterator p = stmtlist.rbegin(); p != stmtlist.rend(); ++p) {
              AstNodePtr cur = *p;
              fa.InsertStmt(s, cur);
           }
           res = s;
           return true;
    }
    return false;
}

