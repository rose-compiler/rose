#ifndef _______SDG_H_______
#define	_______SDG_H_______

#include "newPDG.h"

namespace Backstroke
{

struct SDGNode
{
    
};

struct SDGEdge
{
    //! The type of the SDG edge.
    enum EdgeType
    {
        ControlDependence,
        DataDependence,
        ParameterIn,
        ParameterOut,
        FunctionCall,
        Summary
    };
    
};



class SystemDependenceGraph : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
		CFGNodePtr, SDGEdge >
{
    
};

} // end of Backstroke

#endif	/* _______SDG_H_______ */

