

#include <BreakupStmt.h>
#include <DepInfoAnal.h>
#include <SinglyLinkedList.h>
#include <LoopTransformInterface.h>
#include <assert.h>
#include <CommandOptions.h>

using namespace std;

std::string BreakupStatement:: cmdline_help() 
   { 
    return "-bs <stmtsize> : break up statements in loops at <stmtsize>";
   }

bool BreakupStatement:: cmdline_configure()
{
        vector<string>::const_iterator config = CmdOptions::GetInstance()->GetOptionPosition("-bs");
        if (config == CmdOptions::GetInstance()->opts.end())
            return false;
        ++config;
        assert (config != CmdOptions::GetInstance()->opts.end());
        sscanf( (*config).c_str(), "%zu", &breaksize);
        if (breaksize <= 0) {
           std::cerr << "invalid breaking size. Use default (12)\n";
           breaksize = 12;
        }
       return true;
}
bool BreakupStatement::operator() ( AstInterface& fa, const AstNodePtr& s, 
                                    AstNodePtr &r)
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
             AnalyzeStmtRefs(*la, opd2, colref, colref);
             if (refList.size() >= breaksize) {
                bool alias = false;
                for (SinglyLinkedListWrap<AstNodePtr>::Iterator p(refList); !p.ReachEnd(); ++p) {
                    AstNodePtr r = p.Current();
                    if (la->IsAliasedRef(lhs, r) ) {
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
           r = s;
           return true;
    }
    return false;
}

