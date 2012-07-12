#include "constantPropagation.h"

#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

int constantPropagationAnalysisDebugLevel = 2;

// **********************************************************************
//                      ConstantPropagationLattice
// **********************************************************************

ConstantPropagationLattice::ConstantPropagationLattice()
   {
     this->value = 0;
     this->level = bottom;
   }

ConstantPropagationLattice::ConstantPropagationLattice( int v )
   {
     this->value = v;
     this->level = constantValue;
   }

ConstantPropagationLattice::ConstantPropagationLattice( short level, int v )
   {
     this->value = v;
     this->level = level;
   }

// This is the same as the implicit definition, so it might not be required to be defined explicitly.
// I am searching for the minimal example of the use of the data flow classes.
ConstantPropagationLattice::ConstantPropagationLattice (const ConstantPropagationLattice & X)
   {
     this->value = X.value;
     this->level = X.level;
   }

int
ConstantPropagationLattice::getValue() const
   {
     return value;
   }

short
ConstantPropagationLattice::getLevel() const
   {
     return level;
   }
	
bool
ConstantPropagationLattice::setValue(int x)
   {
  // These are more than access functions, they return if the state of the lattice has changed.
     bool modified = this->level != constantValue || this->value != value;
     this->value = x;
     level = constantValue;
     return modified;
   }

bool
ConstantPropagationLattice::setLevel(short x)
   {
  // These are more than access functions, they return if the state of the lattice has changed.
     bool modified = this->level != x;
     level = x;
     return modified;
   }

bool
ConstantPropagationLattice::setBottom()
   {
  // These are more than access functions, they return if the state of the lattice has changed.
     bool modified = this->level != bottom;
     this->value = 0;
     level = bottom;
     return modified;
   }

bool
ConstantPropagationLattice::setTop()
   {
  // These are more than access functions, they return if the state of the lattice has changed.
     bool modified = this->level != bottom;
     this->value = 0;
     level = top;
     return modified;
   }

// **********************************************
// Required definition of pure virtual functions.
// **********************************************
void
ConstantPropagationLattice::initialize()
   {
  // Use the default constructor (implemented above).
  // So nothing to do here.
   }


// returns a copy of this lattice
Lattice*
ConstantPropagationLattice::copy() const
   {
     return new ConstantPropagationLattice(*this);
   }


// overwrites the state of "this" Lattice with "that" Lattice
void
ConstantPropagationLattice::copy(Lattice* X)
   {
     ConstantPropagationLattice* that = dynamic_cast<ConstantPropagationLattice*>(X);

     this->value = that->value;
     this->level = that->level;
   }


bool
ConstantPropagationLattice::operator==(Lattice* X) /*const*/
   {
  // Implementation of equality operator.
     ConstantPropagationLattice* that = dynamic_cast<ConstantPropagationLattice*>(X);
     return (value == that->value) && (level == that->level);
   }


string
ConstantPropagationLattice::str(string indent)
   {
     ostringstream outs;
     if(level == bottom)
       outs << indent << "ConstantPropagationLattice:[level: bottom]";
     else if(level == constantValue)
       outs << indent << "ConstantPropagationLattice:[level: constantValue, val = "<<value<<"]";
     else if(level == top)
       outs << indent << "ConstantPropagationLattice:[level: top]";

     return outs.str();
   }

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
/*
// The different levels of this lattice

// no information is known about the value of the variable
static const short bottom = 1; // start point

static const short constantValue = 2;

static const short top = 3;
*/
bool
ConstantPropagationLattice::meetUpdate(Lattice* X)
   {
     ConstantPropagationLattice* that = dynamic_cast<ConstantPropagationLattice*>(X);

// Liao, 6/30/2012, clean up the meet function logic.
// we only need a three level lattice: 1) bottom (unknown, or no information, start state) -> 2) constant value -> 3)top (stop state)
//To write a complete and exhaustive meet function, 
//   for each state: 
//     enumerate all possible meet scenarios between the current state and any other states

       if (this->level == bottom) // 1. can only go up: must be the value of that lattice since this lattice is already the bottom
       {
         // all three combinations are handled the same way: that could be bottom, constantValue, or even top
        this->level = that->level;
        this->value = that->value;

        return (that->level != bottom);
       }
       else if (this->level == constantValue)
       { 
         // three combinations:  
        // 1. that lattice is at bottom, the constant value rules, no change
        if (that->level == bottom)
        {
          return false;
        }
        // 2. that lattice is a constant value
        else if (that->level == constantValue)
        {
          if (this->value == that->value) // same value, no change
          {
            return false;
          }
          else   // conflicting values, move to top and change state
          {
            this->level = top;
            return true;
          }
        } 
        // 3. that lattice is at top state
        else if (that->level == top) // top always rules
        {
           this->level = top;  
           return true;
        }
       }
       else if (this->level == top) //5.  already at the top. Cannot go up further.
       {
         // all combinations will result the same result. No change to this lattice
        return false;
       }
       else
       {
         cerr<<"error: ConstantPropagationLattice::meetUpdate() reached invalid branch!"<<endl;
         ROSE_ASSERT(false);
       }
    
    // We should have handled all possible combinations before this point
    cerr<<"error: ConstantPropagationLattice::meetUpdate() reached invalid branch!"<<endl;
    ROSE_ASSERT(false);
  // Make up a return value for now.
     return false;
   }



// **********************************************************************
//                  ConstantPropagationAnalysisTransfer
// **********************************************************************

template <typename T>
void ConstantPropagationAnalysisTransfer::transferArith(SgBinaryOp *sgn, T transferOp)
   {
     ConstantPropagationLattice *arg1Lat, *arg2Lat, *resLat;
     if (getLattices(sgn, arg1Lat, arg2Lat, resLat))
        {
          transferOp(this, arg1Lat, arg2Lat, resLat);
          if (isSgCompoundAssignOp(sgn))
               arg1Lat->copy(resLat);
        }
   }

void
ConstantPropagationAnalysisTransfer::transferArith(SgBinaryOp *sgn, TransferOp transferOp)
   {
     transferArith(sgn, boost::mem_fn(transferOp));
   }

void 
ConstantPropagationAnalysisTransfer::transferIncrement(SgUnaryOp *sgn)
   {
     ConstantPropagationLattice *arg1Lat, *arg2Lat = NULL, *resLat;
     if (getLattices(sgn, arg1Lat, arg2Lat, resLat))
         transferAdditive(arg1Lat, arg2Lat, resLat, isSgPlusPlusOp(sgn));
     delete arg2Lat; // Allocated by getLattices
   }

//! transfer addition or subtraction operations: isAddition: true for addition, false for subtraction
void
ConstantPropagationAnalysisTransfer::transferAdditive(ConstantPropagationLattice *arg1Lat, ConstantPropagationLattice *arg2Lat, ConstantPropagationLattice *resLat, bool isAddition)
   {
     if (arg1Lat->getLevel() == ConstantPropagationLattice::bottom || arg2Lat->getLevel() == ConstantPropagationLattice::bottom)
        {
          updateModified(resLat->setLevel(ConstantPropagationLattice::bottom));
        }
       else 
        {
       // Both knownValue
          if(arg1Lat->getLevel() == ConstantPropagationLattice::constantValue && arg2Lat->getLevel() == ConstantPropagationLattice::constantValue) 
             {
               updateModified(resLat->setValue(isAddition ? arg1Lat->getValue() + arg2Lat->getValue() : arg1Lat->getValue() - arg2Lat->getValue()));
             }
            else
             {
            // Else => Top
               updateModified(resLat->setLevel(ConstantPropagationLattice::top));
             }
        }
   }


void
ConstantPropagationAnalysisTransfer::transferMultiplicative(ConstantPropagationLattice *arg1Lat, ConstantPropagationLattice *arg2Lat, ConstantPropagationLattice *resLat)
   {
     if (arg1Lat->getLevel() == ConstantPropagationLattice::bottom || arg2Lat->getLevel() == ConstantPropagationLattice::bottom)
        {
          updateModified(resLat->setLevel(ConstantPropagationLattice::bottom));
        }
       else 
        {
       // Both knownValue
          if(arg1Lat->getLevel() == ConstantPropagationLattice::constantValue && arg2Lat->getLevel() == ConstantPropagationLattice::constantValue) 
             {
               updateModified(resLat->setValue(arg1Lat->getValue() * arg2Lat->getValue()));
             }
            else
             {
            // Else => Top
               updateModified(resLat->setLevel(ConstantPropagationLattice::top));
             }
        }
   }

void
ConstantPropagationAnalysisTransfer::transferDivision(ConstantPropagationLattice *arg1Lat, ConstantPropagationLattice *arg2Lat, ConstantPropagationLattice *resLat)
   {
     if (arg1Lat->getLevel() == ConstantPropagationLattice::bottom || arg2Lat->getLevel() == ConstantPropagationLattice::bottom)
        {
          updateModified(resLat->setLevel(ConstantPropagationLattice::bottom));
        }
       else 
        {
       // Both knownValue
          if(arg1Lat->getLevel() == ConstantPropagationLattice::constantValue && arg2Lat->getLevel() == ConstantPropagationLattice::constantValue) 
             {
               updateModified(resLat->setValue(arg1Lat->getValue() / arg2Lat->getValue()));
             }
            else
             {
            // Else => Top
               updateModified(resLat->setLevel(ConstantPropagationLattice::top));
             }
        }
   }

//! transfer the % operation
void
ConstantPropagationAnalysisTransfer::transferMod(ConstantPropagationLattice *arg1Lat, ConstantPropagationLattice *arg2Lat, ConstantPropagationLattice *resLat)
   {
     if (arg1Lat->getLevel() == ConstantPropagationLattice::bottom || arg2Lat->getLevel() == ConstantPropagationLattice::bottom)
        {
          updateModified(resLat->setLevel(ConstantPropagationLattice::bottom));
        }
       else 
        {
       // Both knownValue
          if(arg1Lat->getLevel() == ConstantPropagationLattice::constantValue && arg2Lat->getLevel() == ConstantPropagationLattice::constantValue) 
             {
               updateModified(resLat->setValue(arg1Lat->getValue() % arg2Lat->getValue()));
             }
            else
             {
            // Else => Top
               updateModified(resLat->setLevel(ConstantPropagationLattice::top));
             }
        }
   }

void
ConstantPropagationAnalysisTransfer::visit(SgLongLongIntVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgLongIntVal *sgn)
   {
   //TODO: similar logic as visit(SgIntVal *sgn)
   }

void
ConstantPropagationAnalysisTransfer::visit(SgIntVal *sgn)
   {
     ROSE_ASSERT(sgn != NULL);
     ConstantPropagationLattice* resLat = getLattice(sgn);
     ROSE_ASSERT(resLat != NULL);
     resLat->setValue(sgn->get_value());
     resLat->setLevel(ConstantPropagationLattice::constantValue);
   }

void
ConstantPropagationAnalysisTransfer::visit(SgShortVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgUnsignedLongLongIntVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgUnsignedLongVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgUnsignedIntVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgUnsignedShortVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgValueExp *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgPlusAssignOp *sgn)
   {
     transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferAdditive, _1, _2, _3, _4, true ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgMinusAssignOp *sgn)
   {
     transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferAdditive, _1, _2, _3, _4, false));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgMultAssignOp *sgn)
   {
     transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferMultiplicative, _1, _2, _3, _4 ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgDivAssignOp *sgn)
   {
     transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferDivision, _1, _2, _3, _4 ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgModAssignOp *sgn)
   {
     transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferMod, _1, _2, _3, _4 ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgAddOp *sgn)
   {
     transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferAdditive, _1, _2, _3, _4, true ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgSubtractOp *sgn)
   {
     transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferAdditive, _1, _2, _3, _4, false));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgMultiplyOp *sgn)
   {
     transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferMultiplicative, _1, _2, _3, _4 ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgDivideOp *sgn)
   {
     transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferDivision, _1, _2, _3, _4 ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgModOp *sgn)
   {
     transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferMod, _1, _2, _3, _4 ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgPlusPlusOp *sgn)
   {
     transferIncrement(sgn);
   }

void
ConstantPropagationAnalysisTransfer::visit(SgMinusMinusOp *sgn)
   {
     transferIncrement(sgn);
   }

void
ConstantPropagationAnalysisTransfer::visit(SgUnaryAddOp *sgn)
   {
     ConstantPropagationLattice* resLat = getLattice(sgn);
     resLat->copy(getLattice(sgn->get_operand()));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgMinusOp *sgn)
   {
     ConstantPropagationLattice* resLat = getLattice(sgn);

  // This sets the level
     resLat->copy(getLattice(sgn->get_operand()));

  // This fixes up the value if it is relevant (where level is neither top not bottom).
     resLat->setValue(-resLat->getValue());
   }

bool
ConstantPropagationAnalysisTransfer::finish()
   {
     return modified;
   }

ConstantPropagationAnalysisTransfer::ConstantPropagationAnalysisTransfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo)
   : VariableStateTransfer<ConstantPropagationLattice>(func, n, state, dfInfo, constantPropagationAnalysisDebugLevel)
   {
   }




// **********************************************************************
//                     ConstantPropagationAnalysis
// **********************************************************************

ConstantPropagationAnalysis::ConstantPropagationAnalysis(LiveDeadVarsAnalysis* ldva)
   {
     this->ldva = ldva;
   }

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
void
ConstantPropagationAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state, std::vector<Lattice*>& initLattices, std::vector<NodeFact*>& initFacts)
   {
  // ???
  // vector<Lattice*> initLattices;
	map<varID, Lattice*> emptyM;
	// the finite vars exprs product lattice is initialized based on the result of liveness analysis (ldva), but why???
	FiniteVarsExprsProductLattice* l = new FiniteVarsExprsProductLattice((Lattice*)new ConstantPropagationLattice(), emptyM/*genConstVarLattices()*/, 
	                                                                     (Lattice*)NULL, ldva, /*func, */n, state);         
// Liao, 7/1/2012. I don't think constant propagation's lattice initialization should be based on live variables only. So pass NULL to ldva.
//     	                                                                     (Lattice*)NULL, NULL, /*func, */n, state);         
	//Dbg::dbg << "DivAnalysis::genInitState, returning l="<<l<<" n=<"<<Dbg::escape(n.getNode()->unparseToString())<<" | "<<n.getNode()->class_name()<<" | "<<n.getIndex()<<">\n";
	//Dbg::dbg << "    l="<<l->str("    ")<<"\n";
     initLattices.push_back(l);
   }

	
bool
ConstantPropagationAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo)
   {
     assert(0); 
     return false;
   }

boost::shared_ptr<IntraDFTransferVisitor>
ConstantPropagationAnalysis::getTransferVisitor(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo)
   {
  // Why is the boost shared pointer used here?
     return boost::shared_ptr<IntraDFTransferVisitor>(new ConstantPropagationAnalysisTransfer(func, n, state, dfInfo));
   }

