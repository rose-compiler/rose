// Author: Markus Schordan
// $Id: AstDOTGeneration.h,v 1.4 2008/01/08 02:56:38 dquinlan Exp $

#ifndef ASTDOTGENERATION_H
#define ASTDOTGENERATION_H

#include <set>
#include "DOTGeneration.h"
#include "roseInternal.h"
#include "sage3.h"

class AstDOTGeneration : public DOTGeneration<SgNode*>
   {
     public:
          void generate(SgProject* node, traversalType tt=TOPDOWNBOTTOMUP, std::string filenamePostfix="");
          void generate(SgNode* node,  std::string filename, traversalType tt = TOPDOWNBOTTOMUP,std::string filenamePostfix = "");
          void generateInputFiles(SgProject* node, traversalType tt=TOPDOWNBOTTOMUP, std::string filenamePostfix="");
          void generateWithinFile(SgFile* node, traversalType tt=TOPDOWNBOTTOMUP, std::string filenamePostfix="");

          void writeIncidenceGraphToDOTFile(SgIncidenceDirectedGraph* graph,  const std::string& filename);
          void addAdditionalNodesAndEdges(SgNode* node);

     protected:
          DOTInheritedAttribute evaluateInheritedAttribute(SgNode* node, DOTInheritedAttribute ia);
          DOTSynthesizedAttribute evaluateSynthesizedAttribute(SgNode* node, DOTInheritedAttribute ia, SubTreeSynthesizedAttributes l);
          std::string additionalNodeInfo(SgNode* node);

       // DQ (11/1/2003) added mechanism to add node options (to add color, etc.)
          std::string additionalNodeOptions(SgNode* node);

       // DQ (3/5/2007) added mechanism to add edge options (to add color, etc.)
          std::string additionalEdgeInfo(SgNode* from, SgNode* to, std::string label);

       // DQ (3/5/2007) added mechanism to add edge options (to add color, etc.)
          std::string additionalEdgeOptions(SgNode* from, SgNode* to, std::string label);

       // DQ (7/27/2008): Added support to eliminate IR nodes in DOT graphs 
       // (to tailor the presentation of information about ASTs).
          bool commentOutNodeInGraph(SgNode* node);
   };

#endif
