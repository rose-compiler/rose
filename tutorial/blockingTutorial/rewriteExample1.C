#include "rose.h"
#include "rewrite.h"


class MyInheritedAttribute
{
	public:
		MyInheritedAttribute () {};
};

class MySynthesizedAttribute
: public HighLevelRewrite::SynthesizedAttribute
{
	public:
		MySynthesizedAttribute() {};
};

class MyTraversal
: public HighLevelRewrite::RewriteTreeTraversal<MyInheritedAttribute,MySynthesizedAttribute>
{
	public:
		MyTraversal () {};
		MyInheritedAttribute evaluateRewriteInheritedAttribute (
				SgNode* astNode,
				MyInheritedAttribute inheritedAttribute );
		MySynthesizedAttribute evaluateRewriteSynthesizedAttribute (
				SgNode* astNode,
				MyInheritedAttribute inheritedAttribute,
				SubTreeSynthesizedAttributes synthesizedAttributeList );
};

MyInheritedAttribute
MyTraversal::evaluateRewriteInheritedAttribute (
		SgNode* astNode,
		MyInheritedAttribute inheritedAttribute )
{
	MyInheritedAttribute returnAttribute;
	return returnAttribute;
}


MySynthesizedAttribute
MyTraversal::evaluateRewriteSynthesizedAttribute (
		SgNode* astNode,
		MyInheritedAttribute inheritedAttribute,
		SubTreeSynthesizedAttributes synthesizedAttributeList )
{
	MySynthesizedAttribute returnAttribute;
	switch(astNode->variantT())
	{
		case V_SgVariableDeclaration: {
				cout << "found V_SgVariableDeclaration " << endl;
				returnAttribute.insert( astNode, string("// declaration found!"), 
						HighLevelCollectionTypedefs::LocalScope, 
						HighLevelCollectionTypedefs::AfterCurrentPosition );
			} break;
		default:
			break;
	} 
	return returnAttribute;
}

	int
main ( int argc, char** argv )
{
        ROSE_INITIALIZE;
	SgProject* project = frontend(argc,argv);
	MyTraversal treeTraversal;
	MyInheritedAttribute inheritedAttribute;

	treeTraversal.traverse(project,inheritedAttribute);
	return backend(project);
}







