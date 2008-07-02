// Bug Seeding Support header

class SecurityVulnerabilityAttribute : public AstAttribute
   {
  // This are the persistant attribute used to mark locations in the AST where vulnerabilities are possible.
  // Note that this is part of a phase that does not to the static analysis to detect the vulnerability 
  // (that is explicitly not the goal of the security bug seeding).

     public:
          int value;
          SecurityVulnerabilityAttribute (int v) : value(v) {}

          int get_value() { return value; }
   };

class SecurityFlaw
   {
  // This design permits us to organize the design by security flaw, which should scale better.

     public:
          static int uniqueValue();

       // This is the static collection of all security flaws (each is derived from the SecurityFlaw class)
          static std::vector<SecurityFlaw*> securityFlawCollection;

       // This function must be defined since we at least have to detect the vulnerabilities 
       // (unless we just want to generate simple test codes).
          virtual void detectVunerabilities( SgProject *project ) = 0;

       // This function need not be defined  (since we migh only want to detect vulnerabilities).
          virtual void defineSearchSpace();

       // This function need not be defined  (since we migh only want to detect vulnerabilities).
       // virtual void seedSecurityFlaws( SgProject *project );
          virtual void seedWithGrainularity( SgProject *project );

       // This constructs the securityFlawCollection
          static void buildAllVunerabilities();

       // This calls the detectVunerabilities() member function for each SecurityFlaw
          static void detectAllVunerabilities( SgProject *project );

       // This calls the seedSecurityFlaws() member function for each SecurityFlaw
          static void seedAllSecurityFlaws( SgProject *project );

          static void addComment( SgNode* astNode, std::string comment );
   };


