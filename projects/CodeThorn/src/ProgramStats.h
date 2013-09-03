#ifndef PROGRAM_STATS_H
#define PROGRAM_STATS_H

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "DFAnalyzer.h"
#include "RDAstAttribute.h"
#include "Miscellaneous.h"

void computeStatistics(VariableIdMapping* vidm, Labeler* labeler, Flow* icfg, string useDefAstAttributeName);

#endif
