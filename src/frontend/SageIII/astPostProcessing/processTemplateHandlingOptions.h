#ifndef PROCESS_TEMPLATE_HANDLING_OPTIONS_H
#define PROCESS_TEMPLATE_HANDLING_OPTIONS_H

// DQ (8/19/2005):
/*! \brief Fixup template instantiations to unmark them for output following semantics of gnu template options

\implementation This is support for -fno-implicit-templates option and -fno-implicit-inline-templates

\todo Static data members might not be properly be handled.

 */
void processTemplateHandlingOptions ( SgNode* node );

class ProcessTemplateHandlingOptions : public AstSimpleProcessing
   {
     private:
      //! current source file
#if 0 // [Robb Matzke 2021-03-17]: unused
          SgFile* file;
#endif
          bool buildImplicitTemplates;
          bool buildImplicitInlineTemplates;

     public:
          ProcessTemplateHandlingOptions ( SgFile* file, bool buildImplicitTemplates, bool buildImplicitInlineTemplates );
          void visit ( SgNode* node );
   };

// endif for PROCESS_TEMPLATE_HANDLING_OPTIONS_H
#endif
