#include "divAnalysis.h"

#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

int divAnalysisDebugLevel=0;

// CURRENTLY THE DIVISIBILITY ANALYSIS CANNOT HANDLE NEGATIVE NUMBERS!

// GCD implementation from Wikipedia
long gcd(long u, long v)
{
        int shift;
        
        /* GCD(0,x) := x */
        if (u == 0 || v == 0)
                return u | v;
        
        /* Let shift := lg K, where K is the greatest power of 2
        dividing both u and v. */
        for (shift = 0; ((u | v) & 1) == 0; ++shift) {
                u >>= 1;
                v >>= 1;
        }
        
        while ((u & 1) == 0)
                u >>= 1;
        
        /* From here on, u is always odd. */
        do {
                while ((v & 1) == 0)  /* Loop X */
                v >>= 1;
                
                /* Now u and v are both odd, so diff(u, v) is even.
                Let u = min(u, v), v = diff(u, v)/2. */
                if (u <= v) {
                        v -= u;
                } else {
                        int diff = u - v;
                        u = v;
                        v = diff;
                }
                v >>= 1;
        } while (v != 0);
        
        return u << shift;
}

/**********************
 ***** DivLattice *****
 **********************/

// The different levels of this lattice
// no information is known about the value of the variable
const int DivLattice::bottom; 
// the value of the variable is known
const int DivLattice::valKnown; 
// value is unknown but the divisibility (div and rem) of this variable is known 
const int DivLattice::divKnown; 
// this variable holds more values than can be represented using a single value and divisibility
const int DivLattice::top; 


// returns a copy of this lattice
Lattice* DivLattice::copy() const
{
        return new DivLattice(*this);
}

// overwrites the state of this Lattice with that of that Lattice
void DivLattice::copy(Lattice* that_arg)
{
        DivLattice* that = dynamic_cast<DivLattice*>(that_arg);
        
        this->value = that->value;
        this->div   = that->div;
        this->rem   = that->rem;
        this->level = that->level;
}

// returns true if the given value matches the given div, rem combo and false otherwise
bool DivLattice::matchDiv(long value, long div, long rem)
{
        return (value % div) == rem;
}

// Takes two lattices at level divKnown. If the two objects have matching div, rem pairs, returns
// true and sets div and rem to those mathching values. Otherwise, returns false.
bool DivLattice::matchDiv(DivLattice* one, DivLattice* two, long& div, long& rem)
{
        // if these object have the same div
        if(one->div == two->div)
        {
                // if they have the same rem, we're cool
                if(one->rem == two->rem)
                {
                        div = one->div;
                        rem = one->rem;
                        return true;
                }
                // else, if the rems are different
                else
                {
                        int divremgcd;
                        // if one rem is 0 and the other 0 shares factors with the common div, those 
                        // factors can be the new common div
                        if(one->rem == 0 && (divremgcd = gcd(one->div, two->rem))>1)
                        {
                                div = divremgcd;
                                rem = 0;
                                return true;
                        }
                        else if(two->rem == 0 && (divremgcd = gcd(one->div, one->rem))>1)
                        {
                                div = divremgcd;
                                rem = 0;
                                return true;
                        }
                        // if the above doesn't work, the're not compatible
                        else
                                return false;
                }
        }
        // else, if these object have the different divs
        else
        {
                // but they're compatible because they have no remainders
                // and their divs are multiples of the same number
                if(one->rem==0 && two->rem==0)
                {
                        if(one->div > two->div && one->div % two->div==0)
                        {
                                // the common div belongs to two
                                div = two->div;
                                rem = 0;
                                return true;
                        }
                        else if(two->div > one->div && two->div % one->div==0)
                        {
                                // the common div belongs to one
                                div = one->div;
                                rem = 0;
                                return true;
                        }
                        // else, they're not compatible
                        else
                                return false;
                }
                // else, if they're not compatible
                else return false;
        }
}

// Takes two lattices at level divKnown. If the two objects have div, rem pairs that make it
// possible to add or subtract them them and produce div/rem information where div>1, 
// returns true and sets div and rem to correspond to the sum of these values.
// Otherwise, returns false.
// plus - true if the caller want to see one+two and false if one-two
bool DivLattice::matchDivAddSubt(DivLattice* one, DivLattice* two, long& div, long& rem, bool plus)
{
        // If these object have the same div
        if(one->div == two->div)
        {
                Dbg::dbg << "DivLattice::matchDivAddSubt() one="<<one->str("")<<" two="<<two->str("")<<"\n";
                div = one->div;
                if(plus) rem = (one->rem + two->rem)%div;
                else     rem = (one->rem - two->rem + div)%div;
                return true;
        }
        // Else, if these object have the different divs
        else
        {
                div = gcd(one->div, two->div);
                // If their divisors have a factor in common, their sum can be expressed using a divisor that has this factor
                if(div>1) {
                        if(plus) rem = (one->rem%div + two->rem%div)%div;
                        else     rem = (one->rem%div - two->rem%div + div)%div;
                        return true;
                }
                // Else, if they're not compatible
                else
                        return false;
        }
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool DivLattice::meetUpdate(Lattice* that_arg)
{
        long  oldValue = value;
        long  oldDiv = div;
        long  oldRem = rem;
        short oldLevel = level;
        DivLattice* that = dynamic_cast<DivLattice*>(that_arg);

/*Dbg::dbg << "DivLattice::meetUpdate\n";
Dbg::dbg << "this: " << str("") << "\n";
Dbg::dbg << "that: " << that->str("") << "\n";*/

        // if this object is bottom, just copy the state of that
        if(level==bottom)
          {
            if(that->level>bottom)
              copy(that);
            goto Done;
          }
        // else, if that is bottom, leave this alone since this is at least bottom
        else if(that->level==bottom)
          {
            goto Done;
          }
        // else, if both are above bottom, perform the meet
        else
          {
            // if the two objects have known values
            if(level==valKnown && that->level==valKnown)
              {
                // if they disagree on their values, move the state
                // of this object to divKnown
                if(value != that->value)
                  {
                    div = gcd(value, that->value);
                                        
                    // If the gcd is > 1, then we know a useful divisor and the level is divKnown
                    // otherwise, the divisor info is useless and the level becomes top
                    if(div != 1) {
                      level = divKnown;
                      rem = 0;
                      value = -1;
                    } else {
                      // If we cannot find a good divisor, we guess that the variable's new value
                      // was produced via an iteration where the variable's value is repeatedly
                      // incremented by a constant. If this is the case, the divisor is the difference
                      // between the two values. If this guess is wrong, the next time we update
                      // this lattice, we'll update it to top.
                      level = divKnown;
                      div = that->value - value;
                      rem = value % div;
                    }
                    goto Done;
                  }
                // else, if the two objects agree on their values, we can
                // leave this object alone
                else
                  goto Done;
              }
            else if(level==valKnown && that->level==divKnown)
              {
                //Dbg::dbg << "(level==valKnown && that->level==divKnown) value%that->div="<<(value%that->div)<<" that->rem="<<that->rem<<"\n";
                // if this can be divided by that->div to get that->rem as the remainder,
                // we can use that->div, that->rem to represent both objects
                if(value%that->div == that->rem)
                  {
                    value=0;
                    div = that->div;
                    rem = that->rem;
                    level = divKnown;
                    goto Done;
                  }
                // otherwise, the two objects are not compatible
              }
            else if(level==divKnown && that->level==valKnown)
              {
                //Dbg::dbg << "(level==divKnown && that->level==valKnown) that->value%div="<<that->value%div<<" == rem="<<rem<<"\n";
                // if this can be divided by that->div to get that->rem as the remainder,
                // we can use div, rem to represent both objects
                if(that->value%div == rem)
                  {
                    // leave this object alone
                    goto Done;
                  }
                // otherwise, the two objects are not compatible
              }
            else if(level==divKnown && that->level==divKnown)
              {
                long newDiv, newRem;
                bool match = matchDiv(this, that, newDiv, newRem);
                if(match)
                  {
                    div = newDiv;
                    rem = newRem;
                    goto Done;
                  }
              }
          }

        // if we haven't hit a case that goes to a non-top level, make this object top
        div = 1;
        rem = 0;
        value = 0;
        level = top;
        
        // the various cases that do not result in this object becoming top goto this label
        Done:

        // return whether this object was modified
        return (oldValue != value) ||
               (oldDiv != div) ||
               (oldRem != rem) ||
               (oldLevel != level);
}

// computes the meet of this and that and returns the result
/*Lattice* meet(Lattice* that)
{
        DivLattice* newDL = new DivLattice(*this);
        newDL->meetUpdate(that);
}*/

bool DivLattice::operator==(Lattice* that_arg)
{
        DivLattice* that = dynamic_cast<DivLattice*>(that_arg);
        
        /*Dbg::dbg << "operator == ("<<str()<<", "<<that->str()<<"): "<<((value == that->value) &&
               (div == that->div) &&
               (rem == that->rem) &&
               (level == that->level))<<"\n";*/
        
        return (value == that->value) &&
               (div == that->div) &&
               (rem == that->rem) &&
               (level == that->level);
}

/*// widens this from that and saves the result in this
// returns true if this causes this to change and false otherwise
bool DivLattice::widenUpdate(InfiniteLattice* that)
{
        return meetUpdate(that);
}*/

// returns the current state of this object
long DivLattice::getValue() const
{ return value; }

long DivLattice::getDiv() const
{ return div; }

long DivLattice::getRem() const
{ return rem; }

short DivLattice::getLevel() const
{ return level; }
        

// Sets the state of this lattice to bottom
// returns true if this causes the lattice's state to change, false otherwise
bool DivLattice::setBot()
{
        bool modified = this->level != bottom;
        this->value = 0;
        div = -1;
        rem = -1;
        level = bottom;
        return modified;
}

// Sets the state of this lattice to the given value.
// returns true if this causes the lattice's state to change, false otherwise
bool DivLattice::set(long value)
{
        bool modified = this->level != valKnown || this->value != value;
        this->value = value;
        div = -1;
        rem = -1;
        level = valKnown;
        return modified;
}

// Sets the state of this lattice to the given div/rem state.
// returns true if this causes the lattice's state to change, false otherwise
bool DivLattice::set(long div, long rem)
{
        bool modified = this->level != divKnown || this->div != div || this->rem != rem;
        value = 0;
        this->div = div;
        this->rem = rem;
        level = divKnown;
        return modified;
}

// Sets the state of this lattice to top
// returns true if this causes the lattice's state to change, false otherwise
bool DivLattice::setTop()
{
        bool modified = this->level != top;
        this->value = 0;
        div = 1;
        rem = 0;
        level = top;
        return modified;        
}

// Increments the state of this object by value
// returns true if this causes the lattice's state to change, false otherwise
bool DivLattice::incr(long increment)
{
        if(level == valKnown)
                value += increment;
        else if(level == divKnown)
                rem = (rem+increment)%div;
        else
                return false;
        
        return increment!=0;
}

// Multiplies the state of this object by value
// returns true if this causes the lattice's state to change, false otherwise
bool DivLattice::mult(long multiplier)
{
        if(level == valKnown)
                value *= multiplier;
        else if(level == divKnown)
        {
                // (m * div + rem) * c ~ m * (div * c) + rem*c
                div *= multiplier;
                rem *= multiplier;
        }
        else
                return false;
        
        return multiplier!=1;
}
        
string DivLattice::str(string indent)
{
        ostringstream outs;
        if(level == bottom)
                outs << indent << "[level: bottom]";
        else if(level == valKnown)
                outs << indent << "[level: valKnown, val="<<value<<"]";
        else if(level == divKnown)
                outs << indent << "[level: divKnown, div="<<div<<", rem="<<rem<<"]";
        else if(level == top)
                outs << indent << "[level: top]";
        return outs.str();
}

/***********************
 ***** DivAnalysis *****
 ***********************/

map<varID, Lattice*> DivAnalysis::constVars;
bool DivAnalysis::constVars_init=false;

/*// generates the initial variable-specific lattice state for a dataflow node
Lattice* DivAnalysis::genInitVarState(const Function& func, const DataflowNode& n, const NodeState& state)
{
        DivLattice* newLattice = new DivLattice();
        printf("DivLattice::genInitNonVarState() newLattice=%p\n", newLattice);
        printf("DivLattice::genInitNonVarState() newLattice=%s\n", newLattice->str().c_str());
        return newLattice;
}

// generates the initial non-variable-specific lattice state for a dataflow node
Lattice* DivAnalysis::genInitNonVarState(const Function& func, const DataflowNode& n, const NodeState& state)
{
        return NULL;
}*/

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> DivAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void DivAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                               vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
        //vector<Lattice*> initLattices;
        map<varID, Lattice*> emptyM;
        FiniteVarsExprsProductLattice* l = new FiniteVarsExprsProductLattice((Lattice*)new DivLattice(), emptyM/*genConstVarLattices()*/, 
                                                                             (Lattice*)NULL, ldva, /*func, */n, state);         
        //Dbg::dbg << "DivAnalysis::genInitState, returning l="<<l<<" n=<"<<Dbg::escape(n.getNode()->unparseToString())<<" | "<<n.getNode()->class_name()<<" | "<<n.getIndex()<<">\n";
        //Dbg::dbg << "    l="<<l->str("    ")<<"\n";
        initLattices.push_back(l);
        
        
        
/*printf("DivAnalysis::genInitState() initLattices:\n");
for(vector<Lattice*>::iterator it = initLattices.begin(); 
    it!=initLattices.end(); it++)
{       
        Dbg::dbg << *it << ": " << (*it)->str("    ") << "\n";
}*/
        
        //return initLattices;
}

// Returns a map of special constant variables (such as zeroVar) and the lattices that correspond to them
// These lattices are assumed to be constants: it is assumed that they are never modified and it is legal to 
//    maintain only one copy of each lattice may for the duration of the analysis.
/*map<varID, Lattice*>& DivAnalysis::genConstVarLattices() const
{
        if(!constVars_init)
        {
                DivLattice* newL = new DivLattice();
                newL->set(0);
                constVars[zeroVar] = newL;
                
                newL = new DivLattice();
                newL->set(1);
                constVars[oneVar] = newL;

                constVars_init = true;
                / *
                Dbg::dbg << "constVars:\n";
                for(map<varID, Lattice*>::iterator it = constVars.begin(); it!=constVars.end(); it++)
                { Dbg::dbg << it->first.str() << ": " << it->second->str("") << "\n"; }
                
                printf("oneVar == zeroVar = %d\n", oneVar == zeroVar);
                printf("oneVar < zeroVar = %d\n", oneVar < zeroVar);
                printf("zeroVar < oneVar = %d\n", zeroVar < oneVar);* /
        }

        return constVars;
}*/

DivAnalysisTransfer::DivAnalysisTransfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
  : VariableStateTransfer<DivLattice>(func, n, state, dfInfo, divAnalysisDebugLevel)
{ }

// Integral Numeric Constants
template <class T>
void DivAnalysisTransfer::visitIntegerValue(T *sgn)
{
  DivLattice* resLat = getLattice(sgn);

  // If the result expression is live
/// !!! ADD MODIFICATION DETECTION
  if (resLat)
    updateModified(resLat->set(sgn->get_value()));
}
void DivAnalysisTransfer::visit(SgLongLongIntVal *sgn)          { visitIntegerValue(sgn); }
void DivAnalysisTransfer::visit(SgLongIntVal *sgn)              { visitIntegerValue(sgn); }
void DivAnalysisTransfer::visit(SgIntVal *sgn)                  { visitIntegerValue(sgn); }
void DivAnalysisTransfer::visit(SgShortVal *sgn)                { visitIntegerValue(sgn); }
void DivAnalysisTransfer::visit(SgUnsignedLongLongIntVal *sgn)  { visitIntegerValue(sgn); }
void DivAnalysisTransfer::visit(SgUnsignedLongVal *sgn)         { visitIntegerValue(sgn); }
void DivAnalysisTransfer::visit(SgUnsignedIntVal *sgn)          { visitIntegerValue(sgn); }
void DivAnalysisTransfer::visit(SgUnsignedShortVal *sgn)        { visitIntegerValue(sgn); }

// Non-integral Constants
void DivAnalysisTransfer::visit(SgValueExp *sgn) {
  DivLattice* resLat = getLattice(sgn);
  // If the result expression is live
  if(resLat) updateModified(resLat->setTop());
}

void DivAnalysisTransfer::transferAdditive(DivLattice *arg1Lat, DivLattice *arg2Lat, DivLattice *resLat, bool isAddition) {
  int arg1Level = arg1Lat->getLevel(), arg2Level = arg2Lat->getLevel();

  // Either one Bottom
  if (arg1Level == DivLattice::bottom        || arg2Level == DivLattice::bottom) {
    updateModified(resLat->setBot());
    return;
  }

  if (arg1Level == DivLattice::top || arg2Level == DivLattice::top) {
    updateModified(resLat->setTop());
    return;
  }

  if (arg1Level == DivLattice::valKnown && arg2Level == DivLattice::valKnown) {
    long arg1Val = arg1Lat->getValue();
    long arg2Val = arg2Lat->getValue() * (isAddition ? 1 : -1);
    updateModified(resLat->set(arg1Val + arg2Val));
    return;
  }

  if(arg1Level == DivLattice::divKnown && arg2Level == DivLattice::divKnown) {
    long newDiv, newRem;
    
    if(DivLattice::matchDivAddSubt(arg1Lat, arg2Lat, newDiv, newRem, isAddition)) {
      updateModified(resLat->set(newDiv, newRem));
    } else
      updateModified(resLat->setTop());
  }

  // Arg1 ValKnown, Arg2 DivKnown
  /// XXX: Copy result from DivKnown, then increment by value of ValKnown. Need to fix DivLattice::incr to handle negatives correctly
  if(arg1Level == DivLattice::valKnown && arg2Level == DivLattice::divKnown) {
    long rem = (isAddition 
                ? arg1Lat->getValue() + arg2Lat->getRem()
                : arg1Lat->getValue() % arg2Lat->getDiv() - arg2Lat->getRem() + arg2Lat->getDiv())
      % arg2Lat->getDiv();
    updateModified(resLat->set(arg2Lat->getDiv(), rem));
  }
  // Arg1 DivKnown, Arg2 ValKnown
  else if(arg1Level == DivLattice::divKnown && arg2Level == DivLattice::valKnown) {
    long rem = (isAddition ?
                (arg2Lat->getValue() + arg1Lat->getRem()) :
                (arg1Lat->getRem() - (arg2Lat->getValue()%arg1Lat->getDiv()) + arg2Lat->getDiv()))
      % arg1Lat->getDiv();

    updateModified(resLat->set(arg1Lat->getDiv(), rem));
  }
  else   // Else => Top
    updateModified(resLat->setTop());
}
void DivAnalysisTransfer::visit(SgPlusAssignOp *sgn)  { transferArith(sgn, boost::bind(&DivAnalysisTransfer::transferAdditive, _1, _2, _3, _4, true )); }
void DivAnalysisTransfer::visit(SgMinusAssignOp *sgn) { transferArith(sgn, boost::bind(&DivAnalysisTransfer::transferAdditive, _1, _2, _3, _4, false)); }
void DivAnalysisTransfer::visit(SgAddOp *sgn)         { transferArith(sgn, boost::bind(&DivAnalysisTransfer::transferAdditive, _1, _2, _3, _4, true )); }
void DivAnalysisTransfer::visit(SgSubtractOp *sgn)    { transferArith(sgn, boost::bind(&DivAnalysisTransfer::transferAdditive, _1, _2, _3, _4, false)); }

void DivAnalysisTransfer::transferIncrement(SgUnaryOp *sgn) {
  DivLattice *arg1Lat, *arg2Lat, *resLat;
  getLattices(sgn, arg1Lat, arg2Lat, resLat);

  if (arg1Lat == NULL) {
    ROSE_ASSERT(resLat == NULL);
    return;
  }
  
  //long increment = isSgPlusPlusOp(sgn) ? 1 : -1;

  if (sgn->get_mode() == SgUnaryOp::prefix) {
    //arg1Lat->incr(increment);
    transferAdditive(arg1Lat, arg2Lat, arg1Lat, isSgPlusPlusOp(sgn));
    if (resLat != NULL)
      resLat->copy(arg1Lat);
  }
  else {
    if (resLat != NULL)
      resLat->copy(arg1Lat);
    transferAdditive(arg1Lat, arg2Lat, arg1Lat, isSgPlusPlusOp(sgn));
    //arg1Lat->incr(increment);
  }

  delete arg2Lat;
}

void DivAnalysisTransfer::visit(SgPlusPlusOp *sgn) { transferIncrement(sgn); }
void DivAnalysisTransfer::visit(SgMinusMinusOp *sgn) { transferIncrement(sgn); }

void DivAnalysisTransfer::visit(SgUnaryAddOp *sgn) {
  DivLattice *arg1Lat, *arg2Lat = NULL, *resLat;
  getLattices(sgn, arg1Lat, arg2Lat, resLat);
  ROSE_ASSERT(arg1Lat != NULL);
  ROSE_ASSERT(resLat != NULL);
  resLat->copy(arg1Lat);
  modified = true;
  if (arg2Lat != NULL) {
    delete arg2Lat;
  }
}

void DivAnalysisTransfer::visit(SgMinusOp *sgn) {
  DivLattice *arg1Lat, *arg2Lat = NULL, *resLat;
  getLattices(sgn, arg1Lat, arg2Lat, resLat);
  ROSE_ASSERT(arg1Lat != NULL);
  ROSE_ASSERT(resLat != NULL);
  resLat->copy(arg1Lat);
  modified = true;
  if(resLat->getLevel() == DivLattice::valKnown)
    resLat->set(0 - resLat->getValue());
  else if(resLat->getLevel() == DivLattice::divKnown)
    resLat->set(resLat->getDiv(), (resLat->getDiv() - resLat->getRem()) % resLat->getDiv());
  if (arg2Lat != NULL) {
    delete arg2Lat;
  }
}

void DivAnalysisTransfer::transferMultiplicative(DivLattice *arg1Lat, DivLattice *arg2Lat, DivLattice *resLat) {
  if(divAnalysisDebugLevel>=1) Dbg::dbg << "   case i = j * k\n";
          /*printf("arg1Lat = %s\n", arg1Lat->str().c_str());
    printf("arg2Lat = %s\n", arg2Lat->str().c_str());*/
                                
  // Both Bottom
  if(arg1Lat->getLevel() == DivLattice::bottom || arg2Lat->getLevel() == DivLattice::bottom)
    updateModified(resLat->setBot());
  // Both ValKnown
  else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::valKnown)
    updateModified(resLat->set(arg1Lat->getValue() * arg2Lat->getValue()));
  // Arg1 ValKnown, Arg2 DivKnown, Arg1
  else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::divKnown)
    {
      // (m*k.div + k.rem)*j.val*c = (m*(k.div*j.val*c) + k.rem*j.val*c) = 
      updateModified(resLat->set(arg2Lat->getDiv(), arg2Lat->getRem()));
      updateModified(resLat->mult(arg1Lat->getValue()));
    }
  // Arg1 DivKnown, Arg2 ValKnown
  else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::valKnown)
    {
      // (m*j.div + j.rem)*k.val*c = (m*(j.div*k.val*c) + j.rem*k.val*c) = 
      updateModified(resLat->set(arg1Lat->getDiv(), arg1Lat->getRem()));
      updateModified(resLat->mult(arg2Lat->getValue()));
    }
  // Both DivKnown => Top
  else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::divKnown)
    {
      updateModified(resLat->setTop());
    }
  // Else => Top
  else
    updateModified(resLat->setTop());
}

void DivAnalysisTransfer::visit(SgMultiplyOp *sgn) { transferArith(sgn, &DivAnalysisTransfer::transferMultiplicative); }
void DivAnalysisTransfer::visit(SgMultAssignOp *sgn) { transferArith(sgn, &DivAnalysisTransfer::transferMultiplicative); }

void DivAnalysisTransfer::transferDivision(DivLattice *arg1Lat, DivLattice *arg2Lat, DivLattice *resLat) {
  if(divAnalysisDebugLevel>=1) Dbg::dbg << "   case i = j / k\n";
                        
  // Both Bottom
  if(arg1Lat->getLevel() == DivLattice::bottom || arg2Lat->getLevel() == DivLattice::bottom)
    updateModified(resLat->setBot());
  // Both ValKnown
  else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::valKnown)
    updateModified(resLat->set(arg1Lat->getValue() / arg2Lat->getValue()));
  // Arg1 ValKnown, Arg2 DivKnown, Arg1 divisible by Arg2 Div
  else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::divKnown &&
          arg2Lat->getRem()==0 && arg1Lat->getValue()%arg2Lat->getDiv() == 0)
    updateModified(resLat->set(arg1Lat->getValue()/arg2Lat->getDiv()));
  // Arg1 DivKnown, Arg2 ValKnown, Arg1 Div/Rem divisible by Arg2
  else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::valKnown &&
          (arg1Lat->getDiv()%arg2Lat->getValue() == 0) && (arg1Lat->getRem()%arg2Lat->getValue() == 0))
    updateModified(resLat->set(arg1Lat->getDiv()/arg2Lat->getValue(), arg1Lat->getRem()/arg2Lat->getValue()));
  // Both DivKnown, Arg1 Div/Rem divisible by Arg2 Div
  else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::divKnown &&
          arg2Lat->getRem()==0 && (arg1Lat->getDiv()%arg2Lat->getDiv() == 0) && (arg1Lat->getRem()%arg2Lat->getDiv() == 0))
    updateModified(resLat->set(arg1Lat->getDiv()/arg2Lat->getDiv(), arg1Lat->getRem()/arg2Lat->getDiv()));
  // Else => Top
  else
    updateModified(resLat->setTop());
}
void DivAnalysisTransfer::visit(SgDivideOp *sgn) { transferArith(sgn, &DivAnalysisTransfer::transferDivision); }
void DivAnalysisTransfer::visit(SgDivAssignOp *sgn) { transferArith(sgn, &DivAnalysisTransfer::transferDivision); }

template <typename T>
void DivAnalysisTransfer::transferArith(SgBinaryOp *sgn, T transferOp) {
  DivLattice *arg1Lat, *arg2Lat, *resLat;
  if (getLattices(sgn, arg1Lat, arg2Lat, resLat)) {
    transferOp(this, arg1Lat, arg2Lat, resLat);
    if (isSgCompoundAssignOp(sgn))
      arg1Lat->copy(resLat);
  }
}
void DivAnalysisTransfer::transferArith(SgBinaryOp *sgn, TransferOp transferOp) { transferArith(sgn, boost::mem_fn(transferOp)); }

void DivAnalysisTransfer::transferMod(DivLattice *arg1Lat, DivLattice *arg2Lat, DivLattice *resLat) {
  if(divAnalysisDebugLevel>=1) Dbg::dbg << "   case i = j %% k\n";
                                
  // Both Bottom
  if(arg1Lat->getLevel() == DivLattice::bottom || arg2Lat->getLevel() == DivLattice::bottom)
    updateModified(resLat->setBot());
  // Both ValKnown
  else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::valKnown)
    updateModified(resLat->set(arg1Lat->getValue() % arg2Lat->getValue()));
  // Arg1 ValKnown, Arg2 DivKnown
  else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::divKnown &&
          arg2Lat->getRem()==0 && arg1Lat->getValue()%arg2Lat->getDiv() == 0)
    updateModified(resLat->setTop());
  // Arg1 DivKnown, Arg2 ValKnown, Arg1 Div divisible by Arg2
  else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::valKnown &&
          (arg1Lat->getDiv()%arg2Lat->getValue() == 0))
    updateModified(resLat->set(arg1Lat->getRem() % arg2Lat->getValue()));
  // Both DivKnown
  else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::divKnown &&
          arg2Lat->getRem()==0 && (arg1Lat->getDiv()%arg2Lat->getDiv() == 0) && (arg1Lat->getRem()%arg2Lat->getDiv() == 0))
    updateModified(resLat->setTop());
  // Else => Top
  else
    updateModified(resLat->setTop());
}
void DivAnalysisTransfer::visit(SgModOp *sgn) { transferArith(sgn, &DivAnalysisTransfer::transferMod); }
void DivAnalysisTransfer::visit(SgModAssignOp *sgn) { transferArith(sgn, &DivAnalysisTransfer::transferMod); }

// prints the Lattices set by the given DivAnalysis 
void printDivAnalysisStates(DivAnalysis* da, string indent)
{
        vector<int> factNames;
        vector<int> latticeNames;
        latticeNames.push_back(0);
        printAnalysisStates pas(da, factNames, latticeNames, printAnalysisStates::below, indent);
        UnstructuredPassInterAnalysis upia_pas(pas);
        upia_pas.runAnalysis();
}
