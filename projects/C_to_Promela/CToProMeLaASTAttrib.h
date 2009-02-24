
#include "iostream"
#define C2PASTATTRIBSTR "ConvertToPromela"

class C2PASTAttribute: public AstAttribute
{
	protected:
	bool convertToPromela;
	public:
	C2PASTAttribute();
	C2PASTAttribute(bool convert);
	void convert();
	void doNotConvert();
	bool toConvert();
	std::string toString();
};
/*
#define C2PAST_VAR_STATE_ATTRIBSTR "VariableIsInPromelaState"

class C2PASTVarStateAttribute: public AstAttribute
{
	protected:
	bool inStateVector;
	bool globalState;
	public:
	C2PASTVarStateAttribute(){inStateVector=false;gobalState=false;}
	C2PASTVarStateAttribute(bool convert,bool global){inStateVector=convert;globalState=global};
	bool isInStateVector(){return inStateVector;};
	bool isGlobalState(){return globalState;};
	void setGlobal(bool val){globalState=val;};
	void setState(bool val){inStateVector=val;};
};*/
