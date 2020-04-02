#include "sage3basic.h"

#include "DFAstAttributeConversion.h"
#include "RDAstAttribute.h"
#include "UDAstAttribute.h"

using namespace std;
using namespace CodeThorn;

void DFAstAttributeConversion::createUDAstAttributeFromRDAttribute(Labeler* labeler, string rdAttributeName, string udAttributeName) {
  long labelNum=labeler->numberOfLabels();
  for(long i=0;i<labelNum;++i) {
    Label lab=i;
    SgNode* node=labeler->getNode(lab);
    RDAstAttribute* rdAttr=dynamic_cast<RDAstAttribute*>(node->getAttribute(rdAttributeName));
    if(rdAttr)
      node->setAttribute(udAttributeName,new UDAstAttribute(rdAttr, node));
  }
}
