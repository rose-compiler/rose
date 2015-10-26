#include "sage3basic.h"
#include "astPostProcessing.h"
#include "rewriteTemplateImpl.h"

template class MidLevelRewrite<MidLevelInterfaceNodeCollection>;
template class MidLevelRewrite<HighLevelInterfaceNodeCollection>;
template class AbstractInterfaceNodeCollection<MidLevelCollectionTypedefs>;
template class AbstractInterfaceNodeCollection<HighLevelCollectionTypedefs>;
// MS: 11/22/2015: added missing declaration (required for linking with clang++)
template class TransformationStringTemplatedType<MidLevelCollectionTypedefs>;
template class TransformationStringTemplatedType<HighLevelCollectionTypedefs>;

