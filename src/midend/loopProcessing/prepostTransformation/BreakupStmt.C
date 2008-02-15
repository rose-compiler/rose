#include <general.h>
#include <BreakupStmt.h>
#include <DepInfoAnal.h>
#include <SinglyLinkedList.h>
#include <LoopTransformInterface.h>
#include <assert.h>
#include <CommandOptions.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a header file it is OK here!
#define Boolean int

string BreakupStatement:: cmdline_help() 
   { 
    return "-bs <stmtsize> : break up statements in loops at <stmtsize>";
   }

Boolean BreakupStatement:: cmdline_configure()
{
        const vector<string>& opts = CmdOptions::GetInstance()->GetOptions();
        unsigned int config = CmdOptions::GetInstance()->HasOption("-bs");
        if (config == 0)
            return false;
	// config points to the NEXT argument
	if (config == opts.size()) return false;
        sscanf( opts[config].c_str(), "%d", &breaksize);
        if (breaksize <= 0) {
           cerr << "invalid breaking size. Use default (12)\n";
           breaksize = 12;
        }
       return true;
}
bool BreakupStatement::operator() ( AstInterface& fa, const AstNodePtr& s, 
                                    AstNodePtr &r)
{
    AstNodePtr lhs, rhs;
    if (fa.IsAssignment(s, &lhs, &rhs)) {
           AstNodeType lhstype;
           if (! fa.IsExpression( lhs, &lhstype))
                 return(false);
           AstNodePtr  exp = rhs;
           SinglyLinkedListWrap<AstNodePtr> refList;
           AstNodePtr opd2, opd1;
           list<AstNodePtr> stmtlist;
           while ( fa.IsBinaryPlus( exp, &opd1, &opd2 ) ) {
             CollectSinglyLinkedList<AstNodePtr> colref(refList);
             AnalyzeStmtRefs(*la, opd2, colref, colref);
             if (refList.size() >= (unsigned int)breaksize) {
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
           for (list<AstNodePtr>::reverse_iterator p = stmtlist.rbegin(); p != stmtlist.rend(); ++p) {
              AstNodePtr cur = *p;
              fa.InsertStmt(s, cur);
           }    
           r = s;
           return true;
    }
    return false;
}

