#include "procSet.h"

static int debugLevel=1;

/***********
 * procSet *
 ***********/
//int procSet::infinity;

/********************
 * contRangeProcSet *
 ********************/

/*contRangeProcSet::~contRangeProcSet()
{
        if(cg)
        {
                // Disconnect this process set's bounds from its constraint craph
                cg->removeVar(getLB());
                cg->removeVar(getUB());
        }
}*/

// The number of variables that have been generated as range lower and upper bounds
int contRangeProcSet::varCount=0;
        
varID contRangeProcSet::genFreshVar(string prefix)
{
        ostringstream outs;
        if(prefix=="") outs << "boundVar_"<< varCount;
        else           outs << prefix<<"_" << varCount;
        varCount++;
        varID var(outs.str());
        //Dbg::dbg << outs.str()<<" : "<<var.str()<<endl;;
        return var;
}

contRangeProcSet::contRangeProcSet(bool emptyRange)
{
        lb = genFreshVar("LB");
        ub = genFreshVar("UB");
        cg = NULL;
        emptyRange = emptyRange;
        valid = true;
}

contRangeProcSet::contRangeProcSet(varID lb, varID ub, ConstrGraph* cg)
{
        this->cg = cg;
        this->lb = lb;
        this->ub = ub;
        genFreshBounds();
        emptyRange = false;
        valid = true;
        //ROSE_ASSERT(cg->lteVars(this->lb, this->ub));
}

// this->lb*lbA = lb*lbB + lbC
// this->ub*ubA = ub*ubB + ubC
contRangeProcSet::contRangeProcSet(varID lb, int lbA, int lbB, int lbC,
                                   varID ub, int ubA, int ubB, int ubC,
                                   ConstrGraph* cg)
{
        this->cg = cg;
        this->lb = lb;
        this->ub = ub;
        genFreshBounds(lbA, lbB, lbC, ubA, ubB, ubC);
        emptyRange = false;
        valid = true;
        //ROSE_ASSERT(cg->lteVars(this->lb, this->ub));
}

contRangeProcSet::contRangeProcSet(varID lb, int lbA, int lbB, int lbC,
                                   varID ub, int ubA, int ubB, int ubC,
                                   string annotName, void* annot,
                                   ConstrGraph* cg)
{
        this->cg = cg;
        this->lb = lb;
        this->ub = ub;
        genFreshBounds(lbA, lbB, lbC, ubA, ubB, ubC, annotName, annot);
        emptyRange = false;
        valid = true;
        //ROSE_ASSERT(cg->lteVars(this->lb, this->ub));
}

// if freshenVars==true, calls genFreshBounds() to make this set use different lower and upper
// bound variables from that set, while ensuring that the bound variables themselves are 
// equal to each other in cg
contRangeProcSet::contRangeProcSet(const contRangeProcSet& that, bool freshenVars)
{
        lb = that.lb;
        ub = that.ub;
        cg = that.cg;
        
        emptyRange = that.emptyRange;
        valid = that.valid;
        if(freshenVars) genFreshBounds();
        if(cg) {
                if(!(cg->lteVars(lb, ub, 1, 1, 0, "    "))) {
                        Dbg::dbg << "contRangeProcSet::contRangeProcSet() ASSERT FAILURE: ROSE_ASSERT(cg-&lt;lteVars("<<lb<<", "<<ub<<", 1, 1, 0)) cg="<<endl;
                        Dbg::dbg << cg->str() << endl;
                }
                ROSE_ASSERT(cg->lteVars(lb, ub, 1, 1, 0, "    "));
        }
}

// copies that to this, returning true if this is not changed and false otherwise
bool contRangeProcSet::copy(const contRangeProcSet& that)
{
        bool modified = 
                lb != that.lb ||
                ub != that.ub ||
                cg != that.cg ||
                emptyRange != that.emptyRange ||
                valid != that.valid;
        lb = that.lb;
        ub = that.ub;
        cg = that.cg;
        emptyRange = that.emptyRange;
        valid = that.valid;
        
        return modified;
}

// Returns the current value of lb
const varID& contRangeProcSet::getLB() const
{
        return lb;
}

// Returns the current value of ub
const varID& contRangeProcSet::getUB() const
{
        return ub;
}

// Returns the current value of cg
ConstrGraph* contRangeProcSet::getConstr() const
{
        return cg;
}

// Sets lb, returning true if this causes this process set to change
// and false otherwise
bool contRangeProcSet::setLB(const varID& lb)
{
        bool modified = cg->eqVars(this->lb, lb, "    ");
        this->lb = lb;
        return modified;
}

// Sets ub, returning true if this causes this process set to change
// and false otherwise
bool contRangeProcSet::setUB(const varID& ub)
{
        bool modified = cg->eqVars(this->ub, ub, "    ");
        this->ub = ub;
        return modified;
}

// Do not modify the lower bound but add the constraint that it is equal lb:
// lb*a = newLB*b + c
bool contRangeProcSet::assignLB(const varID& newLB, int a, int b, int c)
{
        bool modified=false;
        //Dbg::dbg << "assignLB("<<lb<<" => "<<newLB<<")"<<endl;
        cg->eraseVarConstr(lb);
        //Dbg::dbg << "erased LB "<<lb<<" cg = "<<cg<<" = "<<cg->str()<<endl;
        modified = cg->assertCond(lb, newLB, a, b, c) || modified;
        modified = cg->assertCond(newLB, lb, b, a, 0-c) || modified;
        return modified;
}

// Do not modify the upper bound but add the constraint that it is equal ub:
// ub*a = newUB*b + c
bool contRangeProcSet::assignUB(const varID& newUB, int a, int b, int c)
{
        bool modified=false;
        //Dbg::dbg << "assignUB("<<ub<<" => "<<newUB<<")"<<endl;
        cg->eraseVarConstr(ub);
        //Dbg::dbg << "erased UB "<<ub<<" cg = "<<cg<<" = "<<cg->str()<<endl;
        modified = cg->assertCond(ub, newUB, a, b, c) || modified;
        modified = cg->assertCond(newUB, ub, b, a, 0-c) || modified;
        return modified;
}

// Sets cg, returning true if this causes this process set to change
// and false otherwise
bool contRangeProcSet::setConstr(ConstrGraph* cg)
{
        bool modified = this->cg != cg;
        this->cg = cg;
        return modified;
}

// Asserts all the set-specific invariants in this set's constraint graph
void contRangeProcSet::refreshInvariants()
{
        ROSE_ASSERT(cg);
        
        cg->addVar(lb);
        cg->addVar(ub);
        // lb <= ub
        /*GB 2011-06-01 NO-LB-UB-rankVar : cg->assertCond(lb, ub, 1, 1, 0);
        
        cg->localTransClosure(lb);
        cg->localTransClosure(ub);*/
}

// Generates new lower and upper bound variables that are set to be equal to the original
// lower and upper bound variables in this procSet's constraint graph as:
// newLB*lbA = oldLB*lbB + lbC and newUB*ubA = oldUB*ubB + ubC
// If annotName!="", the bounds variables will have the annotation annotName->annot
void contRangeProcSet::genFreshBounds(int lbA, int lbB, int lbC,
                                      int ubA, int ubB, int ubC, 
                                      string annotName, void* annot)
{
        const varID oldLB = lb;
        lb = genFreshVar("LB");
        if(annotName != "") lb.addAnnotation(annotName, annot);
        cg->addVar(lb);
        Dbg::dbg << "    Assigning="<<lbA<<"*"<<lb<<" = "<<lbB<<"*"<<oldLB<<" + "<<lbC<<endl;
        //cg->assertCond(lb, oldLB, lbA, lbB, lbC);
        //cg->assertCond(oldLB, lb, lbB, lbA, 0-lbC);
        cg->assign(lb, oldLB, lbA, lbB, lbC);

        const varID oldUB = ub;                 
        ub = genFreshVar("UB");
        if(annotName != "") ub.addAnnotation(annotName, annot);
        cg->addVar(ub);
        Dbg::dbg << "    Assigning="<<ubA<<"*"<<ub<<" = "<<ubB<<"*"<<oldUB<<" + "<<ubC<<endl;
        //cg->assertCond(ub, oldUB, ubA, ubB, ubC);
        //cg->assertCond(oldUB, ub, ubB, ubA, 0-ubC);
        cg->assign(ub, oldUB, ubA, ubB, ubC);
        
        // lb <= ub
        cg->assertCond(lb, ub, 1, 1, 0);
        cg->localTransClosure(lb);
        cg->localTransClosure(ub);
}

// Transition from using the current constraint graph to using newCG, while annotating
// the lower and upper bound variables with the given annotation annotName->annot.
// Return true if this causes this set to change, false otherwise.
bool contRangeProcSet::setConstrAnnot(ConstrGraph* newCG, string annotName, void* annot)
{
        bool modified = cg!=newCG;
        cg = newCG;
        modified = lb.addAnnotation(annotName, annot) || modified;
        modified = ub.addAnnotation(annotName, annot) || modified;
        return modified;
}

// Add the given annotation to the lower and upper bound variables
// Return true if this causes this set to change, false otherwise.
bool contRangeProcSet::addAnnotation(string annotName, void* annot)
{
        bool modified = false;
        modified = lb.addAnnotation(annotName, annot) || modified;
        modified = ub.addAnnotation(annotName, annot) || modified;
        return modified;
}

// Remove the given annotation from this set's lower and upper bound variables
// Return true if this causes this set to change, false otherwise.
bool contRangeProcSet::removeVarAnnot(string annotName)
{
        bool modified = false;
        modified = lb.remAnnotation(annotName) || modified;
        modified = ub.remAnnotation(annotName) || modified;
        return modified;
}

// Make this set invalid, return true if this causes the set to change, false otherwise
bool contRangeProcSet::invalidate()
{
        bool modified = (valid == true);
        
        emptyRange = false;
        valid = false;
        
        return modified;
}

// Make this set valid, return true if this causes the set to change, false otherwise
bool contRangeProcSet::makeValid()
{
        bool modified = (valid == false);
        
        emptyRange = false;
        valid = true;
        
        return modified;
}

// Make this set empty, return true if this causes the set to change, false otherwise.
// Also, remove the set's lower and upper bounds from its associated constraint graph (if any).
bool contRangeProcSet::emptify()
{
        bool modified = (emptyRange == false || valid == false);
        
        // Disconnect the lower and upper bounds of this process set from its constraint
        // graph since the process set is now empty and thus its lower and upper bounds don't exist.
        modified = cgDisconnect() || modified;
        
        emptyRange = true;
        valid = true;
        
        return modified;
}

// Make this set non-empty, return true if this causes the set to change, false otherwise
bool contRangeProcSet::makeNonEmpty()
{
        bool modified = (emptyRange == true || valid == false);
        
        emptyRange = false;
        valid = true;
        
        return modified;
}

// Returns true if this and that represent the equal ranges
bool contRangeProcSet::rangeEq(const contRangeProcSet& that) const
{
        ROSE_ASSERT(cg == that.cg);
        //Dbg::dbg << "contRangeProcSet::rangeEq: cg->eqVars("<<lb.str()<<", "<<that.lb.str()<<")="<<cg->eqVars(lb, that.lb)<<endl;
        //Dbg::dbg << "contRangeProcSet::rangeEq: cg->eqVars("<<ub.str()<<", "<<that.ub.str()<<")="<<cg->eqVars(ub, that.ub)<<endl;
        return (cg->eqVars(lb, that.lb, "    ") && cg->eqVars(ub, that.ub, "    "));
}

// Returns true if that is at the top of but not equal to this's range
bool contRangeProcSet::rangeTop(const contRangeProcSet& that) const
{
        ROSE_ASSERT(cg == that.cg);
             // [lb<=that.lb <= (ub=that.ub)]
        //Dbg::dbg << "cg->lteVars("<<lb.str()<<", "<<that.lb.str()<<")="<<cg->lteVars(lb, that.lb)<<endl;
        //Dbg::dbg << "cg->eqVars("<<ub.str()<<", "<<that.ub.str()<<")="<<cg->eqVars(ub, that.ub)<<endl;
        return (cg->lteVars(lb, that.lb, 1, 1, 0, "    ") && cg->eqVars(ub, that.ub, "    "));
}

// Returns true if that is at the bottom of but not equal to this's range
bool contRangeProcSet::rangeBottom(const contRangeProcSet& that) const
{
        //Dbg::dbg << "rangeBottom: cg->eqVars("<<lb.str()<<", "<<that.lb.str()<<")="<<cg->eqVars(lb, that.lb)<<endl;
        //Dbg::dbg << "rangeBottom: cg->lteVars("<<that.ub.str()<<", "<<ub.str()<<")="<<cg->lteVars(that.ub, ub)<<endl;
        ROSE_ASSERT(cg == that.cg);
            // [(lb=that.lb) <= that.ub<=ub]
        return (cg->eqVars(lb, that.lb, "    ") && cg->lteVars(that.ub, ub, 1, 1, 0, "    "));
}

// Returns true if the ranges of this and that must be disjoint
bool contRangeProcSet::rangeDisjoint(const contRangeProcSet& that) const
{
            // [lb, ub] < [that.lb, that.ub] or [that.lb, that.ub] < [lb, ub]
        return (cg->ltVars(ub, that.lb) || cg->ltVars(that.ub, lb));
}

// Returns true if this must overlap, with this Above that: that.lb <= lb <= that.ub <= ub
bool contRangeProcSet::overlapAbove(const contRangeProcSet& that) const
{
            // that.lb <= lb <= that.ub <= ub
        return cg->lteVars(that.lb, lb, 1, 1, 0, "    ") && 
               cg->lteVars(lb, that.ub, 1, 1, 0, "    ") &&
               cg->lteVars(that.ub, ub, 1, 1, 0, "    ");
}

// Returns true if this must overlap, with this Below that: lb <= that.lb <= ub <= that.ub
bool contRangeProcSet::overlapBelow(const contRangeProcSet& that) const
{
            // lb <= that.lb <= ub <= that.ub
        return cg->lteVars(lb, that.lb, 1, 1, 0, "    ") && 
               cg->lteVars(that.lb, ub, 1, 1, 0, "    ") &&
               cg->lteVars(ub, that.ub, 1, 1, 0, "    ");
}

// Copies the given variable's constraints from srcCG to cg.
// !!! May want to label the inserted variables with this procSet object to avoid collisions
// !!! with other procSet objects.
void contRangeProcSet::copyVar(ConstrGraph* srcCG, const varID& var)
{
        cg->copyVar(srcCG, var);
}

// returns a heap-allocated reference to the intersection of this and that
// if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
procSet& contRangeProcSet::intersect(const procSet& that_arg) const
{
        const contRangeProcSet& that = dynamic_cast<const contRangeProcSet&>(that_arg);
        return intersect(that, false);
}

procSet& contRangeProcSet::intersect(const contRangeProcSet& that, bool transClosure) const
{
        //const contRangeProcSet& that = dynamic_cast<const contRangeProcSet&>(that_arg);
        contRangeProcSet* thisCopy = new contRangeProcSet(*this);
        thisCopy->genFreshBounds();
        //Dbg::dbg << "thisCopy="<<thisCopy<<endl;
        thisCopy->intersectUpd(that, transClosure);
        return *thisCopy;
        /*const contRangeProcSet& that = (const contRangeProcSet&)that_arg;

        // If the ranges are equal
        if(rangeEq(that))
        {       
                contRangeProcSet* thisCopy = new contRangeProcSet(*this);
                return *thisCopy;
        }
        // If that is inside this' range
        else if(rangeTop(that) || rangeBottom(that))
        {
                contRangeProcSet* thatCopy = new contRangeProcSet(that);
                return *thatCopy;
        }
        // If this and that have an empty intersection
        else if(rangeDisjoint(that))
        {
                emptyProcSet* e = new emptyProcSet();
                return *e;
        }
        // If this and that must overlap with this Below that: lb <= that.lb <= ub <= that.ub
        else if(overlapBelow(that))
        {
                contRangeProcSet* ret = new contRangeProcSet(that.lb, ub, cg);
                return *ret;
        }
        // If this and that must overlap with this Above that: that.lb <= lb <= that.ub <= ub
        else if(overlapAbove(that))
        {
                contRangeProcSet* ret = new contRangeProcSet(lb, that.ub, cg);
                return *ret;
        }
        // Else, this is an un-representable set
        else
        {
                invalidProcSet* i = new invalidProcSet();
                return *i;
        }*/
}

// updates this with the intersection of this and that
// returns true if this causes the procSet to change, false otherwise
// if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
bool contRangeProcSet::intersectUpd(const procSet& that_arg)
{
        const contRangeProcSet& that = dynamic_cast<const contRangeProcSet&>(that_arg);
        return intersectUpd(that, false);
}

bool contRangeProcSet::intersectUpd(const contRangeProcSet& that, bool transClosure)
{
        bool modified = false;
        
        //Dbg::dbg << "contRangeProcSet::intersectUpd"<<endl;
        
        // If the ranges are equal
        if(rangeEq(that))
        { if(debugLevel>0) Dbg::dbg << "contRangeProcSet::intersectUpd equal ranges"<<endl; }
        // If that is inside this' range
        else if(rangeTop(that) || rangeBottom(that))
        {
                if(debugLevel>0) Dbg::dbg << "contRangeProcSet::intersectUpd this inside that"<<endl; 
                modified = copy(that);
        }
        // If this and that have an empty intersection
        else if(rangeDisjoint(that))
        {
                if(debugLevel>0) Dbg::dbg << "contRangeProcSet::intersectUpd disjoint ranges"<<endl; 
                modified = emptify();
        }
        // If this and that must overlap with this Below that: lb <= that.lb <= ub <= that.ub
        else if(overlapBelow(that))
        {
                if(debugLevel>0) Dbg::dbg << "contRangeProcSet::intersectUpd overlapBelow"<<endl; 
                modified = lb != that.lb;
                lb = that.lb;
        }
        // If this and that must overlap with this Above that: that.lb <= lb <= that.ub <= ub
        else if(overlapAbove(that))
        {
                if(debugLevel>0) Dbg::dbg << "contRangeProcSet::intersectUpd overlapAbove"<<endl; 
                modified = ub != that.ub;
                ub = that.ub;
        }
        // Else, this is an un-representable set
        else
        {
                if(debugLevel>0) Dbg::dbg << "contRangeProcSet::intersectUpd invalid intersection"<<endl; 
                if(debugLevel>0) Dbg::dbg << "    this="<<str()<<endl;
                if(debugLevel>0) Dbg::dbg << "    that="<<that.str()<<endl;
                modified = invalidate();
        }
        
        return modified;
}

// Returns a heap-allocated reference to this - that
// if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
procSet& contRangeProcSet::rem(const procSet& that_arg) const
{
        const contRangeProcSet& that = dynamic_cast<const contRangeProcSet&>(that_arg);
        return rem(that, false);
}

procSet& contRangeProcSet::rem(const contRangeProcSet& that, bool transClosure) const
{
        contRangeProcSet* thisCopy = new contRangeProcSet(*this);
        thisCopy->genFreshBounds();
        thisCopy->remUpd(that, transClosure);
        return *thisCopy;
}

// Updates this with the result of this - that
// returns true if this causes the procSet to change, false otherwise
// if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
bool contRangeProcSet::remUpd(const procSet& that_arg)
{
        const contRangeProcSet& that = dynamic_cast<const contRangeProcSet&>(that_arg);
        return remUpd(that, false);
}

bool contRangeProcSet::remUpd(const contRangeProcSet& that, bool transClosure)
{
        bool modified = false;
        
        if(debugLevel>0) {
                Dbg::dbg << "remUpd: this="<<str("")<<endl;
                Dbg::dbg << "        that="<<that.str("")<<endl;
        }
        //initialized = dynamic_cast<IfMeetLat*>(that)->initialized;
        ROSE_ASSERT(cg == that.cg);
        
        // If the range to be removed is equal to this range
        if(rangeEq(that))
        {
                if(debugLevel>0) Dbg::dbg << "    remUpd: equal"<<endl;
                modified = emptify() || modified;
        }
        // If [lb<=that.lb <= (ub=that.ub)]
        else if(rangeTop(that))
        {
                if(debugLevel>0) Dbg::dbg << "    remUpd: that rangeTop of this"<<endl;
                // [lb, that.lb) : ub = that.lb-1
                cg->eraseVarConstr(ub);
                cg->assertCond(ub, that.lb, 1, 1, -1);
                cg->assertCond(that.lb, lb, 1, 1, 1);
                modified = true;
                
                cg->assertCond(lb, ub, 1, 1, 0);
                cg->localTransClosure(ub);
        }
        // If [that.lb<=lb <= (ub=that.ub)]
        else if(that.rangeTop(*this))
        {
                if(debugLevel>0) Dbg::dbg << "    remUpd: this rangeTop of that"<<endl;
                modified = emptify() || modified;
        }
        // If [(lb=that.lb) <= that.ub<=ub]
        else if(rangeBottom(that))
        {
                if(debugLevel>0) Dbg::dbg << "    remUpd: that rangeBottom of this"<<endl;
                // (that.ub, ub] : that.ub+1 = lb
                cg->eraseVarConstr(lb);
                cg->assertCond(that.ub, lb, 1, 1, -1);
                cg->assertCond(lb, that.ub, 1, 1, 1);
                modified = true;

                cg->assertCond(lb, ub, 1, 1, 0);
                cg->localTransClosure(lb);
        }
        // If [lb<=that.lb <= (ub=that.ub)]
        else if(that.rangeBottom(*this))
        {
                if(debugLevel>0) Dbg::dbg << "    remUpd: this rangeBottom of that"<<endl;
                modified = emptify() || modified;
        }
        // If [lb, ub] < [that.lb, that.ub] or [that.lb, that.ub] < [lb, ub]
        else if(rangeDisjoint(that))
        {
                Dbg::dbg << "    remUpd: that rangeDisjoint this"<<endl;
                // the two sets are non-overlapping, so nothing happens
                modified = false;
        }
        // If this and that must overlap with this Below that: lb <= that.lb <= ub <= that.ub
        else if(overlapBelow(that))
        {
                if(debugLevel>0) Dbg::dbg << "    remUpd: this rangeBelow of that"<<endl;
                // [lb, that.lb) : ub = that.lb-1
                cg->eraseVarConstr(ub);
                cg->assertCond(ub, that.lb, 1, 1, -1);
                cg->assertCond(that.lb, ub, 1, 1, 1);
                modified = true;
                
                cg->assertCond(lb, ub, 1, 1, 0);
                cg->localTransClosure(ub);
        }
        // If this and that must overlap with this Above that: that.lb <= lb <= that.ub <= ub
        else if(overlapAbove(that))
        {
                if(debugLevel>0) Dbg::dbg << "    remUpd: this rangeAbove of that"<<endl;
                // [that.ub, lb) : that.ub = lb-1
                cg->eraseVarConstr(lb);
                cg->assertCond(that.ub, lb, 1, 1, -1);
                cg->assertCond(lb, that.ub, 1, 1, -1);
                modified = true;
                
                cg->assertCond(lb, ub, 1, 1, 0);
                cg->localTransClosure(lb);
        }
        // If the sets may overlap but not in a clean way, the result of the removal
        // is not representable
        else
                modified = invalidate();
        return modified;
}

        
// Returns true if this set is empty, false otherwise
bool contRangeProcSet::emptySet() const
{ return emptyRange; }

// Returns true if this is a representable set, false otherwise
bool contRangeProcSet::validSet() const
{ return valid; }

// The size of this process set, either a specific integer or infinity
int contRangeProcSet::size() const
{
/*Dbg::dbg << "contRangeProcSet::size() : cg->eqVars("<<lb.str()<<", "<<ub.str()<<")="<<cg->eqVars(lb, ub)<<endl;
Dbg::dbg << "    cg = "<<cg->str()<<endl;*/
        if(emptySet())
                return 0;
        // If the lower bound is equal to the upper bound, the domain has a single element
        else if(cg->eqVars(lb, ub, "    "))
                return 1;
        else
        {
                int a, b, c;
                // If both the lower bound and the upper bound are related to each other via a 
                // lb = ub + c relationship
                if(cg->eqVars(lb, ub, a, b, c, "    ") && a==1 && b==1)
                        return c;
                // Otherwise, we don't know the exact size of this set and conservatively report infinity
                else
                        return infinity;
        }
}

// Assigns this to that
procSet& contRangeProcSet::operator=(const procSet& that_arg)
{
        const contRangeProcSet& that = dynamic_cast<const contRangeProcSet&>(that_arg);
        valid = that.valid;
        emptyRange = that.emptyRange;
        cg = that.cg;
        cg->addVar(lb);
        cg->addVar(ub);
        // lb == that.lb
        cg->assertCond(lb, that.lb, 1, 1, 0); 
        cg->assertCond(that.lb, lb, 1, 1, 0); 
        // ub == that.ub
        cg->assertCond(ub, that.ub, 1, 1, 0); 
        cg->assertCond(that.ub, ub, 1, 1, 0); 
        
        return *this;
}

/*procSet& contRangeProcSet::operator=(const procSet& that_arg)
{
        const contRangeProcSet& that = dynamic_cast<const contRangeProcSet&>(that_arg);
        
        lb = that.lb;
        ub = that.ub;
        cg = that.cg;
        emptyRange = that.emptyRange;
        
        return *this;
}
bool contRangeProcSet::operator==(const procSet& that_arg)
{
        const contRangeProcSet& that = dynamic_cast<const contRangeProcSet&>(that_arg);
        
        return lb == that.lb &&
               ub == that.ub &&
               cg == that.cg &&
               emptyRange == that.emptyRange;
}*/


// Comparison
bool contRangeProcSet::operator==(const procSet& that_arg) const
{
        const contRangeProcSet& that = dynamic_cast<const contRangeProcSet&>(that_arg);
        return valid == that.valid &&
               emptyRange == that.emptyRange &&
               cg == that.cg &&
               cg->eqVars(lb, that.lb, "    ") && 
               cg->eqVars(ub, that.ub, "    ");
}

bool contRangeProcSet::operator<(const procSet& that_arg) const
{
        const contRangeProcSet& that = dynamic_cast<const contRangeProcSet&>(that_arg);
        if(!valid)
                return that.valid;
        else if(!that.valid)
                return false;
        else if(emptyRange)
                return !that.emptyRange;
        else if(that.emptyRange)
                return false;
        // Both sets are valid and not empty
        else
                return (lb<that.lb) ||
                       (lb==that.lb && ub<that.ub);
}

// Returns a string representation of this set
string contRangeProcSet::str(string indent)
{ 
        ostringstream outs;
        if(emptyRange)
        { outs << indent << "<contRangeProcSet: empty>"; }
        else if(!valid)
        { outs << indent << "<contRangeProcSet: invalid>"; }
        else
        { 
                outs << indent << "<contRangeProcSet: [";
                if(cg)
                {
                        outs << lb.str()<<"=";
                        map<varID, affineInequality> eqLB = cg->getEqVars(lb);
                        outs << "{" ;
                        if(eqLB.size()>0)
                        {
                                for(map<varID, affineInequality>::iterator it=eqLB.begin(); it!=eqLB.end(); )
                                {
                                        if(it->second.getA()!=1)
                                        {
                                                if(it->second.getB()!=1)
                                                {
                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
                                                        else outs << "("<<it->first.str()<<"*"<<it->second.getB()<<")/"<<it->second.getA();
                                                }
                                                else
                                                {
                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
                                                        else outs << "("<<it->first.str()<<")/"<<it->second.getA();
                                                }
                                        }
                                        else
                                        {
                                                if(it->second.getB()!=1)
                                                {
                                                        if(it->second.getC()!=0) outs << it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC();
                                                        else outs << it->first.str()<<"*"<<it->second.getB();
                                                }
                                                else
                                                {
                                                        if(it->second.getC()!=0) outs << it->first.str()<<"+"<<it->second.getC();
                                                        else outs << it->first.str();
                                                }
                                        }
                                        it++;
                                        if(it!=eqLB.end())
                                                outs << ",";
                                }
                        }
                        else
                                outs << lb.str();
                        
                        outs << "} - {";
                        
                        map<varID, affineInequality> eqUB = cg->getEqVars(ub);
                        if(eqUB.size()>0)
                        {
                                outs << ub.str()<<"=";
                                for(map<varID, affineInequality>::iterator it=eqUB.begin(); it!=eqUB.end(); )
                                {
                                        if(it->second.getA()!=1)
                                        {
                                                if(it->second.getB()!=1)
                                                {
                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
                                                        else outs << "("<<it->first.str()<<"*"<<it->second.getB()<<")/"<<it->second.getA();
                                                }
                                                else
                                                {
                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
                                                        else outs << "("<<it->first.str()<<")/"<<it->second.getA();
                                                }
                                        }
                                        else
                                        {
                                                if(it->second.getB()!=1)
                                                {
                                                        if(it->second.getC()!=0) outs << it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC();
                                                        else outs << it->first.str()<<"*"<<it->second.getB();
                                                }
                                                else
                                                {
                                                        if(it->second.getC()!=0) outs << it->first.str()<<"+"<<it->second.getC();
                                                        else outs << it->first.str();
                                                }
                                        }
                                        it++;
                                        if(it!=eqUB.end())
                                                outs << ",";
                                }
                        }
                        else
                                outs << ub.str();
                        outs << "}]>"; 
                }
                else
                {
                        outs << lb.str() << " - " << ub.str() << "]>";
                }
        }
        return Dbg::escape(outs.str());
}

string contRangeProcSet::str(string indent) const
{ 
        ostringstream outs;
        if(emptyRange)
        { outs << indent << "<contRangeProcSet: empty>"; }
        else if(!valid)
        { outs << indent << "<contRangeProcSet: invalid>"; }
        else
        { 
                outs << indent << "<contRangeProcSet: [";
                if(cg)
                {
                        outs << lb.str()<<":";
                        map<varID, affineInequality> eqLB = cg->getEqVars(lb);
                        outs << "{" ;
                        if(eqLB.size()>0)
                        {
                                for(map<varID, affineInequality>::iterator it=eqLB.begin(); it!=eqLB.end(); )
                                {
                                        if(it->second.getA()!=1)
                                        {
                                                if(it->second.getB()!=1)
                                                {
                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
                                                        else outs << "("<<it->first.str()<<"*"<<it->second.getB()<<")/"<<it->second.getA();
                                                }
                                                else
                                                {
                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
                                                        else outs << "("<<it->first.str()<<")/"<<it->second.getA();
                                                }
                                        }
                                        else
                                        {
                                                if(it->second.getB()!=1)
                                                {
                                                        if(it->second.getC()!=0) outs << it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC();
                                                        else outs << it->first.str()<<"*"<<it->second.getB();
                                                }
                                                else
                                                {
                                                        if(it->second.getC()!=0) outs << it->first.str()<<"+"<<it->second.getC();
                                                        else outs << it->first.str();
                                                }
                                        }
                                        it++;
                                        if(it!=eqLB.end())
                                                outs << ",";
                                }
                        }
                        else
                                outs << lb.str();
                        
                        outs << "} - {";
                        
                        map<varID, affineInequality> eqUB = cg->getEqVars(ub);
                        if(eqUB.size()>0)
                        {
                                outs << ub.str()<<":";
                                for(map<varID, affineInequality>::iterator it=eqUB.begin(); it!=eqUB.end(); )
                                {
                                        if(it->second.getA()!=1)
                                        {
                                                if(it->second.getB()!=1)
                                                {
                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
                                                        else outs << "("<<it->first.str()<<"*"<<it->second.getB()<<")/"<<it->second.getA();
                                                }
                                                else
                                                {
                                                        if(it->second.getC()!=0) outs << "("<<it->first.str()<<"+"<<it->second.getC()<<")/"<<it->second.getA();
                                                        else outs << "("<<it->first.str()<<")/"<<it->second.getA();
                                                }
                                        }
                                        else
                                        {
                                                if(it->second.getB()!=1)
                                                {
                                                        if(it->second.getC()!=0) outs << it->first.str()<<"*"<<it->second.getB()<<"+"<<it->second.getC();
                                                        else outs << it->first.str()<<"*"<<it->second.getB();
                                                }
                                                else
                                                {
                                                        if(it->second.getC()!=0) outs << it->first.str()<<"+"<<it->second.getC();
                                                        else outs << it->first.str();
                                                }
                                        }
                                        it++;
                                        if(it!=eqUB.end())
                                                outs << ",";
                                }
                        }
                        else
                                outs << ub.str();
                        outs << "}]>"; 
                }
                else
                {
                        outs << lb.str() << " - " << ub.str() << "]>";
                }
        }
        return Dbg::escape(outs.str());
}

// Removes the upper and lower bounds of this set from its associated constraint graph
// or the provided constraint graph (if it is not ==NULL) and sets cg to NULL if tgtCG==NULL.
// Returns true if this causes the constraint graph to change, false otherwise.
bool contRangeProcSet::cgDisconnect() 
{
        bool modified=false;
        
        if(emptySet() || !validSet()) return modified;
                
        if(cg!=NULL)
        {
                // Disconnect this process set's bounds from its constraint craph
                modified = cg->removeVar(getLB()) || modified;
                modified = cg->removeVar(getUB()) || modified;
                
                cg = NULL;
        }

        return modified;
}

bool contRangeProcSet::cgDisconnect(ConstrGraph* tgtCG) const
{
        bool modified=false;
        
        if(emptySet() || !validSet()) return modified;
        
        if(tgtCG!=NULL)
        {
                // Disconnect this process set's bounds from its constraint craph
                modified = tgtCG->removeVar(getLB()) || modified;
                modified = tgtCG->removeVar(getUB()) || modified;
        }
        return modified;
}

// ************************
// *** NodeFact methods ***
// ************************

// returns a copy of this node fact
NodeFact* contRangeProcSet::copy() const
{
        return new contRangeProcSet(*this);
}
