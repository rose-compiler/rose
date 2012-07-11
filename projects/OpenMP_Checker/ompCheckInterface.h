#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "rose.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "yices_c.h"
#include "yicesl_c.h"





namespace ompCheckerInterface
{
	
	using std::cout;
	using std::cin;
	using std::endl;
	using std::string;
	using std::vector;
	using std::stringstream;
	
#define List Rose_STL_Container< SgNode* >
	
#define Iter List::iterator
	
#define CIter List::const_iterator
	
#define Query NodeQuery::querySubTree
	
#define EnclNode SageInterface::getEnclosingNode	

                   /* Fucntion Prototypes */

void showNodeInfo( SgNode* );

void process_omp_parallel( SgOmpParallelStatement* );

void getPrivateVarRefs( const SgOmpClausePtrList &,  SgVarRefExpPtrList & );

void flagLocalPrivate( const List &, const List &, SgVarRefExpPtrList & );

void removeExclusive( List &);

void gatherReferences( const List &, List &);

void getUnique( List &);

void getUnique( SgVarRefExpPtrList &);

string getName( SgNode* );

void identifyRaces( const List &, const SgVarRefExpPtrList &, List &);

void showMembers( const List &);

void showMembers( const SgVarRefExpPtrList &);

void removeTrivial( List &);

void printWarnings( const vector<string> &);

void printRaces( const List &, const List &);


}

#endif
