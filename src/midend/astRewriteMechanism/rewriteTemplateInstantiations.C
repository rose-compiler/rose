#include "sage3basic.h"
#include "astPostProcessing.h"
#include "rewriteTemplateImpl.h"

template class MidLevelRewrite<MidLevelInterfaceNodeCollection>;
template class MidLevelRewrite<HighLevelInterfaceNodeCollection>;
template class AbstractInterfaceNodeCollection<MidLevelCollectionTypedefs>;
template class AbstractInterfaceNodeCollection<HighLevelCollectionTypedefs>;
template class TransformationStringTemplatedType<HighLevelCollectionTypedefs>;
