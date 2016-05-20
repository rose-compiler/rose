#include "rose.h"

#include "PromelaMarker.h"
#include "tools.h"
#include <string>

using namespace std;


PromelaMarker::PromelaMarker():toProMeLaString("SPIN_TARGET"),allToProMeLaString("SPIN_TARGET_ALL"),markAllForTransformation(false)
{
}
void PromelaMarker::traverse(SgNode* node, Order treeTraversalOrder)
{
	AstSimpleProcessing::traverse(node,treeTraversalOrder);
	if (markAllForTransformation)
	{
		cout <<"selecting all of program"<<endl;
		list<SgNode*> stmts= NodeQuery::querySubTree(node,V_SgStatement);
//modelTargetList.clear();
		for (list<SgNode*>::iterator i=stmts.begin();i!=stmts.end();i++)
		{
			markForPromelaTransformation(*i);
//	modelTargetList.push_back(*i);
		}
	}
}

void PromelaMarker::visit(SgNode * node)
{

	if (isSgPragmaDeclaration(node))
	{                           // figure out what to do with this pragma
		SgPragma *pragma = isSgPragmaDeclaration(node)->get_pragma();

		string pragma_name(pragma->get_pragma());
		cout <<"found: #pragma "<<pragma_name<<endl;

		if (pragma_name == toProMeLaString)
		{

			// get the next line and mark the statement as` one of the slicing
			// target
			SgStatement *stmnt = dynamic_cast < SgStatement * >(isSgPragmaDeclaration(node));

			if (stmnt == NULL)
				cout << "error" << endl;
			SgStatement *next = rose::getNextStatement(stmnt);
			markForPromelaTransformation(next);
//	modelTargetList.push_back(next);

		}
		else if (pragma_name == allToProMeLaString)
		{
			markAllForTransformation=true;
		}
	}
    return;
}
