// This is where we put the suppprting code for bug seeding.

#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

#include "bugSeeding.h"

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

#if 0
void
SecurityFlaw::seedSecurityFlaws( SgProject *project )
   {
     printf ("Base class function called: SecurityFlaw::seedSecurityFlaws() \n");
   }
#endif

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










