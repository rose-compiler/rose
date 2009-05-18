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
#include <satire_rose.h>
#include "termite.h"

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
#if HAVE_SATIRE_ICFG
            , CFG *cfg = 0
#endif
            )
#if HAVE_PAG || HAVE_SATIRE_ICFG
      :
#endif
#if HAVE_PAG
      pagDfiTextPrinter(analysis_info)
#endif
#if HAVE_PAG && HAVE_SATIRE_ICFG
      ,
#endif
#if HAVE_SATIRE_ICFG
      cfg(cfg)
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
  RoseToProlog termConv;

#if HAVE_SATIRE_ICFG
  /** the CFG */
  CFG *cfg;
#endif

  PrologCompTerm* getAnalysisResult(SgStatement* stmt);  
  PrologCompTerm* pagToProlog(std::string name, std::string dfi);
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
      ((PrologCompTerm*)t)->addSubterm(getAnalysisResult(n));
    } else {
      /* default: empty analysis result */
      PrologCompTerm* ar = new PrologCompTerm("analysis_info");
      PrologList *results = new PrologList();

      /* variable IDs, if appropriate */
#if HAVE_SATIRE_ICFG
      SgVariableSymbol *sym = NULL;
      if (SgVarRefExp *v = isSgVarRefExp(astNode))
        sym = v->get_symbol();
      if (SgInitializedName *in = isSgInitializedName(astNode))
        sym = isSgVariableSymbol(in->get_symbol_from_symbol_table());
      if (sym != NULL) {
        CFG *icfg = get_global_cfg();
        if (icfg != NULL && !icfg->varsyms_ids.empty()) {
          PrologCompTerm *varid_annot = new PrologCompTerm("variable_id");
          std::map<SgVariableSymbol *, unsigned long>::iterator s;
          s = icfg->varsyms_ids.find(sym);
          if (s != icfg->varsyms_ids.end()) {
            unsigned long id = s->second;
            varid_annot->addSubterm(new PrologInt(id));
          } else {
            varid_annot->addSubterm(new PrologAtom("null"));
          }
          results->addFirstElement(varid_annot);
        }
      }
#endif

      /* function call sites, if appropriate */
#if HAVE_SATIRE_ICFG
      if (SgFunctionCallExp *fc = isSgFunctionCallExp(astNode)) {
        CFG *icfg = get_global_cfg();
        if (icfg != NULL) {
          CallSiteAttribute *csa = (CallSiteAttribute *)
                fc->getAttribute("SATIrE ICFG call block");
          PrologCompTerm *callsite_annot = new PrologCompTerm("call_site");
          callsite_annot->addSubterm(new PrologInt(csa->bb->id));
          results->addFirstElement(callsite_annot);
        }
      }
#endif

      /* call strings, if appropriate */
#if HAVE_SATIRE_ICFG && HAVE_PAG
      if (isSgProject(astNode)) {
        CFG *icfg = get_global_cfg();
        if (icfg != NULL && icfg->contextInformation != NULL) {
          PrologTerm *callStrings = icfg->contextInformation->toPrologTerm();
          PrologCompTerm *callStringInfo
              = new PrologCompTerm("callstringinfo");
          callStringInfo->addSubterm(callStrings);
          results->addFirstElement(callStringInfo);
        }
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
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::getAnalysisResult(SgStatement* stmt)
{
  PrologCompTerm *ar;
  ar   = new PrologCompTerm("analysis_info");
  PrologList *infos;
  infos = new PrologList();

#if HAVE_PAG
  if (withPagAnalysisResults && stmt->get_attributeMechanism()) {
    PrologTerm *preInfo, *postInfo;
     preInfo = pagToProlog("pre_info", pagDfiTextPrinter.getPreInfo(stmt));
    postInfo = pagToProlog("post_info",pagDfiTextPrinter.getPostInfo(stmt));
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
  ar->addSubterm(infos);

  return ar;
}

/* Convert the PAG analysis result into a Prolog Term */
extern const char* dfi_input;
extern const char* dfi_input_start;
extern const char* dfi_name;
extern int dfiparse (void);
extern void dfirestart(FILE*);
extern PrologCompTerm* dfiterm;

template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::pagToProlog(std::string name, std::string dfi) {
  // Initialize and call the parser
  dfi_name = name.c_str();
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
  while(it !=synList.rend()) {
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

#endif
