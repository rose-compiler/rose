/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2007-2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/

#include <satire_rose.h>
#include "termite.h"
#include "TermToRose.h"
#include <iostream>
#include <sstream>
#include <string>
#include <assert.h>
#include <boost/regex.hpp>

#if !HAVE_SWI_PROLOG
#  include "termparser.tab.h++"
extern int yyparse();
extern FILE* yyin;
extern PrologTerm* prote;
#endif

using namespace std;
using namespace boost;

#define FI Sg_File_Info::generateDefaultFileInfoForTransformationNode()
static inline string makeFunctionID(const string& func_name, 
				    const string& func_type) {
  return func_name+'-'+func_type;
}

/**
 * Unparse to a file
 */

void PrologToRose::unparseFile(SgSourceFile& f, string prefix, string suffix, 
			       SgUnparse_Info* ui) 
{
  const char* fn = regex_replace(
      regex_replace(f.get_file_info()->get_filenameString(), 
		    regex("(\\..+?)$"),	
		    suffix+string("\\1")),
      regex("^.*/"), prefix+string("/")).c_str();
  ofstream ofile(fn);
  cerr << "Unparsing " << fn << endl;
  ofile << globalUnparseToString(f.get_globalScope(), ui);
}

void PrologToRose::unparse(string filename, string dir, string suffix, 
			   SgNode* node) 
{
  SgUnparse_Info* unparseInfo = new SgUnparse_Info();
  unparseInfo->unset_SkipComments();    // generate comments
  unparseInfo->unset_SkipWhitespaces(); // generate all whitespaces to
					// format the code
  unparseInfo->set_SkipQualifiedNames(); // Adrian: skip qualified
					 // names -> this would cause
					 // a call to the EDG
					 // otherwise

  //resetParentPointers(glob, file);
  //AstPostProcessing(glob);

  /* we stop unparsing at SgGlobal level! output results and be happy.*/

  if (filename != "") {
    // All into one big file
    ofstream ofile(filename.c_str());
    if (SgProject* project = dynamic_cast<SgProject*>(node))
      for (int i = 0; i < project->numberOfFiles(); ++i) {
	SgSourceFile *sageFile = isSgSourceFile((*project)[i]);
    if (sageFile != NULL)
	    ofile << globalUnparseToString(sageFile->get_globalScope(), unparseInfo);
      }
    else if (SgSourceFile* file = dynamic_cast<SgSourceFile*>(node))
      ofile << globalUnparseToString(file->get_globalScope(), unparseInfo);
    else ofile << node->unparseToString();
  } else {
    // seperate files
    if (SgProject* project = dynamic_cast<SgProject*>(node))
      for (int i = 0; i < project->numberOfFiles(); ++i) {
        SgSourceFile *file = isSgSourceFile((*project)[i]);
        if (file != NULL)
          unparseFile(*file, dir, suffix, unparseInfo);
      }
    else if (SgSourceFile* file = dynamic_cast<SgSourceFile*>(node))
      unparseFile(*file, dir, suffix, unparseInfo);
    else cout << node->unparseToString();
  }
}

SgNode*
PrologToRose::toRose(const char* filename) {
#if !HAVE_SWI_PROLOG
  yyin = fopen( filename, "r" );
  yyparse();
#else
  /*open('input.pl',read,_,[alias(rstrm)]),
    read_term(rstrm,X,[double_quotes(string)]),
    close(rstrm),*/

  term_t a0 = PL_new_term_refs(10);
  term_t fn	   = a0 + 0;
  term_t read	   = a0 + 1;
  term_t var	   = a0 + 2;
  term_t alias	   = a0 + 3;
  term_t open      = a0 + 4;
  term_t r	   = a0 + 5;
  term_t term	   = a0 + 6;
  term_t flags     = a0 + 7;
  term_t read_term = a0 + 8;
  term_t close     = a0 + 9;


  PL_put_atom_chars(fn, filename);
  PL_put_atom_chars(read, "read");
  PL_put_variable(var);
  PL_chars_to_term("[alias(r)]", alias);
  PL_cons_functor(open, PL_new_functor(PL_new_atom("open"), 4), 
		  fn, read, var, alias);

  PL_put_atom_chars(r, "r");
  PL_put_variable(term);
  PL_chars_to_term("[double_quotes(string)]", flags);
  PL_cons_functor(read_term, PL_new_functor(PL_new_atom("read_term"), 3), 
		  r, term, flags);

  PL_chars_to_term("close(r)", close);

  assert(PL_call(open, NULL) &&
	 PL_call(read_term, NULL) &&
	 PL_call(close, NULL));

  PrologTerm* prote = PrologTerm::wrap_PL_Term(term);
  
#endif
  SgNode* root = toRose(prote);
  ROSE_ASSERT(initializedNamesWithoutScope.empty());
  ROSE_ASSERT(declarationStatementsWithoutScope.empty());
  return root;
}

/**
 * create ROSE-IR for valid term representation*/
SgNode*
PrologToRose::toRose(PrologTerm* t) {

  SgNode* node;
  if(PrologCompTerm* c = dynamic_cast<PrologCompTerm*>(t)) {

#ifdef COMPACT_TERM_NOTATION
    string tname = c->getName();
    debug("converting " + tname + "\n");
    // Insert a dummy first argument to be backwards compatible 
    // with the old term representation
    c->addFirstSubTerm(new PrologAtom(tname));
    if (dynamic_cast<PrologList*>(c->at(1))) {
      node = listToRose(c,tname);
    } else {
      switch (c->getSubTerms().size()) {
      case (4): node = leafToRose(c,tname); break;
      case (5): node = unaryToRose(c,tname); break;
      case (6): node = binaryToRose(c,tname); break;
      case (7): node = ternaryToRose(c,tname); break;
      case (8): node = quaternaryToRose(c,tname); break;
      default: node = (SgNode*) 0;
      }
    }
#else

    string termType = t->getName();
    /* get type */
    PrologAtom* nterm = dynamic_cast<PrologAtom*>(c->at(0));
    if (nterm == NULL) {
      cerr << "** ERROR: Term has no type argument >>" 
	   << t->getRepresentation() << "<<" << endl;
      ROSE_ASSERT(0);
    }
		
    string tname = nterm->getName();
    debug("converting " + tname + "\n");
    /* depending on the type, call other static member functions*/
    if(termType=="unary_node") {
      node = unaryToRose(c,tname);
    } else if(termType=="binary_node") { 
      node = binaryToRose(c,tname);
    } else if(termType=="ternary_node") { 
      node = ternaryToRose(c,tname);
    } else if(termType=="quaternary_node") { 
      node = quaternaryToRose(c,tname);
    } else if(termType=="list_node") { 
      node = listToRose(c,tname);
    } else if(termType=="leaf_node") { 
      node = leafToRose(c,tname);
    } 
#endif
  } else {
    node = (SgNode*) 0;
  }

  if ((node == NULL) && (t->getRepresentation() != "null")
      && (t->getName() != "source_file")) {
    cerr << "**WARNING: could not translate the term '" 
	 << t->getRepresentation() << "'" 
         << " of arity " << t->getArity() << "." << endl;
  }

  SgLocatedNode* ln = isSgLocatedNode(node);

  // Set the CompilerGenerated Flag
  if (ln != NULL) {
    //Sg_File_Info* fi = ln->get_file_info();
    //fi->set_classificationBitField(fi->get_classificationBitField() 
    //				   | Sg_File_Info::e_compiler_generated 
			   /*| Sg_File_Info::e_output_in_code_generation*/
    //);
	  
    // Set EndOfConstruct
    ln->set_endOfConstruct(ln->get_startOfConstruct());
  }

  // Create the attached PreprocessingInfo
  PrologCompTerm* ct = dynamic_cast<PrologCompTerm*>(t);
  if (ln != NULL && ct != NULL) {
    PrologCompTerm* annot = 
      dynamic_cast<PrologCompTerm*>(ct->at(ct->getArity()-3));
    ROSE_ASSERT(annot);

    PrologCompTerm* ppil = 
      dynamic_cast<PrologCompTerm*>(annot->at(annot->getArity()-1));
    if (ppil) {
      PrologList* l = dynamic_cast<PrologList*>(ppil->at(0));
      if (l) {
	for (deque<PrologTerm*>::iterator it = l->getSuccs()->begin();
	     it != l->getSuccs()->end(); ++it) {

	  PrologCompTerm* ppi = dynamic_cast<PrologCompTerm*>(*it);
	  ROSE_ASSERT(ppi);

	  Sg_File_Info* fi = createFileInfo(ppi->at(ppi->getArity()-1));
	  PreprocessingInfo::RelativePositionType locationInL = 
            (PreprocessingInfo::RelativePositionType)
	    createEnum(ppi->at(1), re.RelativePositionType);
	
	  ln->addToAttachedPreprocessingInfo(
	     new PreprocessingInfo((PreprocessingInfo::DirectiveType)
				   createEnum(ppi, re.DirectiveType),
				   ppi->at(0)->getName(),
				   fi->get_filenameString(),
				   fi->get_line(),
				   fi->get_col(),
				   1 /* FIXME: nol */,
				   locationInL));
	}
      } else ROSE_ASSERT(ppil->at(0)->getName() == "null");
    }
  }

  // Set Scope of children
  SgScopeStatement* scope = dynamic_cast<SgScopeStatement*>(node);
  if (scope != NULL) {
    for (vector<SgInitializedName*>::iterator it = 
	       initializedNamesWithoutScope.begin();
	 it != initializedNamesWithoutScope.end(); it++) {
      (*it)->set_scope(scope);
    }
    initializedNamesWithoutScope.clear();

    for (vector<SgDeclarationStatement*>::iterator it = 
	       declarationStatementsWithoutScope.begin();
	 it != declarationStatementsWithoutScope.end(); it++) {
      (*it)->set_scope(scope);
      SgClassDeclaration* cd = isSgClassDeclaration(*it);
      if ((cd != NULL) && cd->isForward()) {
	// The first nondefining declaration is used to identify and
	// compare symbols:
	// Generate Symbol for the first nondef. class declaration
	//SgClassSymbol* symbol = new SgClassSymbol(cd);
	//scope->insert_symbol(name, symbol);
      }
    }
    declarationStatementsWithoutScope.clear();

    // rebuild the symbol table
    scope->set_symbol_table(NULL);
    SageInterface::rebuildSymbolTable(scope);
  }
  	
  return node;
}

static void assert_arity(PrologCompTerm* t, int arity) {
  if (t->getArity() != arity) {
    cerr << "** ERROR: " << t->getArity() << "-ary Term >>" 
	 << t->getRepresentation() << "<<" 
	 << " does not have the expected arity of " << arity 
	 << endl;
    ROSE_ASSERT(false && "arity error");
  }
}

/** create ROSE-IR for unary node*/
SgNode*
PrologToRose::unaryToRose(PrologCompTerm* t,string tname) {
  debug("unparsing unary"); debug(t->getRepresentation());
  /* assert correct arity of term*/
  assert_arity(t, 5);
  /*get child node (prefix traversal step)*/
  SgNode* child1 = toRose(t->at(1));
  /*create file info and check it*/
  Sg_File_Info* fi = createFileInfo(t->at(4));
  testFileInfo(fi);

  /*node to be created*/
  SgNode* s = NULL;
	
  /* depending on the node type: create it*/
  if(isValueExp(tname)) {
    s = createValueExp(fi,child1,t);
  } else if(isUnaryOp(tname)) {
    s = createUnaryOp(fi,child1,t);
  } else if(tname == SG_PREFIX "source_file") {
    s = createFile(fi,child1,t);
  } else if(tname == SG_PREFIX "return_stmt") {
    s = createReturnStmt(fi,child1,t);
  } else if(tname == SG_PREFIX "function_definition") {
    s = createFunctionDefinition(fi,child1,t);
  } else if(tname == SG_PREFIX "initialized_name") {
    s = createInitializedName(fi,child1,t);
  } else if(tname == SG_PREFIX "assign_initializer") {
    s = createAssignInitializer(fi,child1,t);
  } else if(tname == SG_PREFIX "expr_statement") {
    s = createExprStatement(fi,child1,t);
  } else if(tname == SG_PREFIX "default_option_stmt") {
    s = createDefaultOptionStmt(fi,child1,t);
  } else if(tname == SG_PREFIX "class_declaration") {
    s = createClassDeclaration(fi,child1,t);
  } else if(tname == SG_PREFIX "delete_exp") {
    s = createDeleteExp(fi,child1,t);
  } else if(tname == SG_PREFIX "var_arg_op") {
    s = createVarArgOp(fi,child1,t);
  } else if(tname == SG_PREFIX "var_arg_end_op") {
    s = createVarArgEndOp(fi,child1,t);
  } else if(tname == SG_PREFIX "var_arg_start_one_operand_op") {
    s = createVarArgStartOneOperandOp(fi,child1,t);
  } else if(tname == SG_PREFIX "aggregate_initializer") {
    s = createAggregateInitializer(fi,child1,t);
  } else if(tname == SG_PREFIX "namespace_declaration_statement") {
    s = createNamespaceDeclarationStatement(fi,child1,t);
  } else if(tname == SG_PREFIX "size_of_op") {
    s = createSizeOfOp(fi,child1,t);
  } else if(tname == SG_PREFIX "constructor_initializer") {
    s = createConstructorInitializer(fi,child1,t);
  } else if(tname == SG_PREFIX "pragma_declaration") {
    s = createPragmaDeclaration(fi,child1,t);
  } else if (tname == SG_PREFIX "typedef_declaration") {
    s = createTypedefDeclaration(fi,t);
  } else cerr<<"**WARNING: unhandled Unary Node: "<<tname<<endl;
    

  /* to be:*/
  //ROSE_ASSERT(s != NULL);
	
  /*set s to be the parent of its child node*/
  //cerr<<s->class_name()<<endl;
  if (s != NULL){ //&& !isSgProject(s) && !isSgFile(s) && !isSgGlobal(s)) {
    if(child1 != NULL) {
      child1->set_parent(s);
    }
  }
  return s;
}

/** create ROSE-IR for binary node*/
SgNode*
PrologToRose::binaryToRose(PrologCompTerm* t,string tname) {
  debug("unparsing binary"); debug(t->getRepresentation());
  /* assert correct arity of term*/
  assert_arity(t, 6);
  /*get child nodes (prefix traversal step)*/
  SgNode* child1 = toRose(t->at(1));
  SgNode* child2 = toRose(t->at(2));
  /*create file info and check it*/
  Sg_File_Info* fi = createFileInfo(t->at(5));
  testFileInfo(fi);
  /* node to be created*/
  SgNode* s = NULL;
	
  /* create node depending on type*/
  if (tname == SG_PREFIX "function_declaration") {
    s = createFunctionDeclaration(fi,child1,child2,t);
  } else if (isBinaryOp(tname)) {
    s = createBinaryOp(fi,child1,child2,t);
  } else if (tname == SG_PREFIX "cast_exp") {
    s = createUnaryOp(fi,child1,t);
  } else if (tname == SG_PREFIX "switch_statement") {
    s = createSwitchStatement(fi,child1,child2,t);
  } else if (tname == SG_PREFIX "do_while_stmt") {
    s = createDoWhileStmt(fi,child1,child2,t);
  } else if (tname == SG_PREFIX "while_stmt") {
    s = createWhileStmt(fi,child1,child2,t);
  } else if(tname == SG_PREFIX "var_arg_copy_op") {
    s = createVarArgCopyOp(fi,child1,child2,t);
  } else if(tname == SG_PREFIX "var_arg_start_op") {
    s = createVarArgStartOp(fi,child1,child2,t);	
  } else if(tname == SG_PREFIX "function_call_exp") {
    s = createFunctionCallExp(fi,child1,child2,t);	
  } else if(tname == SG_PREFIX "try_stmt") {
    s = createTryStmt(fi,child1,child2,t);	
  } else if(tname == SG_PREFIX "catch_option_stmt") {
    s = createCatchOptionStmt(fi,child1,child2,t);	
  } else if (tname == SG_PREFIX "source_file") {
    s = createFile(fi,child1,t);
  } else cerr<<"**WARNING: unhandled Binary Node: "<<tname<<endl;

  /*set s to be the parent of its child nodes*/
  if (s != NULL) {
    if(child1 != NULL) {
      child1->set_parent(s);
    }
    if(child2 != NULL) {
      child2->set_parent(s);
    }
  }
  return s;
}

/** create ROSE-IR for ternary node*/
SgNode*
PrologToRose::ternaryToRose(PrologCompTerm* t,string tname) {
  debug("unparsing ternary");
  /* assert correct arity of term*/
  assert_arity(t, 7);
  /*get child nodes (prefix traversal step)*/
  SgNode* child1 = toRose(t->at(1));
  SgNode* child2 = toRose(t->at(2));
  SgNode* child3 = toRose(t->at(3));
  /*create file info and check it*/
  Sg_File_Info* fi = createFileInfo(t->at(6));
  testFileInfo(fi);
  /* create nodes depending on type*/
  SgNode* s = NULL;
  if (tname == SG_PREFIX "if_stmt") {
    s = createIfStmt(fi,child1,child2,child3,t);
  } else if (tname == SG_PREFIX "case_option_stmt") {
    s = createCaseOptionStmt(fi,child1,child2,child3,t);
  } else if (tname == SG_PREFIX "member_function_declaration") {
    s = createMemberFunctionDeclaration(fi,child1,child2,child3,t);
  } else if (tname == SG_PREFIX "new_exp") {
    s = createNewExp(fi,child1,child2,child3,t);
  } else if (tname == SG_PREFIX "conditional_exp") {
    s = createConditionalExp(fi,child1,child2,child3,t);
  } else cerr<<"**WARNING: unhandled Ternary Node: "<<tname<<endl;

  /*set s to be the parent of its child nodes*/
  if (s != NULL) {
    if(child1 != NULL) {
      child1->set_parent(s);
    }
    if(child2 != NULL) {
      child2->set_parent(s);
    }
    if(child3 != NULL) {
      child3->set_parent(s);
    }
  }
  return s;
}
	
/** create ROSE-IR for quaternary node*/
SgNode*
PrologToRose::quaternaryToRose(PrologCompTerm* t,string tname) {
  debug("unparsing quaternary");
  /* assert correct arity of term*/
  assert_arity(t, 8);
  /*get child nodes (prefix traversal step)*/
  SgNode* child1 = toRose(t->at(1));
  SgNode* child2 = toRose(t->at(2));
  SgNode* child3 = toRose(t->at(3));
  SgNode* child4 = toRose(t->at(4));
  /*create file info and check it*/
  Sg_File_Info* fi = createFileInfo(t->at(7));
  testFileInfo(fi);
  /* node to be created*/
  SgNode* s = NULL;
  if(tname == SG_PREFIX "for_statement") {
    s = createForStatement(fi,child1,child2,child3,child4,t);
  } else cerr<<"**WARNING: unhandled Quarternary Node: "<<tname<<endl;
  /*set s to be the parent of its child nodes*/
  if (s != NULL) {
    if(child1 != NULL) {
      child1->set_parent(s);
    }
    if(child2 != NULL) {
      child2->set_parent(s);
    }
    if(child3 != NULL) {
      child3->set_parent(s);
    }
    if(child4 != NULL) {
      child4->set_parent(s);
    }
  }
  return s;
}

/** create ROSE-IR for list node*/
SgNode*
PrologToRose::listToRose(PrologCompTerm* t,string tname) {
	
  debug("unparsing list node");
  PrologList* l = dynamic_cast<PrologList*>(t->at(1));
  assert(l != (PrologList*) 0);
  /* assert correct arity of term*/
  assert_arity(t, 5);
  /*create file info and check it*/
  Sg_File_Info* fi = createFileInfo(t->at(4));
  testFileInfo(fi);
  /*get child nodes (prefix traversal step)*/
  SgNode* cur = NULL;
  /* recursively, create ROSE-IR for list-members*/
  deque<PrologTerm*>* succterms = l->getSuccs();
  deque<SgNode*>* succs = new deque<SgNode*>();
  deque<PrologTerm*>::iterator it = succterms->begin();
  while (it != succterms->end()) {
    cur = toRose(*it);
    if(cur != (SgNode*) 0) {
      succs->push_back(cur);
      debug("added successor of type " + dynamic_cast<PrologCompTerm*>(*it)->at(0)->getName());
    } else {
      debug("did not add NULL successor");
    }
    it++;
  }
  /* depending on the type, create node*/
  SgNode* s = NULL;
  if(tname == SG_PREFIX "global") {
    s = createGlobal(fi,succs);
  } else if (tname == SG_PREFIX "project") {
    s = createProject(fi,succs);
  } else if (tname == SG_PREFIX "function_parameter_list") {
    s = createFunctionParameterList(fi,succs);
  } else if (tname == SG_PREFIX "basic_block") {
    s = createBasicBlock(fi,succs);
  } else if (tname == SG_PREFIX "variable_declaration") {
    s = createVariableDeclaration(fi,succs,t);
  } else if (tname == SG_PREFIX "for_init_statement") {
    s = createForInitStatement(fi,succs);
  } else if (tname == SG_PREFIX "class_definition") {
    s = createClassDefinition(fi,succs,t);
  } else if (tname == SG_PREFIX "enum_declaration") {
    s = createEnumDeclaration(fi,succs,t);
  } else if (tname == SG_PREFIX "expr_list_exp") {
    s = createExprListExp(fi,succs);
  } else if (tname == SG_PREFIX "ctor_initializer_list") {
    s = createCtorInitializerList(fi,succs);
  } else if (tname == SG_PREFIX "namespace_definition_statement") {
    s = createNamespaceDefinitionStatement(fi,succs);
  } else if (tname == SG_PREFIX "catch_statement_seq") {
    s = createCatchStatementSeq(fi,succs);
  }
  /* note that for the list nodes the set_parent operation takes place
   * inside the methods when necessary since they always require
   * an iteration over the list anyway. */

  return s;
}


/**create ROSE-IR from leaf terms*/
SgNode*
PrologToRose::leafToRose(PrologCompTerm* t,string tname) {
  debug("unparsing leaf");
  /* assert correct arity of term*/
  assert_arity(t, 4);
  /* create file info and check it*/
  Sg_File_Info* fi = createFileInfo(t->at(3));
  testFileInfo(fi);
  /* node to be created*/
  SgNode* s = NULL;
  /* some list nodes become leaf nodes when the list is empty
   * -> create dummy list and call corresponding factory methods*/
  if (tname == SG_PREFIX "function_parameter_list") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createFunctionParameterList(fi,adummy);
  } else if (tname == SG_PREFIX "basic_block") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createBasicBlock(fi,adummy);
  } else if (tname == SG_PREFIX "class_definition") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createClassDefinition(fi,adummy,t);
  } else if (tname == SG_PREFIX "ctor_initializer_list") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createCtorInitializerList(fi,adummy);
  } else if (tname == SG_PREFIX "expr_list_exp") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createExprListExp(fi,adummy);
  } else if (tname == SG_PREFIX "namespace_definition_statement") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createNamespaceDefinitionStatement(fi,adummy);
  } else if (tname == SG_PREFIX "for_init_statement") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createForInitStatement(fi,adummy);
    /* regular leaf nodes*/
  } else if (tname == SG_PREFIX "var_ref_exp") {
    s = createVarRefExp(fi,t);
  } else if (tname == SG_PREFIX "break_stmt") {
    s = createBreakStmt(fi,t);
  } else if (tname == SG_PREFIX "continue_stmt") {
    s = createContinueStmt(fi,t);
  } else if (tname == SG_PREFIX "label_statement") {
    s = createLabelStatement(fi,t);
  } else if (tname == SG_PREFIX "goto_statement") {
    s = createGotoStatement(fi,t);
  } else if (tname == SG_PREFIX "ref_exp") {
    s = createRefExp(fi,t);
  } else if (tname == SG_PREFIX "function_ref_exp") {
    s = createFunctionRefExp(fi,t);
  } else if (tname == SG_PREFIX "member_function_ref_exp") {
    s = createMemberFunctionRefExp(fi,t);
  } else if (tname == SG_PREFIX "this_exp") {
    s = createThisExp(fi,t);
  } else if(tname == SG_PREFIX "pragma") {
    s = createPragma(fi,t);
  } else if (tname == SG_PREFIX "null_statement") {
    s = new SgNullStatement(fi);
  } else if (tname == SG_PREFIX "null_expression") {
    s = new SgNullExpression(fi);
  }
  return s;
}

/** test file info soundness*/
void
PrologToRose::testFileInfo(Sg_File_Info* fi) {
  ROSE_ASSERT(fi != NULL);
  ROSE_ASSERT(fi->get_line() >= 0);
  ROSE_ASSERT(fi->get_col() >= 0);
}

/** create Sg_File_Info from term*/
Sg_File_Info*
PrologToRose::createFileInfo(PrologTerm* t) {
  debug("unparsing file info");
  Sg_File_Info *fi = NULL;
  if (PrologAtom *a = dynamic_cast<PrologAtom*>(t)) {
    /*node new or file info removed during transformation*/
    /*=> only possible atom: null*/
    assert(a->getName() == "null");
    fi = FI;
  } else {
    /*file info was preserved*/
    /*=> the file info term should be named file_info and have an arity of 3*/
    PrologCompTerm* u = dynamic_cast<PrologCompTerm*>(t);
    assert((PrologCompTerm*) 0 != u);
    debug(u->getRepresentation());
    assert(u->getName() == "file_info");
    assert_arity(u, 3);
    if ((u->at(0)->getName() == "compilerGenerated") || 
        (u->at(0)->getName() == "<invalid>")) {
      debug("compiler generated node");
      fi = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
    }
    else {
      /* a filename is present => retrieve data from term and generete node*/
      PrologAtom* filename = dynamic_cast<PrologAtom*>(u->at(0));
      PrologInt* line = dynamic_cast<PrologInt*>(u->at(1));
      PrologInt* col = dynamic_cast<PrologInt*>(u->at(2));
      /* filename must be a term representing a character string, 
       * line and col are integers */
      assert(filename != (PrologAtom*) 0);
      assert(line != (PrologInt*) 0);
      assert(col != (PrologInt*) 0);
      assert(line->getValue() >= 0);
      assert(col->getValue() >= 0);
      fi = new Sg_File_Info(filename->getName(),
			    line->getValue(), col->getValue());
    }
  } 
  ROSE_ASSERT(fi != NULL);

  return fi;
}

/**create enum type from annotation*/
SgEnumType*
PrologToRose::createEnumType(PrologTerm* t) {
  /* first subterm is the name of the enum*/
  debug("creating enum type");
  PrologCompTerm* annot = isPrologCompTerm(t);
  ROSE_ASSERT(annot != NULL);
  ROSE_ASSERT(annot->getName() == "enum_type");
  /*create dummy declaration*/
  SgEnumDeclaration* dec = isSgEnumDeclaration(toRose(annot->at(0)));
  ROSE_ASSERT(dec != NULL);
  /* create type using a factory Method*/
  return SgEnumType::createType(dec);
}

/**create pointer type from annotation*/
SgPointerType*
PrologToRose::createPointerType(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  ROSE_ASSERT(c-> getName() == "pointer_type");
  /* first subterm is the base type*/
  SgType* base_type = PrologToRose::createType(c->at(0));
  ROSE_ASSERT(base_type != NULL);
  /* use SgPointerType's factory method*/
  SgPointerType* pt = SgPointerType::createType(base_type);
  ROSE_ASSERT(pt != NULL);
  return pt;
}

/**create reference type from annotation*/
SgReferenceType*
PrologToRose::createReferenceType(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  ROSE_ASSERT(c->getName() == "reference_type");
  /* first subterm is the base type*/
  SgType* base_type = PrologToRose::createType(c->at(0));
  ROSE_ASSERT(base_type != NULL);
  /* use SgPointerType's factory method*/
  SgReferenceType* pt = SgReferenceType::createType(base_type);
  ROSE_ASSERT(pt != NULL);
  return pt;
}
	
/** create array type from annotation*/
SgArrayType*
PrologToRose::createArrayType(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  ROSE_ASSERT(c->getName() == "array_type");
  /*first subterm is base type*/
  SgType* base_type = createType(c->at(0));
  ROSE_ASSERT(base_type != NULL);
  /* second subterm is an expression*/
  SgExpression* e = NULL;
  e = isSgExpression(toRose(c->at(1)));
  /* use factory method of SgArrayType*/
  SgArrayType* at = SgArrayType::createType(base_type,e);
  ROSE_ASSERT(at != NULL);
  return at;
}

/**
 * create SgModifierType
 */
SgModifierType*
PrologToRose::createModifierType(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  SgModifierType* mt = new SgModifierType(createType(c->at(0)));
  setTypeModifier(c->at(1),&(mt->get_typeModifier()));
  return mt;
}
/**
 * create SgTypedefType
 */
SgTypedefType*
PrologToRose::createTypedefType(PrologTerm* t) {
  /* extract declaration*/
  PrologCompTerm* annot = isPrologCompTerm(t);
  ROSE_ASSERT(annot != NULL);
  /*make sure this is supposed to be a typedef type*/
  string tname = annot->getName();
  ROSE_ASSERT(tname == "typedef_type");

  /*extract name*/
  SgName n = *(toStringP(annot->at(0)));
  /*create default file info*/
  /*we don't want to keep the base type empty, use int (irrelevant form unparsing*/
  SgTypeInt* i = new SgTypeInt();
  /*create SgTypedefDeclaration*/
  SgTypedefDeclaration* decl = new SgTypedefDeclaration(FI,n,i,NULL,NULL,NULL);
  ROSE_ASSERT(decl != NULL);
  /*fake parent scope*/
  fakeParentScope(decl);
  declarationStatementsWithoutScope.push_back(decl);
  SgTypedefType* tpe = SgTypedefType::createType(decl);
  ROSE_ASSERT(tpe != NULL);
	
  return tpe;
}


/**
 * create SgType from term, possibly recursively
 */
SgType* 
PrologToRose::createType(PrologTerm* t) {
  if (PrologCompTerm* c = isPrologCompTerm(t)) {
    string tname = t->getName();
    if (tname == "enum_type") {
      return createEnumType(t);
    } else if (tname == "pointer_type") {
      return createPointerType(t);
    } else if (tname == "reference_type") {
      return createReferenceType(t);
    } else if (tname == "class_type") {
      return createClassType(t);
    } else if (tname == "function_type") {
      return createFunctionType(t);
    } else if (tname == "member_function_type") {
      return createMemberFunctionType(t);
    } else if (tname == "array_type") {
      return createArrayType(t);
    } else if (tname == "modifier_type") {
      return createModifierType(t);
    } else if (tname == "typedef_type") {
      return createTypedefType(t);
    }
    /*unknown type*/
    ROSE_ASSERT(false);
  }
  if (PrologAtom* a = dynamic_cast<PrologAtom*>(t)) {
    string tname = a->getName();
    if (tname == "null") {
      warn_msg("warning: no type created");
      return NULL;
    }
    if (tname==SG_PREFIX "type_bool") return new SgTypeBool();
    else if (tname==SG_PREFIX "type_char") return new SgTypeChar();
    else if (tname==SG_PREFIX "type_default") return new SgTypeDefault();
    else if (tname==SG_PREFIX "type_double") return new SgTypeDouble();
    else if (tname==SG_PREFIX "type_float") return new SgTypeFloat();
    else if (tname==SG_PREFIX "type_global_void") return new SgTypeGlobalVoid(); 
    else if (tname==SG_PREFIX "type_ellipse") {return new SgTypeEllipse();}
    else if (tname==SG_PREFIX "type_int") {return new SgTypeInt();}
    else if (tname==SG_PREFIX "type_long") return new SgTypeLong();
    else if (tname==SG_PREFIX "type_long_double") return new SgTypeLongDouble();
    else if (tname==SG_PREFIX "type_long_long") return new SgTypeLongLong();
    else if (tname==SG_PREFIX "type_short") return new SgTypeShort();
    else if (tname==SG_PREFIX "type_signed_char") return new SgTypeSignedChar();
    else if (tname==SG_PREFIX "type_signed_int") return new SgTypeSignedInt();
    else if (tname==SG_PREFIX "type_signed_long") return new SgTypeSignedLong();
    else if (tname==SG_PREFIX "type_signed_short") return new SgTypeSignedShort();
    else if (tname==SG_PREFIX "type_string") return new SgTypeString();
    else if (tname==SG_PREFIX "type_unknown") return new SgTypeUnknown();
    else if (tname==SG_PREFIX "type_unsigned_char") return new SgTypeUnsignedChar();
    else if (tname==SG_PREFIX "type_unsigned_int") return new SgTypeUnsignedInt();
    else if (tname==SG_PREFIX "type_unsigned_long") return new SgTypeUnsignedLong();
    else if (tname==SG_PREFIX "type_unsigned_long_long") return new SgTypeUnsignedLongLong();
    else if (tname==SG_PREFIX "type_unsigned_short") return new SgTypeUnsignedShort(); 
    else if (tname==SG_PREFIX "type_void") return new SgTypeVoid();
    else if (tname==SG_PREFIX "type_wchar") return new SgTypeWchar();

  }
  /* should never happen*/
  ROSE_ASSERT(false);
  return (SgType*) 0;
	
}

/**
 * is this string the name of a SgValueExp?*/
bool
PrologToRose::isValueExp(string tname) {
  if(tname == SG_PREFIX "bool_val_exp")
    return true;
  if(tname == SG_PREFIX "string_val")
    return true;
  if(tname == SG_PREFIX "short_val")
    return true;
  if(tname == SG_PREFIX "char_val")
    return true;
  if(tname == SG_PREFIX "unsigned_char_val")
    return true;
  if(tname == SG_PREFIX "wchar_val")
    return true;
  if(tname == SG_PREFIX "unsigned_short_val")
    return true;
  if(tname == SG_PREFIX "int_val")
    return true;
  if(tname == SG_PREFIX "enum_val")
    return true;
  if(tname == SG_PREFIX "unsigned_int_val")
    return true;
  if(tname == SG_PREFIX "long_int_val")
    return true;
  if(tname == SG_PREFIX "long_long_int_val")
    return true;
  if(tname == SG_PREFIX "unsigned_long_long_int_val")
    return true;
  if(tname == SG_PREFIX "unsigned_long_val")
    return true;
  if(tname == SG_PREFIX "float_val")
    return true;
  if(tname == SG_PREFIX "double_val")
    return true;
  if(tname == SG_PREFIX "long_double_val")
    return true;
  return false;
}


/**
 *
 * UnEscape non-printable characters
 */
char
PrologToRose::unescape_char(std::string s) {
  std::string r;

  switch(s.length()) {
  case 1: return s[0];
  case 2: {
    ROSE_ASSERT(s[0] == '\\');
    switch(s[1]) {
    case '\\': return '\\';
    case '\"': return '\"';
    case '\'': return '\'';
    case 'n': return '\n';
    case 'r': return '\r';
    case 'b': return '\b';
    case 't': return '\t';
    case 'f': return '\f';
    case 'a': return '\a';
    case 'v': return '\v';
    default: ROSE_ASSERT(false);
    }
  }

  case 4: { // Char Val
    ROSE_ASSERT((s[0] == '\\'));
    cerr << "**WARNING: Found C-style escaped char \"" << s 
	 << "\" when expecting \"" << s << "\\\"" << endl;
    int c;
    istringstream instr(s.substr(1, 3));
    instr.setf(ios::oct, ios::basefield);
    instr >> c;
    return c;
  }
  case 5: { // Prolog-style Char Val
    ROSE_ASSERT((s[0] == '\\') && (s[4] == '\\'));
    int c;
    istringstream instr(s.substr(1, 3));
    instr.setf(ios::oct, ios::basefield);
    instr >> c;
    return c;
  }
  case 6: { // Unicode
    ROSE_ASSERT((s[0] == '\\') && (s[1] == 'u'));
    unsigned int c;
    istringstream instr(s.substr(2, 5));
    instr.setf(ios::hex, ios::basefield);
    instr >> c;
    if (c > 255) cerr << "** WARNING: truncated 16bit unicode character" << s << endl;
    return c;
  }
  default: ROSE_ASSERT(false);
  }
}

/** create a SgValueExp*/
SgExpression* 
PrologToRose::createValueExp(Sg_File_Info* fi, SgNode* succ, PrologCompTerm* t) {
  string vtype = t->at(0)->getName();
  SgValueExp* ve = NULL;
  /*integer types */
  if(vtype == SG_PREFIX "int_val") {
    /*Integer*/
    debug("unparsing int");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    PrologInt* i = dynamic_cast<PrologInt*>(annot->at(0));
    ROSE_ASSERT(i != NULL);
    SgIntVal* in = new SgIntVal(fi,i->getValue());
    ve = in;
  } else if (vtype == SG_PREFIX "unsigned_int_val") {
    /*unsigned int*/
    debug("unparsing unsigned int");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    PrologInt* i = dynamic_cast<PrologInt*>(annot->at(0));
    ROSE_ASSERT(i != NULL);
    SgUnsignedIntVal* in = new SgUnsignedIntVal(fi,(unsigned int) i->getValue());
    ve = in;
  } else if (vtype == SG_PREFIX "short_val") {
    /*short*/
    debug("unparsing short");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    PrologInt* i = dynamic_cast<PrologInt*>(annot->at(0));
    ROSE_ASSERT(i != NULL);
    SgShortVal* in = new SgShortVal(fi,(short) i->getValue());
    ve = in;
  } else if (vtype == SG_PREFIX "unsigned_short_val") {
    /*unsigned short*/
    debug("unparsing unsigned short");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    PrologInt* i = dynamic_cast<PrologInt*>(annot->at(0));
    ROSE_ASSERT(i != NULL);
    SgUnsignedShortVal* in = new SgUnsignedShortVal(fi,(unsigned short) i->getValue());
    ve = in;
    /*now some integer types where i used
     * istringstreams/ostringstreams as a way to convert
     * the values from/to a std::string in the annotation */
  } else if (vtype == SG_PREFIX "long_int_val") {
    /*long int*/
    debug("unparsing long int");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0));
    PrologInt* i = dynamic_cast<PrologInt*>(annot->at(0));
    ROSE_ASSERT((s != 0) || (i != 0));
    long int number;
    if (s) {
      istringstream instr(s->getName());
      instr >> number;
    } else number = i->getValue();
    SgLongIntVal* valnode = new SgLongIntVal(fi,number);
    ve = valnode;
  } else if (vtype == SG_PREFIX "unsigned_long_val") {
    /*unsigned long int*/
    debug("unparsing unsigned long int");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0));
    PrologInt* i = dynamic_cast<PrologInt*>(annot->at(0));
    ROSE_ASSERT((s != 0) || (i != 0));
    unsigned long int number;
    if (s) {
      istringstream instr(s->getName());
      instr >> number;
    } else number = i->getValue();
    SgUnsignedLongVal* valnode = new SgUnsignedLongVal(fi,number);
    ve = valnode;
  } else if (vtype == SG_PREFIX "long_long_int_val") {
    /*long long  int*/
    debug("unparsing long long int");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0));
    PrologInt* i = dynamic_cast<PrologInt*>(annot->at(0));
    ROSE_ASSERT((s != 0) || (i != 0));
    long long int number;
    if (s) {
      istringstream instr(s->getName());
      instr >> number;
    } else number = i->getValue();
    SgLongLongIntVal* valnode = new SgLongLongIntVal(fi,number);
    ve = valnode;
  } else if (vtype == SG_PREFIX "unsigned_long_long_int_val") {
    /*unsigned long long  int*/
    debug("unparsing unsigned long long int");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0));
    PrologInt* i = dynamic_cast<PrologInt*>(annot->at(0));
    ROSE_ASSERT((s != 0) || (i != 0));
    unsigned long long int number;
    if (s) {
      istringstream instr(s->getName());
      instr >> number;
    } else number = i->getValue();
    SgUnsignedLongLongIntVal* valnode = new SgUnsignedLongLongIntVal(fi,number);
    ve = valnode;
  } else if (vtype == SG_PREFIX "enum_val") {
    debug("unparsing enum value");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    /* get value and name, create a dummy declaration*/
    assert_arity(annot, 4);
    int value = toInt(annot->at(0));
    SgName v_name = *toStringP(annot->at(1));
    SgEnumDeclaration* decdummy = dynamic_cast<SgEnumDeclaration*>(toRose(annot->at(2)));
    ROSE_ASSERT(decdummy != NULL);
    fakeParentScope(decdummy);
   
    SgEnumVal* valnode = new SgEnumVal(fi,value,decdummy,v_name);
    ROSE_ASSERT(valnode != NULL);
    ROSE_ASSERT(valnode->get_declaration() == decdummy);
    debug("declaration faked");
    ve = valnode;

    /* floating point types*/
  } else if (vtype == SG_PREFIX "float_val") {
    // FIXME 
    //} else if (PrologFloat* val = dynamic_cast<PrologFloat*>(annot->at(0))) {
    //  f = val->getValue();
    debug("unparsing float");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);

    float f;
    if (PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0))) {
      istringstream i(s->getName());
      i >> f;
    } else if (PrologInt* val = dynamic_cast<PrologInt*>(annot->at(0))) {
      f = val->getValue();
    } else {
      // Must be either a string or an int
      // FIXME: or a float?
      ROSE_ASSERT(false);
    }		
    ve = new SgFloatVal(fi,f);
    ROSE_ASSERT(ve != NULL);
  } else if (vtype == SG_PREFIX "double_val") {
    debug("unparsing double");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    double f;
    if (PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0))) {
      istringstream i(s->getName());
      i >> f;
    } else if (PrologInt* val = dynamic_cast<PrologInt*>(annot->at(0))) {
      f = val->getValue();
    } else {
      // Must be either a string or an int
      // FIXME: or a float?
      ROSE_ASSERT(false);
    }		
    ve = new SgDoubleVal(fi,f);
    ROSE_ASSERT(ve != NULL);
  } else if (vtype == SG_PREFIX "long_double_val") {
    debug("unparsing long double");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    long double f;
    if (PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0))) {
      istringstream i(s->getName());
      i >> f;
    } else if (PrologInt* val = dynamic_cast<PrologInt*>(annot->at(0))) {
      f = val->getValue();
    } else {
      // Must be either a string or an int
      // FIXME: or a float?
      ROSE_ASSERT(false);
    }		
    ve = new SgLongDoubleVal(fi,f);
    ROSE_ASSERT(ve != NULL);
	
    /* characters */
  } else if (vtype == SG_PREFIX "char_val") {
    //char
    debug("unparsing char");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    char number;
    if (PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0))) {
      number = unescape_char(s->getName());
    } else if (PrologInt* val = dynamic_cast<PrologInt*>(annot->at(0))) {
      number = val->getValue();
    } else {
      // Must be either a string or an int
      ROSE_ASSERT(false);
    }		
    SgCharVal* valnode = new SgCharVal(fi,number);
    ve = valnode;
  } else if (vtype == SG_PREFIX "unsigned_char_val") {
    //unsigned char
    debug("unparsing unsigned char");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    unsigned char number;
    if (PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0))) {
      /*istringstream instr(s->getName());
	instr >> number;*/
      number = unescape_char(s->getName());
    } else if (PrologInt* val = dynamic_cast<PrologInt*>(annot->at(0))) {
      number = val->getValue();
    } else {
      // Must be either a string or an int
      ROSE_ASSERT(false);
    }
    SgUnsignedCharVal* valnode = new SgUnsignedCharVal(fi,number);
    ve = valnode;
  } else if (vtype == SG_PREFIX "wchar_val") {
    //wchar
    debug("unparsing wchar");
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0));
    ROSE_ASSERT(s != NULL);
    unsigned long number;
    istringstream instr(s->getName());
    instr >> number;
    SgWcharVal* valnode = new SgWcharVal(fi,number);
    ve = valnode;
		
    /* boolean*/
  } else if (vtype == SG_PREFIX "bool_val_exp") {
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0));
    int i;
    istringstream str(s->getName());
    str >> i;
    ve = new SgBoolValExp(fi,i);
  } else if (vtype == SG_PREFIX "string_val") {
    PrologCompTerm* annot = retrieveAnnotation(t);
    ROSE_ASSERT(annot != NULL);
    PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0));
    ve = new SgStringVal(fi,s->getName());
  }
	
	
  if(ve != NULL) {
    /* post construction */

    /* when the value of a SgValueExp can be computed at compile time
     * ROSE computes it and creates a value expression with a child,
     * the original expression tree. In this case we recreate it here
     */
    if(succ != NULL) {
      debug("value expression with child");
      SgExpression* ex = dynamic_cast<SgExpression*>(succ);
      ROSE_ASSERT(ex != NULL);
      ve->set_originalExpressionTree(ex);
    }
  } else {
    debug("Value Type " + vtype + " not implemented yet. Returning null pointer for value expression");
  }
  return ve;
}

/**
 * is the string the name of a unary operator?
 */
bool
PrologToRose::isUnaryOp(string opname) {
  if (opname == SG_PREFIX "address_of_op" ||
      opname == SG_PREFIX "bit_complement_op" || 
      opname == SG_PREFIX "expression_root" || 
      opname == SG_PREFIX "minus_op" || 
      opname == SG_PREFIX "not_op" || 
      opname == SG_PREFIX "pointer_deref_exp" || 
      opname == SG_PREFIX "unary_add_op" || 
      opname == SG_PREFIX "minus_minus_op" || 
      opname == SG_PREFIX "plus_plus_op" || 
      opname == SG_PREFIX "cast_exp" || 
      opname == SG_PREFIX "throw_op")
    return true;	
  return false;
}




/**
 * create a SgUnaryOp*/
SgUnaryOp*
PrologToRose::createUnaryOp(Sg_File_Info* fi, SgNode* succ, PrologCompTerm* t) {
  SgExpression* sgexp = dynamic_cast<SgExpression*>(succ);
  PrologTerm* n = t->at(0);
  string opname = n->getName();
  debug("creating " + opname + "\n");	
  //cerr << t->getRepresentation() << endl << succ << endl;
  ROSE_ASSERT(sgexp != NULL);
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  /* the annotation should have arity of 5*/
  ROSE_ASSERT(annot->getArity() == 5);
  // GB (2008-12-04): A unary op's mode is now represented by an atom
  // 'prefix' or 'postfix', not by a numerical constant.
  // PrologInt* mode = dynamic_cast<PrologInt*>(annot->at(0));
  PrologAtom* mode = dynamic_cast<PrologAtom*>(annot->at(0));
  ROSE_ASSERT(mode != NULL);
  SgType* sgtype = createType(annot->at(1));
  ROSE_ASSERT(sgtype != NULL);
  /*nothing special with these*/
  if (opname == SG_PREFIX "address_of_op") return new SgAddressOfOp(fi,sgexp,sgtype);
  else if (opname == SG_PREFIX "bit_complement_op") return new SgBitComplementOp(fi,sgexp,sgtype);
  else if (opname == SG_PREFIX "expression_root") {
    SgExpressionRoot* er = new SgExpressionRoot(fi,sgexp,sgtype);
    debug("Exp Root: " + er->unparseToString());
    return er;
  }
  else if (opname == SG_PREFIX "minus_op") return new SgMinusOp(fi,sgexp,sgtype);
  else if (opname == SG_PREFIX "not_op") return new SgNotOp(fi,sgexp,sgtype);
  else if (opname == SG_PREFIX "pointer_deref_exp") return new SgPointerDerefExp(fi,sgexp,sgtype);
  else if (opname == SG_PREFIX "unary_add_op") return new SgUnaryAddOp(fi,sgexp,sgtype);	
  /* chose wether to use ++ and -- as prefix or postfix via set_mode*/
  else if (opname == SG_PREFIX "minus_minus_op") {
    SgMinusMinusOp* m = new SgMinusMinusOp(fi,sgexp,sgtype);
    m->set_mode(mode->getName() == "prefix" ? SgUnaryOp::prefix
                                            : SgUnaryOp::postfix);
    return m;
  }
  else if (opname == SG_PREFIX "plus_plus_op") {
    SgPlusPlusOp* p = new SgPlusPlusOp(fi,sgexp,sgtype);
    p->set_mode(mode->getName() == "prefix" ? SgUnaryOp::prefix
                                            : SgUnaryOp::postfix);
    return p;
  }
  /* For a cast we need to retrieve cast type (enum)*/
  else if (opname == SG_PREFIX "cast_exp") {
    //cerr<<"######castexp "<< annot->getRepresentation()<< "bug in ROSE?" <<endl;
    SgCastExp* e = new SgCastExp(fi, sgexp, sgtype,
				 (SgCastExp::cast_type_enum)
				 createEnum(annot->at(2), re.cast_type));
    //cerr<< e->unparseToString()<< endl;
    return e;
  } 
  /* some more initialization necessary for a throw */
  else if (opname == SG_PREFIX "throw_op") { 
    /*need to retrieve throw kind* (enum)*/
    int tkind = createEnum(annot->at(2), re.throw_kind);
    // FIXME: use kind!
    /*need to retrieve types */
    PrologList* typel = dynamic_cast<PrologList*>(annot->at(3));
    ROSE_ASSERT(typel != NULL);
    deque<PrologTerm*>* succs = typel->getSuccs();
    deque<PrologTerm*>::iterator it = succs->begin();
    SgTypePtrListPtr tpl = new SgTypePtrList();
    while (it != succs->end()) {
      tpl->push_back(createType(*it));
      it++;
    }
    return new SgThrowOp(fi,sgexp,sgtype);
  }

  ROSE_ASSERT(false);
  /*never called*/
  return (SgUnaryOp*) 0;	
}




/**
 * create SgProject
 */
SgProject*
PrologToRose::createProject(Sg_File_Info* fi,deque<SgNode*>* succs) {
  SgProject* project = new SgProject();
  SgFilePtrList &fl = project->get_fileList();
  
  for (deque<SgNode*>::iterator it = succs->begin();
       it != succs->end(); ++it) {
    SgSourceFile* file = dynamic_cast<SgSourceFile*>(*it);
    if (file != NULL)  // otherwise, it's a binary file, which shouldn't happen
      fl.push_back(file);
  }
  // project->set_fileList(fl);
  return project;
}


/**
 * create SgSourceFile
 */
SgSourceFile*
PrologToRose::createFile(Sg_File_Info* fi,SgNode* child1,PrologCompTerm*) {
  // GB (2008-10-20): It looks like there is a new SgSourceFile class in
  // ROSE 0.9.3a-2261, and that it is an instance of that class that we
  // need.
  SgSourceFile* file = new SgSourceFile();
  file->set_file_info(fi);

  SgGlobal* glob = isSgGlobal(child1);
  ROSE_ASSERT(glob);

#if 0
  // Fixup the nondefining class decls
  for (multimap<string, SgClassDeclaration*>::iterator 
	 it = nondefiningClassDecls.begin(); 
       it != nondefiningClassDecls.end(); ++it) {
    // Set at least all to use the _same_ first nondefining declaration
    ROSE_ASSERT(nondefiningClassDecls.find(it->first) !=
		nondefiningClassDecls.end());
    it->second->set_firstNondefiningDeclaration(
		nondefiningClassDecls.find(it->first)->second);
    // Set the defining declaration
    ROSE_ASSERT(definingClassDecls.find(it->first) != definingClassDecls.end());
    it->second->set_definingDeclaration(
                definingClassDecls.find(it->first)->second);
  }

  for (vector<SgClassDefinition*>::iterator cd = classDefinitions.begin();
       cd != classDefinitions.end(); ++cd) {
    // FIXME debug this...
    
    ROSE_ASSERT((*cd)->get_declaration()->get_firstNondefiningDeclaration() != NULL);
  }
#endif

  // rebuild the symbol table
  glob->set_symbol_table(NULL);
  SageInterface::rebuildSymbolTable(glob);

  file->set_globalScope(glob);
  glob->set_parent(file);

  // Call all kinds of fixups
  AstPostProcessing(file);
  return file;
}

/**
 * create SgSizeOfOp
 */
SgSizeOfOp*
PrologToRose::createSizeOfOp(Sg_File_Info* fi,SgNode* child1,PrologCompTerm* t) {
  /* retrieve anntoation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  /* get operand type*/
  SgType* otype = createType(annot->at(0));
  /* get expression type*/
  SgType* etype = createType(annot->at(1));
  /* cast child to SgExpression* */
  SgExpression* ex = isSgExpression(child1);
  if (ex == NULL) {
    warn_msg("no expression in sizeof op");
  }
  /* create Op */
  SgSizeOfOp* op = new SgSizeOfOp(fi,ex,otype,etype);
  ROSE_ASSERT(op != NULL);
  return op;
}

/** create a SgReturnStmt*/
SgReturnStmt* 
PrologToRose::createReturnStmt(Sg_File_Info* fi, SgNode* succ,PrologCompTerm* t) {
  /* get expression*/
  SgExpression* exp = dynamic_cast<SgExpression*>(succ);
  SgReturnStmt* s  = new SgReturnStmt(fi,exp);
  if (exp != NULL) {
    //debug(exp->get_type()->class_name());
    exp->set_parent(s);
  }
  return s;
}

/**
 * create a SgBasicBlock
 */
SgBasicBlock*
PrologToRose::createBasicBlock(Sg_File_Info* fi,deque<SgNode*>* succs) {
  SgBasicBlock* b = NULL;
  deque<SgNode*>::iterator it = succs->begin();
  /*first statement comes in the constructor*/
  if(it != succs->end()) {
    b = new SgBasicBlock(fi,dynamic_cast<SgStatement*>(*it));
    it++;
    debug("adding nonempty statement");
  } else {
    return new SgBasicBlock(fi,NULL);
  }
  /*append statements*/
  while(it != succs->end()) {
    if ((*it) != (SgStatement*) 0) {
      SgStatement* stmt = dynamic_cast<SgStatement*>(*it);
      debug((*it)->class_name());
      ROSE_ASSERT(stmt);
      b->append_statement(stmt);
      (*it)->set_parent(b);
      debug("adding nonempty statement");
    } else debug("empty statement not added");
    it++;
  }
  return b;
}

/**
 * create a SgFunctionDefinition
 */
SgFunctionDefinition*
PrologToRose::createFunctionDefinition(Sg_File_Info* fi,SgNode* succ,PrologCompTerm* t) {
  /* create a basic block*/
  SgBasicBlock* b = dynamic_cast<SgBasicBlock*>(succ);
  ROSE_ASSERT(b != NULL);
  return new SgFunctionDefinition(fi,b);
}


/**
 * create SgInitializedName
 */
SgInitializedName*
PrologToRose::createInitializedName(Sg_File_Info* fi, SgNode* succ, PrologCompTerm* t) {
  /* retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  /*create the type*/
  SgType* tpe = createType(annot->at(0));
  ROSE_ASSERT(tpe != NULL);
  /* create the name*/
  SgName sgnm = *toStringP(annot->at(1));
  /* create the initializer and the initialized name*/
  SgInitializer* sgini = dynamic_cast<SgInitializer*>(succ);
  SgInitializedName* siname = new SgInitializedName(sgnm,tpe,sgini,NULL);
  ROSE_ASSERT(siname != NULL);

  siname->set_file_info(FI);

  initializedNamesWithoutScope.push_back(siname);

  return siname;
}

/**
 * create SgInitializedName from PrologCompTerm
 */
SgInitializedName*
PrologToRose::inameFromAnnot(PrologCompTerm* annot) {
  debug("creating initialized name for sg var ref exp");
  /* get type*/
  SgType* tpe = createType(annot->at(0));
  /* create name*/
  PrologAtom *nstring = dynamic_cast<PrologAtom*>(annot->at(1));
  SgName sgnm = nstring->getName().c_str();
  /* create a dummy varialbe declaration, only for the unparser to get the scope */
  //SgVariableDeclaration* vdec = new SgVariableDeclaration(
  //   FI,
  //   sgnm, tpe,  NULL);
  //ROSE_ASSERT(vdec != NULL);
  SgInitializedName* siname = new SgInitializedName(sgnm,tpe,NULL);//,vdec,NULL);
  ROSE_ASSERT(siname != NULL);
  /* the result of get_declaration() in a
   * SgInitializedName node is the first ancestor that is a SgDeclarationStatement.
   * since set_declaration() does not exist, this is the way to go
   */
  //siname->set_parent(vdec);
  //ROSE_ASSERT(siname->get_declaration() == vdec);

  siname->set_file_info(FI);

  /* static?*/
  int stat = (int) createEnum(annot->at(2), re.static_flag);
  if(stat != 0) {
    debug("setting static");
    //vdec->get_declarationModifier().get_storageModifier().setStatic();
  }
  /* create scope*/
  // PrologCompTerm* scope = isPrologCompTerm(annot->at(3));
  // if(scope != NULL) {
  //   string scope_type = scope->getName();
  //   if (scope_type == "class_scope") {
  //     debug("var ref exp class scope");
  //     string scope_name = *(toStringP(scope->at(0)));
  //     SgClassDeclaration::class_types class_type = 
  //         (SgClassDeclaration::class_types) 
  //           createEnum(scope->at(1), re.class_type);
  //     fakeClassScope(scope_name,class_type,vdec);
  //     ROSE_ASSERT(isSgClassDefinition(siname->get_declaration()->get_parent()) != NULL);
  //   } else if (scope_type == "namespace_scope") {
  //     debug("var ref exp namespace scope");
  //     string scope_name = *(toStringP(scope->at(0)));
  //     int scope_int = (toInt(scope->at(1)));
  //     fakeNamespaceScope(scope_name,scope_int,vdec);
  //     ROSE_ASSERT(isSgNamespaceDefinitionStatement(siname->get_declaration()->get_parent()) != NULL);
  //   }
  // }
  initializedNamesWithoutScope.push_back(siname);
  return siname;
}


/**
 * create SgFunctionType
 */
SgFunctionType*
PrologToRose::createFunctionType(PrologTerm* t) {
  PrologCompTerm* tterm = dynamic_cast<PrologCompTerm*>(t);
  ROSE_ASSERT(tterm->getName() == "function_type");
  /* create the reutnr type*/
  SgType* ret_type = createType(tterm->at(0));
  /* has ellipses?*/
  bool has_ellipses = (bool) createEnum(tterm->at(1), re.ellipses_flag);
  /* create type */
  SgFunctionType* func_type = new SgFunctionType(ret_type,has_ellipses);
  ROSE_ASSERT(func_type != NULL);
  /* add argument list*/
  PrologList* atypes = isPrologList(tterm->at(2));
  ROSE_ASSERT(atypes != NULL);
  for(int i = 0; i < atypes->getArity(); i++) {
    func_type->append_argument(createType(atypes->at(i)));
  }
  return func_type;
}

/**
 * create SgMemberFunctionType
 */
SgMemberFunctionType*
PrologToRose::createMemberFunctionType(PrologTerm* t) {
  PrologCompTerm* tterm = dynamic_cast<PrologCompTerm*>(t);
  /* guard -- only simple types implemented yet */
  ROSE_ASSERT(tterm->getName() == "member_function_type");
  /* create the reutnr type*/
  SgType* ret_type = createType(tterm->at(0));
  /* has ellipses?*/
  bool has_ellipses = (bool) createEnum(tterm->at(1), re.ellipses_flag);
  /* mfunc_specifier*/
  int mfuncs = createEnum(tterm->at(3), re.declaration_modifier);
  /*create type
   * note thate no SgMemberFunctionDefinition is given
   * this is because the unparser does not use it!
   */
  SgMemberFunctionType* func_type = new SgMemberFunctionType(ret_type,has_ellipses,NULL,mfuncs);
  ROSE_ASSERT(func_type != NULL);
  /* add argument list*/
  PrologList* atypes = isPrologList(tterm->at(2));
  ROSE_ASSERT(atypes != NULL);
  for(int i = 0; i < atypes->getArity(); i++) {
    func_type->append_argument(createType(atypes->at(i)));
  }
  return func_type;
}


/**
 * create a SgFunctionParameterList
 */
SgFunctionParameterList*
PrologToRose::createFunctionParameterList(Sg_File_Info* fi,deque<SgNode*>* succs) {
  debug("function parameter list");
  /* create list*/
  SgFunctionParameterList* l = new SgFunctionParameterList(fi);
  /*append successors*/
  deque<SgNode*>::iterator it = succs->begin();
  while(it != succs->end()) {
    if((*it) != NULL) {
      l->append_arg(dynamic_cast<SgInitializedName*>(*it));
      (*it)->set_parent(l);
    }
    it++;
  }
  return l;
}

/**
 * create SgFunctionDeclaration
 */
SgFunctionDeclaration*
PrologToRose::createFunctionDeclaration(Sg_File_Info* fi, SgNode* par_list_u,SgNode* func_def_u, PrologCompTerm* t) {
  debug("function declaration:");
  /* cast parameter list and function definition (if exists)*/
  SgFunctionParameterList* par_list = isSgFunctionParameterList(par_list_u);
  SgFunctionDefinition* func_def = isSgFunctionDefinition(func_def_u);
  /* param list must exist*/
  ROSE_ASSERT(par_list != NULL);
  /* get annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  /* create type*/
  SgFunctionType* func_type = isSgFunctionType(createType(annot->at(0)));
  ROSE_ASSERT(func_type != NULL);
  /* get functioon name*/
  PrologAtom* func_name_term = dynamic_cast<PrologAtom*>(annot->at(1));
  ROSE_ASSERT(func_name_term != NULL);
  SgName func_name = func_name_term->getName();
  /* create declaration*/
  SgFunctionDeclaration* func_decl =
    new SgFunctionDeclaration(fi,func_name,func_type,func_def);
  ROSE_ASSERT(func_decl != NULL); 
  func_decl->set_forward(func_def == NULL);
  func_decl->set_parameterList(par_list);
  setDeclarationModifier(annot->at(2),&(func_decl->get_declarationModifier()));

  // Fake the scope for function declaration statements
  //fakeParentScope(func_decl);
  declarationStatementsWithoutScope.push_back(func_decl);

  /*post processing*/
  /*important: otherwise unparsing fails*/
  if (func_def != NULL) {
    func_def->set_declaration(func_decl);
    /* set defining declaration ROSE 0.9.0b */
    func_decl->set_definingDeclaration(func_decl);
    // func_decl->set_firstNondefiningDeclaration(func_decl);

    //SgFunctionDeclaration* nondef_decl = 
      //  new SgFunctionDeclaration(FI,func_name,func_type,NULL);
    //nondef_decl->set_endOfConstruct(FI);

    // Set the internal reference to the non-defining declaration
    //nondef_decl->set_firstNondefiningDeclaration(nondef_decl);
    //nondef_decl->set_definingDeclaration(func_decl);
    //nondef_decl->setForward();
    //fakeParentScope(nondef_decl);
    //cerr<<endl<<func_decl<<" -- "<<nondef_decl<<endl;
    //func_decl->set_firstNondefiningDeclaration(nondef_decl);
  } else {
  }

  /* register the function declaration with our own symbol table */
  string id = makeFunctionID(func_name, annot->at(0)->getRepresentation());
  if (funcDeclMap.find(id) == funcDeclMap.end()) {
    funcDeclMap[id] = func_decl;
  }
  return func_decl;
}


/**
 * create SgMemberFunctionDeclaration
 */
SgMemberFunctionDeclaration*
PrologToRose::createMemberFunctionDeclaration(Sg_File_Info* fi, SgNode* par_list_u,SgNode* func_def_u, SgNode* ctor_list_u, PrologCompTerm* t) {
  debug("member function declaration:");
  /* cast parameter list and function definition (if exists)*/
  SgFunctionParameterList* par_list = dynamic_cast<SgFunctionParameterList*>(par_list_u);
  SgFunctionDefinition* func_def = dynamic_cast<SgFunctionDefinition*>(func_def_u);
  SgCtorInitializerList* ctor_list = isSgCtorInitializerList(ctor_list_u);
  /* param list must exist*/
  ROSE_ASSERT(par_list != NULL);
  /* ctor list is sometimes empty but always exists*/
  ROSE_ASSERT(ctor_list != NULL);
  /* get annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  /* create type*/
  SgFunctionType* func_type = dynamic_cast<SgFunctionType*>(createType(annot->at(0)));
  ROSE_ASSERT(func_type != NULL);
  /* get functioon name*/
  PrologAtom* func_name_term = dynamic_cast<PrologAtom*>(annot->at(1));
  ROSE_ASSERT(func_name_term != NULL);
  SgName func_name = func_name_term->getName();
  /* create declaration*/
  SgMemberFunctionDeclaration* func_decl = new SgMemberFunctionDeclaration(fi,func_name,func_type,func_def);
  ROSE_ASSERT(func_decl != NULL);
  /** if there's no declaration we're dealing with a forward declatation*/
  func_decl->set_forward(func_def == NULL);
  func_decl->set_parameterList(par_list);
  func_decl->set_CtorInitializerList(ctor_list);
  /*post processing*/
  /* fake class scope*/
  PrologCompTerm* scopeTerm = isPrologCompTerm(annot->at(2));
  ROSE_ASSERT(scopeTerm != NULL);
  string scope_name = *(toStringP(scopeTerm->at(0)));
  int scope_type  = toInt(scopeTerm->at(1));
  fakeClassScope(scope_name,scope_type,func_decl);

  /*important: otherwise unparsing fails*/
  if (func_def != NULL) {
    func_def->set_declaration(func_decl);
  }
  ROSE_ASSERT(isSgClassDefinition(func_decl->get_class_scope()) != NULL);
  ROSE_ASSERT(func_decl->get_class_scope()->get_declaration() != NULL);
  func_decl->set_isModified(false);
  debug(func_decl->get_qualified_name().getString());

  /*set declaration modifier*/
  setDeclarationModifier(annot->at(3),&func_decl->get_declarationModifier());

  return func_decl;
}

/**
 * Retrieve Annotation Term and cast to PrologCompTerm* from a node term
 */
PrologCompTerm*
PrologToRose::retrieveAnnotation(PrologCompTerm* t) {
  string tname = t->getName();
  PrologCompTerm* a = NULL;
  /* the position of the annotation depends on the
   * arity of the term*/
  ROSE_ASSERT(a = dynamic_cast<PrologCompTerm*>(t->at(t->getArity()-3)));
  return a;
}

/**
 * create a SgGlobal node
 * and convert content
 */
SgGlobal*
PrologToRose::createGlobal(Sg_File_Info* fi,deque<SgNode*>* succs) {
  /*simple constructor*/
  SgGlobal* glob = new SgGlobal(fi);
  glob->set_endOfConstruct(FI);
  debug("in SgGlobal:");
  testFileInfo(fi);
  /*add successors*/
  deque<SgNode*>::iterator it = succs->begin();
  SgDeclarationStatement* curdec = NULL;
  while(it != succs->end()) {
    /* all successors that arent NULL (we don't create SgNullStatements, instead
     * we return NULL ptrs*/
    if((*it) != NULL) {
      curdec = dynamic_cast<SgDeclarationStatement*>(*it);
      ROSE_ASSERT(curdec != NULL);
      glob->append_declaration(curdec);
      /* set scope if there is none yet
       * as this is essential for unparsing */
      if(curdec->get_scope() == NULL) {
	curdec->set_scope(glob);
      }
      addSymbol(glob, curdec); // ROSE 0.9.0b (Adrian)
      debug ("added declaration of type " + (*it)->class_name());
    }
    it++;
  }
  return glob;
}

/** create SgVarRefExp*/
SgVarRefExp* 
PrologToRose::createVarRefExp(Sg_File_Info* fi, PrologCompTerm* t) {
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  /* cast SgInitializedName*/
  SgInitializedName* init_name = inameFromAnnot(annot);
  ROSE_ASSERT(init_name != NULL);
  /*cast variable symbol*/
  SgVariableSymbol* var_sym = new SgVariableSymbol(init_name);
  ROSE_ASSERT(var_sym != NULL);
  /*create VarRefExp*/
  SgVarRefExp* vre = new SgVarRefExp(fi,var_sym);
  ROSE_ASSERT(vre != NULL);
  // Set parent pointers
  init_name->set_parent(var_sym);
  var_sym->set_parent(vre);
  return vre;
}

/**
 * create a SgAssignInitializer
 */
SgAssignInitializer*
PrologToRose::createAssignInitializer(Sg_File_Info* fi, SgNode* succ, PrologCompTerm* t) {
  SgExpression* exp = dynamic_cast<SgExpression*>(succ);
  // not true.. ROSE_ASSERT(exp != NULL);
  SgAssignInitializer* ai = new SgAssignInitializer(fi,exp);
  ROSE_ASSERT(ai != NULL);
  return ai;
}

/**
 * create a SgBinaryOp
 */
SgBinaryOp*
PrologToRose::createBinaryOp(Sg_File_Info* fi,SgNode* lnode,SgNode* rnode,PrologCompTerm* t) {
  debug("creating binary op");
  PrologAtom* name_atom = dynamic_cast<PrologAtom*>(t->at(0));
  ROSE_ASSERT(name_atom != NULL);
  string op_name = name_atom->getName();
  debug("op type: " + op_name);
  if (op_name == "lshift_assign_op") {
    debug("op type: " + op_name);
  }
  /*get lhs and rhs operand*/
  SgExpression* lhs = dynamic_cast<SgExpression*>(lnode);
  SgExpression* rhs = dynamic_cast<SgExpression*>(rnode);
  abort_unless((lhs != NULL) && (rhs != NULL), "Operands of binary operator must be an expression");
  PrologCompTerm* annot = retrieveAnnotation(t);
  abort_unless(annot != NULL, "Could not retrieve annotation for binary operator");
  /* get type*/
  SgType* op_type = createType(annot->at(0));
  /* depending on the type, call constructor*/
  SgBinaryOp* cur_op = NULL;
  if (op_name == SG_PREFIX "arrow_exp") {
    cur_op = new SgArrowExp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "dot_exp") {
    cur_op = new SgDotExp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "dot_star_op") {
    cur_op = new SgDotStarOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "arrow_star_op") {
    cur_op = new SgArrowStarOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "equality_op") {
    cur_op = new SgEqualityOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "less_than_op") {
    cur_op = new SgLessThanOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "greater_than_op") {
    cur_op = new SgGreaterThanOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "not_equal_op") {
    cur_op = new SgNotEqualOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "less_or_equal_op") {
    cur_op = new SgLessOrEqualOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "greater_or_equal_op") {
    cur_op = new SgGreaterOrEqualOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "add_op") {
    cur_op = new SgAddOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "subtract_op") {
    cur_op = new SgSubtractOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "multiply_op") {
    cur_op = new SgMultiplyOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "divide_op") {
    cur_op = new SgDivideOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "integer_divide_op") {
    cur_op = new SgIntegerDivideOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "mod_op") {
    cur_op = new SgModOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "and_op") {
    cur_op = new SgAndOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "or_op") {
    cur_op = new SgOrOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "bit_xor_op") {
    cur_op = new SgBitXorOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "bit_and_op") {
    cur_op = new SgBitAndOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "bit_or_op") {
    cur_op = new SgBitOrOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "comma_op_exp") {
    cur_op = new SgCommaOpExp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "lshift_op") {
    cur_op = new SgLshiftOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "rshift_op") {
    cur_op = new SgRshiftOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "pntr_arr_ref_exp") {
    cur_op = new SgPntrArrRefExp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "scope_op") {
    cur_op = new SgScopeOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "assign_op") {
    cur_op = new SgAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "plus_assign_op") {
    cur_op = new SgPlusAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "minus_assign_op") {
    cur_op = new SgMinusAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "and_assign_op") {
    cur_op = new SgAndAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "ior_assign_op") {
    cur_op = new SgIorAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "mult_assign_op") {
    cur_op = new SgMultAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "div_assign_op") {
    cur_op = new SgDivAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "mod_assign_op") {
    cur_op = new SgModAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "xor_assign_op") {
    cur_op = new SgXorAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "lshift_assign_op") {
    cur_op = new SgLshiftAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == SG_PREFIX "rshift_assign_op") {
    cur_op = new SgRshiftAssignOp(fi,lhs,rhs,op_type);
  }
  ROSE_ASSERT(cur_op != NULL);
  debug("created binary op");
  return cur_op;
}

/**
 * is this string a binary op name?
 */
bool
PrologToRose::isBinaryOp(string tname) {
  if (tname == SG_PREFIX "arrow_exp") {
    return true;
  } else if (tname == SG_PREFIX "dot_exp") {
    return true;
  } else if (tname == SG_PREFIX "dot_star_op") {
    return true;
  } else if (tname == SG_PREFIX "arrow_star_op") {
    return true;
  } else if (tname == SG_PREFIX "equality_op") {
    return true;
  } else if (tname == SG_PREFIX "less_than_op") {
    return true;
  } else if (tname == SG_PREFIX "greater_than_op") {
    return true;
  } else if (tname == SG_PREFIX "not_equal_op") {
    return true;
  } else if (tname == SG_PREFIX "less_or_equal_op") {
    return true;
  } else if (tname == SG_PREFIX "greater_or_equal_op") {
    return true;
  } else if (tname == SG_PREFIX "add_op") {
    return true;
  } else if (tname == SG_PREFIX "subtract_op") {
    return true;
  } else if (tname == SG_PREFIX "multiply_op") {
    return true;
  } else if (tname == SG_PREFIX "divide_op") {
    return true;
  } else if (tname == SG_PREFIX "integer_divide_op") {
    return true;
  } else if (tname == SG_PREFIX "mod_op") {
    return true;
  } else if (tname == SG_PREFIX "and_op") {
    return true;
  } else if (tname == SG_PREFIX "or_op") {
    return true;
  } else if (tname == SG_PREFIX "bit_xor_op") {
    return true;
  } else if (tname == SG_PREFIX "bit_and_op") {
    return true;
  } else if (tname == SG_PREFIX "bit_or_op") {
    return true;
  } else if (tname == SG_PREFIX "comma_op_exp") {
    return true;
  } else if (tname == SG_PREFIX "lshift_op") {
    return true;
  } else if (tname == SG_PREFIX "rshift_op") {
    return true;
  } else if (tname == SG_PREFIX "pntr_arr_ref_exp") {
    return true;
  } else if (tname == SG_PREFIX "scope_op") {
    return true;
  } else if (tname == SG_PREFIX "assign_op") {
    return true;
  } else if (tname == SG_PREFIX "plus_assign_op") {
    return true;
  } else if (tname == SG_PREFIX "minus_assign_op") {
    return true;
  } else if (tname == SG_PREFIX "and_assign_op") {
    return true;
  } else if (tname == SG_PREFIX "ior_assign_op") {
    return true;
  } else if (tname == SG_PREFIX "mult_assign_op") {
    return true;
  } else if (tname == SG_PREFIX "div_assign_op") {
    return true;
  } else if (tname == SG_PREFIX "mod_assign_op") {
    return true;
  } else if (tname == SG_PREFIX "xor_assign_op") {
    return true;
  } else if (tname == SG_PREFIX "lshift_assign_op") {
    return true;
  } else if (tname == SG_PREFIX "rshift_assign_op") {
    return true;
  } else if (tname == SG_PREFIX "arrow_exp") {
    return true;
  } else return false;
}

/**
 * create SgExprStatement
 */
SgExprStatement*
PrologToRose::createExprStatement(Sg_File_Info* fi, SgNode* succ, PrologCompTerm* t) {
  /* create statement*/
  SgExprStatement* es = NULL;
  debug("creating expression statenemt");
  if (SgExpressionRoot* er = dynamic_cast<SgExpressionRoot*>(succ)) {
    /*either with an expression statment as successor*/
    es = new SgExprStatement(fi,er);
  } else if (SgExpression* exp = dynamic_cast<SgExpression*>(succ)) {
    /*...or with an expression*/
    es = new SgExprStatement(fi,exp);
  }
  ROSE_ASSERT(es != NULL);
  debug("created expression statement");
  return es;
}

/**
 * create a SgVariableDeclaration
 */
SgVariableDeclaration*
PrologToRose::createVariableDeclaration(Sg_File_Info* fi,deque<SgNode*>* succs,PrologCompTerm* t) {
  /*extract annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(t != NULL);
  /* create declaration*/
  SgVariableDeclaration* dec = new SgVariableDeclaration(fi);
  ROSE_ASSERT(fi != NULL);
  debug("created variable declaration");

  /* add initialized names*/
  SgInitializer* ini_initializer;
  SgClassDeclaration* class_decl = 0;

  deque<SgNode*>::iterator it = succs->begin();
  ROSE_ASSERT(it != succs->end());

  // Struct declaration and definition in one?
  for (; it != succs->end(); ++it) {
    // first extract the definition
    if (SgClassDeclaration* cdecl = isSgClassDeclaration(*it)) {
      ROSE_ASSERT(class_decl == NULL);
      class_decl = cdecl;
      debug("added class/struct");
      //fakeParentScope(class_decl);
      // default to self for now...
      class_decl->unsetForward();
      class_decl->set_definingDeclaration(class_decl);
      // register with vardecl
      class_decl->set_parent(dec);
      dec->set_baseTypeDefiningDeclaration(class_decl);
    }
  }

  // now add the initialized names
  for (it = succs->begin(); it != succs->end(); ++it) {
    if (SgInitializedName* ini_name = isSgInitializedName(*it)) {
      debug("added variable");
      ini_name->set_declptr(dec);
      ini_initializer = ini_name->get_initializer();

      if (class_decl) {
	// If this is a Definition as well, insert the pointer to the
	// declaration
	SgClassType* ct = isSgClassType(ini_name->get_typeptr());
	ROSE_ASSERT(ct);
	ct->set_declaration(class_decl);
      }

      dec->append_variable(ini_name,ini_initializer);
    } else if (dynamic_cast<SgClassDeclaration*>(*it)) {
      /* see above */
    } else {
      cerr << (*it)->class_name() << "???" << endl; 
      ROSE_ASSERT(false);
    }
  }
  /* set declaration modifier*/
  setDeclarationModifier(annot->at(0),&(dec->get_declarationModifier()));

  return dec;
}

/**
 * create SgIfStmt
 */
SgIfStmt* 
PrologToRose::createIfStmt(Sg_File_Info* fi, SgNode* child1, SgNode* child2, SgNode* child3, PrologCompTerm* t) {
  /* condition*/
  /* GB (2008-08-21): True and false bodies are now SgStatements, not
   * SgBasicBlocks anymore; changed dynamic_casts to less verbose and more
   * idiomatic is... calls. */
  SgStatement* test_stmt = isSgStatement(child1);
  ROSE_ASSERT(test_stmt != NULL);
  /* if-branch*/
  SgStatement* true_branch = isSgStatement(child2);
  ROSE_ASSERT(true_branch != NULL);
  /* else branch*/
  SgStatement* false_branch = isSgStatement(child3);
  SgIfStmt* if_stmt = NULL;
  /* create statement*/
  if_stmt = new SgIfStmt(fi,test_stmt,true_branch,false_branch);
  ROSE_ASSERT(if_stmt != NULL);
  return if_stmt;
}

/** create SgDoWhileStmt*/
SgDoWhileStmt*
PrologToRose::createDoWhileStmt(Sg_File_Info* fi, SgNode* child1, SgNode* child2,PrologCompTerm* t) {
  /* retrieve basic block -- there is always one*/
  SgStatement* b = isSgStatement(child1);
  /* retrieve statement */
  SgStatement* s = isSgStatement(child2);
  ROSE_ASSERT(s != NULL);
  /* create do/while*/
  SgDoWhileStmt* d = new SgDoWhileStmt(fi,b,s);
  ROSE_ASSERT(d != NULL);
  return d;
}

/** create SgWhileStmt*/
SgWhileStmt*
PrologToRose::createWhileStmt(Sg_File_Info* fi, SgNode* child1, SgNode* child2, PrologCompTerm* t) {
  /* retrieve Statemtent*/
  SgStatement* s = isSgStatement(child1);
  ROSE_ASSERT(s != NULL);
  /* retrieve basic block*/
  SgStatement* b = isSgStatement(child2);
  /*create while statement*/
  SgWhileStmt* w = new SgWhileStmt(fi,s,b);
  ROSE_ASSERT(w != NULL);
  return w;
}

/** create SgForInitStatement*/
SgForInitStatement*
PrologToRose::createForInitStatement(Sg_File_Info* fi,deque<SgNode*>* succs) {
  SgForInitStatement* ini = new SgForInitStatement(fi);
  ROSE_ASSERT(ini != NULL);
  /*append initializer statements*/
  deque<SgNode*>::iterator it = succs->begin();
  while(it != succs->end()) {
    SgStatement* stmt = NULL;
    stmt = dynamic_cast<SgStatement*>(*it);
    ROSE_ASSERT(stmt != NULL);
    ini->append_init_stmt(stmt);
    stmt->set_parent(ini);
    it++;
  }
  return ini;
}

/** create SgForStatement*/
SgForStatement*
PrologToRose::createForStatement(Sg_File_Info* fi, SgNode* child1, SgNode* child2, SgNode* child3, SgNode* child4,PrologCompTerm* t) {
  SgForInitStatement* ini_stmt = dynamic_cast<SgForInitStatement*>(child1);
  SgStatement* test_stmt = dynamic_cast<SgStatement*>(child2);
  SgStatement* loop_body = dynamic_cast<SgStatement*>(child4);
  SgForStatement* f = NULL;
  //std::cerr<<ini_stmt<<std::endl;
  if (SgExpression* e = dynamic_cast<SgExpression*>(child3)) {
    f = new SgForStatement(fi,test_stmt,e,loop_body);
  } else if (SgExpression* e = dynamic_cast<SgExpression*>(child3)) {
    f = new SgForStatement(fi,test_stmt,e,loop_body);
  } else if (SgExprStatement* s = dynamic_cast<SgExprStatement*>(child3)) {
    f = new SgForStatement(fi,test_stmt,s->get_expression(),loop_body);
  }
  ROSE_ASSERT(f != NULL);
  SgStatementPtrList::iterator it = ini_stmt->get_init_stmt().begin();
  while(it != ini_stmt->get_init_stmt().end()) {
    f->append_init_stmt(*it);
    it++;
  }
  //f->append_init_stmt(ini_stmt);
  return f;
}

/**
 * create SgSwitchStatement
 */
SgSwitchStatement*
PrologToRose::createSwitchStatement(Sg_File_Info* fi, SgNode* child1, SgNode* child2, PrologCompTerm* t) {
  SgStatement* switch_expr = NULL;
  SgBasicBlock* switch_block = NULL;
  SgSwitchStatement* s = NULL; 
  /* the first child, the switch selector must be a SgStatement*/
  switch_expr = isSgStatement(child1);
  ROSE_ASSERT(switch_expr != NULL);
  /* the second child, the switch code, must be a SgBasicBlock*/
  switch_block = isSgBasicBlock(child2);
  ROSE_ASSERT(switch_block != NULL);
  /* make sure a SgSwitchStatement is created*/
  s = new SgSwitchStatement(fi,switch_expr,switch_block);
  ROSE_ASSERT(s != NULL);
  ROSE_ASSERT(s->get_file_info() != NULL);
  return s;
}

/**
 * create SgCaseOptionStmt
 */
SgCaseOptionStmt*
PrologToRose::createCaseOptionStmt(Sg_File_Info* fi, SgNode* child1, SgNode* child2, SgNode* child3, PrologCompTerm* t) {
  SgCaseOptionStmt* case_stmt = NULL;
  SgStatement* case_block = NULL;
  /* second child must be a SgStatement*/
  abort_unless(
	       case_block = isSgStatement(child2),
	       "Could not retrieve body of case statement"
	       );
  /* first child must be either a SgExpressionRoot or a SgExpression*/
  if(SgExpressionRoot* er = isSgExpressionRoot(child1)) {
    case_stmt = new SgCaseOptionStmt(fi,er,case_block);
  } else {
    SgExpression *e = NULL;
    abort_unless(
		 e = isSgExpression(child1),
		 "Could not retrieve selector value of case statement"
		 );
    case_stmt = new SgCaseOptionStmt(fi,e,case_block);
  }
  /* make sure a SgCaseOptionStmt has been created*/
  abort_unless(
	       case_stmt != NULL,
	       "Could not create case option statement"
	       );
  ROSE_ASSERT(case_stmt->get_file_info() != NULL);
  return case_stmt;
}

/**
 * create SgDefaultOptionStmt
 */
SgDefaultOptionStmt*
PrologToRose::createDefaultOptionStmt(Sg_File_Info* fi, SgNode* child1, PrologCompTerm* t) {
  SgDefaultOptionStmt* default_stmt = NULL;
  SgStatement* b = NULL;
  /*make sure the body is actually one.*/
  abort_unless(
	       b = isSgStatement(child1),
	       "Body of default option is not a basic block"
	       );
  /* make sure the statement is actually created */
  abort_unless(
	       default_stmt = new SgDefaultOptionStmt(fi,b),
	       "Failed to create default option statement"
	       );
  ROSE_ASSERT(default_stmt->get_file_info() != NULL);
  return default_stmt;
}

/**create a SgBreakStmt*/
SgBreakStmt*
PrologToRose::createBreakStmt(Sg_File_Info* fi, PrologCompTerm* t) {
  SgBreakStmt* b = NULL;
  b = new SgBreakStmt(fi);
  ROSE_ASSERT(b != NULL);
  return b;
}

/** create a SgContinueStmt*/
SgContinueStmt* 
PrologToRose::createContinueStmt(Sg_File_Info* fi,PrologCompTerm* t) {
  SgContinueStmt* s = new SgContinueStmt(fi);
  ROSE_ASSERT(s != NULL);
  return s;
}

/** create a SgLabelStatement for gotos */
SgLabelStatement*
PrologToRose::createLabelStatement(Sg_File_Info* fi,PrologCompTerm* t) {
  /* get annotation data*/
  PrologCompTerm* u = retrieveAnnotation(t);
  ROSE_ASSERT(u != NULL);
  /* extract the label name*/
  PrologAtom* s = dynamic_cast<PrologAtom*>(u->at(0));
  ROSE_ASSERT(s != NULL);
  /* create SgLabelStatement with helper function*/
  /* makeLabel alreay asserts a non-NULL return value*/
  return makeLabel(fi,s->getName());
}

/** create a SgGotoStmt */
SgGotoStatement*
PrologToRose::createGotoStatement(Sg_File_Info* fi,PrologCompTerm* t) {
  /* get annotation data*/
  PrologCompTerm* u = retrieveAnnotation(t);
  ROSE_ASSERT(u != NULL);
  /* extract the label name*/
  PrologAtom* s = dynamic_cast<PrologAtom*>(u->at(0));
  ROSE_ASSERT(s != NULL);
  /* create dummy SgLabelStatement with helper function*/
  Sg_File_Info* fi2 = FI;
  SgLabelStatement* l = makeLabel(fi2,s->getName());
  /*makeLabel asserts a non-NULL return value*/
  /* create goto using dummy label*/
  SgGotoStatement* sgoto = new SgGotoStatement(fi,l);
  ROSE_ASSERT(sgoto != NULL);
  return sgoto;
}

/** create a SgLabelStatement from a string*/
SgLabelStatement*
PrologToRose::makeLabel(Sg_File_Info* fi,string s) {
  /* we need a SgName*/
  SgName n = s;
  SgLabelStatement* l = new SgLabelStatement(fi,n);
  ROSE_ASSERT(l != NULL);
  return l;
}

/** create a class definition*/
SgClassDefinition*
PrologToRose::createClassDefinition(Sg_File_Info* fi, deque<SgNode*>* succs,PrologCompTerm* t) {
  /*the unparser needs a Sg_File_Info for determining the end of construct
   * hence it is put in the annotation and retrieved here */
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  PrologCompTerm* fi_term = dynamic_cast<PrologCompTerm*>(annot->at(0));
  ROSE_ASSERT(fi_term != NULL);	
  Sg_File_Info* end_of_construct = createFileInfo(fi_term);
  ROSE_ASSERT(end_of_construct != NULL);
	
  SgClassDefinition* d = NULL;
  /*create empty class definition*/
  d = new SgClassDefinition(fi);
  ROSE_ASSERT(d != NULL);
  /* append declarations*/
  deque<SgNode*>::iterator it = succs->begin();
  while(it != succs->end()) {
    SgDeclarationStatement* s = NULL;
    s = isSgDeclarationStatement(*it);
    ROSE_ASSERT(s != NULL);
    d->append_member(s);
    s->set_parent(d);
    it++;
  }
  /* set the end of construct*/
  d->set_endOfConstruct(end_of_construct);

  classDefinitions.push_back(d);
  return d;
}


/**
 * create SgClassDeclaration
 */
SgClassDeclaration*
PrologToRose::createClassDeclaration(Sg_File_Info* fi,SgNode* child1 ,PrologCompTerm* t) {
  /* retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  /* if there is a child, it is a definition*/
  SgClassDefinition* class_def = isSgClassDefinition(child1);
  /*retrieve name, class type and type*/
  PrologAtom* class_name_s = dynamic_cast<PrologAtom*>(annot->at(0));
  ROSE_ASSERT(class_name_s != NULL);
  /* get the class_type-enum (struct,class) -- not the type */
  PrologAtom* class_type = dynamic_cast<PrologAtom*>(annot->at(1));
  ROSE_ASSERT(class_type != NULL);
  /* get the type*/
  PrologCompTerm* type_s = dynamic_cast<PrologCompTerm*>(annot->at(2));
  ROSE_ASSERT(type_s != NULL);
  SgClassDeclaration::class_types e_classtype = 
    (SgClassDeclaration::class_types)createEnum(class_type, re.class_type);
  //SgClassType* sg_class_type = createClassType(type_s);
  SgName class_name = class_name_s->getName();
  SgClassDeclaration* d = 
    new SgClassDeclaration(fi, class_name, e_classtype, NULL /*sg_class_type */,
			   class_def);
  ROSE_ASSERT(d != NULL);
  SgClassType* sg_class_type = SgClassType::createType(d);
  d->set_type(sg_class_type);
  classTypeMap[type_s->getRepresentation()] = sg_class_type;

  /* set declaration or the forward flag*/
  if(class_def != NULL) {
    d->set_forward(0);
    d->set_definingDeclaration(d);
    class_def->set_declaration(d);

    SgClassDeclaration* ndcd = 
      new SgClassDeclaration(FI, class_name,e_classtype, NULL, class_def);
    ndcd->set_endOfConstruct(FI);

    // Set the internal reference to the non-defining declaration
    ndcd->set_firstNondefiningDeclaration(ndcd);
    ndcd->set_definingDeclaration(d);
    ndcd->setForward();
    d->set_firstNondefiningDeclaration(ndcd);

#if 0
    definingClassDecls[annot->getRepresentation()] = d;
#endif
    declarationStatementsWithoutScope.push_back(ndcd);
  } else {
    d->set_forward(1);
#if 0
    nondefiningClassDecls.insert(pair<string, SgClassDeclaration*>(annot->getRepresentation(), d));
#endif
  }
  declarationStatementsWithoutScope.push_back(d);

  return d;
}

/** create dummy class scope and add a declaration*/
void
PrologToRose::fakeClassScope(string s, int c_type,SgDeclarationStatement* stat) {
  ROSE_ASSERT(false);
  /*create a dummy class declaration*/
  SgClassDeclaration* d = createDummyClassDeclaration(s,c_type);
  SgClassDefinition* def = new SgClassDefinition(
    FI, d);
  d->set_parent(def);
  // FIXME
  def->set_parent(d);
  ROSE_ASSERT(def != NULL);
  /* scope is changed here as a side effect!*/
  def->append_member(stat);
  if (SgVariableDeclaration* vd = isSgVariableDeclaration(stat)) {
    debug("var ref exp class scope added");
    vd->set_parent(def);
  } else {
    stat->set_scope(def);
  }
}

/** create dummy namespace scope*/
void
PrologToRose::fakeNamespaceScope(string s, int unnamed, SgDeclarationStatement* stat) {
  ROSE_ASSERT(false);
  SgName n = s;
  bool u_b = (bool) unnamed;
  SgNamespaceDefinitionStatement* def =
    new SgNamespaceDefinitionStatement(FI,0);
  /* set scope*/
  def->append_declaration(stat);
  if (SgVariableDeclaration* vd = isSgVariableDeclaration(stat)) {
    debug("var ref exp namespace scope added");
    vd->set_parent(def);
  } else {
    stat->set_scope(def);
  }
  /* create namespace*/
  SgNamespaceDeclarationStatement* dec = 
    new SgNamespaceDeclarationStatement(FI,n,def,u_b);
  def->set_namespaceDeclaration(dec); // AP 4.2.2008 
  if(def != NULL) {
    def->set_namespaceDeclaration(dec);
    dec->set_forward(false);
    dec->set_definingDeclaration(dec);
  }
  ROSE_ASSERT(dec != NULL);
  //fakeParentScope(dec); //Adrian 1.2.2008 remove a ROSE 0.9.0b warning 
  SgGlobal* dummy = new SgGlobal(FI);
  ROSE_ASSERT(dummy != NULL);
  dec->set_parent(dummy);
}
	

/** create dummy class declaration from name*/
SgClassDeclaration*
PrologToRose::createDummyClassDeclaration(string s,int c_type) {
  ROSE_ASSERT(false);

  Sg_File_Info* fi = Sg_File_Info::generateDefaultFileInfo();
  SgName class_name = s;
  SgClassDeclaration* d = new SgClassDeclaration(fi,class_name,(SgClassDeclaration::class_types)c_type,NULL,NULL);
  ROSE_ASSERT(d != NULL);
  //fakeParentScope(d);
  declarationStatementsWithoutScope.push_back(d);
  return d;
}

/** create dummy member function declaration class from name*/
SgMemberFunctionDeclaration*
PrologToRose::createDummyMemberFunctionDeclaration(string s,int c_type) {
  ROSE_ASSERT(false);
  /*    TODO
	Sg_File_Info* fi = Sg_File_Info::generateDefaultFileInfo();
	SgName class_name = s;
	SgClassDeclaration* d = new SgClassDeclaration(fi,class_name,c_type,NULL,NULL);
	ROSE_ASSERT(d != NULL);
	fakeParentScope(d);
	return d;
  */
  return 0;
}

/** create SgClassType from annotation */
SgClassType*
PrologToRose::createClassType(PrologTerm* p) {
  SgClassType* ct;
  if (classTypeMap.find(p->getRepresentation()) != classTypeMap.end())
    // Lookup the canonical class type
    ct = classTypeMap[p->getRepresentation()];
  else {
    // /* must be a composite term*/
    // PrologCompTerm* t = isPrologCompTerm(p);
    // ROSE_ASSERT(t != NULL);
    // /* first term is class name*/
    // string s = *toStringP(t->at(0));
    // /* create dummy declaration*/
    // SgClassDeclaration* d = 
    //   createDummyClassDeclaration(s, createEnum(t->at(1), re.class_type));
    // ROSE_ASSERT(d != NULL);
    // string scopename = *(toStringP(t->at(2)));
    // if(scopename != "::") {
    //   fakeNamespaceScope(scopename,0,d);
    // } else {
    //   //fakeParentScope(d);
    //   declarationStatementsWithoutScope.push_back(d);
    // }
    // /* the unparser wants this*/
    // /*SgClassDefinition* classdef = new SgClassDefinition();
    //   d->set_definition(classdef);*/
    // d->set_definingDeclaration(d);
	
    // d->set_forward(true);
    // ct = SgClassType::createType(d);
    // ROSE_ASSERT(ct != NULL);
    // d->set_parent(ct);
    ct = NULL;
    ROSE_ASSERT(ct != NULL);
  }
  return ct;
}

/**Create SgCtorInitializerList*/
SgCtorInitializerList* 
PrologToRose::createCtorInitializerList(Sg_File_Info* fi,deque<SgNode*>* succs) {
  //this is a typical list node. only needs file info in constructor
  SgCtorInitializerList* l = new SgCtorInitializerList(fi);
  ROSE_ASSERT(l != NULL);
  //append constructor initializers
  deque<SgNode*>::iterator it = succs->begin();
  while(it != succs->end()) {
    SgInitializedName* n = NULL;
    n = isSgInitializedName(*it);
    ROSE_ASSERT(n != NULL);
    l->append_ctor_initializer(n);
    it++;
  }
  return l;
}


/**
 * if condition is not met, output message and exit with failure*/
void
PrologToRose::abort_unless(bool condition,string message) {
  if (condition) return;
  cerr << "\nFatal error while transforming Prolog to ROSE AST:\n"<< message << "\n";
  assert(condition);
}

/** create bit deque from PrologList*/
SgBitVector*
PrologToRose::createBitVector(PrologTerm* t, map<string, int> names) {
  /*cast the argument to the list and extract elements*/
  PrologList* l = dynamic_cast<PrologList*>(t);
  ROSE_ASSERT(t != 0);
  deque<PrologTerm*>* succs = l->getSuccs();
  /*create a bit vector*/
  SgBitVector* bv = new SgBitVector();
  bv->resize(names.size(), false);
  /*extract bits from list*/
  deque<PrologTerm*>::iterator it = succs->begin();
  while(it != succs->end()) {
    PrologAtom *a = dynamic_cast<PrologAtom*>(*it);
    ROSE_ASSERT(a != NULL);
    (*bv)[names[a->getName()]] = true;
    it++;
  }
  return bv;
}

/** create enum from PrologAtom */
int
PrologToRose::createEnum(PrologTerm* t, map<string, int> names) {
  return names[t->getName()];
}

/**
 * create SgEnumDeclaration
 * */
SgEnumDeclaration*
PrologToRose::createEnumDeclaration(Sg_File_Info* fi, deque<SgNode*>* succs, PrologCompTerm* t) {
  /*retrieve name*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(t != NULL);
  SgName e_name = *toStringP(annot->at(0));
  /* leave type blank for now*/
  SgEnumDeclaration* dec = new SgEnumDeclaration(fi,e_name,NULL);
  ROSE_ASSERT(dec != NULL);
  /*create a type*/
  dec->set_type(SgEnumType::createType(dec));	
  /* append enumerators (name or name/value)*/
  deque<SgNode*>::iterator it = succs->begin();
  SgInitializedName* iname;
  while(it != succs->end()) {
    iname = isSgInitializedName(*it);
    ROSE_ASSERT(iname != NULL);
    dec->append_enumerator(iname);
    it++;
  }
  /* postprocessing*/
  dec->set_embedded((bool) toInt(annot->at(2)));
  pciDeclarationStatement(dec,annot->at(1));
  dec->set_definingDeclaration(dec);
  return dec;
}

/**
 * Create SgTypedefDeclaration
 */
SgTypedefDeclaration*
PrologToRose::createTypedefDeclaration(Sg_File_Info* fi, PrologCompTerm* t) {
  debug("typedef declaration");
  /*get annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  /*create name*/
  SgName n = *(toStringP(annot->at(0)));
  /*create nested type*/
  SgType* tpe = createType(annot->at(1));
  ROSE_ASSERT(t != NULL);
  /*create definition, if there is one*/
  SgDeclarationStatement* decs = NULL;
  /* condition is true when a declaration is at this position*/
  PrologCompTerm* ct = isPrologCompTerm(annot->at(2));
  if(ct != NULL) {
    debug("...with declaration");
    decs = isSgDeclarationStatement(toRose(annot->at(2)));
    ROSE_ASSERT(decs != NULL);
    //note that the unparser seems to skip it!
  }
  /*create typedef declaration*/
	
  SgTypedefType* tdt = NULL;
  SgSymbol* smb = NULL;
  SgTypedefDeclaration* tdec = new SgTypedefDeclaration(fi,n,tpe,NULL,decs,NULL);
  ROSE_ASSERT(tdec != NULL);
  /* if there is a declaration, set flag and make sure it is set*/
  if(decs != NULL) {
    tdec->set_typedefBaseTypeContainsDefiningDeclaration(true);	

  };

  return tdec;
}

/**
 * create SgPragma
 */
SgPragma*
PrologToRose::createPragma(Sg_File_Info* fi, PrologCompTerm* t) {
  /* retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  /* create the name*/
  SgName sgnm = *toStringP(annot->at(0));
  /* create the Pragma*/
  SgPragma* p = new SgPragma(strdup(sgnm.str()), fi);
  ROSE_ASSERT(p != NULL);
  return p;
}


/**
 * Post Creation Initializiatoin of SgDeclarationStatement
 * */
void
PrologToRose::pciDeclarationStatement(SgDeclarationStatement* s,PrologTerm* t) {
  /* check wether t is a PrologCompTerm*/
  PrologCompTerm* atts = isPrologCompTerm(t);
  ROSE_ASSERT(atts != NULL);
  ROSE_ASSERT(s != NULL);
  ROSE_ASSERT(atts->getArity() == 5);
  /* set flags (subterms are ints, implicit cast to bool)*/
  s->set_nameOnly(toInt(atts->at(0)));
  s->set_forward(toInt(atts->at(1)));
  s->set_externBrace(toInt(atts->at(2)));
  s->set_skipElaborateType(toInt(atts->at(3)));
  //	s->set_need_name_qualifier(toInt(atts->at(4)));
  if (s->get_scope() == NULL) {
    //fakeParentScope(s);
    declarationStatementsWithoutScope.push_back(s);
  }
}

/**
 * the unparser wants a parent scope for every declaration
 * for dummy declarations we have to fake it.
 */
void
PrologToRose::fakeParentScope(SgDeclarationStatement* s) {
  //nothing to do if there is already a parent scope
  if(s->get_parent()) return;
  debug("faking scope");	
  SgGlobal* dummy = new SgGlobal(FI);
  ROSE_ASSERT(dummy != NULL);
  dummy->set_endOfConstruct(FI);
  // 7.2.2008 ROSE 0.9.0b (Adrian)
  addSymbol(dummy, s);
  

  s->set_parent(dummy);
  s->set_scope(dummy);
  ROSE_ASSERT(s->get_parent());
  ROSE_ASSERT(s->get_scope());
}

/**
 * the unparser now wants a symbol table entry, too
 */
void
PrologToRose::addSymbol(SgGlobal* scope, SgDeclarationStatement* s) {
  {
    SgFunctionDeclaration *decl = isSgFunctionDeclaration(s);
    if (decl) scope->insert_symbol(decl->get_name(), new SgFunctionSymbol(decl));
  }
  {
    SgClassDeclaration *decl = isSgClassDeclaration(s);
    if (decl) scope->insert_symbol(decl->get_name(), new SgClassSymbol(decl));
  }
}

/**
 * cast to PrologInt (analogous to the is... functions of ROSE)
 */
PrologInt*
PrologToRose::isPrologInt(PrologTerm* t) {
  return dynamic_cast<PrologInt*>(t);
}
/**
 * cast to PrologList (analogous to the is... functions of ROSE)
 */
PrologList*
PrologToRose::isPrologList(PrologTerm* t) {
  return dynamic_cast<PrologList*>(t);
}
/**
 * cast to PrologAtom (analogous to the is... functions of ROSE)
 */
PrologAtom*
PrologToRose::isPrologAtom(PrologTerm* t) {
  return dynamic_cast<PrologAtom*>(t);
}
/**
 * cast to PrologCompTerm (analogous to the is... functions of ROSE)
 */
PrologCompTerm*
PrologToRose::isPrologCompTerm(PrologTerm* t) {
  return dynamic_cast<PrologCompTerm*>(t);
}
/** DEPRECATED
 * cast to PrologString (analogous to the is... functions of ROSE)
 *
PrologString*
PrologToRose::isPrologString(PrologTerm* t) {
  return dynamic_cast<PrologString*>(t);
}*/

/**
 * create std::string* from PrologAtom*
 * including downcast from PrologTerm*.
 * If both casts fail, an assertion will fail;
 */
string*
PrologToRose::toStringP(PrologTerm* t) {
  if(PrologAtom* a =isPrologAtom(t)) {
    ROSE_ASSERT(a != NULL);
    return new string(a->getName());
  }
}

/**
 * create int from PrologInt*
 * including downcast from PrologTerm*
 * If cast fails, an assertion will fail
 */
int
PrologToRose::toInt(PrologTerm* t) {
  PrologInt* i = isPrologInt(t);
  ROSE_ASSERT(i);
  return i->getValue();
}

/**
 * create SgDeleteExp
 */
SgDeleteExp*
PrologToRose::createDeleteExp(Sg_File_Info* fi, SgNode* child1, PrologCompTerm* t) {
  // get annotation
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  // cast child fitting constructor	
  SgExpression* e = isSgExpression(child1);
  ROSE_ASSERT(e != NULL);
  // get "flags"	
  short is_array = toInt(annot->at(0));
  short need_g = toInt(annot->at(1));
  // create, test, return	
  SgDeleteExp* del = new SgDeleteExp(fi,e,is_array,need_g);
  ROSE_ASSERT(del != NULL);
  return del;
}

/**
 * create SgExprListExp 
 */
SgExprListExp*
PrologToRose::createExprListExp(Sg_File_Info* fi, deque<SgNode*>* succs) {
  /* just create SgExprListExp* and append expressions*/
  debug("SgExprListExp");
  SgExprListExp* e = new SgExprListExp(fi);
  ROSE_ASSERT(e != NULL);
  deque<SgNode*>::iterator it = succs->begin();
  while (it != succs->end()) {
    SgExpression* ex = NULL;
    ex = isSgExpression(*it);
    ROSE_ASSERT(ex != NULL);
    e->append_expression(ex);
    it++;
  }
  return e;
}
/**
 * create SgRefExp*
 */
SgRefExp*
PrologToRose::createRefExp(Sg_File_Info* fi, PrologCompTerm* t) {
  /* retrieve type from annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  SgType* tpe = createType(annot->at(0));
  ROSE_ASSERT(tpe != NULL);
  SgRefExp* re = new SgRefExp(fi,tpe);
  ROSE_ASSERT(re != NULL);
  return re;
}

/**
 * create SgVarArgOp
 * */
SgVarArgOp*
PrologToRose::createVarArgOp(Sg_File_Info* fi, SgNode* child1,PrologCompTerm* t) {
  /* arg is supposed to be an expression*/
  SgExpression* c1 = isSgExpression(child1);
  ROSE_ASSERT(c1 != NULL);
  /* retrieve type from annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  SgType* tpe = createType(annot->at(0));
  ROSE_ASSERT(tpe != NULL);	
  SgVarArgOp* o = new SgVarArgOp(fi,c1,tpe);
  return o;
}

/**
 * create SgVarArgEndOp
 * */
SgVarArgEndOp*
PrologToRose::createVarArgEndOp(Sg_File_Info* fi, SgNode* child1,PrologCompTerm* t) {
  /* arg is supposed to be an expression*/
  SgExpression* c1 = isSgExpression(child1);
  ROSE_ASSERT(c1 != NULL);
  /* retrieve type from annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  SgType* tpe = createType(annot->at(0));
  ROSE_ASSERT(tpe != NULL);	
  SgVarArgEndOp* o = new SgVarArgEndOp(fi,c1,tpe);
  return o;
}
/**
 * create SgVarArgStartOneOperandOp
 * */
SgVarArgStartOneOperandOp*
PrologToRose::createVarArgStartOneOperandOp(Sg_File_Info* fi, SgNode* child1,PrologCompTerm* t) {
  /* arg is supposed to be an expression*/
  SgExpression* c1 = isSgExpression(child1);
  ROSE_ASSERT(c1 != NULL);
  /* retrieve type from annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  SgType* tpe = createType(annot->at(0));
  ROSE_ASSERT(tpe != NULL);	
  SgVarArgStartOneOperandOp* o = new SgVarArgStartOneOperandOp(fi,c1,tpe);
  return o;
}
/**
 * create SgVarArgStartOp
 * */
SgVarArgStartOp*
PrologToRose::createVarArgStartOp(Sg_File_Info* fi, SgNode* child1,SgNode* child2,PrologCompTerm* t) {
  /* args are supposed to be expressions*/
  SgExpression* c1 = isSgExpression(child1);
  ROSE_ASSERT(c1 != NULL);
  SgExpression* c2 = isSgExpression(child2);
  ROSE_ASSERT(c2 != NULL);
  /* retrieve type from annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  SgType* tpe = createType(annot->at(0));
  ROSE_ASSERT(tpe != NULL);	
  SgVarArgStartOp* o = new SgVarArgStartOp(fi,c1,c2,tpe);
  return o;
}

/**
 * create SgVarArgCopyOp
 * */
SgVarArgCopyOp*
PrologToRose::createVarArgCopyOp(Sg_File_Info* fi, SgNode* child1,SgNode* child2,PrologCompTerm* t) {
  /* args are supposed to be expressions*/
  SgExpression* c1 = isSgExpression(child1);
  ROSE_ASSERT(c1 != NULL);
  SgExpression* c2 = isSgExpression(child2);
  ROSE_ASSERT(c2 != NULL);
  /* retrieve type from annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  SgType* tpe = createType(annot->at(0));
  ROSE_ASSERT(tpe != NULL);	
  SgVarArgCopyOp* o = new SgVarArgCopyOp(fi,c1,c2,tpe);
  return o;
}

/**
 * create SgAccessModifier
 */
SgAccessModifier*
PrologToRose::createAccessModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  SgAccessModifier* a = new SgAccessModifier();
  ROSE_ASSERT(a != NULL);
  a->set_modifier((SgAccessModifier::access_modifier_enum) 
		  createEnum(c->at(0), re.access_modifier));
  return a;
}
/**
 * create SgBaseClassModifier
 */
SgBaseClassModifier*
PrologToRose::createBaseClassModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  SgBaseClassModifier* b = new SgBaseClassModifier();
  ROSE_ASSERT(b != NULL);
  b->set_modifier((SgBaseClassModifier::baseclass_modifier_enum) 
    toInt(c->at(0)));

  b->get_accessModifier().set_modifier(
   (SgAccessModifier::access_modifier_enum) 
   createEnum(c->at(1), re.access_modifier));
  return b;
}
/**
 * create SgFunctionModifier
 */
SgFunctionModifier*
PrologToRose::createFunctionModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  //extract bit vector list and create bit vector
  // ( cast done in createBitVector)
  SgBitVector b = *(createBitVector(c->at(0), re.function_modifier));
  SgFunctionModifier* m = new SgFunctionModifier();
  ROSE_ASSERT(m != NULL);
  m->set_modifierVector(b);
  return m;
}
/**
 * create SgSpecialFunctionModifier
 */
SgSpecialFunctionModifier*
PrologToRose::createSpecialFunctionModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  //extract bit vector list and create bit vector
  // ( cast done in createBitVector)
  SgBitVector b = *(createBitVector(c->at(0), 
				    re.special_function_modifier));
  SgSpecialFunctionModifier* m = new SgSpecialFunctionModifier();
  ROSE_ASSERT(m != NULL);
  m->set_modifierVector(b);
  return m;
}

/**
 * create SgStorageModifier
 */
SgStorageModifier*
PrologToRose::createStorageModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  SgStorageModifier* a = new SgStorageModifier();
  ROSE_ASSERT(a != NULL);
  a->set_modifier((SgStorageModifier::storage_modifier_enum)
	  createEnum(c->at(0), re.storage_modifier));
  return a;
}
/**
 * create SgLinkageModifier
 */
SgLinkageModifier*
PrologToRose::createLinkageModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  SgLinkageModifier* a = new SgLinkageModifier();
  ROSE_ASSERT(a != NULL);
  a->set_modifier((SgLinkageModifier::linkage_modifier_enum) toInt(c->at(0)));
  return a;
}
/**
 * create SgElaboratedTypeModifier
 */
SgElaboratedTypeModifier*
PrologToRose::createElaboratedTypeModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  SgElaboratedTypeModifier* a = new SgElaboratedTypeModifier();
  ROSE_ASSERT(a != NULL);
  a->set_modifier((SgElaboratedTypeModifier::elaborated_type_modifier_enum) toInt(c->at(0)));
  return a;
}

/**
 * create SgConstVolatileModifier
 */
SgConstVolatileModifier*
PrologToRose::createConstVolatileModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  SgConstVolatileModifier* a = new SgConstVolatileModifier();
  ROSE_ASSERT(a != NULL);
  a->set_modifier((SgConstVolatileModifier::cv_modifier_enum)
		  createEnum(c->at(0), re.cv_modifier));
  return a;
}
/**
 * create SgUPC_AccessModifier
 */
SgUPC_AccessModifier*
PrologToRose::createUPC_AccessModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  SgUPC_AccessModifier* a = new SgUPC_AccessModifier();
  ROSE_ASSERT(a != NULL);
  a->set_modifier((SgUPC_AccessModifier::upc_access_modifier_enum)
		  createEnum(c->at(0), re.upc_access_modifier));
  return a;
}

/**
 * create SgTypeModifier
 */
SgTypeModifier*
PrologToRose::createTypeModifier(PrologTerm* t) {
  /* create modifier*/
  SgTypeModifier* m = new SgTypeModifier();
  ROSE_ASSERT(m != NULL);
  /* set modifier*/
  setTypeModifier(t,m);
  return m;
}


/**
 * set SgTypeModifier's values
 */
void
PrologToRose::setTypeModifier(PrologTerm* t, SgTypeModifier* tm) {
  /* cast*/
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  ROSE_ASSERT(tm != NULL);
  /* set bit vector and internal modifiers*/
  SgBitVector b = *(createBitVector(c->at(0), re.type_modifier));
  tm->set_modifierVector(b);

  tm->get_upcModifier().set_modifier(
    (SgUPC_AccessModifier::upc_access_modifier_enum)
    createEnum(c->at(1), re.upc_access_modifier));

  tm->get_constVolatileModifier().set_modifier(
    (SgConstVolatileModifier::cv_modifier_enum)
    createEnum(c->at(2), re.cv_modifier));

  tm->get_elaboratedTypeModifier().set_modifier(
    (SgElaboratedTypeModifier::elaborated_type_modifier_enum)
    createEnum(c->at(3), re.elaborated_type_modifier));
}

/**
 * create SgDeclarationModifier
 */
SgDeclarationModifier*
PrologToRose::createDeclarationModifier(PrologTerm* t) {
  SgDeclarationModifier* d = new SgDeclarationModifier();
  ROSE_ASSERT(d != NULL);
  /* set values*/
  setDeclarationModifier(t,d);
  return d;	
}

/**
 * set SgDeclarationModifier's values
 */
void
PrologToRose::setDeclarationModifier(PrologTerm* t, SgDeclarationModifier* d) {
  debug("setting modifier");
  ROSE_ASSERT(d != NULL);
  /* cast*/
  PrologCompTerm* c = isPrologCompTerm(t);
  ROSE_ASSERT(c != NULL);
  /* create and set bit vector*/
  SgBitVector b = *(createBitVector(c->at(0), re.declaration_modifier));
  d->set_modifierVector(b);
  /* set type modifier values*/
  setTypeModifier(c->at(1),&(d->get_typeModifier()));
  /* set access modifier value*/
  d->get_accessModifier().set_modifier((SgAccessModifier::access_modifier_enum) 
     createEnum(c->at(2), re.access_modifier));
  d->get_storageModifier().set_modifier(
     (SgStorageModifier::storage_modifier_enum)
     createEnum(c->at(3), re.storage_modifier));
}

/**
 * create SgAggregateInitializer
 */
SgAggregateInitializer*
PrologToRose::createAggregateInitializer(Sg_File_Info* fi,SgNode* child1,PrologCompTerm* t) {
  //Child must be a SgExprListExp
  SgExprListExp* e = isSgExprListExp(child1);
  ROSE_ASSERT(e != NULL);
  // create node
  SgAggregateInitializer* i = new SgAggregateInitializer(fi,e);
  ROSE_ASSERT(i != NULL);
  return i;
}


/**
 * create dummy SgFunctionDeclaration
 */
SgFunctionDeclaration*
PrologToRose::createDummyFunctionDeclaration(string* namestr, PrologTerm* type_term) {
  ROSE_ASSERT(namestr != NULL);
  ROSE_ASSERT(type_term != NULL);
  /* create SgName and SgFunctionType from arguments*/
  SgName n = *(namestr);
  SgFunctionType* tpe = dynamic_cast<SgFunctionType*>(createType(type_term));
  ROSE_ASSERT(tpe != NULL);
  /* create SgFunctionDeclaration*/
  SgFunctionDeclaration* d = new SgFunctionDeclaration(FI,n,tpe);
  ROSE_ASSERT(d != NULL);
  /* postprocessing to satisfy unparser*/
  d->setForward();
  fakeParentScope(d);

  return d;
}

/**
 * create dummy SgFunctionSymbol
 */
SgFunctionSymbol*
PrologToRose::createDummyFunctionSymbol(string* namestr, PrologTerm* type_term) {
  ROSE_ASSERT(namestr != NULL);
  ROSE_ASSERT(type_term != NULL);
  /* create dummy declaration*/
  SgFunctionDeclaration* dec = createDummyFunctionDeclaration(namestr,type_term);
  ROSE_ASSERT(dec != NULL);
  /* use declaration to create SgFunctionSymbol*/
  SgFunctionSymbol* sym = new SgFunctionSymbol(dec);
  return sym;
}

/**
 * create dummy SgMemberFunctionSymbol
 */
SgMemberFunctionSymbol*
PrologToRose::createDummyMemberFunctionSymbol(PrologTerm* annot_term) {
  /* retrieve representation of member function from annotation*/
  debug("mfs");
  ROSE_ASSERT(annot_term != NULL);
  PrologCompTerm* annot = isPrologCompTerm(annot_term);
  /* use generic term to rose function for creating a SgNode and try to cast*/
  SgNode* mfunc_uncast = toRose(annot->at(0));
  ROSE_ASSERT(mfunc_uncast != NULL);
  SgMemberFunctionDeclaration* mfunc = isSgMemberFunctionDeclaration(mfunc_uncast);
  ROSE_ASSERT(mfunc != NULL);
  /*add class scope*/
  PrologCompTerm* scope_term = isPrologCompTerm(annot->at(1));
  /*scope name and type*/
  debug("creating scope for member function declaration for symbol ");
  string scope_name = *(toStringP(scope_term->at(0)));
  int scope_type = toInt(scope_term->at(1));
  fakeClassScope(scope_name,scope_type,mfunc);
  ROSE_ASSERT(mfunc->get_class_scope() != NULL);
  /* create symbol */
  SgMemberFunctionSymbol* s = new SgMemberFunctionSymbol(mfunc);
  ROSE_ASSERT(s != NULL);
  return s;
}


/**
 * create SgFunctionRefExp
 */
SgFunctionRefExp*
PrologToRose::createFunctionRefExp(Sg_File_Info* fi, PrologCompTerm* ct) {
  /* extract pointer to string containing the name
   * and a PrologTerm* with the type info from annotation*/
  ROSE_ASSERT(ct != NULL);
  PrologCompTerm* annot = retrieveAnnotation(ct);
  ROSE_ASSERT(annot != NULL);
  ROSE_ASSERT(annot->getArity() == 3);
  string* s = toStringP(annot->at(0));
  ROSE_ASSERT(s != NULL);

  SgFunctionSymbol* sym;
  string id = makeFunctionID(*s, annot->at(1)->getRepresentation());
  if (funcDeclMap.find(id) != funcDeclMap.end()) {
    /* get the real symbol */
    sym = new SgFunctionSymbol(funcDeclMap[id]);
  } else {
    ROSE_ASSERT(false);
    /* create function symbol*/
    debug("symbol");
    sym = createDummyFunctionSymbol(s,annot->at(1));
  }
  ROSE_ASSERT(sym != NULL);

  /* get type from function symbol*/
  SgFunctionType* ft = isSgFunctionType(sym->get_type());
  ROSE_ASSERT(ft != NULL);
  /* create SgFunctionRefExp*/
  SgFunctionRefExp* re = new SgFunctionRefExp(fi,sym,ft);
  ROSE_ASSERT(re != NULL);
  return re;
}

/**
 * create SgMemberFunctionRefExp
 */
SgMemberFunctionRefExp*
PrologToRose::createMemberFunctionRefExp(Sg_File_Info* fi, PrologCompTerm* ct) {
  /* extract pointer to string containing the name
   * and a PrologTerm* with the type info from annotation*/
  ROSE_ASSERT(ct != NULL);
  PrologCompTerm* annot = retrieveAnnotation(ct);
  ROSE_ASSERT(annot != NULL);
  ROSE_ASSERT(annot->getArity() == 3);
  /* create member function symbol*/
  SgMemberFunctionSymbol* sym = createDummyMemberFunctionSymbol(annot->at(0));
  ROSE_ASSERT(sym!= NULL);
  /* virtual call?*/
  int vc = toInt(annot->at(1));
  /* create type*/
  SgFunctionType* tpe = isSgFunctionType(sym->get_type());
  ROSE_ASSERT(tpe != NULL);
  /* need qualifier?*/
  int nc = toInt(annot->at(3));
  SgMemberFunctionRefExp* ref = new SgMemberFunctionRefExp(fi,sym,vc,tpe,nc);
  ROSE_ASSERT(ref != NULL);
  return ref;
}

/**
 * create SgNamespaceDefinitionStatement
 */
SgNamespaceDefinitionStatement*
PrologToRose::createNamespaceDefinitionStatement(Sg_File_Info* fi, deque<SgNode*>* succs) {
  debug("now creating namespace definition");
  /* create definition (declaration is set later)*/
  SgNamespaceDefinitionStatement* d = new SgNamespaceDefinitionStatement(fi,NULL);
  ROSE_ASSERT(d != NULL);
  /* append declarations*/
  deque<SgNode*>::iterator it = succs->begin();
  while(it != succs->end()) {
    SgDeclarationStatement* s = NULL;
    s = isSgDeclarationStatement(*it);
    ROSE_ASSERT(s != NULL);
    d->append_declaration(s);
    s->set_parent(d);
    it++;
  }
  return d;
}

/**
 * create SgNamespaceDeclarationStatement
 */
SgNamespaceDeclarationStatement*
PrologToRose::createNamespaceDeclarationStatement(Sg_File_Info* fi, SgNode* child1, PrologCompTerm* t) {
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  SgNamespaceDefinitionStatement* def = isSgNamespaceDefinitionStatement(child1);
  if (def == NULL) {
    debug("namespace definition is NULL");
  }
  SgName n = *(toStringP(annot->at(0)));
  bool unnamed = (bool) toInt(annot->at(1));
  SgNamespaceDeclarationStatement* dec = new SgNamespaceDeclarationStatement(fi,n,def,unnamed);
  ROSE_ASSERT(dec != NULL);
  if(def != NULL) {
    def->set_namespaceDeclaration(dec);
    dec->set_forward(false);
    dec->set_definingDeclaration(dec);
  }
  /* Unparser has problems if this isn't set*/
  dec->set_firstNondefiningDeclaration(dec);
  return dec;
}


/**
 * create SgFunctionCallExp
 */
SgFunctionCallExp*
PrologToRose::createFunctionCallExp(Sg_File_Info* fi, SgNode* child1, SgNode* child2, PrologCompTerm* ct) {
  /* cast children*/
  SgExpression* re = isSgExpression(child1);
  SgExprListExp* el = isSgExprListExp(child2);
  ROSE_ASSERT(re != NULL);
  ROSE_ASSERT(el != NULL);
  /*create return type*/
  PrologCompTerm* annot = retrieveAnnotation(ct);
  ROSE_ASSERT(annot != NULL);
  SgType* rt = createType(annot->at(0));
  ROSE_ASSERT(rt != NULL);
  /* create SgFunctionCallExp*/
  SgFunctionCallExp* fce = new SgFunctionCallExp(fi,re,el,rt);
  ROSE_ASSERT(fce != NULL);
  return fce;
}

/**
 * create SgTryStmt
 */
SgTryStmt*
PrologToRose::createTryStmt(Sg_File_Info* fi, SgNode* child1, SgNode* child2, PrologCompTerm* ct) {
  /* first child is a SgStatement*/
  SgStatement* b = isSgStatement(child1);
  ROSE_ASSERT(b != NULL);
  /*second child is a SgCatchStatementSeq*/
  SgCatchStatementSeq* s = isSgCatchStatementSeq(child2);
  /* create try statement*/
  SgTryStmt* t = new SgTryStmt(fi,b);	
  /* if catch seq statement exists, set it*/
  if (s != NULL) {
    t->set_catch_statement_seq_root(s);
  }
  return t;
	
}
/**
 * create SgCatchOptionStmt
 */
SgCatchOptionStmt*
PrologToRose::createCatchOptionStmt(Sg_File_Info* fi, SgNode* child1, SgNode* child2, PrologCompTerm* ct) {
  SgVariableDeclaration* dec = isSgVariableDeclaration(child1);
  ROSE_ASSERT(dec != NULL);
  SgStatement* bl = isSgStatement(child2);
  ROSE_ASSERT(bl != NULL);
  SgCatchOptionStmt* s = new SgCatchOptionStmt(fi,dec,bl,NULL);
  ROSE_ASSERT(s != NULL);
  return s;
}
/**
 * create SgCatchStatementSeq
 */
SgCatchStatementSeq*
PrologToRose::createCatchStatementSeq(Sg_File_Info* fi, deque<SgNode*>* succs) {
  SgCatchStatementSeq* seq = new SgCatchStatementSeq(fi);
  ROSE_ASSERT(seq != NULL);
  deque<SgNode*>::iterator it = succs->begin();
  while(it != succs->end()) {
    SgStatement* s = isSgStatement(*it);
    if (s != NULL) {
      seq->append_catch_statement(s);
    }
    it++;
  }
  return seq;
}
/**
 * create a SgThisExp
 */
SgThisExp*
PrologToRose::createThisExp(Sg_File_Info* fi, PrologCompTerm* ct) {
  /*the unparser does not use the class name*/
  SgThisExp* t = new SgThisExp(fi,NULL);
  ROSE_ASSERT(t != NULL);
  return t;
}

/**
 * create a SgConstructorInitializer
 */
SgConstructorInitializer*
PrologToRose::createConstructorInitializer(Sg_File_Info* fi, SgNode* child1,PrologCompTerm* t) {
  /*retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(t != NULL);
  /* get class name*/
  string s = *toStringP(annot->at(0));
  /* create a class for unparsing the name*/
  SgMemberFunctionDeclaration* decl = createDummyMemberFunctionDeclaration(s,0);
  ROSE_ASSERT(decl != NULL);
  /* cast the SgExprListExp*/
  SgExprListExp* el = isSgExprListExp(child1);
  /* create constructor initializer, need_name = true*/
  // old ROSE 0.8.8a: SgConstructorInitializer* ci = new SgConstructorInitializer(fi,NULL,el,decl,true,false,false,false);
  SgConstructorInitializer* ci = new SgConstructorInitializer(fi,decl,el,NULL,true,false,false,false);
  ROSE_ASSERT(ci != NULL);
  ci->set_is_explicit_cast(1);
  return ci;
}

/**
 * create a SgPragmaDeclaration
 */
SgPragmaDeclaration*
PrologToRose::createPragmaDeclaration(Sg_File_Info* fi, SgNode* child1,PrologCompTerm* t) {
  /*retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(t != NULL);
  /* cast the SgPragma*/
  SgPragma* p = isSgPragma(child1);
  /* create constructor initializer, need_name = true*/
  SgPragmaDeclaration* pd = new SgPragmaDeclaration(fi,p);
  ROSE_ASSERT(pd != NULL);
  return pd;
}

/**
 * create a SgNewExp
 */
SgNewExp*
PrologToRose::createNewExp(Sg_File_Info* fi,SgNode* child1,SgNode* child2, SgNode* child3,PrologCompTerm* t) {
  /*retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  /* retrieve type*/
  SgType* tpe = createType(annot->at(0));
  ROSE_ASSERT(tpe != NULL);
  // get expression list expression
  SgExprListExp* ele = isSgExprListExp(child1);
  // get constructor initializer
  SgConstructorInitializer* ci = isSgConstructorInitializer(child2);
  // get expression
  SgExpression* ex = isSgExpression(child3);
  // create SgNewExp
  SgNewExp* ne = new SgNewExp(fi,tpe,ele,ci,ex,0);
  ROSE_ASSERT(ne != NULL);
  return ne;
}

/**
 * create a SgConditionalExp
 */
SgConditionalExp*
PrologToRose::createConditionalExp(Sg_File_Info* fi,SgNode* child1,SgNode* child2, SgNode* child3,PrologCompTerm* t) {
  /*retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  ROSE_ASSERT(annot != NULL);
  /* retrieve type*/
  SgType* tpe = createType(annot->at(0));
  ROSE_ASSERT(tpe != NULL);

  SgExpression* exp1 = isSgExpression(child1);
  ROSE_ASSERT(exp1 != NULL);

  SgExpression* exp2 = isSgExpression(child2);
  ROSE_ASSERT(exp2 != NULL);

  SgExpression* exp3 = isSgExpression(child3);
  ROSE_ASSERT(exp3 != NULL);


  SgConditionalExp* exp = new SgConditionalExp(fi, exp1, exp2, exp3, tpe);
  ROSE_ASSERT(exp != NULL);
  return exp;
}


/** issue a warning*/
void
PrologToRose::warn_msg(string msg) {
  /* since this is only a warning, i think stdout is okay*/
  cerr << "/*" << msg << "*/\n";
}

/** output a debug message, unless
 * compiled with NDEBUG*/
void
PrologToRose::debug(string message) {
  // cerr << message << "\n";
}
