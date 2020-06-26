
#include "LTLOptions.h"

bool LTLOptions::activeOptionsRequireSPOTLibrary() {
  return cegpra.csvStatsFileName.size()>0
     || cegpra.checkAllProperties
    || cegpra.visualizationDotFile.size()>0
    || spotVerificationResultsCSVFileName.size()>0
    || ltlFormulaeFile.size()>0
    || ltlInAlphabet.size()>0
    || ltlOutAlphabet.size()>0
    || ltlDriven
    /*|| resetAnalyzer*/
    || stdIOOnly
    || withCounterExamples
    || withAssertCounterExamples
    || withLTLCounterExamples
    ;
}

bool LTLOptions::CEGPra::ltlPropertyNrIsSet() {
  return ltlPropertyNr!=-1;
}
