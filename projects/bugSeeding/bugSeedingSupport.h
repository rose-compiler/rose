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

                    class Traversal : public SgTopDownProcessing<InheritedAttribute>
                       {
                         public:
                           // This function defines how to recognise the vulnerability
                              InheritedAttribute evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute );
                       };

//             Vulnerability() {}

               void detector( SgProject *project );

             };


          class SeedTraversal : public SgSimpleProcessing
             {
               public:
                 // This function defines how to seed the security flaw at
                 // the previously detected location of the vulnerability
                    void visit( SgNode* node );
             };

         std::vector<Vulnerability*> vulnerabilityKindList;

         void detectVunerabilities( SgProject *project );

         void seedSecurityFlaws( SgProject *project );
   };


#endif
