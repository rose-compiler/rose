#ifndef BACKSTROKE_VALUEGRAPH_CFGFILTER_H
#define	BACKSTROKE_VALUEGRAPH_CFGFILTER_H

#include <rose.h>

namespace Backstroke
{

struct BackstrokeCFGNodeFilter
{
	//! Returns if an expression should be filtered off.
	bool filterExpression(SgExpression* expr) const;

	//! Returns if a CFG node should be filtered off.
	bool operator()(const VirtualCFG::CFGNode& cfgNode) const;
};

//typedef CFG<BackstrokeCFGNodeFilter> BackstrokeCFG;


} // end of Backstroke

#endif	/* BACKSTROKE_VALUEGRAPH_CFGFILTER_H */

