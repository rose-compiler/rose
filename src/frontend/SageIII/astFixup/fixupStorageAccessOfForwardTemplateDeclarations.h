#ifndef __fixupStorageAccess
#define  __fixupStorageAccess
// DQ (5/20/2005):
/*! \brief Fixup the storage access of all compiler-generated forward declarations (non-defining declarations) 
           to match that of the defining declaration.

    This fix is required because EDG builds forward declarations for template instantations but does not mark 
    the forward declaration as static when using the instantiation mode "local" ("--instantiation local").
    The defining declaration is correctly marked as static.  This fixup only applies to compiler generated 
    forward declarations (placed after the template declaration) and having a valid defining declaration.

    \internal This is a bug in EDG, which I suppose we should report.  We need this because need to instantiate
    templates (ROSE puts them out as specializations) but we want them to appear only locally, since the prelink
    mechanism is still not yet working well enogh to rely on it.  Only transformed templates are output, but such 
    template instatiations would be redundently put out in each file where they were used.
 */
void fixupStorageAccessOfForwardTemplateDeclarations( SgNode* node );

/*! \brief Fixup the storage access of all compiler-generated forward declarations (non-defining declarations) 
           to match that of the defining declaration.

    This fix is required because EDG builds forward declarations for template instantations but does not mark 
    the forward declaration as static when using the instantiation mode "local" ("--instantiation local").
    The defining declaration is correctly marked as static.  This fixup only applies to compiler generated 
    forward declarations (placed after the template declaration) and having a valid defining declaration.

    \internal This is a bug in EDG, which I suppose we should report.  We need this because need to instantiate
    templates (ROSE puts them out as specializations) but we want them to appear only locally, since the prelink
    mechanism is still not yet working well enogh to rely on it.  Only transformed templates are output, but such 
    template instatiations would be redundently put out in each file where they were used.
 */
class FixupStorageAccessOfForwardTemplateDeclarations : public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);
   };


#endif
