#ifndef CALL_GRAPH_TRAVERSE_C
#define CALL_GRAPH_TRAVERSE_C

#include "CallGraphTraverse.h"
#include "cfgUtils.h"

#include <set>
using namespace std;
//namespace CallGraph
//{

/****************************
 ********* Function *********
 ****************************/
Function::Function()
{
	decl=NULL;
}
 
Function::Function(string name)
{
	//printf("Function::Function(string name) this=0x%x\n", this);
	//def = NULL;
	
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(cfgUtils::project, V_SgFunctionDeclaration);
	for (Rose_STL_Container<SgNode*>::const_iterator it = functions.begin(); it != functions.end(); it++)
	{
		ROSE_ASSERT(isSgFunctionDeclaration(*it));
		
		if(isSgFunctionDeclaration(*it)->get_name().getString() == name)
		{
			// decl will be initialized to the defining declaration of this function or 
			// the first non-defining declaratio,n if there is no definition
			decl = getCanonicalDecl(isSgFunctionDeclaration(*it));
			break;
			/*decls.insert(isSgFunctionDeclaration(*it));
			if(isSgFunctionDeclaration(*it)->get_definition())
			{
				// save the current function's definition inside def
				// ensure that either def has not been set yet or that there is a unique definition
				if(def==NULL)
					def = isSgFunctionDeclaration(*it)->get_definition();
				else
					ROSE_ASSERT(def == isSgFunctionDeclaration(*it)->get_definition());
			}*/
		}
	}
	
	// every function must have at least one declaration
	//ROSE_ASSERT(decls.size()>0);
	ROSE_ASSERT(decl);
}

Function::Function(SgFunctionDeclaration* sample)
{
	//printf("Function::Function(SgFunctionDeclaration* sample) this=0x%x\n", this);
	init(sample);
}

Function::Function(SgFunctionDefinition* sample)
{
	//printf("Function::Function(SgFunctionDefinition* sample) this=0x%x\n", this);
	init(sample->get_declaration());
	
	// ensure that the definition found by init() is the same as what got passed in here
	//ROSE_ASSERT(def == sample);
}

Function::Function(SgFunctionCallExp* funcCall)
{
	ROSE_ASSERT(isSgFunctionRefExp(funcCall->get_function()));
	init(isSgFunctionRefExp(funcCall->get_function())->get_symbol()->get_declaration());
}

void Function::init(SgFunctionDeclaration* sample)
{
	//SgName mangledName = sample->get_mangled_name();
	//def = NULL;
	
	//printf("Function::init() name=%s=%s\n", this, sample->get_name().str(), mangledName.str());
	
	// decl will be initialized to the defining declaration or the first non-defining declaration if there is no definition
	decl = getCanonicalDecl(sample);
	
	// insert the sample declaration into decls, in case this is a name-less function declaration (i.e. a function pointer)
	// which won't show up in an AST query
	//decls.insert(sample);
	
	/*Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(cfgUtils::project, V_SgFunctionDeclaration);
	for (Rose_STL_Container<SgNode*>::const_iterator it = functions.begin(); it != functions.end(); it++)
	{
		ROSE_ASSERT(isSgFunctionDeclaration(*it));
		
		if(isSgFunctionDeclaration(*it)->get_mangled_name() == mangledName)
		{
			decls.insert(isSgFunctionDeclaration(*it));
			if(isSgFunctionDeclaration(*it)->get_definition())
			{
				// save the current function's definition inside def
				// ensure that either def has not been set yet or that there is a unique definition
				if(def==NULL)
					def = isSgFunctionDeclaration(*it)->get_definition();
				else
					ROSE_ASSERT(def == isSgFunctionDeclaration(*it)->get_definition());
			}
		}
	}
	//decls.insert(sample);
	//def = sample->get_definition();
	
	//printf("Function::init() this: 0x%x, name=%s *(decls.begin())=0x%x  decls.size()=%d def=0x%x\n", this, get_name().str(), *(decls.begin()), decls.size(), def);
	//printf("Function::init() this: 0x%x, name=%s decls.size()=%d def=0x%x\n", this, get_name().str(), decls.size(), def);
	
	// every function must have at least one declaration
	ROSE_ASSERT(decls.size()>0);*/
}

Function::Function(const Function &that)
{
	//printf("Function::Function(const Function &that) this=0x%x\n", this);
	/*def = that.def;
	for(set<SgFunctionDeclaration*>::const_iterator it = that.decls.begin(); it!=that.decls.end(); it++)
	{	//printf("                  declaration: 0x%x\n", *it);
		decls.insert(*it);
	}*/
	decl = that.decl;
}

Function::Function(const Function *that)
{
	//printf("Function::Function(const Function &that) this=0x%x\n", this);
	/*def = that.def;
	for(set<SgFunctionDeclaration*>::const_iterator it = that.decls.begin(); it!=that.decls.end(); it++)
	{	//printf("                  declaration: 0x%x\n", *it);
		decls.insert(*it);
	}*/
	decl = that->decl;
}

SgFunctionDeclaration* Function::getCanonicalDecl(SgFunctionDeclaration* sampleDecl)
{
	SgFunctionDeclaration* canonicalDecl = NULL;
	
	if(sampleDecl->get_definingDeclaration())
		canonicalDecl = isSgFunctionDeclaration(sampleDecl->get_definingDeclaration());
	else
		canonicalDecl = isSgFunctionDeclaration(sampleDecl->get_firstNondefiningDeclaration());
	ROSE_ASSERT(canonicalDecl);
	
	return canonicalDecl;
}

bool Function::eq(const Function &that) const
{
	/*printf("Function::eq() %s  that=%s\n", get_name().str(), that.get_name().str());
	
	printf("               (def != that.def) = %d\n", (def != that.def));
	if(def != that.def) return false;
		
	// check that that's set of declarations is a subset of this's set of declarations
	for(set<SgFunctionDeclaration*>::const_iterator it = that.decls.begin(); it!=that.decls.end(); it++)
	{
		printf("               (decls.find(*it) == decls.end()) = %d\n", (decls.find(*it) == decls.end()));
		if(decls.find(*it) == decls.end()) return false;
	}
	
	// check that this's set of declarations is a subset of that's set of declarations
	for(set<SgFunctionDeclaration*>::iterator it = decls.begin(); it!=decls.end(); it++)
	{
		printf("               (that.decls.find(*it) == that.decls.end()) = %d\n", (that.decls.find(*it) == that.decls.end()));
		if(that.decls.find(*it) == that.decls.end()) return false;
	}*/
	if(decl != that.decl) return false;
	
	//printf("               Equal\n");
	return true;
}

bool Function::operator == (const Function &that) const
{
	return eq(that);
}

bool Function::operator != (const Function &that) const
{
	return !(*this == that);
}


bool Function::lessThan(const Function &that) const
{
	// compare the declarations, since every Function object must have a unique canonical declaration
	return decl < that.decl;
	
	/*// if either object has a definition, compare those
	if(def != that.def) return def < that.def;
		
	// if not, let the function with fewer declarations be smaller
	if(decls.size() < that.decls.size()) return true;
		
	// if both functions have the same number of declarations, compare the declarations themselves
	set<SgFunctionDeclaration*>::iterator itThis;
	set<SgFunctionDeclaration*>::const_iterator itThat;
	for(itThis = decls.begin(), itThat = that.decls.begin(); 
	    itThis!=decls.end() && itThat!=that.decls.end();
	    itThis++, itThat++)
		if(*itThis != *itThat) return *itThis < *itThat;
	
	// ensure that both lists of declarations are finished
	ROSE_ASSERT(itThis==decls.end() && itThat==that.decls.end());
	
	// clearly, these functions are equal
	return false;*/
}
bool Function::operator < (const Function &that) const
{ return lessThan(that); }
bool Function::operator > (const Function &that) const
{ return !((*this) < that) && ((*this) != that); }
bool Function::operator <= (const Function &that) const
{ return ((*this) < that) || ((*this) == that); }
bool Function::operator >= (const Function &that) const
{ return !((*this) < that); }

SgName Function::get_name() const
{
	// every function must have at least one declaration
	/*ROSE_ASSERT(*(decls.begin()));

//printf("Function::get_name() this: 0x%x, *(decls.begin())=0x%x  decls.size()=%d def=0x%x\n", this, *(decls.begin()), decls.size(), def);
	return (*(decls.begin()))->get_name();*/
	if(decl)
		return decl->get_name();
	else
		return "";
}

// returns this function's definition or NULL of it does not have one
SgFunctionDefinition* Function::get_definition() const
{
	//return def;
	if(decl)
		return decl->get_definition();
	else
		return NULL;
}

// returns one of this function's declarations. it is guaranteed to be the same each time get_declaration is called
SgFunctionDeclaration* Function::get_declaration() const
{
	//return *(decls.begin());
	return decl;
}

// returns the file_info of the definition or one of the declarations if there is no definition
Sg_File_Info* Function::get_file_info() const
{
	/*
	// every function must have at least one declaration
	ROSE_ASSERT(*(decls.begin()));
	
	if(def) return def->get_file_info();
	else return (*(decls.begin()))->get_file_info();*/
	if(decl)
		return decl->get_file_info();
	else
		return NULL;
}

// returns the parameters of this function
SgInitializedNamePtrList Function::get_params()
{
	if(decl)
		return get_declaration()->get_args();
	else
	{
		SgInitializedNamePtrList list;
		return list;
	}
}

/******************************
 ********* CGFunction *********
 ******************************/

CGFunction::CGFunction(string name, SgIncidenceDirectedGraph* graph) : Function(name)
{
	this->graph = graph;
	initCGNodes();
}

CGFunction::CGFunction(SgFunctionDeclaration* sample, SgIncidenceDirectedGraph* graph) : Function(sample)
{
	this->graph = graph;
	initCGNodes();
}

CGFunction::CGFunction(SgGraphNode* sample, SgIncidenceDirectedGraph* graph) : Function(isSgFunctionDeclaration(sample->get_SgNode()))
{
	ROSE_ASSERT(isSgFunctionDeclaration(sample->get_SgNode()));
	
	this->graph = graph;
	initCGNodes();
}

CGFunction::CGFunction(const CGFunction &that) : Function(that)
{
	this->graph = that.graph;

	// copy cgNodes from that to this
	for(set<SgGraphNode*>::iterator it = that.cgNodes.begin(); it!=that.cgNodes.end(); it++)
		cgNodes.insert(*it);
}

CGFunction::CGFunction(const CGFunction *that) : Function(that)
{
	this->graph = that->graph;

	// copy cgNodes from that to this
	for(set<SgGraphNode*>::iterator it = that->cgNodes.begin(); it!=that->cgNodes.end(); it++)
		cgNodes.insert(*it);
}

// initializes the cgNodes set
void CGFunction::initCGNodes()
{
	set<SgGraphNode *> nodes = graph->computeNodeSet();
	for(set<SgGraphNode*>::iterator itn = nodes.begin(); itn != nodes.end(); itn++) {
		SgNode* n = (*itn)->get_SgNode();
		ROSE_ASSERT(isSgFunctionDeclaration(n));
		
		SgFunctionDeclaration* cfgDecl = getCanonicalDecl(isSgFunctionDeclaration(n));

		/*// if this declaration is in the list of known declarations for this function
		if(decls.find(decl) != decls.end())*/
		// if the given SgGraphNode refers to this function
		if(cfgDecl == decl)
			cgNodes.insert(*itn);
	}
}

bool CGFunction::operator == (const CGFunction &that) const
{
	if(!this->Function::eq(that)) return false;
		
	// check that that's set of SgGraphNodes is a subset of this's set of SgGraphNodes
	for(set<SgGraphNode*>::const_iterator it = that.cgNodes.begin(); it!=that.cgNodes.end(); it++)
		if(cgNodes.find(*it) == cgNodes.end()) return false;
	
	// check that this's set of SgGraphNodes is a subset of that's set of SgGraphNodes
	for(set<SgGraphNode*>::iterator it = cgNodes.begin(); it!=cgNodes.end(); it++)
		if(that.cgNodes.find(*it) == that.cgNodes.end()) return false;
	
	
	printf("CGFunction::operator == returning true\n");
	return true;
}

bool CGFunction::operator != (const CGFunction &that) const
{
	return !(*this == that);
}

bool CGFunction::operator < (const CGFunction &that) const
{ return Function::lessThan(that); }
bool CGFunction::operator > (const CGFunction &that) const
{ return !((*this) < that) && ((*this) != that); }
bool CGFunction::operator <= (const CGFunction &that) const
{ return ((*this) < that) || ((*this) == that); }
bool CGFunction::operator >= (const CGFunction &that) const
{ return !((*this) < that); }

/*************************************
 ********* TraverseCallGraph *********
 *************************************/
 
  #include <sys/time.h>
 
TraverseCallGraph::TraverseCallGraph(SgIncidenceDirectedGraph* graph)
{
	this->graph = graph;
	
/*	struct timeval itv;
	struct timezone itz;
	struct timeval stv;
	struct timezone stz;
	struct timeval ntv;
	struct timezone ntz;
	
	gettimeofday(&itv, &itz);
	gettimeofday(&stv, &stz);*/
	
//printf("TraverseCallGraph::TraverseCallGraph\n");
	set<SgGraphNode *> nodes = graph->computeNodeSet();
	for(set<SgGraphNode*>::iterator itn = nodes.begin(); itn != nodes.end(); itn++) {
		SgNode* n = (*itn)->get_SgNode();
		ROSE_ASSERT(isSgFunctionDeclaration(n));
		
		//gettimeofday(&ntv, &ntz); printf("0 time = %lf secs, elapsed = %lf secs\n", (ntv.tv_sec + 1000000*ntv.tv_usec) - (stv.tv_sec + 1000000*stv.tv_usec), (ntv.tv_sec + 1000000*ntv.tv_usec) - (itv.tv_sec + 1000000*itv.tv_usec)); stv.tv_sec = ntv.tv_sec; stv.tv_usec = ntv.tv_usec; 
		CGFunction func(isSgFunctionDeclaration(n), graph);
		//gettimeofday(&ntv, &ntz); printf("1 time = %lf secs, elapsed = %lf secs\n", (ntv.tv_sec + 1000000*ntv.tv_usec) - (stv.tv_sec + 1000000*stv.tv_usec), (ntv.tv_sec + 1000000*ntv.tv_usec) - (itv.tv_sec + 1000000*itv.tv_usec)); stv.tv_sec = ntv.tv_sec; stv.tv_usec = ntv.tv_usec; 
		
		//printf("curFunction = %s\n", (*itn)->functionDeclaration->get_name().str());
		
		// if this function is not compiler-generated
		if(!func.get_file_info()->isCompilerGenerated())
		{
			// add the mapping to decl2CFNode
			//decl2CFNode[func].insert(*itn);
			/*printf("TraverseCallGraph::TraverseCallGraph inserting %s\n", func.get_name().str());
			printf("TraverseCallGraph::TraverseCallGraph functions.find(func) == functions.end() = %d\n", functions.find(func) == functions.end());*/
			
			functions.insert(func);
		}
		//gettimeofday(&ntv, &ntz); printf("2 time = %lf secs, elapsed = %lf secs\n", (ntv.tv_sec + 1000000*ntv.tv_usec) - (stv.tv_sec + 1000000*stv.tv_usec), (ntv.tv_sec + 1000000*ntv.tv_usec) - (itv.tv_sec + 1000000*itv.tv_usec)); stv.tv_sec = ntv.tv_sec; stv.tv_usec = ntv.tv_usec; 
	}
	//gettimeofday(&ntv, &ntz); printf("A time = %lf secs, elapsed = %lf secs\n", (ntv.tv_sec + 1000000*ntv.tv_usec) - (stv.tv_sec + 1000000*stv.tv_usec), (ntv.tv_sec + 1000000*ntv.tv_usec) - (itv.tv_sec + 1000000*itv.tv_usec)); stv.tv_sec = ntv.tv_sec; stv.tv_usec = ntv.tv_usec; 
	
	// initially, put all the nodes into noPred
	// initialized numCallers
	/*for(map<SgFunctionDefinition*, set<SgGraphNode*> >::iterator it = decl2CFNode.begin(); 
	    it!=decl2CFNode.end(); it++)*/
	for(set<CGFunction>::iterator it = functions.begin(); it != functions.end(); it++)
	{
		noPred.insert((&*it));
		//numCallers[it->first] = 0;
		numCallers[(&*it)] = 0;
		//printf("noPred: %s\n", it->first->get_declaration()->get_name().str());
	}
	
	//gettimeofday(&ntv, &ntz); printf("B time = %lf secs, elapsed = %lf secs\n", (ntv.tv_sec + 1000000*ntv.tv_usec) - (stv.tv_sec + 1000000*stv.tv_usec), (ntv.tv_sec + 1000000*ntv.tv_usec) - (itv.tv_sec + 1000000*itv.tv_usec)); stv.tv_sec = ntv.tv_sec; stv.tv_usec = ntv.tv_usec; 
	// next, toss out all the nodes that have predecessor nodes
	// and compute the correct numCallers mapping
	
	// set of edges that have been visited by the traversal
	set<pair<const CGFunction*, const CGFunction*> > touchedEdges;
	/*for(map<SgFunctionDefinition*, set<SgGraphNode*> >::iterator it = decl2CFNode.begin(); 
	    it!=decl2CFNode.end(); it++)*/
	for(set<CGFunction>::iterator func = functions.begin(); func != functions.end(); func++)
	{
		//printf("curFunction = %s\n", (*func).get_name().str());
		for(CGFunction::iterator it = (*func).successors(); it != (*func).end(); it++)
		{
			const CGFunction* target = it.getTarget(functions);
			
			// if the target is compiler-generated, skip it
			if(target==NULL) continue;
			
			// if we haven't yet touched this edge	
			pair<const CGFunction*, const CGFunction*> edge(&(*func), target);
			if(touchedEdges.find(edge) == touchedEdges.end())
			{
				// increment the target function's number of callers
				numCallers[target]++;
				//printf("       numCallers[%s]=%d\n", tgtDecl->get_name().str(), numCallers[tgtDecl]);
				
				// if the given callee is currently in noPred
				set<const CGFunction*>::iterator pred_it = noPred.find(target);
				//printf("    pred_it == noPred.end() = %d\n", pred_it == noPred.end());
				if(pred_it != noPred.end())
					// remove it
					noPred.erase(target);
			}
			
			// add this edge to touchedEdges
			touchedEdges.insert(edge);
		}
		//gettimeofday(&ntv, &ntz); printf("C time = %lf secs, elapsed = %lf secs\n", (ntv.tv_sec + 1000000*ntv.tv_usec) - (stv.tv_sec + 1000000*stv.tv_usec), (ntv.tv_sec + 1000000*ntv.tv_usec) - (itv.tv_sec + 1000000*itv.tv_usec)); stv.tv_sec = ntv.tv_sec; stv.tv_usec = ntv.tv_usec; 
	}
	//gettimeofday(&ntv, &ntz); printf("D time = %lf secs, elapsed = %lf secs\n", (ntv.tv_sec + 1000000*ntv.tv_usec) - (stv.tv_sec + 1000000*stv.tv_usec), (ntv.tv_sec + 1000000*ntv.tv_usec) - (itv.tv_sec + 1000000*itv.tv_usec)); stv.tv_sec = ntv.tv_sec; stv.tv_usec = ntv.tv_usec; 
}

// returns a pointer to a CGFunction that matches the given declaration or NULL if no objects do
// the memory of the object persists for the entire lifetime of this TraverseCallGraph object
const CGFunction* TraverseCallGraph::getFunc(SgFunctionDeclaration* decl)
{
	CGFunction func(decl, graph);
	
	set<CGFunction>::iterator findLoc = functions.find(func);
	if(findLoc == functions.end())
		return NULL;
	
	return &(*findLoc);
}

// returns a pointer to a CGFunction object that matches the given Function object
// the memory of the object persists for the entire lifetime of this TraverseCallGraph object
const CGFunction* TraverseCallGraph::getFunc(const Function& func)
{
	return getFunc(func.get_declaration());
}

/************************************
 ***** TraverseCallGraphTopDown *****
 ************************************/

template <class InheritedAttribute>
TraverseCallGraphTopDown<InheritedAttribute>::TraverseCallGraphTopDown(SgIncidenceDirectedGraph* graph): 
	TraverseCallGraph(graph)
{}

template <class InheritedAttribute>
void TraverseCallGraphTopDown<InheritedAttribute>::traverse()
{
	map<const CGFunction*, funcRecord> visitRecords;
	// set of edges that have been visited by the traversal
	set<pair<const CGFunction*, const CGFunction*> > touchedEdges;
	
	//printf("traverse: noPred.size()=%d\n", noPred.size());
	
	// start the traversal from the nodes that are called from no other node
	for(set<const CGFunction*>::iterator it = noPred.begin(); it!=noPred.end(); it++)
	{
		//printf("        *it=0x%x\n", *it);
		
		// create a default attribute
		InheritedAttribute funcAttr = defaultAttrVal();
		//printf("TraverseCallGraphTopDown::traverse() funcDecl=%s\n", (*it)->get_declaration()->get_name().str());
		traverse_rec(*it, visitRecords, touchedEdges, funcAttr);
	}
}

template <class InheritedAttribute>
void TraverseCallGraphTopDown<InheritedAttribute>::traverse_rec(const CGFunction* fd, 
	                                         map<const CGFunction*, funcRecord> &visitRecords, 
	                                         set<pair<const CGFunction*, const CGFunction*> > &touchedEdges,
	                                         InheritedAttribute &fromCaller)
{
	/*printf("traverse_rec: func: 0x%x\n", fd);
	printf("traverse_rec: func->get_declaration(): 0x%x\n", fd->get_declaration());
	printf("traverse_rec: func->get_definition(): 0x%x\n", fd->get_definition());*/
	
	//printf("traverse_rec: %s()\n", fd->get_name().str());


	// add the caller's attribute to its list of inherited attributes
	visitRecords[fd].fromCallers.push_back(fromCaller);
	
	/*printf("                    visitRecords[fd].fromCallers.size()=%d\n", visitRecords[fd].fromCallers.size());
	printf("                    numCallers[fd]=%d\n", numCallers[fd]);*/
	
	// if we've added all of this function's inherited attributes to its record
	if(visitRecords[fd].fromCallers.size()>=numCallers[fd])
	{
		// call visit the current function
		InheritedAttribute results = visit(fd, visitRecords[fd].fromCallers);
	
		//printf("SgGraphNode: %s\n", fd->get_declaration()->unparseToString().c_str());
		// Pass the visit function's output to its callees
		for(CGFunction::iterator it = fd->successors(); it != fd->end(); it++)
		{
			const CGFunction* target = it.getTarget(functions);
			
			// if the target is compiler-generated, skip it
			if(target==NULL) continue;
			
			// if we haven't yet touched this edge	
			pair<const CGFunction*, const CGFunction*> edge(fd, target);
			if(touchedEdges.find(edge) == touchedEdges.end())
			{
				/*printf("    calls: target = 0x%x\n", target);
				printf("    calls: target->get_declaration() = 0x%x\n", target->get_declaration());
				printf("    calls: target->get_definition() = 0x%x\n", target->get_definition());
				printf("    calls: unparse = %s\n", target->get_declaration()->unparseToString().c_str());*/
				//printf("    calls: target %s\n", target->get_name().str());
				// recurse down this edge
				traverse_rec(target, visitRecords, touchedEdges, results);
				//printf("    |---------|\n");
			}
			
			//printf("adding edge from %s to %s\n", fd->get_name().str(), tgtFuncDecl->get_name().str());
			// add this edge to touchedEdges
			touchedEdges.insert(edge);
		}
	}
}

/*************************************
 ***** TraverseCallGraphBottomUp *****
 *************************************/

template <class SynthesizedAttribute>
TraverseCallGraphBottomUp<SynthesizedAttribute>::TraverseCallGraphBottomUp(SgIncidenceDirectedGraph* graph): 
	TraverseCallGraph(graph)
{}

template <class SynthesizedAttribute>
void TraverseCallGraphBottomUp<SynthesizedAttribute>::traverse()
{
	map<const CGFunction*, SynthesizedAttribute> visitRecords;
	// set of edges that have been visited by the traversal
	set<pair<const CGFunction*, const CGFunction*> > touchedEdges;
	
	// start the traversal from the nodes that are called from no other node
	for(set<const CGFunction*>::iterator it = noPred.begin(); it!=noPred.end(); it++)
	{
		//printf("        *it=0x%x\n", *it);
		
		//printf("TraverseCallGraphTopDown::traverse() funcDecl=%s\n", (*it)->get_name().str());
		traverse_rec(*it, visitRecords, touchedEdges);
	}
}

template <class SynthesizedAttribute>
SynthesizedAttribute TraverseCallGraphBottomUp<SynthesizedAttribute>::traverse_rec(
	                                   const CGFunction* fd, 
                                      map<const CGFunction*, SynthesizedAttribute> &visitRecords, 
                                      set<pair<const CGFunction*, const CGFunction*> > &touchedEdges)
{
	// list of attributes from this function's callees
	list <SynthesizedAttribute> fromCallees;
	
	//printf("traverse_rec::function: %s\n", fd->get_name().str());
	//printf("<<<\n");
	// traverse over the callees and compute the list of their synthesized attributes (fromCallees)
	for(CGFunction::iterator it = fd->successors(); it != fd->end(); it++)
	{
		const CGFunction* target = it.getTarget(functions);
			
		// if the target is compiler-generated, skip it
		if(target==NULL) continue;
		//printf("    calls: %s\n", tgtFuncDecl->get_name().str());
		
		// if we haven't yet called this function before
		pair<const CGFunction*, const CGFunction*> edge(fd, target);
		if(visitRecords.find(target) == visitRecords.end())
		{
			//printf("        new edge/function\n");
			// recurse down to this function, storing its result in visitRecords
			traverse_rec(target, visitRecords, touchedEdges);
		}
		
		// if we haven't yet touched this edge
		if(touchedEdges.find(edge) == touchedEdges.end())
		{
			// add the current callee's synthesized attribute into the list for the current function
			fromCallees.push_back(visitRecords[target]);
			
			//printf("   adding edge from %s to %s\n", fd->get_name().str(), tgtFuncDecl->get_name().str());
			if(touchedEdges.find(edge) == touchedEdges.end())
				// add this edge to touchedEdges
				touchedEdges.insert(edge);
		}
	}
	
	// if this function has no callees, fill fromCallees with just a single default instance of SynthesizedAttribute
	if(fromCallees.size()==0)
	{
		SynthesizedAttribute deft = defaultAttrVal();
		fromCallees.push_back(deft);
	}
	
	SynthesizedAttribute res = visit(fd, fromCallees);
	// add the current function's result to the visitRecords for use by this function's callers
	visitRecords[fd] = res;
	//printf(">>>\n");
}

//}

/*************************************
 ***** TraverseCallGraphFreeform *****
 *************************************/
TraverseCallGraphDataflow::TraverseCallGraphDataflow(SgIncidenceDirectedGraph* graph): TraverseCallGraph(graph)
{}

void TraverseCallGraphDataflow::traverse()
{
	// start the traversal from the nodes that are called from no other node
	for(set<const CGFunction*>::iterator it = noPred.begin(); it!=noPred.end(); it++)
		remaining.push_back(*it);
	
	// traverse functions for as long as visit keeps adding them to remaining
	while(remaining.size()>0)
	{
		const CGFunction* func = remaining.front();
		remaining.pop_front();
		visit(func);
	}
}

// adds func to the back of the remaining list, if its not already there
void TraverseCallGraphDataflow::addToRemaining(const CGFunction* func)
{
	for(list<const CGFunction*>::iterator it = remaining.begin(); it!=remaining.end(); it++)
		// if this function is already in remaining, don't bother inserting it
		if(*it == func)
			return;

	// insert func, if it was not found
	remaining.push_back(func);
}

/*********************************************************
 ***               numCallersAnnotator                 ***
 *** Annotates every function's SgFunctionDefinition   ***
 *** node with a numCallersAttribute that contains the ***
 *** number of functions that call the given function. ***
 *********************************************************/
class numCallersAnnotator_Int
{
	public:
	int val;
	
	numCallersAnnotator_Int()
	{
		val=0;
	}
	
	numCallersAnnotator_Int(int val)
	{
		this->val=val;
	}
};

class numCallersAnnotator : public virtual TraverseCallGraphTopDown<numCallersAnnotator_Int>
{
	public:
	numCallersAnnotator(SgIncidenceDirectedGraph* graph) : TraverseCallGraphTopDown<numCallersAnnotator_Int>(graph)
	{}
	
	numCallersAnnotator_Int visit(const CGFunction* func, list<numCallersAnnotator_Int>& fromCallers)
	{
		//printf("visit: fromCallers.size()=%d\n", fromCallers.size());
		
		int totalCallers=0;
		for(list<numCallersAnnotator_Int>::iterator it = fromCallers.begin(); it!=fromCallers.end(); it++)
		{
			totalCallers+=(*it).val;
		}
		//printf("annotating %s / 0x%x\n", func->get_name().str(), func->get_declaration());
		func->get_declaration()->addNewAttribute("numCallers", new numCallersAttribute(totalCallers));
		
		numCallersAnnotator_Int retInt(1);
		return retInt;
	}
};

// = true if annotateNumCallers() has been called and =false otherwise
static bool calledAnnotateNumCallers = false;

// annotates every fucntion's SgFunctionDefinition node with a numCallersAttribute that contains
// the number of functions that call the given function
void annotateNumCallers(SgIncidenceDirectedGraph* graph)
{
	numCallersAnnotator nca(graph);
	nca.traverse();
	calledAnnotateNumCallers = true;
}

// returns the number of functions that call this function or 0 if the function is compiler-generated
int getNumCallers(const Function* func)
{
	ROSE_ASSERT(calledAnnotateNumCallers);
	
	if(func->get_declaration()->get_file_info()->isCompilerGenerated()) return 0;
	//printf("looking up numCallers of func=0x%x=%s()\n", func, func->get_declaration()->get_name().str());
	
	//printf("looking up %s / 0x%x\n", func->get_name().str(), func->get_declaration());
	
	return ((numCallersAttribute*)func->get_declaration()->getAttribute("numCallers"))->getNumCallers();
}

#endif
