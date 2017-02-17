#ifndef FIXUP_TEMPLATE_ARGUMENTS_H
#define FIXUP_TEMPLATE_ARGUMENTS_H

// DQ (11/27/2016): Provide alternative typedef type that when unparsed will not contain private types).
// EDG can in some cases cause a template argument to reference a type which could not be used in the 
// generated (unparsed) code.  An example of this is test2016_109.C which is taken from a slightly more
// complex version (test2016_52.C and test2016_53.C).  This was not a problem with older versions of the 
// GNU compiler because they didn't enforce the rule about using private types in template instantiations.
// GNU version 6.1 does enforce this (and likely versions between GNU version 4.8 and version 6.1).

// When this happens there is always an alternative type that EDG could have used, and the trick is to 
// identify it and provide a pointer to use it instead.  This appears to be a normalization within EDG,
// one that only effects the use in ROSE as a source-to-source compiler.

#if 0
class FixupTemplateArguments
// : public SgSimpleProcessing
   : public ROSE_VisitTraversal
#else
class FixupTemplateArguments : public SgSimpleProcessing
#endif
   {
     public:
      //! Function to support traversal of types (where islands can hide)
       // void visitType ( SgType* typeNode );

      //! Required traversal function
          void visit (SgNode* node);

       // FixupTemplateArguments () {}
          void processTemplateArgument ( SgTemplateArgument* templateArgument, SgScopeStatement* targetScope );

       // This avoids a warning by g++
       // virtual ~FixupTemplateArgumentsOnMemoryPool(); 

          bool contains_private_type ( SgType* type, SgScopeStatement* targetScope );
          bool contains_private_type ( SgTemplateArgument* templateArgument, SgScopeStatement* targetScope );
          bool contains_private_type ( SgTemplateArgumentPtrList & templateArgListPtr, SgScopeStatement* targetScope );

       // DQ (2/11/2017): Debugging support.
          std::string generate_string_name ( SgType* type, SgNode* nodeReferenceToType );
   };


// void fixupTemplateArguments();
void fixupTemplateArguments( SgNode* node );

// endif for FIXUP_TEMPLATE_ARGUMENTS_H
#endif

