#include <rose.h>
#include "MarkAST4Unparse.h"
#include "tools.h"

// bool traverse(SgNode * node) {return traverse(node, false);}
bool MarkAST4Unparse::traverse(SgNode * node)
{
	//      return traverse(node,bool(false));
	bool retVal=AstTopDownBottomUpProcessing <bool,bool>::traverse(node,true);
	return retVal;
}
 bool MarkAST4Unparse::evaluateInheritedAttribute(SgNode * node, bool inher)
{
	return true;
}
 bool MarkAST4Unparse::evaluateSynthesizedAttribute(SgNode * node,bool inher,SubTreeSynthesizedAttributes atts)
{
	bool marked=false;
	if (isSgPragmaDeclaration(node))
	{
		LowLevelRewrite::remove(isSgStatement(node));
	}
	
	if (toConvert(node))
	{
		marked=true;
	}
	else
	{
		// check if any of the childs need to be converted
		for (SubTreeSynthesizedAttributes::iterator i = atts.begin(); i != atts.end(); i++)
		{
			if (*i==true)
			{
				markForPromelaTransformation(node);
				 marked=true;
			}
		}	
	}
	// check for the expression!!!
	SgStatement * cond=NULL;
	if (isSgIfStmt(node))
	{
		cond=isSgIfStmt(node)->get_conditional();
	}
	else if (isSgWhileStmt(node))
	{
		cond=isSgWhileStmt(node)->get_condition ();
	}
	if (cond!=NULL)
	{
		// check the conditional
	}
	return marked;
}
bool MarkAST4Unparse::defaultSynthesizedAttribute(bool inh)
{
	return false;
}

