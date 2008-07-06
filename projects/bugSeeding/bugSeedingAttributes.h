
class SecurityFlaw;
class SecurityFlaw::Vulnerability;

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

          virtual std::string additionalNodeOptions();

          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo();
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

          void set_primaryVulnerabilityInOriginalCode(SgNode* node);
          SgNode* get_primaryVulnerabilityInOriginalCode();

          virtual std::string additionalNodeOptions();

          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo();
   };

