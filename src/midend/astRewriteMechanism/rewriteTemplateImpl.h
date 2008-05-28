// Skip inclusion when we are compiling the rewriteTraversal.C file but include it
// in all other cases since it contains only template functions. This is 
// "Explicit Template Instantiation"
#include "rewriteTreeTraversalImpl.h"
#include "rewriteSynthesizedAttributeTemplatesImpl.h"
#include "rewriteMidLevelInterfaceTemplatesImpl.h"
#include "ASTFragmentCollectorTraversalImpl.h"
#include "prefixGenerationImpl.h"
#include "rewriteASTFragementStringTemplatesImpl.h"
#include "nodeCollectionTemplatesImpl.h"
#include "rewriteDebuggingSupportTemplatesImpl.h"
