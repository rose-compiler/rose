// Bug Seeding Support header

// Note that instead of having multiple classes we could have one and use an enum for the value
// data member to distinquish the functionality.
class SecurityVulnerabilityAttribute : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where vulnerabilities are possible.
  // Note that this is part of a phase that does not to the static analysis to detect the vulnerability 
  // (that is explicitly not the goal of the security bug seeding).

     public:
       // I am not sure that we will really use this value
          int value;

          SecurityVulnerabilityAttribute (int v) : value(v) {}

          int get_value() { return value; }
   };

class SeededSecurityFlawCloneAttribute : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where we have generated
  // copies of the AST in order to seed the copies with security flaws.

     public:
       // I am not sure that we will really use this value
          SgNode* primarySecurityFlaw;

          SeededSecurityFlawCloneAttribute (SgNode* v) : primarySecurityFlaw(v) {}

          SgNode* get_primarySecurityFlaw() { return primarySecurityFlaw; }
   };

class SecurityFlawOriginalSubtreeAttribute : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where original code was copied.

     public:
       // I am not sure that we will really use this value
          int value;

          SecurityFlawOriginalSubtreeAttribute (int v) : value(v) {}

          int get_value() { return value; }
   };


class GrainularitySpecification
   {
  // This class is used to specify the grainularity of copies of the AST into which we seed security flaws.

     public:
         enum GranularityLevelEnum
            {
              e_unknown    = 0,
              e_expression = 1,
              e_statement  = 2,
              e_function   = 3,
              e_class      = 4,
              e_file       = 5,
              LAST_ENUM_VALUE
            };

      // Specify the kind of grainularity for new code to be generated at
         GranularityLevelEnum grainularityLevel;

      // Select all levels of grainularity (mostly for testing)
         bool testAllLevels;

      // If this is an e_expression, e_statement, or e_class; specify the depth of the vulnerability 
      // for a copy.  For example, if the vulnerability is in an expression, e_statement might be
      // specified and the enclosingScopeDepth set to 2 meaning that levels up in the AST containing
      // the vulnerability would define the subtree to be used to build a copy.  That copy would then
      // be inserted after the original subtree (subtrees can not be selected where this would not be
      // possible (e.g. a for loop can only have a single loop body, etc.).  The details of the 
      // implementation languge enforce a number of constraints that must be obeyed.
         int enclosingScopeDepth;

         GrainularitySpecification();
        ~GrainularitySpecification();

         void set_enclosingScopeDepth( int n );
         int get_enclosingScopeDepth();

         void set_testAllLevels( bool t );
         bool get_testAllLevels();

         void set_grainularityLevel( GranularityLevelEnum t );
         GranularityLevelEnum get_grainularityLevel();
   };



class SecurityFlaw
   {
  // This design permits us to organize the design by security flaw, which should scale better.

     public:
       // User option to permit seeding of original code or a separate code fragement and a selected 
       // level of grainularity (e.g. alternate statement, enclosing statement, function, class, file, etc.).
          bool seedOriginalCode;

       // This value is used to control to AST clone generation to support seeding, but only 
       // when (seedOriginalCode == false).
       // GrainularitySpecification seedGrainulatity;

          SecurityFlaw();
          virtual ~SecurityFlaw();

       // Support for generating a unique integer values used to build names of things (e.g. cloned function namens)
          static int uniqueValue();

       // This is the static collection of all security flaws (each is derived from the SecurityFlaw class)
          static std::vector<SecurityFlaw*> securityFlawCollection;

       // This function must be defined since we at least have to detect the vulnerabilities 
       // (unless we just want to generate simple test codes).
          virtual void detectVunerabilities( SgProject *project ) = 0;

       // This function need not be defined  (since we migh only want to detect vulnerabilities).
       // If it is defined then it specifies the points in the search space where security flaws 
       // would be seeded.  Initial testing may just assume that all possible sites will be seeded.
          virtual void defineSearchSpace();

       // This function need not be defined  (since we migh only want to detect vulnerabilities).
       // virtual void seedSecurityFlaws( SgProject *project );
          virtual void seedWithGrainularity( SgProject *project );

       // This constructs the securityFlawCollection (at a later point this collection can be 
       // tailored using some mechanism not yet defined)
          static void buildAllVunerabilities();

       // This calls the detectVunerabilities() member function for each SecurityFlaw.
          static void detectAllVunerabilities( SgProject *project );

       // This calls the seedSecurityFlaws() member function for each SecurityFlaw
          static void seedAllSecurityFlaws( SgProject *project );

       // Low level support for adding comments to locations in the source code where security 
       // vulnerabilities are identified or where security flaws are seeded.
          static void addComment( SgNode* astNode, std::string comment );
   };

