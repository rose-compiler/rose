
class MarkAST4Unparse:public AstTopDownBottomUpProcessing <bool,bool>
{
	public:
	MarkAST4Unparse(){};
	void traverse(SgNode * node);
	protected:
	virtual bool evaluateInheritedAttribute(SgNode * node, bool inher);
	virtual bool evaluateSynthesizedAttribute(SgNode * node,bool inher,SubTreeSynthesizedAttributes atts);
	bool defaultSynthesizedAttribute(bool inh);
};
