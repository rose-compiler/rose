#ifndef FIXUP_DECLARATION_SCOPE_H
#define FIXUP_DECLARATION_SCOPE_H

// DQ (6/11/2013):
/*! \brief Fixup all SgDeclarationStatement to have a consistant scope (between the defining and all non-defining declarations).

    \implementation Some declarations are there own defining declarations.
 */
void fixupAstDeclarationScope ( SgNode* node );

class FixupAstDeclarationScope
// : public AstSimpleProcessing
   : public ROSE_VisitTraversal
   {
  // DQ (6/11/2013): This corrects where EDG can set the scope of a friend declaration to be different from the defining declaration.
  // We need it to be a rule in ROSE that the scope of the declarations are consistant between defining and all non-defining declaration).

     public:
       // This the sets of all associated declaration (defining and all non-defining) and is indexed by the firstNondefiningDeclaration.
          std::map<SgDeclarationStatement*,std::set<SgDeclarationStatement*>* > mapOfSets;

          virtual ~FixupAstDeclarationScope() {};
          void visit ( SgNode* node );

       // DQ (3/24/2016): Adding Robb's meageage mechanism (data member and function).
          static Sawyer::Message::Facility mlog;
          static void initDiagnostics();
   };

// endif for FIXUP_DECLARATION_SCOPE_H
#endif
