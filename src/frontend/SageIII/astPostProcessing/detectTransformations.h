#ifndef DETECT_TRANSFORMATIONS_H
#define DETECT_TRANSFORMATIONS_H

// DQ (5/1/2012): 
/*! \brief Detect nodes marked as transformations (should not be present coming out of the frontend translation).
 */
void detectTransformations( SgNode* node );

void detectTransformations_local( SgNode* node );

/*! \brief There sould not be any IR nodes marked as a transformation coming from the EDG/ROSE translation.
           This test enforces this.

    \internal This is only for testing the AST after translation.  User transformations would be caught by this 
              and it should not be used downstream of user transformations.
 */
class DetectTransformations : public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);
   };

// endif for DETECT_TRANSFORMATIONS_H
#endif
