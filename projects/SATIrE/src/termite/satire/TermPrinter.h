/*  -*- C++ -*-
    Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
              2007 Adrian Prantl
    see LICENSE in the root folder of this project
*/

#ifndef PROLOGTRAVERSAL_H_
#define  PROLOGTRAVERSAL_H_
#include <iostream>
#include <string>
#include <time.h>
#include <rose_config.h>
#include "rose.h"
#include "PrologTerm.h"
#include "PrologCompTerm.h"
#include "PrologAtom.h"
#include "PrologString.h"
#include "PrologInt.h"
#include "PrologList.h"
#ifdef PAG_VERSION
#  include <config.h>
   template<typename DFI_STORE_TYPE> class PagDfiTextPrinter;
#endif

#define BasicTermPrinter TermPrinter<void*>

/* See main.C-template and toProlog.C for examples how to use this */
  
/**
 * Class implementing a traversal to create a PROLOG term 
 * representing a ROSE-IR
 *
 * Additionally, this template can be instatiated with the DFI_STORE type
 * in the case the Terms should contain PAG analysis results.
 * This feature is only enabled if PAG_VERSION is defined.
 */
template<typename DFI_STORE_TYPE>
class TermPrinter: public AstBottomUpProcessing<PrologTerm*> 
{
public:
  TermPrinter(DFI_STORE_TYPE analysis_result = 0) 
# ifdef PAG_VERSION
      :pagDfiTextPrinter(analysis_result)
# endif
  { withPagAnalysisResults = (analysis_result != 0); };

  /** return the term*/
  PrologTerm* getTerm() {return mTerm;};

protected:
  /** reimplemented from AstBottomUpProcessing*/
  virtual PrologTerm*
    evaluateSynthesizedAttribute(SgNode* astNode, SynthesizedAttributesList synList);
  /** reimplemented from AstBottomUpProcessing*/
  virtual PrologTerm*
    defaultSynthesizedAttribute() {return new PrologAtom("null");};

  bool withPagAnalysisResults;
# ifdef PAG_VERSION
    PagDfiTextPrinter<DFI_STORE_TYPE> pagDfiTextPrinter;
# endif

private:
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
  /** add the class name to the term, using PrologSupport::prologize*/
  void addClassname(SgNode* astNode,PrologCompTerm*);
  /** the current term */
  PrologTerm* mTerm;

  PrologCompTerm* getAnalysisResult(SgStatement* stmt);  
  PrologCompTerm* pagToProlog(std::string name, std::string dfi);
};


////////////////////////////////////////////////////////////////////////

/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
          2007 Adrian Prantl
see LICENSE in the root folder of this project
 *
 *
 * TermPrinter (Implementation)
 * Author: Christoph Bonitz
 * Purpose: Traverse AST to create a PROLOG term representing it.
 *
 * Uses:
 * - AstBottomUpProcessing (traversal infrastructure)
 * - PrologSupport for character manipulation (PrologSupport::prologize()),
 *   adding file info and term specific information
 * - PrologTerm* and subclasses for term representation
 */


/*includes*/
//#include "TermPrinter.h"
#include "PrologSupport.h"
#include <sstream>
#include <iostream>

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
    std::cerr << "** WARNING: FileInfo for Node " << astNode 
	      << " \"" << astNode->unparseToString() << "\" was not set." << std::endl;
  }

  if (!fi->isFrontendSpecific()) {

    /* depending on the number of successors, use different predicate names*/
    if(AstTests::numSuccContainers(astNode))
      t = listTerm(astNode, synList);
    else {  
      switch (AstTests::numSingleSuccs(astNode)) {
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
    PrologSupport::addSpecific(astNode, (PrologCompTerm*)t);
    PrologCompTerm* annot = (PrologCompTerm*)((PrologCompTerm*)t)->getSubTerms().back();
    assert(annot);

    /* add analysis information to the term*/
    if (SgStatement* n = isSgStatement(astNode)) {
      /* analysis result */
      annot->addSubterm(getAnalysisResult(n));
    } else {
      /* empty analysis result */
      PrologCompTerm* ar = new PrologCompTerm("analysis_result");
      ar->addSubterm(new PrologAtom("null"));
      annot->addSubterm(ar);
    }

    /* add file info term */
    ((PrologCompTerm*)t)->addSubterm(PrologSupport::getFileInfo(fi));
  }
  else {
    t = new PrologAtom("null");
  }



  /* remember the last term */
  mTerm = t;
  return t;
}

/* Add analysis result */
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::getAnalysisResult(SgStatement* stmt)
{
  PrologCompTerm *ar;
  PrologTerm *preInfo, *postInfo;
  ar   = new PrologCompTerm("analysis_result");

# ifdef PAG_VERSION
  if (withPagAnalysisResults && stmt->get_attributeMechanism()) {
     preInfo = pagToProlog("pre_info", pagDfiTextPrinter.getPreInfo(stmt));
    postInfo = pagToProlog("post_info",pagDfiTextPrinter.getPostInfo(stmt));
  } else 
# endif
  {
     preInfo = new PrologAtom("null");
    postInfo = new PrologAtom("null");
  }
  ar->addSubterm(preInfo);
  ar->addSubterm(postInfo);

  return ar;
}

/* Convert the PAG analysis result into a Prolog Term */
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::pagToProlog(std::string name, std::string dfi) {
  // FIXME: do this by iterating over the data structure instead
  // FIXME: more error recovery

  PrologCompTerm* t = new PrologCompTerm(name);
  PrologList* lst = new PrologList();
  int i = 0;

  if (dfi[i] == '[') {
    ++i;
    if (dfi.substr(i,3) == "TOP") { 
      t->addSubterm(new PrologAtom("top"));
      i += 3;
    } else if (dfi.substr(i,6) == "BOTTOM") {
      t->addSubterm(new PrologAtom("bottom"));
      i += 6;
    } else t->addSubterm(new PrologAtom("error"));
    if (dfi[i] == ':') {
      do {
	++i;
	if (dfi[i] == ']') break;

	// Parse List elements
	std::string tok1 = "";
	while (dfi.substr(i,2) != "->") {
	  if (dfi[i] != '$') // drop '$' chars
	    tok1 += dfi[i];
	  ++i;
	}
	i += 2;
	std::string tok2 = "";
	while (dfi[i] != ',') {
	  if (dfi[i] != '$') // drop '$' chars
	    tok2 += dfi[i];
	  ++i;
	}	

	PrologInfixOperator* ifx = new PrologInfixOperator("->");
	ifx->addSubterm(new PrologAtom(tok1));
	ifx->addSubterm(new PrologAtom(tok2));
	lst->addElement(ifx);
      } while (dfi[i] == ',');

      t->addSubterm(lst);

      if (dfi[i] != ']') {
	std::cerr << "Warning: TermPrinter could not parse " << dfi << std::endl;
      }
      ++i;
    } else t->addSubterm(new PrologAtom("error"));
  }
  //cerr << dfi << endl;
  //cerr << t->getRepresentation() << endl;
  return t;   
}
	
/* Create a prolog term representing a leaf node.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::leafTerm(SgNode* astNode, SynthesizedAttributesList synList) {
	/* create composite term and add class name*/
	PrologCompTerm* t = new PrologCompTerm("leaf_node");
	addClassname(astNode,t);
	return t;
}

/* Create a prolog term representing a unary operator.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::unaryTerm(SgNode* astNode, SynthesizedAttributesList synList) {
	/* create composite term and add class name*/
	PrologCompTerm* t = new PrologCompTerm("unary_node");
	addClassname(astNode,t);
	/* add children's subterms*/
	t->addSubterm(synList.at(0));
	return t;
}

/* Create a prolog term representing a binary operator.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::binaryTerm(SgNode* astNode, SynthesizedAttributesList synList) {
	/* create composite term and add class name*/
	PrologCompTerm* t = new PrologCompTerm("binary_node");
	addClassname(astNode,t);
	/* add children's subterms*/
	t->addSubterm(synList.at(0));
	t->addSubterm(synList.at(1));
	return t;
}

/* Create a prolog term representing a ternary operator.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::ternaryTerm(SgNode* astNode, SynthesizedAttributesList synList) {
	/* create composite term and add class name*/
	PrologCompTerm* t = new PrologCompTerm("ternary_node");
	addClassname(astNode,t);
	t->addSubterm(synList.at(0)); 
	t->addSubterm(synList.at(1));
	t->addSubterm(synList.at(2));
	return t;
}

/* Create a prolog term representing a quaternary operator.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::quaternaryTerm(SgNode* astNode, SynthesizedAttributesList synList) {
	/* create composite term and add class name*/
	PrologCompTerm* t = new PrologCompTerm("quaternary_node"); 
  	addClassname(astNode,t);
	t->addSubterm(synList.at(0));
	t->addSubterm(synList.at(1));
	t->addSubterm(synList.at(2));
	t->addSubterm(synList.at(3));
	return t;
}

/* Create a prolog term representing a node with more than four successors.*/
template<typename DFI_STORE_TYPE>
PrologCompTerm*
TermPrinter<DFI_STORE_TYPE>::listTerm(SgNode* astNode, SynthesizedAttributesList synList) {
  /* create composite term and add class name*/
  PrologCompTerm* t = new PrologCompTerm("list_node");
  addClassname(astNode,t);
 /* add children's subterms to list*/
  PrologList* l = new PrologList();
  SynthesizedAttributesList::iterator it;
  it = synList.begin();
  while(it !=synList.end()) {
    /* strip "null" Atoms */
    PrologAtom* atom = dynamic_cast<PrologAtom*>(*it);
    if (!(atom && (atom->getName() == "null")))
      l->addElement(*it);
    it++;
  }
  /* add list to term*/
  t->addSubterm(l);
  return t;
}

template<typename DFI_STORE_TYPE>
void
TermPrinter<DFI_STORE_TYPE>::addClassname(SgNode* astNode, PrologCompTerm* t) {
	/* create a new atom with Class name (ZigZagCase transfomed to zig_zag_case
	 * to avoid being seen as a variable)*/
	PrologAtom* cName = new PrologAtom(*(PrologSupport::prologize(astNode->class_name())));
	t->addSubterm(cName);
}

#endif
