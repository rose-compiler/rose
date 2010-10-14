#ifndef CALL_GRAPH_TRAVERSE_H
#define CALL_GRAPH_TRAVERSE_H

#include "common.h"
#include "CallGraph.h"
#include <set>
#include <list>


//namespace CallGraph
//{

/* !!! NOTE: TraverseCallGraphDataflow LIMITED TO NON-RECURSIVE PROGRAMS (I.E. CONTROL FLOW GRAPHS WITH NO CYCLES) !!! */

class Function
{
	protected:
	//SgFunctionDefinition* def;
	//set<SgFunctionDeclaration*> decls;
	SgFunctionDeclaration* decl;

	public:
	Function();
	
	Function(string name);
	
	Function(SgFunctionDeclaration* sample);
	
	Function(SgFunctionDefinition* sample);
	
	Function(SgFunctionCallExp* funcCall);
	
	void init(SgFunctionDeclaration* sample);
		
	public:
	Function(const Function &that);
	
	Function(const Function *that);
	
	// returns a unique SgFunctionDeclaration* that is the canonical AST node that represents the given function
	static SgFunctionDeclaration* getCanonicalDecl(SgFunctionDeclaration* decl);
	
	bool eq(const Function &that) const;
	
	bool operator == (const Function &that) const;
	bool operator != (const Function &that) const;
	
	protected:
	bool lessThan(const Function &that) const;
	public:
	bool operator < (const Function &that) const;
	bool operator > (const Function &that) const;
	bool operator <= (const Function &that) const;
	bool operator >= (const Function &that) const;
	
	SgName get_name() const;
	
	// returns this function's definition or NULL of it does not have one
	SgFunctionDefinition* get_definition() const;
	
	// returns one of this function's declarations. it is guaranteed to be the same each time get_declaration is called
	SgFunctionDeclaration* get_declaration() const;
	
	// returns the file_info of the definition or one of the declarations if there is no definition
	Sg_File_Info* get_file_info() const;
	
	// returns the parameters of this function
	SgInitializedNamePtrList get_params();
};

// extension of the generic function class that also contains all the SgGraphNodes that refer to the function
class CGFunction : public Function
{
	set<SgGraphNode*> cgNodes;
	SgIncidenceDirectedGraph* graph;
	
	public:
	CGFunction(string name, SgIncidenceDirectedGraph* graph);
	
	CGFunction(SgFunctionDeclaration* sample, SgIncidenceDirectedGraph* graph);
	
	CGFunction(SgGraphNode* sample, SgIncidenceDirectedGraph* graph);
	
	CGFunction(const CGFunction &that);
	
	CGFunction(const CGFunction *that);
	
	protected:
	// initializes the cgNodes set
	void initCGNodes();
	
	public:	
	bool operator == (const CGFunction &that) const;
	bool operator != (const CGFunction &that) const;
	
	bool operator < (const CGFunction &that) const;
	bool operator > (const CGFunction &that) const;
	bool operator <= (const CGFunction &that) const;
	bool operator >= (const CGFunction &that) const;
	
	// Traverses the callers or callees of this function
	class iterator
	{
		
		// direction in which the iterator is going
		//SgIncidenceDirectedGraph::EdgeDirection iterDir;
		// side of an edge that we're going to be following
		//SgIncidenceDirectedGraph::EdgeDirection edgeDir;
		
		// Direction in which the iterator is going (fw: from callers to callees, bw: from callees to callers)
		public:
		//typedef enum direction {fw=0, bw=1};
		enum direction {fw=0, bw=1};
		protected:
		direction dir;
		
		set<SgGraphNode*>::iterator itn;
		set<SgDirectedGraphEdge*> edges; // The current SgGraphNode's (*itn) incoming or outgoing edges
		set<SgDirectedGraphEdge*>::iterator ite; // The current edge in edges
			
		// The set the SgGraphNodes that have already been visited by this iterator
		// used to eliminate duplicates
		set<SgGraphNode*> visitedCGNodes;
			
		const CGFunction* func;
		
		// =true if this iterator has reached the end of all the edges and =false if not
		bool finished;
			
		public:
		iterator()
		{
			finished = true;
		}
			
		iterator(const CGFunction* const func, direction dir)
		{
			this->func = func;
			this->dir = dir;
			
			finished=false;
			
			itn = func->cgNodes.begin();
			if(dir == fw) edges = func->graph->computeEdgeSetOut(*itn);
			else          edges = func->graph->computeEdgeSetIn(*itn);
			ite = edges.begin();
			
			advanceToNextValidPoint();
		}
		
		// Returns a reference to CGFunction that matches the current SgGraphNode that this iterator refers to,
		// pulling the reference from the given set of CGFunctions
		const CGFunction* getTarget(set<CGFunction> &functions)
		{
			//printf("getTarget finished=%d\n", finished);
			SgGraphNode* target = (dir == fw ? (*ite)->get_to() : (*ite)->get_from());
			ROSE_ASSERT(isSgFunctionDeclaration(target->get_SgNode()));
			
			// Compiler-generated functions do not appear as nodes in the call graph
			if(isSgFunctionDeclaration(target->get_SgNode())->get_file_info()->isCompilerGenerated()) return NULL;
			
			// Find the CGFunction in functions that matches the target SgGraphNode
			for(set<CGFunction>::const_iterator it = functions.begin(); it!=functions.end(); it++)
			{
				//printf("    iteration. current: %s isCompilerGenerated=%d, target=%s, isCompilerGenerated=%d\n", (*it).get_name().str(), (*it).get_declaration()->get_file_info()->isCompilerGenerated(), target->functionDeclaration->get_name().str(), target->functionDeclaration->get_file_info()->isCompilerGenerated());
				// If the target SgGraphNode can be found in the current CGFunction
				if((&(*it))->cgNodes.find(target) != (&(*it))->cgNodes.end())
					return &(*it);
			}
			
			//printf("getTarget returning NULL\n");
			
			// If we can't find it, return NULL
			ROSE_ASSERT(!"Error finding the target function of a call graph edge when the target is not compiler generated!");
			return NULL;
		}
		
		// Returns the function that this iterator is currently referring to
		Function getTarget()
		{
			SgGraphNode* target = (dir == fw ? (*ite)->get_to() : (*ite)->get_from());
			ROSE_ASSERT(isSgFunctionDeclaration(target->get_SgNode()));
			Function result(isSgFunctionDeclaration(target->get_SgNode()));
			return result;
		}
		
		void operator ++( int )
		{
			ite++;

			advanceToNextValidPoint();
		}
		
		// If the current <itn, ite> pair refers to a specific CallGraph node, does nothing.
		// otherwise, advances the <itn, ite> pair until it does refer to a specific CallGraph node
		void advanceToNextValidPoint()
		{
			//printf("Function::iterator::advanceToNextValidPoint()\n");
			// Loop for as long as ite is not pointing to a valid node and hasn't reached the end of the node list
			while(1)
			{
				// If ite is the last incoming/outgoing edge of the current SgGraphNode
				if(ite == edges.end())
				{
					//printf("Function::iterator::advanceToNextValidPoint() while()\n");
					// Advance to the next SgGraphNode in cgNodes
					itn++;
					
					// If this is not the last SgGraphNode in cgNodes
					if(itn != func->cgNodes.end())
					{
						// Set edges to the incoming/outgoing edges of the new SgGraphNode and set ite to refer to the first edge
						if(dir == fw) edges = func->graph->computeEdgeSetOut(*itn);
						else          edges = func->graph->computeEdgeSetIn(*itn);
						ite = edges.begin();
					}
					// otherwise, leave the loop since this iterator has reached the end
					else
					{
						finished=true;
						break;
					}
				}
				// Else, if it is not the last edges
				else
				{
					SgGraphNode* target = (dir == fw ? (*ite)->get_to() : (*ite)->get_from());
					// If we've already seen this node
					if(visitedCGNodes.find(target) != visitedCGNodes.end())
						ite++;
					// Else, we have a valid node. Record that we've visited it and break out since we've found a valid upstream/downstream node
					else {
						visitedCGNodes.insert(target);
						break;
					}
				}
			}
		}
		
		bool operator == (iterator that)
		{
			// if either iterators are finished, then they're equal iff the other is finished, ignoring any other fields
			if(finished) return that.finished;
			else if(that.finished) return finished;
				
			// otherwise, they're equal only if all their other members are equal
			return (dir == that.dir) &&
			       (itn == that.itn) &&
			       (edges == that.edges) &&
			       (ite == that.ite) &&
			       (func == that.func);
		}
		
		bool operator != (iterator that)
		{ return !((*this) == that); }
	};
	
	// Returns an iterator that traverses the callees of this function
	iterator callees() const
	{
		iterator b(this, iterator::fw);
		return b;
	}
	iterator successors() const
	{
		iterator b(this, iterator::fw);
		return b;
	}
	
	// Returns an iterator that traverses all the callers of this function
	iterator callers() const
	{
		iterator b(this, iterator::bw);
		return b;
	}
	iterator predecessors() const
	{
		iterator b(this, iterator::bw);
		return b;
	}
	
	iterator end() const
	{
		iterator b;
		return b;
	}
};

class TraverseCallGraph
{
	protected:
	SgIncidenceDirectedGraph* graph;
	// maps each SgFunctionDefinition to its associated SgGraphNodes
	//   (there may be more than one such node for a given SgFunctionDefinition)
	//map<SgFunctionDefinition*, set<SgGraphNode*> > decl2CFNode;
	
	// The set of functions in this program
	set<CGFunction> functions;
	
	// The number of functions that call each given function
	//map<SgFunctionDefinition*, int> numCallers;
	map<const CGFunction*, int> numCallers;
	
	// set of all the SgFunctionDefinition for all functions that are not called from other functions
	//set<SgFunctionDefinition*> noPred;
	// Set of functions that are not called from other functions.
	// Just contains pointers to the CGFunction objects inside the functions set.
	set<const CGFunction*> noPred;
	
	public:
	TraverseCallGraph(SgIncidenceDirectedGraph* graph);
	
	// Returns a pointer to a CGFunction that matches the given declaration.
	// The memory of the object persists for the entire lifetime of this TraverseCallGraph object.
	const CGFunction* getFunc(SgFunctionDeclaration* decl);
	
	// Returns a pointer to a CGFunction object that matches the given Function object.
	// The memory of the object persists for the entire lifetime of this TraverseCallGraph object.
	const CGFunction* getFunc(const Function& func);
};


template <class InheritedAttribute>
class TraverseCallGraphTopDown : public TraverseCallGraph
{
	class funcRecord
	{
		public:
		// the inherited attributes passed down from callers
		list<InheritedAttribute> fromCallers;
	};
	
	public:
	TraverseCallGraphTopDown(SgIncidenceDirectedGraph* graph);
	
	void traverse();
	
	virtual InheritedAttribute visit(const CGFunction* func, list<InheritedAttribute>& fromCallers)=0;
	
	virtual InheritedAttribute defaultAttrVal() { InheritedAttribute val; return val; }
	
	protected:
	void traverse_rec(const CGFunction* fd, 
	                  map<const CGFunction*, funcRecord> &visitRecords, 
	                  set<pair<const CGFunction*, const CGFunction*> > &touchedEdges,
	                  InheritedAttribute &fromCaller);
};


template <class SynthesizedAttribute>
class TraverseCallGraphBottomUp : public TraverseCallGraph
{
	public:
	TraverseCallGraphBottomUp(SgIncidenceDirectedGraph* graph);
	
	void traverse();
	
	virtual SynthesizedAttribute visit(const CGFunction* func, list<SynthesizedAttribute> fromCallees)=0;
	
	virtual SynthesizedAttribute defaultAttrVal() { SynthesizedAttribute val; return val; }

	protected:
	SynthesizedAttribute traverse_rec(const CGFunction* fd, 
                  map<const CGFunction*, SynthesizedAttribute> &visitRecords, 
                  set<pair<const CGFunction*, const CGFunction*> > &touchedEdges);
};

// CallGraph traversal useful for inter-procedural dataflow analyses because it starts
// at the functions that are not called by any other function and allows such
// analyses to keep adding more nodes depending on how function dataflow information changes
class TraverseCallGraphDataflow : public TraverseCallGraph
{
	public:
	// list of functions that still remain to be processed;
	list<const CGFunction*> remaining;
		
	TraverseCallGraphDataflow(SgIncidenceDirectedGraph* graph);
	
	void traverse();
	
	virtual void visit(const CGFunction* func)=0;
	
	// adds func to the back of the remaining list, if its not already there
	void addToRemaining(const CGFunction* func);
};

/*********************************************************
 ***               numCallersAnnotator                 ***
 *** Annotates every function's SgFunctionDefinition   ***
 *** node with a numCallersAttribute that contains the ***
 *** number of functions that call the given function. ***
 *********************************************************/
 
// The attribute that is associated with each function's declaration to
// record its number of callers.
class numCallersAttribute : public AstAttribute
{
	int numCallers;
	
	public: 
	numCallersAttribute()
	{
		numCallers = 0;
	}
	
	numCallersAttribute(int numCallers)
	{
		this->numCallers = numCallers;
	}
	
	numCallersAttribute(numCallersAttribute& that)
	{
		this->numCallers = that.numCallers;
	}
	
	int getNumCallers()
	{
		return numCallers;
	}
};

// Uses the numCallersAnnotator class to annotate every function's SgFunctionDefinition node 
// with a numCallersAttribute that contains the number of functions that call the given function.
void annotateNumCallers(SgIncidenceDirectedGraph* graph);

// Returns the number of functions that call this function or 0 if the function is compiler-generated.
int getNumCallers(const Function* fDecl);

/*class funcCallersAnalysis
{
	bool analyzed=false;
	
	// maps each function to the list of its callers, each caller record is a pair, containing the SgFunctionCallExp 
	// that is the function call and the function that this call is inside of
	map<Function, list<pair<SgFunctionCallExp*, Function> > > callersMap;
	
	void analyze()
	{
		if(!analyzed)
		{
			
		}
		analyzed = true;
	}
	
	public: 
	void resetAnalysis()
	{
		analyzed = false;
	}
	
	
	list<pair<SgFunctionCallExp*, Function> > getFuncCallers()
	{
		
	}
}*/
//}

#endif
