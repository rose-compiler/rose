#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <boost/regex.hpp>
#include "ompUtils.h"
#include <map>
#include "DataflowCFG.h"
#include "CFGRewrite.h"

using namespace std;
using namespace boost;

/**********************
 *****  ompUtils  *****
 **********************/
const int ompUtils::unknown_type;
const int ompUtils::omp_parallel;
const int ompUtils::omp_for;
const int ompUtils::omp_parallel_for;
const int ompUtils::omp_sections;
const int ompUtils::omp_section;
const int ompUtils::omp_single;
const int ompUtils::omp_master;
const int ompUtils::omp_critical;
const int ompUtils::omp_barrier;
const int ompUtils::omp_atomic;
const int ompUtils::omp_flush;
const int ompUtils::omp_threadprivate;

bool ompUtils::match_OMP_String(string directive, string targetStr)
{
	ostringstream pats;
	pats << "\\s+";
	pats << targetStr;
	pats << "\\s*";
	regex pat(pats.str());

	smatch what; 
	match_flag_type flags = match_default; 
	string::const_iterator start = directive.begin();
	string::const_iterator end = directive.end();
	if(regex_search(start, end, what, pat, flags))
		return true;
	
	return false;
}

int ompUtils::getOmpType(string directive)
{
	regex pat("\\s*#pragma\\s*omp\\s*((?:for)|(?:parallel\\s+for)|(?:parallel)|(?:sections)|(?:section)|(?:single)|(?:master)|(?:critical)|(?:barrier)|(?:atomic)|(?:flush)|(?:threadprivate))");
	
	/*	parallel if(expression)
				 private ( variable-list )
				 firstprivate ( variable-list )
				 lastprivate ( variable-list )
				 shared ( variable-list )
				 default ( shared )
				 default ( none )
				 reduction ( reduction-operator : variable-list )
				 copyin ( variable-list )
	variable-list:
			identifier
			variable-list , identifier

	reduction-operator:
			+ * - & ^ | && || */
	
	sregex_iterator m1(directive.begin(), directive.end(), pat, match_extra);
	sregex_iterator m2;
	int type = unknown_type;
	
	while(m1 != m2)
	{
		
		smatch what = *m1;

		//cout << what[0] << " | " << what[1] << "\n";
		// if this is the second match to the regular expression, then it is not a valid OpenMP pragma
		if(type != unknown_type)
			return unknown_type;
			
		if(what[1] == "parallel")
			type = omp_parallel;
		else if(what[1] == "for")
			type = omp_for;
		else if(what[1] == "sections")
			type = omp_sections;
		else if(what[1] == "section")
			type = omp_section;
		else if(what[1] == "single")
			type = omp_single;
		else if(what[1] == "master")
			type = omp_master;
		else if(what[1] == "critical")
			type = omp_critical;
		else if(what[1] == "barrier")
			type = omp_barrier;
		else if(what[1] == "atomic")
			type = omp_atomic;
		else if(what[1] == "flush")
			type = omp_flush;
		else if(what[1] == "threadprivate")
			type = omp_threadprivate;
		// else, the only remaining option is "parallel\s+for but we can't test for it using string::==
		else //if(what[1] == "parallel for")
			type = omp_parallel_for;

		m1++;
	}
	
	// if we found only one match, return the match
	return type;
}

string ompUtils::ompTypeStr(int ompType)
{
	if(ompType == unknown_type) return "unknown_type";
	else if(ompType == omp_parallel) return "omp_parallel";
	else if(ompType == omp_for) return "omp_for";
	else if(ompType == omp_parallel_for) return "omp_parallel_for";
	else if(ompType == omp_sections) return "omp_sections";
	else if(ompType == omp_section) return "omp_section";
	else if(ompType == omp_single) return "omp_single";
	else if(ompType == omp_master) return "omp_master";
	else if(ompType == omp_critical) return "omp_critical";
	else if(ompType == omp_barrier) return "omp_barrier";
	else if(ompType == omp_atomic) return "omp_atomic";
	else if(ompType == omp_flush) return "omp_flush";
	else if(ompType == omp_threadprivate) return "omp_threadprivate";
	else return "???";
}

/*******************************
 ***** R E D U C T I O N S *****
 *******************************/

const int reductionOperation::unknown_type;
const int reductionOperation::reduction_plus;
const int reductionOperation::reduction_multiply;
const int reductionOperation::reduction_minus;
const int reductionOperation::reduction_logic_AND;
const int reductionOperation::reduction_logic_XOR;
const int reductionOperation::reduction_logic_OR;
const int reductionOperation::reduction_boolean_AND;
const int reductionOperation::reduction_boolean_OR;

reductionOperation::reductionOperation(){}

reductionOperation::reductionOperation(const reductionOperation &that)
{
	op = that.op;
}

reductionOperation::reductionOperation(string opStr)
{
	init(opStr);
}

void reductionOperation::init(string opStr)
{
	if(opStr == "+") op = reduction_plus;
	else if(opStr == "*") op = reduction_multiply;
	else if(opStr == "-") op = reduction_minus;
	else if(opStr == "&") op = reduction_logic_AND;
	else if(opStr == "^") op = reduction_logic_XOR;
	else if(opStr == "|") op = reduction_logic_OR;
	else if(opStr == "&&") op = reduction_boolean_AND;
	else if(opStr == "||") op = reduction_boolean_OR;
	else op = unknown_type;
}

string reductionOperation::str()
{
	if(op == unknown_type) return "?"; 
	else if(op == reduction_plus) return "+"; 
	else if(op == reduction_multiply) return "*"; 
	else if(op == reduction_minus) return "-"; 
	else if(op == reduction_logic_AND) return "&"; 
	else if(op == reduction_logic_XOR) return "^"; 
	else if(op == reduction_logic_OR) return "|"; 
	else if(op == reduction_boolean_AND) return "&&"; 
	else if(op == reduction_boolean_OR) return "||"; 
	else return "??";
}

ompReduction::ompReduction(const ompReduction& that)
{
	operation = that.operation;
	for(list<string>::const_iterator it = that.variables.begin(); it!=that.variables.end(); it++)
		variables.push_back(*it);
}

// numRedVars - the number of reduction variables in this match (needed to get around a boost regex bug)
ompReduction::ompReduction(smatch what, int numRedVars)
{
/*	cout << "ompReduction: what.size()="<<what.size()<<"\n";
	cout << "ompReduction: what[0]="<<what[0]<<"\n";
	cout << "ompReduction: what[1]="<<what[1]<<"\n";
	cout << "ompReduction: what[2]="<<what[2]<<"\n";
	cout << "ompReduction: what[3]="<<what[3]<<"\n";
	cout << "ompReduction: what.captures(3).size()="<<what.captures(3).size()<<"\n";*/
	operation.init(what[1]);
	
	// the first variable
	variables.push_back(what[2]);
	
	// the subsequent variables (if any)
	for(int j=0; j</*what.captures(3).size()*/ numRedVars - 1; j++)
	{
		//cout << "ompReduction: what.captures("<<3<<")["<<j<<"]="<<what.captures(3)[j]<<"\n";
		variables.push_back(what.captures(3)[j]);
	}
}

string ompReduction::str()
{
	ostringstream outs;
	outs << "reduction(" << operation.str() << ":";
	for(list<string>::iterator it = variables.begin(); it!=variables.end(); )
	{
		outs << *it;
		
		it++;
		// if this isn't the last variable, insert a comma 
		if(it!=variables.end()) outs << ", ";
	}
	outs << ")";

	return outs.str();
}

int ompReduction::numVars()
{
	return variables.size();
}

reductionOperation& ompReduction::getOp()
{
	return operation;
}

ompReductionsSet::ompReductionsSet()
{}

ompReductionsSet::ompReductionsSet(const ompReductionsSet& that)
{
	for(list<ompReduction>::const_iterator it = that.reductions.begin(); it!=that.reductions.end(); it++)
	{
		reductions.push_back(*it);
	}
}

ompReductionsSet::ompReductionsSet(string directive)
{
	regex pat("\\s+reduction\\s*\\(\\s*([+*\\-&^\\|]+)\\s*:\\s*(\\w+)\\s*(?:[,]\\s*(\\w+))*\\s*\\)");

/*cout << "ompReductionsSet, directive="<<directive<<"\n";
cout << "                  pattern="<<"\\s+reduction\\s*\\(\\s*([+*\\-&^\\|]+)\\s*:\\s*(\\w+)\\s*(?:[,]\\s*(\\w+))*\\s*\\)"<<"\n";
	sregex_iterator m1(directive.begin(), directive.end(), pat, match_extra);
	sregex_iterator m2;
	
	while(m1 != m2)
	{
		smatch what = *m1;
		cout << "ompReductionsSet: " << what[0] << " | " << what[1] << "\n";

		unsigned i, j;
      std::cout << "** Match found **\n   Sub-Expressions:\n";
      for(i = 0; i < what.size(); ++i)
      {
         std::cout << "      $" << i << " = \"" << what[i] << "\"\n";
         if(firstMatch) lastNumCaptures[0] = 0;
      }
      
      std::cout << "   Captures:\n";
      for(i = 0; i < what.size(); ++i)
      {
         std::cout << "      $" << i << " = {";
         for(j = 0; j < (what.captures(i).size() - lastNumCaptures[i]); ++j)
         {
            if(j)
               std::cout << ", ";
            else
               std::cout << " ";
            std::cout << "\"" << what.captures(i)[j] << "\"";
         }
         std::cout << " }\n";
         if(!firstMatch) lastNumCaptures[i] = what.captures(i).size();
      }
      firstMatch = false;

		m1++;
	}*/

	sregex_iterator m1(directive.begin(), directive.end(), pat, match_extra);
	sregex_iterator m2;
	// This is a workaround for a boost bug where the captures from the previous match
	// get preserved for the next match. These stale captures always get placed at the end
	// of the next match, so if we know how many of them there are, we can ignore them.
	map<int, int> lastNumCaptures;
	bool firstMatch=true;
	
	while(m1 != m2)
	{
		smatch what = *m1;
		//cout << "ompReductionsSet: " << what[0] << " | " << what[1] << "\n";

		if(firstMatch) for(int i = 0; i < what.size(); ++i) lastNumCaptures[i] = 0;

		ompReduction nextRed(what, what.captures(3).size() - lastNumCaptures[3] + 1);
		reductions.push_back(nextRed);
		
		for(int i = 0; i < what.size(); ++i) lastNumCaptures[i] = what.captures(i).size();

		firstMatch = false;

		m1++;
	}

	/*smatch what; 
	match_flag_type flags = match_extra;//match_default; 
	string::const_iterator start = directive.begin();
	string::const_iterator end = directive.end();
	while(regex_search(start, end, what, pat, flags))
	{
		cout << what[0] << " | " << what[1] << "\n";
		
		ompReduction nextRed(what);
		reductions.push_back(nextRed);
		
		// update the search position
		start = what[0].second; 
		flags |= match_prev_avail; 
	}*/
}

int ompReductionsSet::numReductions()
{
	return reductions.size();
}

string ompReductionsSet::str()
{
	ostringstream outs;
	for(list<ompReduction>::iterator it = reductions.begin(); it!=reductions.end(); )
	{
		outs << (*it).str();
		
		it++;
		// if this isn't the last variable, insert a space
		if(it!=reductions.end()) outs << " ";
	}

	return outs.str();
}

/***********************************
 ***** D A T A   C L A U S E S *****
 ***********************************/

ompDataClause::ompDataClause()
{}

ompDataClause::ompDataClause(const ompDataClause &that)
{
	clauseType = that.clauseType;
	for(list<string>::const_iterator it = that.variables.begin(); it!=that.variables.end(); it++)
	{
		variables.push_back(*it);
	}
}

ompDataClause::ompDataClause(string directive, string clauseType)
{
	this->clauseType = clauseType;
	ostringstream pats;
	pats << "\\s+";
	pats << clauseType;
	pats << "\\s*\\(\\s*(\\w+)\\s*(?:,\\s*(\\w+))*\\s*\\)";
	regex pat(pats.str());

	sregex_iterator m1(directive.begin(), directive.end(), pat, match_extra);
	sregex_iterator m2;
	// This is a workaround for a boost bug where the captures from the previous match
	// get preserved for the next match. These stale captures always get placed at the end
	// of the next match, so if we know how many of them there are, we can ignore them.
	map<int, int> lastNumCaptures;
	bool firstMatch=true;
	
	while(m1 != m2)
	{
		smatch what = *m1;
		//cout << "ompReductionsSet: " << what[0] << " | " << what[1] << "\n";

		if(firstMatch) for(int i = 0; i < what.size(); ++i) lastNumCaptures[i] = 0;

		// the first variable
		variables.push_back(what[1]);
		
		// the subsequent variables
		for(int j=0; j<what.captures(2).size()-lastNumCaptures[2] ; j++)
		{
			//cout << "ompDataClause: what.captures("<<3<<")["<<j<<"]="<<what.captures(2)[j]<<"\n";
			variables.push_back(what.captures(2)[j]);
		}
		
		for(int i = 0; i < what.size(); ++i) lastNumCaptures[i] = what.captures(i).size();

		firstMatch = false;

		m1++;
	}
}

int ompDataClause::numVars() const
{
	return variables.size();
}

const list<string>& ompDataClause::getVars() const
{
	return variables;
}

void ompDataClause::addVar(string var)
{
	variables.push_back(var);
}

string ompDataClause::str() const
{
	ostringstream outs;
	outs << clauseType << "(";

	for(list<string>::const_iterator it = variables.begin(); it!=variables.end(); )
	{
		outs << *it;
		
		it++;
		// if this is not the last variable, add a comma
		if(it!=variables.end()) outs << ", ";
	}

	outs << ")";

	return outs.str();
}

const int defaultVarSharing::var_sharing_class_unknown;
const int defaultVarSharing::var_sharing_class_none;
const int defaultVarSharing::var_sharing_class_shared;

defaultVarSharing::defaultVarSharing(const defaultVarSharing& that)
{
	type = that.type;
}

defaultVarSharing::defaultVarSharing(string directive)
{
	ompDataClause tempSharingSettings(directive, "default");
	type=var_sharing_class_unknown;

	// if the programmer did specify a default()
	if(tempSharingSettings.numVars()>0)
	{
		if(tempSharingSettings.getVars().front()=="shared")
			type = var_sharing_class_shared;
		else if(tempSharingSettings.getVars().front()=="none")
			type = var_sharing_class_none;
		else
			cout << "ERROR: Unknown sharing class: \"" << tempSharingSettings.getVars().front() << "\"!";
	}
}

string defaultVarSharing::str()
{
	if(type == var_sharing_class_none)
		return "default(none)";
	else if(type == var_sharing_class_shared)
		return "default(shared)";
	else return "";
}

/********************************
 ***** #pragma omp parallel *****
 ********************************/

ompParallel::ompParallel(string directive, bool verbose) : 
	reductions(directive), privateVars(directive, "private"), firstPrivateVars(directive, "firstprivate"), 
	lastPrivateVars(directive, "lastprivate"), sharedVars(directive, "shared"), 
	copyinVars(directive, "copyin"), varSharingClass(directive)
{ }

ompParallel::ompParallel(ompParallelFor opf) :
						reductions(opf.reductions), privateVars(opf.privateVars),
						firstPrivateVars(opf.firstPrivateVars), lastPrivateVars(opf.lastPrivateVars),
						sharedVars(opf.sharedVars), copyinVars(opf.copyinVars),
						varSharingClass(opf.varSharingClass)
{ }

ompParallel::ompParallel(ompReductionsSet oldReductions, ompDataClause oldPrivateVars,
            ompDataClause oldFirstPrivateVars, ompDataClause oldLastPrivateVars,
            ompDataClause oldSharedVars, ompDataClause oldCopyinVars,
            defaultVarSharing oldVarSharingClass) : 
	reductions(oldReductions), privateVars(oldPrivateVars), firstPrivateVars(oldFirstPrivateVars),
	lastPrivateVars(oldLastPrivateVars), sharedVars(oldSharedVars), 
	copyinVars(oldCopyinVars), varSharingClass(oldVarSharingClass)
{ }

string ompParallel::str()
{
	ostringstream outs;
	outs << "#pragma ";
	outs << str_nopragma();

	return outs.str();
}

string ompParallel::str_nopragma()
{
	ostringstream outs;
	
	outs << "omp parallel";
	if(reductions.numReductions()>0) outs << " " << reductions.str();
	if(privateVars.numVars()>0)      outs << " " << privateVars.str();
	if(firstPrivateVars.numVars()>0) outs << " " << firstPrivateVars.str();
	if(lastPrivateVars.numVars()>0)  outs << " " << lastPrivateVars.str();
	if(sharedVars.numVars()>0)       outs << " " << sharedVars.str();
	if(copyinVars.numVars()>0)       outs << " " << copyinVars.str();
	outs << " " << varSharingClass.str();
	
	return outs.str();
}

/***************************
 ***** #pragma omp for *****
 ***************************/
ompFor::ompFor(string directive, bool verbose) : 
	reductions(directive), privateVars(directive, "private"), firstPrivateVars(directive, "firstprivate"), 
	lastPrivateVars(directive, "lastprivate")
{
	ordered = ompUtils::match_OMP_String(directive, "ordered");
	nowait = ompUtils::match_OMP_String(directive, "nowait");
}

ompFor::ompFor(ompReductionsSet oldReductions, ompDataClause oldPrivateVars,
       ompDataClause oldFirstPrivateVars, ompDataClause oldLastPrivateVars,
       bool oldOrdered, bool oldNoWait) : 
	reductions(oldReductions), privateVars(oldPrivateVars), firstPrivateVars(oldFirstPrivateVars),
	lastPrivateVars(oldLastPrivateVars)
{
	ordered = oldOrdered;
	nowait = oldNoWait;
}

ompFor::ompFor(bool oldOrdered, bool oldNoWait) : 
	reductions((string)""), privateVars((string)"", (string)""), 
	firstPrivateVars((string)"", (string)""), lastPrivateVars((string)"", (string)"")
{
	ordered = oldOrdered;
	nowait = oldNoWait;
}

bool ompFor::getOrdered()
{
	return ordered;
}

bool ompFor::getNowait()
{
	return nowait;
}

string ompFor::str()
{
	ostringstream outs;
	outs << "#pragma ";
	outs << str_nopragma();

	return outs.str();
}

string ompFor::str_nopragma()
{
	ostringstream outs;
	outs << "omp for";
	
	if(reductions.numReductions()>0) outs << " " << reductions.str();
	if(privateVars.numVars()>0)      outs << " " << privateVars.str();
	if(firstPrivateVars.numVars()>0) outs << " " << firstPrivateVars.str();
	if(lastPrivateVars.numVars()>0)  outs << " " << lastPrivateVars.str();

	if(ordered && nowait) outs << " ordered nowait";
	else if(ordered) outs << " ordered";
	else if(nowait) outs << " nowait";

	return outs.str();
}

/******************************
 ***** #pragma omp single *****
 ******************************/
ompSingle::ompSingle(string directive, bool verbose) : 
	privateVars(directive, "private"), firstPrivateVars(directive, "firstprivate"), 
	copyPrivateVars(directive, "copyprivate")
{
	nowait = ompUtils::match_OMP_String(directive, "nowait");
}
	
ompSingle::ompSingle(ompDataClause oldPrivateVars, ompDataClause oldFirstPrivateVars, 
          ompDataClause oldCopyPrivateVars, bool oldNoWait) : 
	privateVars(oldPrivateVars), firstPrivateVars(oldFirstPrivateVars),
	copyPrivateVars(oldCopyPrivateVars)
{
	nowait = oldNoWait;
}

ompSingle::ompSingle(bool oldNoWait)
{
	nowait = oldNoWait;
}

bool ompSingle::getNowait()
{
	return nowait;
}

string ompSingle::str()
{
	ostringstream outs;
	outs << "#pragma ";
	outs << str_nopragma();

	return outs.str();
}
	
string ompSingle::str_nopragma()
{
	ostringstream outs;
	outs << "omp single";
	
	if(privateVars.numVars()>0)      outs << " " << privateVars.str();
	if(firstPrivateVars.numVars()>0) outs << " " << firstPrivateVars.str();
	if(copyPrivateVars.numVars()>0)  outs << " " << copyPrivateVars.str();

	if(nowait) outs << " nowait";

	return outs.str();
}

/************************************
 ***** #pragma omp parallel for *****
 ************************************/

ompParallelFor::ompParallelFor(string directive, bool verbose) : 
	reductions(directive), privateVars(directive, "private"), firstPrivateVars(directive, "firstprivate"), 
	lastPrivateVars(directive, "lastprivate"), sharedVars(directive, "shared"), 
	copyinVars(directive, "copyin"), varSharingClass(directive)
{
	ordered = ompUtils::match_OMP_String(directive, "ordered");
}

ompParallelFor::ompParallelFor(ompReductionsSet oldReductions, ompDataClause oldPrivateVars,
               ompDataClause oldFirstPrivateVars, ompDataClause oldLastPrivateVars,
               ompDataClause oldSharedVars, ompDataClause oldCopyinVars,
               bool oldOrdered, defaultVarSharing oldVarSharingClass) : 
	reductions(oldReductions), privateVars(oldPrivateVars), firstPrivateVars(oldFirstPrivateVars),
	lastPrivateVars(oldLastPrivateVars), sharedVars(oldSharedVars), 
	copyinVars(oldCopyinVars), varSharingClass(oldVarSharingClass)
{
	ordered = oldOrdered;
}

bool ompParallelFor::getOrdered()
{
	return ordered;
}

string ompParallelFor::str()
{
	ostringstream outs;
	outs << "#pragma ";
	outs << str_nopragma();

	return outs.str();
}

string ompParallelFor::str_nopragma()
{
	
	ostringstream outs;
	outs << "omp parallel for";
	
	if(reductions.numReductions()>0) outs << " " << reductions.str();
	if(privateVars.numVars()>0)      outs << " " << privateVars.str();
	if(firstPrivateVars.numVars()>0) outs << " " << firstPrivateVars.str();
	if(lastPrivateVars.numVars()>0)  outs << " " << lastPrivateVars.str();
	if(sharedVars.numVars()>0)       outs << " " << sharedVars.str();
	if(copyinVars.numVars()>0)       outs << " " << copyinVars.str();
	
	if(ordered) outs << " ordered";
		
	outs << " " << varSharingClass.str();

	return outs.str();
}

/*************************************
 ***** #pragma omp threadprivate *****
 *************************************/
ompThreadprivate::ompThreadprivate(string directive, bool verbose) : vars(directive, "threadprivate")
{}

ompThreadprivate::ompThreadprivate(ompThreadprivate& otp) : vars(otp.vars)
{}

ompThreadprivate::ompThreadprivate(ompDataClause oldVars) : vars(oldVars)
{}

string ompThreadprivate::str()
{
	ostringstream outs;
	outs << "#pragma ";
	outs << str_nopragma();

	return outs.str();
}

string ompThreadprivate::str_nopragma()
{
	ostringstream outs;
	
	outs << "omp threadprivate";
	if(vars.numVars()>0)      outs << " " << vars.str();
	
	return outs.str();
}

/*************************************
 ***** OpenMP-specific insertion *****
 *************************************/

class ompForInsertAttribute: public AstAttribute
{
	public:
	// = true if it is legal to insert a statement before or after this node, =false otherwise
	bool insertBefore;
	bool insertAfter;
	
	// if insertable, = NULL if the insert-before/insert-after should actually take place a this node
	// if not, points to the node before/after which the insertion must take place
	SgNode* insertBeforeProxy;
	SgNode* insertAfterProxy;
	
	ompForInsertAttribute(bool insertBefore, bool insertAfter, SgNode* insertBeforeProxy, SgNode* insertAfterProxy)
	{
		this->insertBefore = insertBefore;
		this->insertAfter = insertAfter;
		this->insertBeforeProxy = insertBeforeProxy;
		this->insertAfterProxy = insertAfterProxy;
	}
}; 

// returns the statement that follows the given pragma
SgStatement* getPragmaBody(SgPragmaDeclaration* pragma)
{
	SgBasicBlock* parentBlock = isSgBasicBlock(pragma->get_parent());
	ROSE_ASSERT(parentBlock);
	
	SgStatementPtrList parentBlockStmts = parentBlock->get_statements();
	for(SgStatementPtrList::iterator it = parentBlockStmts.begin(); it!=parentBlockStmts.end(); it++)
	{
		// if we're at the pragma
		if(*it == pragma)
		{
			it++;
			return *it;
		}
	}
	return NULL;
}

// calls annotateOmpFor() on all #pragma omp for SgForStatements in the given AST tree
void annotateAllOmpFors(SgNode* root)
{
	Rose_STL_Container<SgNode*> allpragmas = NodeQuery::querySubTree(root, V_SgPragmaDeclaration);
	for(Rose_STL_Container<SgNode*>::iterator it = allpragmas.begin(); it!=allpragmas.end(); it++)
	{
		SgPragmaDeclaration* pragma = isSgPragmaDeclaration(*it); ROSE_ASSERT(pragma);
		string pragmaStr(pragma->get_pragma()->get_pragma());
		string directive = "#pragma "+pragmaStr;
		
		if(ompUtils::getOmpType(directive) == ompUtils::omp_for)
		{
			SgStatement* body = getPragmaBody(pragma); ROSE_ASSERT(body);
			SgForStatement* forStmt = isSgForStatement(body); ROSE_ASSERT(forStmt);
			annotateOmpFor(pragma, forStmt);
		}
	}
}

// annotates the headers #pragma omp for loops to indicate that no insertion should take place
// in the test and update sub-trees and that all insertions into the inialization sub-tree should 
// actually be placed right before the #pragma omp for
void annotateOmpFor(SgPragmaDeclaration* pragma, SgForStatement* ompFor)
{
	Rose_STL_Container<SgNode*> initList = NodeQuery::querySubTree(ompFor->get_for_init_stmt(), V_SgNode);
	for(Rose_STL_Container<SgNode*>::iterator it = initList.begin(); it!=initList.end(); it++)
	{
		SgNode *n = *it;
		if(!n->attributeExists("ompForInsert"))
		{
			ompForInsertAttribute* attr = new ompForInsertAttribute(true, false, pragma, NULL);
			n->addNewAttribute("ompForInsert", attr);
		}
	}
		
	Rose_STL_Container<SgNode*> testList = NodeQuery::querySubTree(ompFor->get_test(), V_SgNode);
	for(Rose_STL_Container<SgNode*>::iterator it = testList.begin(); it!=testList.end(); it++)
	{
		SgNode *n = *it;
		if(!n->attributeExists("ompForInsert"))
		{
			ompForInsertAttribute* attr = new ompForInsertAttribute(false, false, NULL, NULL);
			n->addNewAttribute("ompForInsert", attr);
		}
	}
	
	Rose_STL_Container<SgNode*> updateList = NodeQuery::querySubTree(ompFor->get_increment(), V_SgNode);	
	for(Rose_STL_Container<SgNode*>::iterator it = updateList.begin(); it!=updateList.end(); it++)
	{
		SgNode *n = *it;
		if(!n->attributeExists("ompForInsert"))
		{
			ompForInsertAttribute* attr = new ompForInsertAttribute(false, false, NULL, NULL);
			n->addNewAttribute("ompForInsert", attr);
		}
	}
}

void OMPcfgRWTransaction::insertBefore(DataflowNode cfgNode, SgExpression* newNode)
{
	printf("OMPcfgRWTransaction::insertBefore\n");
	OMPcfgRWTransaction::insertBefore(cfgNode.getNode(), newNode);
}
void OMPcfgRWTransaction::insertBefore(SgNode* n, SgExpression* newNode)
{
	printf("OMPcfgRWTransaction::insertBefore, attributeExists = %d\n", n->attributeExists("ompForInsert"));
	printf("        n = <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str());
	printf("        newNode = <%s | %s>\n", newNode->unparseToString().c_str(), newNode->class_name().c_str());
	// if this node is inside an omp for construct
	if(n->attributeExists("ompForInsert"))
	{
		ompForInsertAttribute* ompForInsert = (ompForInsertAttribute*)n->getAttribute("ompForInsert");
		if(ompForInsert->insertBefore)
		{
			if(ompForInsert->insertBeforeProxy)
			{
				printf("     inserting before proxy\n");
				cfgRWTransaction::insertBefore(ompForInsert->insertBeforeProxy, newNode);
			}
			else
			{
				printf("     inserting before original 1\n");
				cfgRWTransaction::insertBefore(n, newNode);
			}
		}
		else if(ompForInsert->insertAfter)
			if(ompForInsert->insertAfterProxy)
			{
				printf("     inserting after proxy\n");
				cfgRWTransaction::insertAfter(ompForInsert->insertBeforeProxy, newNode);
			}
	}
	else
	{
		printf("     inserting before original 2\n");
		cfgRWTransaction::insertBefore(n, newNode);
	}
}

void OMPcfgRWTransaction::insertAfter(DataflowNode cfgNode, SgExpression* newNode)
{
	printf("OMPcfgRWTransaction::insertAfter\n");
	OMPcfgRWTransaction::insertAfter(cfgNode.getNode(), newNode);
}
void OMPcfgRWTransaction::insertAfter(SgNode* n, SgExpression* newNode)
{
	printf("OMPcfgRWTransaction::insertAfter, attributeExists = %d\n", n->attributeExists("ompForInsert"));
	printf("        n = <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str());
	printf("        newNode = <%s | %s>\n", newNode->unparseToString().c_str(), newNode->class_name().c_str());

	// if this node is inside an omp for construct
	if(n->attributeExists("ompForInsert"))
	{
		ompForInsertAttribute* ompForInsert = (ompForInsertAttribute*)n->getAttribute("ompForInsert");
		if(ompForInsert->insertAfter)
		{
			if(ompForInsert->insertAfterProxy)
			{
				printf("     inserting after proxy\n");
				cfgRWTransaction::insertAfter(ompForInsert->insertBeforeProxy, newNode);
			}
			else
			{
				printf("     inserting before original 1\n");
				cfgRWTransaction::insertAfter(n, newNode);
			}
		}
		else if(ompForInsert->insertBefore)
			if(ompForInsert->insertBeforeProxy)
			{
				printf("     inserting before proxy\n");
				cfgRWTransaction::insertBefore(ompForInsert->insertBeforeProxy, newNode);
			}

	}
	else
	{
		printf("     inserting before original 2\n");
		cfgRWTransaction::insertAfter(n, newNode);
	}
}

// surrounds the body of the given pragma with a SgBasicBlock, 
// if oldBody!=NULL sets it to this pragma's original body 
// if newBody!=NULL sets it to this pragma's new body
void wrapPragmaBody(SgPragmaDeclaration* pragma, SgStatement** oldBody, SgBasicBlock** newBody)
{
	SgBasicBlock* parentBlock = isSgBasicBlock(pragma->get_parent());
	ROSE_ASSERT(parentBlock);
	
	SgStatementPtrList parentBlockStmts = parentBlock->get_statements();
	for(SgStatementPtrList::iterator it = parentBlockStmts.begin(); it!=parentBlockStmts.end(); it++)
	{
		// if we're at the pragma
		if(*it == pragma)
		{
			it++;
			SgStatement* pragmaBody = *it;
			
			if(oldBody!=NULL) *oldBody = pragmaBody;
			
			// remove the statement that follows the pragma
			LowLevelRewrite::remove(pragmaBody);
				
			// create a new SgBasicBlock that contains the pragma's old body
			SgBasicBlock* newPragmaBody = new SgBasicBlock(SgDefaultFile, pragmaBody);
			pragmaBody->set_parent(newPragmaBody);
			newPragmaBody->set_parent(parentBlock);
			
			myStatementInsert(pragma, newPragmaBody, false, false);
			
			if(newBody!=NULL) *newBody = newPragmaBody;
			return;
		}
	}
	ROSE_ASSERT(!"ERROR: wrapPragmaBody() could not find the body of the given pragma!");
}

// surrounds the given pragma and its body with a SgBasicBlock, 
// returns this new SgBasicBlock
SgBasicBlock* wrapPragmaAndBody(SgPragmaDeclaration* pragma)
{
	SgBasicBlock* parentBlock = isSgBasicBlock(pragma->get_parent());
	ROSE_ASSERT(parentBlock);
	
	SgStatementPtrList parentBlockStmts = parentBlock->get_statements();
	for(SgStatementPtrList::iterator it = parentBlockStmts.begin(); it!=parentBlockStmts.end(); it++)
	{
		// if we're at the pragma
		if(*it == pragma)
		{
			it++;
			SgStatement* pragmaBody = *it;
			
			// remove the statement that follows the pragma
			LowLevelRewrite::remove(pragmaBody);
				
			// create a new SgBasicBlock that contains the pragma's old body
			SgBasicBlock* newBlock = new SgBasicBlock(SgDefaultFile, pragmaBody);
			pragmaBody->set_parent(newBlock);
			newBlock->set_parent(parentBlock);
			
			// insert the block immediately after the pragma
			myStatementInsert(pragma, newBlock, false, false);
			
			// now remove the pragma and insert it inside the block as well
			LowLevelRewrite::remove(pragma);
			newBlock->prepend_statement(pragma);
			pragma->set_parent(newBlock);
			
			return newBlock;
		}
	}
	ROSE_ASSERT(!"ERROR: wrapPragmaAndBody() could not find the body of the given pragma!");
	return NULL;
}

// inserts the given statement to either the top or bottom of the given OpenMP directive 
// supports any directive that may have a basic block as its body
void insertTopBottomOmpDirective(SgPragmaDeclaration* pragma, bool top, SgStatement* stmt)
{
	// wrap the pragma's body with a basic block
	SgStatement* oldBody;
	SgBasicBlock* newBody;
	wrapPragmaBody(pragma, &oldBody, &newBody);
	
	// insert the new statement at the top/bottom of the body
	//printf("insertTopBottomOmpDirective() inserting %s before/after %s\n", newStmt->unparseToString().c_str(), oldBody->unparseToString().c_str());
	myStatementInsert(oldBody, stmt, top, false);
}

// inserts statements generated by stmtCreate at either the top or the bottom of the body of all OpenMP directives of the given 
//     type in the given AST subtree
// supports any directive that may have a basic block as its body
void insertTopBottomOmpDirectives(SgNode* root, int ompType, bool top, SgStatement* (*stmtCreate)())
{
	// ensure that this is a directive that can have a basic block as its body
	ROSE_ASSERT((ompType == ompUtils::omp_parallel) ||
	            (ompType == ompUtils::omp_section) ||
	            (ompType == ompUtils::omp_single) ||
	            (ompType == ompUtils::omp_master) ||
	            (ompType == ompUtils::omp_critical));
	
//	printf("insertTopBottomOmpDirective() root = 0x%x = <%s | %s>\n", root, root->unparseToString().c_str(), root->class_name().c_str());
	
	Rose_STL_Container<SgNode*> allpragmas = NodeQuery::querySubTree(root, V_SgPragmaDeclaration);
	for(Rose_STL_Container<SgNode*>::iterator it = allpragmas.begin(); it!=allpragmas.end(); it++)
	{
		SgPragmaDeclaration* pragma = isSgPragmaDeclaration(*it); ROSE_ASSERT(pragma);
		string pragmaStr(pragma->get_pragma()->get_pragma());
		string directive = "#pragma "+pragmaStr;
		
		// if this directive is of the desired type
		if(ompUtils::getOmpType(directive) == ompType)
		{
			insertTopBottomOmpDirective(pragma, top, stmtCreate());
		}	
	}
}



