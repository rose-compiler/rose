#include "taintedFlowAnalysis.h"

#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

int taintedFlowAnalysisDebugLevel = 2;

// **********************************************************************
//                      TaintedFlowLattice
// **********************************************************************

TaintedFlowLattice::TaintedFlowLattice()
   {
     this->level = bottom;
   }

TaintedFlowLattice::TaintedFlowLattice( short level )
   {
     this->level = level;
   }

// This is the same as the implicit definition, so it might not be required to be defined explicitly.
// I am searching for the minimal example of the use of the data flow classes.
TaintedFlowLattice::TaintedFlowLattice (const TaintedFlowLattice & X)
   {
     this->level = X.level;
   }

short
TaintedFlowLattice::getLevel() const
   {
     return level;
   }
	
bool
TaintedFlowLattice::setLevel(short x)
   {
  // These are more than access functions, they return if the state of the lattice has changed.
  // level = x;
     bool modified = this->level != x;
     level = x;
     return modified;
   }

bool
TaintedFlowLattice::setBottom()
   {
  // These are more than access functions, they return if the state of the lattice has changed.
     bool modified = this->level != bottom;
     level = bottom;
     return modified;
   }

bool
TaintedFlowLattice::setTop()
   {
  // These are more than access functions, they return if the state of the lattice has changed.
     bool modified = this->level != bottom;
     level = top;
     return modified;
   }

// **********************************************
// Required definition of pure virtual functions.
// **********************************************
void
TaintedFlowLattice::initialize()
   {
  // Use the default constructor (implemented above).
  // So nothing to do here.
   }


// returns a copy of this lattice
Lattice*
TaintedFlowLattice::copy() const
   {
     return new TaintedFlowLattice(*this);
   }


// overwrites the state of "this" Lattice with "that" Lattice
void
TaintedFlowLattice::copy(Lattice* X)
   {
     TaintedFlowLattice* that = dynamic_cast<TaintedFlowLattice*>(X);

     this->level = that->level;
   }


bool
TaintedFlowLattice::operator==(Lattice* X) /*const*/
   {
  // Implementation of equality operator.
     TaintedFlowLattice* that = dynamic_cast<TaintedFlowLattice*>(X);
     return (level == that->level);
   }


string
TaintedFlowLattice::str(string indent)
   {
     ostringstream outs;
     if(level == bottom)
          outs << indent << "[level: bottom]";
       else if(level == taintedValue)
          outs << indent << "[level: taintedValue]";
       else if(level == untaintedValue)
          outs << indent << "[level: untaintedValue]";
       else if(level == top)
          outs << indent << "[level: top]";

     return outs.str();
   }

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool
TaintedFlowLattice::meetUpdate(Lattice* X)
   {
     TaintedFlowLattice* that = dynamic_cast<TaintedFlowLattice*>(X);

  // Need to handle bottom, copy from the other side.

  // This is the more technically interesting required function.
     if (this->level == untaintedValue && that->level == untaintedValue)
        {
       // leave it and return false
          return false;
        }
       else
        {
          if (this->level == untaintedValue && that->level == taintedValue)
             {
               this->level = taintedValue;
               return true;
             }
            else
             {
               if (this->level == taintedValue && that->level == untaintedValue)
                  {
                    return false;
                  }
                 else
                  {
                    if (this->level == taintedValue && that->level == taintedValue)
                       {
                         return false;
                       }
                      else
                       {
                         if (this->level == bottom)
                            {
                              this->level = that->level;

                              return (that->level != bottom);
                            }
                           else
                            {
                              if (this->level == top)
                                 {
                                   return false;
                                 }
                                else
                                 {
                                   if (that->level == top)
                                      {
                                        bool modified = this->level != top;
                                        this->level = top;
                                        return modified;
                                      }
                                 }
                            }
                       }
                  }
             }
        }

  // Make up a return value for now.
     return false;
   }



// **********************************************************************
//                  TaintedFlowAnalysisTransfer
// **********************************************************************

template <typename T>
void TaintedFlowAnalysisTransfer::transferArith(SgBinaryOp *sgn, T transferOp)
   {
     TaintedFlowLattice *arg1Lat, *arg2Lat, *resLat;
     if (getLattices(sgn, arg1Lat, arg2Lat, resLat))
        {
          transferOp(this, arg1Lat, arg2Lat, resLat);
          if (isSgCompoundAssignOp(sgn))
               arg1Lat->copy(resLat);
        }
   }

void
TaintedFlowAnalysisTransfer::transferArith(SgBinaryOp *sgn, TransferOp transferOp)
   {
     transferArith(sgn, boost::mem_fn(transferOp));
   }

void 
TaintedFlowAnalysisTransfer::transferIncrement(SgUnaryOp *sgn)
   {
     TaintedFlowLattice *arg1Lat, *arg2Lat = NULL, *resLat;
     if (getLattices(sgn, arg1Lat, arg2Lat, resLat))
         transferTaint(arg1Lat, arg2Lat, resLat);
     delete arg2Lat; // Allocated by getLattices
   }

bool 
TaintedFlowAnalysisTransfer::getLattices(const SgUnaryOp *sgn,  TaintedFlowLattice* &arg1Lat, TaintedFlowLattice* &arg2Lat, TaintedFlowLattice* &resLat)
  {
    arg1Lat = getLattice(sgn->get_operand());
    resLat  = getLattice(sgn);

    // Unary Update
    if(isSgMinusMinusOp(sgn) || isSgPlusPlusOp(sgn)) {
      arg2Lat = new TaintedFlowLattice(TaintedFlowLattice::untaintedValue);
    }
    //Dbg::dbg << "res="<<res.str()<<" arg1="<<arg1.str()<<" arg1Lat="<<arg1Lat<<", arg2Lat="<<arg2Lat<<"\n";
    //Dbg::dbg << "transfer B, resLat="<<resLat<<"\n";

    return (arg1Lat && arg2Lat && resLat);
  }

void
TaintedFlowAnalysisTransfer::transferTaint(TaintedFlowLattice *arg1Lat, TaintedFlowLattice *arg2Lat, TaintedFlowLattice *resLat )
   {
     if (arg1Lat->getLevel() == TaintedFlowLattice::bottom || arg2Lat->getLevel() == TaintedFlowLattice::bottom)
        {
          updateModified(resLat->setLevel(TaintedFlowLattice::bottom));
        }
       else 
        {
       // Both knownValue
          if(arg1Lat->getLevel() == TaintedFlowLattice::taintedValue && arg2Lat->getLevel() == TaintedFlowLattice::taintedValue) 
             {
               updateModified(resLat->setLevel(TaintedFlowLattice::taintedValue));
             }
            else
             {
               if(arg1Lat->getLevel() == TaintedFlowLattice::untaintedValue && arg2Lat->getLevel() == TaintedFlowLattice::untaintedValue) 
                  {
                    updateModified(resLat->setLevel(TaintedFlowLattice::untaintedValue));
                  }
                 else
                  {
                 // Else => Top (mixture of tainted and untained along different paths)
                    updateModified(resLat->setLevel(TaintedFlowLattice::top));
                  }
             }
        }
   }


void
TaintedFlowAnalysisTransfer::visit(SgLongLongIntVal *sgn)
   {
   }

void
TaintedFlowAnalysisTransfer::visit(SgLongIntVal *sgn)
   {
   }

void
TaintedFlowAnalysisTransfer::visit(SgIntVal *sgn)
   {
     TaintedFlowLattice* resLat = getLattice(sgn);
     resLat->setLevel(TaintedFlowLattice::untaintedValue);
   }

void
TaintedFlowAnalysisTransfer::visit(SgFunctionCallExp *sgn)
   {
     TaintedFlowLattice* resLat = getLattice(sgn);
     resLat->setLevel(TaintedFlowLattice::taintedValue);
   }

void
TaintedFlowAnalysisTransfer::visit(SgShortVal *sgn)
   {
   }

void
TaintedFlowAnalysisTransfer::visit(SgUnsignedLongLongIntVal *sgn)
   {
   }

void
TaintedFlowAnalysisTransfer::visit(SgUnsignedLongVal *sgn)
   {
   }

void
TaintedFlowAnalysisTransfer::visit(SgUnsignedIntVal *sgn)
   {
   }

void
TaintedFlowAnalysisTransfer::visit(SgUnsignedShortVal *sgn)
   {
   }

void
TaintedFlowAnalysisTransfer::visit(SgValueExp *sgn)
   {
   }

void
TaintedFlowAnalysisTransfer::visit(SgPlusAssignOp *sgn)
   {
     transferArith(sgn, boost::bind(&TaintedFlowAnalysisTransfer::transferTaint, _1, _2, _3, _4 ));
   }

void
TaintedFlowAnalysisTransfer::visit(SgMinusAssignOp *sgn)
   {
     transferArith(sgn, boost::bind(&TaintedFlowAnalysisTransfer::transferTaint, _1, _2, _3, _4 ));
   }

void
TaintedFlowAnalysisTransfer::visit(SgMultAssignOp *sgn)
   {
     transferArith(sgn, boost::bind(&TaintedFlowAnalysisTransfer::transferTaint, _1, _2, _3, _4 ));
   }

void
TaintedFlowAnalysisTransfer::visit(SgDivAssignOp *sgn)
   {
     transferArith(sgn, boost::bind(&TaintedFlowAnalysisTransfer::transferTaint, _1, _2, _3, _4 ));
   }

void
TaintedFlowAnalysisTransfer::visit(SgModAssignOp *sgn)
   {
     transferArith(sgn, boost::bind(&TaintedFlowAnalysisTransfer::transferTaint, _1, _2, _3, _4 ));
   }

void
TaintedFlowAnalysisTransfer::visit(SgAddOp *sgn)
   {
     transferArith(sgn, boost::bind(&TaintedFlowAnalysisTransfer::transferTaint, _1, _2, _3, _4 ));
   }

void
TaintedFlowAnalysisTransfer::visit(SgSubtractOp *sgn)
   {
     transferArith(sgn, boost::bind(&TaintedFlowAnalysisTransfer::transferTaint, _1, _2, _3, _4 ));
   }

void
TaintedFlowAnalysisTransfer::visit(SgMultiplyOp *sgn)
   {
     transferArith(sgn, boost::bind(&TaintedFlowAnalysisTransfer::transferTaint, _1, _2, _3, _4 ));
   }

void
TaintedFlowAnalysisTransfer::visit(SgDivideOp *sgn)
   {
     transferArith(sgn, boost::bind(&TaintedFlowAnalysisTransfer::transferTaint, _1, _2, _3, _4 ));
   }

void
TaintedFlowAnalysisTransfer::visit(SgModOp *sgn)
   {
     transferArith(sgn, boost::bind(&TaintedFlowAnalysisTransfer::transferTaint, _1, _2, _3, _4 ));
   }

void
TaintedFlowAnalysisTransfer::visit(SgPlusPlusOp *sgn)
   {
     transferIncrement(sgn);
   }

void
TaintedFlowAnalysisTransfer::visit(SgMinusMinusOp *sgn)
   {
     transferIncrement(sgn);
   }

void
TaintedFlowAnalysisTransfer::visit(SgUnaryAddOp *sgn)
   {
     TaintedFlowLattice* resLat = getLattice(sgn);
     resLat->copy(getLattice(sgn->get_operand()));
   }

void
TaintedFlowAnalysisTransfer::visit(SgMinusOp *sgn)
   {
     TaintedFlowLattice* resLat = getLattice(sgn);

  // This sets the level
     resLat->copy(getLattice(sgn->get_operand()));
   }

bool
TaintedFlowAnalysisTransfer::finish()
   {
     return modified;
   }

TaintedFlowAnalysisTransfer::TaintedFlowAnalysisTransfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo)
   : VariableStateTransfer<TaintedFlowLattice>(func, n, state, dfInfo, taintedFlowAnalysisDebugLevel)
   {
   }




// **********************************************************************
//                     TaintedFlowAnalysis
// **********************************************************************

TaintedFlowAnalysis::TaintedFlowAnalysis(LiveDeadVarsAnalysis* ldva)
   {
     this->ldva = ldva;
   }

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
void
TaintedFlowAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state, std::vector<Lattice*>& initLattices, std::vector<NodeFact*>& initFacts)
   {
  // ???
  // vector<Lattice*> initLattices;
	map<varID, Lattice*> emptyM;
	FiniteVarsExprsProductLattice* l = new FiniteVarsExprsProductLattice((Lattice*)new TaintedFlowLattice(), emptyM/*genConstVarLattices()*/, 
	                                                                     (Lattice*)NULL, ldva, /*func, */n, state);         
	//Dbg::dbg << "DivAnalysis::genInitState, returning l="<<l<<" n=<"<<Dbg::escape(n.getNode()->unparseToString())<<" | "<<n.getNode()->class_name()<<" | "<<n.getIndex()<<">\n";
	//Dbg::dbg << "    l="<<l->str("    ")<<"\n";
     initLattices.push_back(l);
   }

	
bool
TaintedFlowAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo)
   {
     assert(0); 
     return false;
   }

boost::shared_ptr<IntraDFTransferVisitor>
TaintedFlowAnalysis::getTransferVisitor(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo)
   {
  // Why is the boost shared pointer used here?
     return boost::shared_ptr<IntraDFTransferVisitor>(new TaintedFlowAnalysisTransfer(func, n, state, dfInfo));
   }


