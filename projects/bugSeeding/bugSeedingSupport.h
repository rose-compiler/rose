// Bug Seeding Support header

#if 0
// Initial idea of design ...

namespace VulnerabilityDetection
   {
  // This namespace separates the support for the detection of vulnerabilities 
  // from the transformations required to seed vulnerabilities.

     class InheritedAttribute
        {
          public:
               bool isLoop;
               bool isVulnerability;
               InheritedAttribute() : isLoop(false),isVulnerability(false) {}
               InheritedAttribute(const InheritedAttribute & X) : isLoop(X.isLoop),isVulnerability(X.isVulnerability) {}
        };

     class Traversal : public SgTopDownProcessing<InheritedAttribute>
        {
          public:
               InheritedAttribute evaluateInheritedAttribute (
                  SgNode* astNode, 
                  InheritedAttribute inheritedAttribute );
        };
   }
#endif


#if 1

class SecurityVulnerabilityAttribute : public AstAttribute
   {
  // This are the persistatn attributes used to mark locations in the AST wehre vulnerabilities are possible.

     public:
          int value;
          SecurityVulnerabilityAttribute (int v) : value(v) {}

          int get_value() { return value; }
   };

class SecurityFlaw
   {
  // This design permits us to organize the design by security flaw, which should scale better.

     public:

       // This function must be defined since we at least have to detect the vulnerabilities 
       // (unless we just want to generate simple test codes).
          virtual void detectVunerabilities( SgProject *project ) = 0;

       // This function need not be defined  (since we migh only want to detect vulnerabilities).
          virtual void defineSearchSpace();

       // This function need not be defined  (since we migh only want to detect vulnerabilities).
          virtual void seedSecurityFlaws( SgProject *project );
   };

class BufferOverFlowSecurityFlaw : public SecurityFlaw
   {
     public:

       // Note that there can be many vulnerabilities for a single security flaw (arrays indexing inside
       // a loop body, array indexing in a conditional test, indexing of a STL vector, etc.)
          class Vulnerability
             {
               public:
                 // This class is only required where the specific security flaw's vulnerability detection requires it.
                    class InheritedAttribute
                       {
                      // This class defines the constraints under which a vulnerability is defined.
                         public:
                              bool isLoop;
                              InheritedAttribute() : isLoop(false) {}
                              InheritedAttribute(const InheritedAttribute & X) : isLoop(X.isLoop) {}
                       };

                 // This is the ROSE AST traversal mechanism (see ROE Tutorial for details).
                    class Traversal : public SgTopDownProcessing<InheritedAttribute>
                       {
                         public:
                           // This function defines how to recognise the vulnerability
                              InheritedAttribute evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute );
                       };

               void detector( SgProject *project );
             };

       // Note that there can be many ways to seed a security flaw into an application 
       // (or generate it separately from it being seeded into an existing application).
          class SeedSecurityFlaw
             {
            // This class introduces a single kind of seeding at either a specific grainularity 
            // (file, function, block, statement) or using a specific mechanism to hide it as a 
            // security flaw (hidden behind modification of array indexing, or behind a modification 
            // to the loop bound, etc.).

               public:

                 // This is the ROSE AST traversal mechanism (see ROE Tutorial for details).
                    class SeedTraversal : public SgSimpleProcessing
                       {
                         public:
                           // This function defines how to seed the security flaw at
                           // the previously detected location of the vulnerability
                              void visit( SgNode* node );
                       };

            // For any security flaw marked previously as a vulnerability, back-track up the AST to a subtree 
            // to copy so that the seeded security flaw can be introduced in the copy (so that we can leave 
            // in place the original code associated with the security vulnerability.
               static SgNode* grainularityOfSeededCode( SgNode* astNode );

               void seed( SgProject *project );
             };

       // Collection of different sorts of vulnerabilities that we want to identify as a buffer over flow 
       // security flaw (there can be many).
          std::vector<Vulnerability*> vulnerabilityKindList;

       // Collection of different way to seed buffer overflow security flaws into code (in a loop, in a 
       // conditional test, etc.).
          std::vector<SeedSecurityFlaw*> seedKindList;

          void detectVunerabilities( SgProject *project );

          void seedSecurityFlaws( SgProject *project );
   };


#endif
