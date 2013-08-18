#include "sage3basic.h"
#include "AnalysisAstAttribute.h"

// default behavior
string AnalysisResultAttribute::getPreInfoString() { return "pre-info: undefined";}
string AnalysisResultAttribute::getPostInfoString() { return "post-info: undefined";}

AnalysisResultAttribute::~AnalysisResultAttribute() {
  // virtual destructor intentionally empty
}

