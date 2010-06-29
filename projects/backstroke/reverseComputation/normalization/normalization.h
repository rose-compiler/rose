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

SgExpression* normalizeExpression(SgExpression* exp);

void splitCommaOpExp(SgExpression* exp);

void removeUselessBraces(SgBasicBlock* body);

#endif
