
// Note that instead of having multiple classes we could have one and use an enum for the value
// data member to distinquish the functionality.
class SecurityVulnerabilityAttribute : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where vulnerabilities are possible.
  // Note that this is part of a phase that does not to the static analysis to detect the vulnerability 
  // (that is explicitly not the goal of the security bug seeding).

     public:
       // I am not sure that we will really use this value
          SgNode* securityVulnerabilityNode;

       // Store a set of clones that have been constructed to support the analysis of this security 
       // flaw at this vulnerability point in the source code.
          std::set<SgNode*> associtedClones;

          void set_associatedClones(SgNode* node);

          SecurityVulnerabilityAttribute (SgNode* securityVulnerabilityNode);

          SgNode* get_securityVulnerabilityNode();
          void set_securityVulnerabilityNode(SgNode* node);

          virtual std::string additionalNodeOptions();

          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
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
   };

class PrimarySecurityVulnerabilityForCloneAttribute : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where we have generated
  // copies of the AST in order to seed the copies with security flaws.

     public:
          SgNode* primarySecurityFlawInClone;
          SgNode* primaryVulnerabilityInOriginalCode;
          SgNode* rootOfClone;

          PrimarySecurityVulnerabilityForCloneAttribute (SgNode* primarySecurityFlawInClone, SgNode* rootOfClone);

          SgNode* get_primarySecurityFlawInClone();

          void set_primaryVulnerabilityInOriginalCode(SgNode* node);
          SgNode* get_primaryVulnerabilityInOriginalCode();

          virtual std::string additionalNodeOptions();

          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
   };

