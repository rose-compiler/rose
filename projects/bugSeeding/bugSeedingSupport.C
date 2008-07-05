// This is where we put the suppprting code for bug seeding.

#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

#include "bugSeeding.h"

SecurityFlaw::SecurityFlaw()
   {
     seedOriginalCode = false;
   }

SecurityFlaw::~SecurityFlaw()
{
}

void
SecurityFlaw::detectVunerabilities( SgProject *project )
   {
  // This is a pure virtual function in the bae class, so it should not be called.
     printf ("Error: Base class function called: SecurityFlaw::detectVunerabilities() \n");
     ROSE_ASSERT(false);
   }

void
SecurityFlaw::defineSearchSpace()
   {
     printf ("Base class function called: SecurityFlaw::defineSearchSpace() \n");
   }

void
SecurityFlaw::seedWithGrainularity( SgProject *project )
   {
     printf ("Base class function called: SecurityFlaw::seedSecurityFlaws() \n");
   }

// Declaration of static data member (collection of all security flaws).
std::vector<SecurityFlaw*> SecurityFlaw::securityFlawCollection;

// This is a static member function
void
SecurityFlaw::buildAllVunerabilities()
   {
  // When there is support for many different kinds of security flaws the list will be assembled here!

  // Build a BufferOverFlowSecurityFlaw object
     BufferOverFlowSecurityFlaw* bufferOverFlowSecurityFlaw = new BufferOverFlowSecurityFlaw();

     securityFlawCollection.push_back(bufferOverFlowSecurityFlaw);
   }

// This is a static member function
void
SecurityFlaw::detectAllVunerabilities( SgProject *project )
   {
  // Call the member function to annotate the AST where each security flaw vulnerabilities exists.
     std::vector<SecurityFlaw*>::iterator i = securityFlawCollection.begin();
     while (i != securityFlawCollection.end())
        {
          (*i)->detectVunerabilities(project);
          i++;
        }
   }

// This is a static member function
void
SecurityFlaw::seedAllSecurityFlaws( SgProject *project )
   {
     std::vector<SecurityFlaw*>::iterator i = securityFlawCollection.begin();
     while (i != securityFlawCollection.end())
        {
       // (*i)->seedSecurityFlaws(project);
          (*i)->seedWithGrainularity(project);
          i++;
        }
   }


void
SecurityFlaw::addComment( SgNode* astNode, std::string comment )
   {
  // This function adds a comment before the statement contained by the input IR node.

  // Now add a comment to make clear that this is a location of a seeded security flaw
  // std::string comment = "// *** NOTE Seeded Security Flaw: BufferOverFlowSecurityFlaw ";
     PreprocessingInfo* commentInfo = new PreprocessingInfo(PreprocessingInfo::CplusplusStyleComment, 
               comment,"user-generated",0, 0, 0, PreprocessingInfo::before, false, true);
     SgStatement* associatedStatement = TransformationSupport::getStatement(astNode);
     associatedStatement->addToAttachedPreprocessingInfo(commentInfo);
   }

int
SecurityFlaw::uniqueValue()
   {
  // This function retruns a unique integer value and is used to build names of functions, 
  // variable, etc. to avoid name collisions.

     static int i = 0;
     i++;

     return i;
   }



GrainularitySpecification::GrainularitySpecification()
   {
  // Select a default:
  //    e_unknown
  //    e_expression
  //    e_statement
  //    e_function
  //    e_class
  //    e_file

     grainularityLevel   = e_function;

     testAllLevels       = false;
     enclosingScopeDepth = 0;
   }

GrainularitySpecification::~GrainularitySpecification()
   {
  // Nothing to do here!
   }


void
GrainularitySpecification::set_enclosingScopeDepth( int n ) 
   {
     enclosingScopeDepth = n;
   }

int
GrainularitySpecification::get_enclosingScopeDepth() 
   {
     return enclosingScopeDepth;
   }

void
GrainularitySpecification::set_testAllLevels( bool t ) 
   {
     testAllLevels = t;
   }

bool
GrainularitySpecification::get_testAllLevels() 
   {
     return testAllLevels;
   }

void
GrainularitySpecification::set_grainularityLevel( GrainularitySpecification::GranularityLevelEnum t ) 
   {
     testAllLevels = t;
   }

GrainularitySpecification::GranularityLevelEnum
GrainularitySpecification::get_grainularityLevel() 
   {
     return grainularityLevel;
   }



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

SecurityVulnerabilityAttribute::SecurityVulnerabilityAttribute (SgNode* securityVulnerabilityNode)
   : securityVulnerabilityNode(securityVulnerabilityNode)
   {
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

std::vector<AstAttribute::AttributeEdgeInfo>
SecurityVulnerabilityAttribute::additionalEdgeInfo()
   {
     std::vector<AstAttribute::AttributeEdgeInfo> v;

     std::set<SgNode*>::iterator i = associtedClones.begin();
     while ( i != associtedClones.end() )
        {
          ROSE_ASSERT(securityVulnerabilityNode != NULL);
          printf ("Adding an edge from %p = %s to %p = %s \n",securityVulnerabilityNode,securityVulnerabilityNode->class_name().c_str(),*i,(*i)->class_name().c_str());
          AstAttribute::AttributeEdgeInfo edge (securityVulnerabilityNode,*i,"associated vulnerabilities","");

          v.push_back(edge);

          i++;
        }
#if 0
     printf ("Exiting at base of SecurityVulnerabilityAttribute::additionalEdgeInfo() \n");
     ROSE_ASSERT(false);
#endif
     return v;
   }







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

std::vector<AstAttribute::AttributeEdgeInfo>
SeededSecurityFlawCloneAttribute::additionalEdgeInfo()
   {
     std::vector<AstAttribute::AttributeEdgeInfo> v;

     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(rootOfCloneInOriginalCode != NULL);

     AstAttribute::AttributeEdgeInfo edgeToRootOfCloneInOriginalCode (primarySecurityFlawInClone,rootOfCloneInOriginalCode,"root of clone in original code"," wieght=10.0 arrowsize=7.0 style=\"setlinewidth(7)\" color=orange ");

     v.push_back(edgeToRootOfCloneInOriginalCode);

     return v;
   }


















PrimarySecurityVulnerabilityForCloneAttribute::PrimarySecurityVulnerabilityForCloneAttribute(SgNode* primarySecurityFlawInClone, SgNode* rootOfClone)
   : primarySecurityFlawInClone(primarySecurityFlawInClone), rootOfClone(rootOfClone)
   {
     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(rootOfClone != NULL);
   }

SgNode*
PrimarySecurityVulnerabilityForCloneAttribute::get_primarySecurityFlawInClone()
   {
     return primarySecurityFlawInClone;
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

std::vector<AstAttribute::AttributeEdgeInfo>
PrimarySecurityVulnerabilityForCloneAttribute::additionalEdgeInfo()
   {
     std::vector<AstAttribute::AttributeEdgeInfo> v;

     ROSE_ASSERT(primarySecurityFlawInClone != NULL);
     ROSE_ASSERT(primaryVulnerabilityInOriginalCode != NULL);

     AstAttribute::AttributeEdgeInfo edgeToOriginalCode (primarySecurityFlawInClone,primaryVulnerabilityInOriginalCode,"vulnerabilities in original code"," arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=red ");
  // AstAttribute::AttributeEdgeInfo edgeToOriginalCode (primarySecurityFlawInClone,primaryVulnerabilityInOriginalCode,"vulnerabilities in original code"," arrowsize=7.0 style=\"setlinewidth(7)\" color=red ");
     AstAttribute::AttributeEdgeInfo edgeToRootOfClone (primarySecurityFlawInClone,rootOfClone,"root of clone"," wieght=10.0 arrowsize=7.0 style=\"setlinewidth(7)\" color=springgreen ");

     v.push_back(edgeToOriginalCode);
     v.push_back(edgeToRootOfClone);

     return v;
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

std::string
SecurityVulnerabilityAttribute::additionalNodeOptions()
   {
     return "fillcolor=\"red\",style=filled";
   }

std::string
SecurityFlawOriginalSubtreeAttribute::additionalNodeOptions()
   {
  // return "fillcolor=\"deepskyblue\",style=filled";
     return "fillcolor=\"darkorange\",style=filled";
   }

std::string
SeededSecurityFlawCloneAttribute::additionalNodeOptions()
   {
  // return "fillcolor=\"yellow\",style=filled";
     return "fillcolor=\"springgreen\",style=filled";

   }

std::string
PrimarySecurityVulnerabilityForCloneAttribute::additionalNodeOptions()
   {
  // return "fillcolor=\"greenyellow\",style=filled";
  // return "fillcolor=\"magenta\",style=filled";
     return "fillcolor=\"purple\",style=filled";
   }
