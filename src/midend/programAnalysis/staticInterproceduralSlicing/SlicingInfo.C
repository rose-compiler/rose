// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 17,4 MB to 7,0MB
#include "sage3basic.h"

#include "SlicingInfo.h"
#include <string>
using namespace std;


SlicingInfo::SlicingInfo():_markFunction(false), _markStatement(false), sliceFunctionCalls("SliceFunction"),
sliceStatement("SliceTarget")
{
    targets = std::list < SgNode * >();

}

void SlicingInfo::visit(SgNode * node)
{

    if (isSgPragmaDeclaration(node))
    {                           // figure out what to do with this pragma
        SgPragma *pragma = isSgPragmaDeclaration(node)->get_pragma();

        string pragma_name(pragma->get_pragma());


        if (pragma_name == sliceFunctionCalls)
        {
						
            _markFunction = true;
        }
        else if (pragma_name == sliceStatement)
        {
            _markStatement = true;
        }
        // get the next line and mark the statement as` one of the slicing
        // target
				if (_markStatement || _markFunction)
				{
					SgStatement *stmnt = dynamic_cast < SgStatement * >(isSgPragmaDeclaration(node));

					if (stmnt == NULL)
						cout << "error" << endl;
					SgStatement *next = ROSE::getNextStatement(stmnt);

					if (next == NULL)
						cout << "Next is NULL" << endl;
						
					else
					{
						// if the following statement is a loop or a if markthe controlexpression instead of the controlflow-stmt
                                                     if (isSgForStatement(next)) next=isSgForStatement(next)->get_test();
                                                else if (isSgDoWhileStmt(next)) next=isSgDoWhileStmt(next)->get_condition ();
                                                else if (isSgWhileStmt(next)) next=isSgWhileStmt(next)->get_condition();
						else if (isSgIfStmt(next)) next = isSgIfStmt(next)->get_conditional();
						
							
						targets.push_back(node);						
						targets.push_back(next);
					}
				}
        return;
    }

    if (_markFunction)
    {
        if (isSgFunctionDeclaration(node))
        {
            _func = isSgFunctionDeclaration(node);
            _markFunction = false;
        }
    }

    if (_markStatement)
    {
        if (isSgStatement(node))
        {
            _target = isSgStatement(node);
            _markStatement = false;
        }
    }

    return;
}
