#include "rose_config.h"
#ifdef HAVE_Z3

#ifndef REACHABILITY_Analyzer_Z3_H
#define REACHABILITY_Analyzer_Z3_H

#include"sage3basic.h"
#include"Analyzer.h"
#include"z3++.h"
#include"ContNodeAttribute.h"
#include"PhiAttribute.h"

#ifdef USE_SAWYER_COMMANDLINE
#include"Sawyer/CommandLineBoost.h"
#else
#include<boost/program_options.hpp>
#endif

using namespace std;
using namespace z3;

enum ReachResult {REACHABLE, UNREACHABLE, UNKNOWN};

/*! 
* \author Maximilian Fecke
* \date 2017.
*/
class ReachabilityAnalyzerZ3
{
	private:
	Sawyer::Message::Facility logger;
	Analyzer* analyzer;
	Flow* flow;
	CTIOLabeler* labeler;

	SSAGenerator* ssaGen;
	int RERSUpperBoundForInput; //RERS specific: All inputs are assumed to be from {1, ..., RERSUpperBoundForInput}
	int RERSVerifierErrorNumber; //RERS specific: Reachability of the function call __VERIFIER_error(RERSVerifierErrorNumber) will be checked

	context contextZ3;
	solver solverZ3;

	public:
	ReachabilityAnalyzerZ3(int RERSUpperBoundForInput, int RERSVerifierErrorNumber, Analyzer* analyzer, Sawyer::Message::Facility* logger): RERSUpperBoundForInput(RERSUpperBoundForInput), RERSVerifierErrorNumber(RERSVerifierErrorNumber), solverZ3(contextZ3)
	{
		this->analyzer = analyzer;
		this->logger = *logger;
		flow = analyzer->getFlow();
		labeler = analyzer->getLabeler();

		ssaGen = new SSAGenerator(RERSVerifierErrorNumber, analyzer, logger);
		ssaGen->generateSSAForm();	
	}
	void generateZ3Formulas();
	ReachResult checkReachability();

	private:
	void processNode(Label* label);
	void processReachableNodes(Label* label, Label* condLabel);
	expr toNumberedVar(string varName, int varNumber); 

	expr processSgExpression(SgExpression* ex);

	void processPhiAttribute(PhiAttribute* phiAtt);

	int getSSANumber(SgNode* node);

	LabelSet* getReachableNodes(Label label);

	expr conditionToExpr(Condition* cond);

};


#endif 

#endif // end of "#ifdef HAVE_Z3"
