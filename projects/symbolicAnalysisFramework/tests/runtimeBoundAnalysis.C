// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;

#include "common.h"
#include "variables.h"
#include "unionFind.h"
#include "cfgUtils.h"
#include "rwAccessLabeler.h"
#include "arrIndexLabeler.h"
#include "startSaveAnalysis.h"
#include "CallGraph.h"

varIDSet generateVars(SgFunctionDefinition* curFunc);
varIDSet getUsedArrays(varIDSet vars);
set<InterestingNode> findAllCheckpointLocations(InterestingNode &cfg);

int main( int argc, char * argv[] ) 
{
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	// Run internal consistancy tests on AST
	//AstTests::runAllTests(project);	
	varID::selfTest();
	
	// preprocess the AST
	
	// add the annotations that identify each part of the AST as used for read or write accesses
	rwAccessLabeler::addRWAnnotations(project);
	
	// add the annotations that identify the index expressions of SgArrRefExps
	arrIndexLabeler::addArrayIndexAnnotations(project);
		
	CallGraphBuilder cgB(project);
	cgB.buildCallGraph();
	SgIncidenceDirectedGraph *callGraph = cgB.getGraph();
	GenerateDotGraph(callGraph, "callGraph.dot");
	
	generatePDF ( *project );
	
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i) {
		SgFunctionDefinition* curFunc = isSgFunctionDefinition(*i);
		ROSE_ASSERT(curFunc);
				
		SgBasicBlock *funcBody = curFunc->get_body();
		InterestingNode funcCFGStart = cfgUtils::getFuncStartCFG(curFunc);
		InterestingNode funcCFGEnd = cfgUtils::getFuncEndCFG(curFunc);
			
		// output the CFG to a file
		ofstream fileCFG;
		fileCFG.open((curFunc->get_declaration()->get_name().getString()+"_cfg.dot").c_str());
		cout << "writing to file "<<(curFunc->get_declaration()->get_name().getString()+"_cfg.dot")<<"\n";
		VirtualCFG::cfgToDot(fileCFG, curFunc->get_declaration()->get_name(), funcCFGStart);
		fileCFG.close();
		
		// finds all variables and the addresses of their declaration nodes. 
		// Produces all_vars2Name, which maps declaration pointers to the string names of the variables
		varIDSet allVars = generateVars(curFunc);
		varIDSet usedArrays = getUsedArrays(allVars);
		
		// determine the set of variables that may be related to the access patterns of these arrays
		// and set vars2Name to include just these varibles, "0" and the arrays themselves
		// does a slicing of the indeces related to the array(s) being considered
		UnionFind uf(allVars);

		varIDSet activeVars = uf.determineInterestSet( funcCFGStart, usedArrays );
		uf.showAllSets();
		
		// map that contains all checkpoint call locations in the current function.
		// chkptLocs maps the ASTNodePtr that is the actual checkpoint call to its containing CFGNode
		set<InterestingNode> chkptLocs = findAllCheckpointLocations(funcCFGStart);
		
		// create this function's StartSaveAnalysis
		StartSaveAnalysis SSA(project, curFunc, activeVars, usedArrays, chkptLocs, funcCFGStart, funcCFGEnd);
		
		// ===================================================
		// ================= Do the analysis =================
		// ===================================================
		cout << "\n########################################\n";
		cout << "##### Doing Forward Dirty Analysis #####\n";
		cout << "########################################\n";
		SSA.computeForwardDirtyDF();
		
		cout << "\n##########################################\n";
		cout << "##### Doing Forward Written Analysis #####\n";
		cout << "##########################################\n";
		SSA.computeBackwardWrittenDF();
		
		SSA.elimCleanArrays();		
	}
	
	// Unparse and compile the project (so this can be used for testing)
	return backend(project);
}


/*class collectVarAccessesTraversal : public AstBottomUpProcessing<list<SgInitializedName *> >
{
	public:
	// virtual function must be defined
	virtual list<SgInitializedName *> evaluateSynthesizedAttribute ( 
	                      SgNode* n, SynthesizedAttributesList childAttributes );
};

list<SgInitializedName *>
collectVarAccessesTraversal::evaluateSynthesizedAttribute ( SgNode* n, SynthesizedAttributesList childAttributes )
{
	list<SgInitializedName *> localResult;
	SgVarRefExp* var = isSgVarRefExp(n);
	if (var != NULL)
	{
//		printf ("   Found variable %s, declaration %p\n", var->get_symbol()->get_name().str(), var->get_symbol()->get_declaration());
		localResult.push_back(var->get_symbol()->get_declaration());
	}
	
	for(SynthesizedAttributesList::iterator curAttr=childAttributes.begin(); curAttr!=childAttributes.end(); curAttr++)
		for(list<SgInitializedName *>::iterator it2=(*curAttr).begin(); it2!=(*curAttr).end(); it2++)
			localResult.push_front(*it2);
	
	return localResult;
}*/

// initializes global data, finds all variables in the source code, initializing all_vars2Name..
// all_vars2Name is a mapping from the unique id's of any variable used by the user program to their string names
// A variable's unique id is a pointer to its SgInitializeName object.
//    (SgInitializeNames represent declarations/definitions of variables)
// Since multiple variables can have the same structured type (i.e. classes, structs), it is not possible to 
//    differentiate between A.x and B.x using just the pointer to Type.x's SgInitializeName. As such, the unique
//    id of such an expression is a 64-bit value containing the pointer to the variables's SgInitializeName
//    (i.e. A, B above) in the high order 32 bits and the pointer to the field's SgInitializeName (i.e. x above)
//    in the low order 32 bits. Given this structure it is currently only possible to represent a single level 
//    of nesting.
varIDSet generateVars(SgFunctionDefinition* curFunc)
{
	varIDSet allVars;
	printf("getting vars of function %s, %s\n", curFunc->get_declaration()->get_name().str(), curFunc->get_file_info()->isCompilerGenerated()? "Compiler generated": "In Original Code");
	
	if(!curFunc->get_file_info()->isCompilerGenerated())
	{
		allVars = getVarRefsInSubtree(curFunc);
		
		// finally, add the predefind variables (ex: Zero)
		addPredefinedVars(allVars);
	}
	
/*	if(!curFunc->get_file_info()->isCompilerGenerated())
	{
		//collectVarAccessesTraversal collectVarAccesses;
		//allVars = collectVarAccesses.traverse(curFunc);
		
		// add all basic variable references (i.e. those not involved in dot expressions)
		Rose_STL_Container<SgNode*> matches = NodeQuery::querySubTree(curFunc, V_SgVarRefExp);
		for(Rose_STL_Container<SgNode*>::iterator it=matches.begin(); it!=matches.end(); it++)
		{
			SgVarRefExp* refExp = isSgVarRefExp(*it);
			ROSE_ASSERT(refExp);
			// if this SgVarRefExp corresponds to a valid variable
			if(varID::isValidVarExp(refExp))
			{
				varID newVar(refExp);
//printf("isValid(refExp)  newVar=%s\n", newVar.str()); 
				allVars.insert(newVar);
			}
		}
		
		// add the variable references involved in dot expressions
		matches = NodeQuery::querySubTree(curFunc, V_SgDotExp);
		for(Rose_STL_Container<SgNode*>::iterator it=matches.begin(); it!=matches.end(); it++)
		{
			SgDotExp* dot = isSgDotExp(*it);
			ROSE_ASSERT(dot);
			// if this SgDotExp corresponds to a valid variable
			if(varID::isValidVarExp(dot))
			{
				varID newVar(dot);
//printf("isValid(dot)  newVar=%s\n", newVar.str()); 
				allVars.insert(newVar);
			}
		}
		
		for(set<varID>::const_iterator it=allVars.begin(); it!=allVars.end(); it++)
		{
			/*printf ("   Found variable %s, declaration in file %s, line %d\n", 
			        (*it)->get_name().str(), (*it)->get_file_info()->get_filename(), (*it)->get_file_info()->get_line());* /
			printf("   %s Type: %s\n", ((varID)*it).str_linenum(), ((varID)*it).isArrayType()?"Array":"Scalar");
		}
		return allVars;
	}
	
/*	// list of all the variable definitions/declarations this project
	Rose_STL_Container<SgNode *> init = NodeQuery::querySubTree( curFunc, V_SgInitializedName );
	
	for ( Rose_STL_Container<SgNode *>::iterator i = init.begin(); i != init.end(); i++ )
	{
		SgInitializedName *initName = isSgInitializedName( *i );
		ROSE_ASSERT ( initName );
		
			// skip the names of compiler-generated variables
		if ( !initName->get_declaration()->get_file_info()->isCompilerGenerated() &&
					 !initName->get_declaration()->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() )
		{
			// the string name of the current variable
			string varName = initName->get_qualified_name().getString();
			#ifdef DEBUG_FLAG1F
			cout << "!Adding variable: " << varName << "\n";
			#endif
			// skip other types of compiler-generated variables
			if ( varName.find( "__builtin_", 0 ) != 0 && varName != ""  )
				//if ( isSgTypeInt( tV ) || isSgTypeQuad( tV ) || isSgTypeQuadQuad( tV ) ||
				//	isSgTypeShort( tV ) || isSgTypeSignedInt( tV ) ||
				//	isSgTypeSignedQuad( tV ) || isSgTypeSignedShort( tV ) ||
				//	isSgTypeUnsignedInt( tV ) || isSgTypeUnsignedQuad( tV ) ||
				//	isSgTypeUnsignedQuadQuad( tV ) || isSgTypeUnsignedShort( tV ) )
				{
					SgType *tc = initName->get_type();
					// get the type of the current variable
					SgClassType *clsty = isSgClassType( tc );
					// if this is a structured type(struct or class), look at its fields
					if ( clsty )
					{
						SgDeclarationStatement *decl = clsty->get_declaration();
						SgClassDeclaration *cls_decl = isSgClassDeclaration( decl );
						ROSE_ASSERT ( cls_decl );
						SgClassDefinition *cls_def = cls_decl->get_definition();
						SgDeclarationStatementPtrList members = cls_def->get_members();
						// iterate over its members
						for (SgDeclarationStatementPtrList::iterator ds = members.begin();
								 ds != members.end(); ds++ )
						{
							SgVariableDeclaration *varDecl = isSgVariableDeclaration( *ds );
							// if the current member is a variable declaration
							if ( varDecl )
							{
								SgInitializedNamePtrList &inits = varDecl->get_variables();
								// iterate over all the individual variable declarations inside this declaration
								for (SgInitializedNamePtrList::iterator inds = inits.begin();
										 inds != inits.end(); inds++)
								{
									// get the name of the variable being declared and convert it into a string
									SgInitializedName *mb_initName = isSgInitializedName( *inds );
									ROSE_ASSERT ( mb_initName );
									string var_mb = varName + "."
										+	mb_initName->get_qualified_name().getString();
#ifdef DEBUG_FLAG1F2
									cout << "Adding variable: " << var_mb << "\n";
#endif
									// generate this field's 64-bit id from the variable and the field
									// (upper 32 bits are the pointer to the variable's SgInitializedName object,
									//  lower 32 bits are the pointer to the field's SgInitializedName object)
									quad addr = ((quad)((int)mb_initName)) | (((quad)((int)initName)) << 32);
									// add the (unique id -> string name) mapping to all_Vars2Name
									all_vars2Name[addr] = var_mb;
								}
							}
						}
					}
					// if this is not a structured type
					else
					{
#ifdef DEBUG_FLAG1F2
									cout << "Adding variable: " << varName << "\n";
#endif
						// add this variable's (unique id -> string name) mapping to all_vars2Name, using as the unique id 
						// the pointer to the variable's SgInitializedName, stored in the lower 32 bits of the id
						all_vars2Name[(quad)((int)initName)] = varName;
					}
				}
			
			// if we find a nameless variable, mention it
			if ( varName == "" )
			{
				cout << "Found empty variable:"
						 << initName->get_declaration()->unparseToString() << "\n";
			}
		}
	}*/
}

// retrieves a list of id's for the arrays used in a function
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

/* goes over the CFG, finds all checkpoints and adds them to a map of checkpoint locations
   that maps the SgNodes of the actual checkpoint calls to their container CFGNodes*/
set<InterestingNode> findAllCheckpointLocations(InterestingNode &cfg)
{
	set<InterestingNode> chkptLocs;
	
	// iterate over all the nodes in the CFG
	for(VirtualCFG::iterator it(cfg); it!=VirtualCFG::iterator::end(); it++)
	{
		SgNode *n = (*it).getNode();
		SgFunctionCallExp* funcCall;
		
		// if ast_node is a function call expression
		if(funcCall=isSgFunctionCallExp(n))
			// if this is a checkpoint call 
			if (funcCall->get_function()->unparseToString() == "checkpoint")
				// add this checkpoint location to the chkptLocs set
				chkptLocs.insert(*it);
	}
	
	return chkptLocs;
}
