#include "PrologTerm.h"
#include "PrologCompTerm.h"
#include "PrologList.h"
#include "PrologAtom.h"
#include "PrologString.h"
#include "PrologInt.h"
#include "PrologVariable.h"

// Defines wether Terms start with "Sg"
#define SG_PREFIX ""
//#define SG_PREFIX "Sg"

// if enabled, use int_val(...) instead of unary_node(intval, ...)
#define COMPACT_TERM_NOTATION
