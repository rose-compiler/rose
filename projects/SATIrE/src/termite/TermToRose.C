/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2007-2009 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/

#include <satire_rose.h>
#include "termite.h"
#include "TermToRose.h"
#include "AstJanitor.h"
#include <iostream>
#include <sstream>
#include <string>
#include <assert.h>
#include <boost/regex.hpp>
#include <typeinfo>

/* some single-word type names */
#include <sys/types.h>
typedef long double               longdouble;
typedef long int                     longint;
typedef long long int            longlongint;
typedef unsigned long long int  ulonglongint;

#if !HAVE_SWI_PROLOG
#  include "termparser.tab.h++"
extern int yyparse();
extern FILE* yyin;
extern PrologTerm* prote;
#endif

using namespace std;
using namespace boost;

/* Hash Keys */
static inline string makeFunctionID(const string& func_name, 
				    const string& func_type) {
  return func_name+'-'+func_type;
}
static inline string makeInameID(PrologCompTerm* annot) {
 return annot->at(0)->getRepresentation()+'-'+annot->at(1)->getRepresentation();
}

/* Error handling. Macros are used to keep a useful line number. */
#define TERM_ASSERT(t, assertion) do { \
    if (!(assertion)) \
      cerr << "** ERROR: In" << t->getArity() << "-ary Term\n  >>"	\
	   << t->getRepresentation() << "<<\n:" << endl;		\
    ROSE_ASSERT(assertion);						\
  } while (0)

#define ARITY_ASSERT(t, arity) do {					\
    if ((t)->getArity() != (arity)) {					\
      cerr << "** ERROR: " << (t)->getArity() << "-ary Term\n  >>"	\
	   << (t)->getRepresentation() << "<<\n"			\
	   << "does not have the expected arity of " << (arity)		\
	   << endl;							\
      ROSE_ASSERT(false && "Arity Error");				\
    }									\
  } while (0)

/* Expect dynamic_casts. Abort if cast fails. */
template< class NodeType >
void expect_node(SgNode* n, NodeType **r) {
  *r = dynamic_cast<NodeType*>(n);
  if (r == NULL) {
    NodeType t;
    std::cerr << "** ERROR: The node\n  " << n->unparseToString() 
	      << "\nof type >>"
	      << n->class_name() << "<< "
	      << " does not have the expected type of >>" 
	      << t.class_name()
	      << "<<" << std::endl;
    ROSE_ASSERT(false && "Type Error");
  }
}

template< class TermType >
void expect_term(PrologTerm* n, TermType **r) {
  *r = dynamic_cast<TermType*>(n);
  if (*r == NULL) {
    TermType t;
    std::cerr << "** ERROR: The term\n  "
	      << n->getRepresentation() << "\nof type >>"
	      << typeid(n).name() << "<< "
	      << " does not have the expected type of >>" 
	      << typeid(t).name()
	      << "<<" << std::endl;
    ROSE_ASSERT(false && "Type Error");
  }
}

/* Special case for Prolog Terms, with check for name  */
template< class TermType >
void expect_term(PrologTerm* n, TermType **r, 
		      std::string name) {
  expect_term(n, r);
  if ((*r)->getName() != name) {
    std::cerr << "** ERROR: The term\n  "
	      << n->getRepresentation() << "\nof type >>"
	      << (*r)->getName() << "<< "
	      << " does not have the expected type of >>" << name 
	      << "<<" << std::endl;
    ROSE_ASSERT(false && "Type Error");
  }
}

/* Special case for Prolog Terms, with check for arity and name  */
template< class TermType >
void expect_term(PrologTerm* n, TermType **r, 
		 std::string name, int arity) {
  expect_term(n, r, name);
  ARITY_ASSERT(*r, arity);
}

#define EXPECT_NODE(type, spec, base)		\
  type spec;					\
  expect_node(base, &spec);
#define EXPECT_TERM(type, spec, base)		\
  type spec;					\
  expect_term(base, &spec);
#define EXPECT_TERM_NAME(type, spec, base, name)	\
  type spec;						\
  expect_term(base, &spec, name);
#define EXPECT_TERM_NAME_ARITY(type, spec, base, name, arity)	\
  type spec;							\
  expect_term(base, &spec, name, arity);

 

/**
 * Unparse to a file
 */

void TermToRose::unparseFile(SgSourceFile& f, string prefix, string suffix, 
			       SgUnparse_Info* ui) 
{
  std::string fn = regex_replace(
      regex_replace(f.get_file_info()->get_filenameString(), 
		    regex("(\\..+?)$"),	
		    suffix+string("\\1")),
      regex("^.*/"), prefix+string("/")).c_str();
  ofstream ofile(fn.c_str());
  cerr << "Unparsing " << fn << endl;
  ofile << globalUnparseToString(f.get_globalScope(), ui);
}

void TermToRose::unparse(string filename, string dir, string suffix, 
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
TermToRose::toRose(const char* filename) {
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
  ROSE_ASSERT(declarationStatementsWithoutScope.empty());
  return root;
}

/**
 * create ROSE-IR for valid term representation*/
SgNode*
TermToRose::toRose(PrologTerm* t) {

  SgNode* node;
  if(PrologCompTerm* c = dynamic_cast<PrologCompTerm*>(t)) {

    string tname = c->getName();
    debug("converting " + tname + "\n");
    if (dynamic_cast<PrologList*>(c->at(0))) {
      node = listToRose(c,tname);
    } else {
      switch (c->getSubTerms().size()) {
      case (3): node = leafToRose(c,tname); break;
      case (4): node = unaryToRose(c,tname); break;
      case (5): node = binaryToRose(c,tname); break;
      case (6): node = ternaryToRose(c,tname); break;
      case (7): node = quaternaryToRose(c,tname); break;
      default: node = (SgNode*) 0;
      }
    }
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
  // Create the attached PreprocessingInfo
  PrologCompTerm* ct = dynamic_cast<PrologCompTerm*>(t);
  if (ln != NULL && ct != NULL) {
    PrologCompTerm* annot = 
      dynamic_cast<PrologCompTerm*>(ct->at(ct->getArity()-3));
    TERM_ASSERT(t, annot);

    PrologCompTerm* ppil = 
      dynamic_cast<PrologCompTerm*>(annot->at(annot->getArity()-1));
    if (ppil) {
      PrologList* l = dynamic_cast<PrologList*>(ppil->at(0));
      if (l) {
	for (deque<PrologTerm*>::iterator it = l->getSuccs()->begin();
	     it != l->getSuccs()->end(); ++it) {

	  EXPECT_TERM(PrologCompTerm*, ppi, *it);

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
      } else TERM_ASSERT(t, ppil->at(0)->getName() == "null");
    }
    // Set end of construct info
    Sg_File_Info *endfi = createFileInfo(ct->at(ct->getArity()-1));
    ln->set_endOfConstruct(endfi);
  }
  return node;
}

/** create ROSE-IR for unary node*/
SgNode*
TermToRose::unaryToRose(PrologCompTerm* t,string tname) {
  debug("unparsing unary"); debug(t->getRepresentation());
  /* assert correct arity of term*/
  ARITY_ASSERT(t, 4);

  /*node to be created*/
  SgNode* s = NULL;
  /*create file info and check it*/
  Sg_File_Info* fi = createFileInfo(t->at(3));
  testFileInfo(fi);
	
  if (tname == "class_declaration") {
    /* class declarations must be handled before their bodies because they
     * can be recursive */
    s = createClassDeclaration(fi,NULL,t);
  }
  /*get child node (prefix traversal step)*/
  SgNode* child1 = toRose(t->at(0));

  /* depending on the node type: create it*/
  if(isValueExp(tname)) {
    s = createValueExp(fi,child1,t);
  } else if(isUnaryOp(tname)) {
    s = createUnaryOp(fi,child1,t);
  } else if(tname == "source_file") {
    s = createFile(fi,child1,t);
  } else if(tname == "return_stmt") {
    s = createReturnStmt(fi,child1,t);
  } else if(tname == "function_definition") {
    s = createFunctionDefinition(fi,child1,t);
  } else if(tname == "initialized_name") {
    s = createInitializedName(fi,child1,t);
  } else if(tname == "assign_initializer") {
    s = createAssignInitializer(fi,child1,t);
  } else if(tname == "expr_statement") {
    s = createExprStatement(fi,child1,t);
  } else if(tname == "default_option_stmt") {
    s = createDefaultOptionStmt(fi,child1,t);
  } else if(tname == "class_declaration") {
    /* class declaration: created above, needs fixup here */
    s = setClassDeclarationBody(isSgClassDeclaration(s),child1);
  } else if(tname == "delete_exp") {
    s = createDeleteExp(fi,child1,t);
  } else if(tname == "var_arg_op") {
    s = createVarArgOp(fi,child1,t);
  } else if(tname == "var_arg_end_op") {
    s = createVarArgEndOp(fi,child1,t);
  } else if(tname == "var_arg_start_one_operand_op") {
    s = createVarArgStartOneOperandOp(fi,child1,t);
  } else if(tname == "aggregate_initializer") {
    s = createAggregateInitializer(fi,child1,t);
  } else if(tname == "namespace_declaration_statement") {
    s = createNamespaceDeclarationStatement(fi,child1,t);
  } else if(tname == "size_of_op") {
    s = createSizeOfOp(fi,child1,t);
  } else if(tname == "constructor_initializer") {
    s = createConstructorInitializer(fi,child1,t);
  } else if(tname == "pragma_declaration") {
    s = createPragmaDeclaration(fi,child1,t);
  } else if (tname == "typedef_declaration") {
    s = createTypedefDeclaration(fi,t);
  } else cerr<<"**WARNING: unhandled Unary Node: "<<tname<<endl;
    
  TERM_ASSERT(t, s != NULL);
	
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
TermToRose::binaryToRose(PrologCompTerm* t,string tname) {
  debug("unparsing binary"); debug(t->getRepresentation());
  /* assert correct arity of term*/
  ARITY_ASSERT(t, 5);
  /*create file info and check it*/
  Sg_File_Info* fi = createFileInfo(t->at(4));
  testFileInfo(fi);
  /* node to be created*/
  SgNode* s = NULL;
  /*get child node 1 (prefix traversal step)*/
  SgNode* child1 = toRose(t->at(0));
  if (tname == "function_declaration") {
    /* function declarations are special: we create an incomplete
     * declaration before traversing the body; this is necessary for
     * recursive functions */
    s = createFunctionDeclaration(fi,child1,t);
  }
  /*get child node 2 (almost-prefix traversal step)*/
  SgNode* child2 = toRose(t->at(1));
	
  /* create node depending on type*/
  if (tname == "function_declaration") {
    /* function declaration: created above, needs a fixup here */
    s = setFunctionDeclarationBody(isSgFunctionDeclaration(s),child2);
  } else if (isBinaryOp(tname)) {
    s = createBinaryOp(fi,child1,child2,t);
  } else if (tname == "cast_exp") {
    s = createUnaryOp(fi,child1,t);
  } else if (tname == "switch_statement") {
    s = createSwitchStatement(fi,child1,child2,t);
  } else if (tname == "do_while_stmt") {
    s = createDoWhileStmt(fi,child1,child2,t);
  } else if (tname == "while_stmt") {
    s = createWhileStmt(fi,child1,child2,t);
  } else if(tname == "var_arg_copy_op") {
    s = createVarArgCopyOp(fi,child1,child2,t);
  } else if(tname == "var_arg_start_op") {
    s = createVarArgStartOp(fi,child1,child2,t);	
  } else if(tname == "function_call_exp") {
    s = createFunctionCallExp(fi,child1,child2,t);	
  } else if(tname == "try_stmt") {
    s = createTryStmt(fi,child1,child2,t);	
  } else if(tname == "catch_option_stmt") {
    s = createCatchOptionStmt(fi,child1,child2,t);	
  } else if (tname == "source_file") {
    TERM_ASSERT(t, false && "a source_file should not be a binary node!");
    s = createFile(fi,child1,t);
  } else cerr<<"**WARNING: unhandled Binary Node: "<<tname<<endl;

  TERM_ASSERT(t, s != NULL);

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
TermToRose::ternaryToRose(PrologCompTerm* t,string tname) {
  debug("unparsing ternary");
  /* assert correct arity of term*/
  ARITY_ASSERT(t, 6);
  /*get child nodes (prefix traversal step)*/
  SgNode* child1 = toRose(t->at(0));
  SgNode* child2 = toRose(t->at(1));
  SgNode* child3 = toRose(t->at(2));
  /*create file info and check it*/
  Sg_File_Info* fi = createFileInfo(t->at(5));
  testFileInfo(fi);
  /* create nodes depending on type*/
  SgNode* s = NULL;
  if (tname == "if_stmt") {
    s = createIfStmt(fi,child1,child2,child3,t);
  } else if (tname == "case_option_stmt") {
    s = createCaseOptionStmt(fi,child1,child2,child3,t);
  } else if (tname == "member_function_declaration") {
    s = createMemberFunctionDeclaration(fi,child1,child2,child3,t);
  } else if (tname == "new_exp") {
    s = createNewExp(fi,child1,child2,child3,t);
  } else if (tname == "conditional_exp") {
    s = createConditionalExp(fi,child1,child2,child3,t);
  } else cerr<<"**WARNING: unhandled Ternary Node: "<<tname<<endl;

  TERM_ASSERT(t, s != NULL);

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
TermToRose::quaternaryToRose(PrologCompTerm* t,string tname) {
  debug("unparsing quaternary");
  /* assert correct arity of term*/
  ARITY_ASSERT(t, 7);
  /*get child nodes (prefix traversal step)*/
  SgNode* child1 = toRose(t->at(0));
  SgNode* child2 = toRose(t->at(1));
  SgNode* child3 = toRose(t->at(2));
  SgNode* child4 = toRose(t->at(3));
  /*create file info and check it*/
  Sg_File_Info* fi = createFileInfo(t->at(6));
  testFileInfo(fi);
  /* node to be created*/
  SgNode* s = NULL;
  if(tname == "for_statement") {
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
TermToRose::listToRose(PrologCompTerm* t,string tname) {
  debug("unparsing list node");
  ARITY_ASSERT(t, 4);
  EXPECT_TERM(PrologList*, l, t->at(0));
  /*create file info and check it*/
  Sg_File_Info* fi = createFileInfo(t->at(3));
  testFileInfo(fi);
  /*get child nodes (prefix traversal step)*/
  SgNode* cur = NULL;
  /* recursively, create ROSE-IR for list-members*/
  deque<PrologTerm*>* succterms = l->getSuccs();

  /* lookeahead hack for variable declarations: the annotation term must be
   * traversed first because it may contain a type declaration */
  SgDeclarationStatement *varDeclBaseTypeDecl = NULL;
  if (tname == "variable_declaration") {
    EXPECT_TERM_NAME_ARITY(PrologCompTerm*, annot, t->at(1),
		 "variable_declaration_specific", 3);
    PrologTerm* typeDeclTerm = annot->at(1);
    if (!(dynamic_cast<PrologAtom*>(typeDeclTerm))) {
      varDeclBaseTypeDecl = isSgDeclarationStatement(toRose(typeDeclTerm));
      declarationStatementsWithoutScope.push_back(varDeclBaseTypeDecl);
    }
  }

  /* lookahead hack */
  if (tname == "global") 
    globalDecls = succterms;

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
  if(tname == "global") {
    s = createGlobal(fi,succs);
  } else if (tname == "project") {
    s = createProject(fi,succs);
  } else if (tname == "function_parameter_list") {
    s = createFunctionParameterList(fi,succs);
  } else if (tname == "basic_block") {
    s = createBasicBlock(fi,succs);
  } else if (tname == "variable_declaration") {
    s = createVariableDeclaration(fi,succs,t,varDeclBaseTypeDecl);
  } else if (tname == "for_init_statement") {
    s = createForInitStatement(fi,succs);
  } else if (tname == "class_definition") {
    s = createClassDefinition(fi,succs,t);
  } else if (tname == "enum_declaration") {
    s = createEnumDeclaration(fi,succs,t);
  } else if (tname == "expr_list_exp") {
    s = createExprListExp(fi,succs);
  } else if (tname == "ctor_initializer_list") {
    s = createCtorInitializerList(fi,succs);
  } else if (tname == "namespace_definition_statement") {
    s = createNamespaceDefinitionStatement(fi,succs);
  } else if (tname == "catch_statement_seq") {
    s = createCatchStatementSeq(fi,succs);
  }
  TERM_ASSERT(t, s != NULL);

  /* note that for the list nodes the set_parent operation takes place
   * inside the methods when necessary since they always require
   * an iteration over the list anyway. */

  return s;
}


/**create ROSE-IR from leaf terms*/
SgNode*
TermToRose::leafToRose(PrologCompTerm* t,string tname) {
  debug("unparsing leaf");
  /* assert correct arity of term*/
  ARITY_ASSERT(t, 3);
  /* create file info and check it*/
  Sg_File_Info* fi = createFileInfo(t->at(2));
  testFileInfo(fi);
  /* node to be created*/
  SgNode* s = NULL;
  /* some list nodes become leaf nodes when the list is empty
   * -> create dummy list and call corresponding factory methods*/
  if (tname == "function_parameter_list") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createFunctionParameterList(fi,adummy);
  } else if (tname == "basic_block") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createBasicBlock(fi,adummy);
  } else if (tname == "class_definition") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createClassDefinition(fi,adummy,t);
  } else if (tname == "ctor_initializer_list") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createCtorInitializerList(fi,adummy);
  } else if (tname == "expr_list_exp") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createExprListExp(fi,adummy);
  } else if (tname == "namespace_definition_statement") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createNamespaceDefinitionStatement(fi,adummy);
  } else if (tname == "for_init_statement") {
    deque<SgNode*>* adummy = new deque<SgNode*>;
    s = createForInitStatement(fi,adummy);
    /* regular leaf nodes*/
  } else if (tname == "var_ref_exp") {
    s = createVarRefExp(fi,t);
  } else if (tname == "break_stmt") {
    s = createBreakStmt(fi,t);
  } else if (tname == "continue_stmt") {
    s = createContinueStmt(fi,t);
  } else if (tname == "label_statement") {
    s = createLabelStatement(fi,t);
  } else if (tname == "goto_statement") {
    s = createGotoStatement(fi,t);
  } else if (tname == "ref_exp") {
    s = createRefExp(fi,t);
  } else if (tname == "function_ref_exp") {
    s = createFunctionRefExp(fi,t);
  } else if (tname == "member_function_ref_exp") {
    s = createMemberFunctionRefExp(fi,t);
  } else if (tname == "this_exp") {
    s = createThisExp(fi,t);
  } else if(tname == "pragma") {
    s = createPragma(fi,t);
  } else if (tname == "null_statement") {
    s = new SgNullStatement(fi);
  } else if (tname == "null_expression") {
    s = new SgNullExpression(fi);
  }
  TERM_ASSERT(t, s != NULL);
  return s;
}

/** test file info soundness*/
void
TermToRose::testFileInfo(Sg_File_Info* fi) {
  ROSE_ASSERT(fi != NULL);
  ROSE_ASSERT(fi->get_line() >= 0);
  ROSE_ASSERT(fi->get_col() >= 0);
}

/** create Sg_File_Info from term*/
Sg_File_Info*
TermToRose::createFileInfo(PrologTerm* t) {
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
    EXPECT_TERM_NAME_ARITY(PrologCompTerm*, u, t, "file_info", 3);
    debug(u->getRepresentation());
    if ((u->at(0)->getName() == "compilerGenerated") || 
        (u->at(0)->getName() == "<invalid>")) {
      debug("compiler generated node");
      fi = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
      // if we do not set the output flag, the unparser will sometimes
      // refuse to output entire basic blocks
      fi->setOutputInCodeGeneration();
    }
    else {
      /* a filename is present => retrieve data from term and generete node*/
      EXPECT_TERM(PrologAtom*, filename, u->at(0));
      EXPECT_TERM(PrologInt*, line, u->at(1));
      EXPECT_TERM(PrologInt*, col, u->at(2));
      /* filename must be a term representing a character string, 
       * line and col are integers */
      assert(line->getValue() >= 0);
      assert(col->getValue() >= 0);
      fi = new Sg_File_Info(filename->getName(),
			    line->getValue(), col->getValue());
    }
  } 
  TERM_ASSERT(t, fi != NULL);

  return fi;
}

/**create enum type from annotation*/
SgEnumType*
TermToRose::createEnumType(PrologTerm* t) {
  /* first subterm is the name of the enum*/
  debug("creating enum type");
  EXPECT_TERM_NAME(PrologCompTerm*, annot, t, "enum_type");
  /*create dummy declaration*/
  string id = annot->at(0)->getRepresentation();
  SgEnumType* type = NULL;
  if (lookupType(&type, id)) {
    // We have a problem since we only use the first definition as a reference
    return type;
  } else {
    cerr<<id<<endl;
    TERM_ASSERT(t, false);
    SgEnumDeclaration* dec = isSgEnumDeclaration(toRose(annot->at(0)));
    TERM_ASSERT(t, dec != NULL);
    /* create type using a factory Method*/
    return SgEnumType::createType(dec);
  }
}

/**create pointer type from annotation*/
SgPointerType*
TermToRose::createPointerType(PrologTerm* t) {
  EXPECT_TERM_NAME(PrologCompTerm*, c, t, "pointer_type");
  /* first subterm is the base type*/
  SgType* base_type = TermToRose::createType(c->at(0));
  TERM_ASSERT(t, base_type != NULL);
  /* use SgPointerType's factory method*/
  SgPointerType* pt = SgPointerType::createType(base_type);
  TERM_ASSERT(t, pt != NULL);
  return pt;
}

/**create reference type from annotation*/
SgReferenceType*
TermToRose::createReferenceType(PrologTerm* t) {
  EXPECT_TERM_NAME(PrologCompTerm*, c, t, "reference_type");
  /* first subterm is the base type*/
  SgType* base_type = TermToRose::createType(c->at(0));
  TERM_ASSERT(t, base_type != NULL);
  /* use SgPointerType's factory method*/
  SgReferenceType* pt = SgReferenceType::createType(base_type);
  TERM_ASSERT(t, pt != NULL);
  return pt;
}
	
/** create array type from annotation*/
SgArrayType*
TermToRose::createArrayType(PrologTerm* t) {
  EXPECT_TERM_NAME(PrologCompTerm*, c, t, "array_type");
  /*first subterm is base type*/
  SgType* base_type = createType(c->at(0));
  TERM_ASSERT(t, base_type != NULL);
  /* second subterm is an expression*/
  SgExpression* e = NULL;
  e = isSgExpression(toRose(c->at(1)));
  /* use factory method of SgArrayType*/
  SgArrayType* at = SgArrayType::createType(base_type,e);
  TERM_ASSERT(t, at != NULL);
  return at;
}

/**
 * create SgModifierType
 */
SgModifierType*
TermToRose::createModifierType(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  TERM_ASSERT(t, c != NULL);
  SgModifierType* mt = new SgModifierType(createType(c->at(0)));
  setTypeModifier(c->at(1),&(mt->get_typeModifier()));
  return mt;
}
/**
 * create SgTypedefType
 */
SgTypedefType*
TermToRose::createTypedefType(PrologTerm* t) {
  /* extract declaration*/
  PrologCompTerm* annot = isPrologCompTerm(t);
  TERM_ASSERT(t, annot != NULL);
  /*make sure this is supposed to be a typedef type*/
  string tname = annot->getName();
  TERM_ASSERT(t, tname == "typedef_type");

  /*extract name*/
  SgName n = *(toStringP(annot->at(0)));
  SgTypedefDeclaration* decl = NULL;
  SgTypedefType* tpe = NULL;
  string id = t->getRepresentation();
  if (lookupDecl(&decl, id, false)) {
    tpe = decl->get_type(); //new SgTypedefType(decl);
#if HAVE_SWI_PROLOG
  } else if (lookaheadDecl(&decl, 
			   "typedef_declaration(_,typedef_annotation("
			   +annot->at(0)->getRepresentation()+",_,_),_,_)")) {
    tpe = decl->get_type();
#else
 // FIXME: Implement a lookahead for the no-SWI case. Until that is done,
 // this will create spurious types, which is not nice.
#endif
  } else {
    SgType* basetype = NULL;
    if (annot->at(1)->getName() != "typedef_type") {
      basetype = TermToRose::createType(annot->at(1));
    } else {
      cerr<<id<<endl;
      TERM_ASSERT(t, false && "FIXME");
      /*we don't want to keep the base type empty, use int (irrelevant
	for unparsing*/
      basetype = new SgTypeInt();
      //decl = new SgTypedefDeclaration(FI,n,basetype,NULL,NULL,NULL);
    }
    decl = createTypedefDeclaration(FI,
        new PrologCompTerm("typedef_declaration", 4,
			   new PrologAtom("null"),
			   new PrologCompTerm("typedef_annotation", 3,
					      annot->at(0), annot->at(1), 
					      new PrologAtom("null"),
					      new PrologAtom("null")),
			   new PrologAtom("null"),
			   new PrologAtom("null")));
    TERM_ASSERT(t, decl != NULL);
    tpe = SgTypedefType::createType(decl);
    declarationStatementsWithoutScope.push_back(decl);
  }
  TERM_ASSERT(t, tpe != NULL);
	
  return tpe;
}


/**
 * create SgType from term, possibly recursively
 */
SgType* 
TermToRose::createType(PrologTerm* t) {
  SgType* type = NULL;
  string id = t->getRepresentation();
  if (lookupType(&type, id, false)) {
    return type;
  }

  if (PrologCompTerm* c = isPrologCompTerm(t)) {
    string tname = t->getName();
    if (tname == "enum_type") {
      type = createEnumType(t);
    } else if (tname == "pointer_type") {
      type = createPointerType(t);
    } else if (tname == "reference_type") {
      type = createReferenceType(t);
    } else if (tname == "class_type") {
      type = createClassType(t);
    } else if (tname == "function_type") {
      type = createFunctionType(t);
    } else if (tname == "member_function_type") {
      type = createMemberFunctionType(t);
    } else if (tname == "array_type") {
      type = createArrayType(t);
    } else if (tname == "modifier_type") {
      type = createModifierType(t);
    } else if (tname == "typedef_type") {
      type = createTypedefType(t);
    } else TERM_ASSERT(t, false && "Unknown type enountered");
  }
  if (PrologAtom* a = dynamic_cast<PrologAtom*>(t)) {
    string tname = a->getName();
    if (tname == "null") {
      warn_msg("warning: no type created");
      type = NULL;
    } else
    if (tname=="type_bool") type = new SgTypeBool();
    else if (tname=="type_char") type = new SgTypeChar();
    else if (tname=="type_default") type = new SgTypeDefault();
    else if (tname=="type_double") type = new SgTypeDouble();
    else if (tname=="type_float") type = new SgTypeFloat();
    else if (tname=="type_global_void") type = new SgTypeGlobalVoid(); 
    else if (tname=="type_ellipse") {type = new SgTypeEllipse();}
    else if (tname=="type_int") {type = new SgTypeInt();}
    else if (tname=="type_long") type = new SgTypeLong();
    else if (tname=="type_long_double") type = new SgTypeLongDouble();
    else if (tname=="type_long_long") type = new SgTypeLongLong();
    else if (tname=="type_short") type = new SgTypeShort();
    else if (tname=="type_signed_char") type = new SgTypeSignedChar();
    else if (tname=="type_signed_int") type = new SgTypeSignedInt();
    else if (tname=="type_signed_long") type = new SgTypeSignedLong();
    else if (tname=="type_signed_short") type = new SgTypeSignedShort();
    else if (tname=="type_string") type = new SgTypeString();
    else if (tname=="type_unknown") type = new SgTypeUnknown();
    else if (tname=="type_unsigned_char") type = new SgTypeUnsignedChar();
    else if (tname=="type_unsigned_int") type = new SgTypeUnsignedInt();
    else if (tname=="type_unsigned_long") type = new SgTypeUnsignedLong();
    else if (tname=="type_unsigned_long_long") type = new SgTypeUnsignedLongLong();
    else if (tname=="type_unsigned_short") type = new SgTypeUnsignedShort(); 
    else if (tname=="type_void") type = new SgTypeVoid();
    else if (tname=="type_wchar") type = new SgTypeWchar();
    else TERM_ASSERT(t, false && "Unknown type enountered");
  }
  typeMap[id] = type;
  return type;
	
}

/**
 * is this string the name of a SgValueExp?*/
bool
TermToRose::isValueExp(string tname) {
  if(tname == "bool_val_exp")
    return true;
  if(tname == "string_val")
    return true;
  if(tname == "short_val")
    return true;
  if(tname == "char_val")
    return true;
  if(tname == "unsigned_char_val")
    return true;
  if(tname == "wchar_val")
    return true;
  if(tname == "unsigned_short_val")
    return true;
  if(tname == "int_val")
    return true;
  if(tname == "enum_val")
    return true;
  if(tname == "unsigned_int_val")
    return true;
  if(tname == "long_int_val")
    return true;
  if(tname == "long_long_int_val")
    return true;
  if(tname == "unsigned_long_long_int_val")
    return true;
  if(tname == "unsigned_long_val")
    return true;
  if(tname == "float_val")
    return true;
  if(tname == "double_val")
    return true;
  if(tname == "long_double_val")
    return true;
  return false;
}


/**
 *
 * UnEscape non-printable characters
 */
char
TermToRose::unescape_char(std::string s) {
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
    ROSE_ASSERT(s[0] == '\\');
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


#define createValue(SGTYPE, TYPE, fi, fromTerm) \
  do { \
    debug("unparsing " + fromTerm->getName()); \
    PrologCompTerm* annot = retrieveAnnotation(fromTerm); \
    TERM_ASSERT(fromTerm, annot != NULL); \
    PrologAtom* a = dynamic_cast<PrologAtom*>(annot->at(0)); \
    PrologInt* i = dynamic_cast<PrologInt*>(annot->at(0)); \
    TERM_ASSERT(fromTerm, (a != 0) || (i != 0)); \
    TYPE value; \
    if (a) { \
      istringstream instr(a->getName()); \
      instr >> value; \
    } else value = i->getValue(); \
    ve = new SGTYPE(fi, value); \
  } while (false)

/** create a SgValueExp*/
SgExpression* 
TermToRose::createValueExp(Sg_File_Info* fi, SgNode* succ, PrologCompTerm* t) {
  string vtype = t->getName();
  SgValueExp* ve = NULL;
  /*integer types */
  if(vtype == "int_val") 
    createValue(SgIntVal, int, fi, t);
  else if (vtype == "unsigned_int_val")
    createValue(SgUnsignedIntVal, uint, fi, t);
  else if (vtype == "short_val") 
    createValue(SgShortVal, short, fi,t);
  else if (vtype == "unsigned_short_val")
    createValue(SgUnsignedShortVal, ushort, fi, t);
  else if (vtype == "long_int_val")
    createValue(SgLongIntVal, longint, fi, t);
  else if (vtype == "unsigned_long_val") 
    createValue(SgUnsignedLongVal, ulong, fi, t);
  else if (vtype == "long_long_int_val")
    createValue(SgLongLongIntVal, longlongint, fi, t);
  else if (vtype == "unsigned_long_long_int_val")
    createValue(SgUnsignedLongLongIntVal, ulonglongint, fi, t);


  else if (vtype == "enum_val") {
    debug("unparsing enum value");
    PrologCompTerm* annot = retrieveAnnotation(t);
    TERM_ASSERT(t, annot != NULL);
    /* get value and name, find the declaration*/
    ARITY_ASSERT(annot, 4);
    int value = toInt(annot->at(0));
    SgName v_name = *toStringP(annot->at(1));

    // Rather than faking a declaration like before, we look it up in the
    // declaration map.
    SgEnumDeclaration* decdummy = NULL;
    PrologTerm* dect = isPrologCompTerm(annot->at(2))->at(0);
    TERM_ASSERT(t, declarationMap.find(dect->getRepresentation())
                != declarationMap.end());
    decdummy = isSgEnumDeclaration(declarationMap[dect->getRepresentation()]);
    TERM_ASSERT(t, decdummy != NULL);
   
    SgEnumVal* valnode = new SgEnumVal(fi,value,decdummy,v_name);
    TERM_ASSERT(t, valnode != NULL);
    TERM_ASSERT(t, valnode->get_declaration() == decdummy);
    ve = valnode;
  }
  
  /* floating point types*/
  else if (vtype == "float_val")
    createValue(SgFloatVal, float, fi, t);
  else if (vtype == "double_val")
    createValue(SgDoubleVal, double, fi, t);
  else if (vtype == "long_double_val")
    createValue(SgLongDoubleVal, longdouble, fi, t);
    
  /* characters */
  else if (vtype == "char_val") {
    //char
    debug("unparsing char");
    PrologCompTerm* annot = retrieveAnnotation(t);
    TERM_ASSERT(t, annot != NULL);
    char number;
    if (PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0))) {
      number = unescape_char(s->getName());
    } else if (PrologInt* val = dynamic_cast<PrologInt*>(annot->at(0))) {
      number = val->getValue();
    } else {
      TERM_ASSERT(t, false && "Must be either a string or an int");
    }		
    SgCharVal* valnode = new SgCharVal(fi,number);
    ve = valnode;
  } else if (vtype == "unsigned_char_val") {
    //unsigned char
    debug("unparsing unsigned char");
    PrologCompTerm* annot = retrieveAnnotation(t);
    TERM_ASSERT(t, annot != NULL);
    unsigned char number;
    if (PrologAtom* s = dynamic_cast<PrologAtom*>(annot->at(0))) {
      number = unescape_char(s->getName());
    } else if (PrologInt* val = dynamic_cast<PrologInt*>(annot->at(0))) {
      number = val->getValue();
    } else {
      TERM_ASSERT(t, false && "Must be either a string or an int");
    }
    SgUnsignedCharVal* valnode = new SgUnsignedCharVal(fi,number);
    ve = valnode;
  } 
  else if (vtype == "wchar_val")
    createValue(SgWcharVal, ulong, fi, t);
    /* boolean*/
  else if (vtype == "bool_val_exp")
    createValue(SgBoolValExp, int, fi, t);
  else if (vtype == "string_val") {
    PrologCompTerm* annot = retrieveAnnotation(t);
    TERM_ASSERT(t, annot != NULL);
    EXPECT_TERM(PrologAtom*, s, annot->at(0));
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
      EXPECT_NODE(SgExpression*, ex, succ);
      ve->set_originalExpressionTree(ex);
    }
    ve->set_endOfConstruct(fi);
  } else {
    debug("Value Type " + vtype + " not implemented yet. Returning null pointer for value expression");
  }
  return ve;
}

/**
 * is the string the name of a unary operator?
 */
bool
TermToRose::isUnaryOp(string opname) {
  if (opname == "address_of_op" ||
      opname == "bit_complement_op" || 
      opname == "expression_root" || 
      opname == "minus_op" || 
      opname == "not_op" || 
      opname == "pointer_deref_exp" || 
      opname == "unary_add_op" || 
      opname == "minus_minus_op" || 
      opname == "plus_plus_op" || 
      opname == "cast_exp" || 
      opname == "throw_op")
    return true;	
  return false;
}




/**
 * create a SgUnaryOp*/
SgUnaryOp*
TermToRose::createUnaryOp(Sg_File_Info* fi, SgNode* succ, PrologCompTerm* t) {
  EXPECT_NODE(SgExpression*,sgexp,succ);
  PrologTerm* n = t->at(0);
  string opname = t->getName();
  debug("creating " + opname + "\n");	
  //cerr << t->getRepresentation() << endl << succ << endl;
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  ARITY_ASSERT(annot, 5);
  // GB (2008-12-04): A unary op's mode is now represented by an atom
  // 'prefix' or 'postfix', not by a numerical constant.
  // PrologInt* mode = dynamic_cast<PrologInt*>(annot->at(0));
  EXPECT_TERM(PrologAtom*, mode, annot->at(0));
  SgType* sgtype = createType(annot->at(1));
  ROSE_ASSERT(sgtype != NULL);
  /*nothing special with these*/
  if (opname == "address_of_op") return new SgAddressOfOp(fi,sgexp,sgtype);
  else if (opname == "bit_complement_op") return new SgBitComplementOp(fi,sgexp,sgtype);
  else if (opname == "expression_root") {
    SgExpressionRoot* er = new SgExpressionRoot(fi,sgexp,sgtype);
    debug("Exp Root: " + er->unparseToString());
    return er;
  }
  else if (opname == "minus_op") return new SgMinusOp(fi,sgexp,sgtype);
  else if (opname == "not_op") return new SgNotOp(fi,sgexp,sgtype);
  else if (opname == "pointer_deref_exp") return new SgPointerDerefExp(fi,sgexp,sgtype);
  else if (opname == "unary_add_op") return new SgUnaryAddOp(fi,sgexp,sgtype);	
  /* chose wether to use ++ and -- as prefix or postfix via set_mode*/
  else if (opname == "minus_minus_op") {
    SgMinusMinusOp* m = new SgMinusMinusOp(fi,sgexp,sgtype);
    m->set_mode(mode->getName() == "prefix" ? SgUnaryOp::prefix
                                            : SgUnaryOp::postfix);
    return m;
  }
  else if (opname == "plus_plus_op") {
    SgPlusPlusOp* p = new SgPlusPlusOp(fi,sgexp,sgtype);
    p->set_mode(mode->getName() == "prefix" ? SgUnaryOp::prefix
                                            : SgUnaryOp::postfix);
    return p;
  }
  /* For a cast we need to retrieve cast type (enum)*/
  else if (opname == "cast_exp") {
    //cerr<<"######castexp "<< annot->getRepresentation()<< "bug in ROSE?" <<endl;
    SgCastExp* e = new SgCastExp(fi, sgexp, sgtype,
				 (SgCastExp::cast_type_enum)
				 createEnum(annot->at(2), re.cast_type));
    //cerr<< e->unparseToString()<< endl;
    return e;
  } 
  /* some more initialization necessary for a throw */
  else if (opname == "throw_op") { 
    /*need to retrieve throw kind* (enum)*/
    int tkind = createEnum(annot->at(2), re.throw_kind);
    // FIXME: use kind!
    /*need to retrieve types */
    EXPECT_TERM(PrologList*, typel, annot->at(3));
    deque<PrologTerm*>* succs = typel->getSuccs();
    deque<PrologTerm*>::iterator it = succs->begin();
    SgTypePtrListPtr tpl = new SgTypePtrList();
    while (it != succs->end()) {
      tpl->push_back(createType(*it));
      it++;
    }
    return new SgThrowOp(fi,sgexp,sgtype);
  }

  TERM_ASSERT(t, false);
  /*never called*/
  return (SgUnaryOp*) 0;	
}




/**
 * create SgProject
 */
SgProject*
TermToRose::createProject(Sg_File_Info* fi,deque<SgNode*>* succs) {
  SgProject* project = new SgProject();
  SgFilePtrList &fl = project->get_fileList();

  for (deque<SgNode*>::iterator it = succs->begin();
       it != succs->end(); ++it) {
    SgSourceFile* file = dynamic_cast<SgSourceFile*>(*it);
    if (file != NULL) { // otherwise, it's a binary file, which shouldn't happen
      fl.push_back(file);
      file->set_parent(project);
    }
  }

  // Make sure all endOfConstruct pointers are set; ROSE includes
  // essentially the same thing, but that prints verbose diagnostics. Also,
  // the AST janitor does this, but apparently it doesn't always visit every
  // node (SgVariableDefinitions, mostly).
  // This traversal also fixes up missing scopes that the AST janitor
  // doesn't catch. This might mean that we create initialized names that
  // are not connected to the AST!
  class MemoryPoolFixer: public ROSE_VisitTraversal {
  protected:
    void visit(SgNode *n) {
      if (SgLocatedNode *ln = isSgLocatedNode(n)) {
        if (ln->get_startOfConstruct() != NULL
            && ln->get_endOfConstruct() == NULL) {
          ln->set_endOfConstruct(ln->get_startOfConstruct());
        }
      }
      if (SgInitializedName *in = isSgInitializedName(n)) {
        SgNode *parent = in->get_parent();
        if (in->get_scope() == NULL || parent == NULL) {
          if (SgVariableDeclaration *v = isSgVariableDeclaration(parent))
            in->set_scope(v->get_scope());
          else if (SgEnumDeclaration *e = isSgEnumDeclaration(parent)) {
            in->set_scope(e->get_scope());
            if (in->get_scope() == NULL) {
              SgScopeStatement *scp = isSgScopeStatement(parent);
              ROSE_ASSERT(false && "enum decl has no parent and no scope?");
              in->set_scope(scp);
            }
          }
          else {
           ROSE_ASSERT(false && "initialized name without a scope, I'm lost");
          }
        }
      }
      if (SgClassDeclaration *cdecl = isSgClassDeclaration(n)) {
        if (cdecl->get_scope() == NULL) {
          SgTypedefDeclaration *tddecl =
            isSgTypedefDeclaration(cdecl->get_parent());
          if (tddecl != NULL) {
            cdecl->set_scope(tddecl->get_scope());
          }
        }
      }
      if (SgEnumDeclaration *edecl = isSgEnumDeclaration(n)) {
        if (edecl->get_scope() == NULL) {
          SgTypedefDeclaration *tddecl =
            isSgTypedefDeclaration(edecl->get_parent());
          if (tddecl != NULL) {
            edecl->set_scope(tddecl->get_scope());
          }
        }
      }
      if (SgFunctionDeclaration *fdecl = isSgFunctionDeclaration(n)) {
        if (fdecl->get_scope() == NULL) {
          ROSE_ASSERT(fdecl->get_parent() != NULL);
          if (isSgTypedefDeclaration(fdecl->get_parent())) {
            fdecl->set_scope(
                isSgTypedefDeclaration(fdecl->get_parent())->get_scope());
          }
          ROSE_ASSERT(fdecl->get_scope() != NULL);
        }
      }
      if (SgDeclarationStatement *d = isSgDeclarationStatement(n)) {
        SgDeclarationStatement *nondef = d->get_firstNondefiningDeclaration();
        SgDeclarationStatement *def = d->get_definingDeclaration();
        /* ROSE wants defining and nondefining declarations to be different
         * for many kinds of declarations */
        if (nondef == def && def != NULL) {
          d->set_firstNondefiningDeclaration(NULL);
        } else if (nondef == def && def == NULL) {
          d->set_firstNondefiningDeclaration(d);
        }
        ROSE_ASSERT(d->get_firstNondefiningDeclaration()
                 != d->get_definingDeclaration());
      }
    }
  };
  MemoryPoolFixer memoryPoolFixer;
  memoryPoolFixer.traverseMemoryPool();

  AstPostProcessing(project);

  return project;
}


/**
 * create SgSourceFile
 */
SgSourceFile*
TermToRose::createFile(Sg_File_Info* fi,SgNode* child1,PrologCompTerm*) {
  // GB (2008-10-20): It looks like there is a new SgSourceFile class in
  // ROSE 0.9.3a-2261, and that it is an instance of that class that we
  // need.
  SgSourceFile* file = new SgSourceFile();
  file->set_file_info(fi);
  // ROSE 0.9.4a fixup
  fi->set_parent(file);

  SgGlobal* glob = isSgGlobal(child1);
  ROSE_ASSERT(glob);

  //ROSE_ASSERT(declarationStatementsWithoutScope.empty());
  ROSE_ASSERT(labelStatementsWithoutScope.empty());

  file->set_globalScope(glob);
  glob->set_parent(file);

  // Do our own fixups
  AstJanitor janitor;
  janitor.traverse(file, InheritedAttribute(this));

  // rebuild the symbol table
  glob->set_symbol_table(NULL);
  SageInterface::rebuildSymbolTable(glob);

  // Memory Pool Fixups
  for (vector<SgDeclarationStatement*>::iterator it = 
   	       declarationStatementsWithoutScope.begin();
   	 it != declarationStatementsWithoutScope.end(); it++) {
    //cerr<<"("<<(*it)->class_name()<<")->set_scope("<<glob->class_name()<<");"<<endl;

    // GB: these nodes are not reachable in the AST, so traverse them
    // explicitly
    // (*it)->set_parent(glob);
    AstJanitor janitor;
    janitor.traverse(*it, InheritedAttribute(this, glob, glob));
    if (!isSgVariableDeclaration(*it)) (*it)->set_scope(glob);

    // GB: we may need to add stuff to the symbol table since some of the
    // declarations may be hidden inside typedef/variable declarations, and
    // rebuildSymbolTable will not find them there
    if (SgClassDeclaration *cdecl = isSgClassDeclaration(*it)) {
      SgSymbol *symbol = new SgClassSymbol(cdecl);
      SgName name = cdecl->get_name();
      glob->get_symbol_table()->insert(name, symbol);
    }
    if (SgEnumDeclaration *edecl = isSgEnumDeclaration(*it)) {
      SgSymbol *symbol = new SgEnumSymbol(edecl);
      SgName name = edecl->get_name();
      glob->get_symbol_table()->insert(name, symbol);
    }
   }
  declarationStatementsWithoutScope.clear();

  // Call all kinds of ROSE fixups
  // GB: Moved to createProject because this accesses global information via
  // the memory pools, so *all* files must have been janitorized before we
  // run this.
  // AstPostProcessing(file);

  // Reset local symbol tables
  classDefinitions.clear();
  typeMap.clear();
  declarationMap.clear();
  globalDecls = NULL;
  return file;
}

/**
 * create SgSizeOfOp
 */
SgSizeOfOp*
TermToRose::createSizeOfOp(Sg_File_Info* fi,SgNode* child1,PrologCompTerm* t) {
  /* retrieve anntoation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  /* get operand type*/
  SgType* otype = NULL;
  if (annot->at(0)->getRepresentation() != "null")
    otype = createType(annot->at(0));
  /* get expression type*/
  SgType* etype = createType(annot->at(1));
  /* cast child to SgExpression* */
  SgExpression* ex = isSgExpression(child1);
  /* create Op */
  SgSizeOfOp* op = new SgSizeOfOp(fi,ex,otype,etype);
  TERM_ASSERT(t, op != NULL);
  return op;
}

/** create a SgReturnStmt*/
SgReturnStmt* 
TermToRose::createReturnStmt(Sg_File_Info* fi, SgNode* succ,PrologCompTerm* t) {
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
TermToRose::createBasicBlock(Sg_File_Info* fi,deque<SgNode*>* succs) {
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
      EXPECT_NODE(SgStatement*, stmt, *it);
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
TermToRose::createFunctionDefinition(Sg_File_Info* fi,SgNode* succ,PrologCompTerm* t) {
  /* create a basic block*/
  EXPECT_NODE(SgBasicBlock*, b, succ);
  SgFunctionDefinition* fd = new SgFunctionDefinition(fi,b);

  // Various Label fixups
  for (vector<SgLabelStatement*>::iterator label = 
	       labelStatementsWithoutScope.begin();
	 label != labelStatementsWithoutScope.end(); label++) {
      (*label)->set_scope(fd);

      string l = (*label)->get_label().getString();
      multimap<string, SgGotoStatement*>::iterator low, high, goto_;
      low = gotoStatementsWithoutLabel.lower_bound(l);
      high = gotoStatementsWithoutLabel.upper_bound(l);
      for(goto_ = low; goto_ != high; ++goto_) {
	goto_->second->set_label(*label);
      }

      //cerr<<endl<<(*label)->unparseToString()<<endl<<b->unparseToString()<<endl;
  }
  labelStatementsWithoutScope.clear();

  return fd;
}


/**
 * create SgInitializedName
 */
SgInitializedName*
TermToRose::createInitializedName(Sg_File_Info* fi, SgNode* succ, PrologCompTerm* t) {
  /* retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  /*create the type*/
  SgType* tpe = createType(annot->at(0));
  TERM_ASSERT(t, tpe != NULL);
  /* create the name*/
  SgName sgnm = *toStringP(annot->at(1));
  /* create the initializer and the initialized name*/
  SgInitializer* sgini = dynamic_cast<SgInitializer*>(succ);
  SgInitializedName* siname = new SgInitializedName(sgnm,tpe,sgini,NULL);
  TERM_ASSERT(t, siname != NULL);
  TERM_ASSERT(t, siname->get_parent() == NULL);

  siname->set_file_info(FI);

  initializedNameMap[makeInameID(annot)] = siname;

  return siname;
}

/**
 * create SgInitializedName from PrologCompTerm
 */
SgInitializedName*
TermToRose::inameFromAnnot(PrologCompTerm* annot) {
  TERM_ASSERT(annot, false && "deprecated function");
  debug("creating initialized name for sg var ref exp");
  /* get type*/
  SgType* tpe = createType(annot->at(0));
  /* create name*/
  PrologAtom *nstring = dynamic_cast<PrologAtom*>(annot->at(1));
  SgName sgnm = nstring->getName().c_str();
  SgInitializedName* siname = new SgInitializedName(sgnm,tpe,NULL);
  TERM_ASSERT(annot, siname != NULL);

  siname->set_file_info(FI);

  /* static?*/
  int stat = (int) createEnum(annot->at(2), re.static_flag);
  if(stat != 0) {
    debug("setting static");
    siname->get_storageModifier().setStatic();
  }
  return siname;
}


/**
 * create SgFunctionType
 */
SgFunctionType*
TermToRose::createFunctionType(PrologTerm* t) {
  EXPECT_TERM_NAME(PrologCompTerm*, tterm, t, "function_type");
  /* create the return type*/
  SgType* ret_type = createType(tterm->at(0));
  /* has ellipses?*/
  bool has_ellipses = (bool) createEnum(tterm->at(1), re.ellipses_flag);
  /* create type */
  SgFunctionType* func_type = new SgFunctionType(ret_type,has_ellipses);
  TERM_ASSERT(t, func_type != NULL);
  /* add argument list*/
  PrologList* atypes = isPrologList(tterm->at(2));
  TERM_ASSERT(t, atypes != NULL);
  for(int i = 0; i < atypes->getArity(); i++) {
    func_type->append_argument(createType(atypes->at(i)));
  }
  return func_type;
}

/**
 * create SgMemberFunctionType
 */
SgMemberFunctionType*
TermToRose::createMemberFunctionType(PrologTerm* t) {
  EXPECT_TERM_NAME(PrologCompTerm*, tterm, t, "member_function_type");
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
  TERM_ASSERT(t, func_type != NULL);
  /* add argument list*/
  PrologList* atypes = isPrologList(tterm->at(2));
  TERM_ASSERT(t, atypes != NULL);
  for(int i = 0; i < atypes->getArity(); i++) {
    func_type->append_argument(createType(atypes->at(i)));
  }
  return func_type;
}


/**
 * create a SgFunctionParameterList
 */
SgFunctionParameterList*
TermToRose::createFunctionParameterList(Sg_File_Info* fi,deque<SgNode*>* succs) {
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
TermToRose::createFunctionDeclaration(Sg_File_Info* fi, SgNode* par_list_u, PrologCompTerm* t) {
  debug("function declaration:");
  /* cast parameter list */
  SgFunctionParameterList* par_list = isSgFunctionParameterList(par_list_u);
  /* param list must exist*/
  TERM_ASSERT(t, par_list != NULL);
  /* get annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  /* create type*/
  SgFunctionType* func_type = isSgFunctionType(createType(annot->at(0)));
  TERM_ASSERT(t, func_type != NULL);
  /* get functioon name*/
  EXPECT_TERM(PrologAtom*, func_name_term, annot->at(1));
  SgName func_name = func_name_term->getName();
  /* create declaration*/
  SgFunctionDeclaration* func_decl =
    new SgFunctionDeclaration(fi,func_name,func_type,/*func_def=*/NULL);
  TERM_ASSERT(t, func_decl != NULL); 
  func_decl->set_parameterList(par_list);
  setDeclarationModifier(annot->at(2),&(func_decl->get_declarationModifier()));

  /* register the function declaration with our own symbol table */
  string id = makeFunctionID(func_name, annot->at(0)->getRepresentation());
  if (declarationMap.find(id) == declarationMap.end()) {
    declarationMap[id] = func_decl;
  }
  /* make sure every function declaration has a first declaration */
  if (func_decl->get_firstNondefiningDeclaration() == NULL) {
    func_decl->set_firstNondefiningDeclaration(declarationMap[id]);
  }
  return func_decl;
}

SgFunctionDeclaration*
TermToRose::setFunctionDeclarationBody(SgFunctionDeclaration* func_decl, SgNode* func_def_u) {
  ROSE_ASSERT(func_decl != NULL);
  SgFunctionDefinition* func_def = isSgFunctionDefinition(func_def_u);
  func_decl->set_forward(func_def == NULL);
  func_decl->set_definition(func_def);
  func_decl->set_definingDeclaration(func_decl);
  func_decl->get_firstNondefiningDeclaration()
           ->set_definingDeclaration(func_decl);

  /*post processing*/
  if (func_def != NULL) { /*important: otherwise unparsing fails*/
    func_def->set_declaration(func_decl);
    /* set defining declaration ROSE 0.9.0b */
    func_decl->set_definingDeclaration(func_decl);
  } else {
  }
  return func_decl;
}


/**
 * create SgMemberFunctionDeclaration
 */
SgMemberFunctionDeclaration*
TermToRose::createMemberFunctionDeclaration(Sg_File_Info* fi, SgNode* par_list_u,SgNode* func_def_u, SgNode* ctor_list_u, PrologCompTerm* t) {
  debug("member function declaration:");
  /* cast parameter list and function definition (if exists)*/
  EXPECT_NODE(SgFunctionParameterList*, par_list, par_list_u);
  SgFunctionDefinition* func_def = 
    dynamic_cast<SgFunctionDefinition*>(func_def_u);
  EXPECT_NODE(SgCtorInitializerList*, ctor_list, ctor_list_u);
  /* get annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  /* create type*/
  EXPECT_NODE(SgFunctionType*, func_type, createType(annot->at(0)));
  /* get function name*/
  EXPECT_TERM(PrologAtom*, func_name_term, annot->at(1));
  SgName func_name = func_name_term->getName();
  /* create declaration*/
  SgMemberFunctionDeclaration* func_decl = 
    new SgMemberFunctionDeclaration(fi,func_name,func_type,func_def);
  TERM_ASSERT(t, func_decl != NULL);
  /** if there's no declaration we're dealing with a forward declatation*/
  func_decl->set_forward(func_def == NULL);
  func_decl->set_parameterList(par_list);
  func_decl->set_CtorInitializerList(ctor_list);
  /*post processing*/
  /* fake class scope*/
  PrologCompTerm* scopeTerm = isPrologCompTerm(annot->at(2));
  TERM_ASSERT(t, scopeTerm != NULL);
  string scope_name = *(toStringP(scopeTerm->at(0)));
  int scope_type  = toInt(scopeTerm->at(1));
  fakeClassScope(scope_name,scope_type,func_decl);

  /*important: otherwise unparsing fails*/
  if (func_def != NULL) {
    func_def->set_declaration(func_decl);
  }
  TERM_ASSERT(t, isSgClassDefinition(func_decl->get_class_scope()) != NULL);
  TERM_ASSERT(t, func_decl->get_class_scope()->get_declaration() != NULL);
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
TermToRose::retrieveAnnotation(PrologCompTerm* t) {
  /* the position of the annotation depends on the arity of the term*/
  EXPECT_TERM(PrologCompTerm*, a, t->at(t->getArity()-3));
  return a;
}

/**
 * create a SgGlobal node
 * and convert content
 */
SgGlobal*
TermToRose::createGlobal(Sg_File_Info* fi,deque<SgNode*>* succs) {
  /*simple constructor*/
  SgGlobal* glob = new SgGlobal(fi);
  glob->set_endOfConstruct(FI);
  debug("in SgGlobal:");
  testFileInfo(fi);
  /*add successors*/
  deque<SgNode*>::iterator it = succs->begin();
  while(it != succs->end()) {
    /* all successors that arent NULL (we don't create SgNullStatements, instead
     * we return NULL ptrs*/
    if((*it) != NULL) {
      EXPECT_NODE(SgDeclarationStatement*, curdec, *it);
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
TermToRose::createVarRefExp(Sg_File_Info* fi, PrologCompTerm* t) {
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  /* cast SgInitializedName*/
  SgInitializedName* init_name;
  string id = makeInameID(annot);
  if (initializedNameMap.find(id) != initializedNameMap.end()) {
    // Lookup the closest iname
    init_name = initializedNameMap[id];
  } else {
    cerr<<id<<endl;
    TERM_ASSERT(t, false);
    init_name = inameFromAnnot(annot);
  }
  TERM_ASSERT(t, init_name != NULL);
  /*cast variable symbol*/
  SgVariableSymbol* var_sym = new SgVariableSymbol(init_name);
  TERM_ASSERT(t, var_sym != NULL);
  /*create VarRefExp*/
  SgVarRefExp* vre = new SgVarRefExp(fi,var_sym);
  TERM_ASSERT(t, vre != NULL);
  // Set parent pointers
  //init_name->set_parent(var_sym);
  var_sym->set_parent(vre);
  return vre;
}

/**
 * create a SgAssignInitializer
 */
SgAssignInitializer*
TermToRose::createAssignInitializer(Sg_File_Info* fi, SgNode* succ, PrologCompTerm* t) {
  SgExpression* exp = dynamic_cast<SgExpression*>(succ);
  // not true.. TERM_ASSERT(t, exp != NULL);
  SgAssignInitializer* ai = new SgAssignInitializer(fi,exp);
  TERM_ASSERT(t, ai != NULL);
  return ai;
}

/**
 * create a SgBinaryOp
 */
SgBinaryOp*
TermToRose::createBinaryOp(Sg_File_Info* fi,SgNode* lnode,SgNode* rnode,PrologCompTerm* t) {
  debug("creating binary op");
  string op_name = t->getName();
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
  if (op_name == "arrow_exp") {
    cur_op = new SgArrowExp(fi,lhs,rhs,op_type);
  } else if (op_name == "dot_exp") {
    cur_op = new SgDotExp(fi,lhs,rhs,op_type);
  } else if (op_name == "dot_star_op") {
    cur_op = new SgDotStarOp(fi,lhs,rhs,op_type);
  } else if (op_name == "arrow_star_op") {
    cur_op = new SgArrowStarOp(fi,lhs,rhs,op_type);
  } else if (op_name == "equality_op") {
    cur_op = new SgEqualityOp(fi,lhs,rhs,op_type);
  } else if (op_name == "less_than_op") {
    cur_op = new SgLessThanOp(fi,lhs,rhs,op_type);
  } else if (op_name == "greater_than_op") {
    cur_op = new SgGreaterThanOp(fi,lhs,rhs,op_type);
  } else if (op_name == "not_equal_op") {
    cur_op = new SgNotEqualOp(fi,lhs,rhs,op_type);
  } else if (op_name == "less_or_equal_op") {
    cur_op = new SgLessOrEqualOp(fi,lhs,rhs,op_type);
  } else if (op_name == "greater_or_equal_op") {
    cur_op = new SgGreaterOrEqualOp(fi,lhs,rhs,op_type);
  } else if (op_name == "add_op") {
    cur_op = new SgAddOp(fi,lhs,rhs,op_type);
  } else if (op_name == "subtract_op") {
    cur_op = new SgSubtractOp(fi,lhs,rhs,op_type);
  } else if (op_name == "multiply_op") {
    cur_op = new SgMultiplyOp(fi,lhs,rhs,op_type);
  } else if (op_name == "divide_op") {
    cur_op = new SgDivideOp(fi,lhs,rhs,op_type);
  } else if (op_name == "integer_divide_op") {
    cur_op = new SgIntegerDivideOp(fi,lhs,rhs,op_type);
  } else if (op_name == "mod_op") {
    cur_op = new SgModOp(fi,lhs,rhs,op_type);
  } else if (op_name == "and_op") {
    cur_op = new SgAndOp(fi,lhs,rhs,op_type);
  } else if (op_name == "or_op") {
    cur_op = new SgOrOp(fi,lhs,rhs,op_type);
  } else if (op_name == "bit_xor_op") {
    cur_op = new SgBitXorOp(fi,lhs,rhs,op_type);
  } else if (op_name == "bit_and_op") {
    cur_op = new SgBitAndOp(fi,lhs,rhs,op_type);
  } else if (op_name == "bit_or_op") {
    cur_op = new SgBitOrOp(fi,lhs,rhs,op_type);
  } else if (op_name == "comma_op_exp") {
    cur_op = new SgCommaOpExp(fi,lhs,rhs,op_type);
  } else if (op_name == "lshift_op") {
    cur_op = new SgLshiftOp(fi,lhs,rhs,op_type);
  } else if (op_name == "rshift_op") {
    cur_op = new SgRshiftOp(fi,lhs,rhs,op_type);
  } else if (op_name == "pntr_arr_ref_exp") {
    cur_op = new SgPntrArrRefExp(fi,lhs,rhs,op_type);
  } else if (op_name == "scope_op") {
    cur_op = new SgScopeOp(fi,lhs,rhs,op_type);
  } else if (op_name == "assign_op") {
    cur_op = new SgAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == "plus_assign_op") {
    cur_op = new SgPlusAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == "minus_assign_op") {
    cur_op = new SgMinusAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == "and_assign_op") {
    cur_op = new SgAndAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == "ior_assign_op") {
    cur_op = new SgIorAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == "mult_assign_op") {
    cur_op = new SgMultAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == "div_assign_op") {
    cur_op = new SgDivAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == "mod_assign_op") {
    cur_op = new SgModAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == "xor_assign_op") {
    cur_op = new SgXorAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == "lshift_assign_op") {
    cur_op = new SgLshiftAssignOp(fi,lhs,rhs,op_type);
  } else if (op_name == "rshift_assign_op") {
    cur_op = new SgRshiftAssignOp(fi,lhs,rhs,op_type);
  }
  TERM_ASSERT(t, cur_op != NULL);
  debug("created binary op");
  return cur_op;
}

/**
 * is this string a binary op name?
 */
bool
TermToRose::isBinaryOp(string tname) {
  if (tname == "arrow_exp") {
    return true;
  } else if (tname == "dot_exp") {
    return true;
  } else if (tname == "dot_star_op") {
    return true;
  } else if (tname == "arrow_star_op") {
    return true;
  } else if (tname == "equality_op") {
    return true;
  } else if (tname == "less_than_op") {
    return true;
  } else if (tname == "greater_than_op") {
    return true;
  } else if (tname == "not_equal_op") {
    return true;
  } else if (tname == "less_or_equal_op") {
    return true;
  } else if (tname == "greater_or_equal_op") {
    return true;
  } else if (tname == "add_op") {
    return true;
  } else if (tname == "subtract_op") {
    return true;
  } else if (tname == "multiply_op") {
    return true;
  } else if (tname == "divide_op") {
    return true;
  } else if (tname == "integer_divide_op") {
    return true;
  } else if (tname == "mod_op") {
    return true;
  } else if (tname == "and_op") {
    return true;
  } else if (tname == "or_op") {
    return true;
  } else if (tname == "bit_xor_op") {
    return true;
  } else if (tname == "bit_and_op") {
    return true;
  } else if (tname == "bit_or_op") {
    return true;
  } else if (tname == "comma_op_exp") {
    return true;
  } else if (tname == "lshift_op") {
    return true;
  } else if (tname == "rshift_op") {
    return true;
  } else if (tname == "pntr_arr_ref_exp") {
    return true;
  } else if (tname == "scope_op") {
    return true;
  } else if (tname == "assign_op") {
    return true;
  } else if (tname == "plus_assign_op") {
    return true;
  } else if (tname == "minus_assign_op") {
    return true;
  } else if (tname == "and_assign_op") {
    return true;
  } else if (tname == "ior_assign_op") {
    return true;
  } else if (tname == "mult_assign_op") {
    return true;
  } else if (tname == "div_assign_op") {
    return true;
  } else if (tname == "mod_assign_op") {
    return true;
  } else if (tname == "xor_assign_op") {
    return true;
  } else if (tname == "lshift_assign_op") {
    return true;
  } else if (tname == "rshift_assign_op") {
    return true;
  } else if (tname == "arrow_exp") {
    return true;
  } else return false;
}

/**
 * create SgExprStatement
 */
SgExprStatement*
TermToRose::createExprStatement(Sg_File_Info* fi, SgNode* succ, PrologCompTerm* t) {
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
  TERM_ASSERT(t, es != NULL);
  debug("created expression statement");
  return es;
}

/**
 * create a SgVariableDeclaration
 */
SgVariableDeclaration*
TermToRose::createVariableDeclaration(Sg_File_Info* fi,deque<SgNode*>* succs,PrologCompTerm* t, SgDeclarationStatement *baseTypeDeclaration) {
  /*extract annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, t != NULL);
  /* create declaration*/
  SgVariableDeclaration* dec = new SgVariableDeclaration(fi);
  TERM_ASSERT(t, fi != NULL);
  debug("created variable declaration");

  // Struct declaration and definition in one?
  SgClassDeclaration* class_decl = 0;
  SgEnumDeclaration* enum_decl = 0;
  if (baseTypeDeclaration != NULL
   && baseTypeDeclaration->get_definingDeclaration() == baseTypeDeclaration) {
    baseTypeDeclaration->unsetForward();
    dec->set_baseTypeDefiningDeclaration(baseTypeDeclaration);
    class_decl = isSgClassDeclaration(baseTypeDeclaration);
    enum_decl = isSgEnumDeclaration(baseTypeDeclaration);
  }

  /* add initialized names*/
  SgInitializer* ini_initializer;

  deque<SgNode*>::iterator it = succs->begin();
  TERM_ASSERT(t, it != succs->end());
  for ( ; it != succs->end(); ++it) {
    if (SgInitializedName* ini_name = isSgInitializedName(*it)) {
      debug("added variable");
      ini_name->set_declptr(dec);
      ini_name->set_parent(dec);
      ini_initializer = ini_name->get_initializer();

      if (class_decl) {
	// If this is a Definition as well, insert the pointer to the
	// declaration
	SgClassType* ct = isSgClassType(ini_name->get_typeptr()->findBaseType());
	TERM_ASSERT(t, ct);
	ct->set_declaration(class_decl);
      } else if (enum_decl) {
	// If this is a Definition as well, insert the pointer to the
	// declaration
	SgEnumType* et = isSgEnumType(ini_name->get_typeptr());
	TERM_ASSERT(t, et);
	et->set_declaration(enum_decl);
      } else {
	//dec->set_definingDeclaration(dec);
	//createDummyNondefDecl(dec, FI, "", 
        //		      ini_name->get_typeptr(), ini_initializer);
      }

      dec->append_variable(ini_name,ini_initializer);
      /* fixup for ROSE 0.9.4 */
      SgVariableDefinition* def = dec->get_definition(ini_name);
      if (def != NULL) def->set_endOfConstruct(fi);  
    } else {
      cerr << (*it)->class_name() << "???" << endl; 
      TERM_ASSERT(t, false);
    }
  }
  /* set declaration modifier*/
  setDeclarationModifier(annot->at(0),&(dec->get_declarationModifier()));

  if (dec->isForward())
    dec->set_firstNondefiningDeclaration(dec);
  else {
    dec->set_definingDeclaration(dec);
    dec->set_firstNondefiningDeclaration(NULL);
  }

  /* fixup for ROSE 0.9.4 */
  SgVariableDefinition* def = dec->get_definition();
  if (def != NULL) def->set_endOfConstruct(fi);  

  return dec;
}

/**
 * create SgIfStmt
 */
SgIfStmt* 
TermToRose::createIfStmt(Sg_File_Info* fi, SgNode* child1, SgNode* child2, SgNode* child3, PrologCompTerm* t) {
  /* condition*/
  /* GB (2008-08-21): True and false bodies are now SgStatements, not
   * SgBasicBlocks anymore; changed dynamic_casts to less verbose and more
   * idiomatic is... calls. */
  SgStatement* test_stmt = isSgStatement(child1);
  TERM_ASSERT(t, test_stmt != NULL);
  /* if-branch*/
  SgStatement* true_branch = isSgStatement(child2);
  TERM_ASSERT(t, true_branch != NULL);
  /* else branch*/
  SgStatement* false_branch = isSgStatement(child3);
  SgIfStmt* if_stmt = NULL;
  /* create statement*/
  if_stmt = new SgIfStmt(fi,test_stmt,true_branch,false_branch);
  TERM_ASSERT(t, if_stmt != NULL);
  return if_stmt;
}

/** create SgDoWhileStmt*/
SgDoWhileStmt*
TermToRose::createDoWhileStmt(Sg_File_Info* fi, SgNode* child1, SgNode* child2,PrologCompTerm* t) {
  /* retrieve basic block -- there is always one*/
  SgStatement* b = isSgStatement(child1);
  /* retrieve statement */
  SgStatement* s = isSgStatement(child2);
  TERM_ASSERT(t, s != NULL);
  /* create do/while*/
  SgDoWhileStmt* d = new SgDoWhileStmt(fi,b,s);
  TERM_ASSERT(t, d != NULL);
  return d;
}

/** create SgWhileStmt*/
SgWhileStmt*
TermToRose::createWhileStmt(Sg_File_Info* fi, SgNode* child1, SgNode* child2, PrologCompTerm* t) {
  /* retrieve Statemtent*/
  SgStatement* s = isSgStatement(child1);
  TERM_ASSERT(t, s != NULL);
  /* retrieve basic block*/
  SgStatement* b = isSgStatement(child2);
  /*create while statement*/
  SgWhileStmt* w = new SgWhileStmt(fi,s,b);
  TERM_ASSERT(t, w != NULL);
  return w;
}

/** create SgForInitStatement*/
SgForInitStatement*
TermToRose::createForInitStatement(Sg_File_Info* fi,deque<SgNode*>* succs) {
  SgForInitStatement* ini = new SgForInitStatement(fi);
  ROSE_ASSERT(ini != NULL);
  /*append initializer statements*/
  deque<SgNode*>::iterator it = succs->begin();
  while(it != succs->end()) {
    EXPECT_NODE(SgStatement*, stmt, *it);
    ini->append_init_stmt(stmt);
    stmt->set_parent(ini);
    it++;
  }
  ini->set_endOfConstruct(fi);
  return ini;
}

/** create SgForStatement*/
SgForStatement*
TermToRose::createForStatement(Sg_File_Info* fi, SgNode* child1, SgNode* child2, SgNode* child3, SgNode* child4,PrologCompTerm* t) {
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
  TERM_ASSERT(t, f != NULL);
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
TermToRose::createSwitchStatement(Sg_File_Info* fi, SgNode* child1, SgNode* child2, PrologCompTerm* t) {
  SgStatement* switch_expr = NULL;
  SgBasicBlock* switch_block = NULL;
  SgSwitchStatement* s = NULL; 
  /* the first child, the switch selector must be a SgStatement*/
  switch_expr = isSgStatement(child1);
  TERM_ASSERT(t, switch_expr != NULL);
  /* the second child, the switch code, must be a SgBasicBlock*/
  switch_block = isSgBasicBlock(child2);
  TERM_ASSERT(t, switch_block != NULL);
  /* make sure a SgSwitchStatement is created*/
  s = new SgSwitchStatement(fi,switch_expr,switch_block);
  TERM_ASSERT(t, s != NULL);
  TERM_ASSERT(t, s->get_file_info() != NULL);
  return s;
}

/**
 * create SgCaseOptionStmt
 */
SgCaseOptionStmt*
TermToRose::createCaseOptionStmt(Sg_File_Info* fi, SgNode* child1, SgNode* child2, SgNode* child3, PrologCompTerm* t) {
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
  TERM_ASSERT(t, case_stmt->get_file_info() != NULL);
  return case_stmt;
}

/**
 * create SgDefaultOptionStmt
 */
SgDefaultOptionStmt*
TermToRose::createDefaultOptionStmt(Sg_File_Info* fi, SgNode* child1, PrologCompTerm* t) {
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
  TERM_ASSERT(t, default_stmt->get_file_info() != NULL);
  return default_stmt;
}

/**create a SgBreakStmt*/
SgBreakStmt*
TermToRose::createBreakStmt(Sg_File_Info* fi, PrologCompTerm* t) {
  SgBreakStmt* b = NULL;
  b = new SgBreakStmt(fi);
  TERM_ASSERT(t, b != NULL);
  return b;
}

/** create a SgContinueStmt*/
SgContinueStmt* 
TermToRose::createContinueStmt(Sg_File_Info* fi,PrologCompTerm* t) {
  SgContinueStmt* s = new SgContinueStmt(fi);
  TERM_ASSERT(t, s != NULL);
  return s;
}

/** create a SgLabelStatement for gotos */
SgLabelStatement*
TermToRose::createLabelStatement(Sg_File_Info* fi,PrologCompTerm* t) {
  /* get annotation data*/
  PrologCompTerm* u = retrieveAnnotation(t);
  TERM_ASSERT(t, u != NULL);
  /* extract the label name*/
  EXPECT_TERM(PrologAtom*, s, u->at(0));
  /* create SgLabelStatement with helper function*/
  /* makeLabel alreay asserts a non-NULL return value*/
  return makeLabel(fi,s->getName());
}

/** create a SgGotoStmt */
SgGotoStatement*
TermToRose::createGotoStatement(Sg_File_Info* fi,PrologCompTerm* t) {
  /* get annotation data*/
  PrologCompTerm* u = retrieveAnnotation(t);
  TERM_ASSERT(t, u != NULL);
  /* extract the label name*/
  EXPECT_TERM(PrologAtom*, s, u->at(0));
  /* create dummy SgLabelStatement with helper function*/
  SgLabelStatement* l = NULL; //makeLabel(FI,s->getName());
  /*makeLabel asserts a non-NULL return value*/
  /* create goto using dummy label*/
  SgGotoStatement* sgoto = new SgGotoStatement(fi,l);
  TERM_ASSERT(t, sgoto != NULL);
  gotoStatementsWithoutLabel.insert(pair<string,SgGotoStatement*>(s->getName(), sgoto));
  return sgoto;
}

/** create a SgLabelStatement from a string*/
SgLabelStatement*
TermToRose::makeLabel(Sg_File_Info* fi,string s) {
  /* we need a SgName*/
  SgName n = s;
  // FIXME: This is apparently necessary to convince the unparser..
  fi->set_classificationBitField(fi->get_classificationBitField() 
				 | Sg_File_Info::e_compiler_generated 
				 | Sg_File_Info::e_output_in_code_generation);
  SgLabelStatement* l = new SgLabelStatement(fi,n);
  ROSE_ASSERT(l != NULL);
  labelStatementsWithoutScope.push_back(l);
  return l;
}

/** create a class definition*/
SgClassDefinition*
TermToRose::createClassDefinition(Sg_File_Info* fi, deque<SgNode*>* succs,PrologCompTerm* t) {
  /*the unparser needs a Sg_File_Info for determining the end of construct
   * hence it is put in the annotation and retrieved here */
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  EXPECT_TERM(PrologCompTerm*, fi_term, annot->at(0));
  Sg_File_Info* end_of_construct = createFileInfo(fi_term);
  TERM_ASSERT(t, end_of_construct != NULL);
	
  SgClassDefinition* d = NULL;
  /*create empty class definition*/
  d = new SgClassDefinition(fi);
  TERM_ASSERT(t, d != NULL);
  /* append declarations*/
  deque<SgNode*>::iterator it = succs->begin();
  while(it != succs->end()) {
    SgDeclarationStatement* s = isSgDeclarationStatement(*it);
    TERM_ASSERT(t, s != NULL);
    d->append_member(s);
    s->set_parent(d);
    //s->set_scope(d);
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
TermToRose::createClassDeclaration(Sg_File_Info* fi,SgNode* child1 ,PrologCompTerm* t) {
  //cerr<<t->getRepresentation()<<endl;
  /* retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  /* if there is a child, it is a definition*/
  SgClassDefinition* class_def = isSgClassDefinition(child1);
  /*retrieve name, class type and type*/
  EXPECT_TERM(PrologAtom*, class_name_s, annot->at(0));
  /* get the class_type-enum (struct,class) -- not the type */
  EXPECT_TERM(PrologAtom*, p_class_type, annot->at(1));
  /* get the type*/
  EXPECT_TERM(PrologCompTerm*, type_s, annot->at(2));
  SgClassDeclaration::class_types e_class_type = 
    (SgClassDeclaration::class_types)createEnum(p_class_type, re.class_type);
  //SgClassType* sg_class_type = createClassType(type_s);
  SgName class_name = class_name_s->getName();
  SgClassDeclaration* d = 
    new SgClassDeclaration(fi, class_name, e_class_type, NULL/*sg_class_type */,
			   class_def);

  // Set the type
  TERM_ASSERT(t, d != NULL);
  SgClassType* sg_class_type = NULL;
  if (!lookupType(&sg_class_type, type_s->getRepresentation(), false)) {
    /* create a hidden forward declaration for this class; otherwise, ROSE's
     * recursive parent-pointer-setting routine dies with infinite recursion
     * on recursive class types
     * also, the AstPostProcessing stuff insists on different nodes for
     * defining and nondefining declaration */
    SgClassDeclaration *forward = NULL;
    forward = new SgClassDeclaration(FI,class_name,e_class_type,NULL,NULL);
    forward->setForward();
    forward->set_firstNondefiningDeclaration(forward);
    d->set_firstNondefiningDeclaration(forward);
    declarationStatementsWithoutScope.push_back(forward);
    sg_class_type = SgClassType::createType(forward);
    typeMap[type_s->getRepresentation()] = sg_class_type;
    /* not present yet */
    TERM_ASSERT(t, declarationMap.find(type_s->getRepresentation())
                == declarationMap.end());
    declarationMap[type_s->getRepresentation()] = forward;
    forward->set_type(sg_class_type);
  }
  d->set_type(sg_class_type);

  /* set declaration or the forward flag*/
  if(class_def != NULL) {
    class_def->set_declaration(d);
  } else {
    d->setForward();

    // SgClassDeclaration* ndd = d;
    // lookupDecl(&ndd, type_s->getRepresentation(), false);
  }

  TERM_ASSERT(t, declarationMap.find(type_s->getRepresentation())
              != declarationMap.end());
  d->set_firstNondefiningDeclaration(
      declarationMap[type_s->getRepresentation()]);

  return d;
}

SgClassDeclaration *
TermToRose::setClassDeclarationBody(SgClassDeclaration* d, SgNode *body) {
  ROSE_ASSERT(d != NULL);
  SgClassDefinition *class_def = isSgClassDefinition(body);
  if(class_def != NULL) {
    class_def->set_declaration(d);
    class_def->set_parent(d);
    d->set_definition(class_def);
    d->set_definingDeclaration(d);
    d->get_firstNondefiningDeclaration()->set_definingDeclaration(d);
    d->unsetForward();
  }
  return d;
}

/** create dummy class scope and add a declaration*/
void
TermToRose::fakeClassScope(string s, int c_type,SgDeclarationStatement* stat) {
  ROSE_ASSERT(false && "deprecated function");
  // /*create a dummy class declaration*/
  // SgClassDeclaration* d = createDummyClassDeclaration(s,c_type);
  // SgClassDefinition* def = new SgClassDefinition(
  //   FI, d);
  // d->set_parent(def);
  // // FIXME
  // def->set_parent(d);
  // ROSE_ASSERT(def != NULL);
  // /* scope is changed here as a side effect!*/
  // def->append_member(stat);
  // if (SgVariableDeclaration* vd = isSgVariableDeclaration(stat)) {
  //   debug("var ref exp class scope added");
  //   vd->set_parent(def);
  // } else {
  //   stat->set_scope(def);
  // }
}

/** create dummy namespace scope*/
void
TermToRose::fakeNamespaceScope(string s, int unnamed, SgDeclarationStatement* stat) {
  ROSE_ASSERT(false && "deprecated function");
  // SgName n = s;
  // bool u_b = (bool) unnamed;
  // SgNamespaceDefinitionStatement* def =
  //   new SgNamespaceDefinitionStatement(FI,0);
  // /* set scope*/
  // def->append_declaration(stat);
  // if (SgVariableDeclaration* vd = isSgVariableDeclaration(stat)) {
  //   debug("var ref exp namespace scope added");
  //   vd->set_parent(def);
  // } else {
  //   stat->set_scope(def);
  // }
  // /* create namespace*/
  // SgNamespaceDeclarationStatement* dec = 
  //   new SgNamespaceDeclarationStatement(FI,n,def,u_b);
  // def->set_namespaceDeclaration(dec); // AP 4.2.2008 
  // if(def != NULL) {
  //   def->set_namespaceDeclaration(dec);
  //   dec->set_forward(false);
  //   dec->set_definingDeclaration(dec);
  // }
  // ROSE_ASSERT(dec != NULL);
  // SgGlobal* dummy = new SgGlobal(FI);
  // ROSE_ASSERT(dummy != NULL);
  // dec->set_parent(dummy);
}
	

/** create dummy class declaration from name*/
SgClassDeclaration*
TermToRose::createDummyClassDeclaration(string s,int c_type) {
  //ROSE_ASSERT(false && "deprecated function");

  //Sg_File_Info* fi = Sg_File_Info::generateDefaultFileInfo();
  SgName class_name = s;
  SgClassDeclaration* d = 
    new SgClassDeclaration(FI,class_name,
			   (SgClassDeclaration::class_types)c_type,NULL,NULL);
  ROSE_ASSERT(d != NULL);
  d->setForward();
  d->set_firstNondefiningDeclaration(d);
  declarationStatementsWithoutScope.push_back(d);
  return d;
}

/** create dummy member function declaration class from name*/
SgMemberFunctionDeclaration*
TermToRose::createDummyMemberFunctionDeclaration(string s,int c_type) {
  ROSE_ASSERT(false && "deprecated function");
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
TermToRose::createClassType(PrologTerm* p) {
  SgClassType* ct = NULL;
  /* must be a composite term*/
  EXPECT_TERM(PrologCompTerm*, t, p);
  /* first term is class name*/
  string s = *toStringP(t->at(0));
  /* lookup class declaration */
  SgClassDeclaration* d = NULL;
  if (declarationMap.find(t->getRepresentation()) != declarationMap.end()) {
    d = isSgClassDeclaration(declarationMap[t->getRepresentation()]);
  } else {
    d = createDummyClassDeclaration(s, createEnum(t->at(1), re.class_type));
    declarationMap[t->getRepresentation()] = d;
  }

  TERM_ASSERT(t, d != NULL);
  string scopename = *(toStringP(t->at(2)));
  if(scopename != "::") {
    fakeNamespaceScope(scopename,0,d);
  } else {
    declarationStatementsWithoutScope.push_back(d);
  }
  /* the unparser wants this*/
  /*SgClassDefinition* classdef = new SgClassDefinition();
    d->set_definition(classdef);*/
  ct = SgClassType::createType(d);
  TERM_ASSERT(t, ct != NULL);
  return ct;
}

/**Create SgCtorInitializerList*/
SgCtorInitializerList* 
TermToRose::createCtorInitializerList(Sg_File_Info* fi,deque<SgNode*>* succs) {
  //this is a typical list node. only needs file info in constructor
  SgCtorInitializerList* l = new SgCtorInitializerList(fi);
  ROSE_ASSERT(l != NULL);
  //append constructor initializers
  deque<SgNode*>::iterator it = succs->begin();
  while(it != succs->end()) {
    EXPECT_NODE(SgInitializedName*, n, *it);
    l->append_ctor_initializer(n);
    it++;
  }
  return l;
}


/**
 * if condition is not met, output message and exit with failure*/
void
TermToRose::abort_unless(bool condition,string message) {
  if (condition) return;
  cerr << "\nFatal error while transforming Prolog to ROSE AST:\n"<< message << "\n";
  assert(condition);
}

/** create bit deque from PrologList*/
SgBitVector*
TermToRose::createBitVector(PrologTerm* t, map<string, int> names) {
  /*cast the argument to the list and extract elements*/
  EXPECT_TERM(PrologList*, l, t);
  deque<PrologTerm*>* succs = l->getSuccs();
  /*create a bit vector*/
  SgBitVector* bv = new SgBitVector();
  bv->resize(names.size(), false);
  /*extract bits from list*/
  deque<PrologTerm*>::iterator it = succs->begin();
  while(it != succs->end()) {
    EXPECT_TERM(PrologAtom*, a, *it);
    (*bv)[names[a->getName()]] = true;
    it++;
  }
  return bv;
}

/** create enum from PrologAtom */
int
TermToRose::createEnum(PrologTerm* t, map<string, int> names) {
  return names[t->getName()];
}

/**
 * create SgEnumDeclaration
 * */
SgEnumDeclaration*
TermToRose::createEnumDeclaration(Sg_File_Info* fi, deque<SgNode*>* succs, PrologCompTerm* t) {
  /*retrieve name*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, t != NULL);
  SgName e_name = *toStringP(annot->at(0));
  /* leave type blank for now*/
  SgEnumDeclaration* dec = new SgEnumDeclaration(fi,e_name,NULL);
  TERM_ASSERT(t, dec != NULL);
  /*create a type*/
  dec->set_type(SgEnumType::createType(dec));
  /* append enumerators (name or name/value)*/
  deque<SgNode*>::iterator it = succs->begin();
  SgInitializedName* iname;
  while(it != succs->end()) {
    iname = isSgInitializedName(*it);
    TERM_ASSERT(t, iname != NULL);
    dec->append_enumerator(iname);
    it++;
  }
  /* postprocessing*/
  dec->set_embedded((bool) toInt(annot->at(2)));
  pciDeclarationStatement(dec,annot->at(1));
  dec->set_definingDeclaration(dec);

  string id = annot->at(0)->getRepresentation();
  TERM_ASSERT(t, id != "" && id != "''");
  typeMap[id] = dec->get_type();
  declarationMap[id] = dec;

  return dec;
}

/**
 * Create SgTypedefDeclaration
 */
SgTypedefDeclaration*
TermToRose::createTypedefDeclaration(Sg_File_Info* fi, PrologCompTerm* t) {
  TERM_ASSERT(t, t != NULL);
  debug("typedef declaration");
  /*get annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  /*create name*/
  SgName n = *(toStringP(annot->at(0)));
  SgType* tpe = NULL;
  /*create definition, if there is one*/
  SgDeclarationStatement* decl = NULL;
  /* condition is true when a declaration is at this position*/
  PrologCompTerm* ct = isPrologCompTerm(t->at(0));
  if(ct != NULL) {
    debug("...with declaration");

    string id;
    if (ct->getName() == "class_declaration") 
      id = dynamic_cast<PrologCompTerm*>(ct->at(1))->at(2)->getRepresentation();
      //id = ct->at(1)->getRepresentation();
    else if (ct->getName() == "enum_declaration")
      id = dynamic_cast<PrologCompTerm*>(ct->at(1))->at(0)->getRepresentation();
    else id = ct->getRepresentation();
    //cerr<<"TDDECKL>>>>"<<id<<endl;
    // Try to look it up
    if (!lookupDecl(&decl, id, false)) {
      // Create it otherwise
      // decl = isSgDeclarationStatement(toRose(annot->at(2)));
      decl = isSgDeclarationStatement(toRose(t->at(0)));
      TERM_ASSERT(t, decl != NULL);
    }
    if (decl->get_definingDeclaration() != NULL) {
      // we are actually interested in defining declarations
      decl = decl->get_definingDeclaration();
    }

    TERM_ASSERT(t, decl != NULL);
    //note that the unparser seems to skip it!
  }

  /*create nested type*/
  if (tpe == NULL)
    tpe = createType(annot->at(1));

  /*create typedef declaration*/
  SgSymbol* smb = NULL;
  SgTypedefDeclaration* d = new SgTypedefDeclaration(fi,n,tpe,NULL,decl);
  SgTypedefType* tdtpe = SgTypedefType::createType(d);
  string tid = "typedef_type("+annot->at(0)->getRepresentation()+", "
    +annot->at(1)->getRepresentation()+")";
  typeMap[tid] = tdtpe;
  TERM_ASSERT(t, d != NULL);
  /* if there is a declaration, set flag and make sure it is set*/
  if(decl != NULL && !decl->isForward()) {
    d->set_typedefBaseTypeContainsDefiningDeclaration(true);
 // createDummyNondefDecl(d, FI, n, tpe, tdtpe);
  }
  if (decl != NULL)
    decl->set_parent(d);

  // Fixup the decl in the type if necessary
  //SgNamedType* nt = isSgNamedType(tpe);
  //if (nt && nt->get_declaration() == NULL) 
  //  nt->set_declaration(d);

  // Symbol table
  string id = "typedef_type("+annot->at(0)->getRepresentation()+", "
    +annot->at(1)->getRepresentation()+")";
  declarationMap[id] = d;
  //cerr<<id<<endl;
  return d;
}

/**
 * create SgPragma
 */
SgPragma*
TermToRose::createPragma(Sg_File_Info* fi, PrologCompTerm* t) {
  /* retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  /* create the name*/
  SgName sgnm = *toStringP(annot->at(0));
  /* create the Pragma*/
  SgPragma* p = new SgPragma(strdup(sgnm.str()), fi);
  TERM_ASSERT(t, p != NULL);
  return p;
}


/**
 * Post Creation Initializiatoin of SgDeclarationStatement
 * */
void
TermToRose::pciDeclarationStatement(SgDeclarationStatement* s,PrologTerm* t) {
  /* check wether t is a PrologCompTerm*/
  PrologCompTerm* atts = isPrologCompTerm(t);
  TERM_ASSERT(t, atts != NULL);
  TERM_ASSERT(t, s != NULL);
  TERM_ASSERT(t, atts->getArity() == 5);
  /* set flags (subterms are ints, implicit cast to bool)*/
  s->set_nameOnly(toInt(atts->at(0)));
  s->set_forward(toInt(atts->at(1)));
  s->set_externBrace(toInt(atts->at(2)));
  s->set_skipElaborateType(toInt(atts->at(3)));
  //s->set_need_name_qualifier(toInt(atts->at(4)));
}

/**
 * the unparser wants a parent scope for every declaration
 * for dummy declarations we have to fake it.
 */
void
TermToRose::fakeParentScope(SgDeclarationStatement* s) {
  //nothing to do if there is already a parent scope
  ROSE_ASSERT(false && "deprecated function");
  // if(s->get_parent()) return;

  // debug("faking scope");	
  // SgGlobal* dummy = new SgGlobal(FI);
  // TERM_ASSERT(t, dummy != NULL);
  // dummy->set_endOfConstruct(FI);
  // // 7.2.2008 ROSE 0.9.0b (Adrian)
  // addSymbol(dummy, s);
  

  // s->set_parent(dummy);
  // s->set_scope(dummy);
  // TERM_ASSERT(t, s->get_parent());
  // TERM_ASSERT(t, s->get_scope());
}

/**
 * the unparser now wants a symbol table entry, too
 */
void
TermToRose::addSymbol(SgScopeStatement* scope, SgDeclarationStatement* s) {
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
TermToRose::isPrologInt(PrologTerm* t) {
  return dynamic_cast<PrologInt*>(t);
}
/**
 * cast to PrologList (analogous to the is... functions of ROSE)
 */
PrologList*
TermToRose::isPrologList(PrologTerm* t) {
  return dynamic_cast<PrologList*>(t);
}
/**
 * cast to PrologAtom (analogous to the is... functions of ROSE)
 */
PrologAtom*
TermToRose::isPrologAtom(PrologTerm* t) {
  return dynamic_cast<PrologAtom*>(t);
}
/**
 * cast to PrologCompTerm (analogous to the is... functions of ROSE)
 */
PrologCompTerm*
TermToRose::isPrologCompTerm(PrologTerm* t) {
  return dynamic_cast<PrologCompTerm*>(t);
}
/** DEPRECATED
 * cast to PrologString (analogous to the is... functions of ROSE)
 *
PrologString*
TermToRose::isPrologString(PrologTerm* t) {
  return dynamic_cast<PrologString*>(t);
}*/

/**
 * create std::string* from PrologAtom*
 * including downcast from PrologTerm*.
 * If both casts fail, an assertion will fail;
 */
string*
TermToRose::toStringP(PrologTerm* t) {
  if(PrologAtom* a =isPrologAtom(t)) {
    TERM_ASSERT(t, a != NULL);
    return new string(a->getName());
  }
}

/**
 * create int from PrologInt*
 * including downcast from PrologTerm*
 * If cast fails, an assertion will fail
 */
int
TermToRose::toInt(PrologTerm* t) {
  PrologInt* i = isPrologInt(t);
  TERM_ASSERT(t, i);
  return i->getValue();
}

/**
 * create SgDeleteExp
 */
SgDeleteExp*
TermToRose::createDeleteExp(Sg_File_Info* fi, SgNode* child1, PrologCompTerm* t) {
  // get annotation
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  // cast child fitting constructor	
  SgExpression* e = isSgExpression(child1);
  TERM_ASSERT(t, e != NULL);
  // get "flags"	
  short is_array = toInt(annot->at(0));
  short need_g = toInt(annot->at(1));
  // create, test, return	
  SgDeleteExp* del = new SgDeleteExp(fi,e,is_array,need_g);
  TERM_ASSERT(t, del != NULL);
  return del;
}

/**
 * create SgExprListExp 
 */
SgExprListExp*
TermToRose::createExprListExp(Sg_File_Info* fi, deque<SgNode*>* succs) {
  /* just create SgExprListExp* and append expressions*/
  debug("SgExprListExp");
  SgExprListExp* e = new SgExprListExp(fi);
  ROSE_ASSERT(e != NULL);
  deque<SgNode*>::iterator it = succs->begin();
  while (it != succs->end()) {
    EXPECT_NODE(SgExpression*, ex, *it);
    e->append_expression(ex);
    it++;
  }
  return e;
}
/**
 * create SgRefExp*
 */
SgRefExp*
TermToRose::createRefExp(Sg_File_Info* fi, PrologCompTerm* t) {
  /* retrieve type from annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  SgType* tpe = createType(annot->at(0));
  TERM_ASSERT(t, tpe != NULL);
  SgRefExp* re = new SgRefExp(fi,tpe);
  TERM_ASSERT(t, re != NULL);
  return re;
}

/**
 * create SgVarArgOp
 * */
SgVarArgOp*
TermToRose::createVarArgOp(Sg_File_Info* fi, SgNode* child1,PrologCompTerm* t) {
  /* arg is supposed to be an expression*/
  SgExpression* c1 = isSgExpression(child1);
  TERM_ASSERT(t, c1 != NULL);
  /* retrieve type from annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  SgType* tpe = createType(annot->at(0));
  TERM_ASSERT(t, tpe != NULL);	
  SgVarArgOp* o = new SgVarArgOp(fi,c1,tpe);
  return o;
}

/**
 * create SgVarArgEndOp
 * */
SgVarArgEndOp*
TermToRose::createVarArgEndOp(Sg_File_Info* fi, SgNode* child1,PrologCompTerm* t) {
  /* arg is supposed to be an expression*/
  SgExpression* c1 = isSgExpression(child1);
  TERM_ASSERT(t, c1 != NULL);
  /* retrieve type from annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  SgType* tpe = createType(annot->at(0));
  TERM_ASSERT(t, tpe != NULL);	
  SgVarArgEndOp* o = new SgVarArgEndOp(fi,c1,tpe);
  return o;
}
/**
 * create SgVarArgStartOneOperandOp
 * */
SgVarArgStartOneOperandOp*
TermToRose::createVarArgStartOneOperandOp(Sg_File_Info* fi, SgNode* child1,PrologCompTerm* t) {
  /* arg is supposed to be an expression*/
  SgExpression* c1 = isSgExpression(child1);
  TERM_ASSERT(t, c1 != NULL);
  /* retrieve type from annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  SgType* tpe = createType(annot->at(0));
  TERM_ASSERT(t, tpe != NULL);	
  SgVarArgStartOneOperandOp* o = new SgVarArgStartOneOperandOp(fi,c1,tpe);
  return o;
}
/**
 * create SgVarArgStartOp
 * */
SgVarArgStartOp*
TermToRose::createVarArgStartOp(Sg_File_Info* fi, SgNode* child1,SgNode* child2,PrologCompTerm* t) {
  /* args are supposed to be expressions*/
  SgExpression* c1 = isSgExpression(child1);
  TERM_ASSERT(t, c1 != NULL);
  SgExpression* c2 = isSgExpression(child2);
  TERM_ASSERT(t, c2 != NULL);
  /* retrieve type from annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  SgType* tpe = createType(annot->at(0));
  TERM_ASSERT(t, tpe != NULL);	
  SgVarArgStartOp* o = new SgVarArgStartOp(fi,c1,c2,tpe);
  return o;
}

/**
 * create SgVarArgCopyOp
 * */
SgVarArgCopyOp*
TermToRose::createVarArgCopyOp(Sg_File_Info* fi, SgNode* child1,SgNode* child2,PrologCompTerm* t) {
  /* args are supposed to be expressions*/
  SgExpression* c1 = isSgExpression(child1);
  TERM_ASSERT(t, c1 != NULL);
  SgExpression* c2 = isSgExpression(child2);
  TERM_ASSERT(t, c2 != NULL);
  /* retrieve type from annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  SgType* tpe = createType(annot->at(0));
  TERM_ASSERT(t, tpe != NULL);	
  SgVarArgCopyOp* o = new SgVarArgCopyOp(fi,c1,c2,tpe);
  return o;
}

/**
 * create SgAccessModifier
 */
SgAccessModifier*
TermToRose::createAccessModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  TERM_ASSERT(t, c != NULL);
  SgAccessModifier* a = new SgAccessModifier();
  TERM_ASSERT(t, a != NULL);
  a->set_modifier((SgAccessModifier::access_modifier_enum) 
		  createEnum(c->at(0), re.access_modifier));
  return a;
}
/**
 * create SgBaseClassModifier
 */
SgBaseClassModifier*
TermToRose::createBaseClassModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  TERM_ASSERT(t, c != NULL);
  SgBaseClassModifier* b = new SgBaseClassModifier();
  TERM_ASSERT(t, b != NULL);
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
TermToRose::createFunctionModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  TERM_ASSERT(t, c != NULL);
  //extract bit vector list and create bit vector
  // ( cast done in createBitVector)
  SgBitVector b = *(createBitVector(c->at(0), re.function_modifier));
  SgFunctionModifier* m = new SgFunctionModifier();
  TERM_ASSERT(t, m != NULL);
  m->set_modifierVector(b);
  return m;
}
/**
 * create SgSpecialFunctionModifier
 */
SgSpecialFunctionModifier*
TermToRose::createSpecialFunctionModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  TERM_ASSERT(t, c != NULL);
  //extract bit vector list and create bit vector
  // ( cast done in createBitVector)
  SgBitVector b = *(createBitVector(c->at(0), 
				    re.special_function_modifier));
  SgSpecialFunctionModifier* m = new SgSpecialFunctionModifier();
  TERM_ASSERT(t, m != NULL);
  m->set_modifierVector(b);
  return m;
}

/**
 * create SgStorageModifier
 */
SgStorageModifier*
TermToRose::createStorageModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  TERM_ASSERT(t, c != NULL);
  SgStorageModifier* a = new SgStorageModifier();
  TERM_ASSERT(t, a != NULL);
  a->set_modifier((SgStorageModifier::storage_modifier_enum)
	  createEnum(c->at(0), re.storage_modifier));
  return a;
}
/**
 * create SgLinkageModifier
 */
SgLinkageModifier*
TermToRose::createLinkageModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  TERM_ASSERT(t, c != NULL);
  SgLinkageModifier* a = new SgLinkageModifier();
  TERM_ASSERT(t, a != NULL);
  a->set_modifier((SgLinkageModifier::linkage_modifier_enum) toInt(c->at(0)));
  return a;
}
/**
 * create SgElaboratedTypeModifier
 */
SgElaboratedTypeModifier*
TermToRose::createElaboratedTypeModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  TERM_ASSERT(t, c != NULL);
  SgElaboratedTypeModifier* a = new SgElaboratedTypeModifier();
  TERM_ASSERT(t, a != NULL);
  a->set_modifier((SgElaboratedTypeModifier::elaborated_type_modifier_enum) toInt(c->at(0)));
  return a;
}

/**
 * create SgConstVolatileModifier
 */
SgConstVolatileModifier*
TermToRose::createConstVolatileModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  TERM_ASSERT(t, c != NULL);
  SgConstVolatileModifier* a = new SgConstVolatileModifier();
  TERM_ASSERT(t, a != NULL);
  a->set_modifier((SgConstVolatileModifier::cv_modifier_enum)
		  createEnum(c->at(0), re.cv_modifier));
  return a;
}
/**
 * create SgUPC_AccessModifier
 */
SgUPC_AccessModifier*
TermToRose::createUPC_AccessModifier(PrologTerm* t) {
  PrologCompTerm* c = isPrologCompTerm(t);
  TERM_ASSERT(t, c != NULL);
  SgUPC_AccessModifier* a = new SgUPC_AccessModifier();
  TERM_ASSERT(t, a != NULL);
  a->set_modifier((SgUPC_AccessModifier::upc_access_modifier_enum)
		  createEnum(c->at(0), re.upc_access_modifier));
  return a;
}

/**
 * create SgTypeModifier
 */
SgTypeModifier*
TermToRose::createTypeModifier(PrologTerm* t) {
  /* create modifier*/
  SgTypeModifier* m = new SgTypeModifier();
  TERM_ASSERT(t, m != NULL);
  /* set modifier*/
  setTypeModifier(t,m);
  return m;
}


/**
 * set SgTypeModifier's values
 */
void
TermToRose::setTypeModifier(PrologTerm* t, SgTypeModifier* tm) {
  /* cast*/
  PrologCompTerm* c = isPrologCompTerm(t);
  TERM_ASSERT(t, c != NULL);
  TERM_ASSERT(t, tm != NULL);
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
TermToRose::createDeclarationModifier(PrologTerm* t) {
  SgDeclarationModifier* d = new SgDeclarationModifier();
  TERM_ASSERT(t, d != NULL);
  /* set values*/
  setDeclarationModifier(t,d);
  return d;	
}

/**
 * set SgDeclarationModifier's values
 */
void
TermToRose::setDeclarationModifier(PrologTerm* t, SgDeclarationModifier* d) {
  debug("setting modifier");
  TERM_ASSERT(t, d != NULL);
  /* cast*/
  PrologCompTerm* c = isPrologCompTerm(t);
  TERM_ASSERT(t, c != NULL);
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
TermToRose::createAggregateInitializer(Sg_File_Info* fi,SgNode* child1,PrologCompTerm* t) {
  //Child must be a SgExprListExp
  SgExprListExp* e = isSgExprListExp(child1);
  TERM_ASSERT(t, e != NULL);
  // create node
  SgAggregateInitializer* i = new SgAggregateInitializer(fi,e);
  TERM_ASSERT(t, i != NULL);
  return i;
}


/**
 * create dummy SgFunctionDeclaration
 */
SgFunctionDeclaration*
TermToRose::createDummyFunctionDeclaration(string* namestr, PrologTerm* type_term) {
  ROSE_ASSERT(false && "deprecated function");
  ROSE_ASSERT(namestr != NULL);
  ROSE_ASSERT(type_term != NULL);
  /* create SgName and SgFunctionType from arguments*/
  SgName n = *(namestr);
  EXPECT_NODE(SgFunctionType*, tpe, createType(type_term));
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
TermToRose::createDummyFunctionSymbol(string* namestr, PrologTerm* type_term) {
  ROSE_ASSERT(false && "deprecated function");
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
TermToRose::createDummyMemberFunctionSymbol(PrologTerm* annot_term) {
  /* retrieve representation of member function from annotation*/
  debug("mfs");
  TERM_ASSERT(annot_term, annot_term != NULL);
  PrologCompTerm* annot = isPrologCompTerm(annot_term);
  /* use generic term to rose function for creating a SgNode and try to cast*/
  SgNode* mfunc_uncast = toRose(annot->at(0));
  TERM_ASSERT(annot_term, mfunc_uncast != NULL);
  SgMemberFunctionDeclaration* mfunc = 
    isSgMemberFunctionDeclaration(mfunc_uncast);
  TERM_ASSERT(annot_term, mfunc != NULL);
  /*add class scope*/
  PrologCompTerm* scope_term = isPrologCompTerm(annot->at(1));
  /*scope name and type*/
  debug("creating scope for member function declaration for symbol ");
  string scope_name = *(toStringP(scope_term->at(0)));
  int scope_type = toInt(scope_term->at(1));
  fakeClassScope(scope_name,scope_type,mfunc);
  TERM_ASSERT(annot_term, mfunc->get_class_scope() != NULL);
  /* create symbol */
  SgMemberFunctionSymbol* s = new SgMemberFunctionSymbol(mfunc);
  TERM_ASSERT(annot_term, s != NULL);
  return s;
}


/**
 * create SgFunctionRefExp
 */
SgFunctionRefExp*
TermToRose::createFunctionRefExp(Sg_File_Info* fi, PrologCompTerm* ct) {
  /* extract pointer to string containing the name
   * and a PrologTerm* with the type info from annotation*/
  TERM_ASSERT(ct, ct != NULL);
  PrologCompTerm* annot = retrieveAnnotation(ct);
  TERM_ASSERT(ct, annot != NULL);
  ARITY_ASSERT(annot, 3);
  string* s = toStringP(annot->at(0));
  TERM_ASSERT(ct, s != NULL);

  SgFunctionSymbol* sym;
  string id = makeFunctionID(*s, annot->at(1)->getRepresentation());
  SgFunctionDeclaration* decl = NULL;
  if (lookupDecl(&decl, id)) {
    /* get the real symbol */
    sym = new SgFunctionSymbol(decl);
  } else {
    cerr<<id<<endl;
    TERM_ASSERT(ct, false);
    /* create function symbol*/
    debug("symbol");
    sym = createDummyFunctionSymbol(s,annot->at(1));
  }
  TERM_ASSERT(ct, sym != NULL);

  /* get type from function symbol*/
  SgFunctionType* ft = isSgFunctionType(sym->get_type());
  TERM_ASSERT(ct, ft != NULL);
  /* create SgFunctionRefExp*/
  SgFunctionRefExp* re = new SgFunctionRefExp(fi,sym,ft);
  TERM_ASSERT(ct, re != NULL);
  return re;
}

/**
 * create SgMemberFunctionRefExp
 */
SgMemberFunctionRefExp*
TermToRose::createMemberFunctionRefExp(Sg_File_Info* fi, PrologCompTerm* ct) {
  /* extract pointer to string containing the name
   * and a PrologTerm* with the type info from annotation*/
  TERM_ASSERT(ct, ct != NULL);
  PrologCompTerm* annot = retrieveAnnotation(ct);
  ARITY_ASSERT(annot, 3);
  /* create member function symbol*/
  SgMemberFunctionSymbol* sym = createDummyMemberFunctionSymbol(annot->at(0));
  TERM_ASSERT(ct, sym!= NULL);
  /* virtual call?*/
  int vc = toInt(annot->at(1));
  /* create type*/
  SgFunctionType* tpe = isSgFunctionType(sym->get_type());
  TERM_ASSERT(ct, tpe != NULL);
  /* need qualifier?*/
  int nc = toInt(annot->at(3));
  SgMemberFunctionRefExp* ref = new SgMemberFunctionRefExp(fi,sym,vc,tpe,nc);
  TERM_ASSERT(ct, ref != NULL);
  return ref;
}

/**
 * create SgNamespaceDefinitionStatement
 */
SgNamespaceDefinitionStatement*
TermToRose::createNamespaceDefinitionStatement(Sg_File_Info* fi, deque<SgNode*>* succs) {
  debug("now creating namespace definition");
  /* create definition (declaration is set later)*/
  SgNamespaceDefinitionStatement* d = new SgNamespaceDefinitionStatement(fi,NULL);
  ROSE_ASSERT(d != NULL);
  /* append declarations*/
  deque<SgNode*>::iterator it = succs->begin();
  while(it != succs->end()) {
    EXPECT_NODE(SgDeclarationStatement*, s, *it);
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
TermToRose::createNamespaceDeclarationStatement(Sg_File_Info* fi, SgNode* child1, PrologCompTerm* t) {
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  SgNamespaceDefinitionStatement* def = isSgNamespaceDefinitionStatement(child1);
  if (def == NULL) {
    debug("namespace definition is NULL");
  }
  SgName n = *(toStringP(annot->at(0)));
  bool unnamed = (bool) toInt(annot->at(1));
  SgNamespaceDeclarationStatement* dec = new SgNamespaceDeclarationStatement(fi,n,def,unnamed);
  TERM_ASSERT(t, dec != NULL);
  if(def != NULL) {
    def->set_namespaceDeclaration(dec);
    createDummyNondefDecl(dec, FI, n, (SgNamespaceDefinitionStatement*)NULL, 
			  unnamed);
  } else {
    dec->setForward();
    dec->set_firstNondefiningDeclaration(dec);
  }
  return dec;
}


/**
 * create SgFunctionCallExp
 */
SgFunctionCallExp*
TermToRose::createFunctionCallExp(Sg_File_Info* fi, SgNode* child1, SgNode* child2, PrologCompTerm* ct) {
  /* cast children*/
  EXPECT_NODE(SgExpression*, re, child1);
  EXPECT_NODE(SgExprListExp*, el, child2);
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
TermToRose::createTryStmt(Sg_File_Info* fi, SgNode* child1, SgNode* child2, PrologCompTerm* ct) {
  /* first child is a SgStatement*/
  EXPECT_NODE(SgStatement*,b,child1);
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
TermToRose::createCatchOptionStmt(Sg_File_Info* fi, SgNode* child1, SgNode* child2, PrologCompTerm* ct) {
  EXPECT_NODE(SgVariableDeclaration*, dec, child1);
  EXPECT_NODE(SgStatement*, bl, child2);
  SgCatchOptionStmt* s = new SgCatchOptionStmt(fi,dec,bl,NULL);
  ROSE_ASSERT(s != NULL);
  return s;
}
/**
 * create SgCatchStatementSeq
 */
SgCatchStatementSeq*
TermToRose::createCatchStatementSeq(Sg_File_Info* fi, deque<SgNode*>* succs) {
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
TermToRose::createThisExp(Sg_File_Info* fi, PrologCompTerm* ct) {
  /*the unparser does not use the class name*/
  SgThisExp* t = new SgThisExp(fi,NULL);
  ROSE_ASSERT(t != NULL);
  return t;
}

/**
 * create a SgConstructorInitializer
 */
SgConstructorInitializer*
TermToRose::createConstructorInitializer(Sg_File_Info* fi, SgNode* child1,PrologCompTerm* t) {
  /*retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, t != NULL);
  /* get class name*/
  string s = *toStringP(annot->at(0));
  /* create a class for unparsing the name*/
  SgMemberFunctionDeclaration* decl = createDummyMemberFunctionDeclaration(s,0);
  TERM_ASSERT(t, decl != NULL);
  /* cast the SgExprListExp*/
  SgExprListExp* el = isSgExprListExp(child1);
  /* create constructor initializer, need_name = true*/
  SgConstructorInitializer* ci = new SgConstructorInitializer(fi,decl,el,NULL,true,false,false,false);
  TERM_ASSERT(t, ci != NULL);
  ci->set_is_explicit_cast(1);
  return ci;
}

/**
 * create a SgPragmaDeclaration
 */
SgPragmaDeclaration*
TermToRose::createPragmaDeclaration(Sg_File_Info* fi, SgNode* child1,PrologCompTerm* t) {
  /*retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, t != NULL);
  /* cast the SgPragma*/
  SgPragma* p = isSgPragma(child1);
  /* create constructor initializer, need_name = true*/
  SgPragmaDeclaration* pd = new SgPragmaDeclaration(fi,p);
  TERM_ASSERT(t, pd != NULL);
  return pd;
}

/**
 * create a SgNewExp
 */
SgNewExp*
TermToRose::createNewExp(Sg_File_Info* fi,SgNode* child1,SgNode* child2, SgNode* child3,PrologCompTerm* t) {
  /*retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  /* retrieve type*/
  SgType* tpe = createType(annot->at(0));
  TERM_ASSERT(t, tpe != NULL);
  // get expression list expression
  SgExprListExp* ele = isSgExprListExp(child1);
  // get constructor initializer
  SgConstructorInitializer* ci = isSgConstructorInitializer(child2);
  // get expression
  SgExpression* ex = isSgExpression(child3);
  // create SgNewExp
  SgNewExp* ne = new SgNewExp(fi,tpe,ele,ci,ex,0);
  TERM_ASSERT(t, ne != NULL);
  return ne;
}

/**
 * create a SgConditionalExp
 */
SgConditionalExp*
TermToRose::createConditionalExp(Sg_File_Info* fi,SgNode* child1,SgNode* child2, SgNode* child3,PrologCompTerm* t) {
  /*retrieve annotation*/
  PrologCompTerm* annot = retrieveAnnotation(t);
  TERM_ASSERT(t, annot != NULL);
  /* retrieve type*/
  SgType* tpe = createType(annot->at(0));
  TERM_ASSERT(t, tpe != NULL);
  SgExpression* exp1 = isSgExpression(child1);
  TERM_ASSERT(t, exp1 != NULL);
  SgExpression* exp2 = isSgExpression(child2);
  TERM_ASSERT(t, exp2 != NULL);
  SgExpression* exp3 = isSgExpression(child3);
  TERM_ASSERT(t, exp3 != NULL);

  SgConditionalExp* exp = new SgConditionalExp(fi, exp1, exp2, exp3, tpe);
  TERM_ASSERT(t, exp != NULL);
  return exp;
}


/** issue a warning*/
void
TermToRose::warn_msg(string msg) {
  /* since this is only a warning, i think stdout is okay*/
  cerr << "/*" << msg << "*/\n";
}

/** output a debug message, unless
 * compiled with NDEBUG*/
void
TermToRose::debug(string message) {
  //cerr << message << "\n";
}
