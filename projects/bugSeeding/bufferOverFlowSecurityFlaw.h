// This is the header file just for the BufferOverFlowSecurityFlaw,
// other header files would be defined, one for each security flaw.

// This file represents a single type of security flaw, with both locations in source code where 
// such flaws can be present, and ways in which such flaws can be seeded into source code:
//    1) The class BufferOverFlowSecurityFlaw
//       can have many nested classes to represent different ways in which the security flaw can
//       appear in source code (locations in the source code where it could exist).  The 
//       Vulnerability class is just one instance. 
//    2) The class BufferOverFlowSecurityFlaw can also have many nested classes to represent ways
//       in which the security flaw could be seeded into an application.  The SeedSecurityFlaw class
//       is just one instance.
// It may or may not be a one-to-one mapping between these nested classes.

class BufferOverFlowSecurityFlaw : public SecurityFlaw
   {
     public:

       // Note that there can be many vulnerabilities for a single security flaw (arrays indexing inside
       // a loop body, array indexing in a conditional test, indexing of a STL vector, etc.).
       // In general, vulnerabilities can be independent of the ways in which a security flaw can be seeded,
       // so this is no one-to-one mapping of Vulnerability classes to SeedSecurityFlaw classes.
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

       // Constructor and destructor
          BufferOverFlowSecurityFlaw();
          virtual ~BufferOverFlowSecurityFlaw();

       // Marks all locations in source code where this type of security flaw could exist (marks with AST 
       // attribute). This function uses the vulnerabilityKindList to address each kind of source code 
       // vulnerability for this security flaw.
          void detectVunerabilities( SgProject *project );

       // Seeds this security flaw into the marked location of a potential vulnerability (using multiple 
       // ways of seeding the flaw as defined in the seedKindList).
          void seedSecurityFlaws( SgProject *project );
   };

