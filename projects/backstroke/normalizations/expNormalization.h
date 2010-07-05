#ifndef REVERSE_COMPUTATION_NORMALIZATION
#define REVERSE_COMPUTATION_NORMALIZATION

#include <rose.h>
#include <boost/foreach.hpp>
#include <utility>

#define foreach BOOST_FOREACH


using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;


bool isAssignmentOp(SgExpression* e);

// The first return value is the expression which should be relocated, and the second 
// return value indicate the position (before(true)/after(false)).
std::pair<SgExpression*, bool> getAndReplaceModifyingExpression(SgExpression* exp);

// Normalize an event function.
void normalizeEvent(SgFunctionDefinition* func_def);

SgExpression* normalizeExpression(SgExpression* exp);

// Split a comma expression into several statements.
void splitCommaOpExp(SgExpression* exp);

// Extend comma operation expressions, e.g. (a, b) + c ==> (a, b + c).
SgExpression* extendCommaOpExp(SgExpression* exp);

// Remove braces of a basic block in which there is no variable declared.
void removeUselessBraces(SgBasicBlock* body);



#endif
