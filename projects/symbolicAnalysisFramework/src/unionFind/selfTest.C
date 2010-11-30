#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>

#include "common.h"
#include "unionFind.h"
#include "variables.h"
#include "rwAccessLabeler.h"
#include "arrIndexLabeler.h"
using namespace std;

varIDSet getUsedArrays(varIDSet vars);

int main(int argc, char** argv)
{
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	// add the annotations that identify each part of the AST as used for read or write accesses
	rwAccessLabeler::addRWAnnotations(project);
	
	// add the annotations that identify the index expressions of SgArrRefExps
	arrIndexLabeler::addArrayIndexAnnotations(project);
		
	// set the correct results
	list<list<list<string> > > funcs;
		list<list<string> > foo;																								
			list<string> foo_set1;																								
				foo_set1.push_back("ZERO");																								
				foo_set1.push_back("array");
				foo_set1.push_back("indexes");
				foo_set1.push_back("i");																								
				foo_set1.push_back("j");																								
			foo.push_back(foo_set1);																								
			list<string> foo_set2;																								
				foo_set2.push_back("k");
				foo_set2.push_back("l");
			foo.push_back(foo_set2);
		funcs.push_back(foo);																								
		list<list<string> > bar;
			list<string> bar_set1;
				bar_set1.push_back("ZERO");
				bar_set1.push_back("array");
				bar_set1.push_back("indexes");
				bar_set1.push_back("a");
				bar_set1.push_back("b");
				bar_set1.push_back("f");
			bar.push_back(bar_set1);
			list<string> bar_set2;
				bar_set2.push_back("c");
				bar_set2.push_back("d");
			bar.push_back(bar_set2);
			list<string> bar_set3;
				bar_set3.push_back("e");
			bar.push_back(bar_set3);
		funcs.push_back(bar);
	
	
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	list<list<list<string> > >::iterator it_funcs = funcs.begin();
	for(Rose_STL_Container<SgNode*>::const_iterator i = functions.begin();
	    i != functions.end(); 
	    ++i, it_funcs++) {
		SgFunctionDefinition* curFunc = isSgFunctionDefinition(*i);
		ROSE_ASSERT(curFunc);
				
		//SgBasicBlock *funcBody = curFunc->get_body();
		//InterestingNode funcCFG = VirtualCFG::makeInterestingCfg(funcBody);
		DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(curFunc);
		
		varIDSet allVars = getVarRefsInSubtree(curFunc);
		allVars.insert(zeroVar);
		varIDSet usedArrays = getUsedArrays(allVars);
				
		UnionFind uf(allVars);
		varIDSet activeVars = uf.determineInterestSet(/*funcCFG*/funcCFGStart, usedArrays);
		
		list<list<string> >::iterator it_sets = (*it_funcs).begin();
		int j;
		for(UnionFind::iterator it = uf.begin();
		    it!=uf.end(); it++, it_sets++, j++)
		{
			//printf("set %d: ", j);
			
			varIDSet::iterator it_cur_set = it->second->begin();
			list<string>::iterator it_strings = (*it_sets).begin();
			for(; it_cur_set != it->second->end(); it_cur_set++, it_strings++)
			{
				//printf("%s   ", (*it_strings).c_str());
				if(((varID)*it_cur_set).str() != *it_strings)
				{
					printf("ERROR: correct variable %s did not match detected variable %s!\n", (*it_strings).c_str(), ((varID)*it_cur_set).str().c_str());
					return -1;
				}
			}
			if(it_strings!=(*it_sets).end()) { printf("ERROR: too few strings in current set!"); return -1;}
			//printf("\n");
		}
		if(it_sets!=(*it_funcs).end()) { printf("ERROR: too few sets in current function!"); return -1;}
	}
	if(it_funcs!=funcs.end()) { printf("ERROR: too few functions!"); return -1;}
	
	printf("union_find: PASSED!\n");
	
	return 0;
}

varIDSet getUsedArrays(varIDSet vars)
{
	varIDSet arrays;
	for(varIDSet::iterator it=vars.begin(); it!=vars.end(); it++)
	{
		if(((varID)(*it)).isArrayType())
			arrays.insert((*it));
	}
	return arrays;
}
