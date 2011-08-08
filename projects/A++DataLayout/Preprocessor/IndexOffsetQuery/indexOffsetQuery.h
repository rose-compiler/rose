#ifndef ROSE_INDEX_OFFSET_QUERY
#define ROSE_INDEX_OFFSET_QUERY

// We only need a synthesized attribute for this query
typedef string IndexOffsetExpressionQuerySynthesizedAttributeType;

class IndexOffsetQuery : public SgBottomUpProcessing<IndexOffsetExpressionQuerySynthesizedAttributeType>
{
public:
	static IndexOffsetExpressionQuerySynthesizedAttributeType transformation ( SgNode* astNode );

	IndexOffsetExpressionQuerySynthesizedAttributeType evaluateSynthesizedAttribute
	( SgNode* astNode, SubTreeSynthesizedAttributes attributList );

};

// endif for ROSE_INDEX_OFFSET_QUERY
#endif
