// #################################################
// ############## CONSTRAINT GRAPHS ################
// #################################################
#include "ConstrGraph.h"
using std::string;
using std::pair;
using std::map;
#include <sys/time.h>
#include <algorithm>
#include <set>
using std::set;
#include <sstream>
using std::ostringstream;
#include <iostream>
using std::endl;
#include <utility>

using namespace cfgUtils;

int CGDebugLevel=1;
int CGmeetDebugLevel=1;
int CGprofileLevel=0;
int CGdebugTransClosure=1;

/**** Constructors & Destructors ****/
/*ConstrGraph::ConstrGraph(bool initialized, string indent="")
{
        // Start this constraint graph as Uninitialized or Bottom, as requested
        if(initialized) level = bottom;
        else            level = uninitialized;
        
        constrChanged=false;
        inTransaction=false;
}*/

ConstrGraph::ConstrGraph(const Function& func, const DataflowNode& n, const NodeState& state, bool initialized, string indent) :
        func(func), state(state)
{
        // Start this constraint graph as Uninitialized or Bottom, as requested
        if(initialized) level = bottom;
        else            level = uninitialized;
        
        constrChanged=false;
        inTransaction=false;
        
        set<NodeDesc> nodes; nodes.insert(NodeDesc(n, state));
        initCG(func, nodes, false, indent);
}

ConstrGraph::ConstrGraph(const Function& func, const DataflowNode& n, const NodeState& state, 
              LiveDeadVarsAnalysis* ldva, FiniteVarsExprsProductLattice* divL, 
              // GB : 2011-03-05 (Removing Sign Lattice Dependence)FiniteVarsExprsProductLattice* sgnL, 
              bool initialized, string indent) : func(func), state(state)
{
        this->ldva = ldva;
        // Initialize the map of divisibility and sign lattices, associating divL and sgnL with the 
        // wildcard annotation that matches all variables       
        if(divL) {
                pair<string, void*> noAnnot("", NULL);
                this->divL[noAnnot] = divL;
        }
        // GB : 2011-03-05 (Removing Sign Lattice Dependence) this->sgnL[noAnnot] = sgnL;

        set<NodeDesc> nodes; nodes.insert(NodeDesc(n, state));
        initCG(func, nodes, false, indent);
}

ConstrGraph::ConstrGraph(const Function& func, const DataflowNode& n, const NodeState& state, 
                              LiveDeadVarsAnalysis* ldva, 
                              const map<pair<string, void*>, FiniteVarsExprsProductLattice*>& divL, 
                              // GB : 2011-03-05 (Removing Sign Lattice Dependence)const map<pair<string, void*>, FiniteVarsExprsProductLattice*>& sgnL, 
                              bool initialized, string indent) : func(func), state(state)
{
        this->ldva = ldva;
        // Initialize the map of divisibility and sign lattices
        this->divL = divL;
        // GB : 2011-03-05 (Removing Sign Lattice Dependence)this->sgnL = sgnL; 

        set<NodeDesc> nodes; nodes.insert(NodeDesc(n, state));
        initCG(func, nodes, false, indent);
}

ConstrGraph::ConstrGraph(const Function& func, const set<NodeDesc>& nodes, const NodeState& state, 
                              LiveDeadVarsAnalysis* ldva, 
                              const map<pair<string, void*>, FiniteVarsExprsProductLattice*>& divL, 
                              // GB : 2011-03-05 (Removing Sign Lattice Dependence)const map<pair<string, void*>, FiniteVarsExprsProductLattice*>& sgnL, 
                              bool initialized, string indent) : func(func), state(state)
{
        this->ldva = ldva;
        // Initialize the map of divisibility and sign lattices
        this->divL = divL;
        // GB : 2011-03-05 (Removing Sign Lattice Dependence)this->sgnL = sgnL; 
        
        initCG(func, nodes, false, indent);
}

ConstrGraph::ConstrGraph(ConstrGraph &that, bool initialized, string indent) : func(that.func), state(that.state)
{
        vars = that.vars;
// noDivVars    divVars = that.divVars;
        copyFrom(that, indent+"    ");
        ldva = that.ldva;
        divL = that.divL;
        // GB : 2011-03-05 (Removing Sign Lattice Dependence)sgnL = that.sgnL;
}

ConstrGraph::ConstrGraph(const ConstrGraph* that, bool initialized, string indent) : func(that->func), state(that->state)
{
        vars = that->vars;
// noDivVars    divVars = that->divVars;
        copyFrom(*((ConstrGraph*)that), indent+"    ");
        ldva = that->ldva;
        divL = that->divL;
        // GB : 2011-03-05 (Removing Sign Lattice Dependence)sgnL = that->sgnL;
}

// Creates a constraint graph that contains the given set of inequalities, 
//// which are assumed to correspond to just vars
ConstrGraph::ConstrGraph(const set<varAffineInequality>& ineqs, 
                         const Function& func, const DataflowNode& n, const NodeState& state,
                         LiveDeadVarsAnalysis* ldva, 
                         FiniteVarsExprsProductLattice* divL, 
                         // GB : 2011-03-05 (Removing Sign Lattice Dependence)FiniteVarsExprsProductLattice* sgnL, 
                         string indent) : 
                                func(func), state(state)
{
        this->ldva = ldva;
        // Initialize the map of divisibility and sign lattices, associating divL and sgnL with the 
        // wildcard annotation that matches all variables       
        if(divL) {
                pair<string, void*> noAnnot("", NULL);
                this->divL[noAnnot] = divL;
        }
        // GB : 2011-03-05 (Removing Sign Lattice Dependence)this->sgnL[noAnnot] = sgnL;

        set<NodeDesc> nodes; nodes.insert(NodeDesc(n, state));
        initCG(func, nodes, false, indent);
        
        for(set<varAffineInequality>::const_iterator it = ineqs.begin();
            it!=ineqs.end(); it++)
        {       
                assertCond(*it);
                
                /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(it->getX()); */
                modifiedVars.insert(it->getX());
                /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(it->getY()); */
                modifiedVars.insert(it->getY());
        }
        // Record that we know the constraints represented by this graph and it 
        // is currently a conjunction of constraints
        level = constrKnown;
        constrType = conj;

        transitiveClosure();
}

ConstrGraph::ConstrGraph(const set<varAffineInequality>& ineqs, 
                         const Function& func, const DataflowNode& n, const NodeState& state,
                         LiveDeadVarsAnalysis* ldva,
                         const map<pair<string, void*>, FiniteVarsExprsProductLattice*>& divL, 
                         // GB : 2011-03-05 (Removing Sign Lattice Dependence) const map<pair<string, void*>, FiniteVarsExprsProductLattice*>& sgnL, 
                         string indent) : 
                                  func(func), state(state)
{
        this->ldva = ldva;
        // Initialize the map of divisibility and sign lattices
        this->divL = divL;
        // GB : 2011-03-05 (Removing Sign Lattice Dependence) this->sgnL = sgnL;

        set<NodeDesc> nodes; nodes.insert(NodeDesc(n, state));
        initCG(func, nodes, false, indent);
        
        for(set<varAffineInequality>::const_iterator it = ineqs.begin();
            it!=ineqs.end(); it++)
        {
                assertCond(*it);
                
                /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(it->getX()); */
                modifiedVars.insert(it->getX());
                /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(it->getY()); */
                modifiedVars.insert(it->getY());
        }
        // Record that we know the constraints represented by this graph and it 
        // is currently a conjunction of constraints
        level = constrKnown;
        constrType = conj;
        
        transitiveClosure();
}

// Initialization code that is common to multiple constructors. 
// func - The function that the object corresponds to
// nodes - set of NodeDesc objects, each of which contains
//    n - a Dataflow node this ConstrGraph corresponds to
//    state - the NodeState of node n
//    annotName/annotVal - the annotation that will be associated with all variables live at node n
// initialized - If false, starts this ConstrGraph as uninitialized. If false, starts it at bottom.
void ConstrGraph::initCG(const Function& func, const set<NodeDesc>& nodes, bool initialized, string indent)
{
        // Start this constraint graph as Uninitialized or Bottom, as requested
        if(initialized) level = bottom;
        else            level = uninitialized;
        constrType = unknown;
        
        // Initialize vars to hold all the variables that are live at DataflowNode n
        for(set<NodeDesc>::const_iterator nt=nodes.begin(); nt!=nodes.end(); nt++) {
                // Get the variables live at DataflowNode n
                set<varID> nodeVars;
                getAllLiveVarsAt(ldva, nt->state, nodeVars, indent+"    ");
                
                // Add these variables to vars, using annotation nt->annotName/nt->annotVal if it is provided
                for(set<varID>::iterator nv=nodeVars.begin(); nv!=nodeVars.end(); nv++) {
                        varID v = *nv;
                        if(nt->annotName != "") v.addAnnotation(nt->annotName, nt->annotVal);
                        vars.insert(v);
                }
                
                // Also add any variables in nt->varsToInclude, using annotation nt->annotName/nt->annotVal if it is provided
                for(set<varID>::iterator nv=nt->varsToInclude.begin(); nv!=nt->varsToInclude.end(); nv++) {
                        varID v = *nv;
                        if(nt->annotName != "") v.addAnnotation(nt->annotName, nt->annotVal);
                        vars.insert(v);
                }
        }
        
        // Add the zeroVar constant to the set of variables
        vars.insert(zeroVar);
        
        // Initially, all variables are "modified"
        constrChanged=true;
        for(varIDSet::iterator var = this->vars.begin(); var != this->vars.end(); var++)
        {
                /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(*var); */
                modifiedVars.insert(*var);
        }
        
        // Look over all variables and if there exists divisibility information for a given variable, 
        // add its divisibility variable
// !!! Not using divVars for now        for(varIDSet::iterator var = this->vars.begin(); var != this->vars.end(); var++) {
// !!! Not using divVars for now                FiniteVarsExprsProductLattice* divLattice = getDivLattice(*var, indent+"    ");
// !!! Not using divVars for now                if(divLattice) {
// !!! Not using divVars for now                        DivLattice* varDivL = dynamic_cast<DivLattice*>(divLattice->getVarLattice(*var));
// !!! Not using divVars for now                        if(varDivL && (/*varDivL->getLevel() == DivLattice::valKnown || */varDivL->getLevel() == DivLattice::divKnown))
// !!! Not using divVars for now                                divVars.insert(getDivVar(*var));
// !!! Not using divVars for now                }
// !!! Not using divVars for now        }
        
        // Initially we're not inside of a transaction
        inTransaction=false;    
}

ConstrGraph::~ConstrGraph ()
{
        vars.clear();
// noDivVars    divVars.clear();
        eraseConstraints(true, "");
        
        //if(CGDebugLevel>=1) Dbg::dbg << "Deleting ConstrGraph "<<this<<"\n";
}

// Initializes this Lattice to its default state, if it is not already initialized
void ConstrGraph::initialize(string indent)
{
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        if(l.first == uninitialized) {
                if(vars.size()==0) setToBottom(indent+"    ");
                else setToConstrKnown(conj, false, indent+"    ");
        }
}

// For a given variable returns the corresponding divisibility variable
// noDivVars varID ConstrGraph::getDivVar(const varID& scalar)
// noDivVars {
// noDivVars    varID divScalar("DV_"+scalar.str());
// noDivVars    return divScalar;
// noDivVars }

// Returns true if the given variable is a divisibility variable and false otherwise
// noDivVars bool ConstrGraph::isDivVar(const varID& var)
// noDivVars {
// noDivVars    // its a divisibility var if its name begins with "DV_"
// noDivVars    return var.str().find("DV_", 0)==0;
// noDivVars }

// Returns a divisibility product lattice that matches the given variable
FiniteVarsExprsProductLattice* ConstrGraph::getDivLattice(const varID& var, string indent)
{
        for(map<pair<string, void*>, FiniteVarsExprsProductLattice*>::iterator itDiv=divL.begin();
                    itDiv!=divL.end(); itDiv++)
        {
                // If the current annotation matches all annotations
                if(itDiv->first.first=="" && itDiv->first.second==NULL)
                        return itDiv->second;
                
                // Else, if we have different divisibility lattices for different variable annotations, 
                // find the lattice with the correct annotation
                for(map<string, void*>::const_iterator itVar=var.getAnnotations().begin();
                    itVar!=var.getAnnotations().end(); itVar++)
                {
                        if((itDiv->first.first=="" || itDiv->first.first==itVar->first) &&
                           (itDiv->first.second==NULL || itDiv->first.second==itVar->second))
                                return itDiv->second;
                }
        }
        return NULL;
}

string ConstrGraph::DivLattices2Str(string indent)
{
        ostringstream oss;
        
        bool firstLine=true;
        Dbg::dbg << "    divL="<<divL.size()<<"\n";
        for(map<pair<string, void*>, FiniteVarsExprsProductLattice*>::iterator itDiv=divL.begin();
                    itDiv!=divL.end(); itDiv++)
        {
                if(!firstLine) { Dbg::dbg << indent; }
                oss << "Annotation \""<<itDiv->first.first<<"\"->"<<itDiv->first.second<<"\n";
                oss << indent << "    "<<itDiv->second->str(indent+"    ")<<"\n";
                firstLine = false;
        }
        
        return oss.str();
}

// Returns a sign product lattice that matches the given variable
// GB : 2011-03-05 (Removing Sign Lattice Dependence) 
/*FiniteVarsExprsProductLattice* ConstrGraph::getSgnLattice(const varID& var, string indent)
{
        for(map<pair<string, void*>, FiniteVarsExprsProductLattice*>::iterator itSgn=sgnL.begin();
                 itSgn!=sgnL.end(); itSgn++)
        {
                // If the current annotation matches all annotations
                if(itSgn->first.first=="" && itSgn->first.second==NULL)
                        return itSgn->second;
                
                // Else, if we have different divisibility lattices for different variable annotations, 
                // find the lattice with the correct annotation
                for(map<string, void*>::const_iterator itVar=var.getAnnotations().begin();
                    itVar!=var.getAnnotations().end(); itVar++)
                {
                        if((itSgn->first.first=="" || itSgn->first.first==itVar->first) &&
                           (itSgn->first.second==NULL || itSgn->first.second==itVar->second))
                                return itSgn->second;
                }
        }
        return NULL;
}*/

// Adds the given variable to the variables list, returning true if this causes
// the constraint graph to change and false otherwise.
bool ConstrGraph::addVar(const varID& var, string indent)
{
        pair<varIDSet::iterator,bool> loc = vars.insert(var);
        return loc.second;
}

// Removes the given variable and its divisibility variables (if one exists) from the variables list
// and removes any constraints that involve them. 
// Returning true if this causes the constraint graph to change and false otherwise.
bool ConstrGraph::removeVar(const varID& var, string indent)
{
        bool modified=false;
        
        // noDivVars varID divVar = getDivVar(var);
        
        // Remove the constraints
        // noDivVars modified = eraseVarConstr(divVar, true, indent+"    ") || modified;
        modified = eraseVarConstr(var, true, indent+"    ") || modified;
        
        // Remove the variables from divVars and vars
        // noDivVars modified = (divVars.erase(divVar) > 0) || modified;
        modified = (vars.erase(var) > 0) || modified;
        
        return modified;
}

// Returns a reference to the constraint graph's set of variables
const varIDSet& ConstrGraph::getVars() const
{
        return vars;
}

// Returns a modifiable reference to the constraint graph's set of variables
varIDSet& ConstrGraph::getVarsMod()
{
        return vars;
}

/***** Copying *****/

// Overwrites the state of this Lattice with that of that Lattice
void ConstrGraph::copy(Lattice* that)
{
        copyFrom(*(dynamic_cast<ConstrGraph*>(that)), "");
}

// Returns a copy of this lattice
Lattice* ConstrGraph::copy() const
{
        return new ConstrGraph(this);
}

// Returns a copy of this LogicalCond object
/*LogicalCond* ConstrGraph::copy()
{
        return new ConstrGraph(this);
}*/

// Copies the state of that to this constraint graph
// Returns true if this causes this constraint graph's state to change
bool ConstrGraph::copyFrom(ConstrGraph &that, string indent)
{
        bool modified = (level != that.level);
        level = that.level;
        
        modified = (constrType != that.constrType) || modified;
        constrType = that.constrType;

        /*if(CGDebugLevel>=1)
        {
                if(that.vars != vars)
                {
                        Dbg::dbg << indent << "!!!!!ConstrGraph::copyFrom() Different vars:\n";
                        Dbg::dbg << indent << "    vars=";
                        for(set<varID>::iterator it=vars.begin(); it!=vars.end(); it++)
                        { Dbg::dbg << (*it).str() << " "; }
                        Dbg::dbg << "\n";
                        
                        Dbg::dbg << indent << "    that.vars=";
                        for(set<varID>::iterator it=that.vars.begin(); it!=that.vars.end(); it++)
                        { Dbg::dbg << (*it).str() << " "; }
                        Dbg::dbg << "\n";
                }
                Dbg::dbg .flush();
        }*/
        
        // Ensure that both constraint graphs map the same set of variables
        //ROSE_ASSERT(vars == that.vars);
        //ROSE_ASSERT(divVars == that.divVars);
        
        // Copy the constraint information from cg to this              
        modified = copyConstraints(that, indent+"    ") || modified;

        // Copy over lattice information from that      
        // No, we must keep state information that belongs to this ConstraintGraph's original DataflowNode
        //ldva = that.ldva;
        //divL = that.divL;
        //sgnL = that.sgnL;
        
        // Copy over transaction status from that.
        inTransaction = that.inTransaction;
        
        return  modified;
}

// Copies the state of That into This constraint graph, but mapping constraints of varFrom to varTo, even
//    if varFrom is not mapped by This and is only mapped by That. varTo must be mapped by This.
// Returns true if this causes this constraint graph's state to change.
bool ConstrGraph::copyFromReplace(ConstrGraph &that, varID varTo, varID varFrom, string indent)
{
        bool modified = (level != that.level);
        level = that.level;
        
        modified = (constrType != that.constrType) || modified;
        constrType = that.constrType;

        // Copy the constraint information from cg to this              
        modified = copyConstraintsReplace(that, varFrom, varTo, indent+"    ") || modified;

        // Copy over transaction status from that.
        inTransaction = that.inTransaction;
        
        return  modified;
}

// Copies the given var and its associated constrants from that to this.
// Returns true if this causes this constraint graph's state to change; false otherwise.
bool ConstrGraph::copyVar(const ConstrGraph& that, const varID& var)
{
        bool modified = false;

        // If we're already top or inconsistent, do nothing
        if((level == top) ||
           (level == constrKnown && constrType == inconsistent))
                return false;
        
        // It is only worthwhile to copy a variable if that object contains real constraints
        if((level != constrKnown) || (that.level == constrKnown && that.constrType == inconsistent))
                return false;
        
        // If we were uninitialized, upgrade to being initialized, with known constraints
        if(level == uninitialized) { level = constrKnown; modified = true; }
        
        ROSE_ASSERT(level==constrKnown && constrType!=inconsistent);
        ROSE_ASSERT(that.level==constrKnown && that.constrType!=inconsistent);
        
        //printf("ConstrGraph::copyVar(var=%s)\n", var.str().c_str());
        // Add var to vars if it isn't already there.
        if(vars.find(var) == vars.end()) {
                modified = true;
                vars.insert(var);
        }
        
        // True if either both constraint graphs are negated or not negated (if a graph is negated, all of 
        // its inequalities are flipped from ax<=by+x to by<ax-c)
        bool sameNeg = (constrType==that.constrType);
        // Only implementing the case where the negation is the same since it is unclear what the semantics should be if 
        ROSE_ASSERT(sameNeg);
        
        // Iterate over all the var<=x and      x<=var pairs.
        for(map<varID, map<varID, affineInequality> >::const_iterator itX = that.vars2Value.begin();
            itX!=that.vars2Value.end(); itX++)
        {
                //Dbg::dbg << "    copyVar itX->first="<<itX->first.str()<<"\n";
                // var <= constraints
                if(itX->first == var)
                {
                        // Assert in this all the var <= x constraints in that
                        for(map<varID, affineInequality>::const_iterator itY = itX->second.begin();
                            itY != itX->second.end(); itY++)
                        {
                                //Dbg::dbg << "ConstrGraph::copyVar: calling assertCond1("<<itX->first.str()<<", "<<var.str()<<", "<<Dbg::escape(itY->second.str())<<")\n";
                                assertCond(var, itY->first, itY->second);
                        }
                }
                // x <= var constraints
                else
                {
                        // Assert in this all the x <= var constraints in that
                        for(map<varID, affineInequality>::const_iterator itY = itX->second.begin();
                            itY!=itX->second.end(); itY++)
                        {
                                if(itY->first == var)
                                {
                                        //Dbg::dbg << "ConstrGraph::copyVar: calling assertCond2("<<itX->first.str()<<", "<<var.str()<<", "<<Dbg::escape(itY->second.str())<<")\n";
                                        assertCond(itX->first, var, itY->second);
                                }
                        }
                }
        }

        return modified;
}

// Determines whether constraints in that are different from
// the constraints in this
bool ConstrGraph::diffConstraints(ConstrGraph &that, string indent)
{
        //Check to see if map::operator== is sufficient for this
        // if these two constraint graphs differ on their bottom-ness
        if(getLevel() != that.getLevel()) return true;
        
        // If these two constraint graphs have different constraints or 
        // map different sets of variables
        if(vars2Value != that.vars2Value ||
                vars       != that.vars/* ||
 noDivVars              divVars    != that.divVars*/)
                return true;
        // !!! NOTE: this has a bug in that a bottom constraint may be represented by either not having a constraint/
        // !!! or a constraint that has lattice level bottom
        
        return false;
}

// Copies the constraints of cg into this constraint graph.
// Returns true if this causes this constraint graph's state to change.
bool ConstrGraph::copyConstraints(ConstrGraph &that, string indent)
{
        bool modified;
        
        // this constraint graph will be modified if it is currently uninitialized
        modified = (level == uninitialized);
        
        // !!! THIS IS NOT QUITE RIGHT SINCE WE'RE NOT COPYING ALL CONSTRAINTS FROM THAT
        modified = modified || diffConstraints(that);
        
        /*Dbg::dbg << indent << "copyConstraints()\n";
        Dbg::dbg << indent << "    vars=\n";
        for(set<varID>::iterator v=vars.begin(); v!=vars.end(); v++)
                Dbg::dbg << indent << "        "<<*v<<"\n";
        Dbg::dbg << indent << "    divVars=\n";
        for(set<varID>::iterator v=divVars.begin(); v!=divVars.end(); v++)
                Dbg::dbg << indent << "        "<<*v<<"\n";
        Dbg::dbg << indent << "    that.vars=\n";
        for(set<varID>::iterator v=that.vars.begin(); v!=that.vars.end(); v++)
                Dbg::dbg << indent << "        "<<*v<<"\n";
                Dbg::dbg << indent << "That="<<that.str(indent+"    ")<<"\n";*/
        
        // Erase the current state of vars2Value
        map<varID, map<varID, affineInequality> >::iterator itX;
        for(itX = vars2Value.begin(); itX!=vars2Value.end(); itX++)
                itX->second.clear();
        vars2Value.clear();
        
        // Copy the portions of that.vars2Value that mention variables in vars and divVars or the 
//!!!   // divisibility variables of the variables in vars that are not in divVars 
        for(map<varID, map<varID, affineInequality> >::iterator iterX=that.vars2Value.begin(); iterX!=that.vars2Value.end(); iterX++) {
                //pair<varID, bool> v = divVar2Var(iterX->first, indent+"    ");
                if(vars.find(iterX->first)==vars.end() /* noDivVars && divVars.find(iterX->first)==divVars.end()*/) { /*Dbg::dbg << indent << "    skipping x="<<iterX->first<<"\n"; */continue; }
                
                for(map<varID, affineInequality>::iterator iterY=iterX->second.begin(); iterY!=iterX->second.end(); iterY++) {
                        if(vars.find(iterY->first)==vars.end()  /* noDivVars && divVars.find(iterY->first)==divVars.end()*/) { /*Dbg::dbg << indent << "    skipping x="<<iterX->first<<" y="<<iterY->first<<"\n"; */continue; }
                        //Dbg::dbg << indent << "    copying x="<<iterX->first<<" y="<<iterY->first<<" constraint="<<Dbg::escape(that.vars2Value[iterX->first][iterY->first].str())<<"\n";
                        vars2Value[iterX->first][iterY->first] = that.vars2Value[iterX->first][iterY->first];
                }
        }
        
        //Dbg::dbg << indent << "Final state="<<str(indent+"    ")<<"\n";
        
        // Copy the modification information from that since the state of this ConstrGraph
        // object is a direct copy of that
        modifiedVars  = that.modifiedVars;
        /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars = that.newConstrVars; */
        constrChanged = that.constrChanged;
        
        return modified;
}

// Copies the constraints of That into This constraint graph, but mapping constraints of varFrom to varTo, even
//    if varFrom is not mapped by This and is only mapped by That. varTo must be mapped by This.
// Returns true if this causes this constraint graph's state to change.
bool ConstrGraph::copyConstraintsReplace(ConstrGraph &that, varID varTo, varID varFrom, string indent)
{
        bool modified;
        
        Dbg::dbg << indent << "copyConstraintsReplace(varFrom="<<varFrom<<", varTo="<<varTo<<"\n";
        ROSE_ASSERT(vars.find(varTo)!=vars.end() /* noDivVars  || divVars.find(varTo)!=divVars.end()*/);
        
        // This constraint graph will be modified if it is currently uninitialized
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        modified = (l.first==uninitialized);
        // This constraint graph will now definitely be initialized
        if(l.first==uninitialized) return setToBottom(indent+"    ");
        
        // !!! THIS IS NOT QUITE RIGHT SINCE WE'RE NOT COPYING ALL CONSTRAINTS FROM THAT
        modified = modified || diffConstraints(that);
        
        // Erase the current state of vars2Value
        map<varID, map<varID, affineInequality> >::iterator itX;
        for(itX = vars2Value.begin(); itX!=vars2Value.end(); itX++)
                itX->second.clear();
        vars2Value.clear();
        
        // Copy the portions of that.vars2Value that mention variables in vars and divVars
        for(map<varID, map<varID, affineInequality> >::iterator iterX=that.vars2Value.begin(); iterX!=that.vars2Value.end(); iterX++) {
                if(iterX->first!=varFrom && vars.find(iterX->first)==vars.end()/* noDivVars && divVars.find(iterX->first)==divVars.end()*/) { Dbg::dbg << indent << "    skipping x="<<iterX->first<<"\n"; continue; }
                
                for(map<varID, affineInequality>::iterator iterY=iterX->second.begin(); iterY!=iterX->second.end(); iterY++) {
                        if(iterX->first!=varTo && vars.find(iterY->first)==vars.end()/* noDivVars && divVars.find(iterY->first)==divVars.end()*/) { Dbg::dbg << indent << "    skipping y="<<iterY->first<<"\n"; continue; }
                        Dbg::dbg << indent << "    copying x="<<iterX->first<<" y="<<iterY->first<<" constraint="<<Dbg::escape(that.vars2Value[iterX->first][iterY->first].str())<<"\n";
                        if(iterX->first==varFrom)
                                vars2Value[varTo][iterY->first] = that.vars2Value[varFrom][iterY->first];
                        else if(iterY->first==varFrom)
                                vars2Value[iterX->first][varTo] = that.vars2Value[iterX->first][varFrom];
                        else
                                vars2Value[iterX->first][iterY->first] = that.vars2Value[iterX->first][iterY->first];
                }
        }
        
        // Copy the modification information from that since the state of this ConstrGraph
        // object is a direct copy of that
        modifiedVars  = that.modifiedVars;
        /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars = that.newConstrVars; */
        constrChanged = that.constrChanged;
        
        return modified;
}

// updates the constraints of cg into this constraint graph, leaving 
//    this graph's original constraints alone and only modifying the constraints that are in that.
// returns true if this causes this constraint graph's state to change
/*bool ConstrGraph::updateConstraints(ConstrGraph &that)
{
        bool modified;
        // we don't do copies from ConstrGraph from different functions
        ROSE_ASSERT(that.arrays == arrays);
        ROSE_ASSERT(that.vars == vars);
        
        // this constraint graph will be modified if it is currently uninitialized
        modified = !initialized;
        
        modified = modified || diffConstraints(that);
        
        //vars2Value = that.vars2Value;
        // erase vars2Value
        map<varID, map<varID, affineInequality> >::iterator itX;
        for(itX = vars2Value.begin(); itX!=vars2Value.end(); itX++)
                itX->second.clear();
        vars2Value.clear();
        
        // copy vars2Value from that
        vars2Value = that.vars2Value;
        
        initialized = true; // this constraint graph is now definitely initialized
        
        return modified;
}*/

/**** Erasing ****/
// erases all constraints from this constraint graph
// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
//                                                      or to not bother checking self-consistency and just return the last-known value (=true)
void ConstrGraph::eraseConstraints(bool noConsistencyCheck, string indent)
{
//              std::Dbg::dbg << indent << "eraseConstraints() checkSelfConsistency()="<<checkSelfConsistency()<<"\n";
        // If this graph has constraints to be considered
        if(hasConsistentConstraints(noConsistencyCheck, indent))
        {
                for(map<varID, map<varID, affineInequality> >::iterator it = vars2Value.begin(); it!=vars2Value.end(); it++)
                        it->second.clear();
                vars2Value.clear();
                modifiedVars.clear();
                /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.clear(); */
                
                constrChanged = true;   
        }
}

// Erases all constraints that relate to variable eraseVar and its corresponding divisibility variable 
// from this constraint graph
// Returns true if this causes the constraint graph to change and false otherwise
// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
//                                                      or to not bother checking self-consistency and just return the last-known value (=true)
bool ConstrGraph::eraseVarConstr(const varID& eraseVar, bool noConsistencyCheck, string indent)
{
        bool modified = false;
        // noDivVars varID eraseDivVar = getDivVar(eraseVar);
        
        // If this graph has constraints to be considered
        if(hasConsistentConstraints(noConsistencyCheck, indent))
        {
                // First erase all mappings from eraseVar to other variables
                if(vars2Value.find(eraseVar) != vars2Value.end()/* || 
                         noDivVars vars2Value.find(eraseDivVar) != vars2Value.end()*/) {
                        vars2Value[eraseVar].clear();
                        // noDivVars vars2Value[eraseDivVar].clear();
                        modifiedVars.insert(eraseVar);
                        // noDivVars modifiedVars.insert(eraseDivVar);
                        modified = true;
                }
                                                                
                // Iterate over all variable mappings, erasing links from other variables to eraseVar
                for(map<varID, map<varID, affineInequality> >::iterator curVar = vars2Value.begin(); 
                    curVar != vars2Value.end(); curVar++ )
                {
                        modified = curVar->second.find(eraseVar) != curVar->second.end() || modified;
                        // noDivVars modified = curVar->second.find(eraseDivVar) != curVar->second.end() || modified;
                        
                        (curVar->second).erase(eraseVar);
                        // noDivVars (curVar->second).erase(eraseDivVar);
                        modifiedVars.insert(curVar->first);
                }
        }
        
        constrChanged = constrChanged || modified;
        
        return modified;
}

// Erases all constraints that relate to variable eraseVar but not its divisibility variable from 
//    this constraint graph
// Returns true if this causes the constraint graph to change and false otherwise
// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
//                                                      or to not bother checking self-consistency and just return the last-known value (=true)
bool ConstrGraph::eraseVarConstrNoDiv(const varID& eraseVar, bool noConsistencyCheck, string indent)
{
        //printf("eraseVarConstrNoDiv eraseVar=%s\n", eraseVar.str().c_str());
        bool modified = false;
        
        // If this graph has constraints to be considered
        if(hasConsistentConstraints(noConsistencyCheck, indent))
        {
                modified = modified || vars2Value[eraseVar].size()>0;
                
                // First erase all mappings from eraseVar to other variables
                vars2Value[eraseVar].clear();
                modifiedVars.insert(eraseVar);
                                                
                // Iterate over all variable mappings, erasing links from other variables to eraseVar
                for(map<varID, map<varID, affineInequality> >::iterator curVar = vars2Value.begin(); 
                    curVar != vars2Value.end(); curVar++ )
                {
                        modified = curVar->second.find(eraseVar) != curVar->second.end() || modified;
                        (curVar->second).erase(eraseVar);
                        modifiedVars.insert(curVar->first);
                }
        }
        
        constrChanged = constrChanged || modified;
        
        return modified;
}

// Erases all constraints between eraseVar and vars in this constraint graph but leave the constraints 
//    that relate to its divisibility variable alone
// Returns true if this causes the constraint graph to change and false otherwise
// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
//                                                      or to not bother checking self-consistency and just return the last-known value (=true)
bool ConstrGraph::eraseVarConstrNoDivVars(const varID& eraseVar, bool noConsistencyCheck, string indent)
{
        bool modified = false;
        
        // If this graph has constraints to be considered
        if(hasConsistentConstraints(noConsistencyCheck, indent))
        {
                modified = modified || vars2Value[eraseVar].size()>0;
                
                // remove all the eraseVar->scalar connections
                for(varIDSet::iterator it=vars.begin(); it!=vars.end(); it++)
                {
                        modified = vars2Value[eraseVar].erase(*it) > 0 || modified;
                }
                modifiedVars.insert(eraseVar);
                                                
                // iterate over all variable mappings, erasing links from vars to eraseVar
                for(map<varID, map<varID, affineInequality> >::iterator curVar = vars2Value.begin(); 
                    curVar != vars2Value.end(); curVar++)
                {
                        // if this is a scalar
                        if(vars.find(curVar->first) != vars.end())
                        {
                                modified = (curVar->second).erase(eraseVar) > 0 || modified;
                                modifiedVars.insert(curVar->first);
                        }
                }
        }
        
        constrChanged = constrChanged || modified;
        
        return modified;
}

// Removes any constraints between the given pair of variables
// Returns true if this causes the constraint graph to change and false otherwise
/*bool ConstrGraph::disconnectVars(const varID& x, const varID& y)
{
        bool modified = false;
        // if this constraint graph is already bottom, don't bother
        if(!isBottom())
        {
                if(vars2Value[x].find(y)!=vars2Value[y].end())
                        vars2Value[x].erase(y);
                if(vars2Value[y].find(x)!=vars2Value[y].end())
                        vars2Value[y].erase(x);
                modifiedVars.insert(x);
                modifiedVars.insert(y);
        }
        
        constrChanged = constrChanged || modified;
        
        return modified;        
}*/

// Replaces all instances of origVar with newVar. Both are assumed to be scalars.
// Returns true if this causes the constraint graph to change and false otherwise
// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
//                                                      or to not bother checking self-consistency and just return the last-known value (=true)
bool ConstrGraph::replaceVar(const varID& origVar, const varID& newVar, bool noConsistencyCheck, string indent)
{
        bool modified = false;
        // noDivVars varID origDivVar = getDivVar(origVar);
        // noDivVars varID newDivVar = getDivVar(newVar);
        
        // If this graph has constraints to be considered
        if(hasConsistentConstraints(noConsistencyCheck, indent))
        {
                //Dbg::dbg << "replaceVar("<<origVar.str()<<", "<<newVar.str()<<");\n";
                modified = modified || vars.find(origVar)!=vars.end();//vars2Value[origVar].size()>0 || vars2Value[origDivVar].size()>0;

                // First erase the origVar and then re-add it as a scalar with no constraints
                modified = eraseVarConstr(newVar, noConsistencyCheck, "") || modified;
                modified = addVar(newVar, "") || modified;

/*              Dbg::dbg << "variables vars2Value ===\n";
                for(map<varID, map<varID, affineInequality> >::iterator it=vars2Value.begin(); it!=vars2Value.end(); it++)
                { Dbg::dbg << "replaceVar: "<<it->first.str()<<", ==origVar = "<<(it->first == origVar)<<", vars2Value[var].size()="<<vars2Value[it->first].size()<<", vars2Value[origVar].size()="<<vars2Value[origVar].size()<<"==newVar = "<<(it->first == newVar)<<"\n"; }
                
                Dbg::dbg << "variables vars ===\n";
                for(varIDSet::iterator it=vars.begin(); it!=vars.end(); it++)
                { Dbg::dbg << "replaceVar: "<<(*it).str()<<", ==origVar = "<<((*it) == origVar)<<", vars2Value[var].size()="<<vars2Value[(*it)].size()<<", vars2Value[origVar].size()="<<vars2Value[origVar].size()<<"==newVar = "<<((*it) == newVar)<<"\n"; }          
                
                Dbg::dbg << "replaceVar: origVar ==== vars2Value["<<origVar.str()<<"].size()="<<vars2Value[origVar].size()<<"\n";
                for(map<varID, affineInequality>::iterator it=vars2Value[origVar].begin(); it!=vars2Value[origVar].end(); it++)
                { Dbg::dbg << "replaceVar: "<<origVar.str()<<" -> "<<it->first.str()<<" = "<<Dbg::escape(it->second.str())<<"\n"; }
                */
                
                // Copy over all the mappings origVar <= ... to be newVar <= ... and delete the origVar <= ... mappings
                vars2Value[newVar]=vars2Value[origVar];
                /*Dbg::dbg << "replaceVar: Before ====\n";
                for(map<varID, affineInequality>::iterator it=vars2Value[newVar].begin(); it!=vars2Value[newVar].end(); it++)
                { Dbg::dbg << "replaceVar: "<<newVar.str()<<" -> "<<it->first.str()<<" = "<<Dbg::escape(it->second.str())<<"\n"; }*/
                vars2Value[origVar].clear();
                // noDivVars vars2Value[newDivVar]=vars2Value[origDivVar];
                // noDivVars vars2Value[origDivVar].clear();
                
                /*Dbg::dbg << "replaceVar: After ====\n";
                for(map<varID, affineInequality>::iterator it=vars2Value[newVar].begin(); it!=vars2Value[newVar].end(); it++)
                { Dbg::dbg << "replaceVar: "<<newVar.str()<<" -> "<<it->first.str()<<" = "<<it->second.str()<<"\n"; }*/
                
                // Copy over all the mappings ... <= origVar to be ... <= newVar and delete the ... <= origVar mappings
                // by iterating over all variable mappings, copying links from other variables to origVar
                // to make them into links to newVar
                for(map<varID, map<varID, affineInequality> >::iterator curVar = vars2Value.begin(); 
                    curVar != vars2Value.end(); curVar++ )
                {
                        if(curVar->second.find(origVar) != curVar->second.end())
                        {
                                modified = true;
                                (curVar->second)[newVar] = (curVar->second)[origVar];
                                (curVar->second).erase(origVar);
                        }
                        
                        // noDivVars if(curVar->second.find(origDivVar) != curVar->second.end())
                        // noDivVars {
                        // noDivVars    modified = true;
                        // noDivVars    (curVar->second)[newDivVar] = (curVar->second)[origDivVar];
                        // noDivVars    (curVar->second).erase(origDivVar);
                        // noDivVars }
                }
                
                // We don't modify modifiedVars or newConstrVars because the new variable has constraints
                // identical to the old, meaning that this operation has no affect on the transitive closure
        }
        
        constrChanged = constrChanged || modified;
        
        return modified;
}
\
// Used by copyAnnotVars() and mergeAnnotVars() to identify variables that are interesting
// from their perspective.
bool ConstrGraph::annotInterestingVar(const varID& var, const set<pair<string, void*> >& noCopyAnnots, const set<varID>& noCopyVars,
                                      const string& annotName, void* annotVal, string indent)
{
        return !varHasAnnot(var, noCopyAnnots) && noCopyVars.find(var)==noCopyVars.end() && 
                    varHasAnnot(var, annotName, annotVal);
}


// Copies the constraints on all the variables that have the given annotation (srcAnnotName -> srcAnnotVal).
// For each such variable we create a copy variable that is identical except that the
//    (srcAnnotName -> srcAnnotVal) annotation is replaced with the (tgtAnnotName -> tgtAnnotVal) annotation.
// If two variables match the (srcAnnotName -> srcAnnotVal) annotation and the constraint graph has a relation
//    between them, their copies will have the same relation between each other but will have no relation to the
//    original variables. If two variables have a relation and only one is copied, then the copy maintains the 
//    original relation to the non-copied variable.
// A variable matches the given (srcAnnotName -> srcAnnotVal) annotation if this is one of the variable's annotations
//    or if srcAnnotName=="" and the variable has no annotations.
// Avoids copying variables with annotations in the noCopyAnnots set and variables in the noCopyVars set.
// Returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::copyAnnotVars(string srcAnnotName, void* srcAnnotVal, 
                                string tgtAnnotName, void* tgtAnnotVal,
                                const set<pair<string, void*> >& noCopyAnnots,
                                     const set<varID>& noCopyVars, string indent)
{
        bool modified = false;
        map<varID, map<varID, affineInequality> > xCopyAdditions;
        
        for(map<varID, map<varID, affineInequality> >::iterator itX=vars2Value.begin();
            itX!=vars2Value.end(); itX++)
        {
                const varID& x = itX->first;
                //Dbg::dbg << "x="<<x.str()<<"\n";
                
                // If x is a matching variable, copy its constraints
                if(!varHasAnnot(x, noCopyAnnots) && noCopyVars.find(x)==noCopyVars.end() && 
                   ((srcAnnotName=="" && x.numAnnotations()==0) || 
                   (x.hasAnnotation(srcAnnotName) && x.getAnnotation(srcAnnotName)==srcAnnotVal)))
                {
                        //Dbg::dbg << "    match\n";
                        // Create the copy variable, which is identical to x, except with replaced annotations
                        varID xCopy(x);
                        xCopy.remAnnotation(srcAnnotName);
                        xCopy.addAnnotation(tgtAnnotName, tgtAnnotVal);
                        
                        xCopyAdditions[xCopy] = itX->second;
                        modifiedVars.insert(xCopy);
                        /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(xCopy); */
                        
                        // Update xCopy's map to perform copies on the variables inside the map
                        map<pair<varID, varID>, affineInequality> yCopyChanges;
                        for(map<varID, affineInequality>::iterator itY=xCopyAdditions[xCopy].begin(); itY!=xCopyAdditions[xCopy].end(); itY++)
                        {
                                const varID& y = itY->first;
                                //Dbg::dbg << "    y="<<y.str()<<"\n";
                                
                                // If y is also matching variable, copy the x->y constraint to apply to xCopy->yCopy
                                if(!varHasAnnot(y, noCopyAnnots) && noCopyVars.find(y)==noCopyVars.end() && 
                         ((srcAnnotName=="" && y.numAnnotations()==0) || 
                         (y.hasAnnotation(srcAnnotName) && y.getAnnotation(srcAnnotName)==srcAnnotVal)))
                                {
                                        //Dbg::dbg << "        match\n";
                                        // Create the copy variable, which is identical to y, except with replaced annotations
                                        varID yCopy(y);
                                        yCopy.remAnnotation(srcAnnotName);
                                        yCopy.addAnnotation(tgtAnnotName, tgtAnnotVal);
                                        
                                        pair<varID, varID> oldnewVars(y, yCopy);
                                        yCopyChanges[oldnewVars] = itY->second;
                                        //itY->first.remAnnotation(srcAnnotName);
                                        //itY->first.addAnnotation(tgtAnnotName, tgtAnnotVal);
                                }
                        }
                        
                        // Perform the changes
                        for(map<pair<varID, varID>, affineInequality>::iterator it=yCopyChanges.begin();
                            it!=yCopyChanges.end(); it++)
                        {
                                //Dbg::dbg << "Erasing "<<xCopy.str()<<" -> "<<it->first.first.str()<<"\n";
                                (xCopyAdditions[xCopy]).erase(it->first.first);
                                (xCopyAdditions[xCopy])[it->first.second] = it->second;
                        }
                }
                // If x is NOT a matching variable, process its relations to other variables
                else
                {
                        //Dbg::dbg << "    no match\n";
                        map<varID, affineInequality> yCopyAdditions;
                        for(map<varID, affineInequality>::iterator itY=itX->second.begin(); itY!=itX->second.end(); itY++)
                        {
                                const varID& y = itY->first;
                                //Dbg::dbg << "y="<<y.str()<<"\n";
                                
                                // If y is also matching variable, create a new x->CopyY constraints, which is a copy of the x->y constraint
                                if(!varHasAnnot(y, noCopyAnnots) && noCopyVars.find(y)==noCopyVars.end() && 
                         ((srcAnnotName=="" && y.numAnnotations()==0) || 
                         (y.hasAnnotation(srcAnnotName) && y.getAnnotation(srcAnnotName)==srcAnnotVal)))
                                {
                                        //Dbg::dbg << "    match\n";
                                        // Create the copy variable, which is identical to y, except with replaced annotations
                                        varID yCopy(y);
                                        yCopy.remAnnotation(srcAnnotName);
                                        yCopy.addAnnotation(tgtAnnotName, tgtAnnotVal);
                                        
                                        yCopyAdditions[yCopy] = itY->second;
                                }
                        }
                        
                        // If x has constraints that will be copied
                        if(yCopyAdditions.begin() != yCopyAdditions.end())
                        {
                                modifiedVars.insert(x);
                                /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(x); */
                        }
                        
                        // Insert the newly-copied constraints back into itX->second
                        for(map<varID, affineInequality>::iterator it=yCopyAdditions.begin(); it!=yCopyAdditions.end(); it++)
                                itX->second[it->first] = it->second;
                }
        }
        
        // Insert the newly-copied constraints back into vars2Value
        for(map<varID, map<varID, affineInequality> >::iterator it=xCopyAdditions.begin();
            it!=xCopyAdditions.end(); it++) {
                if(vars2Value[it->first] != it->second) modified = true;
                vars2Value[it->first] = it->second;
        }
        
        // ------------------------------------
        // Add copy scalar variables to vars
        varIDSet copyVars;
        for(varIDSet::iterator it=vars.begin(); it!=vars.end(); it++)
        {
                const varID& var = *it;
                
                //Dbg::dbg << "copyAnnotVars: var = "<<var.str()<<"\n";
                if(!varHasAnnot(var, noCopyAnnots) && noCopyVars.find(var)==noCopyVars.end() && 
                   ((srcAnnotName=="" && var.numAnnotations()==0) || 
                   (var.hasAnnotation(srcAnnotName) && var.getAnnotation(srcAnnotName)==srcAnnotVal)))
                {
                        // Create the copy variable, which is identical to var, except with replaced annotations
                        varID varCopy(var);
                        varCopy.remAnnotation(srcAnnotName);
                        varCopy.addAnnotation(tgtAnnotName, tgtAnnotVal);
                        //Dbg::dbg << "      varCopy = "<<varCopy.str()<<"\n";
                        
                        // Record the copy
                        copyVars.insert(varCopy);
                }
        }
        
        for(varIDSet::iterator it=copyVars.begin(); it!=copyVars.end(); it++) {
                if(vars.find(*it) == vars.end()) modified = true;
                vars.insert(*it);
        }
        
        // ----------------------------------
        // Add copy divVars variables to divVars
// noDivVars    varIDSet copydivVars;
// noDivVars    for(varIDSet::iterator it=divVars.begin(); it!=divVars.end(); it++)
// noDivVars    {
// noDivVars            const varID& var = *it;
// noDivVars            
// noDivVars            if(!varHasAnnot(var, noCopyAnnots) && noCopyVars.find(var)==noCopyVars.end() && 
// noDivVars               ((srcAnnotName=="" && var.numAnnotations()==0) || 
// noDivVars               (var.hasAnnotation(srcAnnotName) && var.getAnnotation(srcAnnotName)==srcAnnotVal)))
// noDivVars            {
// noDivVars                    // Create the copy variable, which is identical to var, except with replaced annotations
// noDivVars                    varID varCopy(var);
// noDivVars                    varCopy.remAnnotation(srcAnnotName);
// noDivVars                    varCopy.addAnnotation(tgtAnnotName, tgtAnnotVal);
// noDivVars                    
// noDivVars                    // Record the copy
// noDivVars                    copydivVars.insert(varCopy);
// noDivVars            }
// noDivVars    }
// noDivVars    
// noDivVars    for(varIDSet::iterator it=copydivVars.begin(); it!=copydivVars.end(); it++) {
// noDivVars            if(divVars.find(*it) == divVars.end()) return modified;
// noDivVars            divVars.insert(*it);
// noDivVars    }

        return modified;
}
                   
// Merges the state of the variables in the constraint graph with the [finalAnnotName -> finalAnnotVal] annotation
//    with the state of the variables with the [remAnnotName -> remAnnotVal]. Each constraint that involves a variable
//    with the former annotation and the same variable with the latter annotation is replaced with the union of the 
//    two constraints and will only involve the variable with the [finalAnnotName -> finalAnnotVal] (latter) annotation.
// All variables with the [remAnnotName -> remAnnotVal] annotation are removed from the constraint graph.
// A variable matches the given (srcAnnotName -> srcAnnotVal) annotation if this is one of the variable's annotations
//    or if srcAnnotName=="" and the variable has no annotations.
// Avoids merging variables with annotations in the noCopyAnnots set and variables in the noCopyVars set.
// Returns true if this causes the constraint graph to change and false otherwise.
// It is assumed that variables that match [finalAnnotName -> finalAnnotVal] differ from variables that match
//    [remAnnotName -> remAnnotVal] in only that annotation.
bool ConstrGraph::mergeAnnotVars(const string& finalAnnotName, void* finalAnnotVal, 
                                 const string& remAnnotName,   void* remAnnotVal,
                                 const set<pair<string, void*> >& noCopyAnnots,
                                 const set<varID>& noCopyVars, string indent)
{
        bool modified=false;
        
        // The set of variables whose submaps we'll be deleting because they belong
        // to variables that have the [remAnnotName -> remAnnotVal] annotation
        set<varID> toDeleteX;
        
        for(map<varID, map<varID, affineInequality> >::iterator itX=vars2Value.begin();
            itX!=vars2Value.end(); itX++)
        {
                const varID& x = itX->first;
                //Dbg::dbg << "x="<<x.str()<<"\n";
                
                // If x matches the final annotation
                if(annotInterestingVar(x, noCopyAnnots, noCopyVars, finalAnnotName, finalAnnotVal))
                {
                        // Create a version of x with the final annotations replaced with the rem annotations
                        varID xRem(x);
                        ROSE_ASSERT( xRem.swapAnnotations(finalAnnotName, finalAnnotVal, remAnnotName, remAnnotVal) );
                        //Dbg::dbg << "xRem="<<xRem.str()<<", itX->second.size()="<<itX->second.size()<<"\n";
                        
                        // Union x's sub-map, unioning any inequalities x<=yFinal and x<=yRem and deleting the latter
                        modified = mergeAnnotVarsSubMap(itX->second, finalAnnotName, finalAnnotVal, 
                                              remAnnotName, remAnnotVal, noCopyAnnots, noCopyVars) || modified;
                        
                        map<varID, map<varID, affineInequality> >::iterator itXrem = vars2Value.find(xRem);
                        // If x's associated rem variable has its own sub-map
                        if(itXrem != vars2Value.end())
                        {
                                //Dbg::dbg << "    both rem and final exist, itXrem->second.size()="<<itXrem->second.size()<<"\n";
                                // Iterate through xRem's sub-map, transferring state to x's sub-map
                                for(map<varID, affineInequality>::iterator itYRem = itXrem->second.begin(); 
                                    itYRem!=itXrem->second.end(); itYRem++)
                                {
                                        const varID& yRem = itYRem->first;
                                        //Dbg::dbg << "    yRem="<<yRem.str()<<", ineq="<<itYRem->second.str()<<"\n";
                                        
                                        // For the current y variable, consider all three possibilities: y has 
                                        // a final annotation, a rem annotation or neither.
                                        if(!varHasAnnot(itYRem->first, noCopyAnnots) && noCopyVars.find(itYRem->first)==noCopyVars.end())
                                        {
                                                // xRem <= yFinal
                                                if(varHasAnnot(yRem, finalAnnotName, finalAnnotVal))
                                                {
                                                        // Union the x <= yFinal inequality with the x <= yRem inequality
                                                        unionXYsubMap(itX->second, yRem, itYRem->second, indent+"    ");
                                                }
                                                // xRem <= yRem
                                                else if(varHasAnnot(yRem, remAnnotName, remAnnotVal))
                                                {
                                                        varID yFinal(yRem);
                                                        ROSE_ASSERT( yFinal.swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
                                                        
                                                        // Update the x->yFinal inequality with this one
                                                        unionXYsubMap(itX->second, yFinal, itYRem->second, indent+"    ");
                                                }
                                                // xRem <= y (no rem or final annotation)
                                                else
                                                        // Update the x->y inequality with this one
                                                        unionXYsubMap(itX->second, yRem, itYRem->second, indent+"    ");
                                        }
                                // xRem <= y (no rem or final annotation)
                                else
                                        // Update the x->y inequality with this one
                                                unionXYsubMap(itX->second, yRem, itYRem->second, indent+"    ");
                                }
                                
                                modified = modified || (itXrem->second.size()>0);
                                // Record xRem's submap for future deletion
                                toDeleteX.insert(xRem);
                                
                                modifiedVars.insert(x);
                                modifiedVars.insert(xRem);
                                /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(x); */
                        }
                        // If x's associated rem variable doesn't have its own sub-map, we don't need to
                        // do anything since there is nothing to union with the x <= ? inequalities
                }
                // If x matches the rem annotation
                else if(annotInterestingVar(x, noCopyAnnots, noCopyVars, remAnnotName, remAnnotVal))
                {
                        // Create a version of x with the rem annotations replaced with the final annotations
                        varID xFinal(x);
                        ROSE_ASSERT( xFinal.swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
                        
                        map<varID, map<varID, affineInequality> >::iterator itXfinal = vars2Value.find(xFinal);
                        // If x's equivalent final variable does not have its own sub-map
                        // (the case where both variables exist is covered above)
                        if(itXfinal == vars2Value.end())
                        {
                                // Copy x's sub-map over under xFinal's name by simply re-annotating x with the final annotation
                                //ROSE_ASSERT( (itX->first).swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
                                vars2Value[xFinal] = itX->second;
                                
                                // Union x's sub-map, unioning any inequalities x<=yFinal and x<=yRem and deleting the latter
                                mergeAnnotVarsSubMap(/*itX->second**/(vars2Value.find(xFinal))->second, finalAnnotName, finalAnnotVal, 
                                 remAnnotName, remAnnotVal, noCopyAnnots, noCopyVars);
                                
                                // Remove itXfinal from this sub-map (if its there) to make sure that xFinal doesn't 
                                // have a relation to itself
                                itX->second.erase(xFinal);
                                
                                modified = true;
                        }
                }
                // If this variable has neither the rem nor the final annotation
                else
                {
                        // Union x's sub-map, unioning any inequalities x<=yFinal and x<=yRem and deleting the latter
                        modified = mergeAnnotVarsSubMap(itX->second, finalAnnotName, finalAnnotVal, 
                                              remAnnotName, remAnnotVal, noCopyAnnots, noCopyVars) || modified;
                }
        }
        
        // Delete the sub-maps recorded in toDeleteX
        for(set<varID>::iterator it=toDeleteX.begin(); it!=toDeleteX.end(); it++)
                vars2Value.erase(*it);
                
        // -------------------------------------------
        // Now update vars, arrays and divVars
        
        return modified;
}

// Union the current inequality for y in the given subMap of vars2Value with the given affine inequality
// Returns true if this causes a change in the subMap, false otherwise.
bool ConstrGraph::unionXYsubMap(map<varID, affineInequality>& subMap, const varID& y, const affineInequality& ineq, string indent)
{
        bool modified = false;
        
//Dbg::dbg << "unionXYsubMap("<<y.str()<<", "<<ineq.str()<<")\n";
        
        // Update the x->y inequality with this one
        map<varID, affineInequality>::iterator loc = subMap.find(y);
        if(loc != subMap.end())
                modified = loc->second.unionUpd(ineq) || modified;
        else
        {
                subMap[y] = ineq;
                modified = true;
        }
        
//Dbg::dbg << "unionXYsubMap() subMap["<<y.str()<<"] = "<<subMap[y].str()<<"\n";
        
        return modified;
}

// Merges the given sub-map of var2Vals, just like mergeAnnotVars. Specifically, for every variable in the subMap
// that has a [remAnnotName -> remAnnotVal] annotation,
// If there exists a corresponding variable that has the [finalAnnotName -> finalAnnotVal] annotation, 
//    their respective inequalities are unioned. This union is left with the latter variable and the former
//    variable's entry in subMap is removed
// If one does not exist, we simply replace the variable's record with an identical one that now belongs
//    to its counterpart with the [finalAnnotName -> finalAnnotVal] annotation.
// Other entries are left alone.
// Returns true if this causes the subMap to change, false otherwise.
bool ConstrGraph::mergeAnnotVarsSubMap(map<varID, affineInequality>& subMap, 
                                       string finalAnnotName, void* finalAnnotVal, 
                                       string remAnnotName,   void* remAnnotVal,
                                       const set<pair<string, void*> >& noCopyAnnots,
                                       const set<varID>& noCopyVars, string indent)
{
        bool modified = false;
        
        // The set of variables whose sub-submaps we'll be deleting because they belong
        // to variables that have the [remAnnotName -> remAnnotVal] annotation
        set<varID> toDeleteY;
        
        // Replace all variables that have the rem annotation with equivalent variables that have 
        // the final annotation or perform a union if both are present
        for(map<varID, affineInequality>::iterator itYRem = subMap.begin(); 
            itYRem!=subMap.end(); itYRem++)
        {
                const varID& y = itYRem->first;
                //Dbg::dbg << "    mergeAnnotVarsSubMap y="<<y.str()<<"\n";
                
                // If the current y matches the rem annotation
                if(annotInterestingVar(y, noCopyAnnots, noCopyVars, remAnnotName, remAnnotVal, indent+"    "))
                {
                        // Generate the version of y that has the final annotation
                        varID yFinal(y);
                        ROSE_ASSERT( yFinal.swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
                        
                        map<varID, affineInequality>::iterator yFinalIt = subMap.find(yFinal);
                        //Dbg::dbg << "    mergeAnnotVarsSubMap yFinal="<<yFinal.str()<<", found="<<(yFinalIt != subMap.end())<<"\n";

                        // If this sub-map contains both a rem and a final version of the same variable
                        if(yFinalIt != subMap.end())
                        {
                                // Union both their inequalities and place the result into the final variable
                                //Dbg::dbg << "        old ineq="<<yFinalIt->second.str()<<"\n";
                                yFinalIt->second.unionUpd(itYRem->second);
                                //Dbg::dbg << "        new ineq="<<yFinalIt->second.str()<<"\n";
                                // Record the rem variable for deletion
                                toDeleteY.insert(y);
                        }
                        // If this sub-map only contains a rem version of this variable
                        else
                        {
                                // Swap its annotation so that it now has the final annotation
                                //ROSE_ASSERT( (itYRem->first).swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
                                subMap[yFinal] = itYRem->second;
                                // Record the rem variable for deletion
                                toDeleteY.insert(y);
                        }
                        
                        modified = true;
                }
        }
        
        // Erase all the y entries in x's submap that belong rem variables
        for(set<varID>::iterator it=toDeleteY.begin(); it!=toDeleteY.end(); it++)
                subMap.erase(*it);
        
        // Filter vars, arrays and divVars to remove any rem variables. When variables
        // have a rem version but not a final version, we replace the rem version with the final version.
        modified = mergeAnnotVarsSet(vars, 
                                     finalAnnotName, finalAnnotVal, remAnnotName, remAnnotVal,
                                     noCopyAnnots, noCopyVars, indent+"    ") || modified;
// noDivVars    modified = mergeAnnotVarsSet(divVars, 
// noDivVars                                 finalAnnotName, finalAnnotVal, remAnnotName, remAnnotVal,
// noDivVars                                 noCopyAnnots, noCopyVars, indent+"    ") || modified;
        
        return modified;        
}

// Support routine for mergeAnnotVars(). Filters out any rem variables in the given set, replacing
// them with their corresponding final versions if those final versions are not already in the set
// Returns true if this causes the set to change, false otherwise.
bool ConstrGraph::mergeAnnotVarsSet(set<varID> varsSet, 
                                    string finalAnnotName, void* finalAnnotVal, 
                                    string remAnnotName,   void* remAnnotVal,
                                    const set<pair<string, void*> >& noCopyAnnots,
                                    const set<varID>& noCopyVars, string indent)
{
        bool modified = false;
        
        set<varID> varsToDelete;
        set<varID> varsToInsert;
        
        for(set<varID>::iterator it=varsSet.begin(); it!=varsSet.end(); it++)
        {
                const varID& var = *it;
                
                // If this is a rem variable
                if(annotInterestingVar(var, noCopyAnnots, noCopyVars, remAnnotName, remAnnotVal, indent+"    "))
                {
                        // Create a version of var with the rem annotations replaced with the final annotations
                        varID varFinal(var);
                        ROSE_ASSERT( varFinal.swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
                        
                        // If the corresponding final variable exists in the set
                        if(varsSet.find(varFinal)!=varsSet.end())
                                // Record that we're going to remove var
                                varsToDelete.insert(var);
                        else
                        {
                                varID varFinal(var);
                                // Change var's annotation to convert it from rem to final
                                ROSE_ASSERT( varFinal.swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
                                
                                // Record that we're going to remove var and insert varFinal
                                varsToDelete.insert(var);
                                varsToInsert.insert(varFinal);
                        }
                        
                        modified = true;
                }
        }
        
        for(set<varID>::iterator it=varsToDelete.begin(); it!=varsToDelete.end(); it++)
                varsSet.erase(*it);
        
        for(set<varID>::iterator it=varsToInsert.begin(); it!=varsToInsert.end(); it++)
                varsSet.insert(*it);
        
        return modified;
}

// Returns true if the given variable has an annotation in the given set and false otherwise.
// The variable matches an annotation if its name and value directly match or if the variable
// has no annotations and the annotation's name is "".
bool ConstrGraph::varHasAnnot(const varID& var, const set<pair<string, void*> >& annots, string indent)
{
        for(set<pair<string, void*> >::const_iterator it=annots.begin(); it!=annots.end(); it++)
        {
                if(varHasAnnot(var, (*it).first, (*it).second, indent))
                        return true;
        }
        return false;
}

// Returns true if the given variable has an annotation in the given set and false otherwise.
// The variable matches an annotation if its name and value directly match or if the variable
// has no annotations and the annotName=="".
bool ConstrGraph::varHasAnnot(const varID& var, string annotName, void* annotVal, string indent)
{
        // If the annotation matches variables with no annotation and this variable has no annotations
        if(annotName=="" && var.numAnnotations()==0)
                return true;
        
        // If var has the given annotation, which matches matches the given value
        if(var.hasAnnotation(annotName) && var.getAnnotation(annotName)==annotVal)
                return true;
                        
        return false;
}

// Returns a constraint graph that only includes the constrains in this constraint graph that involve the
// variables in focusVars and their respective divisibility variables, if any. 
// It is assumed that focusVars only contains scalars and not array ranges.
ConstrGraph* ConstrGraph::getProjection(const varIDSet& focusVars, string indent)
{
        //ConstrGraph* pCG = new ConstrGraph(func, n, state, ldva, divL, // GB : 2011-03-05 (Removing Sign Lattice Dependence) sgnL, 
        //                                   getLevel(true).first!=uninitialized, indent+"    ");

        ConstrGraph* pCG = dynamic_cast<ConstrGraph*>(copy());
        pCG->setToConstrKnown(conj, true, indent+"    ");
        
        // focusVars that are inside this->vars and their respective divisibility variables, if any
        // We record these variables in allFocusVars and only worry about them when extracting
        //    the projection constraints. 
        // // Furthermore, we add these variables to the list of vars
        // //    and divisibility variables of pCG.
        varIDSet allFocusVars;
        for(varIDSet::iterator it=focusVars.begin(); it!=focusVars.end(); it++)
        {
                const varID& var = *it;
                if(vars.find(var) != vars.end())
                {
                        addVar(var, "");
                        allFocusVars.insert(var);
//                      pCG->addVar(var, "");
                        
// noDivVars                    varID divVar = getDivVar(var);
// noDivVars                    if(divVars.find(divVar) != divVars.end())
// noDivVars                    {
// noDivVars                            allFocusVars.insert(divVar);
// noDivVars//                          pCG->addDivVar(var);
// noDivVars                    }
                }
        }
        
        // We simply copy the sets of vars and divisibility variables from this to pCG
        // We may not need all of them but its simpler this way
        pCG->vars = vars;
// noDivVars    pCG->divVars = divVars;
        
        // Copy all constraints in vars2Value that pertain to variables in allFocusVars to pCG
        for(map<varID, map<varID, affineInequality> >::iterator itX=vars2Value.begin(); itX!=vars2Value.end(); itX++)
        {
                // If itX->first appears in allFocusVars
                if(allFocusVars.find(itX->first) != allFocusVars.end())
                {
                        // Copy all of the [itX->first <= ???] conditions to pCG
                        pCG->vars2Value[itX->first] = itX->second;
                        pCG->modifiedVars.insert(itX->first);
                }
                else
                {
                        bool foundMatches=false; // Set to true when we find a variable in itX->second that appears in allFocusVars
                        for(map<varID, affineInequality>::iterator itY=itX->second.begin(); itY!=itX->second.end(); itY++)
                        {
                                // if itY->first appears in allFocusVars
                                if(allFocusVars.find(itY->first) != allFocusVars.end())
                                {
                                        if(!foundMatches)
                                        {
                                                // Create a map in pCG for itX->first and insert itY->first ==> itY->second into this map
                                                map<varID, affineInequality> initMap;
                                                initMap[itY->first] = itY->second;
                                                pCG->vars2Value[itX->first] = initMap;
                                                foundMatches = true;
                                        }
                                        else
                                        {
                                                // Insert itY->first ==> itY->second into the pCG's map of itX->first
                                                (pCG->vars2Value[itX->first])[itY->first] = itY->second;
                                        }
                                        modifiedVars.insert(itY->first);
                                }
                        }
                }
        }
        
        return pCG;
}

// Creates a new constraint graph that is the disjoint union of the two given constraint graphs.
// The variables in cg1 and cg2 that are not in the noAnnot set, are annotated with cg1Annot and cg2Annot, respectively,
// under the name annotName.
// cg1 and cg2 are assumed to have identical constraints between variables in the noAnnotset.
ConstrGraph* ConstrGraph::joinCG(ConstrGraph* cg1, void* cg1Annot, ConstrGraph* cg2, void* cg2Annot, 
                                 string annotName, const varIDSet& noAnnot, string indent)
{
        // Both constraint graphs correspond to the same function, dataflow node and state
        ROSE_ASSERT(cg1->func     == cg2->func);
        //ROSE_ASSERT(cg1->n        == cg2->n);
        ROSE_ASSERT(&(cg1->state) == &(cg2->state));
        
        // The annotations that will be associated with the two constraint graphs are different 
        // (otherwise there'd be collistions)
        ROSE_ASSERT(cg1Annot != cg2Annot);
        
        //ConstrGraph* combo = new ConstrGraph(cg1->func, cg1->n, cg1->state, true, indent+"    ");
        ConstrGraph* combo = dynamic_cast<ConstrGraph*>(cg1->copy());
        combo->setToBottom();
        if(CGDebugLevel>=1)
        {
                Dbg::dbg << indent << "joinCG("<<cg1<<", "<<cg1Annot<<", "<<cg2<<", "<<cg2Annot<<", "<<annotName<<", noAnnot: [";
                for(varIDSet::const_iterator it = noAnnot.begin(); it!=noAnnot.end(); )
                { Dbg::dbg << (*it).str(); it++; if(it!=noAnnot.end()) Dbg::dbg << ", "; }
                Dbg::dbg << "]\n";
                Dbg::dbg << indent << "=== joinCG_copyState1 === \n";
        }
        joinCG_copyState(combo, cg1, cg1Annot, annotName, noAnnot, indent+"    ");
        if(CGDebugLevel>=1) Dbg::dbg << indent << "=== joinCG_copyState2 === \n";
        joinCG_copyState(combo, cg2, cg2Annot, annotName, noAnnot, indent+"    ");
        if(CGDebugLevel>=1) Dbg::dbg << indent << "=== transitiveClosure === \n";
        combo->transitiveClosure(indent+"    ");
        
        return combo;
}

// Copies the per-variable contents of srcCG to tgtCG, while ensuring that in tgtCG all variables that are not
// in noAnnot are annotated with the annotName->annot label. For variables in noAnnot, the function ensures
// that tgtCG does not have inconsistent mappings between such variables.
void ConstrGraph::joinCG_copyState(ConstrGraph* tgtCG, ConstrGraph* srcCG, void* annot, 
                                   string annotName, const varIDSet& noAnnot, string indent)
{
        // === vars2Value ===
        for(map<varID, map<varID, affineInequality> >::iterator itX=srcCG->vars2Value.begin();
            itX!=srcCG->vars2Value.end(); itX++)
        {
                // Only worry about vars
                if(srcCG->vars.find(itX->first) == srcCG->vars.end()) continue;
                
                varID x = itX->first;
                                
                // Annotate x if necessary and add a fresh map for x -> ? mappings in tgtCG->vars2Value, if necessary
                if(noAnnot.find(x) == noAnnot.end())
                {
                        //Dbg::dbg << "joinCG_copyState: annotating "<<x.str();
                        // Annotate x if it is not already annotated
                        if(!x.getAnnotation(annotName))
                                x.addAnnotation(annotName, annot);
                        //Dbg::dbg << " : "<<x.str()<<"\n";
                        map<varID, affineInequality> empty;
                        tgtCG->vars2Value[x] = empty;
                        
                        // Add the annotated variable as a scalar to tgtCG to ensure that transitiveClosure operates on it
                        tgtCG->addVar(x, "");
                }
                else if(tgtCG->vars2Value.find(x) == tgtCG->vars2Value.end())
                {
                        map<varID, affineInequality> empty;
                        tgtCG->vars2Value[x] = empty;
                }
                tgtCG->modifiedVars.insert(x);
                /* GB 2011-06-02 : newConstrVars->modifiedVars : tgtCG->newConstrVars.insert(x); */
                map<varID, affineInequality>& xToTgt = tgtCG->vars2Value[x];
                
                for(map<varID, affineInequality>::iterator itY=itX->second.begin();
                    itY!=itX->second.end(); itY++)
                {
                        // Only worry about vars
                        if(srcCG->vars.find(itY->first) == srcCG->vars.end()) continue;
                        
                        varID y = itY->first;
                        // Annotate y if necessary and add a fresh x->y mapping, if necessary
                        if(noAnnot.find(y) == noAnnot.end())
                        {
                                //Dbg::dbg << "joinCG_copyState: annotating     ->"<<y.str();
                                // Annotate y if it is not already annotated
                                if(!y.getAnnotation(annotName))
                                        y.addAnnotation(annotName, annot);
                                //Dbg::dbg << " : "<<y.str()<<"\n";
                                xToTgt[y] = itY->second;
                                
                                // Add the annotated variable as a scalar to tgtCG to ensure that transitiveClosure operates on it
                                tgtCG->addVar(y, "");
                                
                                if(CGDebugLevel>=1) 
                                        Dbg::dbg << indent << "joinCG_copyState: addingA "<<x.str()<<"->"<<y.str()<<": "<<xToTgt[y].str()<<"\n";
                        }
                        // // We do not allow disagreements about the value of the x->y mapping 
                        // If there are disagreements about the value of the x->y mapping (can only happen if both
                        // x and y are in noAnnot and they have different mappings in the various constraint graphs)
                        // we take the union of the two options
                        else if(xToTgt.find(y) != xToTgt.end())
                        {
                                /*if(xToTgt[y] != itY->second)
                                {
                                        Dbg::dbg << "x="<<x.str()<<"y="<<y.str()<<"\n";
                                        for(varIDSet::const_iterator it = noAnnot.begin(); it!=noAnnot.end(); it++)
                                                Dbg::dbg << "noAnnot: "<<(*it).str()<<"\n";
                                        ROSE_ASSERT(xToTgt[y] == itY->second);
                                }*/
                                xToTgt[y] += itY->second;
                                if(CGDebugLevel>=1) 
                                        Dbg::dbg << indent << "joinCG_copyState: unioning "<<x.str()<<"->"<<y.str()<<" from "<<itY->second.str()<<" to "<<xToTgt[y].str()<<"\n";
                        }
                        else
                        {
                                xToTgt[y] = itY->second;
                                if(CGDebugLevel>=1) 
                                        Dbg::dbg << indent << "joinCG_copyState: addingB "<<x.str()<<"->"<<y.str()<<": "<<xToTgt[y].str()<<"\n";
                        }
                        tgtCG->modifiedVars.insert(y);
                        /* GB 2011-06-02 : newConstrVars->modifiedVars : tgtCG->newConstrVars.insert(y); */
                }
        }
        
        // === vars ===
        for(varIDSet::iterator it=srcCG->vars.begin(); it!=srcCG->vars.end(); it++)
        {
                varID var = *it;
                if(noAnnot.find(*it) == noAnnot.end())
                        var.addAnnotation(annotName, annot);
                tgtCG->vars.insert(var);
        }
        
        // === divVars ===
// noDivVars    for(varIDSet::iterator it=srcCG->divVars.begin(); it!=srcCG->divVars.end(); it++)
// noDivVars    {
// noDivVars            varID var = *it;
// noDivVars            if(noAnnot.find(*it) == noAnnot.end())
// noDivVars                    var.addAnnotation(annotName, annot);
// noDivVars            tgtCG->divVars.insert(var);
// noDivVars    }
        
        /*for(varIDSet::iterator it = tgtCG->vars.begin(); it!=tgtCG->vars.end(); it++)
                Dbg::dbg << "    var: "<< (*it).str()<<"\n";*/
}

// Replaces all references to variables with the given annotName->annot annotation to references to variables without the annotation
// Returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::removeVarAnnot(string annotName, void* annot, string indent)
{
        bool modified=false;
        
        // === vars2Value ===
        // Set of vars x whose x->? mappings we'll need to change
        varIDSet changedVarsX;
        
        for(map<varID, map<varID, affineInequality> >::iterator itX=vars2Value.begin();
            itX!=vars2Value.end(); itX++)
        {
                varID x = itX->first; 
                if(x.getAnnotation(annotName) == annot)
                {
                        changedVarsX.insert(x);
                        modifiedVars.insert(x); // ???
                }
                
                // Set of vars y for which we'll need to remove the annotation
                varIDSet changedVarsY;
                for(map<varID, affineInequality>::iterator itY=itX->second.begin();
                    itY!=itX->second.end(); itY++)
                {
                        varID y = itY->first; 
                        if(y.getAnnotation(annotName) == annot)
                                changedVarsY.insert(y);
                }
                
                modified = (changedVarsY.size()>0) || modified;
                
                // Change all the x->y mappings for all variables y for which we need to remove the annotation
                for(varIDSet::iterator it=changedVarsY.begin(); it!=changedVarsY.end(); it++)
                {
                        varID y = *it;
                        // Remove the annotation from y to produce y'
                        ROSE_ASSERT(y.remAnnotation(annotName));
                        // Move the x->y mapping to become the x->y' mapping
                        (itX->second)[y] = (itX->second)[*it];
                        (itX->second).erase(*it);
                }
        }       
        
        modified = (changedVarsX.size()>0) || modified;
        
        // Change all the x->? mappings for all variables x for which we need to remove the annotation
        for(varIDSet::iterator it=changedVarsX.begin(); it!=changedVarsX.end(); it++)
        {
                varID x = *it;
                // Remove the annotation from x to produce x'
                ROSE_ASSERT(x.remAnnotation(annotName));
                // Move the x->? mapping to become the x'->? mapping
                vars2Value[x] = vars2Value[*it];
                vars2Value.erase(*it);
        }
        
        // === vars ===
        varIDSet newVars;
        for(varIDSet::iterator it=vars.begin(); it!=vars.end(); it++)
        {
                varID var = *it;
                if(var.getAnnotation(annotName) == annot)
                        modified = var.remAnnotation(annotName) || modified;
                newVars.insert(var);
        }
        vars = newVars;
        
        // === divVars ===
// noDivVars    varIDSet newdivVars;
// noDivVars    for(varIDSet::iterator it=divVars.begin(); it!=divVars.end(); it++)
// noDivVars    {
// noDivVars            varID var = *it;
// noDivVars            if(var.getAnnotation(annotName) == annot)
// noDivVars                    modified = var.remAnnotation(annotName) || modified;
// noDivVars            newdivVars.insert(var);
// noDivVars    }
// noDivVars    divVars = newdivVars;   
        
        return modified;
}

// Replaces all references to variables with the given annotName->annot annotation to 
// references to variables without the annotation
// Returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::replaceVarAnnot(string oldAnnotName, void* oldAnnot,
                                  string newAnnotName, void* newAnnot, string indent)
{
        bool modified=false;
        
        // === vars2Value ===
        // Set of vars x whose x->? mappings we'll need to change
        varIDSet changedVarsX;
        
        for(map<varID, map<varID, affineInequality> >::iterator itX=vars2Value.begin();
            itX!=vars2Value.end(); itX++)
        {
                varID x = itX->first; 
                if(x.getAnnotation(oldAnnotName) == oldAnnot)
                {
                        changedVarsX.insert(x);
                        // This is not a modification that can affect the transitive closure
                        //modifiedVars.insert(x);
                }
                
                // Set of vars y for which we'll need to remove the annotation
                varIDSet changedVarsY;
                for(map<varID, affineInequality>::iterator itY=itX->second.begin();
                    itY!=itX->second.end(); itY++)
                {
                        varID y = itY->first; 
                        if(y.getAnnotation(oldAnnotName) == oldAnnot)
                                changedVarsY.insert(y);
                }
                
                modified = (changedVarsY.size()>0) || modified;
                
                // Change all the x->y mappings for all variables y for which we need to remove the annotation
                for(varIDSet::iterator it=changedVarsY.begin(); it!=changedVarsY.end(); it++)
                {
                        varID y = *it;
                        // Remove the annotation from y to produce y'
                        ROSE_ASSERT(y.swapAnnotations(oldAnnotName, oldAnnot, newAnnotName, newAnnot));
                        // Move the x->y mapping to become the x->y' mapping
                        (itX->second)[y] = (itX->second)[*it];
                        (itX->second).erase(*it);
                }
        }       
        
        modified = (changedVarsX.size()>0) || modified;
        
        // Change all the x->? mappings for all variables x for which we need to remove the annotation
        for(varIDSet::iterator it=changedVarsX.begin(); it!=changedVarsX.end(); it++)
        {
                varID x = *it;
                // Remove the annotation from x to produce x'
                ROSE_ASSERT(x.swapAnnotations(oldAnnotName, oldAnnot, newAnnotName, newAnnot));
                // Move the x->? mapping to become the x'->? mapping
                vars2Value[x] = vars2Value[*it];
                vars2Value.erase(*it);
        }

        // === vars ===
        varIDSet newVars;
        for(varIDSet::iterator it=vars.begin(); it!=vars.end(); it++)
        {
                varID var = *it;
                if(var.getAnnotation(oldAnnotName) == oldAnnot)
                        modified = var.swapAnnotations(oldAnnotName, oldAnnot, newAnnotName, newAnnot) || modified;
                newVars.insert(var);
        }
        vars = newVars;
        
        // === divVars ===
// noDivVars    varIDSet newdivVars;
// noDivVars    for(varIDSet::iterator it=divVars.begin(); it!=divVars.end(); it++)
// noDivVars    {
// noDivVars            varID var = *it;
// noDivVars            if(var.getAnnotation(oldAnnotName) == oldAnnot)
// noDivVars                    modified = var.swapAnnotations(oldAnnotName, oldAnnot, newAnnotName, newAnnot) || modified;
// noDivVars            newdivVars.insert(var);
// noDivVars    }
// noDivVars    divVars = newdivVars;   
        
        return modified;
}

// For all variables that have a string (tgtAnnotName -> tgtAnnotVal) annotation 
//    (or if tgtAnnotName=="" and the variable has no annotations), add the annotation
//    (newAnnotName -> newAnnotVal).
// Returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::addVarAnnot(string tgtAnnotName, void* tgtAnnotVal, string newAnnotName, void* newAnnotVal, string indent)
{
        bool modified=false;
        // === vars2Value ===
        // Set of vars x whose x->? mappings we'll need to change
        varIDSet changedVarsX;
        
        for(map<varID, map<varID, affineInequality> >::iterator itX=vars2Value.begin();
            itX!=vars2Value.end(); itX++)
        {
                varID x = itX->first; 
                if((tgtAnnotName=="" && x.numAnnotations()==0) || x.getAnnotation(tgtAnnotName)==tgtAnnotVal)
                        changedVarsX.insert(x);
                
                // This is not a modification that can affect the transitive closure
                //modifiedVars.insert(x);
                
                // Set of vars y for which we'll need to add the annotation
                varIDSet changedVarsY;
                for(map<varID, affineInequality>::iterator itY=itX->second.begin();
                    itY!=itX->second.end(); itY++)
                {
                        varID y = itY->first; 
                        if((tgtAnnotName=="" && y.numAnnotations()==0) || y.getAnnotation(tgtAnnotName)==tgtAnnotVal)
                                changedVarsY.insert(y);
                }
                
                modified = (changedVarsY.size()>0) || modified;
                
                // Change all the x->y mappings for all variables y for which we need to remove the annotation
                for(varIDSet::iterator it=changedVarsY.begin(); it!=changedVarsY.end(); it++)
                {
                        varID y = *it;
                        // Add the annotation from y to produce y'
                        y.addAnnotation(newAnnotName, newAnnotVal);
                        
                        // Move the x->y mapping to become the x->y' mapping
                        (itX->second)[y] = (itX->second)[*it];
                        (itX->second).erase(*it);
                }
        }
        
        modified = (changedVarsX.size()>0) || modified;
        
        // Change all the x->? mappings for all variables x for which we need to remove the annotation
        for(varIDSet::iterator it=changedVarsX.begin(); it!=changedVarsX.end(); it++)
        {
                varID x = *it;
                // Add the annotation from x to produce x'
                x.addAnnotation(newAnnotName, newAnnotVal);
                        
                // Move the x->? mapping to become the x'->? mapping
                vars2Value[x] = vars2Value[*it];
                vars2Value.erase(*it);
        }
        
        // === vars ===
        varIDSet newVars;
        for(varIDSet::iterator it=vars.begin(); it!=vars.end(); it++)
        {
                varID var = *it;
                if((tgtAnnotName=="" && var.numAnnotations()==0) || var.getAnnotation(tgtAnnotName)==tgtAnnotVal)
                        modified = var.addAnnotation(newAnnotName, newAnnotVal) || modified;
                newVars.insert(var);
        }
        vars = newVars;
        
        // === divVars ===
// noDivVars    varIDSet newdivVars;
// noDivVars    for(varIDSet::iterator it=divVars.begin(); it!=divVars.end(); it++)
// noDivVars    {
// noDivVars            varID var = *it;
// noDivVars            if((tgtAnnotName=="" && var.numAnnotations()==0) || var.getAnnotation(tgtAnnotName)==tgtAnnotVal)
// noDivVars                    modified = var.addAnnotation(newAnnotName, newAnnotVal) || modified;
// noDivVars            newdivVars.insert(var);
// noDivVars    }
// noDivVars    divVars = newdivVars;
        
        return modified;
}

// Adds a new range into this constraint graph 
/*void ConstrGraph::addRange(varID rangeVar)
{
        arrays.insert(rangeVar);
        emptyRange[rangeVar] = false;
}*/

/**** Transfer Function-Related Updates ****/

// Negates the constraint graph.
// Returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::negate(string indent)
{
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        
        // If the negation of this constraint graph is equal to itself
        if(l.first==uninitialized || l.first==bottom || l.first==top ||
           (l.first==constrKnown && l.second==inconsistent))
        { return false; }
        
        if(l.second==conj) constrType = negConj;
        else               constrType = conj;
        
        return true;
}

// updates the constraint graph with the information that x*a = y*b+c
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::assign(const varAffineInequality& cond, string indent)
{
        return assign(cond.getX(), cond.getY(), cond.getA(), cond.getB(), cond.getC(), indent+"    ");
}

bool ConstrGraph::assign(varID x, varID y, const affineInequality& ineq, string indent)
{
        return assign(x, y, ineq.getA(), ineq.getB(), ineq.getC(), indent+"    ");
}

bool ConstrGraph::assign(varID x, varID y, int a, int b, int c, string indent)
{
        Dbg::dbg << indent << "ConstrGraph::assign, x="<<x<<", y="<<y<<", a="<<a<<", b="<<b<<" c="<<c<<"\n";
        
        bool modified = false;
        map<varID, map<varID, affineInequality> >::iterator mapIter;
                
        // This constraint graph will now definitely be initialized
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        if(l.first==uninitialized || l.first==bottom) { setToConstrKnown(conj, false, indent+"    "); }
                
        // If the graph is maximal, there is no need to bother adding anything
        if(isMaximalState(true, indent+"    ")) return modified;

        modifiedVars.insert(x);
        /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(x); */
        
        // x = x*b + c
        if(x == y && a==1)
        {
// noDivVars            varID divX = getDivVar(x);
                
                // Remove x's divisibility variable from the constraint graph and add the divisibility constraints
//!!!           addDivVar(x, true, indent+"    ");
                
                // Iterate over all other variables i and update all the i -> x constraints
                for(map<varID, map<varID, affineInequality> >::iterator iterI = vars2Value.begin(); 
                    iterI != vars2Value.end(); iterI++)
                {
                        //const varID& i = iterI->first;
                        // Don't update the connection between x and its divisibility variable and x and itself
// noDivVars                    if(iterI->first == divX || iterI->first == x) continue;
                                
                        // Update all i->x pairs
                        for(map<varID, affineInequality>::iterator iterJ = iterI->second.begin();
                            iterJ != iterI->second.end(); iterJ++)
                        {
                                const varID& j = iterJ->first;
                                if(j != x) continue;
                                
                                // If x and z have a known constraint relationship
                                if(iterJ->second.getLevel() == affineInequality::constrKnown)
                                {
                                        affineInequality& constrIX = iterJ->second;
                                        // original constraint:
                                        // i*a <= x*b + c AND x' ==> x*b'+c'
                                        // i*a - c <= x*b
                                        // new constraint:
                                        // x'*b = (x*b'+c')*b = x*b'*b + c'*b
                                        // x'*b >= (i*a-c)*b' + c'*b
                                        // x'*b >= i*a*b' - c*b' + c'*b
                                        // i*a*b' <= x'*b + c*b' - c'*b
                                        
                                        Dbg::dbg << indent << "    assign() new constraint: "<<x<<"*"<<(constrIX.getA()*b)<<" &lt;= "<<x.str()<<"*"<<(constrIX.getB())<<" + "<<(constrIX.getC()*b - c*constrIX.getB())<<"\n";
                                        // update the constraints
                                        modified = constrIX.set(constrIX.getA()*b, constrIX.getB(), constrIX.getC()*b - c*constrIX.getB()) || modified;
                                        
                                        modifiedVars.insert(iterI->first);
                                        /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(iterI->first); */
                                }
                        }
                }
                
                // Iterate over all of x->z constraints
                if(vars2Value.find(x) != vars2Value.end()) {
                        for(map<varID, affineInequality>::iterator iterZ = vars2Value[x].begin(); iterZ != vars2Value[x].end(); iterZ++)
                        {
                                //const varID& z = iterZ->first;
                                // Don't update the connection between x and its divisibility variable
// noDivVars                            if(iterZ->first == divX) continue;
                                        
                                affineInequality& constrXZ = iterZ->second;
                                
                                // If x and z have a known constraint relationship
                                if(constrXZ.getLevel() == affineInequality::constrKnown)
                                {
                                        // original constraint:
                                        // x*a <= z*b + c AND x' = x*b'+c'
                                        // new constraint:
                                        // x'*a = (x*b'+c')*a = x*b'*a + c'*a
                                        // x'*a = x*a*b' + c'*a <= (z*b + c)*b' + c'*a
                                        // x'*a <= z*b*b' + c*b' + c'*a
                                        
                                        Dbg::dbg << indent << "    assign() new constraint: "<<x<<"*"<<(b*constrXZ.getA())<<" &lt;= "<<iterZ->first<<"*"<<(constrXZ.getB()*b)<<" + "<<(constrXZ.getC()*b + c*constrXZ.getA())<<"\n";
                                        // update the constraints
                                        modified = constrXZ.set(constrXZ.getA(), constrXZ.getB()*b, constrXZ.getC()*b + c*constrXZ.getA()) || modified;
                                        
                                        modifiedVars.insert(iterZ->first);
                                        /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(iterZ->first); */
                                }
                        }
                }
        }
        // case x*a = y*b + c
        else if(x!=y)
        {
                // Disconnect all variables from x
                eraseVarConstr(x, true, indent+"    ");
                
                // Remove x's divisibility variable from the constraint graph and add the divisibility constraints
//!!!           addDivVar(x, true, indent+"    ");
                Dbg::dbg << indent << "    assign() new constraint: "<<x.str()<<"*"<<a<<" &lt;= "<<y.str()<<"*"<<b<<" + "<<c<<"\n";
                
                // x*a <= y*b + c
                setVal(x, y, a, b, c, indent+"    ");
                // y*b <= x*a - c
                setVal(y, x, b, a, 0-c, indent+"    ");
                
                modifiedVars.insert(y);
                /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(y); */
                
                modified = true;
        }
        // x*a = x*1
        else if(x==y && b==1 && c==0)
        {
                ROSE_ASSERT(0);
        }
        else
                // x*a = x*b + c
                ROSE_ASSERT(0);
        
        constrChanged = constrChanged || modified;
        
        return modified;
}

// Updates the constraint graph to record that there are no constraints in the given variable.
// Returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::assignBot(varID var, string indent)
{
        bool modified = false;
        map<varID, map<varID, affineInequality> >::iterator mapIter;
                
        // This constraint graph will now definitely be initialized
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        if(l.first==uninitialized || l.first==bottom) { setToConstrKnown(conj, false, indent+"    "); }
                
        // If the graph is maximal, there is no need to bother adding anything
        if(isMaximalState(true, indent+"    ")) return modified;

        modifiedVars.insert(var);
        /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(var); */
        
        // If there are constraints a*var <= b*y + c
        if(vars2Value.find(var) != vars2Value.end()) {
                // Remove these constraints
                modified = (vars2Value[var].size()>0) || modified;
                vars2Value.erase(var);
        }
        
        // Look for constraints ... a*x <= b*var + c
        for(map<varID, map<varID, affineInequality> >::iterator iterX = vars2Value.begin(); 
                    iterX != vars2Value.end(); iterX++)
        {
                // If such a constraint exists, remove it
                if(iterX->second.find(var) != iterX->second.end()) {
                        modified = true;
                        iterX->second.erase(var);
                }
        }
        
        return modified;
}

// Updates the constraint graph to record that the constraints between the given variable and
//    other variables are Top.
// Returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::assignTop(varID var, string indent)
{
        bool modified = false;
        map<varID, map<varID, affineInequality> >::iterator mapIter;
                
        // This constraint graph will now definitely be initialized
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        if(l.first==uninitialized || l.first==bottom) { setToConstrKnown(conj, false, indent+"    "); }
                
        // If the graph is maximal, there is no need to bother adding anything
        if(isMaximalState(true, indent+"    ")) return modified;

        modifiedVars.insert(var);
        /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(var); */
        
        // If there are constraints a*var <= b*y + c
        if(vars2Value.find(var) != vars2Value.end()) {
                for(map<varID, affineInequality>::iterator iterY = vars2Value[var].begin(); 
                    iterY != vars2Value[var].end(); iterY++)
                        modified = iterY->second.setToTop() || modified;
        }
        
        // Look for constraints ... a*x <= b*var + c
        for(map<varID, map<varID, affineInequality> >::iterator iterX = vars2Value.begin(); 
                    iterX != vars2Value.end(); iterX++)
        {
                // If such a constraint exists, set it to Top
                if(iterX->second.find(var) != iterX->second.end()) {
                        modified = iterX->second[var].setToTop() || modified;
                }
        }
        
        return modified;
}

/*// Undoes the i = j + c assignment for backwards analysis
void
ConstrGraph::undoAssignment( quad i, quad j, quad c )
{
        m_quad2str::iterator mapIter, mapIter2;

        // j = j+c 
        // given that i's pre-assignment value is unknown, remove all links
        // from i to anything else
        if ( i != j )
        {
                // iterate over all pairs of variables in vars2Name
                for ( mapIter = vars2Name.begin(); mapIter != vars2Name.end(); mapIter++ ) 
                        for ( mapIter2 = vars2Name.begin(); mapIter2 != vars2Name.end(); mapIter2++ ) 
                        {
                                quad x = mapIter->first, y = mapIter2->first;
                                if ( x != y )
                                        if ( x == i || y == i )
                                                setVal( x, y, INF );
                                        else
                                                setVal( x, y, getVal( x, y ) );
                        }
        }
        // i = i + c
        // decrement i by c
        else
        {
#ifdef DEBUG_FLAG2
                Dbg::dbg << "i = i + c ---- reverting: " << vars2Name[i] << "="
                                 << vars2Name[i] << "+" << c << "\n";
#endif

                // iterate over all variables
                for ( mapIter = vars2Name.begin(); mapIter != vars2Name.end(); mapIter++ )
                {
                        quad y = mapIter->first;
                        // change the connection between the current variable and i to account
                        // for i being c smaller than before
                        if ( y != i )
                        {
                                quad val = getVal(i, y);
                                setVal( i, y, (val == INF ? INF : val - c) );
                                val = getVal(y, i);
                                setVal( y, i, (val == INF ? INF : val + c) );
                        }
                }
        }
        
        initialized = true; // this constraint graph is now definitely initialized
}*/

/*
// kills all links from variable x to every other variable
void
ConstrGraph::killVariable( quad x )
{
        m_quad2str::iterator mapIter;
        for ( mapIter = vars2Name.begin(); mapIter != vars2Name.end(); mapIter++ )
                if (mapIter->first != x)
                {
                        if (getVal(mapIter->first, x) != INF)
                                setVal(mapIter->first, x, INF);
                        if (getVal(x, mapIter->first) != INF)
                                setVal(x, mapIter->first, INF);
                }
        
        initialized = true; // this constraint graph is now definitely initialized
}
*/

// add the condition (x*a <= y*b + c) to this constraint graph
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::assertCond(const varAffineInequality& cond, string indent)
{
        /*Dbg::dbg << "assertCond cond.getX()="<<cond.getX().str()<<"\n";
        Dbg::dbg << "assertCond cond.getY()="<<cond.getY().str()<<"\n";*/
        return assertCond(cond.getX(), cond.getY(), cond.getIneq(), indent);
}

// add the condition (x*a <= y*b + c) to this constraint graph
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::assertCond(const varID& x, const varID& y, const affineInequality& ineq, string indent)
{
        bool modified = false;
        
        // Note: assertCond doesn't check whether x and y are arrays 
        //   with empty ranges only because setVal() does this already
        affineInequality* constr = getVal(x, y, indent+"    ");
        Dbg::dbg << indent << "    assertCond("<<x<<", "<<x<<")\n";
        // if there is already a constraint between x and y, update it
        if(constr)
                modified = constr->intersectUpd(ineq) || modified;
        // else, create a new constraint
        else
        {
                modified = setVal(x, y, ineq, indent+"    ") || modified;
                
                //affineInequality* constrV1V2 = getVal(x, y);
                //printf("x=%s, y=%s, constrXY=%p\n", x.str().c_str(), y.str().c_str(), constrV1V2);
        }
        
        constrChanged = constrChanged || modified;
        return modified;
}

// Add the condition (x*a <= y*b + c) to this constraint graph. The addition is done via a conjunction operator, 
// meaning that the resulting graph will be left with either (x*a <= y*b + c) or the original condition, whichever is stronger.
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::assertCond(const varID& x, const varID& y, int a, int b, int c, string indent)
{
        bool modified = false;
        
        // Note: assertCond doesn't check whether x and y are arrays 
        //   with empty ranges only because setVal() does this already
        affineInequality* constr = getVal(x, y, indent+"    ");
        Dbg::dbg << "    assertCond("<<x<<", "<<y<<", "<<a<<", "<<b<<", "<<c<<") constr="<<constr<<"\n";
        // If there is already a constraint between x and y, update it
        if(constr)
        {
                affineInequality newConstr(a, b, c, x==zeroVar, y==zeroVar, //GB : 2011-03-05 (Removing Sign Lattice Dependence) getVarSign(x, indent+"    "), getVarSign(y, , indent+"    ")
                                           affineInequality::unknownSgn, affineInequality::unknownSgn);
                Dbg::dbg << "        Original constr="<<constr->str("")<<"\n";
                Dbg::dbg << "        newConstr="<<newConstr.str("")<<"\n";
                (*constr) *= newConstr;
                Dbg::dbg << "        ==&gt; Updated constr"<<constr->str("")<<"\n";
                modified = true;
        }
        // else, create a new constraint
        else
        {
                modified = setVal(x, y, a, b, c, indent+"    ") || modified;
                
                //affineInequality* constrV1V2 = getVal(x, y);
                //printf("x=%s, y=%s, constrXY=%p\n", x.str().c_str(), y.str().c_str(), constrV1V2);
                Dbg::dbg << "       Created new constraint\n";
        }
        
        constrChanged = constrChanged || modified;
        return modified;
}

// add the condition (x*a = y*b + c) to this constraint graph
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::assertEq(const varAffineInequality& cond, string indent)
{
        return assertEq(cond.getX(), cond.getY(), cond.getA(), cond.getB(), cond.getC(), indent);
}

bool ConstrGraph::assertEq(varID x, varID y, const affineInequality& ineq, string indent)
{
        return assertEq(x, y, ineq.getA(), ineq.getB(), ineq.getC(), indent);
}

bool ConstrGraph::assertEq(const varID& x, const varID& y, int a, int b, int c, string indent)
{
        bool modified = false;
        // x*a <= y*b + c
        modified = assertCond(x, y, a, b, c, indent) || modified;
        // y*b <= x*a - c
        modified = assertCond(y, x, b, a, 0-c, indent) || modified;
        
        return modified;
}

/**** Dataflow Functions ****/

// returns the sign of the given variable
// GB : 2011-03-05 (Removing Sign Lattice Dependence) 
/*affineInequality::signs ConstrGraph::getVarSign(const varID& var, string indent)
{
/ *     affineInequality* constrZeroVar = getVal(zeroVar, var);
        affineInequality* constrVarZero = getVal(var, zeroVar);
        
        affineInequality::signs varSign = affineInequality::unknownSgn;

        if(constrZeroVar && constrZeroVar->getC()==0 &&
           constrVarZero && constrVarZero->getC()==0)
                varSign  = affineInequality::eqZero;
        else if(constrZeroVar && constrZeroVar->getC()<=0)
                varSign = affineInequality::posZero;
        else if(constrVarZero && constrVarZero->getC()<=0)
                varSign = affineInequality::negZero;* /
        
        FiniteVarsExprsProductLattice* sgnLattice = getSgnLattice(var, indent+"    ");
        if(sgnLattice)
        {
                SgnLattice* sign = dynamic_cast<SgnLattice*>(sgnLattice->getVarLattice(var));
                if(sign)
                {
                        //Dbg::dbg << "    getVarSign() "<<var.str()<<" : "<<sign->str("")<<"\n";
                        if(sign->getLevel() == SgnLattice::eqZero)
                                return affineInequality::eqZero;
                        else if(sign->getLevel() == SgnLattice::sgnKnown)
                                if(sign->getSgnState() == SgnLattice::posZero)
                                        return affineInequality::posZero;
                                if(sign->getSgnState() == SgnLattice::negZero)  
                                        return affineInequality::negZero;
                }
                / *else
                        Dbg::dbg << "    getVarSign() "<<var.str()<<" : NULL\n";* /
        }
        
        return affineInequality::unknownSgn;
}*/

bool ConstrGraph::isEqZero(const varID& var, string indent)
{
/*      if(var==zeroVar) return true;
        // a divisibility scalar is =0 if its original variable is =0 and the remainder ==0
        if(isDivScalar(var))
        {
                varID origVar = divVar2OrigVar[var];
                DivLattice* d = dynamic_cast<DivLattice*>(divL->getVarLattice(origVar));
                if(d->getLevel() == DivLattice::divKnown)
                        return d->getValue()==0;
        }
        else
        {       
                DivLattice* d = dynamic_cast<DivLattice*>(divL->getVarLattice(var));
                
                if(d->getLevel() == DivLattice::valKnown)
                        return d->getValue()==0;
        }
        return false;*/
        
        /*affineInequality* constrZeroVar = getVal(zeroVar, var);
        affineInequality* constrVarZero = getVal(var, zeroVar);

        if(constrZeroVar && constrZeroVar->getC()==0 &&
           constrVarZero && constrVarZero->getC()==0)
                return true;

        return false;*/
        
        return eqVars(zeroVar, var, indent);
}

// Returns true if v1*a = v2*b + c and false otherwise
bool ConstrGraph::eqVars(const varID& v1, const varID& v2, int a, int b, int c, string indent)
{
        return v1==v2 || (lteVars(v1, v2, a, b, c, indent) && lteVars(v2, v1, b, a, 0-c, indent));
}

// If v1*a = v2*b + c, sets a, b and c appropriately and returns true. 
// Otherwise, returns false.
bool ConstrGraph::isEqVars(const varID& v1, const varID& v2, int& a, int& b, int& c, string indent)
{
        if(v1 == v2) return true;
                
        // If v1*constrV1V2.getA() <= v2*constrV1V2.getB() + constrV1V2.getC() AND
        //    v1*constrV1V2.getA() >= v2*constrV1V2.getB() + constrV1V2.getC()
        affineInequality* constrV1V2 = getVal(v1, v2);
        if(constrV1V2 && lteVars(v2, v1, constrV1V2->getB(), constrV1V2->getA(), 0-constrV1V2->getC()))
        {
                a = constrV1V2->getA();
                b = constrV1V2->getB();
                c = constrV1V2->getC();
                return true;
        }
        else
                return false;
}

// Returns a list of variables that are equal to var in this constraint graph as a list of pairs
// <x, ineq>, where var*ineq.getA() = x*ineq.getB() + ineq.getC()
map<varID, affineInequality> ConstrGraph::getEqVars(varID var, string indent)
{
        map<varID, affineInequality> res;
        for(map<varID, affineInequality>::iterator it=vars2Value[var].begin();
            it!=vars2Value[var].end(); it++)
        {
                // var*a <= x*b + c
                affineInequality& constrVarX = it->second;
                // x*b <= var*a - c
                affineInequality* constrXVar = getVal(it->first, var, indent+"    ");
                if(constrXVar)
                        if(constrVarX.getA() == constrXVar->getB() &&
                           constrVarX.getB() == constrXVar->getA() &&
                           constrVarX.getC() == (0-constrXVar->getC()))
                        {
                                res[it->first] = constrVarX;
                        }
        }
        return res;
}

// Returns true if v1*a <= v2*b + c and false otherwise
bool ConstrGraph::lteVars(const varID& v1, const varID& v2, int a, int b, int c, string indent)
{
        if(v1==v2) return c>=0;
        
        affineInequality* constrV1V2 = getVal(v1, v2, indent+"    ");
        //if(constrV1V2) Dbg::dbg << "lteVars("<<v1<<", "<<v2<<", "<<a<<", "<<b<<", "<<c<<"), constrV1V2="<<constrV1V2->str()<<"\n";
        //else           Dbg::dbg << "lteVars("<<v1<<", "<<v2<<", "<<a<<", "<<b<<", "<<c<<"), constrV1V2=NULL\n";
                
        if(constrV1V2)
                return constrV1V2->getA()==a &&
                       constrV1V2->getB()==b &&
                       // stronger constraints (smaller c) imply looser constraints (larger c)
                       constrV1V2->getC()<=c;
        else
                return false;
}

// Returns true if v1*a < v2*b + c and false otherwise
bool ConstrGraph::ltVars(const varID& v1, const varID& v2, int a, int b, int c, string indent)
{
        return lteVars(v1, v2, a, b, c-1, indent);
}

/*********************************
 **** ConstrGraph::leIterator ****
 *********************************/

// Class used to iterate over all the constraints x*a <= y*b + c for a given variable x
ConstrGraph::leIterator::leIterator(const ConstrGraph* parent,
                                    const map<varID, map<varID, affineInequality> >::iterator& curX)
{
        this->parent = parent;
        this->curX = curX;
}

ConstrGraph::leIterator::leIterator(const ConstrGraph* parent, 
                                    const varID& x)
{
        this->parent = parent;  
        this->x = x;
        curX = parent->vars2Value.find(x);
        if(parent->vars2Value.find(x)!=parent->vars2Value.end())
                curY = curX->second.begin();
}

bool ConstrGraph::leIterator::isDone() const
{
        return curX == parent->vars2Value.end() ||
               curY == curX->second.end();
}

varAffineInequality ConstrGraph::leIterator::operator*() const
{
        ROSE_ASSERT(!isDone());
        varAffineInequality vai(x, curY->first, curY->second);
        return vai;     
}

void ConstrGraph::leIterator::operator ++ ()
{
        if(curX != parent->vars2Value.end())
                curY++;
}

void ConstrGraph::leIterator::operator ++ (int)
{
        if(curX != parent->vars2Value.end())
                curY++;
}

bool ConstrGraph::leIterator::operator==(const leIterator& otherIt) const
{
        return (isDone() && otherIt.isDone()) ||
               (parent == otherIt.parent &&
                x      == otherIt.x      &&
                curX    == otherIt.curX  &&
                curY    == otherIt.curY);
}

bool ConstrGraph::leIterator::operator!=(const leIterator& otherIt) const
{
        return !(*this == otherIt);
}

// Beginning and end points of the iteration over all constraints x*a <= y*b + c for a 
// given variable x.
ConstrGraph::leIterator ConstrGraph::leBegin(const varID& x)
{
        leIterator lei(this, x);
        return lei;
}

ConstrGraph::leIterator ConstrGraph::leEnd()
{
        leIterator lei(this, vars2Value.end());
        return lei;
}
/*********************************
 **** ConstrGraph::geIterator ****
 *********************************/

ConstrGraph::geIterator::geIterator()
{
        isEnd = true;
}

// Class used to iterate over all the constraints x*a <= y*b + c for a given variable y
ConstrGraph::geIterator::geIterator(const ConstrGraph* parent, const varID& y): parent(parent), y(y)
{
        
        isEnd = false;
        curX = parent->vars2Value.begin();
        curY = curX->second.begin();
        /*Dbg::dbg << "geIterator::geIterator()\n";
        Dbg::dbg << "geIterator::geIterator() curX==vars2Value.end() = "<<(curX==parent->vars2Value.end())<<"\n";
        if(curX!=parent->vars2Value.end())
        {
                Dbg::dbg << "geIterator::geIterator() curX="<<curX->first.str()<<" curX.size()="<<curX->second.size()<<" curY==curX->second.end() = "<<(curY==curX->second.end())<<"\n";
                if(curY!=curX->second.end())
                Dbg::dbg << "geIterator::geIterator() curY="<<curY->first.str()<<" curY.ineq="<<curY->second.str()<<"\n";
        }*/
        //if(curY->first != y)
                advance();
}

ConstrGraph::geIterator::geIterator(const ConstrGraph* parent, const varID& y,
           const map<varID, map<varID, affineInequality> >::iterator& curX,
           const map<varID, affineInequality>::iterator& curY): curX(curX), curY(curY), y(y)
{
        this->parent = parent;
        isEnd = false;
}

// Advances curX and curY by one step. Returns false if curX/curY is already at the
// end of parent->vars2Value and true otherwise (i.e. successful step).
bool ConstrGraph::geIterator::step()
{
        if(isEnd) return false;
        // If we've run out of x's, return false because we're done.
        if(curX == parent->vars2Value.end()) return false;
        /*else
        {
                 // Move x and y along until we hit the next non-empty x
                if(curY == curX->second.end())
                {
                        do
                        {
                                // Move on the the beginning of the next x
                                curX++;
                                curY = curX->second.begin();
                        }
                        while(curY == curX->second.end());
                }
                else
                {
                        // Move on to the next y in the current x
                        curY++;
                }*/
                if(curY != curX->second.end())
                {
                        // Move on to the next y in the current x
                        curY++;
                }
        //}
        
        // If we've run out of y's for the current x, advance forward to the next
        // x that has an associated y. If there are none, return false.
        if(curY == curX->second.end())
        {
                do {
                        curX++;
                        if(curX == parent->vars2Value.end()) return false;
                        curY = curX->second.begin();
                } while(curY == curX->second.end());
                return true;
        }
        // If we have another y for the current x, this is it
        else
                return true;
}

// Move curX/curY to the next x/y pair with a matching y (may leave curX/curY already satisfy this).
// Returns true if there are no more such pairs.
bool ConstrGraph::geIterator::advance()
{
        //Dbg::dbg << "geIterator::advance() isEnd="<<isEnd<<" isDone()="<<isDone()<<"\n";
        if(isEnd) return false;
        if(isDone()) return false;
        
        bool notDone=true;
        while(notDone && (curY==curX->second.end() || curY->first != y))
        {
                notDone = step();
                /*if(curX!=parent->vars2Value.end())
                {
                        varAffineInequality vai(getX(), curY->first, curY->second);
                        Dbg::dbg << "advance() "<<vai.str()<<"\n";
                }*/
        }
        
        return notDone;
}

bool ConstrGraph::geIterator::isDone() const
{
        /*if(!isEnd)
        { Dbg::dbg << "geIterator::isDone() isEnd="<<isEnd<<" (curX == parent->vars2Value.end())="<<(curX == parent->vars2Value.end())<<"\n"; }
        if(!isEnd && curX != parent->vars2Value.end())
        { Dbg::dbg << "geIterator::isDone() curX="<<curX->first.str()<<" (curY == curX->second.end())="<<(curY == curX->second.end())<<"\n"; }
        if(!isEnd && curX != parent->vars2Value.end() && curY != curX->second.end())
        { Dbg::dbg << "geIterator::isDone() curY="<<curY->first.str()<<"\n"; }*/
        
        return isEnd || curX == parent->vars2Value.end()/* || curY == curX->second.end()*/;
}

const varID& ConstrGraph::geIterator::getX() const
{
        ROSE_ASSERT(!isDone());
        
        return curX->first;
}

varAffineInequality ConstrGraph::geIterator::operator*() const 
{ 
        ROSE_ASSERT(!isDone());
        varAffineInequality vai(getX(), curY->first, curY->second);
        return vai;
}

void ConstrGraph::geIterator::operator++()
{
        step();
        advance();
}

void ConstrGraph::geIterator::operator++(int)
{
        step();
        advance();
}

bool ConstrGraph::geIterator::operator==(const geIterator& otherIt) const
{
//      Dbg::dbg << "isEnd="<<isEnd<<"  otherIt.isEnd="<<otherIt.isEnd<<"\n";
        return (isEnd == otherIt.isEnd) ||
               (isDone() == otherIt.isDone()) ||
               (parent == otherIt.parent &&
                y      == otherIt.y      &&
                curX   == otherIt.curX   &&
                curY   == otherIt.curY);
}

bool ConstrGraph::geIterator::operator!=(const geIterator& otherIt) const
{
        return !(*this == otherIt);
}

// Beginning and End points of the iteration over all constraints x*a <= y*b + c for a 
// given variable y.
ConstrGraph::geIterator ConstrGraph::geBegin(const varID& y)
{
        geIterator gei(this, y);
        return gei;
}

ConstrGraph::geIterator ConstrGraph::geEnd()
{
        geIterator gei;
        return gei;
}


// Widens this from that and saves the result in this
// returns true if this causes this to change and false otherwise
bool ConstrGraph::widenUpdate(InfiniteLattice* that_arg, string indent)
{
        return widenUpdate_ex(that_arg, false, indent);
}

// Widens this from that and saves the result in this, while ensuring that if a given constraint
// doesn't exist in that, its counterpart in this is not modified
// returns true if this causes this to change and false otherwise
bool ConstrGraph::widenUpdateLimitToThat(InfiniteLattice* that_arg, string indent)
{
        return widenUpdate_ex(that_arg, true, indent);
}

// Common code for widenUpdate() and widenUpdateLimitToThat()
bool ConstrGraph::widenUpdate_ex(InfiniteLattice* that_arg, bool limitToThat, string indent)
{
        ConstrGraph* that = dynamic_cast<ConstrGraph*>(that_arg);
        bool modified = false;
        
        // If this constraint graph is Bottom or Uninitialized, the widening is that
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        if(l.first == uninitialized || l.first==bottom) {
                modified = copyFrom(*that, indent+"    ") || modified;
                
                // Transitively close divisibility information in the context of the divisibility info
                // known at the current DataflowNode
                if(CGdebugTransClosure) Dbg::dbg << indent << "Before Transitive Closure:\n"<<str(indent+"    ")<<"\n";
                modified = transitiveClosure() || modified;
                return modified;
        }
        
        // If that is Bottom or Uninitialized then we don't need to change this
        pair <levels, constrTypes> tl = that->getLevel(true, indent+"    ");
        if(tl.first == uninitialized || tl.first==bottom)
                return false;
        
        // If this is top, then we don't need to change it
        if(tl.first == top)
                return false;
        
        // If that is top 
        if(tl.first==top) {
                // If both graphs are top, there is nothing to be done
                if(l.first==top)
                        return false;
                // If this is not top but that is top, make this top
                else
                        return setToTop(false, indent+"    ");
        }
        
        // This and That must be constrKnown
        
        // If that is inconsistent, this will become inconsistent
        if(tl.first==constrKnown && tl.second==inconsistent)
                return setToInconsistent(indent+"    ");
        // If that is not inconsistent but this is, it will not change
        else if(l.first==constrKnown && l.second==inconsistent) {
                //return false;
                // Transitively close divisibility information in the context of the divisibility info
                // known at the current DataflowNode
                if(CGdebugTransClosure) Dbg::dbg << indent << "Before Transitive Closure:\n"<<str(indent+"    ")<<"\n";
                modified = transitiveClosure() || modified;
                return modified;
        }
        
        // This and That must be constrKnown/(conj / negConj)
        
        // If both this and that are not bottom
        return OrAndWidenUpdate(that, false, true, limitToThat, indent+"    ");
}

// Computes the union of this constraint graph with cg, returning this union
//ConstrGraph *ConstrGraph::unionMe(const ConstrGraph &that)
// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
// The meet is the union of the information from both constraint graphs. If a given variable pair
//    has a constraint in one graph but not the other, the resulting graph has this constraint.
//    If the pair has constraints in both graphs, their constraint in the resulting graph will be 
//    the union of these constraints.
bool ConstrGraph::meetUpdate(Lattice* that_arg, string indent)
{
        return meetUpdate_ex(that_arg, false, indent);
}

// Meet this and that and saves the result in this, while ensuring that if a given constraint
// doesn't exist in that, its counterpart in this is not modified
// returns true if this causes this to change and false otherwise
bool ConstrGraph::meetUpdateLimitToThat(InfiniteLattice* that_arg, string indent)
{
        return meetUpdate_ex(that_arg, true, indent);
}

// Common code for meetUpdate() and meetUpdateLimitToThat()
bool ConstrGraph::meetUpdate_ex(Lattice* that_arg, bool limitToThat, string indent)
{
        ConstrGraph* that = dynamic_cast<ConstrGraph*>(that_arg);
        bool modified = false;
        
        // If this constraint graph is Uninitialized or Bottom, the meet is that
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        pair <levels, constrTypes> tl = that->getLevel(true, indent+"    ");
        
        // If one graph is strictly looser than the other, the union = the looser graph
        if(*that <<= *this)
        {
                if(CGDebugLevel>=1) Dbg::dbg << indent << "(*that &lt;&lt;= *this)\n";
                // this is already the union
                //return false;
                
                // Transitively close divisibility information in the context of the divisibility info
                // known at the current DataflowNode
                if(CGdebugTransClosure) Dbg::dbg << indent << "Before Transitive Closure:\n"<<str(indent+"    ")<<"\n";
                modified = transitiveClosure() || modified;
                return modified;
        }
        else if(*this <<= *that)
        {
                if(CGDebugLevel>=1) Dbg::dbg << indent << "(*this &lt;&lt;= *that)\n";
                Dbg::dbg << indent << "    that="<<that->str(indent + "        ")<<"\n";
                modified = copyFrom(*that, indent+"    ") || modified;
                
                // Transitively close divisibility information in the context of the divisibility info
                // known at the current DataflowNode
                if(CGdebugTransClosure) Dbg::dbg << indent << "Before Transitive Closure:\n"<<str(indent+"    ")<<"\n";
                modified = transitiveClosure() || modified;
                return modified;
        }
        
        // If the two graphs are not strictly related to each other but their levels are such
        //      that they have no stored constraints: levels uninitialized, bottom, constrKnown/inconsistent or top.
        // Note that if only one graph is at this level then the two must be equal to each other
        //    or one is strictly tighter or looser than the other.
        if(l.first==uninitialized || l.first==bottom || (l.first==constrKnown && l.second==inconsistent) || l.first==top)
        {
                ROSE_ASSERT(l == tl);
                return false;
        }
        
        // The two graphs must be constrKnown/(conj or negConj) and are not strictly ordered in information content
        if(CGDebugLevel>=1) Dbg::dbg << indent << "calling OrAndWidenUpdate\n";
        return OrAndWidenUpdate(that, true, true, limitToThat, indent+"    ");
}

// <from LogicalCond>
bool ConstrGraph::orUpd(LogicalCond& that_arg, string indent)
{
        ConstrGraph* that = dynamic_cast<ConstrGraph*>(&that_arg);

        return meetUpdate((Lattice*)that, indent+"    ");
}

// <from LogicalCond>
bool ConstrGraph::andUpd(LogicalCond& that_arg, string indent)
{
        ConstrGraph* that = dynamic_cast<ConstrGraph*>(&that_arg);
        return andUpd(that, indent+"    ");
}

bool ConstrGraph::andUpd(ConstrGraph* that, string indent)
{
        bool modified = false;
        
        // If this constraint graph is Uninitialized or Bottom, the meet is that
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        pair <levels, constrTypes> tl = that->getLevel(true, indent+"    ");
        
        // If one graph is strictly looser than the other, the intersection = the tighter graph
        if(*that <<= *this)
        {
                if(CGDebugLevel>=1) Dbg::dbg << indent << "(*this &lt;&lt;= *that)\n";
                modified = copyFrom(*that, indent+"    ") || modified;
                
                // Transitively close divisibility information in the context of the divisibility info
                // known at the current DataflowNode
                Dbg::dbg << indent << "Before Transitive Closure:\n"<<str(indent+"    ")<<"\n";
                modified = transitiveClosure() || modified;
                return modified;
        }
        else if(*this <<= *that)
        {
                if(CGDebugLevel>=1) Dbg::dbg << indent << "(*that &lt;&lt;= *this)\n";
                // This is already the intersection
                
                // Transitively close divisibility information in the context of the divisibility info
                // known at the current DataflowNode
                Dbg::dbg << indent << "Before Transitive Closure:\n"<<str(indent+"    ")<<"\n";
                modified = transitiveClosure() || modified;
                return modified;
        }
        
        // If the two graphs are not strictly related to each other but their levels are such
        //      that they have no stored constraints: levels uninitialized, bottom, constrKnown/inconsistent or top.
        // Note that if only one graph is at this level then the two must be equal to each other
        //    or one is strictly tighter or looser than the other.
        if(l.first==uninitialized || l.first==bottom || (l.first==constrKnown && l.second==inconsistent) || l.first==top)
        {
                ROSE_ASSERT(l == tl);
                return false;
        }
        
        // The two graphs must be constrKnown/(conj or negConj) and are not strictly ordered in information content
        if(CGDebugLevel>=1) Dbg::dbg << indent << "calling OrAndWidenUpdate\n";
        return OrAndWidenUpdate(that, true, false, false, indent+"    ");
        
/*      // Merge the vars, arrays and divVars sets of the two objects
        for(varIDSet::const_iterator it=that->vars.begin(); it!=that->vars.end(); it++)
                vars.insert(*it);
        for(varIDSet::const_iterator it=that->arrays.begin(); it!=that->arrays.end(); it++)
                arrays.insert(*it);
        for(varIDSet::const_iterator it=that->divVars.begin(); it!=that->divVars.end(); it++)
                divVars.insert(*it);*/
        
        return modified;
}

// Unified function for Or(meet), And and Widening
// If meet == true, this function computes the meet and if =false, computes the widening.
// If OR == true, the function computes the OR of each pair of inequalities and otherwise, computes the AND.
// if limitToThat == true, if a given constraint does not exist in that, this has no effect on the meet/widening
bool ConstrGraph::OrAndWidenUpdate(ConstrGraph* that, bool meet, bool OR, bool limitToThat, string indent)
{
        bool modified = false;
        ostringstream funcName;
        if(CGmeetDebugLevel>0) {
                if(meet) funcName << "meetUpdate() OR="<<OR;
                else     funcName<< "widenUpdate() OR="<<OR;
                Dbg::enterFunc(funcName.str());
                /*Dbg::dbg << indent << "   this: "<<str(indent+"    ")<<"\n";
                Dbg::dbg << indent << "   that: "<<that->str(indent+"    ")<<"\n";*/
        }
        
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        pair <levels, constrTypes> tl = that->getLevel(true, indent+"    ");

        // The calling functions must ensure that both graphs have known constraints that are consistent
        ROSE_ASSERT(l.first==constrKnown && l.first==tl.first && 
                    (l.second==conj  || l.second==negConj) &&
                    (tl.second==conj || tl.second==negConj));
        
        // If the negation status of these graphs is not the same, quit because the meet/widening is too hard
        ROSE_ASSERT(l.second == tl.second);

        /* // If we're computing a weaker approximation of the union of two conjunctions of inequalities:
        // (xy AND yz AND ... ) OR (xy' AND yz' AND ...) => ((xy OR xy') AND (yz OR yz') AND ...) */
        
        map<varID, map<varID, affineInequality> > additionsToThisX;
        
        /*Dbg::dbg << indent<<"vars2Value[x]=\n";
        for(map<varID, map<varID, affineInequality> >::iterator x=vars2Value.begin(); x!=vars2Value.end(); x++)
                Dbg::dbg << indent << "    "<<x->first<<"\n";
        Dbg::dbg << indent<<"that.vars2Value[x]=\n";
        for(map<varID, map<varID, affineInequality> >::iterator x=that->vars2Value.begin(); x!=that->vars2Value.end(); x++)
                Dbg::dbg << indent << "    "<<x->first<<"\n";*/
        
        // Iterate over all constraints in both constraint graphs and union/widen them individually
        //printf("vars2Value.size()=%d, that->vars2Value.size()=%d\n", vars2Value.size(), that->vars2Value.size());
        map<varID, map<varID, affineInequality> >::iterator itThisX, itThatX;
        for(itThisX = vars2Value.begin(), itThatX = that->vars2Value.begin(); 
            itThisX!=vars2Value.end() && itThatX!=that->vars2Value.end(); )
        {
                //Dbg::dbg << indent << "    itThisX = "<<itThisX->first.str()<< "  itThatX = "<<itThatX->first.str()<<" (itThisX->first < itThatX->first)="<<(itThisX->first < itThatX->first)<<"\n";
                
                // If itThisX->first exists in this, but not in that
                if(itThisX->first < itThatX->first)
                        OrAndWidenUpdate_XinThisNotThat(OR, limitToThat, itThisX, modified, indent+"    ");
                // If itThatX->first exists in that, but not in this
                else if(itThisX->first > itThatX->first)
                        OrAndWidenUpdate_XinThatNotThis(OR, limitToThat, that, itThatX, additionsToThisX, modified, indent+"    ");
                // If, itThisX->first exists in both this and that
                else
                {
                        Dbg::dbg << indent << "<span style=\"text-decoration: underline;\">Common Var X=<span style=\"font-weight:bold;\">"<<itThisX->first.str()<<"</span></span>\n";
                        varID x = itThisX->first;
                        /*affineInequality::signs xSign = getVarSign(x, indent+"    ");
                        ROSE_ASSERT(xSign == that->getVarSign(x, indent+"    "));*/
                        map<varID, affineInequality> additionsToThisY;
                        
                        // Union/Widen each <x->???> constraint
                        map<varID, affineInequality>::iterator itThisY, itThatY;
                        for(itThisY = itThisX->second.begin(), itThatY = itThatX->second.begin();
                            itThisY!=itThisX->second.end() && itThatY!=itThatX->second.end(); )
                        {
                                varID y = itThisY->first;
                                //Dbg::dbg << indent << "itThisY = "<<itThisY->first.str()<< "  itThatY = "<<itThatY->first.str()<<"\n";
                                                                
                                // If itThisY->first exists in this, but not in that
                                if(itThisY->first < itThatY->first)
                                        OrAndWidenUpdate_YinThisNotThat(OR, limitToThat, itThisX, itThisY, 
                                                   modified, indent+"    ");
                                // If itThatY->first exists in that, but not in this
                                else if(itThisY->first > itThatY->first)
                                        OrAndWidenUpdate_YinThatNotThis(OR, limitToThat, itThatX, itThatY, 
                                                         additionsToThisY,
                                                         modified, indent+"    ");
                                // else, <itThisX->first -> itThisY->first> exists in both this and that
                                else
                                {
                                        // Union
                                        if(meet)
                                        {
                                                affineInequality tmp = itThisY->second;
                                                //if(l.second == conj)
                                                if(OR)
                                                        // OR this constraint in this with the corresponding constraint in that
                                                        // ax <= by+c OR a'x <= b'y+c'
                                                        modified = itThisY->second.unionUpd(itThatY->second) || modified;
                                                else
                                                        // AND this constraint in this with the corresponding constraint in that
                                                        modified = itThisY->second.intersectUpd(itThatY->second) || modified;
                                                if(CGmeetDebugLevel>0 && modified) {
                                                        Dbg::dbg << indent << "    OrAndWidenUpdate "<<tmp.str(itThisX->first, itThisY->first)<<" && "<<itThatY->second.str(itThisX->first, itThisY->first)<<" =&gt;\n";
                                                        Dbg::dbg << indent << "        "<<itThisY->second.str(itThisX->first, itThisY->first)<<"\n";
                                                }
                                                        
                                                modifiedVars.insert(itThisX->first);  
                                                modifiedVars.insert(itThisY->first);
                                                /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(itThisX->first);
                                                newConstrVars.insert(itThisY->first);*/
                                        }
                                        // Widening
                                        else
                                        {
                                                // widen this constraint in this with the corresponding constraint in that
                                                if(itThisY->second != itThatY->second)
                                                {
                                                        //Dbg::dbg <<itThisX->first.str() << " -> "<<itThisY->first.str()<<"\n";
                                                        //Dbg::dbg <<"itThisY->second = "<<itThisY->second.str()<<"\n";
                                                        //Dbg::dbg <<"itThatY->second = "<<itThatY->second.str()<<"\n";
                                                        // If the new constraint is more relaxed than the old constraint, we immediately
                                                        //    jump the constraint to top (the most relaxed constraint) because the lattice is 
                                                        //    infinite and if we consistently choose to widen to the constraint to a looser one,
                                                        //    we may end up doing this infinitely many times.
                                                        if((/*l.second==conj*/OR    && /*itThisY->second.semLessThan(itThatY->second, isEqZero(x), isEqZero(y))*/
                                                                                              itThisY->second!=itThatY->second && itThisY->second.semLessThan(itThatY->second, 
                                                                                                                          x==zeroVar?NULL:getVal(x, zeroVar), x==zeroVar?NULL:getVal(zeroVar, x), 
                                                                                                                          y==zeroVar?NULL:getVal(y, zeroVar), y==zeroVar?NULL:getVal(zeroVar, y), indent+"    ")) ||
                                                           (/*l.second==conjNg*/!OR && itThisY->second.semLessThanNeg(itThatY->second, isEqZero(x), isEqZero(y))))
                                                        {
                                                                /* // Before we remove this constraint, first check if the constraint between x and y's 
                                                                // divisibility variables has changed. If one exists and it hasn't changed, simply
                                                                // copy over the x-y constraint from that to this, since the divisibility variables
                                                                // represent the same information as the regular variables
                                                                varID divX = ConstrGraph::getDivVar(x);
                                                                varID divY = ConstrGraph::getDivVar(y);
                                                                affineInequality* thisDivXY = getVal(divX, divY);
                                                                affineInequality* thatDivXY = that->getVal(divX, divY);
                                                                
                                                                if(thisDivXY && thatDivXY && 
                                                                   thisDivXY->getLevel()!=affineInequality::top && 
                                                                   thatDivXY->getLevel()!=affineInequality::top &&
                                                                   thisDivXY == thatDivXY)
                                                                {
                                                                        itThisY->second = itThatY->second;
                                                                }
                                                                else*/
                                                                Dbg::dbg << indent << "OrAndWidenUpdate widening itThisY "<<itThisY->first<<" to Top"<<endl;
                                                                modified = itThisY->second.setToTop() || modified;
                                                                modifiedVars.insert(itThisX->first);
                                                                modifiedVars.insert(itThisY->first);
                                                        }
                                                        //Dbg::dbg <<"itThisY->second ^ itThatY->second = "<<itThisY->second.str()<<"\n";
                                                }
                                        }
                                        
                                        itThisY++;
                                        itThatY++;
                                }
                        }
                        
                        // For all x->y constraints in This that is not in That
                        while(itThisY!=itThisX->second.end())
                                OrAndWidenUpdate_YinThisNotThat(OR, limitToThat, itThisX, itThisY, 
                                                     modified, indent+"    ");
                        
                        // For all x->y constraints in That that is not in This
                        while(itThatY!=itThatX->second.end())
                                OrAndWidenUpdate_YinThatNotThis(OR, limitToThat, itThatX, itThatY, 
                                                      additionsToThisY,
                                                      modified, indent+"    ");
                        
                        // Add all the new mappings in additionsToThisY to This. We're guaranteed that if a variable
                        // is mapped by additionsToThisY, it is not mapped by vars2Value[iterX->first]
                        for(map<varID, affineInequality>::iterator iterY=additionsToThisY.begin(); iterY!=additionsToThisY.end(); iterY++) {
                                ROSE_ASSERT(itThisX->second.find(iterY->first) == itThisX->second.end());
                                itThisX->second.insert(*iterY);
                        }
                                        
                        //Dbg::dbg << "pre-increment, itThisX==vars2Value.end()="<<(itThisX==vars2Value.end())<<" && itThatX==that->vars2Value.end()="<<(itThatX==that->vars2Value.end())<<"\n";
                        itThisX++;
                        itThatX++;
                }
                //Dbg::dbg << "bottom, itThisX==vars2Value.end()="<<(itThisX==vars2Value.end())<<" && itThatX==that->vars2Value.end()="<<(itThatX==that->vars2Value.end())<<"\n";
        }
        
//Dbg::dbg << indent << "   this loop end: "<<str(indent+"    ")<<"\n";
//Dbg::dbg << indent << "   modified = "<<modified<<"\n";
        
        // For all x constraints in This that is not in That
        while(itThisX!=vars2Value.end())
                OrAndWidenUpdate_XinThisNotThat(OR, limitToThat, itThisX, modified, indent+"    ");

        // For all x constraints in That that is not in This
        while(itThatX!=that->vars2Value.end())
                OrAndWidenUpdate_XinThatNotThis(OR, limitToThat, that, itThatX, additionsToThisX, modified, indent+"    ");
        
        // Add all the new mappings in additionsToThisX to This. We're guaranteed that if a variable
        // is mapped at the first level by additionsToThisX, it is not mapped at the first level by vars2Value
        for(map<varID, map<varID, affineInequality> >::iterator iterX=additionsToThisX.begin(); iterX!=additionsToThisX.end(); iterX++) {
                ROSE_ASSERT(vars2Value.find(iterX->first) == vars2Value.end());
                vars2Value.insert(*iterX);
        }
        
        // Close if the widening will cause this graph to change
        if (modified)
        {
                constrChanged = true;
                if(CGmeetDebugLevel>0) Dbg::dbg << indent << "Before Transitive Closure:\n"<<str(indent+"    ")<<"\n";
                transitiveClosure();
        }
        
/*      // Merge the vars, arrays and divVars sets of the two objects
        for(varIDSet::const_iterator it=that->vars.begin(); it!=that->vars.end(); it++)
                vars.insert(*it);
        for(varIDSet::const_iterator it=that->divVars.begin(); it!=that->divVars.end(); it++)
                divVars.insert(*it);*/
        
        /*Dbg::dbg << "OrAndWidenUpdate vars = ";
        for(varIDSet::iterator it=vars.begin(); it!=vars.end(); it++)
        { Dbg::dbg << (*it).str() << ", "; }
        Dbg::dbg << "\n";*/
        
//Dbg::dbg << "   this final: "<<str("")<<"\n";
//Dbg::dbg << "   modified = "<<modified<<"\n";
        if(CGmeetDebugLevel>0) Dbg::exitFunc(funcName.str());
        return modified;
}

// Portion of OrAndWidenUpdate that deals with x variables for which there exist x->y mapping 
// in This but not in That. Increments itThisX and updates modified and modifiedVars in case this 
// function modifies the constraint graph.
void ConstrGraph::OrAndWidenUpdate_XinThisNotThat(
                                    bool OR, bool limitToThat, 
                                    map<varID, map<varID, affineInequality> >::iterator& itThisX, bool& modified,
                                    string indent)
{
        if(!limitToThat)
        {
                // Do we need separate Union and Widening cases? This should not happen in a loop situation
                //if(ct == conj) {
                if(OR) {
                        // new: old_constraint OR bottom = bottom and bottom can be represented as an unmapped x->y pair
                        Dbg::dbg << indent << "X="<<itThisX->first<<" in This, not That (OR="<<OR<<", limitToThat="<<limitToThat<<endl;
                        itThisX->second.clear();
                        modifiedVars.insert(itThisX->first);
                        modified = true;
                } else {
                        // new: old_constraint AND bottom = constraint, so no need to change this
                }
        }
        itThisX++;      
}

// Portion of OrAndWidenUpdate that deals with x variables for which there exist x->y mapping 
// in That but not in This. Increments itThisX and updates modified and modifiedVars in case this 
// function modifies the constraint graph.
// additionsToThis - Records the new additions to vars2Value that need to be made after we are done iterating 
//      over it. It guaranteed that the keys mapped by the first level of additionsToThis are not mapped
//      at the first level by vals2Value.
void ConstrGraph::OrAndWidenUpdate_XinThatNotThis(
                                    bool OR, bool limitToThat, 
                                    ConstrGraph* that,
                                    map<varID, map<varID, affineInequality> >::iterator& itThatX, 
                                    map<varID, map<varID, affineInequality> >& additionsToThis, 
                                    bool& modified, string indent)
{
        Dbg::dbg << indent << "X="<<itThatX->first<<" in That, not This (OR="<<OR<<", limitToThat="<<limitToThat<<endl;

        // Ignore variables that are not mapped in This because they're not live at the DataflowNode of this constraint graph
        if(vars.find(itThatX->first)==vars.end() /* noDivVars  && divVars.find(itThatX->first)==divVars.end()*/) { itThatX++; return; }
        
        //if(ct == conj) {
        if(OR) {
// noDivVars            // new: Bottom OR that.constraint = Bottom, so leave x->* constraint in This as Bottom
// noDivVars            // NOT CERTAIN THAT THIS IS VALID
// noDivVars            // If X is a divisibility variable for a variable that does exist in This, copy its constraints over, 
// noDivVars            // since the only reason why we don't already have constraints for x is because they would have been too tight.
// noDivVars            if(isDivVar(itThatX->first)) {
// noDivVars                    pair<varID, bool> p = divVar2Var(itThatX->first);
// noDivVars                    if(p.second) {
// noDivVars                            for(map<varID, affineInequality>::iterator itThatY=itThatX->second.begin(); itThatY!=itThatX->second.end(); itThatY++) {
// noDivVars                                    // Ignore variables that are not mapped in This because they're not live at the DataflowNode of this constraint graph
// noDivVars                                    if(vars.find(itThatY->first)==vars.end() && divVars.find(itThatY->first)==divVars.end()) continue;
// noDivVars                                    additionsToThis[itThatX->first].insert(*itThatY);
// noDivVars                            }
// noDivVars                            modifiedVars.insert(itThatX->first);
// noDivVars                            modified = true;
// noDivVars                    }
// noDivVars            }
        } else {
                Dbg::dbg << indent << "OrAndWidenUpdate_XinThatNotThis(OR="<<OR<<", limitToThat="<<limitToThat<<", itThatX="<<itThatX->first<<endl;
                // new: Bottom AND that.constraint = constraint, so copy constraint from That to This
                //vars2Value[itThatX->first] = that->vars2Value[itThatX->first];
                // Copy all the x <= y constraints from That where y is a variable mapped in This
                for(map<varID, affineInequality>::iterator itThatY=itThatX->second.begin(); itThatY!=itThatX->second.end(); itThatY++) {
                        // Ignore variables that are not mapped in This because they're not live at the DataflowNode of this constraint graph
                        if(vars.find(itThatY->first)==vars.end()/* noDivVars && divVars.find(itThatY->first)==divVars.end()*/) continue;
                        additionsToThis[itThatX->first].insert(*itThatY);
                }
                modifiedVars.insert(itThatX->first);
                modified = true;
        }
        itThatX++;
}

// Portion of OrAndWidenUpdate that deals with x->y pairs for which there exist x->y mapping 
// in This but not in That. Increments itThisX and updates modified and modifiedVars in case this 
// function modifies the constraint graph.
void ConstrGraph::OrAndWidenUpdate_YinThisNotThat(
                                    bool OR, bool limitToThat, 
                                    map<varID, map<varID, affineInequality> >::iterator& itThisX,
                                    map<varID, affineInequality>::iterator& itThisY, 
                                    bool& modified, string indent)
{
        /*// do nothing, since all <itThisX->first -> itThisY->first> constraints in that are assumed to be bottom*/
        // Only bother with this case if we've been asked to
        if(!limitToThat)
        {
                // Do we need separate Union and Widening cases? This should not happen in a loop situation
                //if(ct == conj) {
                if(OR) {
                        Dbg::dbg << indent << "X="<<itThisX->first<<" Y="<<itThisY->first<<" in This, not That (OR="<<OR<<", limitToThat="<<limitToThat<<endl;
                        // new: old_constraint OR Bottom = Bottom, and bottom can be represented as an unmapped x->y pair or 
                        //      by just setting the constraint to Bottom
                        modified = itThisY->second.setToBottom() || modified;
                        modifiedVars.insert(itThisY->first);
                        modifiedVars.insert(itThisX->first);
                        // !!! above should be a removal
                        modified = true;
                } else {
                        // new: old_constraint AND bottom = constraint, so no need to change this
                }
        }
        
        itThisY++;
}

// Portion of OrAndWidenUpdate that deals with x->y pairs for which there exist x->y mapping 
// in That but not in This. Increments itThisX and updates modified and modifiedVars in case this 
// function modifies the constraint graph.
void ConstrGraph::OrAndWidenUpdate_YinThatNotThis(
                                    bool OR, bool limitToThat, 
                                    map<varID, map<varID, affineInequality> >::iterator& itThatX,
                                    map<varID, affineInequality>::iterator& itThatY, 
                               map<varID, affineInequality>& additionsToThis, 
                                    bool& modified, string indent)
{
        // Ignore variables that are not mapped in this because they're not live at the DataflowNode of this constraint graph
        if(vars.find(itThatY->first)==vars.end()/* noDivVars && divVars.find(itThatY->first)==divVars.end()*/) { itThatY++; return; }
        
        //if(ct == conj) {
        if(OR) {
                // new: Bottom OR that.constraint = Bottom, so leave x->y constraint in This as Bottom
                
// noDivVars            // NOT CERTAIN THAT THIS IS VALID
// noDivVars            // If Y is a divisibility variable for a variable that does exist in This, copy its constraints over, 
// noDivVars            // since the only reason why we don't already have constraints for y is because they would have been too tight.
// noDivVars            if(isDivVar(itThatY->first)) {
// noDivVars                    pair<varID, bool> p = divVar2Var(itThatY->first);
// noDivVars                    if(p.second) {
// noDivVars                            additionsToThis.insert(*itThatY);
// noDivVars                            modifiedVars.insert(itThatX->first);
// noDivVars                            modified = true;
// noDivVars                    }
// noDivVars            }
        } else {
                Dbg::dbg << indent << "X="<<itThatX->first<<" Y="<<itThatY->first<<" in That, not This (OR="<<OR<<", limitToThat="<<limitToThat<<")"<<endl;
                // new: Bottom AND that.constraint = constraint, so copy constraint from That to This
                additionsToThis.insert(*itThatY);
                modifiedVars.insert(itThatX->first);
                modified = true;
        }
        itThatY++;
}

// Computes the transitive closure of the given constraint graph, and updates the graph to be that transitive closure. 
// Returns true if this causes the graph to change and false otherwise.
bool ConstrGraph::transitiveClosure(string indent)
{
        int numSteps=0, numInfers=0, numFeasibleChecks=0, numLocalClosures=0;
        struct timeval startTime, endTime;
        if(CGprofileLevel>=1)
        {
                Dbg::dbg << "transitiveClosure() {{{\n";
                gettimeofday(&startTime, NULL);
        }
        // don't take transitive closures in the middle of a transaction
        if(inTransaction) return false;
        
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        // This constraint graph will now definitely be initialized
        if(l.first==uninitialized) return setToBottom(indent+"    ");
                
        // Don't do anything if this lattice has no explicit constraints
        if(l.first==uninitialized || l.first==bottom || l.first==top ||
                (l.first==constrKnown && l.second==inconsistent))
                return false;
        
if(CGdebugTransClosure) {
        Dbg::enterFunc("Beginning transitive closure");
        Dbg::dbg << indent << "    Pre-closure: \n" << str("    ") << "\n";
}
        bool modified = false;
        bool iterModified = true;
        
        // !!! Not using divVars for now                modified = transitiveClosureDiv(indent+"    ");
        
        // First, compute the transitive closure of all variables. This determines
        // all the constraints on the current program state. 
        // If this is a normal conjunction of inequalities, we'll be performing inferences 
        //    through inequalities of the form ax <= by+c.      
        // If this is a negated conjunction, we will not perform any inferences since a negated
        //    conjunction is just a disjunction of negated inequalities and you can't infer
        //    anything from ax > by+x OR dy > ez+f since they may not both hold at the same time.
        
        if(l.first==constrKnown && l.second==negConj)
                return false;
        
        if(CGdebugTransClosure) {
                Dbg::dbg << "    modifiedVars = [";
                for(varIDSet::iterator itX = modifiedVars.begin(); itX!=modifiedVars.end(); ) {
                        Dbg::dbg << *itX;
                        itX++;
                        if(itX!=modifiedVars.end())
                                Dbg::dbg << ", ";
                }
                Dbg::dbg << "]" << endl;
        }
        
        // Iterate until a fixed point is reached
        while (iterModified)
        {
                iterModified = false;
                // Iterate through every triple of variables x, y, z, where x is a variable that constraints
                //    of which have changed and y and z are any variables.
                // Infer from ax <= by+c and dy <= ez+f something about gx <= hz+i
                //for ( mIter1 = vars2Name.begin(); mIter1 != vars2Name.end(); mIter1++ )
                /* GB 2011-06-02 : newConstrVars->modifiedVars : for(varIDSet::iterator itX = newConstrVars.begin(); itX!=newConstrVars.end(); itX++) */
                for(varIDSet::iterator itX = modifiedVars.begin(); itX!=modifiedVars.end(); itX++)
                {
//Dbg::dbg << "itX = "<<itX->str()<<"\n";
                        varID x = *itX;

                        for(varIDSet::iterator itY = vars.begin(); itY!=vars.end(); itY++)
                                ConstrGraph::transitiveClosureY(x, *itY, modified, numSteps, numInfers, iterModified, indent);
/* noDivVars                    for(varIDSet::iterator itY = divVars.begin(); itY!=divVars.end(); itY++)
                                ConstrGraph::transitiveClosureY(x, *itY, modified, numSteps, numInfers, iterModified, indent);*/
                        ConstrGraph::transitiveClosureY(x, zeroVar, modified, numSteps, numInfers, iterModified, indent);
                }

                //numFeasibleChecks++;
                //// look for cycles
                //quad r = checkSelfConsistency();
                //if(r!=1) break;
                modified = iterModified || modified;
                if(CGdebugTransClosure) {
                        Dbg::dbg << indent << "Post_closure: modified="<<modified<<" iterModified="<<iterModified<<"\n";
                        Dbg::dbg << indent << str(indent) <<"\n";
                        Dbg::dbg.flush();
                }
        }
        //numFeasibleChecks++;
        // look for cycles
        //quad r = checkSelfConsistency();
        
        // Reset the variable modification state
        modifiedVars.clear();
        /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.clear(); */

        // Check for self-consistency
        modified = checkSelfConsistency() || modified;
        if(CGdebugTransClosure) {
                Dbg::dbg << indent << "    Constraints at the end of full transitiveClosure\n";
                Dbg::dbg << indent << str(indent+"    ") << "\n";
        }
/*
#ifdef DEBUG_FLAG
        Dbg::dbg << "    Ending transitive closure\n";
#endif
        return r;*/
        if(CGprofileLevel>=1) 
        {
                gettimeofday(&endTime, NULL);
                Dbg::dbg << indent << "transitiveClosure() }}} numSteps="<<numSteps<<", numInfers="<<numInfers<<", numFeasibleChecks="<<numFeasibleChecks<<", numLocalClosures="<<numLocalClosures<<", numVars="<<vars.size()<<", time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n";
        }


if(CGdebugTransClosure) {
        Dbg::exitFunc("Beginning transitive closure");
}       
        return modified;
}

bool ConstrGraph::transitiveClosureDiv(string indent)
{
        bool modified=false;
        
        for(set<varID>::iterator x=vars.begin(); x!=vars.end(); x++)
                for(set<varID>::iterator y=vars.begin(); y!=vars.end(); y++)
                {
                        // If both x and y are currently constants but their divisibility level is divKnown, this means that 
                        // the inferred constraint will correspond to their difference as constants, whereas by the time we 
                        // reach a fixed point we'll realize that these variables are not constants. In fact, odds are that
                        // both variables are iterators, since the most common way to be stably divisible by a constant is to
                        // be continually incremented by this constant in iterations of some loop nest. The divisibility analysis
                        // has already figured this out since it has already reached its fixed point. We'll now use it in this
                        // analysis to set the x->y constraint to reflect this information. In particular, we'll assume that
                        // the most appropriate relationship between x=d_x * i + r_x and y=d_y * j + r_y is that in a given 
                        // iteration both are incremented by d_x and d_y respectively. Other relationships are possible and are 
                        // consistent with their divisibility information and their current values. However, this is the one
                        // that is most common and most useful for our current purposes.
                        int aX, bX, cX, aY, bY, cY;
                        Dbg::dbg << indent << "ConstrGraph::transitiveClosureDiv x="<<x<<" y="<<y<<"\n";
                        DivLattice* divLX = dynamic_cast<DivLattice*>(getDivLattice(*x, indent+"    ")->getVarLattice(*x));
                        DivLattice* divLY = dynamic_cast<DivLattice*>(getDivLattice(*y, indent+"    ")->getVarLattice(*y));
                        if(divLX && divLY &&  
                                divLX->getLevel()==DivLattice::divKnown && divLY->getLevel()==DivLattice::divKnown &&
                                isEqVars(*x, zeroVar, aX, bX, cX, indent+"    ") && isEqVars(*y, zeroVar, aY, bY, cY, indent+"    "))
                        {
                                ROSE_ASSERT(aX==1 && bX==1 && aY==1 && bY==1);
                                affineInequality inferredXY;
                                // this: x = cX
                                // this: x = cY
                                // divLX: x=d_x * i + r_x
                                // divLX: y=d_y * j + r_y
                                // inferredXY => d_y * x = d_x * y + q
                                //      d_y * cX - d_x * cY = q
                                inferredXY.set(divLY->getDiv(), divLX->getDiv(), divLY->getDiv() * cX - divLX->getDiv() * cY);
                                if(getVal(*x, *y)) Dbg::dbg << indent << "transitiveClosureDiv() Current="<<getVal(*x, *y)->str(*x, *y, indent+"    ")<<" inferredXY="<<inferredXY.str(*x, *y, indent+"    ")<<"\n";
                                else               Dbg::dbg << indent << "transitiveClosureDiv() Current=NONE inferredXY="<<inferredXY.str(*x, *y, indent+"    ")<<"\n";
                                
                                if(getVal(*x, *y)==NULL) Dbg::dbg << indent << "    No Original Constraint. Setting.\n";
                                else Dbg::dbg << indent << "    semLessThan ="<<getVal(*x, *y)->semLessThan(inferredXY, getVal(*x, zeroVar), getVal(zeroVar, *x), 
                                                                           getVal(*y, zeroVar), getVal(zeroVar, *y), indent+"            ")<<"\n";
                                // If either no x->y constraint is currently recorded or 
                                //   there is one but the inferred constraint is not looser than the previous one, 
                                //   replace the original with the inferred
                                if(getVal(*x, *y)==NULL || 
                                   !getVal(*x, *y)->semLessThan(inferredXY, getVal(*x, zeroVar), getVal(zeroVar, *x), 
                                                                            getVal(*y, zeroVar), getVal(zeroVar, *y), indent+"            ")) {
                                        modified = setVal(*x, *y, inferredXY) || modified;
                                }
                        }
                }
        return modified;
}

void ConstrGraph::transitiveClosureY(const varID& x, const varID& y, bool& modified, int& numSteps, int& numInfers, bool& iterModified, string indent)
{
// noDivVars    varID divY = getDivVar(y);
        
        // if x and y are different variables and they're not both constants
        //    (we don't want to do inference on constants since we can't learn anything more 
        //     about them and we might lose information because we're being conservative)
/*                              // and one is not the other's divisibility variable*/
        if(x != y && ((x!=zeroVar && x!=oneVar) || (y!=zeroVar && y!=oneVar))
           /*&& x!=divY && y!=divX*/)
        {
//Dbg::dbg << "itY = "<<itY->str()<<"\n";
                //quad xy = getVal(x, y);
                //affineInequality* constrXY = NULL;
                // skip the rest if there is no x->y constraint
                //if(constrXY)
                {
                        //for ( mIter3 = vars2Name.begin(); mIter3 != vars2Name.end(); mIter3++ )
                        for(varIDSet::iterator itZ = vars.begin(); itZ!=vars.end(); itZ++)
                                ConstrGraph::transitiveClosureZ(x, y, *itZ, modified, numSteps, numInfers, iterModified, indent);
/* noDivVars                    for(varIDSet::iterator itZ = divVars.begin(); itZ!=divVars.end(); itZ++)
                                ConstrGraph::transitiveClosureZ(x, y, *itZ, modified, numSteps, numInfers, iterModified, indent);*/
                        ConstrGraph::transitiveClosureZ(x, y, zeroVar, modified, numSteps, numInfers, iterModified, indent);
                }
        }       
}

void ConstrGraph::transitiveClosureZ(const varID& x, const varID& y, const varID& z, bool& modified, int& numSteps, int& numInfers, bool& iterModified, string indent)
{
        numSteps++;

        affineInequality* constrXY = NULL;
//Dbg::dbg << "itZ = "<<itZ->str()<<"\n";
        // x, y and z are three different variables and x and y are not each other divisibility variables (we don't want to find new constraints for these)
// noDivVars    varID divX = getDivVar(x);
// noDivVars    varID divY = getDivVar(y);
        if(z!=x && z!=y) // noDivVars && x!=divY && y!=divX)
        {
                if(!constrXY) constrXY = getVal(x, y);
                        
                affineInequality* constrXZ = getVal(x, z);
                affineInequality* constrZY = getVal(z, y);
                /*if(constrXZ) Dbg::dbg << "                  "<<x.str()<<"-&gt;"<<z.str()<<" = " << constrXZ->str(x, z, "") << "\n";
                if(constrZY) Dbg::dbg << "                  "<<z.str()<<"-&gt;"<<y.str()<<" = " << constrZY->str(z, y, "") << "\n";*/
                // if the x->z->y path results in a tighter constraint than the
                // x->y path, update the latter to the former
                //if ( xz != INF && zy != INF )
                if(constrXZ && constrXZ->getLevel()==affineInequality::constrKnown && 
                        constrZY && constrZY->getLevel()==affineInequality::constrKnown)
                {
                        numInfers++;
                        affineInequality inferredXY(*constrXZ, *constrZY/*, x==zeroVar, y==zeroVar, 
                                                    dynamic_cast<DivLattice*>(divL->getVarLattice(x)), 
                                                    dynamic_cast<DivLattice*>(divL->getVarLattice(y)), z*/);
                        //affineInequality *constrXY = getVal(x, y);
                        
//printf("transitiveClosure() constrXY=%p\n", &inferredXY, constrXY);

                        // If there doesn't exist an x-y constraint in the graph, add it
                        if(!constrXY)
                        {
                                //if(CGdebugTransClosure) {
                                //      //Dbg::dbg << "    " << x.str() << "-&gt;" << z.str() << " -&gt;" << y.str() << "\n";
                                //      Dbg::dbg << indent << "        Current = None\n";
                                //      //Dbg::dbg << "        Inferred = " << inferredXY.str(x, y, "") << "\n";
                                //}
                                Dbg::dbg << indent << "    " << x << "-&gt;" << z << " -&gt;" << y << "\n";
                                Dbg::dbg << indent << "        Inferred Fresh = " << inferredXY.str(x, y, "") << " from "<<constrXZ->str(x, z, "")<<" and "<<constrZY->str(z, y, "")<<"\n";

                                setVal(x, y, inferredXY);
                                constrXY = getVal(x, y);
                                iterModified = true;
                        }
                        // else, if the inferred x-y constraint it strictly tighter than the current x-y constraint
                        //else if(inferredXY.semLessThan(*constrXY, isEqZero(x), isEqZero(y)))
                        else if(inferredXY!=*constrXY)
                        {
                                //int a, b, c;
                                //Dbg::dbg << indent << "    isEqVars(x, zeroVar)="<<isEqVars(x, zeroVar, a, b, c, indent+"    ")<<" isEqVars(y, zeroVar)="<<isEqVars(y, zeroVar, a, b, c, indent+"    ")<<"\n";
                                // True if x and y are constants and false otherwise
                                //bool xyConst = isEqVars(x, zeroVar, a, b, c, indent+"    ") && isEqVars(y, zeroVar, a, b, c, indent+"    ");
                                // If both x and y are constants, use semantic <=
                                if(/*(xyConst && 
                                         inferredXY.semLessThanEq(*constrXY, x==zeroVar, x==zeroVar?NULL:getVal(x, zeroVar), x==zeroVar?NULL:getVal(zeroVar, x), 
                                                                                 y==zeroVar, y==zeroVar?NULL:getVal(y, zeroVar), y==zeroVar?NULL:getVal(zeroVar, y), indent+"            ")) ||
                                   // Otherwise, use semantic <
                                   (!xyConst && 
                                        */ inferredXY.semLessThan(*constrXY, x==zeroVar?NULL:getVal(x, zeroVar), x==zeroVar?NULL:getVal(zeroVar, x), 
                                                                             y==zeroVar?NULL:getVal(y, zeroVar), y==zeroVar?NULL:getVal(zeroVar, y), indent+"            ")/*)*/) {
                                        if(CGdebugTransClosure) {
                                                //Dbg::dbg << indent << "    " << x.str() << "-&gt;" << z.str() << " -&gt;" << y.str() << "\n";
                                                //Dbg::dbg << indent << "        Setting " << constrXY->str(x, y, "") << " =&gt; "<<inferredXY.str(x, y, "")<<"\n";
                                                //Dbg::dbg << indent << "        Inferred = " << inferredXY.str(x, y, "") << "\n
                                                Dbg::dbg << indent << "    " << x << "-&gt;" << z << " -&gt;" << y << "\n";
                                                Dbg::dbg << indent << "        Inferred Tighter = " << inferredXY.str(x, y, "") << " from "<<constrXZ->str(x, z, "")<<" and "<<constrZY->str(z, y, "")<<"\n";
                                        }
                                        // Replace the current x-y constraint with the inferred one
                                        constrXY->set(inferredXY);
                                        iterModified = true;
                                }
                        }
                }
//if(CGdebugTransClosure) Dbg::dbg << indent << "    iterModified="<<iterModified<<"\n";
        }
}

// Computes the transitive closure of the given constraint graph, focusing on the constraints 
//    of vars that have divisibility variables. We only bother propagating constraints to each such 
//    variable through its divisibility variable.
// Returns true if this causes the graph to change and false otherwise.
// noDivVars bool ConstrGraph::divVarsClosure(string indent)
// noDivVars {
// noDivVars    int numVarClosures=0, numFeasibleChecks=0;
// noDivVars    struct timeval startTime, endTime;
// noDivVars    if(CGprofileLevel>=1)
// noDivVars    {
// noDivVars            Dbg::dbg << indent << "divVarsClosure() {{{\n";
// noDivVars            gettimeofday(&startTime, NULL);
// noDivVars    }
// noDivVars    
// noDivVars    // don't take transitive closures in the middle of a transaction
// noDivVars    if(inTransaction) return false;
// noDivVars    
// noDivVars    pair <levels, constrTypes> l = getLevel(true, indent+"    ");
// noDivVars 
// noDivVars    // This constraint graph will now definitely be initialized
// noDivVars    if(l.first==uninitialized) return setToBottom(indent+"    ");
// noDivVars    
// noDivVars    if(CGdebugTransClosure) {
// noDivVars            Dbg::dbg << indent << "Beginning divVarsClosure\n";
// noDivVars            Dbg::dbg << indent << "    Pre-closure: \n" << str("    ") << "\n";
// noDivVars    }
// noDivVars    bool modified = false;
// noDivVars    bool iterModified=true;
// noDivVars            
// noDivVars    // Iterate until a fixed point is reached or the constraint graph is discovered to be inconsistent
// noDivVars    while(iterModified && !(l.first==constrKnown && l.second==inconsistent))
// noDivVars    {
// noDivVars            iterModified = false;
// noDivVars            // Iterate through every triple of variables
// noDivVars            for(varIDSet::iterator itX = vars.begin(); itX!=vars.end(); itX++)
// noDivVars            {
// noDivVars                    varID x = *itX;
// noDivVars                    varID divX = getDivVar(x);
// noDivVars                    affineInequality* constrXDivX = getVal(x, divX);
// noDivVars                    affineInequality* constrDivXX = getVal(divX, x);
// noDivVars                    
// noDivVars                    //affineInequality::signs xSign = getVarSign(x, indent+"    ");
// noDivVars                    
// noDivVars                    for(varIDSet::iterator itY = vars.begin(); itY!=vars.end(); itY++)
// noDivVars                    {
// noDivVars                            varID y = *itY;
// noDivVars                            ROSE_ASSERT(divX != y);
// noDivVars                            //affineInequality::signs ySign = getVarSign(y, indent+"    ");
// noDivVars                            numVarClosures++;
// noDivVars                            iterModified = divVarsClosure_perY(x, divX, y, constrXDivX, constrDivXX/*, xSign, ySign*/, indent+"    ") || iterModified;
// noDivVars                    }
// noDivVars                    
// noDivVars                    /*for(varIDSet::iterator itY = arrays.begin(); itY!=arrays.end(); itY++)
// noDivVars                    {
// noDivVars                            varID y = *itY;
// noDivVars                            iterModified = divVarsClosure_perY(x, divX, y, constrXDivX, constrDivXX, indent+"    ") || iterModified;
// noDivVars                    }*/
// noDivVars            }
// noDivVars 
// noDivVars            numFeasibleChecks++;
// noDivVars            // look for cycles
// noDivVars            iterModified = checkSelfConsistency() || iterModified;
// noDivVars            modified = iterModified || modified;
// noDivVars            l = getLevel(true, indent+"    ");
// noDivVars    }
// noDivVars    if(CGprofileLevel>=1) 
// noDivVars    {
// noDivVars            gettimeofday(&endTime, NULL);
// noDivVars            Dbg::dbg << indent << "divVarsClosure() }}} numVarClosures="<<numVarClosures<<", numFeasibleChecks="<<numFeasibleChecks<<", numVars="<<vars.size()<<", time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n";
// noDivVars    }
// noDivVars    
// noDivVars    return modified;
// noDivVars }
// noDivVars 
// noDivVars // The portion of divVarsClosure that is called for every y variable. Thus, given x and x' (x's divisibility variable)
// noDivVars // divVarsClosure_perY() is called for every scalar or array y to infer the x->y connection thru x->x'->y and
// noDivVars // infer the y->x connection thru y->x'->x
// noDivVars bool ConstrGraph::divVarsClosure_perY(const varID& x, const varID& divX, const varID& y, 
// noDivVars                                       affineInequality* constrXDivX, affineInequality* constrDivXX/*,
// noDivVars                                       affineInequality::signs xSign, affineInequality::signs ySign*/, string indent)
// noDivVars {
// noDivVars    if(CGdebugTransClosure) Dbg::dbg << indent << "divVarsClosure_perY(x="<<x<<", divX="<<divX<<", y="<<y<<", constrXDivX="<<constrXDivX<<", constrDivXX="<<constrDivXX<<")\n";
// noDivVars    
// noDivVars    int numInfers=0;
// noDivVars    struct timeval startTime, endTime;
// noDivVars    if(CGprofileLevel>=1)
// noDivVars    {
// noDivVars            Dbg::dbg << indent << "divVarsClosure_perY("<<x.str()<<", "<<divX.str()<<", "<<y.str()<<") {{{\n";
// noDivVars            gettimeofday(&startTime, NULL);
// noDivVars    }
// noDivVars    bool modified = false;
// noDivVars    
// noDivVars    // if x and y are different variables
// noDivVars    if(x != y)
// noDivVars    {
// noDivVars            // Infer an x <= y constraint through x <= divX <= y
// noDivVars            {
// noDivVars                    affineInequality* constrDivXY = getVal(divX, y);
// noDivVars                    affineInequality* constrXY = getVal(x, y);
// noDivVars                    
// noDivVars                    // if the x->divX->y path results in a tighter constraint than the
// noDivVars                    // x->y path, update the latter to the former
// noDivVars                    /*if(!constrDivXY) Dbg::dbg << indent << "    constrDivXY="<<constrDivXY<<"\n";
// noDivVars                    else             Dbg::dbg << indent << "    constrDivXY="<<constrDivXY->str(indent+"    ")<<"\n";*/
// noDivVars                    
// noDivVars                    if(constrDivXY && constrDivXY->getLevel()==affineInequality::constrKnown)
// noDivVars                    {
// noDivVars                            affineInequality inferredXY(*constrXDivX, *constrDivXY/*, x==zeroVar, y==zeroVar, getVarSign(x, indent+"    "), getVarSign(y, indent+"    ")*/);
// noDivVars                            
// noDivVars                            numInfers++;
// noDivVars                    
// noDivVars                            // if there doesn't exist an x-y constraint in the graph, add it
// noDivVars                            if(!constrXY)
// noDivVars                            {
// noDivVars                                    if(CGdebugTransClosure) {
// noDivVars                                            Dbg::dbg << indent << "    dvc(x-&gt;x'-&gt;y): " << x << "-&gt;" << y << "\n";
// noDivVars                                            Dbg::dbg << indent << "    dvc(x-&gt;x'-&gt;y):     Current = None\n";
// noDivVars                                            Dbg::dbg << indent << "    dvc(x-&gt;x'-&gt;y):     Inferred("<<x<<"-&gt;"<<divX<<"-&gt;"<<y<<") = " << inferredXY.str(x, y, "") << "\n";
// noDivVars                                    }
// noDivVars                                    setVal(x, y, inferredXY);
// noDivVars                                    modified = true;
// noDivVars                            }
// noDivVars                            // Else, if the inferred x-y constraint it strictly tighter than the current x-y constraint
// noDivVars                            //else if(inferredXY.semLessThan(*constrXY, isEqZero(x), isEqZero(y)))
// noDivVars                            else if(inferredXY!=*constrXY && inferredXY.semLessThan(*constrXY, x==zeroVar?NULL:getVal(x, zeroVar), x==zeroVar?NULL:getVal(zeroVar, x), 
// noDivVars                                                                                          y==zeroVar?NULL:getVal(y, zeroVar), y==zeroVar?NULL:getVal(zeroVar, y), indent+"    "))
// noDivVars                            {
// noDivVars                                    if(CGdebugTransClosure) {
// noDivVars                                            Dbg::dbg << indent << "    dvc(x-&gt;x'-&gt;y): " << x << "-&gt;" << y << "\n";
// noDivVars                                            Dbg::dbg << indent << "    dvc(x-&gt;x'-&gt;y):     Current = " << constrXY->str(x, y, "") << "\n";
// noDivVars                                            Dbg::dbg << indent << "    dvc(x-&gt;x'-&gt;y):     Inferred ("<<x<<"-&gt;"<<divX<<"-&gt;"<<y<<") = " << inferredXY.str(x, y, "") << "\n";
// noDivVars                                    }
// noDivVars                                    // Replace the current x-y constraint with the inferred one
// noDivVars                                    constrXY->set(inferredXY);
// noDivVars                                    modified = true;
// noDivVars                            }
// noDivVars                    }
// noDivVars            }
// noDivVars            
// noDivVars            // Infer an y <= x constraint through y <= divX <= x
// noDivVars            {
// noDivVars                    affineInequality* constrYDivX = getVal(y, divX);
// noDivVars                    affineInequality* constrYX = getVal(y, x);
// noDivVars                    
// noDivVars                    numInfers++;
// noDivVars                    
// noDivVars                    // If the y->divX->x path results in a tighter constraint than the
// noDivVars                    // y->x path, update the latter to the former
// noDivVars                    /*if(!constrYDivX) Dbg::dbg << indent << "    constrYDivX="<<constrYDivX<<"\n";
// noDivVars                    else             Dbg::dbg << indent << "    constrYDivX="<<constrYDivX->str(indent+"    ")<<"\n";*/
// noDivVars                    if(constrYDivX && constrYDivX->getLevel()==affineInequality::constrKnown)
// noDivVars                    {
// noDivVars                            affineInequality inferredYX(*constrYDivX, *constrDivXX/*, y==zeroVar, x==zeroVar, getVarSign(y, indent+"    "), getVarSign(x, indent+"    ")*/);
// noDivVars                    
// noDivVars                            // If there doesn't exist an y-x constraint in the graph, add it
// noDivVars                            if(!constrYX)
// noDivVars                            {
// noDivVars                                    if(CGdebugTransClosure) {
// noDivVars                                            Dbg::dbg << indent << "    dvc(y-&gt;x'-&gt;x): " << y.str() << "-&gt;" << x.str() << "\n";
// noDivVars                                            Dbg::dbg << indent << "    dvc(y-&gt;x'-&gt;x):     Current = None\n";
// noDivVars                                            Dbg::dbg << indent << "    dvc(y-&gt;x'-&gt;x):     Inferred("<<y.str()<<"-&gt;"<<divX.str()<<"-&gt;"<<x.str()<<") = " << inferredYX.str(y, x, "") << "\n";
// noDivVars                                    }
// noDivVars                                    setVal(y, x, inferredYX);
// noDivVars                                    modified = true;
// noDivVars                            }
// noDivVars                            // Else, if the inferred y-x constraint it strictly tighter than the current y-x constraint
// noDivVars                            //else if(inferredYX.semLessThan(*constrYX, isEqZero(y), isEqZero(x)))
// noDivVars                            else if(inferredYX!=*constrYX && inferredYX.semLessThan(*constrYX, x==zeroVar?NULL:getVal(x, zeroVar), x==zeroVar?NULL:getVal(zeroVar, x), 
// noDivVars                                                                                          y==zeroVar?NULL:getVal(y, zeroVar), y==zeroVar?NULL:getVal(zeroVar, y), indent+"    "))
// noDivVars                            {
// noDivVars                                    if(CGdebugTransClosure) {
// noDivVars                                            Dbg::dbg << indent << "    dvc(y-&gt;x'-&gt;x): " << y.str() << "-&gt;" << x.str() << "\n";
// noDivVars                                            Dbg::dbg << indent << "    dvc(y-&gt;x'-&gt;x):     Current = " << constrYX->str(x, y, "") << "\n";
// noDivVars                                            Dbg::dbg << indent << "    dvc(y-&gt;x'-&gt;x):     Inferred ("<<y.str()<<"-&gt;"<<divX.str()<<"-&gt;"<<x.str()<<") = " << inferredYX.str(y, x, "") << "\n";
// noDivVars                                    }
// noDivVars                                    // replace the current y-x constraint with the inferred one
// noDivVars                                    constrYX->set(inferredYX);
// noDivVars                                    modified = true;
// noDivVars                            }
// noDivVars                    }
// noDivVars            }
// noDivVars    }
// noDivVars    if(CGprofileLevel>=1)
// noDivVars    {
// noDivVars            gettimeofday(&endTime, NULL);
// noDivVars            Dbg::dbg << indent << "divVarsClosure_perY("<<x.str()<<", "<<divX.str()<<", "<<y.str()<<") }}} numInfers="<<numInfers<<", numVars="<<vars.size()<<", time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n";
// noDivVars    }
// noDivVars    
// noDivVars    return modified;
// noDivVars }

// Computes the transitive closure of this constraint graph while modifying 
// only the constraints that involve the given variable
// Returns true if this causes the graph to change and false otherwise.
bool ConstrGraph::localTransClosure(const varID& tgtVar, string indent)
{
        // don't take transitive closures in the middle of a transaction
        if(inTransaction) return false;
        
        int numSteps=0, numInfers=0, numFeasibleChecks=0;
        struct timeval startTime, endTime;
        if(CGdebugTransClosure) Dbg::enterFunc("localTransClosure("+tgtVar.str()+")");
        //cout << "A"<<endl;
        if(CGprofileLevel>=1)
                gettimeofday(&startTime, NULL);
        
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        //cout << "B"<<endl;

        // This constraint graph will now definitely be initialized
        if(l.first==uninitialized) return setToBottom(indent+"    ");
        
        if(CGdebugTransClosure) {
                //cout << "C"<<endl;
                Dbg::dbg << indent << "    Beginning local closure("<<tgtVar.str()<<")\n";
                Dbg::dbg << indent << "    " << str(indent+"        ") << "\n";
        }

        bool modified = false;
        bool iterModified=true;
        
        // iterate until a fixed point is reached
        while (iterModified)
        {
                iterModified = false;
                // iterate through every pair of variables
                //for ( mIter1 = vars2Name.begin(); mIter1 != vars2Name.end(); mIter1++ )
                //for(varIDSet::iterator itX = vars.begin(); itX!=vars.end(); itX++)
                /* GB 2011-06-02 : newConstrVars->modifiedVars : for(varIDSet::iterator itX = newConstrVars.begin(); itX!=newConstrVars.end(); itX++) */
                for(varIDSet::iterator itX = modifiedVars.begin(); itX!=modifiedVars.end(); itX++)
                {
                        //quad x = mIter1->first;
                        varID x = *itX;
                        //affineInequality::signs xSign = getVarSign(x, indent+"    ");
                        
                        //for ( mIter2 = vars2Name.begin(); mIter2 != vars2Name.end(); mIter2++ )
                        for(varIDSet::iterator itY = vars.begin(); itY!=vars.end(); itY++)
                        {
                                //quad y = mIter2->first;
                                varID y = *itY;
                                //affineInequality::signs ySign = getVarSign(y, indent+"    ");
                                
                                // if tgtVar, x and y are three different variables
                                if(x!=y && tgtVar!=x && tgtVar!=y)
                                {
                                        //quad xy = getVal(x, y);
                                        affineInequality* constrXY = getVal(x, y);
                                        //if(CGdebugTransClosure) {
                                        //      if(constrXY) Dbg::dbg << indent << "                  "<<x.str()<<"-&gt;"<<y.str()<<" = " << constrXY->str(x, y, "") << "\n";
                                        //      else Dbg::dbg << indent << "                  "<<x.str()<<"-&gt;"<<y.str()<<" = None\n";
                                        //}
                                        
                                        if(constrXY && constrXY->getLevel()==affineInequality::constrKnown)
                                        {
                                                // examine the constraint chain tgtVar->x->y
                                                {
                                                        affineInequality* constrTgtX = getVal(tgtVar, x);
                                                        //if(CGdebugTransClosure) {
                                                        //      if(constrTgtX) Dbg::dbg << indent << "                      "<<tgtVar.str()<<"-&gt;"<<x.str()<<" = " << constrTgtX->str(tgtVar, x, "") << "\n";
                                                        //      else Dbg::dbg << indent << "                       "<<tgtVar.str()<<"-&gt;"<<x.str()<<" = None\n";
                                                        //}
                                                        numSteps++;
                                                        
                                                        if(constrTgtX && constrTgtX->getLevel()==affineInequality::constrKnown)
                                                        {
                                                                affineInequality inferredTgtY(*constrTgtX, *constrXY/*, tgtVar==zeroVar, y==zeroVar, affineInequality::posZero, getVarSign(y, indent+"    "),
                                                                                              dynamic_cast<DivLattice*>(divL->getVarLattice(tgtVar)), 
                                                                                                   dynamic_cast<DivLattice*>(divL->getVarLattice(y)), x*/);
                                                                affineInequality* constrTgtY = getVal(tgtVar, y);
                                                                
                                                                numInfers++;
                                                                
                                                                // if there doesn't exist a constraint between tgtVar and y, add it
                                                                if(!constrTgtY)
                                                                {
                                                                        setVal(tgtVar, y, inferredTgtY);
                                                                        iterModified = true;
                                                                        
                                                                        if(CGdebugTransClosure) {
                                                                                Dbg::dbg << indent << "    " << tgtVar.str() << "-&gt;" << y.str() << "\n";
                                                                                Dbg::dbg << indent << "        Current = None\n";
                                                                                Dbg::dbg << indent << "        Inferred ("<<tgtVar.str()<<"-&gt;"<<x.str()<<"-&gt;"<<y.str()<<") = " << inferredTgtY.str(tgtVar, y, "") << " from constrTgtX="<<constrTgtX->str(tgtVar, x, "")<<" and constrXY="<<constrXY->str(x, y, "")<<"\n";
                                                                        }
                                                                }
                                                                // if the inferred tgtVar-y constraint is strictly tighter than the current tgtVar-y constraint
                                                                else if(inferredTgtY!=*constrTgtY && inferredTgtY.semLessThan(*constrTgtY, tgtVar==zeroVar?NULL:getVal(tgtVar, zeroVar), tgtVar==zeroVar?NULL:getVal(zeroVar, tgtVar), 
                                                                      y==zeroVar?NULL:getVal(y, zeroVar),           y==zeroVar?NULL:getVal(zeroVar, y), indent+"    "))
                                                                {
                                                                        if(CGdebugTransClosure) {
                                                                                Dbg::dbg << indent << "    " << tgtVar.str() << "-&gt;" << y.str()<< "\n";
                                                                                Dbg::dbg << indent << "        Current = " << constrTgtY->str(tgtVar, y, "") << "\n";
                                                                                Dbg::dbg << indent << "        Inferred ("<<tgtVar.str()<<"-&gt;"<<x.str()<<"-&gt;"<<y.str()<<")= " << inferredTgtY.str(tgtVar, y, "") << " from constrTgtX="<<constrTgtX->str(tgtVar, x, "")<<" and constrXY="<<constrXY->str(x, y, "")<<"\n";
                                                                        }
                                                                        // replace the current constraint with the inferred one
                                                                        constrTgtY->set(inferredTgtY);
                                                                        iterModified = true;                    
                                                                }
                                                        }
        
/*                                                      quad tgtX = getVal(tgtVar, x);
                                                        quad tgtY = getVal(tgtVar, y);
                                                        
                                                        // if the tgtVar->x->y path results in a tighter constraint than the
                                                        // tgtVar->y path, update the latter to the former
                                                        if ( xy != INF && tgtX != INF && tgtX + xy < tgtY )
                                                        {
        #ifdef DEBUG_FLAG_TC
                                                                Dbg::dbg << "    " << vars2Name[tgtVar] << "---" << vars2Name[y] << " = " << tgtY << " <- "
                                                                                 << tgtX << " + " << xy << "  through "<<vars2Name[x]<<"\n";
        #endif
                                                                                setVal(tgtVar, y, tgtX + xy);
                                                                                // remember that the fixed point has not been reached yet
                                                                                iterModified = true;
                                                        }*/
                                                }
                                                // examine the constraint chain x->y->tgtVar
                                                {
                                                        affineInequality* constrYTgt = getVal(y, tgtVar);
#ifdef DEBUG_FLAG_TC
                                                        //if(constrYTgt) Dbg::dbg << indent << "                      "<<y.str()<<"-&gt;"<<tgtVar.str()<<" = " << constrYTgt->str(y, tgtVar, "") << "\n";
                                                        //else Dbg::dbg << indent << "                       "<<y.str()<<"-&gt;"<<tgtVar.str()<<" = None\n";
#endif
        
                                                        if(constrYTgt && constrYTgt->getLevel()==affineInequality::constrKnown)
                                                        {
                                                                affineInequality inferredXTgt(*constrXY, *constrYTgt/*, x==zeroVar, tgtVar==zeroVar, getVarSign(x, indent+"    "), affineInequality::posZero,
                                                                                              dynamic_cast<DivLattice*>(divL->getVarLattice(x)), 
                                                                                                   dynamic_cast<DivLattice*>(divL->getVarLattice(tgtVar)), y*/);
                                                                affineInequality* constrXTgt = getVal(x, tgtVar);
                                                                numInfers++;
                                                                                                                    
                                                                // if there doesn't exist a constraint between x and tgtVar, add it
                                                                if(!constrXTgt)
                                                                {
                                                                        setVal(x, tgtVar, inferredXTgt);
                                                                        iterModified = true;
                                                                        
#ifdef DEBUG_FLAG_TC
                                                                        Dbg::dbg << indent << "    " << x.str() << "-&gt;" << tgtVar.str() << "\n";
                                                                        Dbg::dbg << indent << "        Current = None\n";
                                                                        Dbg::dbg << indent << "        Inferred ("<<x.str()<<"-&gt;"<<y.str()<<"-&gt;"<<tgtVar.str()<<") = " << inferredXTgt.str(x, tgtVar, "") << " from constrXY="<<constrXY->str(x, y, "")<<" and constrYTgt="<<constrYTgt->str(y, tgtVar, "")<<"\n";
#endif
                                                                }
                                                                // if the inferred x-tgtVar constraint is strictly tighter than the current x-tgtVar constraint
                                                                //else if(inferredXTgt.semLessThan(*constrXTgt, isEqZero(x), false))
                                                                else if(inferredXTgt!=*constrXTgt && inferredXTgt.semLessThan(*constrXTgt, y==zeroVar?NULL:getVal(x, zeroVar),           y==zeroVar?NULL:getVal(zeroVar, x), 
                                                                      tgtVar==zeroVar?NULL:getVal(tgtVar, zeroVar), tgtVar==zeroVar?NULL:getVal(zeroVar, tgtVar), indent+"    "))
                                                                {
#ifdef DEBUG_FLAG_TC
                                                                        Dbg::dbg << indent << "    " << x.str() << "-&gt;" << tgtVar.str()<< "\n";
                                                                        Dbg::dbg << indent << "        Current = " << constrXTgt->str(x, tgtVar, "") << "\n";
                                                                        Dbg::dbg << indent << "        Inferred ("<<x.str()<<"-&gt;"<<y.str()<<"-&gt;"<<tgtVar.str()<<") = " << inferredXTgt.str(x, tgtVar, "") << " from constrXY="<<constrXY->str(x, y, "")<<" and constrYTgt="<<constrYTgt->str(y, tgtVar, "")<<"\n";
#endif
                                                                        // replace the current constraint with the inferred one
                                                                        constrXTgt->set(inferredXTgt);
                                                                        iterModified = true;                                    
                                                                }
                                                        }
                                                        
/*                                                      quad yTgt = getVal(y, tgtVar);
                                                        quad xTgt = getVal(x, tgtVar);
                                                        
                                                        // if the x->y->tgtVar path results in a tighter constraint than the
                                                        // x->tgtVar path, update the latter to the former
                                                        if ( xy != INF && yTgt != INF && xy + yTgt< xTgt )
                                                        {
                                                                if(CGdebugTransClosure) Dbg::dbg << "    " << vars2Name[x] << "---" << vars2Name[tgtVar] << " = " << xTgt << " <- " << xy << " + " << yTgt << "  through "<<vars2Name[y]<<"\n";
                                                                                setVal(x, tgtVar, xy+yTgt);
                                                                                // remember that the fixed point has not been reached yet
                                                                                iterModified = true;
                                                        }*/
                                                }
                                                
                                                // update tgtVarSign if either x or y are zeroVar (this can be made more precide to cut down on the number of calls to getVarSign
                                                /*if(x==zeroVar || y==zeroVar)
                                                        tgtVarSign = getVarSign(tgtVar, indent+"    ");*/
                                        }
                                }
                        }                               
                }
                
                //numFeasibleChecks++;
                //// look for any cycles that go through arrays
                //quad r = checkSelfConsistency();
                //if(r!=1) break;
                
                modified = iterModified || modified;
                
                
                if(CGdebugTransClosure) {
                        Dbg::dbg << indent << "    Local closure("<<tgtVar.str()<<"): modified = "<<modified<<"\n";
                        Dbg::dbg << indent << "    " << str(indent+"        ") << "\n";
                }
        }
        numFeasibleChecks++;
        
        // look for any cycles that go through arrays
        //quad r = checkSelfConsistency();
        //if(r!=1) break;
                
        if(CGprofileLevel>=1)
        {
                gettimeofday(&endTime, NULL);
                Dbg::dbg << indent << "localTransClosure("<<tgtVar.str()<<") }}} numSteps="<<numSteps<<", numInfers="<<numInfers<<", numFeasibleChecks="<<numFeasibleChecks<<", numVars="<<vars.size()<<", time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n";
        }
        if(CGdebugTransClosure) Dbg::exitFunc("localTransClosure("+tgtVar.str()+")");
        
        return modified;
}

// Searches this constraint graph for negative cycles, which indicates that the constraints represented
//    by the graph are not self-consistent (the code region where the graph holds is unreachable). Modifies
//    the level of this graph as needed.
// Returns true if this call caused a modification in the graph and false otherwise.
bool ConstrGraph::checkSelfConsistency(string indent)
{
        int numConsistenChecks=0, numInconsistentSteps=0;
        struct timeval startTime, endTime;
        
        // This constraint graph will now definitely be initialized
        if(level==uninitialized) { return setToBottom(indent+"    "); }
        
        // Only bother checking consistency if we know the constraints and they corresponds to 
        // a conjunction. If the graph corresponds to a disjunction (negation of a conjunction), 
        // we won't get an inconsistency.
        if(!(level==constrKnown && constrType==conj)) return false;
                
        // Don't do self-consistency checks in the middle of a transaction
        if(inTransaction) return false;
        
        if(CGprofileLevel>=1 || CGDebugLevel>=1)
        {
                Dbg::dbg << "checkSelfConsistency() {{{\n";
                gettimeofday(&startTime, NULL);
        }
        
//printf("checkSelfConsistency()\n");
//Dbg::dbg << "checkSelfConsistency() constrChanged="<<constrChanged<<"\n";

        // Loop through all the pairs of variables
        for(varIDSet::iterator itX = vars.begin(); itX!=vars.end(); itX++)
                for(varIDSet::iterator itY = vars.begin(); itY!=vars.end(); itY++)
                {
                        varID x = *itX;
                        varID y = *itY;
                        // Only focus on pairs of distinct variables
                        if(x == y) continue;
                        
                        affineInequality* constrXY = getVal(x, y);
                        affineInequality* constrYX = getVal(y, x);
                        
                        if(constrXY && constrXY->getLevel()==affineInequality::constrKnown && 
                                constrYX && constrYX->getLevel()==affineInequality::constrKnown)
                        {
                                numConsistenChecks++;
                                // If there is a negative cycle that goes through x and y
                                if(!affineInequality::mayConsistent(*constrXY, *constrYX))
                                {
                                        numInconsistentSteps++;
                                        if(CGDebugLevel>=1) {
                                                Dbg::enterFunc("!!!NOT CONSISTENT!!!");
                                                Dbg::dbg << indent << "Inconsistent: X:"<< x.str() << " -&gt; Y:" << y.str() <<"\n";
                                                Dbg::dbg << indent << "    "<<x.str()<<"=&gt;"<<y.str()<<" = "<<constrXY->str("")<<"\n";
                                                Dbg::dbg << indent << "    "<<y.str()<<"=&gt;"<<x.str()<<" = "<<constrYX->str("")<<"\n";
                                                Dbg::dbg << indent << "CG = "<<str("", false)<<"\n";
                                                Dbg::exitFunc("!!!NOT CONSISTENT!!!");
                                        }
                                        
                                        // Since there is a negative cycle through non-array variables, 
                                        // the constraint graph is top
                                        setToInconsistent(indent+"    ");
                                        
                                        // Reset constrChanged since the state of the constraint graph now 
                                        // reflects its consistency status
                                        constrChanged = false;
                                        
                                        if(CGprofileLevel>=1)
                                        {
                                                gettimeofday(&endTime, NULL);
                                                Dbg::dbg << "checkSelfConsistency() }}} numConsistenChecks="<<numConsistenChecks<<", numInconsistentSteps="<<numInconsistentSteps<<", numVars="<<vars.size()<<", time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n";
                                        }
                                        
                                        return true;
                                }
                        }
                }

        if(CGprofileLevel>=1 || CGDebugLevel>=1)
        {
                gettimeofday(&endTime, NULL);
                Dbg::dbg << indent << "checkSelfConsistency() }}} numConsistenChecks="<<numConsistenChecks<<", numInconsistentSteps="<<numInconsistentSteps<<", numVars="<<vars.size()<<", time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n";
        }
                                                                
        // this constraint graph is not bottom
        return false;
}

// Creates a divisibility variable for the given variable and adds it to the constraint graph
// If var = r (mod d), then the relationship between x and x' (the divisibility variable)
// will be x = x'*d + r
// returns true if this causes the constraint graph to be modified (it may not if this 
//    information is already in the graph) and false otherwise
// noDivVars bool ConstrGraph::addDivVar(varID var/*, int div, int rem*/, bool killDivVar, string indent)
// noDivVars {
// noDivVars    bool modified = false;
// noDivVars    
// noDivVars    FiniteVarsExprsProductLattice* divLattice = getDivLattice(var, indent+"    ");
// noDivVars    //Dbg::dbg << indent << "addDivVar() divLattice="<<divLattice<<", killDivVar="<<killDivVar<<"\n";
// noDivVars    if(divLattice)
// noDivVars    {
// noDivVars            varID divVar = getDivVar(var);
// noDivVars            // Add the important constraints (other constraints will be recomputed during transitive closure)
// noDivVars            DivLattice* varDivL = dynamic_cast<DivLattice*>(divLattice->getVarLattice(var));
// noDivVars            
// noDivVars            // Only bother if we have usable divisibility information for this variable
// noDivVars            if(varDivL && (varDivL->getLevel() == DivLattice::divKnown || varDivL->getLevel() == DivLattice::valKnown))
// noDivVars            {
// noDivVars                    // Record that this constraint graph contains divisibility information for var
// noDivVars                    //divVars.insert(divVar);
// noDivVars                    //divVar2OrigVar[divVar] = var;
// noDivVars            
// noDivVars                    // First, disconnect the divisibility variable from all other variables
// noDivVars                    if(killDivVar)
// noDivVars                            modified = eraseVarConstrNoDiv(divVar, true, indent+"    ") || modified;
// noDivVars                    
// noDivVars                    // Incorporate this variable's divisibility information (if any)
// noDivVars                    if(varDivL->getLevel() == DivLattice::divKnown)
// noDivVars                    {
// noDivVars                            modified = setVal(var, divVar, 1, varDivL->getDiv(), varDivL->getRem(), // GB : 2011-03-05 (Removing Sign Lattice Dependence) getVarSign(var, indent+"    "), getVarSign(var, indent+"    "), 
// noDivVars                                              indent+"    ") || modified;
// noDivVars                            modified = setVal(divVar, var, varDivL->getDiv(), 1, 0-varDivL->getRem(), //GB : 2011-03-05 (Removing Sign Lattice Dependence) getVarSign(var, indent+"    "), getVarSign(var, indent+"    "), 
// noDivVars                                              indent+"    ") || modified;
// noDivVars                    }
// noDivVars                    /*else if(varDivL->getLevel() != DivLattice::bottom)
// noDivVars                    {
// noDivVars                            modified = setVal(var, divVar, 1, 1, 0, getVarSign(var, indent+"    "), getVarSign(var, indent+"    "), indent+"    ") || modified;
// noDivVars                            modified = setVal(divVar, var, 1, 1, 0, getVarSign(var, indent+"    "), getVarSign(var, indent+"    "), indent+"    ") || modified;
// noDivVars                    }*/
// noDivVars                    
// noDivVars                    divVars.insert(divVar);
// noDivVars            }
// noDivVars            /*else
// noDivVars                    printf("WARNING: No divisibility info for variable %s in function %s!\n", var.str().c_str(), func.get_name().str());*/
// noDivVars    }
// noDivVars    
// noDivVars    return modified;
// noDivVars }

// Disconnect this variable from all other variables except its divisibility variable. This is done 
// in order to compute the original variable's relationships while taking its divisibility information 
// into account.
// Returns true if this causes the constraint graph to be modified and false otherwise
// noDivVars bool ConstrGraph::disconnectDivOrigVar(varID var/*, int div, int rem*/, string indent)
// noDivVars {
// noDivVars    bool modified = false;
// noDivVars    //Dbg::dbg << indent << "#divL="<<divL.size()<<"\n";
// noDivVars    FiniteVarsExprsProductLattice* divLattice = getDivLattice(var, "");
// noDivVars    //Dbg::dbg << indent << "disconnectDivOrigVar("<<var<<") divLattice="<<divLattice<<"\n";
// noDivVars    if(divLattice)
// noDivVars    {
// noDivVars            varID divVar = getDivVar(var);
// noDivVars            DivLattice* varDivL = dynamic_cast<DivLattice*>(divLattice->getVarLattice(var));
// noDivVars            
// noDivVars            // Only bother if we have usable divisibility information for this variable
// noDivVars            if(varDivL && (varDivL->getLevel() == DivLattice::divKnown || varDivL->getLevel() == DivLattice::valKnown))
// noDivVars            {
// noDivVars                    // record that this constraint graph constains divisibility information for var
// noDivVars                    //divVars.insert(divVar);
// noDivVars                    
// noDivVars                    // First, disconnect var from all vars but only if var is conntected to its divisibility variable
// noDivVars                    ROSE_ASSERT(!((getVal(var, divVar)!=NULL) ^ (getVal(divVar, var)!=NULL)));
// noDivVars                    if(getVal(var, divVar) && getVal(divVar, var))
// noDivVars                            modified = eraseVarConstrNoDivVars(var, true, indent+"    ") || modified;
// noDivVars                            
// noDivVars                    // Add the important constraints (other constraints will be recomputed during transitive closure).
// noDivVars                    // We don't update modified since we're assuming that these constraints were in the constraint
// noDivVars                    // graph before the eraseVarConstr() call, since they should have been added by the preceding 
// noDivVars                    // addDivVar() call.
// noDivVars    
// noDivVars                    //Dbg::dbg << indent << "disconnectDivOrigVar() var="<<var<<" divVar="<<divVar<<" varDivL="<<varDivL<<"\n";
// noDivVars                    //Dbg::dbg << indent << "DivL="<<divLattice->str(indent+"    ")<<"\n";
// noDivVars                    
// noDivVars                    // incorporate this variable's divisibility information (if any)
// noDivVars                    if(varDivL->getLevel() == DivLattice::divKnown)
// noDivVars                    {
// noDivVars                            modified = setVal(var, divVar, 1, varDivL->getDiv(), varDivL->getRem(), //GB : 2011-03-05 (Removing Sign Lattice Dependence)  getVarSign(var, indent+"    "), getVarSign(var, indent+"    "), 
// noDivVars                                              indent+"    ") || modified;
// noDivVars                            modified = setVal(divVar, var, varDivL->getDiv(), 1, 0-varDivL->getRem(), //GB : 2011-03-05 (Removing Sign Lattice Dependence) getVarSign(var, indent+"    "), getVarSign(var, indent+"    "), 
// noDivVars                                              indent+"    ") || modified;
// noDivVars                    }
// noDivVars                    /*else if(varDivL->getLevel() != DivLattice::bottom)
// noDivVars                    {
// noDivVars                            modified = setVal(var, divVar, 1, 1, 0, getVarSign(var, indent+"    "), getVarSign(var, indent+"    "), indent+"    ") || modified;
// noDivVars                            modified = setVal(divVar, var, 1, 1, 0, getVarSign(var, indent+"    "), getVarSign(var, indent+"    "), indent+"    ") || modified;
// noDivVars                    }*/
// noDivVars            }
// noDivVars    }
// noDivVars    
// noDivVars    return modified;
// noDivVars }

// Finds the variable within this constraint graph that corresponds to the given divisibility variable.
//    If such a variable exists, returns the pair <variable, true>.
//    Otherwise, returns <???, false>.
// noDivVars pair<varID, bool> ConstrGraph::divVar2Var(const varID& divVar, string indent)
// noDivVars {
// noDivVars    for(set<varID>::iterator var=vars.begin(); var!=vars.end(); var++)
// noDivVars            if(getDivVar(*var) == divVar)
// noDivVars                    return make_pair(*var, true);
// noDivVars    return make_pair(zeroVar, false);
// noDivVars }

// Adds a new divisibility lattice, with the associated anotation
// Returns true if this causes the constraint graph to be modified and false otherwise
bool ConstrGraph::addDivL(FiniteVarsExprsProductLattice* divLattice, string annotName, void* annot, string indent)
{
        bool modified = false;
        pair<string, void*> divLAnnot(annotName, annot);
        map<pair<string, void*>, FiniteVarsExprsProductLattice*>::iterator loc = divL.find(divLAnnot);
        // If we already have a divisibility lattice associated with the given annotation
        if(loc != divL.end())
        {
                // Update the mapping
                modified = loc->second != divLattice;
                loc->second = divLattice;
        }
        else
        {
                // Create a new mapping
                modified = true;
                divL[divLAnnot] = divLattice;
        }
        
        return modified;
}

// Adds a new sign lattice, with the associated anotation
// Returns true if this causes the constraint graph to be modified and false otherwise
// GB : 2011-03-05 (Removing Sign Lattice Dependence) 
/*bool ConstrGraph::addSgnL(FiniteVarsExprsProductLattice* sgnLattice, string annotName, void* annot, string indent)
{
        bool modified = false;
        pair<string, void*> sgnLAnnot(annotName, annot);
        map<pair<string, void*>, FiniteVarsExprsProductLattice*>::iterator loc = sgnL.find(sgnLAnnot);
        // If we already have a divisibility lattice associated with the given annotation
        if(loc != sgnL.end())
        {
                // Update the mapping
                modified = loc->second != sgnLattice;
                loc->second = sgnLattice;
        }
        else
        {
                // Create a new mapping
                modified = true;
                sgnL[sgnLAnnot] = sgnLattice;
        }
        
        return modified;
}*/

/**** State Accessor Functions *****/

// Returns true if this constraint graph includes constraints for the given variable
// and false otherwise
bool ConstrGraph::containsVar(const varID& var, string indent)
{
        // First check if there are any var <= x constraints
        if(vars2Value.find(var) != vars2Value.end())
                return true;
        else
        {
                // Now look for x <= var constraints
                for(map<varID, map<varID, affineInequality> >::iterator itX = vars2Value.begin();
                    itX!=vars2Value.end(); itX++)
                {
                        if(itX->second.find(var) != itX->second.end())
                                return true;
                }
        }
        return false;
}

// returns the x->y constraint in this constraint graph
affineInequality* ConstrGraph::getVal(varID x, varID y, string indent)
{
        if(x == y)
                return NULL;

        map<varID, map<varID, affineInequality> >::iterator xIt = vars2Value.find(x);
        // we don't have constraints from x
        if(xIt == vars2Value.end()) return NULL;
        
        map<varID, affineInequality>::iterator yIt = xIt->second.find(y);
        // we don't have an x-y constraint
        if(yIt == xIt->second.end()) return NULL;
                
        return &(yIt->second);
}

// set the x->y connection in this constraint graph to: x*a <= y*b + c
// return true if this results this constraint graph being changed
// xSign, ySign: the default signs for x and y. If they're set to unknown, setVal computes them on its own using getVarSign.
//     otherwise, it uses the given signs 
bool ConstrGraph::setVal(varID x, varID y, int a, int b, int c, 
                         // GB : 2011-03-05 (Removing Sign Lattice Dependence) affineInequality::signs xSign, affineInequality::signs ySign, 
                         string indent)
{
        //Dbg::dbg << indent << "setVal(): "<<x<<"*"<<a<<" <= "<<y<<"*"<<b<<" + "<<c<<"\n";
        // This constraint graph will now definitely be initialized
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        if(l.first==uninitialized || l.first==bottom) { setToConstrKnown(conj, false, indent+"    "); }
                
        //Dbg::dbg << indent << "    setVal(): "<<str(indent+"    ")<<"\n";
        
        // If the graph is maximal, there is no need to bother adding anything
        if(isMaximalState(true, indent+"    ")) return false;

        map<varID, map<varID, affineInequality> >::iterator xIt = vars2Value.find(x);
        // GB : 2011-03-05 (Removing Sign Lattice Dependence) xSign = (xSign==affineInequality::unknownSgn? getVarSign(x, indent+"    "): xSign);
        // GB : 2011-03-05 (Removing Sign Lattice Dependence) ySign = (ySign==affineInequality::unknownSgn? getVarSign(y, indent+"    "): ySign);
        affineInequality newConstr(a, b, c, x==zeroVar, y==zeroVar, // GB : 2011-03-05 (Removing Sign Lattice Dependence) , xSign, ySign
                                   affineInequality::unknownSgn, affineInequality::unknownSgn);
        
        vars.insert(x);
        vars.insert(y);
        modifiedVars.insert(x);
        modifiedVars.insert(y);
        /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.insert(x);
        newConstrVars.insert(y);*/
                                                
        // we don't have constraints from x
        if(xIt == vars2Value.end())
        {
                constrChanged = true;
                vars2Value[x][y] = newConstr;
                //Dbg::dbg << "vars2Value[x][y] = " << Dbg::escape(vars2Value[x][y].str("")) << "\n";
                //Dbg::dbg << "newConstr = " << Dbg::escape(newConstr.str("")) << "\n";
                return true;
        }
        
        map<varID, affineInequality>::iterator yIt = xIt->second.find(y);
        // we don't have an x->y constraint
        if(yIt == xIt->second.end())
        {
                vars2Value[x][y] = newConstr;
                constrChanged = true;
                return true;
        }
        
        affineInequality& constrXY = yIt->second;
        constrChanged = constrChanged || (constrXY != newConstr);
        bool modified = constrXY.set(newConstr);
        return modified;
}

// GB : 2011-03-05 (Removing Sign Lattice Dependence)
bool ConstrGraph::setVal(varID x, varID y, const affineInequality& ineq, string indent)
{
        return setVal(x, y, ineq.getA(), ineq.getB(), ineq.getC()// GB : 2011-03-05 (Removing Sign Lattice Dependence), ineq.getXSign(), ineq.getYSign()
                     );
}

// Sets the state of this constraint graph to Uninitialized, without modifying its contents. Thus, 
//    the graph will register as uninitalized but when it is next used, its state will already be set up.
// Returns true if this causes the constraint graph to be modified and false otherwise.
bool ConstrGraph::setToUninitialized_KeepState(string indent)
{
        bool modified = (level != uninitialized);
        
        // This graph is now uninitialized
        level = uninitialized;
        constrType = unknown;
        
        return modified;
}

// Sets the state of this constraint graph to Bottom
// Returns true if this causes the constraint graph to be modified and false otherwise.
bool ConstrGraph::setToBottom(string indent)
{
        bool modified = (level != bottom);
        
        // Erase all the data in this constraint graph
        //Dbg::dbg << indent << "    #vars2Value="<<vars2Value.size()<<"\n";
        eraseConstraints(true, "");
        /*Dbg::dbg << indent << "    #vars2Value="<<vars2Value.size()<<"\n";
        Dbg::dbg << indent << "    AFTER CONSTRAINTS ERASED="<<str(indent+"        ")<<"\n";*/
        
        // This graph now contains no constraints
        level = bottom;
        constrType = unknown;

        
        // Erase the modification state
        modifiedVars.clear();
        /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.clear(); */
        
        // Reset constrChanged because the state of the graph is now correct with 
        // respect to its constraints.
        constrChanged = false;
        
        return modified;
}

// Sets the state of this constraint graph to constrKnown, with the given constraintType
// eraseCurConstr - if true, erases the current set of constraints and if false, leaves them alone
// Returns true if this causes the constraint graph to be modified and false otherwise.
bool ConstrGraph::setToConstrKnown(constrTypes ct, bool eraseCurConstr, string indent)
{
        bool modified = (level != constrKnown);
        
        // This graph now contains no constraints
        level = constrKnown;
        constrType = ct;

        // Only erase constraints if requested
        if(eraseCurConstr) {
                // Erase all the data in this constraint graph
                eraseConstraints(true, "");
                
                // Erase the modification state
                modifiedVars.clear();
                /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.clear(); */
        }
        // Reset constrChanged because the state of the graph is now correct with 
        // respect to its constraints.
        constrChanged = false;
        
        return modified;
}

// Sets the state of this constraint graph to Inconsistent
// noConsistencyCheck - flag indicating whether this function should do nothing if this noConsistencyCheck() returns 
//              true (=false) or to not bother checking with isBottom (=true)
// Returns true if this causes the constraint graph to be modified and false otherwise.
bool ConstrGraph::setToInconsistent(string indent)
{
        bool modified = !(level == constrKnown && constrType == inconsistent);
        
        level = constrKnown;
        constrType = inconsistent;
        
        // Erase all the data in this constraint graph
        eraseConstraints(true, indent+"    ");
        
        // Erase the modification state
        modifiedVars.clear();
        /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.clear(); */
        
        // Reset constrChanged because the state of the graph is now correct with 
        // respect to its constraints and will not change from now on.
        constrChanged = false;
        
        return modified;
}


// Sets the state of this constraint graph to Top 
// If onlyIfNotInit=true, this is only done if the graph is currently uninitialized
// Returns true if this causes the constraint graph to be modified and false otherwise.
bool ConstrGraph::setToTop(bool onlyIfNotInit, string indent)
{
        bool modified = false;
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        if(!onlyIfNotInit || l.first==uninitialized)
        {
                modified = (level != top) || modified;
                level = top;
                constrType = unknown;
                
                // Erase all the data in this constraint graph
                eraseConstraints(true, indent+"    ");
                
                // Erase the modification state
                modifiedVars.clear();
                /* GB 2011-06-02 : newConstrVars->modifiedVars : newConstrVars.clear(); */
                
                // Reset constrChanged because the state of the graph is now correct with 
                // respect to its constraints and will not change from now on.
                constrChanged = false;
        }
        return modified;
}

// Returns the level and constraint type of this constraint graph
// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
//                                                      or to not bother checking self-consistency and just return the last-known value (=true)
pair<ConstrGraph::levels,ConstrGraph::constrTypes> ConstrGraph::getLevel(bool noConsistencyCheck, string indent)
{
        // If we need to check for self-inconsistency
        if(!noConsistencyCheck) {
                // If the graph's constraints are known and can cause an inconsistency
                if(level == constrKnown && constrType==conj) {
                        // If the constraints have changed since the last time this constraint graph 
                        // was checked for self-consistency, update this graph's feasibility state
                        if(constrChanged) {
                                // Check for self-consistency and update the state of this constraint graph 
                                // to reflect its feasibility status
                                checkSelfConsistency(indent+"    ");
                        }
                }
        }
        return std::make_pair(level, constrType);
}

// Returns true if this graph is self-consistent and false otherwise
// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
//                                                      or to not bother checking self-consistency and just return the last-known value (=true)
bool ConstrGraph::isSelfConsistent(bool noConsistencyCheck, string indent)
{
        pair<levels, constrTypes> p = getLevel(noConsistencyCheck, indent);
        return !(p.first==constrKnown && p.second==inconsistent);
}

// Returns true if this graph has valid constraints and is self-consistent
// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
//                                                      or to not bother checking self-consistency and just return the last-known value (=true)
bool ConstrGraph::hasConsistentConstraints(bool noConsistencyCheck, string indent)
{
        pair<levels, constrTypes> p = getLevel(noConsistencyCheck, indent);
        return p.first==constrKnown && p.second!=inconsistent;
}

// Returns true if this constraint graph is maximal in that it can never reach a higher lattice state: it is
//    either top or inconsistent). Returns false if it not maximal.
// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
//                                                      or to not bother checking self-consistency and just return the last-known value (=true)
bool ConstrGraph::isMaximalState(bool noConsistencyCheck, string indent)
{
        pair<levels, constrTypes> p = getLevel(noConsistencyCheck, indent);
        return (p.first==constrKnown && p.second==inconsistent) ||
               p.first==top;
}

/**** String Output *****/

// Returns the string representation of the constraints held by this constraint graph, 
//    with a line for each pair of variables for which the constraint is < bottom. It also prints
//    the names of all the arrays that have empty ranges in this constraint graph
// There is no \n on the last line of output, even if it is a multi-line string
string ConstrGraph::str(string indent)
{
        return str(indent, true);
}

// Returns the string representation of the constraints held by this constraint graph, 
//    with a line for each pair of variables for which the constraint is < bottom. It also prints
//    the names of all the arrays that have empty ranges in this constraint graph
// There is no \n on the last line of output, even if it is a multi-line string
// If useIsBottom=true, isBottom() is used to determine whether the graph is =bottom.
// Otherwise, the bottom variable is checked.
// If useIsBottom=true, isBottom() is used to determine whether the graph is =bottom.
// Otherwise, the bottom variable is checked.
string ConstrGraph::str(string indent, bool useIsBottom)
{
        ostringstream outs;
        
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        
        if(l.first==uninitialized)
                outs << "ConstrGraph : uninitialized";
        else if(l.first==bottom)
                outs << "ConstrGraph : bottom";
        else if(l.first==top)
                outs << "ConstrGraph : top";
        else if(l.first==constrKnown)
        {
                if(l.second==unknown) outs << "ConstrGraph : constrKnown - Unknown";
                else if(l.second==inconsistent) outs << "ConstrGraph : constrKnown - Inconsistent";
                else {
                        //bool needEndl=false; // =true if the previous line was printed and needs a \n before the next line can begin
                        outs << "ConstrGraph : \n";
        //              Dbg::dbg << "vars2Value.size()="<<vars2Value.size()<<"\n";
                        /*for(map<varID, map<varID, affineInequality> >::iterator itX = vars2Value.begin();
                            itX!=vars2Value.end(); itX++)
                        {
        //outs << indent << "\nvars2Value["<<itX->first.str()<<"].size()="<<vars2Value[itX->first].size()<<"\n";
                                for(map<varID, affineInequality>::iterator itY = itX->second.begin();
                                    itY!=itX->second.end(); itY++)
                                {
                                        const affineInequality& constr = itY->second;
                                        if(needEndl) { outs << "\n"; }
                                        if(l.second==conj)
                                                outs << indent << "        " << Dbg::escape(constr.str(itX->first, itY->first, indent+"    "));
                                        else if(l.second==negConj)
                                                outs << indent << "        " << Dbg::escape(constr.strNeg(itX->first, itY->first, indent+"    "));
                                        needEndl = true;
                                }
                        }*/
                        
                        /*outs << indent << "  vars: \n";
                        varSetStatusToStream(vars, outs, needEndl, indent+"    ");
                        if(needEndl) outs << "\n";
                        outs << indent << "  divVars: \n";
                        varSetStatusToStream(divVars, outs, needEndl, indent+"    ");*/
                        outs << Dbg::addDOTStr(*this);
                }
        }
        
        return outs.str();
}

void ConstrGraph::varSetStatusToStream(const set<varID>& vars, ostringstream& outs, bool &needEndl, string indent)
{
        
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        
        for(set<varID>::const_iterator v=vars.begin(); v!=vars.end(); v++)
        {
                bool printedVarName=false;
                
                // Print all inequalities x <= ...
                if(vars2Value.find(*v) != vars2Value.end()) {
                        if(needEndl) { outs << "\n"; needEndl=false; }
                        if(!printedVarName) { outs << indent <<(*v)<<":\n"; printedVarName=true; }
                                        
                        for(map<varID, affineInequality>::iterator itY = vars2Value[*v].begin();
                            itY!=vars2Value[*v].end(); itY++)
                        {
                                const affineInequality& constr = itY->second;
                                if(needEndl) { outs << "\n"; needEndl=false; }
                                if(l.second==conj)
                                        outs << indent << "    " << Dbg::escape(constr.str(*v, itY->first, indent+"    "));
                                else if(l.second==negConj)
                                        outs << indent << "    " << Dbg::escape(constr.strNeg(*v, itY->first, indent+"    "));
                                
                                // If there exist both constraints x <= y and y <= x, print both on the same line
                                if(vars2Value.find(itY->first) != vars2Value.end() && 
                                   vars2Value[itY->first].find(*v) != vars2Value[itY->first].end()) {
                                   const affineInequality& constr = vars2Value[itY->first][*v];
                                   outs << "\n";
                                        if(l.second==conj)
                                                outs << indent << "    " << Dbg::escape(constr.str(itY->first, *v, indent+"    "));
                                        else if(l.second==negConj)
                                                outs << indent << "    " << Dbg::escape(constr.strNeg(itY->first, *v, indent+"    "));
                                }
                                needEndl = true;
                        }
                }
                
                // Print all inequalities ... <= x
                for(map<varID, map<varID, affineInequality> >::iterator itX = vars2Value.begin();
                    itX!=vars2Value.end(); itX++)
                {
//outs << indent << "\nvars2Value["<<itX->first.str()<<"].size()="<<vars2Value[itX->first].size()<<"\n";
                        for(map<varID, affineInequality>::iterator itY = itX->second.begin();
                            itY!=itX->second.end(); itY++)
                        {
                                if(itY->first != *v) continue;
                                        
                                // If there exist both constraints x <= y and y <= x, then we've already printed this constraint
                                if(vars2Value.find(*v) != vars2Value.end() && 
                                   vars2Value[*v].find(itX->first) != vars2Value[*v].end())
                                        continue;
                                
                                const affineInequality& constr = itY->second;
                                if(needEndl) { outs << "\n"; needEndl=false; }
                                if(!printedVarName) { outs << indent <<(*v)<<":\n"; printedVarName=true; }
                                if(l.second==conj)
                                        outs << indent << "    " << Dbg::escape(constr.str(itX->first, itY->first, indent+"    "));
                                else if(l.second==negConj)
                                        outs << indent << "        " << Dbg::escape(constr.strNeg(itX->first, itY->first, indent+"    "));
                                needEndl = true;
                        }
                }
        }
}

string ineq2Str(const string& x, const string& y, const string& rel, const int& a, const int& b, const int& c)
{
        ostringstream oss;
        if(a==1) {
                if(b==1) {
                        if(c==0)     oss << x << rel << y;
                        else if(c>0) oss << x << rel << y << "+" << c;
                        else if(c==-1) {
                                if(rel == "=") oss << x << rel << y << "-" << -c;
                                else           oss << x << "<" << y;
                        } else if(c<-1) oss << x << rel << y << "-" << -c;
                } else {
                        if(c==0)     oss << x << rel << y << "*" << b;
                        else if(c>0) oss << x << rel << y << "*" << b << "+" << c;
                        else if(c<0) oss << x << rel << y << "*" << b << "-" << -c;
                }
        } else {
                if(b==1) {
                        if(c==0)     oss << x << "*" << a << rel << y;
                        else if(c>0) oss << x << "*" << a << rel << y << "+" << c;
                        else if(c<0) oss << x << "*" << a << rel << y << "-" << -c;
                } else {
                        if(c==0)     oss << x << "*" << a << rel << y << "*" << b;
                        else if(c>0) oss << x << "*" << a << rel << y << "*" << "+" << c;
                        else if(c<0) oss << x << "*" << a << rel << y << "*" << "-" << -c;
                }
        }
        
        return oss.str();
}

// Returns a normalized version of this variable's name that can be consumed by DOT
string normV(const varID& v) {
        string n = v.str(true);
        Dbg::dbg << "Pre: "<<n<<"\n";

     // DQ (12/6/2016): Fixed compiler warning: -Wsign-compare
     // for(int i=0; i<n.length(); i++) if(n[i] == ':' || n[i] == '-') n[i] = '_';
        for(size_t i=0; i<n.length(); i++) if(n[i] == ':' || n[i] == '-') n[i] = '_';

        Dbg::dbg << "Post: "<<n<<"\n";
        return n;
}

// Returns a normalized version of this variable's name with annotations that can be consumed by DOT
string normAV(const varID& v) {
        string n = v.str(false);

     // DQ (12/6/2016): Fixed compiler warning: -Wsign-compare
        for(size_t i=0; i<n.length(); i++) if(n[i] == ':' || n[i] == '-') n[i] = '_';

        return n;
}

// Returns a string that containts the representation of this constraint graph as a graph in the DOT language
// that has the given name
string ConstrGraph::toDOT(string graphName) {
        return toDOT(graphName, vars);
}
// Returns a string that containts the representation of this constraint graph as a graph in the DOT language
// that has the given name, focusing the graph on just the variables inside focusVars.
string ConstrGraph::toDOT(string graphName, set<varID>& focusVars)
{
        ostringstream oss;
        
        oss << "graph "<<graphName<<"{\n";
        oss << "\trankdir=LR;\n";
        oss << "\tnode [shape=box];\n";
        oss << "\tgraph [bgcolor=transparent]\n";
        
        // Identify the variables that are actually involved in constraints
        set<varID> constrVars;
        for(map<varID, map<varID, affineInequality> >::iterator x=vars2Value.begin(); x!=vars2Value.end(); x++) {
                constrVars.insert(x->first);
                for(map<varID, affineInequality>::iterator y=x->second.begin(); y!=x->second.end(); y++)
                        constrVars.insert(y->first);
        }
        
        // Maps different annotations to all the variables that share these annotations 
        map<map<string, void*>, set<varID> > annot2var;
        
        // Organize all the variables according to their annotations
        for(set<varID>::iterator v=constrVars.begin(); v!=constrVars.end(); v++) {
                map<string, void*> annot = v->getAnnotations();
                annot2var[annot].insert(*v);
        }
        
        int i=0;
        for(map<map<string, void*>, set<varID> >::iterator it=annot2var.begin(); it!=annot2var.end(); it++, i++) {
                oss << "\tsubgraph clusterSG_"<<i<<"{\n";
                oss << "\t\tlabel = \"";
                        for(map<string, void*>::const_iterator a=it->first.begin(); a!=it->first.end(); ) {
                                oss << a->first<<":"<<a->second;
                                a++;
                                if(a!=it->first.end()) oss << " : ";
                        }
                oss << "\";\n";
                oss << "\t\tranksep = 100;\n";
                oss << "\t\tbgcolor = \"#F2C2FE\";\n";
                oss << "\t\t\n";

                for(set<varID>::iterator v=it->second.begin(); v!=it->second.end(); v++)
                        oss << "\t\t"<<normAV(*v)<<" [style=filled, fontcolor=\"#000077\", fillcolor=\"#ffffff\", label=\""<<v->str(true)<<"\", shape=box];\n";
                
                oss << "\t}\n";
        }
        
        for(map<varID, map<varID, affineInequality> >::iterator x=vars2Value.begin(); x!=vars2Value.end(); x++) {
                string xStr = normAV(x->first);
                map<string, void*> xAnnot = x->first.getAnnotations();
                
                for(map<varID, affineInequality>::iterator y=x->second.begin(); y!=x->second.end(); y++) {
                        string yStr = normAV(y->first);
                        map<string, void*> yAnnot = y->first.getAnnotations();
                        
                        
                        // Edges within an annotation and across annotations are given different colors
                        string edgestyle;
                        if(xAnnot == yAnnot) edgestyle = "solid";
                        else                 edgestyle = "dashed";
                        string edgefontcolor;
                        if(xAnnot == yAnnot) edgefontcolor = "770000";
                        else                 edgefontcolor = "007700";
                        
                        int a, b, c;
                        if(isEqVars(x->first, y->first, a, b, c)) {
                                if(x->first < y->first) {
                                        oss << "\t"<<xStr<<" -- "<<yStr<<"  [label=\"";
                                        if(y->second.getLevel() == affineInequality::bottom) oss << "bottom"; 
                                        else if(y->second.getLevel() == affineInequality::top) oss << "top";
                                        else oss << ineq2Str(x->first.str(true), y->first.str(true), "=", a, b, c);
                                        oss << "\", style=\""<<edgestyle<<"\", fontcolor=\"#"<<edgefontcolor<<"\"];\n"; // href=\"http://www.cnn.com\"
                                }
                        } else {
                                oss << "\t"<<xStr<<" -- "<<yStr<<"  [label=\"";
                                if(y->second.getLevel() == affineInequality::bottom) oss << "bottom"; 
                                else if(y->second.getLevel() == affineInequality::top) oss << "top";
                                else oss << ineq2Str(x->first.str(true), y->first.str(true), "<=", y->second.getA(), y->second.getB(), y->second.getC());
                                oss << "\", style=\""<<edgestyle<<"\", fontcolor=\"#"<<edgefontcolor<<"\"];\n";
                        }
                }
        }
        oss << "}\n";
        
        //Dbg::dbg << oss.str() <<"\n";
        
        return oss.str();
}

/**** Comparison Functions ****/

// two constraint graphs are unequal if there exists some pair of variables
// that have different constraints (to get semantic equivalence apply transitive
// closure first)
bool ConstrGraph::operator != (ConstrGraph &that)
{
        // if either constraint graph is uninitialized, it isn't equal to any other graph
        if(getLevel(true, "    ").first==uninitialized ||
           that.getLevel(true, "    ").first==uninitialized)
           return true;
        
        return diffConstraints(that, "    ");// || diffArrays(&that);
}

// two graphs are equal if they're not unequal
bool ConstrGraph::operator == (ConstrGraph &that)
{
        return !(*this != that);
}

bool ConstrGraph::operator==(Lattice* that)
{
        return *this == *dynamic_cast<ConstrGraph*>(that);
}

// this is <<= that if the constraints in this are equal to or strictly tighter (tight is bottom, loose is top) 
// than the constraints in that for every pair of variables. As this function does not itself apply 
// transitive closure, the input graphs need to be closed for this to be a semantic <=.
bool ConstrGraph::operator <<= (ConstrGraph &that)
{
        //printf("ConstrGraph::operator <=, initialized=%d, that.initialized=%d\n", initialized, that.initialized);
        pair <levels, constrTypes> l = getLevel(true, "    ");
        pair <levels, constrTypes> tl = that.getLevel(true, "    ");
        
        // If this constraint graph is Uninitialized, it is <= than any other graph
        if(l.first == uninitialized) return true;
        // Else, if that is Uninitialized, it must be smaller
        else if(tl.first == uninitialized) return false;
        
        // Both this and that must be initialized
        
        // If this constraint graph is Bottom, it is <= than any other initialized graph
        if(l.first == bottom) return true;
        // Else, if that is Bottom, it must be smaller
        else if(tl.first == bottom) return false;
        
        // Both this and that must be initialized and not bottom
        
        // If that constraint graph is Top, it is >= than any other initialized graph
        if(tl.first == top) return true;
        // Else, if this is top, it must be larger than that
        else if(l.first == top) return false;
        
        // Both this and that must be constrKnown
        
        // If that constraint graph is inconsistent, it is >= any other constrKnown
        if(tl.second == inconsistent) return true;
        // Else, if this is top, it must be larger than that
        else if(l.second == inconsistent) return false;
                
        // If the two graphs have different negation states (one is a conjunction and the other is a negated conjunction)
        if(l.second != tl.second)
                // They're not comparable
                return false;
        
        // Flags that indicate whether this/that map has extra terms which do not exist in that/this
        bool thisHasExtra=false, thatHasExtra=false; 
        // True if in all the terms that appear in both this and that, the one in this is always tighter than the one in that
        bool thisAlwaysTighter=true;
        
        map<varID, map<varID, affineInequality> >::const_iterator itThisX, itThatX;
        for(itThisX = vars2Value.begin(), itThatX = that.vars2Value.begin();
            itThisX!=vars2Value.end() && itThatX!=that.vars2Value.end(); )
        {
                // If both constraint graphs have constraints for itThisX->first
                if(itThisX->first == itThatX->first)
                {
                        /*affineInequality::signs xSign = getVarSign(itThisX->first, indent+"    ");
                        ROSE_ASSERT(xSign == that.getVarSign(itThisX->first, indent+"    "));*/
                        varID x = itThisX->first;
                        
                        map<varID, affineInequality>::const_iterator itThisY, itThatY;
                        for(itThisY = itThisX->second.begin(), itThatY = itThatX->second.begin();
                            itThisY!=itThisX->second.end() && itThatY!=itThatX->second.end(); )
                        {
                                // if both constraint graphs have constraints for <itThisX->first, itThisY->first>
                                if(itThisY->first == itThatY->first)
                                {
                                        varID y = itThisY->first;
                                        
                                        // Compare the information content in this inequality in both graphs and
                                        //      record if the <x->y> constraint in that has more information than the one in this
                                        if((l.second == conj && /*itThatY->second.semLessThan(itThisY->second, isEqZero(x), isEqZero(y))*/
                                                                     itThatY->second!=itThisY->second && itThatY->second.semLessThan(itThisY->second, 
                                                                                                 x==zeroVar?NULL:getVal(x, zeroVar), x==zeroVar?NULL:getVal(zeroVar, x), 
                                                                   y==zeroVar?NULL:getVal(y, zeroVar), y==zeroVar?NULL:getVal(zeroVar, y), "    ")) ||
                                           (l.second == negConj && itThatY->second.semLessThanNeg(itThisY->second, isEqZero(x), isEqZero(y))))
                                                thisAlwaysTighter = false;
                                        
                                        // Advance both Y iterators
                                        itThisY++;
                                        itThatY++;
                                }
                                // Else, if this and that don't agree on the list of inequalities of the form ax <= ...
                                else
                                        // If this has an extra ax <= ... mapping that doesn't exist in that
                                        if(itThisY->first < itThatY->first) {
                                                thisHasExtra = true;
                                                // Advance the Y iterator in this to the next mapping
                                                itThisY++;
                                        // If that has an extra ax <= ... mapping that doesn't exist in this
                                        } else {
                                                thatHasExtra = true;
                                                // Advance the Y iterator in that to the next mapping
                                                itThatY++;
                                        }
                        }
                        
                        // Record if there are variables that have constraints in itThatX->second but not in itThisX->second
                        if(itThisY!=itThisX->second.end()) thisHasExtra = true;
                        
                        // Record if there are variables that have constraints in itThatX->second but not in itThisX->second
                        if(itThatY!=itThatX->second.end()) thatHasExtra = true;
                        
                        // Advance both X iterators
                        itThisX++;
                        itThatX++;
                }
                // Else, if this and that don't agree on the list of variables for which inequalities exist
                else
                {
                        // If this has an extra set of mapping for some variable that do not exist in that
                        if(itThisX->first < itThatX->first) {
                                thisHasExtra = true;
                                // Advance the X iterator in this to the next mapping
                                itThisX++;
                        // If that has an extra set of mapping for some variable that do not exist in this
                        } else {
                                thatHasExtra = true;
                                // Advance the X iterator in that to the next mapping
                                itThatX++;
                        }
                }
        }
        
        // if there are variables that have constraints in that but not in this
        if(itThatX!=vars2Value.end())
                // that has some constraints that this does not and is therefore either tighter or non-comparable
                return false;
        
        // Record if there are variables that have constraints in this but not in that
        if(itThisX!=vars2Value.end()) thisHasExtra = true;
        
        // Record if there are variables that have constraints in that but not in this
        if(itThatX!=vars2Value.end()) thatHasExtra = true;
        
        // This <<= That if for all constraints mapped by both This and That, the ones mapped by This are always tigher AND
        // If this graph is a conjunction, This has the same or more constraints than That and
        // If this graph is a disjunction, This has the same or fewer constraints as That
        return thisAlwaysTighter && 
               ((l.second == conj && !thatHasExtra) || (l.second == negConj && !thisHasExtra));
}
        
// Returns true if x*b+c MUST be outside the range of y and false otherwise. 
// If two variables are unrelated, it is assumed that there is no information 
// about their relationship and mustOutsideRange() thus proceeds conservatively (returns true).
bool ConstrGraph::mustOutsideRange(varID x, int b, int c, varID y, string indent)
{
        // Do a transitive closure in case one is overdue
        if(constrChanged) transitiveClosure(indent+"    ");
        
        affineInequality* constrXY = getVal(x, y);
        affineInequality* constrYX = getVal(y, x);
        
        if(CGDebugLevel>=1)
        {
                if(CGDebugLevel>=1) 
                {
                        Dbg::dbg << indent << "mustOutsideRange("<<x.str()<<"*"<<b<<"+"<<c<<", "<<y.str()<<")\n";
                        Dbg::dbg << indent << "    constrXY="<<constrXY<<" constrYX="<<constrYX<<"\n";
                        if(constrXY) Dbg::dbg << indent << "mustOutsideRange() "<<x.str()<<"->"<<y.str()<<"="<<Dbg::escape(constrXY->str(x, y, ""))<<" b="<<b<<" c="<<c<<"\n";
                        if(constrYX) Dbg::dbg << indent << "mustOutsideRange() "<<y.str()<<"->"<<x.str()<<"="<<Dbg::escape(constrYX->str(y, x, ""))<<" b="<<b<<" c="<<c<<"\n";
                }
        }
        //Dbg::dbg << str("    ") <<"\n";
                
        ROSE_ASSERT(b==1);
        if(constrXY) ROSE_ASSERT(constrXY->getA()==1 && constrXY->getB()==1);
        if(constrYX) ROSE_ASSERT(constrYX->getA()==1 && constrYX->getB()==1);
        
        // if (x*a' <= y*b' + c'), then 
        // (x*b + c < y) == (x*b*b' + c*b' + c' < y*b' + c' >= x*a') == (x*b*b' + c*b' + c' < x*a')
        // to keep this simple, we only deal with the case where a'=1, b'=1 and b'=1, which reduces the above to:
        // x + c + c' < x
        // c + c' < 0
        if(constrXY) return (constrXY && (constrXY->getC() + c)<0);
        // if (y*a' <= x*b' + c'), then 
        // (x*b + c > y) == (x*b*a' + c*a' > y*a' <= x*b' + c') == (x*b*a' + c*a' > x*b' + c')
        // to keep this simple, we only deal with the case where a'=1, b'=1 and b=1, which reduces the above to:
        // x + c > x + c'
        // c - c' > 0
        if(constrYX) return (constrYX && (c - constrYX->getC())>0);
                
        return false;
        
        
/*      // x<=y+c'
        if((getVal(x,y)!=INF && (getVal(x,y)+c)<0) || 
               (getVal(y, x)!=INF && getVal(y,x)<c))
                Dbg::dbg << "    must be outside the range\n";
        else Dbg::dbg << "    may be inside the range\n";
                
        return (getVal(x,y)!=INF && (getVal(x,y)+c)<0) || 
               (getVal(y, x)!=INF && getVal(y,x)<c);*/
}

/*// returns true if x+c MUST be inside the range of y and false otherwise
// If two variables are unrelated, it is assumed that there is no information 
// about their relationship and mustInsideRange() thus proceeds conservatively.
bool ConstrGraph::mustInsideRange(varID x, int b, int c, varID y)
{
        return !mayOutsideRange(x, b, c, y);
}
*/

// returns true if this logical condition must be true and false otherwise
// <from LogicalCond>
bool ConstrGraph::mayTrue(string indent)
{
        if(constrChanged) transitiveClosure(indent+"    ");
        pair <levels, constrTypes> l = getLevel(true, indent+"    ");
        return !(l.first==constrKnown && l.second==inconsistent);
}

/* Transactions */
void ConstrGraph::beginTransaction(string indent)
{
        inTransaction = true;
}

void ConstrGraph::endTransaction(string indent)
{
        ROSE_ASSERT(inTransaction);
        inTransaction = false;
        transitiveClosure();
        
        /*Dbg::dbg << "vars = ";
        for(varIDSet::iterator it=vars.begin(); it!=vars.end(); it++)
        { Dbg::dbg << (*it).str() << " "; }
        Dbg::dbg << "\n";*/
}

