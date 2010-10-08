// Liao, 10/6/2010, rose.h must be included first !!
#include "rose.h"
//#include <list>
//#include <sstream>
//#include <iostream>
//#include <fstream>
//#include <string.h>
//using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "variables.h"
#include "cfgUtils.h"
#include "DataflowCFG.h"
#if 0  // Liao 10/7/2010, not found
#include "DataflowCFG.cfgToDot.h"
#endif 
#include "CallGraphTraverse.h"
#include "CallGraphTraverse.C"
using namespace std;
using namespace cfgUtils;
//using namespace CallGraph;

typedef enum {assign, conditional_LTEQ, conditional_EQ, fail} exprType;
typedef struct
{
	exprType type;
	short op;
	string i;
	bool negI;
	string j;
	bool negJ;
	string k;
	bool negK;
	long c;
} outcomesType;

outcomesType outcomes[] = 
	{{assign, none,   "i",            false, zeroVar.str(),  false, zeroVar.str(), false, 1},
	 {assign, none,   "j",            false, zeroVar.str(),  false, zeroVar.str(), false, 2},
	 {assign, none,   "q.b",          false, zeroVar.str(),  false, zeroVar.str(), false, 1},
	 {assign, add,    "q.a",          false, "q.b",          false, zeroVar.str(), false, 5},
	 {assign, add,    "i",            false, "j",            false, zeroVar.str(), false, 1},
	 {assign, none,   "q.theClass.y", false, "i",            false, zeroVar.str(), false, 0},
	 {assign, none,   "j",            false, "q.theClass.z", false, zeroVar.str(), false, 0},
	 {assign, none,   "q.theClass.z", false, zeroVar.str(),  false, zeroVar.str(), false, -98},
	 {assign, add,    "q.a",          false, "q.b",          false, zeroVar.str(), false, -5},
	 {assign, add,    "i",            false, "j",            false, zeroVar.str(), false, -12},
	 {assign, add,    "q.b",          false, "q.b",          false, zeroVar.str(), false, 2},
	 {assign, add,    "q.b",          false, "q.b",          false, "q.a",         false, 0},
	 {assign, add,    "q.b",          false, "q.b",          false, "q.a",         true,  0},
	 {assign, add,    "q.theClass.x", false, "q.theClass.x", false, zeroVar.str(), false, -16},
	 {assign, add,    "q.b",          false, "q.b",          false, zeroVar.str(), false, -32},
	 {assign, add,    "q.theClass.x", false, "q.theClass.x", false, zeroVar.str(), false, 33},
	 {assign, add,    "q.theClass.x", false, "q.theClass.x", false, zeroVar.str(), false, 1},
	 {assign, add,    "q.theClass.x", false, "q.theClass.x", false, zeroVar.str(), false, -1},
	 {assign, add,    "q.theClass.x", false, "q.theClass.x", false, zeroVar.str(), false, 1},
	 {assign, add,    "q.theClass.x", false, "q.theClass.x", false, zeroVar.str(), false, -1},
	 {assign, add,    "q.a",          false, "q.b",          false, "j",           false, 0},
	 {assign, add,    "q.a",          false, "q.b",          false, "j",           true,  0},
	 {assign, add,    "q.a",          false, "j",            true,  zeroVar.str(), false, 5},
	 {assign, mult,   "i",            false, "i",            false, oneVar.str(),  false, 5},
	 {assign, divide, "i",            false, "i",            false, "j",           false, 1},
	 {assign, divide, "i",            false, "j",            false, oneVar.str(),  false, -5},
	 {assign, mult,   "i",            false, "j",            false, oneVar.str(),  false, -5},
	 {assign, mult,   "q.b",          false, "q.theClass.x", false, "i",           false, 1},
	 {assign, divide, "q.b",          false, "q.theClass.x", false, "i",           false, 1},
	 
	 {conditional_LTEQ, none, "i",            false, "j",            false, zeroVar.str(), false, 6},
	 {conditional_LTEQ, none, "q.a",          false, "j",            false, zeroVar.str(), false, 1},
	 {conditional_LTEQ, none, "i",            false, "q.a",          false, zeroVar.str(), false, -1},
	 {conditional_LTEQ, none, "q.theClass.z", false, zeroVar.str(),  false, zeroVar.str(), false, 1},
	 {conditional_LTEQ, none, "i",            false, zeroVar.str(),  false, zeroVar.str(), false, -1},
	 {conditional_LTEQ, none, zeroVar.str(),  false, "q.a",          false, zeroVar.str(), false, -1},
	 {conditional_LTEQ, none, zeroVar.str(),  false, "j",            false, zeroVar.str(), false, 1},
	 {conditional_LTEQ, none, zeroVar.str(),  false, "j",            false, zeroVar.str(), false, 981},
	 {conditional_LTEQ, none, "i",            false, "q.theClass.z", false, zeroVar.str(), false, -761},
	 
	 {conditional_LTEQ, none, "j",            false, "i",            false, zeroVar.str(), false, -6},
	 {conditional_LTEQ, none, "j",            false, "q.a",          false, zeroVar.str(), false, -1},
	 {conditional_LTEQ, none, "q.a",          false, "i",            false, zeroVar.str(), false, 1},
	 {conditional_LTEQ, none, zeroVar.str(),  false, "q.theClass.z", false, zeroVar.str(), false, -1},
	 {conditional_LTEQ, none, zeroVar.str(),  false, "i",            false, zeroVar.str(), false, 1},
	 {conditional_LTEQ, none, "q.a",          false, zeroVar.str(),  false, zeroVar.str(), false, 1},
	 {conditional_LTEQ, none, "j",            false, zeroVar.str(), -false, zeroVar.str(), false, -1},
	 {conditional_LTEQ, none, "j",            false, zeroVar.str(),  false, zeroVar.str(), false, -981},
	 {conditional_LTEQ, none, "q.theClass.z", false, "i",            false, zeroVar.str(), false, -761},
	 
	 {conditional_LTEQ, none, "i",            false, "j",            false, zeroVar.str(), false, 5},
	 {conditional_LTEQ, none, "q.a",          false, "j",            false, zeroVar.str(), false, 0},
	 {conditional_LTEQ, none, "i",            false, "q.a",          false, zeroVar.str(), false, -2},
	 {conditional_LTEQ, none, "q.theClass.z", false, zeroVar.str(),  false, zeroVar.str(), false, 0},
	 {conditional_LTEQ, none, "i",            false, zeroVar.str(),  false, zeroVar.str(), false, -2},
	 {conditional_LTEQ, none, zeroVar.str(),  false, "q.a",          false, zeroVar.str(), false, -2},
	 {conditional_LTEQ, none, zeroVar.str(),  false, "j",            false, zeroVar.str(), false, 0},
	 {conditional_LTEQ, none, zeroVar.str(),  false, "j",            false, zeroVar.str(), false, 980},
	 {conditional_LTEQ, none, "i",            false, "q.theClass.z", false, zeroVar.str(), false, 760},
	 
	 {conditional_LTEQ, none, "j",            false, "i",            false, zeroVar.str(), false, -7},
	 {conditional_LTEQ, none, "j",            false, "q.a",          false, zeroVar.str(), false, -2},
	 {conditional_LTEQ, none, "q.a",          false, "i",            false, zeroVar.str(), false, 0},
	 {conditional_LTEQ, none, zeroVar.str(),  false, "q.theClass.z", false, zeroVar.str(), false, -2},
	 {conditional_LTEQ, none, zeroVar.str(),  false, "i",            false, zeroVar.str(), false, 0},
	 {conditional_LTEQ, none, "q.a",          false, zeroVar.str(),  false, zeroVar.str(), false, 0},
	 {conditional_LTEQ, none, "j",            false, zeroVar.str(),  false, zeroVar.str(), false, -2},
	 {conditional_LTEQ, none, "j",            false, zeroVar.str(),  false, zeroVar.str(), false, -982},
	 {conditional_LTEQ, none, "q.theClass.z", false, "i",            false, zeroVar.str(), false, -762},
	 
	 {conditional_EQ, none, "i",            false, "j",            false, zeroVar.str(), false, 6},
	 {conditional_EQ, none, "q.a",          false, "j",            false, zeroVar.str(), false, 1},
	 {conditional_EQ, none, "i",            false, "q.a",          false, zeroVar.str(), false, -1},
	 {conditional_EQ, none, "q.theClass.z", false, zeroVar.str(),  false, zeroVar.str(), false, 1},
	 {conditional_EQ, none, "i",            false, zeroVar.str(),  false, zeroVar.str(), false, -1},
	 {conditional_EQ, none, zeroVar.str(),  false, "q.a",          false, zeroVar.str(), false, -1},
	 {conditional_EQ, none, zeroVar.str(),  false, "j",            false, zeroVar.str(), false, 1},
	 {conditional_EQ, none, zeroVar.str(),  false, "j",            false, zeroVar.str(), false, 981},
	 {conditional_EQ, none, "i",            false, "q.theClass.z", false, zeroVar.str(), false, -761},
	 
	 {conditional_EQ, none, "i",            false, "j",            false, zeroVar.str(), false, 6},
	 {conditional_EQ, none, "q.a",          false, "j",            false, zeroVar.str(), false, 1},
	 {conditional_EQ, none, "i",            false, "q.a",          false, zeroVar.str(), false, -1},
	 {conditional_EQ, none, "q.theClass.z", false, zeroVar.str(),  false, zeroVar.str(), false, 1},
	 {conditional_EQ, none, "i",            false, zeroVar.str(),  false, zeroVar.str(), false, -1},
	 {conditional_EQ, none, zeroVar.str(),  false, "q.a",          false, zeroVar.str(), false, -1},
	 {conditional_EQ, none, zeroVar.str(), -false, "j",            false, zeroVar.str(), false, 1},
	 {conditional_EQ, none, zeroVar.str(),  false, "j",            false, zeroVar.str(), false, 981},
	 {conditional_EQ, none, "i",            false, "q.theClass.z", false, zeroVar.str(), false, 761},
	 
	 {fail, none, "", false, "", false, "", false, 0}, 
	 {fail, none, "", false, "", false, "", false, 0}, 
	 {fail, none, "", false, "", false, "", false, 0}, 
	 {fail, none, "", false, "", false, "", false, 0}, 
	 {fail, none, "", false, "", false, "", false, 0}, 
	 {fail, none, "", false, "", false, "", false, 0}
	 };     
           
int curStmt=0;
int numFails=0;

class traveler : public AstSimpleProcessing
{
	public:
	void visit(SgNode* n)
	{
		SgExprStatement* exprStmt;
		SgExpression* expr=NULL;
		
		if(exprStmt = isSgExprStatement(n))
			expr = exprStmt->get_expression();
		else
			expr = isSgAssignInitializer(n);
			
		if(expr)
		{
			//printf("class_name: %s node: %s\n", n->class_name().c_str(), n->unparseToString().c_str());
			varID i, j, k;
			bool negI, negJ, negK;
			long c;
			short op;
					
			if(outcomes[curStmt].type == assign)
			{
				if(parseAssignment(expr, op, i, j, negJ, k, negK, c))
				{
					if((outcomes[curStmt].i != i.str()) ||
						(outcomes[curStmt].op != op) ||
						(outcomes[curStmt].negI != false) ||
						(outcomes[curStmt].j != j.str()) ||
						(outcomes[curStmt].negJ != negJ) ||
						(outcomes[curStmt].k != k.str()) ||
						(outcomes[curStmt].negK != negK) ||
						(outcomes[curStmt].c != c))
					{
						printf("Error: statement %d <%s | %s>\n", curStmt, expr->class_name().c_str(), expr->unparseToString().c_str());
						printf("Failed match on: %s = %s%s %s %s%s %s %d\n", i.str().c_str(), negJ?"-":"", j.str().c_str(), (op==none?"":(op==add?"+": (op==subtract?"-": (op==mult?"*": (op==divide?"/": "???"))))), negK?"-":"", k.str().c_str(), (op==none?"":(op==add?"+": (op==subtract?"-": (op==mult?"*": (op==divide?"/": "???"))))), c);
						printf("        Correct: %s = %s%s %s %s%s %s %d\n", outcomes[curStmt].i.c_str(), outcomes[curStmt].negJ?"-":"", outcomes[curStmt].j.c_str(), (outcomes[curStmt].op==none?"":(outcomes[curStmt].op==add?"+": (outcomes[curStmt].op==subtract?"-": (outcomes[curStmt].op==mult?"*": (outcomes[curStmt].op==divide?"/": "???"))))), outcomes[curStmt].negK?"-":"", outcomes[curStmt].k.c_str(), (outcomes[curStmt].op==none?"":(outcomes[curStmt].op==add?"+": (outcomes[curStmt].op==subtract?"-": (outcomes[curStmt].op==mult?"*": (outcomes[curStmt].op==divide?"/": "???"))))), outcomes[curStmt].c);
						printf("                 (outcomes[curStmt].type == assign) = %d\n", outcomes[curStmt].type == assign);
						printf("                 (outcomes[curStmt].op == op) = %d\n", (outcomes[curStmt].op == op));
						printf("                 (outcomes[curStmt].i == i.str()) = %d\n", (outcomes[curStmt].i == i.str()));
						printf("                 (outcomes[curStmt].negI == false) = %d\n", (outcomes[curStmt].negI == false));
						printf("                 (outcomes[curStmt].j == j.str()) = %d\n", (outcomes[curStmt].j == j.str()));
						printf("                 (outcomes[curStmt].negJ == negJ) = %d\n", (outcomes[curStmt].negJ == negJ));
						printf("                 (outcomes[curStmt].k == k.str()) = %d\n", (outcomes[curStmt].k == k.str()));
						printf("                 (outcomes[curStmt].negK == negK) = %d\n", (outcomes[curStmt].negK == negK));
						printf("                 (outcomes[curStmt].c(%d) == c(%d)) = %d\n", outcomes[curStmt].c, c, (outcomes[curStmt].c == c));
						numFails++;
					}
				}
				else
				{
					printf("Error: couldn't parse statement %d as assignment!\n", curStmt);
					numFails++;
				}
			}
			else if(outcomes[curStmt].type == conditional_LTEQ)
			{
				if(computeTermsOfIfCondition_LTEQ(expr, i, negI, j, negJ, c))
				{
					if((outcomes[curStmt].type != conditional_LTEQ) ||
						(outcomes[curStmt].i != i.str()) ||
						(outcomes[curStmt].negI != negI) ||
						(outcomes[curStmt].j != j.str()) ||
						(outcomes[curStmt].negJ != negJ) ||
						(outcomes[curStmt].c != c))
					{
						printf("Error: statement %d <%s | %s>\n", curStmt, expr->class_name().c_str(), expr->unparseToString().c_str());
						printf("Failed to match on: %s%s <= %s%s + %d\n", negI?"-":"", i.str().c_str(), negJ?"-":"", j.str().c_str(), c);
						printf("           Correct: %s%s <= %s%s + %d\n", outcomes[curStmt].negI?"-":"", outcomes[curStmt].i.c_str(), outcomes[curStmt].negJ?"-":"", outcomes[curStmt].j.c_str(), outcomes[curStmt].c);
						printf("                   (outcomes[curStmt].i == i.str()) = %d\n", (outcomes[curStmt].i == i.str()));
						printf("                   (outcomes[curStmt].negI == negI) = %d\n", (outcomes[curStmt].negI == negI));
						printf("                   (outcomes[curStmt].j == j.str()) = %d\n", (outcomes[curStmt].j == j.str()));
						printf("                   (outcomes[curStmt].negJ == negJ) = %d\n", (outcomes[curStmt].negJ == negJ));
						printf("                   (outcomes[curStmt].c(%d) == c(%d)) = %d\n", outcomes[curStmt].c, c, (outcomes[curStmt].c == c));
						numFails++;
					}
				}
				else
				{
					printf("Error: couldn't parse statement %d as a condition!\n", curStmt);
					numFails++;
				}
			}
			else if(outcomes[curStmt].type == conditional_EQ)
			{
				if(computeTermsOfIfCondition_EQ(expr, i, negI, j, negJ, c))
				{
					if((outcomes[curStmt].type != conditional_EQ) ||
						(outcomes[curStmt].i != i.str()) ||
						(outcomes[curStmt].negI != negI) ||
						(outcomes[curStmt].j != j.str()) ||
						(outcomes[curStmt].negJ != negJ) ||
						(outcomes[curStmt].c != c))
					{
						printf("Error: statement %d <%s | %s>\n", curStmt, expr->class_name().c_str(), expr->unparseToString().c_str());
						printf("Failed to match on: %s%s == %s%s + %d\n", negI?"-":"", i.str().c_str(), negJ?"-":"", j.str().c_str(), c);
						printf("           Correct: %s%s == %s%s + %d\n", outcomes[curStmt].negI?"-":"", outcomes[curStmt].i.c_str(), outcomes[curStmt].negJ?"-":"", outcomes[curStmt].j.c_str(), outcomes[curStmt].c);
						printf("                   (outcomes[curStmt].i == i.str()) = %d\n", (outcomes[curStmt].i == i.str()));
						printf("                   (outcomes[curStmt].negI == negI) = %d\n", (outcomes[curStmt].negI == negI));
						printf("                   (outcomes[curStmt].j == j.str()) = %d\n", (outcomes[curStmt].j == j.str()));
						printf("                   (outcomes[curStmt].negJ == negJ) = %d\n", (outcomes[curStmt].negJ == negJ));
						printf("                   (outcomes[curStmt].c(%d) == c(%d)) = %d\n", outcomes[curStmt].c, c, (outcomes[curStmt].c == c));
						numFails++;
					}
				}
				else
				{
					printf("Error: couldn't parse statement %d as a condition!\n", curStmt);
					numFails++;
				}
			}
			else if(outcomes[curStmt].type == fail)
			{
				if(parseAssignment(expr, op, i, j, negJ, k, negK, c))
				{ 
					printf("Error: statement %d <%s | %s>, parseAssignment() succeeded, should have failed: %s = %s%s %s %s%s %s %d\n", curStmt, expr->class_name().c_str(), expr->unparseToString().c_str(), i.str().c_str(), negJ?"-":"", j.str().c_str(), (op==none?"":(op==add?"+": (op==subtract?"-": (op==mult?"*": (op==divide?"/": "???"))))), negK?"-":"", k.str().c_str(), (op==none?"":(op==add?"+": (op==subtract?"-": (op==mult?"*": (op==divide?"/": "???"))))), c); 
					numFails++;
				}
				else if(computeTermsOfIfCondition_LTEQ(expr, i, negI, j, negJ, c))
				{ 
					printf("Error: statement %d <%s | %s>, parseAssignment() succeeded, should have failed: %s%s <= %s%s + %d\n", curStmt, expr->class_name().c_str(), expr->unparseToString().c_str(), negI?"-":"", i.str().c_str(), negJ?"-":"", j.str().c_str(), c); 
					numFails++;
				}
			}
			curStmt++;
		}
	}
};

void testExprParsing(SgProject* project)
{
	traveler t;
	
	// test getFuncStartCFG and getFuncEndCFG
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i) {
		SgFunctionDefinition* curFunc = isSgFunctionDefinition(*i);
		ROSE_ASSERT(curFunc);
		
		// if this is not one of the functions for this test
		if(curFunc->get_declaration()->get_name().getString().find("testExprParsing", 0) != 0)
			// skip it
			continue;
		
		t.traverse(curFunc, preorder);
	}
}

void testCFGTraversal(SgProject* project)
{
	// test getFuncStartCFG and getFuncEndCFG
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i) {
		SgFunctionDefinition* curFunc = isSgFunctionDefinition(*i);
		ROSE_ASSERT(curFunc);
		
/*		printf("==================\n");
		printf("Function %s()\n", curFunc->get_declaration()->get_name().getString().c_str());
		printf("==================\n");*/
		
		/*if(curFunc->get_declaration()->get_name().getString() != "add")
			continue;*/
					
		DataflowNode startCFG = getFuncStartCFG(curFunc);
		DataflowNode endCFG = getFuncEndCFG(curFunc);
		set<DataflowNode> visited;
		
		// output the CFG to a file
		/*ofstream fileCFG;
		fileCFG.open((curFunc->get_declaration()->get_name().getString()+"_cfg.dot").c_str());
		cout << "    writing to file "<<(curFunc->get_declaration()->get_name().getString()+"_cfg.dot")<<"\n";
		VirtualCFG::cfgToDot(fileCFG, curFunc->get_declaration()->get_name(), startCFG);
		fileCFG.close();*/
		
		bool reachedEnd=false;
		// test that endCFG is reachable from startCFG
		for(VirtualCFG::iterator it(startCFG); it!=VirtualCFG::iterator::end(); it++)
		{
			visited.insert(*it);
			//printf("Forward: <%s: 0x%x: %s: %d> visited=%d\n", (*it).getNode()->class_name().c_str(), (*it).getNode(), (*it).getNode()->unparseToString().c_str(), (*it).getIndex(), it.visited.find(*it)!=it.visited.end());
			if(*it == endCFG)
			{
				reachedEnd=true;
			}
		}
		if(!reachedEnd){ printf("Error: forward iteration of DataflowNodes from the start CFG node for function \"%s\" did not reach the end CFG node!\n", curFunc->get_declaration()->get_name().getString().c_str()); numFails++; }
			
		bool reachedStart=false;
		// test that startCFG is reachable from endCFG
		for(VirtualCFG::back_iterator it(endCFG); it!=VirtualCFG::iterator::end(); it++)
		{
			//printf("Backward: <%s: 0x%x: %s: %d> visited=%d\n", (*it).getNode()->class_name().c_str(), (*it).getNode(), (*it).getNode()->unparseToString().c_str(), (*it).getIndex(), it.visited.find(*it)!=it.visited.end());
			if(visited.find(*it) == visited.end())
			{
				printf("Error: During backward pass found <%s: 0x%x: %s: %d>, which was not found during forward pass!\n", 
				       (*it).getNode()->class_name().c_str(), (*it).getNode(), (*it).getNode()->unparseToString().c_str(), (*it).getIndex());
				numFails++;
			}
			else
				visited.erase(visited.find(*it));
			
			if(*it == startCFG)
			{
				reachedStart=true;
			}
		}
		if(!reachedStart){ printf("Error: backward iteration of DataflowNodes from the end CFG node for function \"%s\" did not reach the start CFG node!\n", curFunc->get_declaration()->get_name().getString().c_str()); numFails++; }
		// if there are nodes that were visited in the forwards direction but not the backwards direction
		if(visited.size()>0)
		{
			for(set<DataflowNode>::iterator it = visited.begin(); it!=visited.end(); it++)
			{
				printf("Error: During forward pass found <%s: 0x%x: %s: %d>, which was not found during backward pass!\n", 
				       (*it).getNode()->class_name().c_str(), (*it).getNode(), (*it).getNode()->unparseToString().c_str(), (*it).getIndex());
				numFails++;
			}
		}
	}	
}

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  NOTE: THIS TEST DOES NOT CURRENTLY TEST THE DATAFLOW ITERATORS
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#define SgDefaultFile Sg_File_Info::generateDefaultFileInfoForTransformationNode()
	
void testCFGTransform(SgProject* project)
{
//printf("testCFGTransform() code=\"%s\"\n", project->unparseToString().c_str());
	
	// test getFuncStartCFG and getFuncEndCFG
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i) {
		SgFunctionDefinition* curFunc = isSgFunctionDefinition(*i);
		ROSE_ASSERT(curFunc);
		
/*		printf("==================\n");
		printf("Function %s()\n", curFunc->get_declaration()->get_name().getString().c_str());
		printf("==================\n");*/
		
		// if this is not one of the functions for this test
		if(curFunc->get_declaration()->get_name().getString().find("testCFGTransform", 0) != 0)
			// skip it
			continue;
		
		DataflowNode startCFG = getFuncStartCFG(curFunc);
		DataflowNode endCFG = getFuncEndCFG(curFunc);
#if 0   // Liao 10/7/2010, extension not found	
		ofstream fileCFG;
		fileCFG.open((curFunc->get_declaration()->get_name().getString()+"_cfg.dot").c_str());
		cout << "writing to file "<<(curFunc->get_declaration()->get_name().getString()+"_cfg.dot")<<"\n";
		VirtualCFG::cfgToDot(fileCFG, curFunc->get_declaration()->get_name(), startCFG);
		fileCFG.close();
#endif		
		cfgRWTransaction trans;
		trans.beginTransaction();
		
		bool reachedEnd=false;
		
		// find the declaration of foo()
		SgFunctionDeclaration *ib_targetFuncDecl=NULL, *ia_targetFuncDecl=NULL;
		SgFunctionType *ib_targetFuncType=NULL, *ia_targetFuncType=NULL;
		Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
		for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i) {
			//printf("hitting function %s\n", isSgFunctionDeclaration(*i)->get_name().str());
			if(isSgFunctionDeclaration(*i)->get_name().getString() == "insert_before")
			{
				//printf("   found ib!\n");
				ib_targetFuncDecl = isSgFunctionDeclaration(*i);
				ib_targetFuncType = isSgFunctionDeclaration(*i)->get_type();
			}
			else if(isSgFunctionDeclaration(*i)->get_name().getString() == "insert_after")
			{
				//printf("   found ia!\n");
				ia_targetFuncDecl = isSgFunctionDeclaration(*i);
				ia_targetFuncType = isSgFunctionDeclaration(*i)->get_type();
			}
		}
		ROSE_ASSERT(ib_targetFuncDecl);
		ROSE_ASSERT(ib_targetFuncType);
		ROSE_ASSERT(ia_targetFuncDecl);
		ROSE_ASSERT(ia_targetFuncType);
		
		list<DataflowNode> foundNodes;
		int i=0;
		for(VirtualCFG::iterator it(startCFG); it!=VirtualCFG::iterator::end(); it++)
		{
//			printf("Current: <%s | %s>\n", (*it).getNode()->class_name().c_str(), (*it).getNode()->unparseToString().c_str());
			if(isSgAssignOp((*it).getNode()) || isSgPlusPlusOp((*it).getNode()) ||
				isSgLessThanOp((*it).getNode()))
			{
				{
					SgFunctionSymbol* funcSymb = new SgFunctionSymbol(ib_targetFuncDecl);
					SgFunctionRefExp* funcRefExp = new SgFunctionRefExp(SgDefaultFile, funcSymb, ib_targetFuncType);				
/*					printf("<<<\n");
					printf("funcRefExp=%s\n", funcRefExp->unparseToString().c_str());
					printf(">>>\n");*/
					
					SgExprListExp* argList = new SgExprListExp(SgDefaultFile);
					SgIntVal* intI = new SgIntVal(SgDefaultFile, i);
					argList->append_expression(intI);
					intI->set_parent(argList);
/*					printf("<<<\n");
					printf("funcRefExp=%s\n", funcRefExp->unparseToString().c_str());
					printf(">>>\n");*/
					SgFunctionCallExp* funcCall = new SgFunctionCallExp(SgDefaultFile, funcRefExp, argList, ib_targetFuncType);
					//funcRefExp->set_parent(funcCall);
/*					printf("<<<\n");
					printf("funcRefExp->get_parent()=0x%x\n", funcRefExp->get_parent());
					printf("funcRefExp=%s\n", funcRefExp->unparseToString().c_str());
					printf(">>>\n");*/
					
					argList->set_parent(funcCall);
/*					printf("<<<\n");
					printf("funcCall=%s\n", funcCall->unparseToString().c_str());
					printf(">>>\n");
					
*/					
					
					trans.insertBefore(*it, funcCall);
				}
				{
					SgFunctionSymbol* funcSymb = new SgFunctionSymbol(ia_targetFuncDecl);
					SgFunctionRefExp* funcRefExp = new SgFunctionRefExp(SgDefaultFile, funcSymb, ia_targetFuncType);
					SgExprListExp* argList = new SgExprListExp(SgDefaultFile);
					SgIntVal* intI = new SgIntVal(SgDefaultFile, i);
					argList->append_expression(intI);
					intI->set_parent(argList);
					SgFunctionCallExp* funcCall = new SgFunctionCallExp(SgDefaultFile, funcRefExp, argList, ia_targetFuncType);
					funcRefExp->set_parent(funcCall);
					argList->set_parent(funcCall);
					
					trans.insertAfter(*it, funcCall);
				}
				i++;
			}
		}
		
		trans.commitTransaction();
	}
	
	backend(project);
	system("diff rose_test_example.c test_example.valid_rose_output.c > selfTest.out");
	struct stat file;
   stat("selfTest.out",&file);
	//printf("file size %d bytes \n", file.st_size);
	if(file.st_size!=0)
	{
		printf("Error: found differences between rose_test_example.c and the canonical test_example.valid_rose_output.c! Details in selfTest.out.\n");
		numFails++;
	}
}

list<string> topDownFuncNames;
list<int> topDownFuncCounts;
class Int
{
	public:
	int val;
	
	Int()
	{
		val=0;
	}
	
	Int(int val)
	{
		this->val=val;
	}
};

set<pair<CGFunction, CGFunction> > callDepsCGF;
set<pair<SgFunctionDeclaration *, SgFunctionDeclaration *> > callDeps;

class TopDown : public virtual TraverseCallGraphTopDown<Int>
{
	public:
	TopDown(SgIncidenceDirectedGraph* graph) : TraverseCallGraphTopDown<Int>(graph)
	{}
	
	Int visit(const CGFunction* func, list<Int>& fromCallers)
	{
		///printf("TopDown: %s\n", func->get_name().str());
		const Function* funcFound = getFunc(func->get_declaration());
		if(func != funcFound)
		{
			printf("Error in top-down call graph traversal: (func != funcFound) in function %s\n", func->get_declaration()->get_name().str());
			numFails++;
		}
		
		CGFunction fromCGF(func->get_declaration(), graph);
		SgFunctionDeclaration * from = func->get_declaration();
		for(CGFunction::iterator it = func->successors(); it!=func->end(); it++)
		{
			CGFunction toCGF(it.getTarget(functions)->get_declaration(), graph);
			SgFunctionDeclaration * to = it.getTarget(functions)->get_declaration();
			
			pair<CGFunction, CGFunction> edgeCGF(fromCGF, toCGF);
			callDepsCGF.insert(edgeCGF);
			
			pair<SgFunctionDeclaration *, SgFunctionDeclaration *> edge(from, to);
			callDeps.insert(edge);
			//printf("     added edge <0x%x|%s, 0x%x|%s>\n", from.get_declaration(), from.get_name().str(), to.get_declaration(), to.get_name().str());
			//printf("     added edge <0x%x|%s, 0x%x|%s>\n", from, from->get_name().str(), to, to->get_name().str());
		}
		
		// if this is not one of the functions for this test
		if(func->get_declaration()->get_name().getString().find("testCallGraphTraversal", 0) != 0)
		{
			Int res(0);
			// skip it
			return res;
		}
		
		//printf("function: %s\n", func->get_name().str());
		// if we hit the wrong function, report an error
		if(func->get_declaration()->get_name().getString() != topDownFuncNames.front())
		{
			printf("Error in top-down call graph traversal: expected %s but got %s\n", topDownFuncNames.front().c_str(), func->get_declaration()->get_name().str());
			numFails++;
		}
		topDownFuncNames.pop_front();
		
		Int res(1);
		for(list<Int>::iterator it = fromCallers.begin(); it != fromCallers.end(); it++)
		{
			res.val+=(*it).val;
		}
		//printf("  val=%d\n", res.val);
		
		if(res.val != topDownFuncCounts.front())
		{
			printf("Error in top-down call graph traversal, function %s: expected count %d but got count %d\n", func->get_declaration()->get_name().getString().c_str(), topDownFuncCounts.front(), res.val);
			numFails++;
		}
		topDownFuncCounts.pop_front();
		
		return res;
	}
};

list<string> bottomUpFuncs;
list<int> bottomUpCounts;
class BottomUp : public virtual TraverseCallGraphBottomUp<Int>
{
	public:
	BottomUp(SgIncidenceDirectedGraph* graph) : TraverseCallGraphBottomUp<Int>(graph)
	{}
	
	Int visit(const CGFunction* func, list<Int> fromCallees)
	{
		const Function* funcFound = getFunc(func->get_declaration());
		if(func != funcFound)
		{
			printf("Error in bottom-up call graph traversal: (func != funcFound) in function %s\n", func->get_declaration()->get_name().str());
			numFails++;
		}
		
		//printf("BottomUp : %s()\n", func->get_name().str());
		
		CGFunction toCGF(func);
		SgFunctionDeclaration * to = func->get_declaration();
		for(CGFunction::iterator it = func->predecessors(); it!=func->end(); it++)
		{
			CGFunction fromCGF(it.getTarget(functions));
			SgFunctionDeclaration * from = it.getTarget(functions)->get_declaration();
			
			pair<CGFunction, CGFunction> edgeCGF(fromCGF, toCGF);
			pair<SgFunctionDeclaration *, SgFunctionDeclaration *> edge(from, to);
			
			/*printf("     searching for edge <0x%x, 0x%x>\n", from.decl, to.decl);
			if(callDeps.find(edge) != callDeps.end()) { printf("      found edge\n"); }
			else { printf("      !!!\n"); }*/
			
			if(callDepsCGF.find(edgeCGF) == callDepsCGF.end())
			{
				printf("Error in bottom-up call graph traversal: found edge <0x%x|%s, 0x%x|%s> that was not seen in top-down traversal\n", fromCGF.get_declaration(), fromCGF.get_name().str(), toCGF.get_declaration(), toCGF.get_name().str()); 
				numFails++;
			}
			else
				callDepsCGF.erase(callDepsCGF.find(edgeCGF));
				
			if(callDeps.find(edge) == callDeps.end())
			{
				printf("Error in bottom-up call graph traversal: found edge <0x%x|%s, 0x%x|%s> that was not seen in top-down traversal\n", from, from->get_name().str(), to, to->get_name().str()); 
				numFails++;
			}
			else
				callDeps.erase(callDeps.find(edge));
		}
		
		// if this is not one of the functions for this test
		if(func->get_declaration()->get_name().getString().find("testCallGraphTraversal", 0) != 0)
		{
			Int res(0);
			// skip it
			return res;
		}
		//printf("function: %s\n", func->get_name().str());
		// if we hit the wrong function, report an error
		if(func->get_declaration()->get_name().getString() != bottomUpFuncs.front())
		{
			printf("Error in bottom-up call graph traversal: expected %s but got %s\n", bottomUpFuncs.front().c_str(), func->get_declaration()->get_name().getString().c_str());
			numFails++;
		}
		bottomUpFuncs.pop_front();
		
		Int res(1);
		for(list<Int>::iterator it = fromCallees.begin(); it != fromCallees.end(); it++)
		{
			res.val+=(*it).val;
		}
		
		//printf("visit: %s, val=%d  fromCallees.size()=%d\n", func->get_name().str(), res.val, fromCallees.size());
		if(res.val != bottomUpCounts.front())
		{
			printf("Error in bottom-up call graph traversal, function %s: expected count %d but got count %d\n", func->get_declaration()->get_name().getString().c_str(), bottomUpCounts.front(), res.val);
			numFails++;
		}
		bottomUpCounts.pop_front();
		return res;
	}
};

void testCallGraphTraversal(SgProject *project)
{
	// Create the Call Graph
	CallGraphBuilder cgb(project);
	cgb.buildCallGraph();
	SgIncidenceDirectedGraph* graph = cgb.getGraph(); 
#if 0  //Liao, 10/7/2010, not found      
	GenerateDotGraph(graph, "test_example.callgraph.dot");
#endif 	
	//printf("--- TopDown ---\n");
	topDownFuncNames.push_back("testCallGraphTraversal_0");  topDownFuncCounts.push_back(1 );
	topDownFuncNames.push_back("testCallGraphTraversal_1");  topDownFuncCounts.push_back(2 );
	topDownFuncNames.push_back("testCallGraphTraversal_2");  topDownFuncCounts.push_back(4 );
	topDownFuncNames.push_back("testCallGraphTraversal_5");  topDownFuncCounts.push_back(5 );
	topDownFuncNames.push_back("testCallGraphTraversal_3");  topDownFuncCounts.push_back(7 );
	topDownFuncNames.push_back("testCallGraphTraversal_6");  topDownFuncCounts.push_back(8 );
	topDownFuncNames.push_back("testCallGraphTraversal_4");  topDownFuncCounts.push_back(13);
	topDownFuncNames.push_back("testCallGraphTraversal_7");  topDownFuncCounts.push_back(14);
	topDownFuncNames.push_back("testCallGraphTraversal_9");  topDownFuncCounts.push_back(15);
	topDownFuncNames.push_back("testCallGraphTraversal_10"); topDownFuncCounts.push_back(15 );
	topDownFuncNames.push_back("testCallGraphTraversal_8");  topDownFuncCounts.push_back(1 );
	//TraverseCallGraphTopDown<int> td(graph);
	TopDown td(graph);
	td.traverse();
	
	//printf("--- BottomUp ---\n");
	bottomUpFuncs.push_back("testCallGraphTraversal_9");  bottomUpCounts.push_back(1 );
	bottomUpFuncs.push_back("testCallGraphTraversal_10"); bottomUpCounts.push_back(1 );
	bottomUpFuncs.push_back("testCallGraphTraversal_7");  bottomUpCounts.push_back(3 );
	bottomUpFuncs.push_back("testCallGraphTraversal_4");  bottomUpCounts.push_back(4 );
	bottomUpFuncs.push_back("testCallGraphTraversal_6");  bottomUpCounts.push_back(5 );
	bottomUpFuncs.push_back("testCallGraphTraversal_3");  bottomUpCounts.push_back(6 );
	bottomUpFuncs.push_back("testCallGraphTraversal_5");  bottomUpCounts.push_back(7 );
	bottomUpFuncs.push_back("testCallGraphTraversal_2");  bottomUpCounts.push_back(12);
	bottomUpFuncs.push_back("testCallGraphTraversal_1");  bottomUpCounts.push_back(13);
	bottomUpFuncs.push_back("testCallGraphTraversal_0");  bottomUpCounts.push_back(32);
	bottomUpFuncs.push_back("testCallGraphTraversal_8");  bottomUpCounts.push_back(1 );

	
	BottomUp bu(graph);
	bu.traverse();
	
	for(set<pair<CGFunction, CGFunction> >::iterator it = callDepsCGF.begin(); it != callDepsCGF.end(); it++)
	{
		printf("Error in bottom-up call graph traversal: edgeCGF <%s, %s> from top-down traversal was not seen in bottom-up traversal\n", (*it).first.get_name().str(), (*it).second.get_name().str()); 
		numFails++;
	}

	for(set<pair<SgFunctionDeclaration *, SgFunctionDeclaration *> >::iterator it = callDeps.begin(); it != callDeps.end(); it++)
	{
		printf("Error in bottom-up call graph traversal: edge <%s, %s> from top-down traversal was not seen in bottom-up traversal\n", (*it).first->get_name().str(), (*it).second->get_name().str()); 
		numFails++;
	}

	
	annotateNumCallers(graph);
	
	map<string, int> numCallers;
	numCallers["testCallGraphTraversal_0" ] = 0;
	numCallers["testCallGraphTraversal_1" ] = 1;
	numCallers["testCallGraphTraversal_2" ] = 2;
	numCallers["testCallGraphTraversal_3" ] = 2;
	numCallers["testCallGraphTraversal_4" ] = 2;
	numCallers["testCallGraphTraversal_5" ] = 1;
	numCallers["testCallGraphTraversal_6" ] = 1;
	numCallers["testCallGraphTraversal_7" ] = 1;
	numCallers["testCallGraphTraversal_8" ] = 0;
	numCallers["testCallGraphTraversal_9" ] = 1;
	numCallers["testCallGraphTraversal_10"] = 1;
	
 	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
	for (Rose_STL_Container<SgNode*>::iterator it = functions.begin(); it != functions.end(); it++)
	{
		ROSE_ASSERT(isSgFunctionDeclaration(*it));
		Function func(isSgFunctionDeclaration(*it));
		
		// if this is not one of the functions for this test
		if(func.get_name().getString().find("testCallGraphTraversal", 0) != 0)
			// skip it
			continue;
		
		//printf("function %s has %d callers\n", isSgFunctionDefinition(*it)->get_declaration()->get_name().str(), getNumCallers(isSgFunctionDefinition(*it)));
		if(numCallers[func.get_name().getString()] != getNumCallers(&func))
		{
			printf("Error: expected for function %s to have %d callers but found %d callers\n", func.get_declaration()->get_name().str(), numCallers[func.get_name().getString()], getNumCallers(&func));
			numFails++;
		}
	}
}
  
int main(int argc, char** argv)
{
	 time_t start = time(NULL);
	 time_t next;
	 
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	next = time(NULL); printf("frontend time = %d secs\n", next - start); start = next;
	
	/*generatePDF(*project);
	
	next = time(NULL); printf("generatePDF time = %d secs\n", next - start); start = next;*/
	
	initCFGUtils(project);
	
	next = time(NULL); printf("initCFGUtils time = %d secs\n", next - start); start = next;
	
	testExprParsing(project);
	
	next = time(NULL); printf("testExprParsing time = %d secs\n", next - start); start = next;
	
	testCFGTraversal(project);
	
	next = time(NULL); printf("testCFGTraversal time = %d secs\n", next - start); start = next;
	
	testCFGTransform(project);
	
	next = time(NULL); printf("testCFGTransform time = %d secs\n", next - start); start = next;
	
	testCallGraphTraversal(project);
	
	next = time(NULL); printf("testCallGraphTraversal time = %d secs\n", next - start); start = next;

	if(numFails==0)
	{
		printf("cfgUtils: PASSED!\n");
		return 0;
	}
	else
	{
		printf("cfgUtils: FAILED!\n");
		backend(project);
		return -1;
	}
}
