#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <string>

#include "variables.h"
#include "varSets.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "CallGraphTraverse.C"
using namespace std;


int numFails=0;
void check(varIDSet& allVars, list<string>& refVarNm, list<string>& refVarUnprsNm, list<string>& scopes, list<bool>& global);
void check(set<arrayElt>& allVars, list<string>& refVarNm, list<string>& refVarUnprsNm, list<string>& scopes, list<bool>& global);

int main(int argc, char** argv)
{
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);

	cfgUtils::initCFGUtils(project);
	varIDSet allVars = getVarRefsInSubtree(project);
	
	// all variables
	list<string> refVarNm;
	list<string> refVarUnprsNm;
	list<string> scopes;
	list<bool> global;
	scopes.push_back("SgGlobal");                       global.push_back(true);  refVarNm.push_back("globalArr");               refVarUnprsNm.push_back("globalArr");                      
	scopes.push_back("SgNamespaceDefinitionStatement"); global.push_back(true);  refVarNm.push_back("namespaceArr");            refVarUnprsNm.push_back("::name::namespaceArr");                      
	scopes.push_back("SgGlobal");                       global.push_back(true);  refVarNm.push_back("a");                       refVarUnprsNm.push_back("a");                      
	scopes.push_back("SgGlobal");                       global.push_back(true);  refVarNm.push_back("r");                       refVarUnprsNm.push_back("r");                      
	scopes.push_back("SgBasicBlock");                   global.push_back(false); refVarNm.push_back("x");                       refVarUnprsNm.push_back("x");                      
	scopes.push_back("SgBasicBlock");                   global.push_back(false); refVarNm.push_back("y");                       refVarUnprsNm.push_back("y");                      
	scopes.push_back("SgBasicBlock");                   global.push_back(false); refVarNm.push_back("z");                       refVarUnprsNm.push_back("z");                      
	scopes.push_back("SgBasicBlock");                   global.push_back(false); refVarNm.push_back("y");                       refVarUnprsNm.push_back("y");                      
	scopes.push_back("SgBasicBlock");                   global.push_back(false); refVarNm.push_back("z");                       refVarUnprsNm.push_back("z");                      
	scopes.push_back("SgNamespaceDefinitionStatement"); global.push_back(true);  refVarNm.push_back("instanceA.field1.field2"); refVarUnprsNm.push_back("::blah::instanceA.(::classA::field1.::classB::field2)");
	scopes.push_back("SgNamespaceDefinitionStatement"); global.push_back(true);  refVarNm.push_back("instanceA.field2");        refVarUnprsNm.push_back("::blah::instanceA.::classA::field2");       
	scopes.push_back("SgBasicBlock");                   global.push_back(false); refVarNm.push_back("instanceB.field3");        refVarUnprsNm.push_back("instanceB.::classB::field3");       
	scopes.push_back("SgClassDefinition");              global.push_back(false); refVarNm.push_back("field2");                  refVarUnprsNm.push_back("::classA::field2");                      
	scopes.push_back("SgBasicBlock");                   global.push_back(false); refVarNm.push_back("i");                       refVarUnprsNm.push_back("i");
	
	check(allVars, refVarNm, refVarUnprsNm, scopes, global);
	
	// all variables
	varIDSet allArrayVars = getArrayVarRefsInSubtree(project);
	
	list<string> arr_refVarNm;
	list<string> arr_refVarUnprsNm;
	list<string> arr_scopes;
	list<bool> arr_global;
	arr_scopes.push_back("SgGlobal");                       arr_global.push_back(true);  arr_refVarNm.push_back("globalArr");               arr_refVarUnprsNm.push_back("globalArr");                      
	arr_scopes.push_back("SgNamespaceDefinitionStatement"); arr_global.push_back(true);  arr_refVarNm.push_back("namespaceArr");            arr_refVarUnprsNm.push_back("::name::namespaceArr");                      
	arr_scopes.push_back("SgNamespaceDefinitionStatement"); arr_global.push_back(true);  arr_refVarNm.push_back("instanceA.field1.field2"); arr_refVarUnprsNm.push_back("::blah::instanceA.(::classA::field1.::classB::field2)");
	arr_scopes.push_back("SgBasicBlock");                   arr_global.push_back(false); arr_refVarNm.push_back("instanceB.field3");        arr_refVarUnprsNm.push_back("instanceB.::classB::field3");       
	
	check(allArrayVars, arr_refVarNm, arr_refVarUnprsNm, arr_scopes, arr_global);
	
	// array elements (includes index expressions)
	set<arrayElt> allArrayElts = getArrayRefsInSubtree(project);
	
	list<string> arrElt_refVarNm;
	list<string> arrElt_refVarUnprsNm;
	list<string> arrElt_scopes;
	list<bool> arrElt_global;
	arrElt_scopes.push_back("SgGlobal");                       arrElt_global.push_back(true);  arrElt_refVarNm.push_back("globalArr[x][x][x]");                arrElt_refVarUnprsNm.push_back("globalArr[x][x][x]");                      
	arrElt_scopes.push_back("SgNamespaceDefinitionStatement"); arrElt_global.push_back(true);  arrElt_refVarNm.push_back("namespaceArr[0][0][0]");             arrElt_refVarUnprsNm.push_back("::name::namespaceArr[0][0][0]");                      
	arrElt_scopes.push_back("SgNamespaceDefinitionStatement"); arrElt_global.push_back(true);  arrElt_refVarNm.push_back("instanceA.field1.field2[x][(((instanceB.::classB::field3)[x])[x + x])[x * x]]"); arrElt_refVarUnprsNm.push_back("::blah::instanceA.(::classA::field1.::classB::field2)[x][(((instanceB.::classB::field3)[x])[x + x])[x * x]]");
	arrElt_scopes.push_back("SgBasicBlock");                   arrElt_global.push_back(false); arrElt_refVarNm.push_back("instanceB.field3[x * x][x + x][x]"); arrElt_refVarUnprsNm.push_back("instanceB.::classB::field3[x * x][x + x][x]");       
	
	/*for(set<arrayElt>::iterator it = allArrayElts.begin(); it!=allArrayElts.end(); it++)
	{
		cout << "array : "<<(*it).str() << "\n";
		cout << "array reconstructed : "<<(*it).toSgExpression()->unparseToString() << "\n";
	}*/
	check(allArrayElts, arrElt_refVarNm, arrElt_refVarUnprsNm, arrElt_scopes, arrElt_global);
	
	{
		/* Note, currently does not test the constructors varID(SgVarRefExp *refExp) and varID(SgDotExp *dotExp)
		   and method isValidVarExp(SgExpression* exp)*/
		Rose_STL_Container<SgNode*> initNamesList = NodeQuery::querySubTree(project, V_SgInitializedName);
		//initNamesList.sort();
                // Liao 10/8/2010
		std::sort(initNamesList.begin(), initNamesList.end());
		vector<SgInitializedName*> initNamesVec(initNamesList.size());
		int i=0;
		for(Rose_STL_Container<SgNode*>::iterator it=initNamesList.begin(); it!=initNamesList.end(); it++, i++)
		{
			initNamesVec[i] = (SgInitializedName*)*it;
		}
		
		// numVars is initNamesList.size(), rounded to 2
		int numVars = (initNamesList.size()/2)*2;
		
		varID var0a(initNamesVec[0]), var0b(initNamesVec[9]);
		varID var1(initNamesVec[1]);
		
		// test the equality operations that compare varIDs and SgInitializedName
		ROSE_ASSERT(var0a == initNamesVec[0]); ROSE_ASSERT(!(var0a != initNamesVec[0]));
		ROSE_ASSERT(var0b == initNamesVec[9]); ROSE_ASSERT(!(var0b != initNamesVec[9]));
		ROSE_ASSERT(var1  == initNamesVec[1]); ROSE_ASSERT(!(var1  != initNamesVec[1]));
		ROSE_ASSERT(var0a != initNamesVec[9]); ROSE_ASSERT(!(var0a == initNamesVec[9]));
		ROSE_ASSERT(var0b != initNamesVec[1]); ROSE_ASSERT(!(var0b == initNamesVec[1]));
		ROSE_ASSERT(var1  != initNamesVec[0]); ROSE_ASSERT(!(var1  == initNamesVec[0]));
		ROSE_ASSERT(initNamesVec[0] == var0a); ROSE_ASSERT(!(initNamesVec[0] != var0a));
		ROSE_ASSERT(initNamesVec[9] == var0b); ROSE_ASSERT(!(initNamesVec[9] != var0b));
		ROSE_ASSERT(initNamesVec[1] == var1 ); ROSE_ASSERT(!(initNamesVec[1] != var1 ));
		ROSE_ASSERT(initNamesVec[9] != var0a); ROSE_ASSERT(!(initNamesVec[9] == var0a));
		ROSE_ASSERT(initNamesVec[1] != var0b); ROSE_ASSERT(!(initNamesVec[1] == var0b));
		ROSE_ASSERT(initNamesVec[0] != var1 ); ROSE_ASSERT(!(initNamesVec[0] == var1 ));
		
		varID var0_0_10a, var0_0_10b, var0_10_10, var0_5_10, var10_0_10, var10_5_10;
		stringstream c;
		string numStr;
		for(i=0; i<numVars; i++)
		{
			var0_0_10a.add(initNamesVec[0]);
			var0_0_10b.add(initNamesVec[0]);
			var0_10_10.add(initNamesVec[i]);
			var0_5_10.add(initNamesVec[((int)i/2)]);
			var10_0_10.add(initNamesVec[numVars-i-1]);
			var10_5_10.add(initNamesVec[numVars-((int)i/2)-1]);
		}
		varID var0_0_5, var0_5_5, var5_0_5;
		for(i=0; i<((int)(numVars/2)); i++)
		{
			var0_0_5.add(initNamesVec[0]);
			var0_5_5.add(initNamesVec[i]);
			var5_0_5.add(initNamesVec[numVars-i-1]);
		}
		
		ROSE_ASSERT(var0a != var0b);
		var0b.components[0] = initNamesVec[0];
		ROSE_ASSERT(var0a == var0b);
		
		ROSE_ASSERT(var1 == var1);
		ROSE_ASSERT(var1 >= var1);
		ROSE_ASSERT(var1 <= var1);
		ROSE_ASSERT(!(var1 < var1));
		ROSE_ASSERT(!(var1 > var1));
			
		ROSE_ASSERT(var0_0_10a == var0_0_10b);
		ROSE_ASSERT(!(var0_0_10a < var0_0_10b));
		ROSE_ASSERT(!(var0_0_10a > var0_0_10b));
		ROSE_ASSERT(var0_0_10a <= var0_0_10b);
		ROSE_ASSERT(var0_0_10a >= var0_0_10b);
		
		ROSE_ASSERT(var0_0_10a < var0_10_10);
		
		ROSE_ASSERT(var0_5_10 < var0_10_10);
		ROSE_ASSERT(var10_0_10 > var0_5_10);
		ROSE_ASSERT(var10_0_10 >= var0_5_10);
		ROSE_ASSERT(var10_0_10 > var0_5_10);
		ROSE_ASSERT(var10_0_10 >= var0_5_10);
		ROSE_ASSERT(var10_0_10 < var10_5_10);
		ROSE_ASSERT(var10_0_10 <= var10_5_10);
		
		ROSE_ASSERT(var0_0_5 < var0_0_10b);
		ROSE_ASSERT(var0_0_5 <= var0_0_10b);
		ROSE_ASSERT(var0_5_5 < var0_10_10);
		ROSE_ASSERT(var0_5_5 <= var0_10_10);
		ROSE_ASSERT(var5_0_5 < var10_0_10);
		ROSE_ASSERT(var5_0_5 <= var10_0_10);
		
		for(i=0; i<numVars; i++)
			var10_5_10.components[i] = initNamesVec[i];
	
		ROSE_ASSERT(var0_10_10 == var10_5_10);
	}
	
	varIDSet::iterator it;
	list<string>::iterator itS;
	
	list<string> globalVarsStr, globalArraysStr, globalScalarsStr;
	globalVarsStr.push_back("a");              globalScalarsStr.push_back("a");             
	globalVarsStr.push_back("r");              globalScalarsStr.push_back("r");           
	globalVarsStr.push_back("globalArr");      globalArraysStr.push_back("globalArr");   
	globalVarsStr.push_back("namespaceArr");   globalArraysStr.push_back("namespaceArr");
	globalVarsStr.push_back("instanceA");      globalScalarsStr.push_back("instanceA");     
	varIDSet globalVars = varSets::getGlobalVars(project);
	//printf("global variables:\n");
	for(it = globalVars.begin(), itS=globalVarsStr.begin(); it!=globalVars.end(); it++, itS++)
	{
		//printf("   %s | %s\n", (*it).str().c_str(), (*itS).c_str());
		if((*it).str() != *itS) { printf("ERROR: expecting global var %s but got var %s\n", (*itS).c_str(), (*it).str().c_str()); numFails++; }
	}
	
	varIDSet globalArrays = varSets::getGlobalArrays(project);
	printf("global arrays:\n");
	for(it = globalArrays.begin(), itS=globalArraysStr.begin(); it!=globalArrays.end(); it++, itS++)
	{
		//printf("   %s | %s\n", (*it).str().c_str(), (*itS).c_str());
		if((*it).str() != *itS) { printf("ERROR: expecting global array var %s but got var %s\n", (*itS).c_str(), (*it).str().c_str()); numFails++; }
	}
	
	varIDSet globalScalars = varSets::getGlobalScalars(project);
	printf("global scalars:\n");
	for(it = globalScalars.begin(), itS=globalScalarsStr.begin(); it!=globalScalars.end(); it++, itS++)
	{
		//printf("   %s | %s\n", (*it).str().c_str(), (*itS).c_str());
		if((*it).str() != *itS) { printf("ERROR: expecting global scalar %s but got var %s\n", (*itS).c_str(), (*it).str().c_str()); numFails++; }
	}
	
	map<string, list<string> > localVarsStr, localArraysStr, localScalarsStr;
	{
		list<string> fooLocalVarsStr, fooLocalArraysStr, fooLocalScalarsStr;
		fooLocalVarsStr.push_back("a");              fooLocalScalarsStr.push_back("a");
		fooLocalVarsStr.push_back("b");              fooLocalScalarsStr.push_back("b");
		localVarsStr["foo"] = fooLocalVarsStr;
		localScalarsStr["foo"] = fooLocalScalarsStr;
		localArraysStr["foo"] = fooLocalArraysStr;
	}
	
	{
		list<string> classmethodLocalVarsStr, classmethodLocalArraysStr, classmethodLocalScalarsStr;
		classmethodLocalVarsStr.push_back("i");              classmethodLocalScalarsStr.push_back("i");
		localVarsStr["class_method"] = classmethodLocalVarsStr;
		localScalarsStr["class_method"] = classmethodLocalScalarsStr;
		localArraysStr["class_method"] = classmethodLocalArraysStr;
	}
	
	{
		list<string> mainLocalVarsStr, mainLocalArraysStr, mainLocalScalarsStr;
		mainLocalVarsStr.push_back("x");                        mainLocalScalarsStr.push_back("x");
		mainLocalVarsStr.push_back("y");                        mainLocalScalarsStr.push_back("y");
		mainLocalVarsStr.push_back("z");                        mainLocalScalarsStr.push_back("z");
		mainLocalVarsStr.push_back("q");                        mainLocalScalarsStr.push_back("q");
		mainLocalVarsStr.push_back("y");                        mainLocalScalarsStr.push_back("y");
		mainLocalVarsStr.push_back("z");                        mainLocalScalarsStr.push_back("z");
		mainLocalVarsStr.push_back("instanceB");                mainLocalScalarsStr.push_back("instanceB");
		localVarsStr["main"] = mainLocalVarsStr;
		localScalarsStr["main"] = mainLocalScalarsStr;
		localArraysStr["main"] = mainLocalArraysStr;
	}
	
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	//map<string,  >::iterator funcVarIt, funcArrIt, funcScalIt;
	Rose_STL_Container<SgNode*>::iterator funcIT;
	for(funcIT=functions.begin()/*, funcVarIt=localVarsStr.begin(),
		 funcArrIt=localArraysStr.begin(), funcScalIt=localScalarsStr.begin()*/; 
		 funcIT!=functions.end(); 
		 funcIT++/*, funcVarIt++, funcArrIt++, funcScalIt++*/)
	{
		Function func(isSgFunctionDefinition(*funcIT));
		//printf("local variables, function %s():\n", func.get_name().str());
		varIDSet localVars = varSets::getLocalVars(func);
		list<string>& funcVars = localVarsStr[(string)func.get_name().str()];
		list<string>& funcArr = localArraysStr[(string)func.get_name().str()];
		list<string>& funcScal = localScalarsStr[(string)func.get_name().str()];
			
		if(localVars.size() != funcVars.size()) { printf("ERROR in %s(): expecting %lu local vars but got %lu\n", func.get_name().str(), (unsigned long)(funcVars.size()), (unsigned long) (localVars.size())); numFails++; }
		//printf("   localVars.size() = %d\n", localVars.size());
		list<string>::iterator localStrIt;
		for(it = localVars.begin(), localStrIt=funcVars.begin(); 
		    it!=localVars.end(); it++, localStrIt++)
		{
			//printf("   %s | %s\n", (*it).str().c_str(), (*localStrIt).c_str());
			if((*it).str() != *localStrIt) { printf("ERROR in %s(): expecting local variable var %s but got var %s\n", func.get_name().str(), (*localStrIt).c_str(), (*it).str().c_str()); numFails++; }
		}
	
		varIDSet localArrays = varSets::getLocalArrays(func);
		if(localArrays.size() != funcArr.size()) { printf("ERROR in %s(): expecting %lu local arrays but got %lu\n", func.get_name().str(), (unsigned long)(funcArr.size()), (unsigned long) (localArrays.size())); numFails++; }
		//printf("local arrays, function %s():\n", func.get_name().str());
		for(it = localArrays.begin(), localStrIt=funcArr.begin();
		    it!=localArrays.end(); it++, localStrIt++)
		{
			//printf("   %s | %s\n", (*it).str().c_str(), (*localStrIt).c_str());
			if((*it).str() != *localStrIt) { printf("ERROR in %s(): expecting local array var %s but got var %s\n", func.get_name().str(), (*localStrIt).c_str(), (*it).str().c_str()); numFails++; }
		}
		
		varIDSet localScalars = varSets::getLocalScalars(func);
		if(localScalars.size() != funcScal.size()) { printf("ERROR in %s(): expecting %lu local scalars but got %lu\n", func.get_name().str(), (unsigned long)(funcScal.size()), (unsigned long)(localScalars.size())); numFails++; }
		//printf("local scalars, function %s():\n", func.get_name().str());
		for(it = localScalars.begin(), localStrIt=funcScal.begin();
		    it!=localScalars.end(); it++, localStrIt++)
		{
			//printf("   %s | %s\n", (*it).str().c_str(), (*localStrIt).c_str());
			if((*it).str() != *localStrIt) { printf("ERROR in %s(): expecting local scalar var %s but got var %s\n", func.get_name().str(), (*localStrIt).c_str(), (*it).str().c_str()); numFails++; }
		}
	}
	
	map<string, list<string> > funcRefVarsStr, funcRefArraysStr, funcRefScalarsStr;
	{
		list<string> fooRefVarsStr, fooRefArraysStr, fooRefScalarsStr;
		fooRefVarsStr.push_back("r");              fooRefScalarsStr.push_back("r");
		funcRefVarsStr["foo"] = fooRefVarsStr;
		funcRefScalarsStr["foo"] = fooRefScalarsStr;
		funcRefArraysStr["foo"] = fooRefArraysStr;
	}
	
	{
		list<string> classmethodRefVarsStr, classmethodRefArraysStr, classmethodRefScalarsStr;
		classmethodRefVarsStr.push_back("field2");         classmethodRefScalarsStr.push_back("field2");
		classmethodRefVarsStr.push_back("i");              classmethodRefScalarsStr.push_back("i");
		funcRefVarsStr["class_method"] = classmethodRefVarsStr;
		funcRefScalarsStr["class_method"] = classmethodRefScalarsStr;
		funcRefArraysStr["class_method"] = classmethodRefArraysStr;
	}
	
	{
		list<string> mainRefVarsStr, mainRefArraysStr, mainRefScalarsStr;
		mainRefVarsStr.push_back("a");                        mainRefScalarsStr.push_back("a");
		mainRefVarsStr.push_back("globalArr");                mainRefArraysStr.push_back("globalArr");
		mainRefVarsStr.push_back("namespaceArr");             mainRefArraysStr.push_back("namespaceArr");
		mainRefVarsStr.push_back("instanceA.field1.field2");  mainRefArraysStr.push_back("instanceA.field1.field2");
		mainRefVarsStr.push_back("instanceA.field2");         mainRefScalarsStr.push_back("instanceA.field2");
		mainRefVarsStr.push_back("x");                        mainRefScalarsStr.push_back("x");
		mainRefVarsStr.push_back("y");                        mainRefScalarsStr.push_back("y");
		mainRefVarsStr.push_back("z");                        mainRefScalarsStr.push_back("z");
		mainRefVarsStr.push_back("y");                        mainRefScalarsStr.push_back("y");
		mainRefVarsStr.push_back("z");                        mainRefScalarsStr.push_back("z");
		mainRefVarsStr.push_back("instanceB.field3");         mainRefArraysStr.push_back("instanceB.field3");
		funcRefVarsStr["main"] = mainRefVarsStr;
		funcRefScalarsStr["main"] = mainRefScalarsStr;
		funcRefArraysStr["main"] = mainRefArraysStr;
	}
	
	for(funcIT=functions.begin(); funcIT!=functions.end(); funcIT++)
	{
		Function func(isSgFunctionDefinition(*funcIT));
		//printf("funcRef variables, function %s():\n", func.get_name().str());
		varIDSet funcRefVars = varSets::getFuncRefVars(func);
		list<string>& funcVars = funcRefVarsStr[(string)func.get_name().str()];
		list<string>& funcArr = funcRefArraysStr[(string)func.get_name().str()];
		list<string>& funcScal = funcRefScalarsStr[(string)func.get_name().str()];
			
		if(funcRefVars.size() != funcVars.size()) { printf("ERROR in %s(): expecting %lu referenced vars but got %lu\n", func.get_name().str(), (unsigned long)(funcVars.size()), (unsigned long)(funcRefVars.size())); numFails++; }
		//printf("   funcRefVars.size() = %d\n", funcRefVars.size());
		list<string>::iterator refStrIt;
		for(it = funcRefVars.begin(), refStrIt=funcVars.begin(); 
		    it!=funcRefVars.end(); it++, refStrIt++)
		{
			//printf("   %s | %s\n", (*it).str().c_str(), (*refStrIt).c_str());
			if((*it).str() != *refStrIt) { printf("ERROR in %s(): expecting referenced variable var %s but got var %s\n", func.get_name().str(), (*refStrIt).c_str(), (*it).str().c_str()); numFails++; }
		}
	
		varIDSet funcRefArrays = varSets::getFuncRefArrays(func);
		if(funcRefArrays.size() != funcArr.size()) { printf("ERROR in %s(): expecting %lu referenced arrays but got %lu\n", func.get_name().str(), (unsigned long)(funcArr.size()), (unsigned long)(funcRefArrays.size())); numFails++; }
		//printf("funcRef arrays, function %s():\n", func.get_name().str());
		for(it = funcRefArrays.begin(), refStrIt=funcArr.begin();
		    it!=funcRefArrays.end(); it++, refStrIt++)
		{
			//printf("   %s | %s\n", (*it).str().c_str(), (*refStrIt).c_str());
			if((*it).str() != *refStrIt) { printf("ERROR in %s(): expecting referenced array var %s but got var %s\n", func.get_name().str(), (*refStrIt).c_str(), (*it).str().c_str()); numFails++; }
		}
		
		varIDSet funcRefScalars = varSets::getFuncRefScalars(func);
		if(funcRefScalars.size() != funcScal.size()) { printf("ERROR in %s(): expecting %lu referenced scalars but got %lu\n", func.get_name().str(), (unsigned long)(funcScal.size()), (unsigned long)(funcRefScalars.size())); numFails++; }
		//printf("funcRef scalars, function %s():\n", func.get_name().str());
		for(it = funcRefScalars.begin(), refStrIt=funcScal.begin();
		    it!=funcRefScalars.end(); it++, refStrIt++)
		{
			//printf("   %s | %s\n", (*it).str().c_str(), (*refStrIt).c_str());
			if((*it).str() != *refStrIt) { printf("ERROR in %s(): expecting referenced scalar var %s but got var %s\n", func.get_name().str(), (*refStrIt).c_str(), (*it).str().c_str()); numFails++; }
		}
	}
	
	if(numFails==0)
		printf("variables: PASSED!\n");
	else
		printf("variables: FAILED!\n");

	return 0;
}


void check(set<varID>& allVars, list<string>& refVarNm, list<string>& refVarUnprsNm, list<string>& scopes, list<bool>& global)
{
	// check to see if allVars includes refVarNm
	for(set<varID>::iterator it=allVars.begin(); it!=allVars.end(); it++)
	{
		//printf("allVars: %s\n", (*it).str().c_str());
		list<string>::iterator it2;
		list<string>::iterator it3;
		list<string>::iterator it4;
		list<bool>::iterator it5;
		for(it2=refVarNm.begin(), it3=refVarUnprsNm.begin(), it4=scopes.begin(), it5=global.begin(); 
		    it2!=refVarNm.end(); it2++, it3++, it4++, it5++)
		{
			//printf("    %s/%s\n", (*it2).str().c_str(), (*it3).str().c_str());
			if((*it).str() == *it2)
			{
				//printf("       %s\n", (*it).toSgExpression()->unparseToString().c_str());
				if((*it).toSgExpression()->unparseToString() != *it3)
				{ fprintf(stderr, "ERROR in getVarRefsInSubtree test: toSgExpression()->unparseToString() produces the wrong string (%s) for variable \"%s\"!\n", (*it).toSgExpression()->unparseToString().c_str(), (*it3).c_str()); exit(-1); }
				if((*it).getScope()->class_name() != *it4)
				{ fprintf(stderr, "ERROR in getVarRefsInSubtree test: getScope()->class_name() produces the wrong scope name (%s) for variable \"%s\"! Scope should be %s.\n", (*it).getScope()->class_name().c_str(), (*it3).c_str(), (*it4).c_str()); exit(-1); }
				if((*it).isGlobal()!= *it5)
				{ fprintf(stderr, "ERROR in getVarRefsInSubtree test: getScope()->isGlobal()=%s is wrong! Global should be %s.\n", (*it).isGlobal() ? "true": "false", (*it5)  ? "true": "false"); exit(-1); }
				break;
			}
		}
		if(it2==refVarNm.end())
		{ fprintf(stderr, "ERROR in getVarRefsInSubtree test: found variable \"%s\" not in official set!\n", (*it).str().c_str()); exit(-1); }
	}
	
	// check to see if refVarNm includes allVars
	list<string>::iterator it2;
	list<string>::iterator it3;
	for(it2=refVarNm.begin(), it3=refVarUnprsNm.begin(); 
	    it2!=refVarNm.end(); it2++, it3++)
	{
		//printf("refVarNm: %s\n", (*it2).c_str());
		set<varID>::iterator it;
		for(it=allVars.begin(); it!=allVars.end(); it++)
		{
			if((*it).str() == *it2)
			{
				if((*it).toSgExpression()->unparseToString() != *it3)
				{ fprintf(stderr, "ERROR in getVarRefsInSubtree test: toSgExpression()->unparseToString() produces the wrong string (%s) for variable \"%s\"!\n", (*it).toSgExpression()->unparseToString().c_str(), (*it3).c_str()); exit(-1); }
				break;
			}
		}
		if(it==allVars.end())
		{ fprintf(stderr, "ERROR in getVarRefsInSubtree test: reference variable \"%s\" not found by pass!\n", (*it2).c_str()); exit(-1); }
	}
}

void check(set<arrayElt>& allVars, list<string>& refVarNm, list<string>& refVarUnprsNm, list<string>& scopes, list<bool>& global)
{
	// check to see if allVars includes refVarNm
	for(set<arrayElt>::iterator it=allVars.begin(); it!=allVars.end(); it++)
	{
		//printf("allVars: %s\n", (*it).str().c_str());
		list<string>::iterator it2;
		list<string>::iterator it3;
		list<string>::iterator it4;
		list<bool>::iterator it5;
		for(it2=refVarNm.begin(), it3=refVarUnprsNm.begin(), it4=scopes.begin(), it5=global.begin(); 
		    it2!=refVarNm.end(); it2++, it3++, it4++, it5++)
		{
			//printf("    %s/%s\n", (*it2).str().c_str(), (*it3).str().c_str());
			if((*it).str() == *it2)
			{
				//printf("       %s\n", (*it).toSgExpression()->unparseToString().c_str());
				if((*it).toSgExpression()->unparseToString() != *it3)
				{ fprintf(stderr, "ERROR in getVarRefsInSubtree test: toSgExpression()->unparseToString() produces the wrong string (%s) for variable \"%s\"!\n", (*it).toSgExpression()->unparseToString().c_str(), (*it3).c_str()); exit(-1); }
				if((*it).getScope()->class_name() != *it4)
				{ fprintf(stderr, "ERROR in getVarRefsInSubtree test: getScope()->class_name() produces the wrong scope name (%s) for variable \"%s\"! Scope should be %s.\n", (*it).getScope()->class_name().c_str(), (*it3).c_str(), (*it4).c_str()); exit(-1); }
				if((*it).isGlobal()!= *it5)
				{ fprintf(stderr, "ERROR in getVarRefsInSubtree test: getScope()->isGlobal()=%s is wrong! Global should be %s.\n", (*it).isGlobal() ? "true": "false", (*it5)  ? "true": "false"); exit(-1); }
				break;
			}
		}
		if(it2==refVarNm.end())
		{ fprintf(stderr, "ERROR in getVarRefsInSubtree test: found variable \"%s\" not in official set!\n", (*it).str().c_str()); exit(-1); }
	}
	
	// check to see if refVarNm includes allVars
	list<string>::iterator it2;
	list<string>::iterator it3;
	for(it2=refVarNm.begin(), it3=refVarUnprsNm.begin(); 
	    it2!=refVarNm.end(); it2++, it3++)
	{
		//printf("refVarNm: %s\n", (*it2).c_str());
		set<arrayElt>::iterator it;
		for(it=allVars.begin(); it!=allVars.end(); it++)
		{
			if((*it).str() == *it2)
			{
				if((*it).toSgExpression()->unparseToString() != *it3)
				{ fprintf(stderr, "ERROR in getVarRefsInSubtree test: toSgExpression()->unparseToString() produces the wrong string (%s) for variable \"%s\"!\n", (*it).toSgExpression()->unparseToString().c_str(), (*it3).c_str()); exit(-1); }
				break;
			}
		}
		if(it==allVars.end())
		{ fprintf(stderr, "ERROR in getVarRefsInSubtree test: reference variable \"%s\" not found by pass!\n", (*it2).c_str()); exit(-1); }
	}
}
