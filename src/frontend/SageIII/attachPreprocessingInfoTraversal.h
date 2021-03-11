#ifndef _ATTACH_PREPROCESSING_INFO_TRAVERSAL_H_
#define _ATTACH_PREPROCESSING_INFO_TRAVERSAL_H_

// DQ (4/5/2006): Andreas has removed this code!

// void printOutComments ( SgLocatedNode* locatedNode );

// Need dummy classes and the actual tree traversal class
// DQ: Renamed classes, can't have DI and DS polluting the global name space (potential for strange errors)
// class DI : public SgInheritedAttribute {};
// class DS : public SgSynthesizedAttribute {};

// DQ (4/30/2020): We no long need this in the new simplified support for CPP directivces and comments and unparsing of header files.
// DQ (12/12/2008): This is the type use to hold all the CPP directives and comments for each of many files.
// typedef std::map<int, ROSEAttributesList*> AttributeMapType;
// typedef std::map<int, int> StartingIndexAttributeMapType;
// typedef std::map<int, SgLocatedNode*> previousLocatedNodeInFileType;

// DQ (11/29/2008): I don't think these are required to be derived from a special class any more!
// class AttachPreprocessingInfoTreeTraversalInheritedAttrribute  : public AstInheritedAttribute {};
// class AttachPreprocessingInfoTreeTraversalSynthesizedAttribute : public AstSynthesizedAttribute {};
class AttachPreprocessingInfoTreeTraversalInheritedAttrribute
   {
  // DQ (11/30/2008): I want to permit different list of directives and comments to be woven into the AST.
  // Comments and directives from the original source file need to be inserted into the AST for C/C++/Fortran.
  // However, for Fortran we also need to gather and insert the linemarker directives into the AST so that
  // we can support an analysis of the AST that will mark where code has been included from for the case of 
  // Fortran using CPP directives (e.g. #include directives).  To support this the mechanism for weaving
  // the ROSEAttributesList has to be used twice (just for CPP Fortran code) and we need to use this
  // weaving implementation with two different lists of directives.  By moving the ROSEAttributesList
  // into the inherited attribute we can set it differently for the two times we require it to be done.

     public:
       // DQ (12/12/2008): Make this a map to handle the attributes from more than one file (even if we 
       // only handle a single file, this added flexability is easier to support directly than to have 
       // an outer traversal vll an inner traversal).  This more general interface supports the case
       // where we save all comments and CPP directives used from include files in addition to the main 
       // source file.
       // ROSEAttributesList* currentListOfAttributes;
       // AttributeMapType* attributeMapForAllFiles;

#if 0
       // Constructor.
          AttachPreprocessingInfoTreeTraversalInheritedAttrribute(ROSEAttributesList* listOfAttributes)
             : currentListOfAttributes(listOfAttributes)
             {
            // Nothing else to do here.
             }
#else
       // AttachPreprocessingInfoTreeTraversalInheritedAttrribute(AttributeMapType* attributeMap) : attributeMapForAllFiles(attributeMap)
          AttachPreprocessingInfoTreeTraversalInheritedAttrribute()
             {
            // Nothing else to do here.

            // DQ (8/6/2012): This is a part of fixing the CPP directives for templates until they are no longer unparsed as strings.
               isPartOfTemplateDeclaration = false;

            // DQ (7/1/2014): We need to make sure that CPP directives will not be attached to template instantiations that might 
            // not be unparsed (see test2014_68.C).
               isPartOfTemplateInstantiationDeclaration = false;

            // Pei-Hung (09/17/2020): We need to check if SgInitializedName is part of SgFunctionParameterList and the comment
            // will not be attached to it. 
               isPartOfFunctionParameterList = false;
             }
#endif

       // DQ (8/6/2012): Added copy constructor.
          AttachPreprocessingInfoTreeTraversalInheritedAttrribute(const AttachPreprocessingInfoTreeTraversalInheritedAttrribute & X);

       // DQ (8/6/2012): This is a part of fixing the CPP directives for templates until they are no longer unparsed as strings.
          bool isPartOfTemplateDeclaration;

       // DQ (7/1/2014): We need to make sure that CPP directives will not be attached to template instantiations that might 
       // not be unparsed (see test2014_68.C).
          bool isPartOfTemplateInstantiationDeclaration;

      // Pei-Hung (09/17/2020): We need to check if SgInitializedName is part of SgFunctionParameterList and the comment
      // will not be attached to it. 
         bool isPartOfFunctionParameterList;
   };

// This is an empty class, meaning that we could likely just have implemented just a TopDownProcessing traversal.
class AttachPreprocessingInfoTreeTraversalSynthesizedAttribute {};

class AttachPreprocessingInfoTreeTrav 
   : public SgTopDownBottomUpProcessing<AttachPreprocessingInfoTreeTraversalInheritedAttrribute,
                                        AttachPreprocessingInfoTreeTraversalSynthesizedAttribute>
   {
     // negara1 (08/12/2011): These lists contain correspondingly pairs of <include_stmt_to_be_inserted, stmt_before_which_should_insert>
     //                       and <include_stmt_to_be_inserted, stmt_after_which_should_insert>
     private:
         std::list<std::pair<SgIncludeDirectiveStatement*, SgStatement*> > statementsToInsertBefore;
         std::list<std::pair<SgIncludeDirectiveStatement*, SgStatement*> > statementsToInsertAfter;
         
     protected: // Pi-- private:
       //! accumulator attribute
       // SgLocatedNode *previousLocNodePtr;

       // DQ (4/30/2020): We no longer need this in the new simplified support for CPP directivces and comments and unparsing of header files.
       // Store the location in the AST of the previous node associated with each file.
       // previousLocatedNodeInFileType previousLocatedNodeMap;
          SgLocatedNode* previousLocatedNode;

       // DQ (4/30/2020): We no long need this in the new simplified support for CPP directivces and comments and unparsing of header files.
       // DQ (11/30/2008): This is now stored in the inherited attribute (so that it can be set external to the traversal).
       // List of all comments and CPP directives
       // ROSEAttributesList *currentListOfAttributes;
       // AttributeMapType attributeMapForAllFiles;

       // DQ (4/30/2020): Adding back the original simile level of support for a single ROSEAttributesList data member.
          ROSEAttributesList *currentListOfAttributes;

       // DQ (12/12/2008): I don't think this is required since it is just the list size!
       // size of list?
       // int sizeOfCurrentListOfAttributes;

       // DQ (12/12/2008): This allows buildCommentAndCppDirectiveList() to get information about what language 
       // and version of language (fixed or free format for Fortran) as required to gather CPP directives and 
       // comments (more for comments than for CPP directives).  This is required even if processing other files
       // (include files).
      //! current source file name id (only handle strings from current file)
       // int currentFileNameId;
         SgSourceFile* sourceFile;

      // DQ (6/23/2020): We only want to process located nodes that are associated with this file id.
         int target_source_file_id;

      // DQ (2/28/2019): We need to return the line that is associated with the source file where this can be a node shared between multiple ASTs.
         int source_file_id;

      //! AS(011306) Map of ROSEAttributesLists mapped to filename from Wave
       // DQ (12./12/2008): this should be updated to use int instead of strings.
       // For now I will not touch the Wave specific implementation.
       // std::map<std::string,ROSEAttributesList*>* currentMapOfAttributes;

      //! Use_Wave == true specifies if a wave preprocessor is used
          bool use_Wave;

      //  The mapOfAttributes declaration is specific to wave usage.
      //! Map of filenames to list of attributes as found by WAVE.  
       //   std::map<std::string,ROSEAttributesList*>* mapOfAttributes; 

       // DQ (4/30/2020): We no long need this in the new simplified support for CPP directivces and comments and unparsing of header files.
      //! AS(092107) Optimization variable to avoid n^2 complexity in 
      //! iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber()         
       // int start_index;
       // StartingIndexAttributeMapType startIndexMap;

       // DQ (4/30/2020): Adding back the original simile level of support for a single ROSEAttributesList data member.
          int start_index;

       // DQ (12/16/2008): Added support to collect CPP directives and comments from all 
       // include files (except should specified using exclusion lists via the command line).
          bool processAllIncludeFiles;

     public:
       // DQ (9/24/2007): Moved function definition to source file from header file.
       // AS(011306) Constructor for use of Wave Preprocessor
          AttachPreprocessingInfoTreeTrav( std::map<std::string,ROSEAttributesList*>* attrMap);

     public:

       // Destructor
          ~AttachPreprocessingInfoTreeTrav();

       // DQ (6/2/2020): Change the API for this function.
       // DQ (9/24/2007): Moved function definition to source file from header file.
       // Constructor
       // AttachPreprocessingInfoTreeTrav( SgSourceFile* file, bool includeDirectivesAndCommentsFromAllFiles );
          AttachPreprocessingInfoTreeTrav( SgSourceFile* file, ROSEAttributesList* listOfAttributes );
#if 0
          AttachPreprocessingInfoTreeTrav();
#endif
          void setupPointerToPreviousNode (SgLocatedNode* currentLocNodePtr );

          void iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
             ( SgLocatedNode* locatedNode, int lineNumber, PreprocessingInfo::RelativePositionType location,
               bool reset_start_index, ROSEAttributesList *currentListOfAttributes );

       // Member function to be executed on each node of the AST
       // in the course of its traversal
          AttachPreprocessingInfoTreeTraversalInheritedAttrribute
             evaluateInheritedAttribute( SgNode *n, AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh);

          AttachPreprocessingInfoTreeTraversalSynthesizedAttribute
             evaluateSynthesizedAttribute( SgNode *n, AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh, SubTreeSynthesizedAttributes st);

       // DQ (10/27/2007): Added display function to output information gather durring the collection of 
       // comments and CPP directives across all files.
          void display(const std::string & label) const;

       // DQ (4/30/2020): We no long need this in the new simplified support for CPP directivces and comments and unparsing of header files.
       // AttributeMapType & get_attributeMapForAllFiles() { return attributeMapForAllFiles; }
       
       // Access function for elements in the map of attribute lists.
          ROSEAttributesList* getListOfAttributes ( int currentFileNameId );

          void setMapOfAttributes();


       // output for debugging.
       // void display_static_data( const std::string & label ) const;

       // DQ (7/4/2020): Make this a static function to support Fortran handling.
       // DQ (11/30/2008): Refactored code to isolate this from the inherited attribute evaluation.
       // static ROSEAttributesList* buildCommentAndCppDirectiveList ( SgFile *currentFilePtr, std::map<std::string,ROSEAttributesList*>* mapOfAttributes, bool use_Wave );
       // ROSEAttributesList* buildCommentAndCppDirectiveList ( bool use_Wave, std::string currentFilename );
       // static ROSEAttributesList* buildCommentAndCppDirectiveList ( bool use_Wave, std::string currentFilename );
          static ROSEAttributesList* buildCommentAndCppDirectiveList ( bool use_Wave, SgSourceFile* sourceFile, std::string currentFilename );
   };

#endif

// EOF
