#include <rose.h>
#include "IdentifyInlineTargets.h"
#include "tools.h"

// bool traverse(SgNode * node) {return traverse(node, false);}
bool IdentifyInlineTargets::traverse(SgNode * node)
{
	//      return traverse(node,bool(false));
	bool retVal=AstBottomUpProcessing <bool>::traverse(node);
	return retVal;
}

bool IdentifyInlineTargets::evaluateSynthesizedAttribute(SgNode * node,SubTreeSynthesizedAttributes atts)
{
	
	if (toConvert(node))
	{
		return true;
	}
	else
	{
		// check if any of the childs need to be converted
		for (SubTreeSynthesizedAttributes::iterator i = atts.begin(); i != atts.end(); i++)
		{
			if (*i==true)
			{
				markForPromelaTransformation(node);
				return true;
			}
		}	
	}
	return false;
}
