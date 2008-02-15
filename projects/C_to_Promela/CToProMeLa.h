
class CToProMeLa:public AstTopDownBottomUpProcessing <bool,bool>
{

	public:
	CToProMeLa(std::string filename):f(filename.c_str())
	{
	}
	// bool traverse(SgNode * node) {return traverse(node, false);}
	bool traverse(SgNode * node)
	{
		//      return traverse(node,bool(false));
		bool retVal=AstTopDownBottomUpProcessing <bool,bool>::traverse(node,true);
		f.close();
		return retVal;
	}
	protected:

	virtual bool evaluateInheritedAttribute(SgNode * node, bool inher);
	virtual bool evaluateSynthesizedAttribute(SgNode * node,bool inher,SubTreeSynthesizedAttributes atts);  
	bool defaultSynthesizedAttribute(bool inh);

	private:
	std::ofstream f;
};

