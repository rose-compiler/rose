
#include "rose.h"
#include <CFGImpl.h>

#define TEMPLATE_ONLY

#include <CFG_OA.C>
namespace OpenAnalysis{
#ifndef CFGIMPL_TEMPLATE_ONLY
template void BuildCFG<CFGNodeImpl, CFGEdgeImpl>(AstInterface &, AstNodePtr const &, BuildCFGConfig<CFGNodeImpl, CFGEdgeImpl> &);
#endif
};

#include <CFG_ROSE.C>
namespace ROSE_Analysis {
#ifndef CFGIMPL_TEMPLATE_ONLY
template void BuildCFG<CFGNodeImpl, CFGEdgeImpl>(AstInterface &, AstNodePtr const &, BuildCFGConfig<CFGNodeImpl, CFGEdgeImpl> &);
#endif
};

#include <support/IteratorCompound.h>
#include <support/IteratorTmpl.h>
#include <graphs/GraphInterface.h>

#if 1
// DQ (9/4/2005): I think this is Peter's fixup (part of cvs update conflict)
// I'm hoping that it is a better alternative then including the source file.
// The IDGraphCreate.C is better put into the IDGraphCreate.h file (at the bottom)
// this way every file that uses IDGraphCreate.h need not be fixed up to also
// include IDGraphCreate.C.

// PC (9/23/2006): prevent multiple explicit instantiations (controlFlowGraph.C)
#ifndef CFGIMPL_TEMPLATE_ONLY
template class CFGImplTemplate<CFGNodeImpl, CFGEdgeImpl>;
template class IDGraphCreateTemplate<CFGNodeImpl, CFGEdgeImpl>;

// pmp 08JUN05
//   was: template class GraphAccessTemp<CFGNodeImpl, CFGEdgeImpl>::NodeIterator;
template class Iterator2Wrap<GraphNode*, CFGNodeImpl*>;

template class Iterator2ImplTemplate<GraphNode*, CFGNodeImpl*, 
                   IDGraphCreateTemplate<CFGNodeImpl, CFGEdgeImpl>::IDNodeIterator>;
#endif
#else
// DQ (9/4/2005): I think this is Qing's fixup (part of cvs update conflict)
template class CFGImplTemplate<CFGNodeImpl, CFGEdgeImpl>;
#include <IDGraphCreate.C>
template class IDGraphCreateTemplate<CFGNodeImpl, CFGEdgeImpl>;
template class Iterator2ImplTemplate<GraphNode*, CFGNodeImpl*, 
                   IDGraphCreateTemplate<CFGNodeImpl, CFGEdgeImpl>::IDNodeIterator>;
#endif
