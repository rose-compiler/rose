

class PropagatePromelaTargetTraversal:public AstBottomUpProcessing <bool>
{
	public:
	PropagatePromelaTargetTraversal(){};
	bool traverse(SgNode * node);
	protected:
	virtual bool evaluateSynthesizedAttribute(SgNode * node,SubTreeSynthesizedAttributes atts);
	bool defaultInheritedAttribute(){return false;}
	bool defaultSynthesizedAttribute(bool inh){
		return false;
	}
	bool defaultSynthesizedAttribute(){
		return false;
	}
};
