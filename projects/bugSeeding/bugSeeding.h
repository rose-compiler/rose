// Bug Seeding Header


class InheritedAttribute
   {
     public:
          bool isLoop;
          bool isVulnerability;
          InheritedAttribute() : isLoop(false),isVulnerability(false) {}
          InheritedAttribute(const InheritedAttribute & X) : isLoop(X.isLoop),isVulnerability(X.isVulnerability) {}
   };

class BugSeeding : public SgTopDownProcessing<InheritedAttribute>
   {
     public:
          InheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode, 
             InheritedAttribute inheritedAttribute );
   };


