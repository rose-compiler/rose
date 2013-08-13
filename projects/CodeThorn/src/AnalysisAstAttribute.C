#include "sage3basic.h"
#include "AnalysisAstAttribute.h"

// default behavior
string AnalysisResultAttribute::getPreInfoString() { return "pre-info: none";}
string AnalysisResultAttribute::getPostInfoString() { return "post-info: none";}

AnalysisResultAttribute::~AnalysisResultAttribute() {
  // virtual destructor intentionally empty
}

