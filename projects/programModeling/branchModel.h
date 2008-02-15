#include "programModel.h"

/* This modeling attribute marks branchs in the control flow
 */
class BranchModelAttribute : public ModelingAttribute
   {
     public:

      //! Branches can be modled in different ways
          enum BranchTypeEnum 
             {
               unknownBranch        = 0,
               speculativeBranch    = 1, //! marks likely register access
               nonSpeculativeBranch = 2, //! marks likely scalar access
               LAST_BRANCH_TYPE
             };

          BranchModelEnum    branchModelType;

          modelingAttribute (BranchTypeEnum branchTypeParameter = nonSpeculativeBranch ) : branchType(branchTypeParameter) {}
   };


