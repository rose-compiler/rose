#include "rose.h"
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

#include "common.h"
#include "variables.h"
#include "cfgUtils.h"
#include "analysisCommon.h"
#include "functionState.h"
#include "latticeFull.h"
#include "analysis.h"
#include "dataflow.h"
#include "dominatorAnalysis.h"
#include "saveDotAnalysis.h"

int numFails=0;

class evaluateAnalysisStates : public UnstructuredPassIntraAnalysis
{
	public:
	SgProject* project;
	string indent;
	evaluateAnalysisStates(SgProject* project, string indent) : project(project), indent(indent)
	{}
	
	void visit(const Function& func, const DataflowNode& n, NodeState& state)
	{
		if(isSgFunctionCallExp(n.getNode()) && isSgFunctionCallExp(n.getNode())->getAssociatedFunctionSymbol()) {
			string funcName = isSgFunctionCallExp(n.getNode())->getAssociatedFunctionSymbol()->get_name().getString();
			if(funcName.find("testFunc")!=string::npos) {
				if(dominatorAnalysisDebugLevel>=1) {
					cout << "#################################################################\n";
					cout << "    " << n.getNode()->unparseToString() << " | " << n.getNode()->class_name() << "\n";
					cout << "#################################################################\n";
				}
				
				const set<DataflowNode>& dominators = getDominators(project, func, n, indent+"    ");
	
				if(dominatorAnalysisDebugLevel >=1) {
					cout << indent << "Dominators:\n";	
					for(set<DataflowNode>::const_iterator dom=dominators.begin(); dom!=dominators.end(); dom++)
						cout << indent << "    " << dom->getNode()->unparseToString() << " | " << dom->getNode()->class_name() << "\n";
				}
				
				//varID funcCallVar = SgExpr2Var(isSgFunctionCallExp(n.getNode()));
				list<string> domStrs;
				if(funcName == "testFunc0") {
					domStrs.push_back("b | SgVarRefExp");
					domStrs.push_back("b++ | SgPlusPlusOp");
					domStrs.push_back("b++; | SgExprStatement");
					domStrs.push_back("testFunc1(); | SgExprStatement");
					domStrs.push_back("testFunc0 | SgFunctionRefExp");
					domStrs.push_back("testFunc1 | SgFunctionRefExp");
					domStrs.push_back("foo | SgFunctionRefExp");
					domStrs.push_back("testFunc0() | SgFunctionCallExp");
					domStrs.push_back("testFunc1() | SgFunctionCallExp");
					domStrs.push_back("foo() | SgFunctionCallExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back("0 | SgIntVal");
					domStrs.push_back("0 | SgAssignInitializer");
					domStrs.push_back("b | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("void testFunc1(){} | SgFunctionDefinition");
					domStrs.push_back("int b; | SgVariableDeclaration");
					domStrs.push_back("int a = 0; | SgVariableDeclaration");
				} else if(funcName == "testFunc1") {
					domStrs.push_back("testFunc1 | SgFunctionRefExp");
					domStrs.push_back("testFunc1() | SgFunctionCallExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back("0 | SgIntVal");
					domStrs.push_back("0 | SgAssignInitializer");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("int a = 0; | SgVariableDeclaration");
				} else if(funcName == "testFunc2") {
					domStrs.push_back("b | SgVarRefExp");
					domStrs.push_back("b++ | SgPlusPlusOp");
					domStrs.push_back("b++; | SgExprStatement");
					domStrs.push_back("testFunc0(); | SgExprStatement");
					domStrs.push_back("testFunc1(); | SgExprStatement");
					domStrs.push_back("foo(); | SgExprStatement");
					domStrs.push_back("testFunc0 | SgFunctionRefExp");
					domStrs.push_back("testFunc1 | SgFunctionRefExp");
					domStrs.push_back("foo | SgFunctionRefExp");
					domStrs.push_back("testFunc2 | SgFunctionRefExp");
					domStrs.push_back("testFunc0() | SgFunctionCallExp");
					domStrs.push_back("testFunc1() | SgFunctionCallExp");
					domStrs.push_back("foo() | SgFunctionCallExp");
					domStrs.push_back("testFunc2() | SgFunctionCallExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back("return 5; | SgReturnStmt");
					domStrs.push_back("5 | SgIntVal");
					domStrs.push_back("0 | SgIntVal");
					domStrs.push_back("0 | SgAssignInitializer");
					domStrs.push_back("b | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("void testFunc0(){} | SgFunctionDefinition");
					domStrs.push_back("void testFunc1(){} | SgFunctionDefinition");
					domStrs.push_back("int b; | SgVariableDeclaration");
					domStrs.push_back("int a = 0; | SgVariableDeclaration");
				} else if(funcName == "testFunc3") {
					domStrs.push_back("b | SgVarRefExp");
					domStrs.push_back("a | SgVarRefExp");
					domStrs.push_back("b++ | SgPlusPlusOp");
					domStrs.push_back("b++; | SgExprStatement");
					domStrs.push_back("testFunc0(); | SgExprStatement");
					domStrs.push_back("testFunc1(); | SgExprStatement");
					domStrs.push_back("foo(); | SgExprStatement");
					domStrs.push_back("testFunc2(); | SgExprStatement");
					domStrs.push_back("a == 0; | SgExprStatement");
					domStrs.push_back("testFunc0 | SgFunctionRefExp");
					domStrs.push_back("testFunc1 | SgFunctionRefExp");
					domStrs.push_back("foo | SgFunctionRefExp");
					domStrs.push_back("testFunc2 | SgFunctionRefExp");
					domStrs.push_back("testFunc3 | SgFunctionRefExp");
					domStrs.push_back("testFunc0() | SgFunctionCallExp");
					domStrs.push_back("testFunc1() | SgFunctionCallExp");
					domStrs.push_back("foo() | SgFunctionCallExp");
					domStrs.push_back("testFunc2() | SgFunctionCallExp");
					domStrs.push_back("testFunc3() | SgFunctionCallExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back("return 5; | SgReturnStmt");
					domStrs.push_back("5 | SgIntVal");
					domStrs.push_back("0 | SgIntVal");
					domStrs.push_back("0 | SgIntVal");
					domStrs.push_back("0 | SgAssignInitializer");
					domStrs.push_back("a == 0 | SgEqualityOp");
					domStrs.push_back("b | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("void testFunc0(){} | SgFunctionDefinition");
					domStrs.push_back("void testFunc1(){} | SgFunctionDefinition");
					domStrs.push_back("void testFunc2(){} | SgFunctionDefinition");
					domStrs.push_back("int b; | SgVariableDeclaration");
					domStrs.push_back("int a = 0; | SgVariableDeclaration");
					domStrs.push_back("if(a == 0) a += 1;else a += 2; | SgIfStmt");
				} else if(funcName == "testFunc4") {
					domStrs.push_back("b | SgVarRefExp");
					domStrs.push_back("a | SgVarRefExp");
					domStrs.push_back("a | SgVarRefExp");
					domStrs.push_back("a | SgVarRefExp");
					domStrs.push_back("b++ | SgPlusPlusOp");
					domStrs.push_back("b++; | SgExprStatement");
					domStrs.push_back("testFunc0(); | SgExprStatement");
					domStrs.push_back("testFunc1(); | SgExprStatement");
					domStrs.push_back("foo(); | SgExprStatement");
					domStrs.push_back("testFunc2(); | SgExprStatement");
					domStrs.push_back("a == 0; | SgExprStatement");
					domStrs.push_back("testFunc3(); | SgExprStatement");
					domStrs.push_back("a--; | SgExprStatement");
					domStrs.push_back("a < 100; | SgExprStatement");
					domStrs.push_back("testFunc0 | SgFunctionRefExp");
					domStrs.push_back("testFunc1 | SgFunctionRefExp");
					domStrs.push_back("foo | SgFunctionRefExp");
					domStrs.push_back("testFunc2 | SgFunctionRefExp");
					domStrs.push_back("testFunc3 | SgFunctionRefExp");
					domStrs.push_back("testFunc4 | SgFunctionRefExp");
					domStrs.push_back("testFunc0() | SgFunctionCallExp");
					domStrs.push_back("testFunc1() | SgFunctionCallExp");
					domStrs.push_back("foo() | SgFunctionCallExp");
					domStrs.push_back("testFunc2() | SgFunctionCallExp");
					domStrs.push_back("testFunc3() | SgFunctionCallExp");
					domStrs.push_back("testFunc4() | SgFunctionCallExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back("return 5; | SgReturnStmt");
					domStrs.push_back("5 | SgIntVal");
					domStrs.push_back("0 | SgIntVal");
					domStrs.push_back("0 | SgIntVal");
					domStrs.push_back("100 | SgIntVal");
					domStrs.push_back("0 | SgAssignInitializer");
					domStrs.push_back("a == 0 | SgEqualityOp");
					domStrs.push_back("a-- | SgMinusMinusOp");
					domStrs.push_back("a < 100 | SgLessThanOp");
					domStrs.push_back("b | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("void testFunc0(){} | SgFunctionDefinition");
					domStrs.push_back("void testFunc1(){} | SgFunctionDefinition");
					domStrs.push_back("void testFunc2(){} | SgFunctionDefinition");
					domStrs.push_back("void testFunc3(){} | SgFunctionDefinition");
					domStrs.push_back("int b; | SgVariableDeclaration");
					domStrs.push_back("int a = 0; | SgVariableDeclaration");
					domStrs.push_back("if(a == 0) a += 1;else a += 2; | SgIfStmt");
					domStrs.push_back("while(a < 100){a++;} | SgWhileStmt");
				} else if(funcName == "testFunc5") {
					domStrs.push_back("b | SgVarRefExp");
					domStrs.push_back("b++ | SgPlusPlusOp");
					domStrs.push_back("b++; | SgExprStatement");
					domStrs.push_back("testFunc0(); | SgExprStatement");
					domStrs.push_back("testFunc1(); | SgExprStatement");
					domStrs.push_back("foo(); | SgExprStatement");
					domStrs.push_back("testFunc0 | SgFunctionRefExp");
					domStrs.push_back("testFunc1 | SgFunctionRefExp");
					domStrs.push_back("foo | SgFunctionRefExp");
					domStrs.push_back("testFunc5 | SgFunctionRefExp");
					domStrs.push_back("testFunc0() | SgFunctionCallExp");
					domStrs.push_back("testFunc1() | SgFunctionCallExp");
					domStrs.push_back("foo() | SgFunctionCallExp");
					domStrs.push_back("testFunc5() | SgFunctionCallExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back("return 5; | SgReturnStmt");
					domStrs.push_back("5 | SgIntVal");
					domStrs.push_back("0 | SgIntVal");
					domStrs.push_back("0 | SgAssignInitializer");
					domStrs.push_back("b | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("void testFunc0(){} | SgFunctionDefinition");
					domStrs.push_back("void testFunc1(){} | SgFunctionDefinition");
					domStrs.push_back("int b; | SgVariableDeclaration");
					domStrs.push_back("int a = 0; | SgVariableDeclaration");
				} else if(funcName == "testFunc6") {
					domStrs.push_back("b | SgVarRefExp");
					domStrs.push_back("a | SgVarRefExp");
					domStrs.push_back("a | SgVarRefExp");
					domStrs.push_back("b++ | SgPlusPlusOp");
					domStrs.push_back("b++; | SgExprStatement");
					domStrs.push_back("testFunc0(); | SgExprStatement");
					domStrs.push_back("testFunc1(); | SgExprStatement");
					domStrs.push_back("foo(); | SgExprStatement");
					domStrs.push_back("testFunc5(); | SgExprStatement");
					domStrs.push_back("a *= 2; | SgExprStatement");
					domStrs.push_back("a < 100; | SgExprStatement");
					domStrs.push_back("testFunc0 | SgFunctionRefExp");
					domStrs.push_back("testFunc1 | SgFunctionRefExp");
					domStrs.push_back("foo | SgFunctionRefExp");
					domStrs.push_back("testFunc5 | SgFunctionRefExp");
					domStrs.push_back("testFunc6 | SgFunctionRefExp");
					domStrs.push_back("testFunc0() | SgFunctionCallExp");
					domStrs.push_back("testFunc1() | SgFunctionCallExp");
					domStrs.push_back("foo() | SgFunctionCallExp");
					domStrs.push_back("testFunc5() | SgFunctionCallExp");
					domStrs.push_back("testFunc6() | SgFunctionCallExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back(" | SgExprListExp");
					domStrs.push_back("return 5; | SgReturnStmt");
					domStrs.push_back("5 | SgIntVal");
					domStrs.push_back("0 | SgIntVal");
					domStrs.push_back("2 | SgIntVal");
					domStrs.push_back("100 | SgIntVal");
					domStrs.push_back("0 | SgAssignInitializer");
					domStrs.push_back("a < 100 | SgLessThanOp");
					domStrs.push_back("a *= 2 | SgMultAssignOp");
					domStrs.push_back("b | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("a | SgInitializedName");
					domStrs.push_back("void testFunc0(){} | SgFunctionDefinition");
					domStrs.push_back("void testFunc1(){} | SgFunctionDefinition");
					domStrs.push_back("void testFunc5(){} | SgFunctionDefinition");
					domStrs.push_back("int b; | SgVariableDeclaration");
					domStrs.push_back("int a = 0; | SgVariableDeclaration");
					domStrs.push_back("while(a < 100){a++;} | SgWhileStmt");
				}
				
				// Compare the set of computed dominators and correct dominators
				set<DataflowNode>::const_iterator compDom;
				list<string>::iterator realDom;
				if(dominators.size() != domStrs.size())
				{ cout << indent << "ERROR at "<<funcName<<"(): Expected "<<domStrs.size()<<" dominators but got "<<dominators.size()<<"!\n"; numFails++; cout.flush(); return;  }
				int i=0;
				for(compDom=dominators.begin(), realDom=domStrs.begin(); compDom!=dominators.end(); compDom++, realDom++, i++)
				{
					string nodeStr = compDom->getNode()->unparseToString() + " | " + compDom->getNode()->class_name();
					if(*realDom != nodeStr)
					{ cout << indent << "ERROR at "<<funcName<<"(): Expected dominator node "<<i<<" \""<<nodeStr<<"\" but got \""<<*realDom<<"\"!\n"; numFails++; cout.flush(); return;  }
				}
			}
		}
	}
};

int main( int argc, char * argv[] ) 
{
	printf("========== S T A R T ==========\n");
	
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	initAnalysis(project);
	
	/*analysisDebugLevel = 0;
	
	SaveDotAnalysis sda;
	UnstructuredPassInterAnalysis upia_sda(sda);
	upia_sda.runAnalysis();*/
	
	dominatorAnalysisDebugLevel = 0;
	analysisDebugLevel = 0;
	if(dominatorAnalysisDebugLevel) {
		printf("**********************************************************\n");
		printf("*****************   Dominator Analysis   *****************\n");
		printf("**********************************************************\n");
	}

	evaluateAnalysisStates eas(project, "    ");
	UnstructuredPassInterAnalysis upia_eas(eas);
	upia_eas.runAnalysis();

	if(numFails==0)
		printf("PASS\n");
	else
		printf("FAIL!\n");
	
	printf("==========  E  N  D  ==========\n");
	
	// Unparse and compile the project (so this can be used for testing)
	return backend(project);
}
