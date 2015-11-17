
class SecurityFlaw;
// class SecurityFlaw::Vulnerability;

// Note that instead of having multiple classes we could have one and use an enum for the value
// data member to distinquish the functionality.
class SecurityVulnerabilityAttribute : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where vulnerabilities are possible.
  // Note that this is part of a phase that does not to the static analysis to detect the vulnerability 
  // (that is explicitly not the goal of the security bug seeding).

     public:

       // This is the pointer to the IR node with the potential for a security vulnerability
          SgNode* securityVulnerabilityNode;
          SgNode* securityVulnerabilityNodeInOriginalCode;

       // The pointer to the mechanism used to detect the potential vulnerability
          SecurityFlaw::Vulnerability* vulnerabilityPointer;

       // Store a set of clones that have been constructed to support the analysis of this security 
       // flaw at this vulnerability point in the source code.
          std::set<SgNode*> associtedClones;

       // Member function declarations
          void set_associatedClones(SgNode* node);

          SecurityVulnerabilityAttribute (SgNode* securityVulnerabilityNode, SecurityFlaw::Vulnerability* vulnerabilityPointer );

          SgNode* get_securityVulnerabilityNode();
          void set_securityVulnerabilityNode(SgNode* node);

          SgNode* get_securityVulnerabilityNodeInOriginalCode();
          void set_securityVulnerabilityNodeInOriginalCode(SgNode* node);

       // Support for graphics output of IR nodes using attributes (see the DOT graph of the AST)
          virtual std::string additionalNodeOptions();
          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo();

       // Support for the coping of AST and associated attributes on each IR node (required for attributes 
       // derived from AstAttribute, else just the base class AstAttribute will be copied).
          virtual AstAttribute* copy() const;
   };

class SecurityFlawOriginalSubtreeAttribute : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where original code was copied.

     public:
       // I am not sure that we will really use this value
          int value;

          SecurityFlawOriginalSubtreeAttribute (int v) : value(v) {}

          int get_value() { return value; }

          virtual std::string additionalNodeOptions();

          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo();

          virtual AstAttribute* copy() const;
   };


class SeededSecurityFlawCloneAttribute : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where we have generated
  // copies of the AST in order to seed the copies with security flaws.

     public:
          SgNode* primarySecurityFlawInClone;
          SgNode* rootOfCloneInOriginalCode;

          SeededSecurityFlawCloneAttribute (SgNode* primarySecurityFlawInClone, SgNode* rootOfCloneInOriginalCode);

          void set_primarySecurityFlawInClone(SgNode* node);
          SgNode* get_primarySecurityFlawInClone();

          void set_rootOfCloneInOriginalCode(SgNode* node);
          SgNode* get_rootOfCloneInOriginalCode();

          virtual std::string additionalNodeOptions();

          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo();

          virtual AstAttribute* copy() const;
   };

class PrimarySecurityVulnerabilityForCloneAttribute : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where we have generated
  // copies of the AST in order to seed the copies with security flaws.

     public:
          SgNode* primarySecurityFlawInClone;
          SgNode* primaryVulnerabilityInOriginalCode;
          SgNode* rootOfClone;

       // The pointer to the mechanism used to detect the potential vulnerability
          SecurityFlaw::Vulnerability* vulnerabilityPointer;

          PrimarySecurityVulnerabilityForCloneAttribute (SgNode* primarySecurityFlawInClone, SgNode* rootOfClone, SecurityFlaw::Vulnerability* vulnerabilityPointer );

          SgNode* get_primarySecurityFlawInClone();
          void set_primarySecurityFlawInClone(SgNode* node);

          void set_primaryVulnerabilityInOriginalCode(SgNode* node);
          SgNode* get_primaryVulnerabilityInOriginalCode();

          virtual std::string additionalNodeOptions();

          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo();

          virtual AstAttribute* copy() const;
   };

class SeedMethodologyCloneAttribute : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where original code was copied.

     public:
       // I am not sure that we will really use this value
          SgNode* rootOfSeedMethodologyClone;
          SgNode* originalTreeInVulnerabilityClone;

          SecurityFlaw::SeedSecurityFlaw* seedSecurityFlawPointer;

          SeedMethodologyCloneAttribute ( SgNode* temp_rootOfSeedMethodologyClone, SgNode* temp_originalTreeInVulnerabilityClone, SecurityFlaw::SeedSecurityFlaw* temp_seedSecurityFlawPointer)
             : rootOfSeedMethodologyClone(temp_rootOfSeedMethodologyClone), originalTreeInVulnerabilityClone(temp_originalTreeInVulnerabilityClone), seedSecurityFlawPointer(temp_seedSecurityFlawPointer) {}

          SgNode* get_rootOfSeedMethodologyClone() { return rootOfSeedMethodologyClone; }
          SgNode* get_originalTreeInVulnerabilityClone() { return originalTreeInVulnerabilityClone; }

          SecurityFlaw::SeedSecurityFlaw* get_seedSecurityFlawPointer () { return seedSecurityFlawPointer; }

          virtual std::string additionalNodeOptions();

          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo();

          virtual AstAttribute* copy() const;
   };



class PrimarySeedLocationForSeedMethodologyAttribute : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where we have generated
  // copies of the AST in order to allow each seeding methodology to seed the copies with security flaws.

     public:
          SgNode* primarySecurityFlawInClone;
          SgNode* primaryVulnerabilityInOriginalCode;
          SgNode* rootOfClone;

       // The pointer to the mechanism used to detect the potential vulnerability
          SecurityFlaw::Vulnerability* vulnerabilityPointer;

          PrimarySeedLocationForSeedMethodologyAttribute (SgNode* primarySecurityFlawInClone, SgNode* rootOfClone, SecurityFlaw::Vulnerability* vulnerabilityPointer );

          SgNode* get_primarySecurityFlawInClone();

          void set_primaryVulnerabilityInOriginalCode(SgNode* node);
          SgNode* get_primaryVulnerabilityInOriginalCode();

          virtual std::string additionalNodeOptions();

          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo();

          virtual AstAttribute* copy() const;
   };


class PruningAttribute : public AstAttribute
   {
  // This is a persistant attribute used to mark locations in the AST where we don't want IR nodes to be generated for the DOT graph.

     public:
          bool commentOutNodeInGraph();

          virtual AstAttribute* copy() const;

          virtual ~PruningAttribute();
   };

