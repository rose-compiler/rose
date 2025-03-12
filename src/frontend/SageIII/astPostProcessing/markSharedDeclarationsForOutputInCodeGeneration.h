#ifndef MARK_SHARED_DECLARATIONS_FOR_OUTPUT_IN_CODE_GENERATION_H
#define MARK_SHARED_DECLARATIONS_FOR_OUTPUT_IN_CODE_GENERATION_H

// DQ (2/26/2019):
/*! \brief Mark an AST subtree to be unparsed using an alternative file (for multi-file unparsing).
    \internal The fileIDsToUnparse set in Sg_File_Info is used to indicate what statements shuould be unparsed from alternative file.
 */
void markSharedDeclarationsForOutputInCodeGeneration( SgNode* node );

class MarkSharedDeclarationsInheritedAttribute
   {
     public:
      //! Store previous SgSourceFile for reference.
          SgSourceFile* currentFile;

          SgFileIdList unparseFileIdList;

          MarkSharedDeclarationsInheritedAttribute();
          MarkSharedDeclarationsInheritedAttribute(const MarkSharedDeclarationsInheritedAttribute &);
          MarkSharedDeclarationsInheritedAttribute& operator=(const MarkSharedDeclarationsInheritedAttribute &) = default; // defeat warning
  };

/*! \brief Mark an AST subtree to be unparsed using an alternative file (for multi-file unparsing).
    \internal The fileIDsToUnparse set in Sg_File_Info is used to indicate what statements shuould be unparsed from alternative file.
 */
class MarkSharedDeclarationsForOutputInCodeGeneration : public SgTopDownProcessing<MarkSharedDeclarationsInheritedAttribute>
   {
     public:
       // Adding map to record defining declarations and their associated file ID.
          std::set<int> fileNodeSet;
          std::set<SgDeclarationStatement*> definingDeclarationsSet;

          MarkSharedDeclarationsForOutputInCodeGeneration();

      //! Required traversal function
          MarkSharedDeclarationsInheritedAttribute
               evaluateInheritedAttribute(SgNode* node, MarkSharedDeclarationsInheritedAttribute inheritedAttribute);
   };

// endif for MARK_SHARED_DECLARATIONS_FOR_OUTPUT_IN_CODE_GENERATION_H
#endif
