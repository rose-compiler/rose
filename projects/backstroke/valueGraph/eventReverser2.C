#include "valueGraph.h"


namespace Backstroke
{

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH


void EventReverser::buildForwardAndReverseEvent()
{
	map<OperatorNode*, tuple<VGVertex, VGVertex, VGVertex> > opReversals;
	foreach (VGEdge e, dagEdges_)
	{
		VGVertex src = source(e, valueGraph_);
		VGVertex tar = target(e, valueGraph_);


		// If the source is the root, then it is a state saving.
		if (src == root_)
		{
			// If the target is an available node, skip.
			if (valueGraph_[e]->cost == 0)
				continue;
			cout << "State Saving:" << valueGraph_[tar]->toString() << endl;
		}


		// If the target is an operator node, store it first.
		else if (OperatorNode* opNode = isOperatorNode(valueGraph_[tar]))
		{
			ROSE_ASSERT(isOrderedEdge(valueGraph_[e]));
			if (isOrderedEdge(valueGraph_[e])->index == 0)
				opReversals[opNode].get<1>() = src;
			else
				opReversals[opNode].get<2>() = src;
		}

		else if (OperatorNode* opNode = isOperatorNode(valueGraph_[src]))
		{
			VGVertex result = opReversals[opNode].get<0>() = tar;
			VGVertex operand1 = opReversals[opNode].get<1>();
			VGVertex operand2 = opReversals[opNode].get<2>();

			ROSE_ASSERT(operand1 != nullVertex() &&	operand2 != nullVertex());

			cout << "Reversal: " << valueGraph_[result]->toString() << " = " <<
					valueGraph_[operand1]->toString() << opNode->toString() <<
					valueGraph_[operand2]->toString() << endl;
		}

		// A normal assignment.
		else
		{
			cout << "Reversal: " << valueGraph_[tar]->toString() <<
					" = " << valueGraph_[src]->toString() << endl;
		}
	}

}

} // End of namespace Backstroke
