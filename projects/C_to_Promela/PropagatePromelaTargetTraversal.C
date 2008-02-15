#include <rose.h>
#include "PropagatePromelaTargetTraversal.h"
#include "tools.h"

// bool traverse(SgNode * node) {return traverse(node, false);}
bool PropagatePromelaTargetTraversal::traverse(SgNode * node)
{
	//      return traverse(node,bool(false));
	bool retVal=AstBottomUpProcessing <bool>::traverse(node);
	return retVal;
}

bool PropagatePromelaTargetTraversal::evaluateSynthesizedAttribute(SgNode * node,SubTreeSynthesizedAttributes atts)
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
