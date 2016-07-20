
#ifndef REFINEMENT_CONSTRAINTS_H
#define REFINEMENT_CONSTRAINTS_H

#include "sage3basic.h"
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "ExprAnalyzer.h"
#include "CFAnalysis.h"

//BOOST includes
#include "boost/unordered_set.hpp"


using namespace std;

namespace CodeThorn {

  typedef pair<VariableId, int> RConstraint;
  typedef boost::unordered_map<VariableId, set<int> > VarToIntMap;

  // Manages variable value constraints for the refinement of an over-approximating analysis.
  //   Constraints are saved as pairs <VariableId, int> with the integer value describing a "less than" condition for that variable.
  //   Please note: The current implementation does not take conditions into account that contain branches with assignments relevant to 
  //                the constraint variables themselves.
  class RefinementConstraints {
  public:
    RefinementConstraints(Flow* flow, SPRAY::IOLabeler* labeler, ExprAnalyzer* exprAnalyzer, CFAnalysis* cfanalyzer, VariableIdMapping* varIdMapping) {
      _cfg = flow; _labeler=labeler; _exprAnalyzer=exprAnalyzer; _cfanalyzer=cfanalyzer; _varIdMapping=varIdMapping;
    };
    // collects the atomic propositions of conditional statements that decide about the path from the start of the program to "label".
    // It is assumed that each atomic proposition found desribes a comparision between a variable and a constant. For each of these 
    // propositions, add one constraint to the map of maintained constraints. 
    void addConstraintsByLabel(Label label);
    // returns all "less than"-constraint values for a certain VariableId.
    set<int> getConstraintsForVariable(VariableId varId);
   /// ideas for future public functions
    // returns true iff one or more of the value constraints for "varId" are in the range of "interval". 
   /// bool isViolatingConstraints(NumberIntervalLattice interval, VariableId varId);
    // returns true iff the result of merging "a" and "b" would violate at least one maintained variable value constraint.
   /// bool constraintViolationIfMerged(IntervalPropertyState a, ItervalPropertyState b);
    
  private:
    // collect the atomic propositions of conditional statements leading from the start of the program to "label".
    set<RConstraint> getAtomicPropositionsFromConditions(Label label);
    // converts a Sage binaryOp relation into "less-than-value"-constraints (pairs of VarId + value)
    set<RConstraint> conditionToConstraints(SgBinaryOp* binOp);
    // return two "less than value"-constraints for a certain variable, forming left and right borders describing 
    // the specified value that is checked in the atomic proposition (according to the proposition's lhs and rhs). 
    pair<RConstraint, RConstraint> createEqualityConstraints(SgNode* lhs, SgNode* rhs);
    // checks if an SgNode* leads to an SgIntVal either directly or as the child of an unary minus operator.
    bool isIntValue(SgNode* node);
    // retrieves the int value from an SgNode* according to "isIntVal(...)"'s specification (see above).
    int getIntValue(SgNode* node);
    // create a constraint (a pair of VariableId and int value. The refined analysis should always keep track of whether or not
    // the variable with "varId" is less than (val+offsetFromVal).)
    RConstraint createConstraint(VariableId varId, int val, int offsetFromVal);

    VarToIntMap _constraintsLessThan;

    Flow* _cfg;
    CFAnalysis* _cfanalyzer;
    SPRAY::IOLabeler* _labeler;
    ExprAnalyzer* _exprAnalyzer;
    VariableIdMapping* _varIdMapping; 
  };

} // end of namespace CodeThorn

#endif
