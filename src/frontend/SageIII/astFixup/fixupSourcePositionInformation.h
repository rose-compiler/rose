// DQ (11/1/2006):
/*! \brief Fixup source position information in IR nodes.

   The source position information is important for weaving the 
comments and CPP directives back into the AST ansl also for the
copy-based unparser.


 */
#ifndef __fixupSourcePos
#define __fixupSourcePos



void fixupSourcePositionInformation ( SgNode* node );


class FixupSourcePositionInformationInheritedAttribute 
   {
     public:
          FixupSourcePositionInformationInheritedAttribute();
   };

class FixupSourcePositionInformationSynthesizedAttribute 
   {
     public:
          FixupSourcePositionInformationSynthesizedAttribute();
          FixupSourcePositionInformationSynthesizedAttribute(const FixupSourcePositionInformationSynthesizedAttribute &);
          FixupSourcePositionInformationSynthesizedAttribute&
             operator=(const FixupSourcePositionInformationSynthesizedAttribute &) = default;

          Sg_File_Info* startingPosition;
          Sg_File_Info* endingPosition;
          SgNode* associatedNode;
   };

// class FixupSourcePositionInformation : public AstSimpleProcessing
class FixupSourcePositionInformation 
   : public SgTopDownBottomUpProcessing<FixupSourcePositionInformationInheritedAttribute,
                                        FixupSourcePositionInformationSynthesizedAttribute>
   {
     public:
       // void visit ( SgNode* node );
          FixupSourcePositionInformationInheritedAttribute
               evaluateInheritedAttribute ( SgNode* node,
                                            FixupSourcePositionInformationInheritedAttribute ia );

          FixupSourcePositionInformationSynthesizedAttribute
               evaluateSynthesizedAttribute ( SgNode* node,
                                            FixupSourcePositionInformationInheritedAttribute ia,
                                            SynthesizedAttributesList salist );
     private:
          void processChildSynthesizedAttributes ( FixupSourcePositionInformationSynthesizedAttribute & returnAttribute, SynthesizedAttributesList & l );
          void fixupCompilerGeneratedNodesToBeUniformlyMarked ( SgLocatedNode* locatedNode );
          void fixupSourcePositionUnavailableInFrontendToBeUniformlyMarked ( SgLocatedNode* locatedNode );
   };



/*! \brief This traversal uses the Memory Pool traversal to fixup the source position constructs.

    This allows us to make sure that there is alwasy and ending position construct to match the starting position construct.
 */
class FixupSourcePositionConstructs : public ROSE_VisitTraversal
   {
     public:
     virtual ~FixupSourcePositionConstructs() {};
      //! Required traversal function
          void visit (SgNode* node);
   };

void fixupSourcePositionConstructs();

#endif

