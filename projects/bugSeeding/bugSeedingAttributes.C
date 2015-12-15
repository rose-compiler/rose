// This is where we put the supporting code for bug seeding attributes.

#include "rose.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

#include "bugSeeding.h"

// *************************************************
//         SecurityVulnerabilityAttribute
// *************************************************

void
SecurityVulnerabilityAttribute::set_associatedClones(SgNode* node)
   {
     ROSE_ASSERT(node != NULL);
     printf ("Inside of SecurityVulnerabilityAttribute::set_associatedClones node = %p \n",node);
     if (associtedClones.find(node) == associtedClones.end())
        {
          printf ("Adding node = %p to associtedClones set \n",node);
          associtedClones.insert(node);
        }
   }

SecurityVulnerabilityAttribute::SecurityVulnerabilityAttribute (SgNode* securityVulnerabilityNode, SecurityFlaw::Vulnerability* vulnerabilityPointer)
   : securityVulnerabilityNode(securityVulnerabilityNode), vulnerabilityPointer(vulnerabilityPointer)
   {
  // Initially set this to the same value as securityVulnerabilityNode
  // when any copy is made we will reset the securityVulnerabilityNode
  // value and leave the securityVulnerabilityNodeInOriginalCode alone.
     securityVulnerabilityNodeInOriginalCode = securityVulnerabilityNode;
   }

SgNode*
SecurityVulnerabilityAttribute::get_securityVulnerabilityNode()
   {
     return securityVulnerabilityNode;
   }

void
SecurityVulnerabilityAttribute::set_securityVulnerabilityNode(SgNode* node)
   {
     securityVulnerabilityNode = node;
   }

SgNode*
SecurityVulnerabilityAttribute::get_securityVulnerabilityNodeInOriginalCode()
   {
     return securityVulnerabilityNodeInOriginalCode;
   }

void
SecurityVulnerabilityAttribute::set_securityVulnerabilityNodeInOriginalCode(SgNode* node)
   {
     securityVulnerabilityNodeInOriginalCode = node;
   }

// DOT graph support for attributes to add additional edges to AST dot graphs
// (useful for debugging)
vector<AstAttribute::AttributeEdgeInfo>
SecurityVulnerabilityAttribute::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

     set<SgNode*>::iterator i = associtedClones.begin();
     while ( i != associtedClones.end() )
        {
          ROSE_ASSERT(securityVulnerabilityNode != NULL);
          printf ("Adding an edge from %p = %s to %p = %s \n",securityVulnerabilityNode,securityVulnerabilityNode->class_name().c_str(),*i,(*i)->class_name().c_str());
          AstAttribute::AttributeEdgeInfo edge (securityVulnerabilityNode,*i,"associated vulnerabilities"," constraint=false ");

          v.push_back(edge);

          i++;
        }

  // string vulnerabilityName = "SecurityVulnerabilityAttribute" + vulnerabilityPointer->get_name();
     string vulnerabilityName = vulnerabilityPointer->get_name();
     string vulnerabilityColor = vulnerabilityPointer->get_color();

  // Note that we need the trailing " "
     string vulnerabilityOptions = " arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=" + vulnerabilityColor + " ";
  // string options = " constraint=false color=deepskyblue4";
  // string options = " constraint=false color=deepskyblue4 ";
  // printf ("vulnerabilityOptions = %s \n",vulnerabilityOptions.c_str());

  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,"SecurityVulnerabilityAttribute"," minlen=1 splines=\"false\" arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=deepskyblue4 ");
  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,"SecurityVulnerabilityAttribute"," maxlen=\"1\" arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=deepskyblue4 ");
  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,"SecurityVulnerabilityAttribute"," maxlen=\"1\" arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=" + color );
  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,"SecurityVulnerabilityAttribute",options);
  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,"SecurityVulnerabilityAttribute","");
  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,"SecurityVulnerabilityAttribute",options);
     AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,securityVulnerabilityNode,vulnerabilityName,vulnerabilityOptions);
     v.push_back(additional_edge);

#if 0
     printf ("Exiting at base of SecurityVulnerabilityAttribute::additionalEdgeInfo() \n");
     ROSE_ASSERT(false);
#endif
     return v;
   }

std::vector<AstAttribute::AttributeNodeInfo>
SecurityVulnerabilityAttribute::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;

     string vulnerabilityName    = " " + vulnerabilityPointer->get_name();
     string vulnerabilityColor   = vulnerabilityPointer->get_color();
     string vulnerabilityOptions = " arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false fillcolor=" + vulnerabilityColor + ",style=filled ";

  // AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) vulnerabilityPointer, "SecurityVulnerabilityAttribute"," fillcolor=\"red\",style=filled ");
     AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) vulnerabilityPointer, vulnerabilityName, vulnerabilityOptions);
     v.push_back(vulnerabilityNode);

     return v;
   }


AstAttribute*
SecurityVulnerabilityAttribute::copy() const
   {
     return new SecurityVulnerabilityAttribute(*this);
   }












// *************************************************
//      SecurityFlawOriginalSubtreeAttribute
// *************************************************

// DOT graph support for attributes to add additional edges to AST dot graphs
// (useful for debugging)
vector<AstAttribute::AttributeEdgeInfo>
SecurityFlawOriginalSubtreeAttribute::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

     return v;
   }

std::vector<AstAttribute::AttributeNodeInfo>
SecurityFlawOriginalSubtreeAttribute::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;
#if 0
     AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) this, "SecurityFlawOriginalSubtreeAttribute"," fillcolor=\"darkorange\",style=filled ");
     v.push_back(vulnerabilityNode);
#endif
     return v;
   }

AstAttribute*
SecurityFlawOriginalSubtreeAttribute::copy() const
   {
     return new SecurityFlawOriginalSubtreeAttribute(*this);
   }







// *************************************************
//       SeededSecurityFlawCloneAttribute
// *************************************************

SeededSecurityFlawCloneAttribute::SeededSecurityFlawCloneAttribute (SgNode* primarySecurityFlawInClone, SgNode* rootOfCloneInOriginalCode)
   : primarySecurityFlawInClone(primarySecurityFlawInClone), 
     rootOfCloneInOriginalCode(rootOfCloneInOriginalCode)
   {
   }

SgNode*
SeededSecurityFlawCloneAttribute::get_primarySecurityFlawInClone()
   {
     return primarySecurityFlawInClone;
   }

void
SeededSecurityFlawCloneAttribute::set_primarySecurityFlawInClone(SgNode* node)
   {
     primarySecurityFlawInClone = node;
   }


SgNode*
SeededSecurityFlawCloneAttribute::get_rootOfCloneInOriginalCode()
   {
     return rootOfCloneInOriginalCode;
   }

void
SeededSecurityFlawCloneAttribute::set_rootOfCloneInOriginalCode(SgNode* node)
   {
     rootOfCloneInOriginalCode = node;
   }

// DOT graph support for attributes to add additional edges to AST dot graphs
// (useful for debugging)
vector<AstAttribute::AttributeEdgeInfo>
SeededSecurityFlawCloneAttribute::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(rootOfCloneInOriginalCode != NULL);

     AstAttribute::AttributeEdgeInfo edgeToRootOfCloneInOriginalCode (primarySecurityFlawInClone,rootOfCloneInOriginalCode,"root of clone in original code"," wieght=10.0 arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=orange ");

     v.push_back(edgeToRootOfCloneInOriginalCode);

     return v;
   }

std::vector<AstAttribute::AttributeNodeInfo>
SeededSecurityFlawCloneAttribute::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;

     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(rootOfCloneInOriginalCode != NULL);
#if 0
     AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) this, "SeededSecurityFlawCloneAttribute"," fillcolor=\"springgreen\",style=filled ");
     v.push_back(vulnerabilityNode);
#endif
     return v;
   }

AstAttribute*
SeededSecurityFlawCloneAttribute::copy() const
   {
     return new SeededSecurityFlawCloneAttribute(*this);
   }
















// *************************************************
//   PrimarySecurityVulnerabilityForCloneAttribute
// *************************************************

PrimarySecurityVulnerabilityForCloneAttribute::PrimarySecurityVulnerabilityForCloneAttribute(SgNode* primarySecurityFlawInClone, SgNode* rootOfClone, SecurityFlaw::Vulnerability* vulnerabilityPointer)
   : primarySecurityFlawInClone(primarySecurityFlawInClone), rootOfClone(rootOfClone), vulnerabilityPointer(vulnerabilityPointer)
   {
     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(rootOfClone != NULL);
     ROSE_ASSERT(vulnerabilityPointer != NULL);
   }

SgNode*
PrimarySecurityVulnerabilityForCloneAttribute::get_primarySecurityFlawInClone()
   {
     return primarySecurityFlawInClone;
   }

void
PrimarySecurityVulnerabilityForCloneAttribute::set_primarySecurityFlawInClone(SgNode* node)
   {
     primarySecurityFlawInClone = node;
   }


void
PrimarySecurityVulnerabilityForCloneAttribute::set_primaryVulnerabilityInOriginalCode(SgNode* node)
   {
     primaryVulnerabilityInOriginalCode = node;
   }

SgNode*
PrimarySecurityVulnerabilityForCloneAttribute::get_primaryVulnerabilityInOriginalCode()
   {
     return primaryVulnerabilityInOriginalCode;
   }

// DOT graph support for attributes to add additional edges to AST dot graphs
// (useful for debugging)
vector<AstAttribute::AttributeEdgeInfo>
PrimarySecurityVulnerabilityForCloneAttribute::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(primaryVulnerabilityInOriginalCode != NULL);

     AstAttribute::AttributeEdgeInfo edgeToOriginalCode (primarySecurityFlawInClone,primaryVulnerabilityInOriginalCode,"vulnerabilities in original code"," arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=red ");
  // AstAttribute::AttributeEdgeInfo edgeToOriginalCode (primarySecurityFlawInClone,primaryVulnerabilityInOriginalCode,"vulnerabilities in original code"," arrowsize=7.0 style=\"setlinewidth(7)\" color=red ");
  // AstAttribute::AttributeEdgeInfo edgeToRootOfClone (primarySecurityFlawInClone,rootOfClone,"root of clone"," wieght=10.0 arrowsize=7.0 style=\"setlinewidth(7)\" color=springgreen ");
     AstAttribute::AttributeEdgeInfo edgeToRootOfClone (primarySecurityFlawInClone,rootOfClone,"root of clone"," wieght=10.0 arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=firebrick ");

     v.push_back(edgeToOriginalCode);
     v.push_back(edgeToRootOfClone);

     ROSE_ASSERT(vulnerabilityPointer != NULL);
     string vulnerabilityName  = vulnerabilityPointer->get_name();
     string vulnerabilityColor = vulnerabilityPointer->get_color();

  // Note that we need the trailing " ", plus forcing constraint=true places the node nearest the original node
  // string vulnerabilityOptions = " arrowsize=7.0 style=\"setlinewidth(7)\" constraint=true color=" + vulnerabilityColor + " ";
     string vulnerabilityOptions = " arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=" + vulnerabilityColor + " ";
#if 1
     AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,primarySecurityFlawInClone,vulnerabilityName,vulnerabilityOptions);
  // AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) vulnerabilityPointer,primaryVulnerabilityInOriginalCode,vulnerabilityName,vulnerabilityOptions);
     v.push_back(additional_edge);
#endif
     return v;
   }


std::vector<AstAttribute::AttributeNodeInfo>
PrimarySecurityVulnerabilityForCloneAttribute::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;

     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(primaryVulnerabilityInOriginalCode != NULL);

#if 0
     AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) this, "PrimarySecurityVulnerabilityForCloneAttribute"," fillcolor=\"purple\",style=filled ");
     v.push_back(vulnerabilityNode);
#endif

     return v;
   }

AstAttribute*
PrimarySecurityVulnerabilityForCloneAttribute::copy() const
   {
     return new PrimarySecurityVulnerabilityForCloneAttribute(*this);
   }









// **********************************
//    SeedMethodologyCloneAttribute
// **********************************

// DOT graph support for attributes to add additional edges to AST dot graphs
// (useful for debugging)
vector<AstAttribute::AttributeEdgeInfo>
SeedMethodologyCloneAttribute::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

     ROSE_ASSERT(rootOfSeedMethodologyClone != NULL);
     ROSE_ASSERT(originalTreeInVulnerabilityClone != NULL);

     AstAttribute::AttributeEdgeInfo edgeToOriginalCode (rootOfSeedMethodologyClone,originalTreeInVulnerabilityClone,"original tree for seeding methodology clone"," arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=springgreen ");

     v.push_back(edgeToOriginalCode);

     string seedingMethodName = seedSecurityFlawPointer->get_name();
  // AstAttribute::AttributeEdgeInfo edgeToSeedingMethod ((SgNode*) seedSecurityFlawPointer,rootOfSeedMethodologyClone,seedingMethodName," arrowsize=7.0 style=\"setlinewidth(7)\" constraint=true color=springgreen ");
     AstAttribute::AttributeEdgeInfo edgeToSeedingMethod ((SgNode*) seedSecurityFlawPointer,rootOfSeedMethodologyClone,seedingMethodName," arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=springgreen ");

     v.push_back(edgeToSeedingMethod);

     return v;
   }

std::vector<AstAttribute::AttributeNodeInfo>
SeedMethodologyCloneAttribute::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;

     string seedingMethodName = seedSecurityFlawPointer->get_name();

#if 1
     AstAttribute::AttributeNodeInfo seedingMethodNode ( (SgNode*) seedSecurityFlawPointer, seedingMethodName," fillcolor=\"pink\",style=filled ");
     v.push_back(seedingMethodNode);
#endif
     return v;
   }

AstAttribute*
SeedMethodologyCloneAttribute::copy() const
   {
     return new SeedMethodologyCloneAttribute(*this);
   }




// **********************************
//           PruningAttribute
// **********************************

PruningAttribute::~PruningAttribute()
   {
   }

bool
PruningAttribute::commentOutNodeInGraph()
   {
     return true;
   }

AstAttribute*
PruningAttribute::copy() const
   {
     return new PruningAttribute(*this);
   }




/* Colors: (from http://www.graphviz.org/doc/info/colors.html)
aquamarine aliceblue 	antiquewhite 	antiquewhite1 	antiquewhite2 	antiquewhite3
antiquewhite4 	aquamarine 	aquamarine1 	aquamarine2 	aquamarine3
aquamarine4 	azure 	azure1 	azure2 	azure3
azure4 	beige 	bisque 	bisque1 	bisque2
bisque3 	bisque4 	black 	blanchedalmond 	   blue   
blue1 	blue2 	blue3 	blue4 	blueviolet
brown 	brown1 	brown2 	brown3 	brown4
burlywood 	burlywood1 	burlywood2 	burlywood3 	burlywood4
cadetblue 	cadetblue1 	cadetblue2 	cadetblue3 	cadetblue4
chartreuse 	chartreuse1 	chartreuse2 	chartreuse3 	chartreuse4
chocolate 	chocolate1 	chocolate2 	chocolate3 	chocolate4
coral 	coral1 	coral2 	coral3 	coral4
cornflowerblue 	cornsilk 	cornsilk1 	cornsilk2 	cornsilk3
cornsilk4 	crimson 	   cyan    	cyan1 	cyan2
cyan3 	cyan4 	darkgoldenrod 	darkgoldenrod1 	darkgoldenrod2
darkgoldenrod3 	darkgoldenrod4 	darkgreen 	darkkhaki 	darkolivegreen
darkolivegreen1 	darkolivegreen2 	darkolivegreen3 	darkolivegreen4 	darkorange
darkorange1 	darkorange2 	darkorange3 	darkorange4 	darkorchid
darkorchid1 	darkorchid2 	darkorchid3 	darkorchid4 	darksalmon
darkseagreen 	darkseagreen1 	darkseagreen2 	darkseagreen3 	darkseagreen4
darkslateblue 	darkslategray 	darkslategray1 	darkslategray2 	darkslategray3
darkslategray4 	darkslategrey 	darkturquoise 	darkviolet 	deeppink
deeppink1 	deeppink2 	deeppink3 	deeppink4 	deepskyblue
deepskyblue1 	deepskyblue2 	deepskyblue3 	deepskyblue4 	dimgray
dimgrey 	dodgerblue 	dodgerblue1 	dodgerblue2 	dodgerblue3
dodgerblue4 	firebrick 	firebrick1 	firebrick2 	firebrick3
firebrick4 	floralwhite 	forestgreen 	gainsboro 	ghostwhite
gold    	gold1 	gold2 	gold3 	gold4
goldenrod 	goldenrod1 	goldenrod2 	goldenrod3 	goldenrod4
gray
*/

// DOT graph support for attributes to color AST IR nodes in AST dot graphs (useful for debugging)
string
SecurityVulnerabilityAttribute::additionalNodeOptions()
   {
     return "fillcolor=\"red\",style=filled";
   }

// DOT graph support for attributes to color AST IR nodes in AST dot graphs (useful for debugging)
string
SecurityFlawOriginalSubtreeAttribute::additionalNodeOptions()
   {
  // return "fillcolor=\"deepskyblue\",style=filled";
     return "fillcolor=\"orange\",style=filled";
   }

// DOT graph support for attributes to color AST IR nodes in AST dot graphs (useful for debugging)
string
SeededSecurityFlawCloneAttribute::additionalNodeOptions()
   {
  // return "fillcolor=\"yellow\",style=filled";
  // return "fillcolor=\"springgreen\",style=filled";
  // return "fillcolor=\"firebrick\",style=filled";
     return "fillcolor=\"cyan\",style=filled";
   }

// DOT graph support for attributes to color AST IR nodes in AST dot graphs (useful for debugging)
string
PrimarySecurityVulnerabilityForCloneAttribute::additionalNodeOptions()
   {
  // return "fillcolor=\"greenyellow\",style=filled";
  // return "fillcolor=\"magenta\",style=filled";
  // return "fillcolor=\"aquamarine\",style=filled";
  // return "fillcolor=\"cyan\",style=filled";
     return "fillcolor=\"firebrick\",style=filled";
   }

// DOT graph support for attributes to color AST IR nodes in AST dot graphs (useful for debugging)
string
SeedMethodologyCloneAttribute::additionalNodeOptions()
   {
  // return "fillcolor=\"deepskyblue\",style=filled";
     return "fillcolor=\"springgreen\",style=filled";
   }

