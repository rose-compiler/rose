#include "programModel.h"

/* This modeling attribute marks function calls
 */
class FunctionCallModelAttribute : public ModelingAttribute
   {
     public:
      //! Function Calls can be modeled in different ways
          enum FunctionCallTypeEnum 
             {
               unknownFunctionCall = 0,
               inlinedFunctionCall = 1, //! marks likely inlined function
               normalFunctionCall  = 2, //! marks likely normal function call
               LAST_FUNCTION_CALL_TYPE
             };

          FunctionCallModelEnum functionCallModelType;

          modelingAttribute (FunctionCallTypeEnum functionCallTypeParameter = normalFunctionCall ) : functionCallModelType(functionCallModelTypeParameter) {}
   };

