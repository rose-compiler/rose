
#include "ParProOptions.h"

bool ParProOptions::activeOptionsRequireSPOTLibrary() {
  return mineNumVerifiable>0
    || mineNumFalsifiable>0
    || ltlMode.size()>0
    || ltlPropertiesOutput.size()>0
    || promelaOutput.size()>0
    || promelaOutputOnly
    || outputWithResults
    || outputWithAnnotations
    ;
}
