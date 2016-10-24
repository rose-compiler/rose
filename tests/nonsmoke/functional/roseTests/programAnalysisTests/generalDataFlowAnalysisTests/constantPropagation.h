#ifndef CONSTANT_PROPAGATION_ANALYSIS_H
#define CONSTANT_PROPAGATION_ANALYSIS_H
// Author: Dan Quinlan, with Phil Miller
// Date: 9/8/2011
/*
TODO: the constant propagation analysis is limited to live variables at a point. 
     This is not correct behavior.
 Liao, 7/1/2012
*/

#include "VariableStateTransfer.h"

extern int constantPropagationAnalysisDebugLevel;

class ConstantPropagationLattice : public FiniteLattice
   {
     private:
       // the current value of the variable (if known)
       //TODO: support other types of constants, like floating point numbers
          int value;

          //bool undefined;

     private:
       // this object's current level in the lattice
          short level;

     public:
       // The different levels of this lattice

       // no information is known about the value of the variable. Initial state. 
          static const short bottom = 1; 

          static const short constantValue = 2; 
        // final state.
          static const short top = 3;

     public:
       // Do we need a default constructor?
          ConstantPropagationLattice();

       // This constructor builds a constant value lattice.
          ConstantPropagationLattice( int v );

          ConstantPropagationLattice( short level, int v );

       // Do we need th copy constructor?
          ConstantPropagationLattice(const ConstantPropagationLattice & X);

       // Access functions.
          int getValue() const;
          short getLevel() const;

          bool setValue(int x);
          bool setLevel(short x);

          bool setBottom();
          bool setTop();

       // **********************************************
       // Required definition of pure virtual functions.
       // **********************************************
          void initialize();

       // returns a copy of this lattice
          Lattice* copy() const;

       // overwrites the state of "this" Lattice with "that" Lattice
          void copy(Lattice* that);

          bool operator==(Lattice* that) /*const*/;

       // computes the meet of this and that and saves the result in this
       // returns true if this causes this to change and false otherwise
          bool meetUpdate(Lattice* that);

          std::string str(std::string indent="");
   };
   

class ConstantPropagationAnalysisTransfer : public VariableStateTransfer<ConstantPropagationLattice>
   {
     private:
          typedef void (ConstantPropagationAnalysisTransfer::*TransferOp)(ConstantPropagationLattice *, ConstantPropagationLattice *, ConstantPropagationLattice *);
          template <typename T> void transferArith(SgBinaryOp *sgn, T transferOp);
          template <class T> void visitIntegerValue(T *sgn);

          void transferArith(SgBinaryOp *sgn, TransferOp transferOp);

          void transferIncrement(SgUnaryOp *sgn);
          void transferCompoundAdd(SgBinaryOp *sgn);
          void transferAdditive(ConstantPropagationLattice *arg1Lat, ConstantPropagationLattice *arg2Lat, ConstantPropagationLattice *resLat, bool isAddition);
          void transferMultiplicative(ConstantPropagationLattice *arg1Lat, ConstantPropagationLattice *arg2Lat, ConstantPropagationLattice *resLat);
          void transferDivision(ConstantPropagationLattice *arg1Lat, ConstantPropagationLattice *arg2Lat, ConstantPropagationLattice *resLat);
          void transferMod(ConstantPropagationLattice *arg1Lat, ConstantPropagationLattice *arg2Lat, ConstantPropagationLattice *resLat);

     public:
      //  void visit(SgNode *);
          void visit(SgLongLongIntVal *sgn);
          void visit(SgLongIntVal *sgn);
          void visit(SgIntVal *sgn);
          void visit(SgShortVal *sgn);
          void visit(SgUnsignedLongLongIntVal *sgn);
          void visit(SgUnsignedLongVal *sgn);
          void visit(SgUnsignedIntVal *sgn);
          void visit(SgUnsignedShortVal *sgn);
          void visit(SgValueExp *sgn);
          void visit(SgPlusAssignOp *sgn);
          void visit(SgMinusAssignOp *sgn);
          void visit(SgMultAssignOp *sgn);
          void visit(SgDivAssignOp *sgn);
          void visit(SgModAssignOp *sgn);
          void visit(SgAddOp *sgn);
          void visit(SgSubtractOp *sgn);
          void visit(SgMultiplyOp *sgn);
          void visit(SgDivideOp *sgn);
          void visit(SgModOp *sgn);
          void visit(SgPlusPlusOp *sgn);
          void visit(SgMinusMinusOp *sgn);
          void visit(SgUnaryAddOp *sgn);
          void visit(SgMinusOp *sgn);

          bool finish();

          ConstantPropagationAnalysisTransfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo);
   };


class ConstantPropagationAnalysis : public IntraFWDataflow
   {
     protected:
          static std::map<varID, Lattice*> constVars;
          static bool constVars_init;
	
       // The LiveDeadVarsAnalysis that identifies the live/dead state of all application variables.
       // Needed to create a FiniteVarsExprsProductLattice.
       //TODO the justification is weak. Can we have a refactored fuction/analysis to just create a FiniteVarsExprsProductLattice??
       // It is not intutive to run liveness analysis before running constant propagation.
          LiveDeadVarsAnalysis* ldva; 
	
     public:
          ConstantPropagationAnalysis(LiveDeadVarsAnalysis* ldva);

       // generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
          void genInitState(const Function& func, const DataflowNode& n, const NodeState& state, std::vector<Lattice*>& initLattices, std::vector<NodeFact*>& initFacts);
	
          bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo);

          boost::shared_ptr<IntraDFTransferVisitor> getTransferVisitor(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo);
   };


#endif
