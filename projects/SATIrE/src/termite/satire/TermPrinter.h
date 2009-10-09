/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2007-2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
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
#endif

#if HAVE_PAG
#  include <config.h>
// GB (2008-10-03): The declaration (and definition) of PagDfiTextPrinter is
// now provided in a special header.
#  include <PagDfiTextPrinter.h>
#endif

#include "RoseToTerm.h"
#include <sstream>
#include <iostream>


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
#endif
            )
      :
#if HAVE_PAG
      pagDfiTextPrinter(analysis_info),
#endif
      analysisname(analysisname_ != "" ? analysisname_ : "unknown")
#if HAVE_SATIRE_ICFG
    , cfg(cfg)
#else
    , cfg(NULL)
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
#if HAVE_PAG
    PagDfiTextPrinter<DFI_STORE_TYPE> pagDfiTextPrinter;
#endif

private:
  /** should generate list instead of tuple? */
  bool isContainer(SgNode* astNode);
  /** return the number of successors */
  int getArity(SgNode* astNode);

  /** create leaf nodes*/
  PrologCompTerm* leafTerm(SgNode* astNode, SynthesizedAttributesList synList);
  /** create unary nodes*/
  PrologCompTerm* unaryTerm(SgNode* astNode, SynthesizedAttributesList synList);
  /** create binary nodes*/
  PrologCompTerm* binaryTerm(SgNode* astNode, SynthesizedAttributesList synList);
  /** create ternary nodes*/
  PrologCompTerm* ternaryTerm(SgNode* astNode, SynthesizedAttributesList synList);
  /** create quaternary nodes*/
  PrologCompTerm* quaternaryTerm(SgNode* astNode, SynthesizedAttributesList synList);
  /** create list nodes*/
  PrologCompTerm* listTerm(SgNode* astNode, SynthesizedAttributesList synList);

  /** the current term */
  PrologTerm* rootTerm;

  /** the converter */
  RoseToTerm termConv;

  /** the name of the analysis, if available */
  std::string analysisname;

#if HAVE_SATIRE_ICFG
  /** the CFG */
  CFG *cfg;

  /** create integer term with variable ID or "null" atom */
  PrologTerm* varidTerm(SgVariableSymbol *symbol);
#else
  /** dummy member */
  void *cfg;
#endif

  PrologList* getAnalysisResultList(SgStatement* stmt);
  PrologCompTerm* pagToProlog(std::string name, std::string analysis,
                              std::string dfi);
  PrologCompTerm* functionIdAnnotation(std::string funcname);
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
PrologTerm* 
TermPrinter<DFI_STORE_TYPE>::evaluateSynthesizedAttribute(SgNode* astNode, SynthesizedAttributesList synList) {
  /*
   * Compute the PROLOG representation of a node using
   * the successors' synthesized attributes.
   * o nodes with a fixed number of atmost 4 successors will be represented by a term in which
   *   the successor nodes have fixed positions
   * o nodes with more successors or a variable # of succ. will be represented by a term that contains
   *   a list of successor nodes.
   */

  PrologTerm* t;

#if HAVE_SATIRE_ICFG
  if (cfg == NULL)
      cfg = get_global_cfg();
#endif

  /* See if this node is intended to be unparsed -> decls inserted by EDG will be stripped */
  Sg_File_Info* fi = astNode->get_file_info();
  if (fi == NULL) {
    fi = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
    if (isSgLocatedNode(astNode)) {
      std::cerr << "** WARNING: FileInfo for Node " << astNode->class_name()  
		<< " \"" << astNode->unparseToString() << "\" was not set." << std::endl;
    }
  }

  if (!fi->isFrontendSpecific()) {

    /* depending on the number of successors, use different predicate names*/
    if(isContainer(astNode))
      t = listTerm(astNode, synList);
    else {  
      switch (getArity(astNode)) {
      case 0:
	t = leafTerm(astNode, synList);
	break;
      case 1:
	t = unaryTerm(astNode, synList);
	break;
      case 2:
	t = binaryTerm(astNode, synList);
	break;
      case 3:
	t = ternaryTerm(astNode, synList);
	break;
      case 4:
	t = quaternaryTerm(astNode, synList);
	break;
      default:
	t = listTerm(astNode, synList);
	break;
      }
    }
    /* add node specific information to the term*/
    termConv.addSpecific(astNode, (PrologCompTerm*)t);

    /* add analysis information to the term*/
    if (SgStatement* n = isSgStatement(astNode)) {
      /* analysis result */
      PrologCompTerm* ar = new PrologCompTerm("analysis_info");
      PrologList *results = getAnalysisResultList(n);

      /* function IDs, if appropriate */
#if HAVE_SATIRE_ICFG
      std::string funcname = "";
      if (SgFunctionDeclaration *d = isSgFunctionDeclaration(astNode))
        funcname = d->get_name().str();
      if (funcname != "" && cfg != NULL) {
        results->addFirstElement(functionIdAnnotation(funcname));
      }
#endif

      ar->addSubterm(results);
      ((PrologCompTerm*)t)->addSubterm(ar);
    } else {
      /* default: empty analysis result */
      PrologCompTerm* ar = new PrologCompTerm("analysis_info");
      PrologList *results = new PrologList();

      /* variable IDs, if appropriate */
#if HAVE_SATIRE_ICFG
      SgVariableSymbol *sym = NULL;
      if (SgVarRefExp *v = isSgVarRefExp(astNode))
        sym = v->get_symbol();
      if (SgInitializedName *in = isSgInitializedName(astNode)) {
        sym = isSgVariableSymbol(in->get_symbol_from_symbol_table());
        if (sym == NULL) {
          /* ROSE has NULL symbols for some unused things; for example,
           * argument names in forward function declarations. We invent a
           * number for these and hope that nothing breaks. */
          PrologCompTerm *varid_annot = new PrologCompTerm("variable_id");
          varid_annot->addSubterm(new PrologInt(INT_MAX));
          results->addFirstElement(varid_annot);
        }
      }
      if (sym != NULL) {
        if (cfg != NULL && !cfg->varsyms_ids.empty()) {
          PrologCompTerm *varid_annot = new PrologCompTerm("variable_id");
          varid_annot->addSubterm(varidTerm(sym));
          results->addFirstElement(varid_annot);
        }
      }
#endif

      /* function IDs, if appropriate */
#if HAVE_SATIRE_ICFG
      std::string funcname = "";
      if (SgFunctionRefExp *f = isSgFunctionRefExp(astNode))
        funcname = f->get_symbol()->get_name().str();
      if (funcname != "" && cfg != NULL) {
        std::multimap<std::string, Procedure *>::iterator mmi;
        mmi = cfg->proc_map.lower_bound(funcname);
        ROSE_ASSERT(mmi != cfg->proc_map.end());
        PrologCompTerm *funcid_annot = new PrologCompTerm("function_id");
        funcid_annot->addSubterm(new PrologInt(mmi->second->procnum));
        results->addFirstElement(funcid_annot);
      }
#endif

      /* function call sites, if appropriate */
#if HAVE_SATIRE_ICFG
      if (SgFunctionCallExp *fc = isSgFunctionCallExp(astNode)) {
        if (cfg != NULL) {
          CallSiteAttribute *csa = (CallSiteAttribute *)
                fc->getAttribute("SATIrE ICFG call block");
          PrologCompTerm *callsite_annot = new PrologCompTerm("call_site");
          callsite_annot->addSubterm(new PrologInt(csa->bb->id));
          results->addFirstElement(callsite_annot);
          /* TODO: add information on possible call targets? */
        }
      }
#endif

      /* call strings, if appropriate */
#if HAVE_SATIRE_ICFG && HAVE_PAG
      if (isSgProject(astNode)) {
        if (cfg != NULL && cfg->contextInformation != NULL) {
          PrologTerm *callStrings = cfg->contextInformation->toPrologTerm();
          PrologCompTerm *callStringInfo
              = new PrologCompTerm("callstringinfo");
          callStringInfo->addSubterm(callStrings);
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
        PrologCompTerm *locationInfo = new PrologCompTerm("locations");
        PrologList *locations = new PrologList();
        std::vector<PointsToAnalysis::Location *> locs;
        pto->interesting_locations(locs);
        std::vector<PointsToAnalysis::Location *>::const_iterator loc;
        for (loc = locs.begin(); loc != locs.end(); ++loc) {
          PrologCompTerm *loct = new PrologCompTerm("location_varids_funcs");
          loct->addSubterm(new PrologInt(pto->location_id(*loc)));
          PrologList *varids = new PrologList();
          PrologList *funcs = new PrologList();
          const std::list<SgSymbol *> &syms = pto->location_symbols(*loc);
          std::list<SgSymbol *>::const_iterator s;
          for (s = syms.begin(); s != syms.end(); ++s) {
            if (SgVariableSymbol *varsym = isSgVariableSymbol(*s)) {
              varids->addFirstElement(varidTerm(varsym));
            } else if (SgFunctionSymbol *funsym = isSgFunctionSymbol(*s)) {
              funcs->addFirstElement(new PrologAtom(funsym->get_name().str()));
            } else { // {{{ error handling
              std::cerr
                << "* unexpected symbol type in points-to location: "
                << (*s)->class_name()
                << " <" << (*s)->get_name().str() << ">"
                << std::endl;
              std::abort(); // }}}
            }
          }
          loct->addSubterm(varids);
          loct->addSubterm(funcs);
          locations->addFirstElement(loct);
        }
        locationInfo->addSubterm(locations);
        results->addFirstElement(locationInfo);

        /* mapping: variables to locations in each context */
        PrologCompTerm *variable_locations
          = new PrologCompTerm("variable_locations");
        PrologList *vlocs = new PrologList();
        std::map<SgVariableSymbol *, unsigned long>::const_iterator v;
        for (v = cfg->varsyms_ids.begin(); v != cfg->varsyms_ids.end(); ++v) {
          const std::vector<ContextInformation::Context> &ctxs
            = cfg->contextInformation->allContexts();
          std::vector<ContextInformation::Context>::const_iterator ctx;
          for (ctx = ctxs.begin(); ctx != ctxs.end(); ++ctx) {
            if (pto->symbol_has_location(v->first, *ctx)) {
              PrologCompTerm *vcl
                = new PrologCompTerm("varid_context_location");
              vcl->addSubterm(varidTerm(v->first));
              vcl->addSubterm(ctx->toPrologTerm());
              vcl->addSubterm(new PrologInt(
                  pto->location_id(pto->symbol_location(v->first, *ctx))));
              vlocs->addFirstElement(vcl);
            }
          }
        }
        variable_locations->addSubterm(vlocs);
        results->addFirstElement(variable_locations);

        /* mapping (or graph...): points-to relationships */
        PrologCompTerm *points_to_relations
          = new PrologCompTerm("points_to_relations");
        PrologList *points_tos = new PrologList();
        for (loc = locs.begin(); loc != locs.end(); ++loc) {
          PointsToAnalysis::Location *base = pto->base_location(*loc);
          if (pto->valid_location(base)) {
            PrologCompTerm *points_to = new PrologCompTerm("->");
            points_to->addSubterm(new PrologInt(pto->location_id(*loc)));
            points_to->addSubterm(new PrologInt(pto->location_id(base)));
            points_tos->addFirstElement(points_to);
          }
        }
        points_to_relations->addSubterm(points_tos);
        results->addFirstElement(points_to_relations);

        /* mapping: function nodes to return and argument locations */

        /* mapping: structure locations to named members */
        // "structlocation_member_location" terms
      }
#endif

      ar->addSubterm(results);
      ((PrologCompTerm*)t)->addSubterm(ar);
    }

    /* add file info term */
    ((PrologCompTerm*)t)->addSubterm(termConv.getFileInfo(fi));
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

#if HAVE_PAG
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
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::leafTerm(SgNode* astNode, SynthesizedAttributesList synList) 
{
	PrologCompTerm* t = new PrologCompTerm(termConv.prologize(astNode->class_name()));
	return t;
}

/* Create a prolog term representing a unary operator.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::unaryTerm(SgNode* astNode, SynthesizedAttributesList synList) 
{
	PrologCompTerm* t = new PrologCompTerm(termConv.prologize(astNode->class_name()));
	/* add children's subterms*/
	t->addSubterm(synList.at(0));
	return t;
}

/* Create a prolog term representing a binary operator.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::binaryTerm(SgNode* astNode, SynthesizedAttributesList synList) 
{
	PrologCompTerm* t = new PrologCompTerm(termConv.prologize(astNode->class_name()));
	/* add children's subterms*/
	t->addSubterm(synList.at(0));
	t->addSubterm(synList.at(1));
	return t;
}

/* Create a prolog term representing a ternary operator.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::ternaryTerm(SgNode* astNode, SynthesizedAttributesList synList) 
{
	PrologCompTerm* t = new PrologCompTerm(termConv.prologize(astNode->class_name()));
	t->addSubterm(synList.at(0)); 
	t->addSubterm(synList.at(1));
	t->addSubterm(synList.at(2));
	return t;
}

/* Create a prolog term representing a quaternary operator.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::quaternaryTerm(SgNode* astNode, SynthesizedAttributesList synList) 
{
	PrologCompTerm* t = new PrologCompTerm(termConv.prologize(astNode->class_name()));
	t->addSubterm(synList.at(0));
	t->addSubterm(synList.at(1));
	t->addSubterm(synList.at(2));
	t->addSubterm(synList.at(3));
	return t;
}

/* Create a prolog term representing a node with more than four successors.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::listTerm(SgNode* astNode, SynthesizedAttributesList synList) 
{
  PrologCompTerm* t = new PrologCompTerm(termConv.prologize(astNode->class_name()));
 /* add children's subterms to list*/
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
    /* strip "null" Atoms */
    PrologAtom* atom = dynamic_cast<PrologAtom*>(*it);
    if (!(atom && (atom->getName() == "null")))
      l->addFirstElement(*it);
    it++;
  }
  /* add list to term*/
  t->addSubterm(l);
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
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::functionIdAnnotation(std::string funcname) {
  std::multimap<std::string, Procedure *>::iterator mmi;
  PrologCompTerm *funcid_annot = new PrologCompTerm("function_id");
  mmi = cfg->proc_map.lower_bound(funcname);
  if (mmi != cfg->proc_map.end())
    funcid_annot->addSubterm(new PrologInt(mmi->second->procnum));
  else
    funcid_annot->addSubterm(new PrologInt(INT_MAX));
  return funcid_annot;
}
#endif

#endif
