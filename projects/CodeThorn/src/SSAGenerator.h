//#pragma once
#ifndef SSA_GENERATOR_H
#define SSA_GENERATOR_H

#include"sage3basic.h"
#include"Analyzer.h"
#include"ContNodeAttribute.h"
#include<map>

#ifdef USE_SAWYER_COMMANDLINE
#include"Sawyer/CommandLineBoost.h"
#else
#include<boost/program_options.hpp>
#endif

using namespace std;


class Condition
{
	public:
	virtual string toString() = 0;
	Condition* operator&&(Condition& other);
	Condition* operator||(Condition& other);
	Condition* operator!();

	protected:
	Condition(){};
};

class CondAtomic: public Condition
{
	public:
	Label label;
	CondAtomic(Label label): label(label){}
	string toString();
};

class CondTrue: public Condition
{
	public:
	CondTrue(){}
	string toString();
};

class CondFalse: public Condition
{
	public:
	CondFalse(){}
	string toString();
};

class CondNeg: public Condition
{
	public:
	Condition* cond;
	CondNeg(Condition* cond): cond(cond){}
	string toString();
};

class CondConj: public Condition
{
	public:
	Condition* cond1;
	Condition* cond2;
	CondConj(Condition* cond1, Condition* cond2): cond1(cond1), cond2(cond2){}
	string toString();
};

class CondDisj: public Condition
{
	public:
	Condition* cond1;
	Condition* cond2;
	CondDisj(Condition* cond1, Condition* cond2): cond1(cond1), cond2(cond2){}
	string toString();
};

/*! 
* \author Maximilian Fecke
* \date 2017.
*/
class SSAGenerator
{
	private:
	Sawyer::Message::Facility logger;
	Analyzer* analyzer;
	Flow* flow;
	CTIOLabeler* labeler;

	bool prepareReachabilityAnalysisZ3; //RERS specific: True iff the generated SSA form is meant to be used by a ReachabilityAnalyzerZ3 object
	int RERSVerifierErrorNumber; //RERS specific: Reachability of the function call __VERIFIER_error(RERSVerifierErrorNumber) will be checked
	std::map<string, int> currentNumberMap = {}; //Associates each variable name s with the highest SSA form number created for s so far
	std::map<Label, Label> conditionMap = {}; //Associates the label of each CFG node with its enclosing if statement's condition label; If a CFG node is not enclosed in an if statement the map contains no entry for it 
	std::map<Label, Condition*> reachingConditionMap = {}; //Associates each label with the compound condition that has to be true so that the related node is reached

	public:
	//Regular constructor that only generates the plain SSA form
	SSAGenerator(Analyzer* analyzer, Sawyer::Message::Facility* logger)
	{
		this->analyzer = analyzer;
		this->logger = *logger;
		flow = analyzer->getFlow();
		labeler = analyzer->getLabeler();
		prepareReachabilityAnalysisZ3 = false;	
	}


	//RERS specific constructor
	//Additionally generates SSA form numbers and phi statements for calls to __VERIFIER_error(RERSVerifierErrorNumber)
	//Use if the generated SSA form is meant to be used by a ReachabilityAnalyzerZ3 object 
	SSAGenerator(int RERSVerifierErrorNumber, Analyzer* analyzer, Sawyer::Message::Facility* logger): SSAGenerator(analyzer, logger)
	{
		this->RERSVerifierErrorNumber = RERSVerifierErrorNumber;
		prepareReachabilityAnalysisZ3 = true;	
	}

	
	void generateSSAForm();
	int currentNumber(string s, Label* condLabel, bool inTrueBranch);
	bool isExitOrContOrContOfPred(Label* label1, Label* label2);	
	Label* getContinueLabel(Label label);

	private:
	void findConditionNodes();
	pair<Label, BranchReturns> findContinueNodes(Label* label, Label* callingCondLabel);
	ContNodeAttribute* getContinueNodeAttr(Label label);
	
	int nextNumber(string s, Label* condLabel, bool inTrueBranch);
	void processNode(Label* label, Label* condLabel, bool inTrueBranch);

	void findReachingConditions(Label* label, Label* condLabel, bool inTrueBranch, bool updateContNode);
	void updateReachCond(Label* label, Condition* cond);
	Condition* getReachCond(Label* label);

	void processSgExpression(SgExpression* ex, Label* condLabel, bool inTrueBranch);
	void processAssignmentTo(SgVarRefExp* varRef, Label* condLabel, bool inTrueBranch);

	bool isCont(Label* label, Label* condLabel);
	bool isContOfPred(Label* label1, Label* label2); 
	bool isExitOrContOfPred(Label* label1, Label* label2);
};

#endif
