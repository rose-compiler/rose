// Header file to support attaching preprocessingInfo objects
// to AST nodes

#ifndef _ATTACH_PREPROCESSING_INFO_H_
#define _ATTACH_PREPROCESSING_INFO_H_

//#include "rose.h"
//#include "Cxx_Grammar.h"

// #include "sage3.h"
// #include "roseInternal.h"
// #include <typeinfo>
// #include "AstProcessing.h"

#if 1
// DQ (4/5/2006): Andreas has removed this code!

// void printOutComments ( SgLocatedNode* locatedNode );

// This function is defined in preproc.C and used to collect the attributes
// extern ROSEAttributesList *getPreprocessorDirectives(const char *fileName);
// extern ROSEAttributesList *getPreprocessorDirectives(const char *fileName);
// ROSEAttributesList *getPreprocessorDirectives(const char *fileName);
// ROSEAttributesList *getPreprocessorDirectives( std::string fileName );
// ROSEAttributesList *getPreprocessorDirectives( std::string fileName, LexTokenStreamTypePointer & input_token_stream_pointer );
ROSEAttributesList *getPreprocessorDirectives( std::string fileName );

// Need dummy classes and the actual tree traversal class
// DQ: Renamed classes, can't have DI and DS polluting the global name space (potential for strange errors)
// class DI : public SgInheritedAttribute {};
// class DS : public SgSynthesizedAttribute {};
class AttachPreprocessingInfoTreeTraversalInheritedAttrribute  : public AstInheritedAttribute {};
class AttachPreprocessingInfoTreeTraversalSynthesizedAttribute : public AstSynthesizedAttribute {};

class AttachPreprocessingInfoTreeTrav 
   : public SgTopDownBottomUpProcessing<AttachPreprocessingInfoTreeTraversalInheritedAttrribute,
                                        AttachPreprocessingInfoTreeTraversalSynthesizedAttribute>
   {
   protected: //Pi-- private:
       //! accumulator attribute
          SgLocatedNode *previousLocNodePtr;

      //! List of all comments and CPP directives
          ROSEAttributesList *currentListOfAttributes;

      //! size of list?
          int sizeOfCurrentListOfAttributes;

      //! current file name id (only handle strings from current file)
          int currentFileNameId;

      //! AS(011306) Map of ROSEAttributesLists mapped to filename from Wave
          std::map<std::string,ROSEAttributesList*>* currentMapOfAttributes;

      //! AS(011306) Use_Wave == true specifies if a wave preprocessor is used
          bool use_Wave;

      //! AS(092107) Optimization variable to avoid n^2 complexity in 
      //! iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber()	  
          int start_index;


     public:
       // DQ (9/24/2007): Moved function definition to source file from header file.
       // AS(011306) Constructor for use of Wave Preprocessor
          AttachPreprocessingInfoTreeTrav( std::map<std::string,ROSEAttributesList*>* attrMap);

     public:

       // DQ (9/24/2007): Moved function definition to source file from header file.
       // Constructor
          AttachPreprocessingInfoTreeTrav();

          void setupPointerToPreviousNode (SgLocatedNode* currentLocNodePtr );

          void iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
             ( SgLocatedNode* locatedNode, int lineNumber, PreprocessingInfo::RelativePositionType location, bool reset_start_index );

       // Member function to be executed on each node of the AST
       // in the course of its traversal
          AttachPreprocessingInfoTreeTraversalInheritedAttrribute
          evaluateInheritedAttribute( SgNode *n, 
                                      AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh);

          AttachPreprocessingInfoTreeTraversalSynthesizedAttribute
          evaluateSynthesizedAttribute( SgNode *n,
                                        AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh,
                                        SubTreeSynthesizedAttributes st);

       // DQ (10/27/2007): Added display function to output information gather durring the collection of 
       // comments and CPP directives across all files.
          void display(const std::string & label) const;
   };
#endif

void attachPreprocessingInfo(SgFile *sageFile);

// AS (012006) Added the function
//     void attachPreprocessingInfo(SgFile *sageFile,  std::map<string,ROSEAttributesList*>*);
// to support reintroductions of macros and fetching of preprocessor directive using Wave. 
// This function does not in itself rely on Wave, but simply takes the same arguement as 
// 'attachPreprocessingInfo(SgFile *sageFile)', but adds an argument std::map<string,ROSEAttributesList*>* 
// which is a map of a pair (filename,list of attributes in that file). The two functions will perform 
// the same tasks in all ways but the way they find the preprocessor diretives. In addition to that
// the mechanism here opens up for reintroduction of unexpanded macros into the AST without
// changing the implementation at all. This relies on some external mechanism, that be the 
// Wave preprocessor or some other, to find the preprocessor directives and the unexpanded 
// macros.
void attachPreprocessingInfo(SgFile *sageFile,  std::map<std::string,ROSEAttributesList*>*);

#endif

// EOF
