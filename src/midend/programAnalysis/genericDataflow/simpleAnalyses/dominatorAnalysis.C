#include "dominatorAnalysis.h"
#include <assert.h>
#include <algorithm>

int dominatorAnalysisDebugLevel=0;

// ############################
// ##### DominatorLattice #####
// ############################

//DominatorLattice::DominatorLattice()
//{ level = uninitialized; }
DominatorLattice::DominatorLattice(const DataflowNode& n) : n(n)
{ 
        level = uninitialized;
}
DominatorLattice::DominatorLattice(const DataflowNode& n, const DataflowNode& nodes) : n(n)
{
        domNodes.insert(nodes);
        level = initialized;
}
DominatorLattice::DominatorLattice(const DataflowNode& n, const set<DataflowNode>& nodes) : n(n)
{
        domNodes = nodes;
        level = initialized;
}
DominatorLattice::DominatorLattice(const DominatorLattice& that) : n(that.n)
{
        level = that.level;
        if(level == initialized)
                domNodes = that.domNodes;
}

// Initializes this Lattice to its default state, if it is not already initialized
void DominatorLattice::initialize()
{
        level = initialized;
}

// Returns a copy of this lattice
Lattice* DominatorLattice::copy() const
{
        return new DominatorLattice(*this);
}

// Overwrites the state of this Lattice with that of that Lattice
void DominatorLattice::copy(Lattice* that_arg)
{
        DominatorLattice* that = dynamic_cast<DominatorLattice*>(that_arg);
        level = that->level;
        n = that->n;
        if(level == initialized)
                domNodes = that->domNodes;
}

// Overwrites the state of this Lattice with that of that Lattice.
// Returns true if this causes this Lattice to chance and false otherwise.
bool DominatorLattice::copyFrom(DominatorLattice* that, string indent)
{
        bool modified=false;
        
        modified = (level != that->level);
        level = that->level;
        
        modified = (n != that->n);
        n = that->n;
        
        if(level == initialized) {
                modified = modified || (domNodes != that->domNodes);
                domNodes = that->domNodes;
        }
        return modified;
}

// Called by analyses to create a copy of this lattice. However, if this lattice maintains any 
//    information on a per-variable basis, these per-variable mappings must be converted from 
//    the current set of variables to another set. This may be needed during function calls, 
//    when dataflow information from the caller/callee needs to be transferred to the callee/calleer.
// We do not force child classes to define their own versions of this function since not all
//    Lattices have per-variable information.
// varNameMap - maps all variable names that have changed, in each mapping pair, pair->first is the 
//              old variable and pair->second is the new variable
// func - the function that the copy Lattice will now be associated with
void DominatorLattice::remapVars(const map<varID, varID>& varNameMap, const Function& newFunc)
{
        // Nothing to do. This analysis doesn't maintain per-variable information
}

// Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
// that contains data for a set of variables and incorporateVars must overwrite the state of just
// those variables, while leaving its state for other variables alone.
// We do not force child classes to define their own versions of this function since not all
//    Lattices have per-variable information.
void DominatorLattice::incorporateVars(Lattice* that_arg)
{
        // Nothing to do. This analysis doesn't maintain per-variable information
        DominatorLattice* that = dynamic_cast<DominatorLattice*>(that_arg);
        copyFrom(that, "    ");
}

// Returns a Lattice that describes the information known within this lattice
// about the given expression. By default this could be the entire lattice or any portion of it.
// For example, a lattice that maintains lattices for different known variables and expression will 
// return a lattice for the given expression. Similarly, a lattice that keeps track of constraints
// on values of variables and expressions will return the portion of the lattice that relates to
// the given expression. 
// It it legal for this function to return NULL if no information is available.
// The function's caller is responsible for deallocating the returned object
Lattice* DominatorLattice::project(SgExpression* expr)
{
        // Nothing to do. This analysis doesn't maintain per-variable information
        return new DominatorLattice(n); //copy();
}

// The inverse of project(). The call is provided with an expression and a Lattice that describes
// the dataflow state that relates to expression. This Lattice must be of the same type as the lattice
// returned by project(). unProject() must incorporate this dataflow state into the overall state it holds.
// Call must make an internal copy of the passed-in lattice and the caller is responsible for deallocating it.
// Returns true if this causes this to change and false otherwise.
bool DominatorLattice::unProject(SgExpression* expr, Lattice* exprState)
{
        // This analysis doesn't maintain per-variable information, so the information true of exprState is true for all variables.
        /*DominatorLattice* that = dynamic_cast<DominatorLattice*>(exprState);
        return copyFrom(that, "    ");*/
        return false;
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool DominatorLattice::meetUpdate(Lattice* that_arg)
{
        DominatorLattice* that = dynamic_cast<DominatorLattice*>(that_arg);

        //cout << "        DominatorLattice::meetUpdate()\n";   
        set<DataflowNode> toDelete;
        set<DataflowNode>::iterator itThis = domNodes.begin();
        set<DataflowNode>::iterator itThat = that->domNodes.begin();
        while(itThis!=domNodes.end() && itThat!=that->domNodes.end())
        {
                // cout << "        itThis: "<<itThis->getNode()->unparseToString() << " | " << itThis->getNode()->class_name()<<"\n";
                // cout << "        itThat: "<<itThat->getNode()->unparseToString() << " | " << itThat->getNode()->class_name()<<"\n";
                // If there is a node in this->domNodes but not in that->domNodes, remove it
                if(*itThis < *itThat) {
                        toDelete.insert(*itThis);
                        itThis++;
                // If there is a node in that->domNodes but not in this->domNodes, skip it
                } else if(*itThat < *itThis)
                        itThat++;
                else {
                        itThis++;
                        itThat++;
                }
        }
        // Record that any nodes in this->domNodes that have not been touched should be 
        // deleted since these nodes don't exist in that->domNodes
        while(itThis!=domNodes.end()) {
                toDelete.insert(*itThis);
                itThis++;
        }

        bool modified = false;  
        // Delete all nodes in toDelete from domNodes
        //cout << "            #toDelete="<<toDelete.size()<<"\n";
        for(set<DataflowNode>::iterator it=toDelete.begin(); it!=toDelete.end(); it++) {
                if(*it != n) {
                        modified = true;
                        //cout << "            Deleting "<<it->getNode()->unparseToString() << " | " << it->getNode()->class_name()<<"\n";
                        domNodes.erase(*it);
                }
        }
        
        return modified;
}

bool DominatorLattice::operator==(Lattice* that_arg)
{
        DominatorLattice* that = dynamic_cast<DominatorLattice*>(that_arg);
        return (this->level == that->level) &&
               (this->n == that->n) &&
               (this->domNodes == that->domNodes);
}

// Functions used to inform this lattice that a given variable is now in use (e.g. a variable has entered 
//    scope or an expression is being analyzed) or is no longer in use (e.g. a variable has exited scope or
//    an expression or variable is dead).
// It is assumed that a newly-added variable has not been added before and that a variable that is being
//    removed was previously added
// Returns true if this causes the lattice to change and false otherwise.
bool DominatorLattice::addNode(const DataflowNode& n, string indent)
{
        bool modified = (domNodes.find(n) == domNodes.end());
        domNodes.insert(n);
        return modified;
}
bool DominatorLattice::remNode(const DataflowNode& n, string indent)
{
        bool modified = (domNodes.find(n) != domNodes.end());
        domNodes.erase(n);
        return modified;
}

// Returns true if the given node dominates / post-dominates the node associated with this lattice
bool DominatorLattice::isDominator(const DataflowNode& n, string indent)
{
        return (domNodes.find(n) != domNodes.end());
}
                
// The string that represents this object
// If indent!="", every line of this string must be prefixed by indent
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string DominatorLattice::str(string indent)
{
        ostringstream outs;
        
        outs << "<DominatorLattice: n="<<n.getNode()->unparseToString() << " | " << n.getNode()->class_name()<<" domNodes=\n";
        for(set<DataflowNode>::iterator it=domNodes.begin(); it!=domNodes.end(); ) {
                outs << indent << "    " << it->getNode()->unparseToString() << " | " << it->getNode()->class_name();
                it++;
                if(it!=domNodes.end())
                        outs << "\n";
        }
        outs << ">";
        return outs.str();
}

// #############################
// ##### DominatorAnalysis #####
// #############################

DominatorAnalysis::DominatorAnalysis( const set<DataflowNode>& allNodes, string indent): 
                        allNodes(allNodes)
{
        
}
        
// Generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
void DominatorAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
        DominatorLattice* dLat;
        
        //cout << "    genInitState: "<<n.getNode()->unparseToString() << " | " << n.getNode()->class_name() << "\n";;
        // If n is the application's starting node
        //if(func.get_name().getString() == "main" && n == func.get_definition()->cfgForBeginning()/*cfgUtils::getFuncStartCFG(func.get_definition())*/) {
        assert(func.get_definition() != NULL);
        if(func.get_name().getString() == "main" && n == DataflowNode(func.get_definition()->cfgForBeginning(),filter) /*cfgUtils::getFuncStartCFG(func.get_definition())*/) {
                //cout << "        STARTING NODE\n";
                dLat = new DominatorLattice(n, n);
        } else {
                dLat = new DominatorLattice(n, allNodes);
        }
        //cout << "    "<<dLat->str("        ")<<"\n";
        initLattices.push_back(dLat);   
}

bool DominatorAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
        //DominatorLattice* dLat = dynamic_cast<DominatorLattice*>(*(dfInfo.begin()));
        bool modified = false;
        
        //modified = dLat->addNode(tgtNode) || modified;
        
        return modified;
}


// ################################
// ##### FindAllNodesAnalysis #####
// ################################

FindAllNodesAnalysis::FindAllNodesAnalysis(const Function& func, string indent) : func(func)
{}

void FindAllNodesAnalysis::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
        allNodes.insert(n);
}

// #####################################
// ##### Module Interface Routines #####
// #####################################

bool dominatorsComputed=false;
DominatorAnalysis* da=NULL;
// Set of all DataflowNodes within each function
map <Function, set<DataflowNode> > allFuncNodes;

// Returns the set of DataflowNodes that dominate the given node
const set<DataflowNode>& getDominators(SgProject* project, const Function& func, const DataflowNode& n, string indent)
{
        if(da==NULL) {
                // If we have not yet computed the set of all DataflowNodes within this function
                if(allFuncNodes.find(func) == allFuncNodes.end())
                {
                        // Run the FindAllNodesAnalysis to compute this set and save it in allFuncNodes[func]
                        FindAllNodesAnalysis fana(func);
                        UnstructuredPassInterAnalysis upia_fana(fana);
                        upia_fana.runAnalysis();
                        allFuncNodes[func] = fana.allNodes;
                }
                
                //cout << indent << "allFuncNodes[func]=\n";
                //for(set<DataflowNode>::iterator it=allFuncNodes[func].begin(); it!=allFuncNodes[func].end(); it++)
                //      cout << indent << "    " << it->getNode()->unparseToString() << " | " << it->getNode()->class_name() << "\n";;
                
                da = new DominatorAnalysis(allFuncNodes[func], indent+"    ");
                /*UnstructuredPassInterDataflow upid_da(da);
                upid_da.runAnalysis();*/
                CallGraphBuilder cgb(project);
                cgb.buildCallGraph();
                SgIncidenceDirectedGraph* graph = cgb.getGraph(); 
                ContextInsensitiveInterProceduralDataflow ciipd_da(da, graph);
                ciipd_da.runAnalysis();
                
                if(dominatorAnalysisDebugLevel>=1) printDominatorAnalysisStates(da, "[");
        }
        const vector<NodeState*> nodeStates = NodeState::getNodeStates(n);
        //NodeState* state = nodeStates[n.getIndex()];
        NodeState* state = *(nodeStates.begin());
        //cout << indent << "da="<<da<<" state="<<state<<" #nodeStates="<<nodeStates.size()<<" n.getIndex()="<<n.getIndex()<<" #nodeStates="<<nodeStates.size()<<"\n";
        //cout << indent << "state="<<state->str(da, indent+"    ")<<"\n";
        DominatorLattice* dLat = dynamic_cast<DominatorLattice*>(state->getLatticeAbove(da, 0));
        
        /*cout << indent << "dLat="<<dLat<<" dLat->domNodes:\n";        
        for(set<DataflowNode>::const_iterator dom=dLat->domNodes.begin(); dom!=dLat->domNodes.end(); dom++)
                cout << indent << "    " << dom->getNode()->unparseToString() << " | " << dom->getNode()->class_name() << "\n";*/
        
        return dLat->domNodes;  
}

// Returns true if node a dominates node b and false otherwise
bool dominates(const DataflowNode& a, const DataflowNode& b, string indent)
{
        /*const set<DataflowNode>& dominators = getDominators(const Function& func, b, indent+"    ");
        return (dominators.find(a) != dominators.end());*/
        return true;
}

// prints the Lattices set by the given LiveDeadVarsAnalysis 
void printDominatorAnalysisStates(DominatorAnalysis* da, string indent)
{
        vector<int> factNames;
        vector<int> latticeNames;
        latticeNames.push_back(0);
        printAnalysisStates pas(da, factNames, latticeNames, printAnalysisStates::above, indent);
        UnstructuredPassInterAnalysis upia_pas(pas);
        upia_pas.runAnalysis();
}
