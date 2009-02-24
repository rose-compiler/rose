
class IdentifyInlineTargets:public AstBottomUpProcessing <bool>
{
	public:
	IdentifyInlineTargets(){};
	bool traverse(SgNode * node);
	protected:
	virtual bool evaluateSynthesizedAttribute(SgNode * node,SubTreeSynthesizedAttributes atts);
	bool defaultSynthesizedAttribute(bool inh){
		return false;
	}
};
