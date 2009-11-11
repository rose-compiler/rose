/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2007-2009 Adrian Prantl <adrian@complang.tuwien.ac.at>
          2009 Gergö Barany <gergo@complang.tuwien.ac.at>
*/
#ifndef PROLOGTRAVERSAL_H_
#define  PROLOGTRAVERSAL_H_
#include <iostream>
#include <string>
#include <time.h>
#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include <satire_rose.h>
#include "termite.h"
#include <climits>

// GB (2009-02-25): Want to build with ICFG support unless explicitly told
// (by c2term.C) not to.
#ifndef HAVE_SATIRE_ICFG
#  define HAVE_SATIRE_ICFG 1
#endif

#if HAVE_SATIRE_ICFG
#  include <cfg_support.h>
#  include <satire_program_representation.h>
#endif

#if HAVE_PAG && !defined(DO_NOT_USE_DFIPRINTER)
#  include <config.h>
// GB (2008-10-03): The declaration (and definition) of PagDfiTextPrinter is
// now provided in a special header.
#  include <PagDfiTextPrinter.h>
#endif

#include "RoseToTerm.h"
#include <sstream>
#include <iostream>

#include <aslanalysis.h>
#include <aslattribute.h>


/* See main.C-template and toProlog.C for examples how to use this */
  
/**
 * Class implementing a traversal to create a PROLOG term 
 * representing a ROSE-IR
 *
 * Additionally, this template can be instatiated with the DFI_STORE type
 * in the case the Terms should contain PAG analysis results.
 * This feature is only enabled if Termite was configured with PAG support
 * (i.e., HAVE_PAG is defined).
 */
template<typename DFI_STORE_TYPE>
class TermPrinter: public AstBottomUpProcessing<PrologTerm*> 
{
public:
  TermPrinter(DFI_STORE_TYPE analysis_info = 0
            , std::string analysisname_ = ""
#if HAVE_SATIRE_ICFG
            , CFG *cfg = 0
            , SATIrE::Program *program = 0
#endif
            )
      :
#if HAVE_PAG && !defined(DO_NOT_USE_DFIPRINTER)
      pagDfiTextPrinter(analysis_info),
#endif
      analysisname(analysisname_ != "" ? analysisname_ : "unknown")
#if HAVE_SATIRE_ICFG
    , cfg(cfg)
    , program(program)
#else
    , cfg(NULL)
    , program(NULL)
#endif
  { 
#if HAVE_SWI_PROLOG
    int argc;
    char **argv;
    assert(PL_is_initialised(&argc, &argv) 
     && "please run init_termite(argc, argv) first.");
#endif  
    withPagAnalysisResults = (analysis_info != 0); 
    rootTerm = new PrologAtom("error");
  }


  /** return the term*/
  PrologTerm* getTerm() {return rootTerm;};

protected:
  /** reimplemented from AstBottomUpProcessing*/
  virtual PrologTerm*
    evaluateSynthesizedAttribute(SgNode* astNode, SynthesizedAttributesList synList);
  /** reimplemented from AstBottomUpProcessing*/
  virtual PrologTerm*
    defaultSynthesizedAttribute() {return new PrologAtom("null");};

  bool withPagAnalysisResults;
#if HAVE_PAG && !defined(DO_NOT_USE_DFIPRINTER)
    PagDfiTextPrinter<DFI_STORE_TYPE> pagDfiTextPrinter;
#endif

private:
  /** should generate list instead of tuple? */
  bool isContainer(SgNode* astNode);
  /** return the number of successors */
  int getArity(SgNode* astNode);

  /** create leaf nodes*/
  PrologCompTerm* leafTerm(SgNode* astNode, SynthesizedAttributesList synList,
                           PrologTerm* specific, PrologTerm* ar, PrologTerm* fiTerm);
  /** create unary nodes*/
  PrologCompTerm* unaryTerm(SgNode* astNode, SynthesizedAttributesList synList,
                           PrologTerm* specific, PrologTerm* ar, PrologTerm* fiTerm);
  /** create binary nodes*/
  PrologCompTerm* binaryTerm(SgNode* astNode, SynthesizedAttributesList synList,
                           PrologTerm* specific, PrologTerm* ar, PrologTerm* fiTerm);
  /** create ternary nodes*/
  PrologCompTerm* ternaryTerm(SgNode* astNode, SynthesizedAttributesList synList,
                           PrologTerm* specific, PrologTerm* ar, PrologTerm* fiTerm);
  /** create quaternary nodes*/
  PrologCompTerm* quaternaryTerm(SgNode* astNode, SynthesizedAttributesList synList,
                           PrologTerm* specific, PrologTerm* ar, PrologTerm* fiTerm);
  /** create list nodes*/
  PrologCompTerm* listTerm(SgNode* astNode, SynthesizedAttributesList synList,
                           PrologTerm* specific, PrologTerm* ar, PrologTerm* fiTerm);

  /** the current term */
  PrologTerm* rootTerm;

  /** the converter */
  RoseToTerm termConv;

  /** the name of the analysis, if available */
  std::string analysisname;

#if HAVE_SATIRE_ICFG
  /** the CFG */
  CFG *cfg;
  SATIrE::Program *program;

  /** create integer term with variable ID or "null" atom */
  PrologTerm* varidTerm(SgVariableSymbol *symbol);
  /** create a term containing the procnum for the given procedure, when
   * looked up in a certain file (the file matters for static functions);
   * and a term containing a pair Entry-Exit of ICFG labels; plus a helper
   * function */
  PrologCompTerm* functionIdAnnotation(std::string funcname, SgFile *file);
  PrologCompTerm* functionEntryExitAnnotation(std::string funcname,
                                              SgFile *file);
  Procedure* procedureNode(std::string funcname, SgFile *file);
#else
  /** dummy member */
  void *cfg;
  void *program;
#endif

  PrologList* getAnalysisResultList(SgStatement* stmt);
  PrologCompTerm* pagToProlog(std::string name, std::string analysis,
                              std::string dfi);
};

typedef TermPrinter<void*> BasicTermPrinter;

/***********************************************************************
 * TermPrinter (Implementation)
 ***********************************************************************/

template<typename DFI_STORE_TYPE>
bool
TermPrinter<DFI_STORE_TYPE>::isContainer(SgNode* astNode) 
{
  // AP 2.2.2008 new rose (hotel) compatibility
  // GB (2008-12-11): AstTests::numSuccContainers does not report container
  // nodes that happen to be empty at the moment. This is a clear design
  // problem in the way it is implemented (in theory, a correct
  // implementation could be generated for each node by using ROSETTA). This
  // is really problematic for empty SgFunctionParameterLists! However,
  // other than patching ROSE, which is not really an option, I can't see
  // how to implement this correctly. In any case, I added the parameter
  // lists as a special case here, and this should be done for any other
  // maybe-empty list nodes we stumble upon.
  // FIXME: Revisit this issue, and think of a real fix!
  // GB (2009-JAN): Added SgBasicBlock and SgExprListExp as maybe-emtpy
  // containers. Added SgForInitStatement.
  return AstTests::numSuccContainers(astNode) ||
    isSgFunctionParameterList(astNode) ||
    isSgExprListExp(astNode) ||
    isSgBasicBlock(astNode) ||
    isSgForInitStatement(astNode) ||
    isSgClassDefinition(astNode) ||
    isSgVariableDeclaration(astNode);
}

template<typename DFI_STORE_TYPE>
int
TermPrinter<DFI_STORE_TYPE>::getArity(SgNode* astNode) 
{
  return AstTests::numSingleSuccs(astNode);
}

template<typename DFI_STORE_TYPE>
PrologTerm* TermPrinter<DFI_STORE_TYPE>::evaluateSynthesizedAttribute(
  SgNode* astNode, SynthesizedAttributesList synList) {
  /*
   * Compute the PROLOG representation of a node using
   * the successors' synthesized attributes.
   *
   * o nodes with a fixed number of atmost 4 successors will be
   *   represented by a term in which the successor nodes have fixed
   *   positions
   *
   * o nodes with more successors or a variable # of succ. will be
   *   represented by a term that contains a list of successor nodes.
   */

  PrologTerm* t;

#if HAVE_SATIRE_ICFG
  if (cfg == NULL)
    cfg = get_global_cfg();
  assert(cfg != NULL);
  if (program == NULL)
    program = cfg->program;
  assert(program != NULL);
#endif

  /* See if this node is intended to be unparsed -> decls inserted by EDG will be stripped */
  Sg_File_Info* fi = astNode->get_file_info();
  if (fi == NULL) {
    fi = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
    if (isSgLocatedNode(astNode)) {
      std::cerr << "** WARNING: FileInfo for Node " << astNode->class_name()  
        << " \"" << astNode->unparseToString() 
        << "\" was not set." << std::endl;
    }
  }

  if (fi->isCompilerGenerated() && isSgBasicBlock(astNode)) {
    /* Adrian 2009/10/27:
       ROSE insists on wrapping loop bodies into a second SgBasicBlock now.
       We don't need that */
 // GB (2009-11-05): We can't assert his here because it fails sometimes.
 // (Don't know when.) If the condition is true, take the way out;
 // otherwise, continue and maybe risk duplicated basic blocks.
 // assert(synList.size() == 1);
    if (synList.size() == 1) {
      t = synList.at(0);
      return t;
    }
  }

  if (!fi->isFrontendSpecific()) {
    /* add node specific information to the term*/
    PrologTerm* specific = termConv.getSpecific(astNode);
 
    /* analysis results */
    PrologTerm* ar = NULL;

    /* add analysis information to the term*/
    if (SgStatement* n = isSgStatement(astNode)) {
      /* analysis result */
      PrologList *results = getAnalysisResultList(n);

      /* function IDs, if appropriate */
#if HAVE_SATIRE_ICFG
      std::string funcname = "";
      SgFunctionDeclaration *d = isSgFunctionDeclaration(astNode);
      if (d != NULL)
        funcname = d->get_name().str();
      if (funcname != "" && cfg != NULL) {
        SgNode *p = d->get_parent();
        while (p != NULL && !isSgFile(p))
          p = p->get_parent();
        results->addFirstElement(functionIdAnnotation(funcname, isSgFile(p)));
        PrologTerm* entryExit
          = functionEntryExitAnnotation(funcname, isSgFile(p));
        if (entryExit != NULL) {
          results->addFirstElement(entryExit);
        }
      }
#endif

      ar = new PrologCompTerm("analysis_info", /*1,*/ results);
    } else {
      /* default: empty analysis result */
      PrologList *results = new PrologList();

      /* variable IDs, if appropriate */
#if HAVE_SATIRE_ICFG
      SgVariableSymbol *sym = NULL;
      if (SgVarRefExp *v = isSgVarRefExp(astNode))
        sym = program->get_symbol(v);
      if (SgInitializedName *in = isSgInitializedName(astNode)) {
        sym = program->get_symbol(in);
        if (sym == NULL) {
          /* ROSE has NULL symbols for some unused things; for example,
           * argument names in forward function declarations. But also for
           * global variables that are declared more than once (which is
           * totally allowed). Look up this variable in the special little
           * table for global variable IDs; if it's not there, we invent a
           * number for these and hope that nothing breaks. */
          /* GB (2009-11-11): Since we now use SATIrE's own global symbol
           * table, the branch below is probably dead code. */
          PrologCompTerm *varid_annot = NULL;
          SgVariableDeclaration *d = isSgVariableDeclaration(in->get_parent());
          if (d != NULL && isSgGlobal(d->get_parent())
           && !d->get_declarationModifier().get_storageModifier().isStatic()) {
            /* If there is no symbol, ROSE does not give a variable name
             * either. But we can hack one out of the mangled name, where
             * the variable name comes right after the substring
             * "variable_name_". */
            std::string mname = d->get_mangled_name().str();
            const char *key = "variable_name_";
            std::string::size_type pos = mname.find(key);
            if (pos != std::string::npos && cfg != NULL) {
              std::string varname = mname.substr(pos + strlen(key));
              std::map<std::string, unsigned long>::iterator idi;
              idi = cfg->globalvarnames_ids.find(varname);
              if (idi != cfg->globalvarnames_ids.end()) {
                varid_annot = new PrologCompTerm("variable_id", //1,
                                                 new PrologInt(idi->second));
              }
            }
          }
          if (varid_annot == NULL) {
            varid_annot = new PrologCompTerm("variable_id", //1,
                                             new PrologInt(INT_MAX));
          }
          results->addFirstElement(varid_annot);
        }
      }
      if (sym != NULL) {
        if (cfg != NULL && !cfg->varsyms_ids.empty()) {
          PrologCompTerm *varid_annot = new PrologCompTerm("variable_id", //1,
                                                           varidTerm(sym));
          results->addFirstElement(varid_annot);
        }
      }
#endif

      /* function IDs, if appropriate */
#if HAVE_SATIRE_ICFG
      std::string funcname = "";
      SgFunctionRefExp *f = isSgFunctionRefExp(astNode);
      if (f != NULL)
        funcname = f->get_symbol()->get_name().str();
      if (funcname != "" && cfg != NULL) {
        SgNode *p = f->get_parent();
        while (p != NULL && !isSgFile(p))
          p = p->get_parent();
        results->addFirstElement(functionIdAnnotation(funcname, isSgFile(p)));
      }
#endif

      /* function call sites, if appropriate */
#if HAVE_SATIRE_ICFG
      if (SgFunctionCallExp *fc = isSgFunctionCallExp(astNode)) {
        SgExpression *function = fc->get_function();
        if (cfg != NULL) {
          CallSiteAttribute *csa = (CallSiteAttribute *)
                fc->getAttribute("SATIrE ICFG call block");
          PrologInt *callsite = new PrologInt(csa->bb->id);
          PrologCompTerm *callsite_annot = new PrologCompTerm("call_site", //1,
                                                              callsite);
          results->addFirstElement(callsite_annot);
          /* add information on possible call targets */
          if (!isSgFunctionRefExp(function)) {
            using SATIrE::Analyses::PointsToAnalysis;
            PointsToAnalysis *cspta = cfg->contextSensitivePointsToAnalysis;
            PrologList *callsite_locs = NULL;
#if HAVE_PAG
            /* output context-sensitive callsite-target location mapping;
             * but only for the contexts that are actually relevant for the
             * calling function */
            SgNode *p = fc->get_parent();
            while (p != NULL && !isSgFunctionDeclaration(p))
              p = p->get_parent();
            assert(isSgFunctionDeclaration(p));
            std::string funcname =
              isSgFunctionDeclaration(p)->get_name().str();
            while (p != NULL && !isSgFile(p))
              p = p->get_parent();
            assert(isSgFile(p));
            Procedure *proc = procedureNode(funcname, isSgFile(p));
            if (cspta != NULL) {
              const std::vector<ContextInformation::Context> &ctxs
                = cfg->contextInformation->allContexts();
              std::vector<ContextInformation::Context>::const_iterator ctx;
              for (ctx = ctxs.begin(); ctx != ctxs.end(); ++ctx) {
                if (ctx->procnum != proc->procnum)
                  continue;
                PointsToAnalysis::Location *loc =
                  cspta->expressionLocation(function, *ctx);
                PrologInt *pLocation =
                  new PrologInt(cspta->location_id(cspta->base_location(loc)));
                PrologCompTerm *ccl =
                  new PrologCompTerm("context_location", //2,
                                     ctx->toPrologTerm(),
                                     pLocation);
                if (callsite_locs == NULL)
                  callsite_locs = new PrologList();
                callsite_locs->addFirstElement(ccl);
              }
              PointsToAnalysis::Location *loc =
                cspta->expressionLocation(function);
              PrologInt *pLocation =
                new PrologInt(cspta->location_id(cspta->base_location(loc)));
              if (callsite_locs == NULL)
                callsite_locs = new PrologList();
              callsite_locs->addFirstElement(pLocation);
            }
#else
            PointsToAnalysis *pto = cfg->pointsToAnalysis;
            if (pto != NULL) {
              PointsToAnalysis::Location *loc =
                pto->expressionLocation(function);
              PrologInt *pLocation =
                new PrologInt(pto->location_id(pto->base_location(loc)));
              if (callsite_locs == NULL)
                callsite_locs = new PrologList();
              callsite_locs->addFirstElement(pLocation);
            }
#endif
            if (callsite_locs != NULL) {
              PrologCompTerm *callsite_locations =
		new PrologCompTerm("callsite_locations", //2,
                                     callsite,
                                     callsite_locs);
              results->addFirstElement(callsite_locations);
            }
          }
        }
        if (function->attributeExists(ASL_ATTRIBUTE_ID)) {
          ASLAttribute *attribute =
            (ASLAttribute *) function->getAttribute(ASL_ATTRIBUTE_ID);
          std::string str = attribute->toString();
          PrologCompTerm *asl_annot = new PrologCompTerm("asl_annot", //1,
                                                         new PrologAtom(str));
          results->addFirstElement(asl_annot);
        }
      }
#endif

      /* call strings, if appropriate */
#if HAVE_SATIRE_ICFG && HAVE_PAG
      if (isSgProject(astNode)) {
        if (cfg != NULL && cfg->contextInformation != NULL) {
          PrologTerm *callStrings = cfg->contextInformation->toPrologTerm();
          PrologCompTerm *callStringInfo
            = new PrologCompTerm("callstringinfo", /*1,*/ callStrings);
          results->addFirstElement(callStringInfo);
        }
      }
#endif

      /* points-to information, if appropriate */
#if HAVE_SATIRE_ICFG && HAVE_PAG
      if (isSgProject(astNode) && cfg != NULL
          && cfg->contextSensitivePointsToAnalysis != NULL) {
        using SATIrE::Analyses::PointsToAnalysis;
        // PointsToAnalysis *merged_pto = cfg->pointsToAnalysis;
        PointsToAnalysis *pto = cfg->contextSensitivePointsToAnalysis;

        /* mapping: locations to their contents; this automagically defines
         * the set of all locations */
        PrologList *locations = new PrologList();
        std::vector<PointsToAnalysis::Location *> locs;
        pto->interesting_locations(locs);
        std::vector<PointsToAnalysis::Location *>::const_iterator loc;
        for (loc = locs.begin(); loc != locs.end(); ++loc) {
          PrologList *varids = new PrologList();
          PrologList *funcs = new PrologList();
          const std::list<SgSymbol *> &syms = pto->location_symbols(*loc);
          std::list<SgSymbol *>::const_iterator s;
          for (s = syms.begin(); s != syms.end(); ++s) {
            if (SgVariableSymbol *varsym = isSgVariableSymbol(*s)) {
              varids->addFirstElement(varidTerm(varsym));
            } else if (SgFunctionSymbol *funsym = isSgFunctionSymbol(*s)) {
              std::string funcname = funsym->get_name().str();
              Sg_File_Info* fi = funsym->get_declaration()->get_file_info();
              assert(fi != NULL);
              SgNode* p = funsym->get_declaration();
              while (p != NULL && !isSgFile(p))
                p = p->get_parent();
              SgFile* file = isSgFile(p);
              assert(file != NULL);
              Procedure* proc = procedureNode(funcname, file);
           // If a Procedure node for this function exists, i.e., we have a
           // definition for it, then mangle its function number into its
           // name for unique display.
              if (proc != NULL) {
                std::stringstream name;
                name << funcname << "::" << proc->procnum;
                funcname = name.str();
              }
              funcs->addFirstElement(new PrologAtom(funcname));
            } else { // {{{ error handling
              std::cerr
                << "* unexpected symbol type in points-to location: "
                << (*s)->class_name()
                << " <" << (*s)->get_name().str() << ">"
                << std::endl;
              std::abort(); // }}}
            }
          }
          PrologCompTerm *loct
            = new PrologCompTerm("location_varids_funcs", //3,
                                 new PrologInt(pto->location_id(*loc)),
                                 varids,
                                 funcs);
          locations->addFirstElement(loct);
        }
        PrologCompTerm *locationInfo = new PrologCompTerm("locations", //1,
                                                          locations);
        results->addFirstElement(locationInfo);

        /* mapping: variables to locations in each context */
        PrologList *vlocs = new PrologList();
        std::map<SgVariableSymbol *, unsigned long>::const_iterator v;
        for (v = cfg->varsyms_ids.begin(); v != cfg->varsyms_ids.end(); ++v) {
          const std::vector<ContextInformation::Context> &ctxs
            = cfg->contextInformation->allContexts();
          std::vector<ContextInformation::Context>::const_iterator ctx;
          for (ctx = ctxs.begin(); ctx != ctxs.end(); ++ctx) {
            if (pto->symbol_has_location(v->first, *ctx)) {
              PrologInt *pLocation = new PrologInt(
                  pto->location_id(pto->symbol_location(v->first, *ctx)));
              PrologCompTerm *vcl
                = new PrologCompTerm("varid_context_location", //3,
                                     varidTerm(v->first),
                                     ctx->toPrologTerm(),
                                     pLocation);
              vlocs->addFirstElement(vcl);
            }
          }
        }
        PrologCompTerm *variable_locations
          = new PrologCompTerm("variable_locations", /*1,*/ vlocs);
        results->addFirstElement(variable_locations);

        /* mapping (or graph...): points-to relationships */
        PrologList *points_tos = new PrologList();
        for (loc = locs.begin(); loc != locs.end(); ++loc) {
          PointsToAnalysis::Location *base = pto->base_location(*loc);
          if (pto->valid_location(base)) {
            PrologCompTerm *points_to
              = new PrologCompTerm("->", //2,
                                   new PrologInt(pto->location_id(*loc)),
                                   new PrologInt(pto->location_id(base)));
            points_tos->addFirstElement(points_to);
          }
        }
        PrologCompTerm *points_to_relations
          = new PrologCompTerm("points_to_relations", /*1,*/ points_tos);
        results->addFirstElement(points_to_relations);

        /* mapping: function nodes to return and argument locations */

        /* mapping: structure locations to named members */
        // "structlocation_member_location" terms
      }
#elif HAVE_SATIRE_ICFG && !HAVE_PAG
      /* context-insensitive points-to information */
      if (isSgProject(astNode) && cfg != NULL
          && cfg->pointsToAnalysis != NULL) {
        using SATIrE::Analyses::PointsToAnalysis;
        PointsToAnalysis *pto = cfg->pointsToAnalysis;

        /* mapping: locations to their contents; this automagically defines
         * the set of all locations */
        PrologList *locations = new PrologList();
        std::vector<PointsToAnalysis::Location *> locs;
        pto->interesting_locations(locs);
        std::vector<PointsToAnalysis::Location *>::const_iterator loc;
        for (loc = locs.begin(); loc != locs.end(); ++loc) {
          PrologList *varids = new PrologList();
          PrologList *funcs = new PrologList();
          const std::list<SgSymbol *> &syms = pto->location_symbols(*loc);
          std::list<SgSymbol *>::const_iterator s;
          for (s = syms.begin(); s != syms.end(); ++s) {
            if (SgVariableSymbol *varsym = isSgVariableSymbol(*s)) {
              varids->addFirstElement(varidTerm(varsym));
            } else if (SgFunctionSymbol *funsym = isSgFunctionSymbol(*s)) {
              std::string funcname = funsym->get_name().str();
              Sg_File_Info* fi = funsym->get_declaration()->get_file_info();
              assert(fi != NULL);
              SgNode* p = funsym->get_declaration();
              while (p != NULL && !isSgFile(p))
                p = p->get_parent();
              SgFile* file = isSgFile(p);
              assert(file != NULL);
              Procedure* proc = procedureNode(funcname, file);
           // If a Procedure node for this function exists, i.e., we have a
           // definition for it, then mangle its function number into its
           // name for unique display.
              if (proc != NULL) {
                std::stringstream name;
                name << funcname << "::" << proc->procnum;
                funcname = name.str();
              }
              funcs->addFirstElement(new PrologAtom(funcname));
            } else { // {{{ error handling
              std::cerr
                << "* unexpected symbol type in points-to location: "
                << (*s)->class_name()
                << " <" << (*s)->get_name().str() << ">"
                << std::endl;
              std::abort(); // }}}
            }
          }
          PrologCompTerm *loct
            = new PrologCompTerm("location_varids_funcs", // 3,
                                 new PrologInt(pto->location_id(*loc)),
                                 varids,
                                 funcs);
          locations->addFirstElement(loct);
        }
        PrologCompTerm *locationInfo = new PrologCompTerm("locations", //1,
                                                          locations);
        results->addFirstElement(locationInfo);

        /* mapping: variables to locations in each context */
        PrologList *vlocs = new PrologList();
        std::map<SgVariableSymbol *, unsigned long>::const_iterator v;
        for (v = cfg->varsyms_ids.begin(); v != cfg->varsyms_ids.end(); ++v) {
          if (pto->symbol_has_location(v->first)) {
            PrologInt *pLocation = new PrologInt(
                pto->location_id(pto->symbol_location(v->first)));
            PrologCompTerm *vcl
              = new PrologCompTerm("varid_location", //2,
                                   varidTerm(v->first),
                                   pLocation);
            vlocs->addFirstElement(vcl);
          }
        }
        PrologCompTerm *variable_locations
          = new PrologCompTerm("variable_locations", /*1,*/ vlocs);
        results->addFirstElement(variable_locations);

        /* mapping (or graph...): points-to relationships */
        PrologList *points_tos = new PrologList();
        for (loc = locs.begin(); loc != locs.end(); ++loc) {
          PointsToAnalysis::Location *base = pto->base_location(*loc);
          if (pto->valid_location(base)) {
            PrologCompTerm *points_to
              = new PrologCompTerm("->", //2,
                                   new PrologInt(pto->location_id(*loc)),
                                   new PrologInt(pto->location_id(base)));
            points_tos->addFirstElement(points_to);
          }
        }
        PrologCompTerm *points_to_relations
	  = new PrologCompTerm("points_to_relations", /*1,*/ points_tos);
        results->addFirstElement(points_to_relations);

        /* mapping: function nodes to return and argument locations */

        /* mapping: structure locations to named members */
        // "structlocation_member_location" terms
      }
#endif

      ar = new PrologCompTerm("analysis_info", /*1,*/ results);
    }

    /* add file info term */
    PrologTerm* fiTerm = termConv.getFileInfo(fi);

    assert(specific != NULL);
    assert(ar != NULL);
    assert(fiTerm != NULL);

    /* depending on the number of successors, use different predicate names*/
    if(isContainer(astNode))
      t = listTerm(astNode, synList, specific, ar, fiTerm);
    else {
      switch (getArity(astNode)) {
      case 0:
        t = leafTerm(astNode, synList, specific, ar, fiTerm);
        break;
      case 1:
        t = unaryTerm(astNode, synList, specific, ar, fiTerm);
        break;
      case 2:
        t = binaryTerm(astNode, synList, specific, ar, fiTerm);
        break;
      case 3:
        t = ternaryTerm(astNode, synList, specific, ar, fiTerm);
        break;
      case 4:
        t = quaternaryTerm(astNode, synList, specific, ar, fiTerm);
        break;
      default:
        t = listTerm(astNode, synList, specific, ar, fiTerm);
        break;
      }
    }
  }
  else {
    t = new PrologAtom("null");
  }

  /* remember the last term */
  rootTerm = t;
  return t;
}

/* Add analysis result */
template<typename DFI_STORE_TYPE>
PrologList*
TermPrinter<DFI_STORE_TYPE>::getAnalysisResultList(SgStatement* stmt)
{
  PrologList *infos;
  infos = new PrologList();

#if HAVE_PAG && !defined(DO_NOT_USE_DFIPRINTER)
  if (withPagAnalysisResults && stmt->get_attributeMechanism()) {
    PrologTerm *preInfo, *postInfo;
     preInfo = pagToProlog("pre_info",  analysisname,
                           pagDfiTextPrinter.getPreInfo(stmt));
    postInfo = pagToProlog("post_info", analysisname,
                           pagDfiTextPrinter.getPostInfo(stmt));
    infos->addFirstElement(postInfo);
    infos->addFirstElement(preInfo);
  }
#endif
#if HAVE_SATIRE_ICFG
  if (cfg != NULL && cfg->statementHasLabels(stmt)) {
    std::pair<int, int> entryExit = cfg->statementEntryExitLabels(stmt);
    PrologCompTerm *pair = new PrologInfixOperator("-");
    pair->addSubterm(new PrologInt(entryExit.first));
    pair->addSubterm(new PrologInt(entryExit.second));
    PrologCompTerm *ee;
    ee = new PrologCompTerm("entry_exit_labels");
    ee->addSubterm(pair);
    infos->addFirstElement(ee);
    /* ICFG node labels identifying branch headers */
    if (stmt->attributeExists("PAG statement head")) {
      StatementAttribute *a =
        (StatementAttribute *) stmt->getAttribute("PAG statement head");
      PrologInt *l = new PrologInt(a->get_bb()->id);
      PrologCompTerm *head = new PrologCompTerm("branch_head_label");
      head->addSubterm(l);
      infos->addFirstElement(head);
    }
  }
#endif

  return infos;
}

/* Convert the PAG analysis result into a Prolog Term */
extern const char* dfi_input;
extern const char* dfi_input_start;
extern const char* dfi_name;
extern const char* dfi_analysisname;
extern int dfiparse (void);
extern void dfirestart(FILE*);
extern PrologCompTerm* dfiterm;

template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::pagToProlog(
        std::string name, std::string analysis, std::string dfi) {
  // Initialize and call the parser
  dfi_name = name.c_str();
  dfi_analysisname = analysis.c_str();
  dfi_input = dfi_input_start = dfi.c_str();
  dfirestart(0);
  dfiparse();
  return dfiterm;
}

/* Create a prolog term representing a leaf node.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm* TermPrinter<DFI_STORE_TYPE>::leafTerm(
  SgNode* astNode, SynthesizedAttributesList synList,
  PrologTerm* specific, PrologTerm* ar, PrologTerm* fiTerm)
{
  PrologCompTerm* t =
    new PrologCompTerm(termConv.prologize(astNode->class_name()), //0+3,
                       specific, ar, fiTerm);
  return t;
}

/* Create a prolog term representing a unary operator.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm* TermPrinter<DFI_STORE_TYPE>::unaryTerm(
  SgNode* astNode, SynthesizedAttributesList synList,
  PrologTerm* specific, PrologTerm* ar, PrologTerm* fiTerm)
{
  PrologCompTerm* t =
    new PrologCompTerm(termConv.prologize(astNode->class_name()), //1+3,
                       synList.at(0),
                       specific, ar, fiTerm);
  return t;
}

/* Create a prolog term representing a binary operator.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm* TermPrinter<DFI_STORE_TYPE>::binaryTerm(
  SgNode* astNode, SynthesizedAttributesList synList,
  PrologTerm* specific, PrologTerm* ar, PrologTerm* fiTerm)
{
  PrologCompTerm* t =
    new PrologCompTerm(termConv.prologize(astNode->class_name()), //2+3,
                       synList.at(0),
                       synList.at(1),
                       specific, ar, fiTerm);
  return t;
}

/* Create a prolog term representing a ternary operator.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm* TermPrinter<DFI_STORE_TYPE>::ternaryTerm(
  SgNode* astNode, SynthesizedAttributesList synList,
  PrologTerm* specific, PrologTerm* ar, PrologTerm* fiTerm)
{
  PrologCompTerm* t =
    new PrologCompTerm(termConv.prologize(astNode->class_name()), //3+3,
                       synList.at(0),
                       synList.at(1),
                       synList.at(2),
                       specific, ar, fiTerm);
  return t;
}

/* Create a prolog term representing a quaternary operator.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm* TermPrinter<DFI_STORE_TYPE>::quaternaryTerm(
  SgNode* astNode, SynthesizedAttributesList synList,
  PrologTerm* specific, PrologTerm* ar, PrologTerm* fiTerm)
{
  PrologCompTerm* t =
    new PrologCompTerm(termConv.prologize(astNode->class_name()), //4+3,
                       synList.at(0),
                       synList.at(1),
                       synList.at(2),
                       synList.at(3),
                       specific, ar, fiTerm);
  return t;
}

/* Create a prolog term representing a node with more than four successors.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm* TermPrinter<DFI_STORE_TYPE>::listTerm(
  SgNode* astNode, SynthesizedAttributesList synList,
  PrologTerm* specific, PrologTerm* ar, PrologTerm* fiTerm)
{
  /* add children's subterms to list */
  PrologList* l = new PrologList();
  SynthesizedAttributesList::reverse_iterator it;
  it = synList.rbegin();
  SynthesizedAttributesList::reverse_iterator end;
  end = synList.rend();

  /* Special case for variable declarations: The first traversal successor
   * may be a type declaration or definition. In the ROSE AST, it is an
   * argument by itself, not one of the initialized names in the list. This
   * is the only node type that is a mixture of "list node" and "fixed-arity
   * node". In the Termite term, we will add this subterm in the variable
   * declaration's annotation term (variable_declaration_specific). */
  if (isSgVariableDeclaration(astNode)) {
    /* skip the first element in the loop below */
    --end;
  }

  while(it != end) {
    /* strip frontend-specific "null" Atoms (see above) */
    PrologAtom* atom = dynamic_cast<PrologAtom*>(*it);
    if (!(atom && (atom->getName() == "null")))
      l->addFirstElement(*it);
    it++;
  }
  /* add list to term*/
  PrologCompTerm* t =
    new PrologCompTerm(termConv.prologize(astNode->class_name()), //1+3,
                      l,
                      specific, ar, fiTerm);
  return t;
}

#if HAVE_SATIRE_ICFG
template<typename DFI_STORE_TYPE>
PrologTerm*
TermPrinter<DFI_STORE_TYPE>::varidTerm(SgVariableSymbol *sym)
{
  PrologTerm *result = NULL;
  std::map<SgVariableSymbol *, unsigned long>::iterator s;
  if (cfg != NULL
      && (s = cfg->varsyms_ids.find(sym)) != cfg->varsyms_ids.end()) {
    unsigned long id = s->second;
    result = new PrologInt(id);
  } else {
    result = new PrologAtom("null");
  }
  return result;
}

template<typename DFI_STORE_TYPE>
Procedure*
TermPrinter<DFI_STORE_TYPE>::procedureNode(std::string funcname,
                                           SgFile *file) {
  std::multimap<std::string, Procedure *>::iterator mmi, limit;
  mmi = cfg->proc_map.lower_bound(funcname);
  if (mmi != cfg->proc_map.end()) {
    /* If we got here, we found *some* functions with the correct name in
     * the procedure map. To see which one we really want, we prefer a
     * static function in the same file, if there is one; otherwise, a
     * non-static implementation. */
    Procedure *staticCandidate = NULL;
    Procedure *nonStaticCandidate = NULL;
    limit = cfg->proc_map.upper_bound(funcname);
    while (mmi != limit) {
      Procedure *p = mmi++->second;
      if (p->isStatic && p->containingFile == file) {
        staticCandidate = p;
        break;
      } else if (!p->isStatic) {
        nonStaticCandidate = p;
      }
    }
    if (staticCandidate != NULL)
      return staticCandidate;
    else if (nonStaticCandidate != NULL)
      return nonStaticCandidate;
  }
  return NULL;
}

template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::functionIdAnnotation(std::string funcname,
                                                  SgFile *file) {
  Procedure* p = procedureNode(funcname, file);
  PrologInt* funcid_value = NULL;
  if (p != NULL) {
    funcid_value = new PrologInt(p->procnum);
  } else {
    funcid_value = new PrologInt(INT_MAX);
  }
  PrologCompTerm *funcid_annot
    = new PrologCompTerm("function_id", /*1,*/ funcid_value);
  return funcid_annot;
}

template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::functionEntryExitAnnotation(
    std::string funcname, SgFile *file) {
  Procedure* p = procedureNode(funcname, file);
  PrologCompTerm* entryExit = NULL;
  if (p != NULL) {
    PrologCompTerm* pair = new PrologCompTerm("-", //2,
                                   new PrologInt(p->entry->id),
                                   new PrologInt(p->exit->id));
    entryExit = new PrologCompTerm("function_entry_exit", /*1,*/ pair);
  }
  return entryExit;
}
#endif

#endif
