
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "rewrite.h"
#include "sageBuilder.h"
#include <iostream>
#include <iomanip>
#include "pre.h"


// DQ (8/1/2005): test use of new static function to create 
// Sg_File_Info object that are marked as transformations
#undef SgNULL_FILE
#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

#include "replaceExpressionWithStatement.h"
#include "inlinerSupport.h"

using namespace SageInterface;
// void FixSgTree(SgNode*);
// void FixSgProject(SgProject&);

SgExpression* generateAssignmentMaybe(SgExpression* lhs, SgExpression* rhs)
   {
  // If lhs is NULL, return rhs without doing an assignment
  // If lhs is not NULL, assign rhs to it
  // Used as a helper in inliner

#if 0
     if (lhs)
          return new SgAssignOp(SgNULL_FILE, lhs, rhs);
       else
          return rhs;
#else
     SgExpression* returnAssignmentOperator = NULL;
     if (lhs != NULL)
        {
          returnAssignmentOperator = new SgAssignOp(SgNULL_FILE, lhs, rhs);
          returnAssignmentOperator->set_endOfConstruct(SgNULL_FILE);
          printf ("Built a SgAssignOp = %p \n",returnAssignmentOperator);
        }
       else
          returnAssignmentOperator = rhs;

     return returnAssignmentOperator;
#endif
   }

// Change all return statements in a block of code to assignments to a
// variable and gotos to a given label.  Used internally by the inliner.
class ChangeReturnsToGotosVisitor: public AstSimpleProcessing
   {
     private:
     SgLabelStatement* label;
     SgExpression* where_to_write_answer;

     public:
     ChangeReturnsToGotosVisitor(SgLabelStatement* label,SgExpression* where_to_write_answer)
        : label(label), where_to_write_answer(where_to_write_answer) {}

     virtual void visit(SgNode* n)
        {
	  SgReturnStmt* rs = isSgReturnStmt(n);
          if (rs)
             {
	       // std::cout << "Converting return statement " << rs->unparseToString();
	       // std::cout << " into possible assignment to " << where_to_write_answer->unparseToString();
	       // std::cout << " and jump to " << label->get_name().getString() << std::endl;
               SgExpression* return_expr = rs->get_expression();
               SgBasicBlock* block = SageBuilder::buildBasicBlock();
            // printf ("Building IR node #1: new SgBasicBlock = %p \n",block);
               if (return_expr)
                  {
                    SgExpression* assignment = generateAssignmentMaybe(where_to_write_answer,return_expr);
		    if (where_to_write_answer)
		      where_to_write_answer->set_parent(assignment);
		    if (return_expr != assignment)
		      return_expr->set_parent(assignment);
                    SgStatement* assign_stmt = SageBuilder::buildExprStatement(assignment);
                    SageInterface::appendStatement(assign_stmt, block);
                  }

            // block->get_statements().push_back(new SgGotoStatement(SgNULL_FILE, label));
               SgGotoStatement* gotoStatement = new SgGotoStatement(SgNULL_FILE, label);
               gotoStatement->set_endOfConstruct(SgNULL_FILE);
               ROSE_ASSERT(n->get_parent() != NULL);
            // printf ("Building gotoStatement #2 = %p  n->get_parent() = %p = %s \n",gotoStatement,n->get_parent(),n->get_parent()->class_name().c_str());
               SageInterface::appendStatement(gotoStatement, block);
               isSgStatement(n->get_parent())->replace_statement(rs, block);
	       block->set_parent(n->get_parent());
               ROSE_ASSERT(gotoStatement->get_parent() != NULL);
             }
        }
   };

// One curried step of the inlining process.  This class just rearranges
// the parameters and sends them on to ChangeReturnsToGotosVisitor.
class ChangeReturnsToGotosPrevisitor: public SageInterface::StatementGenerator {
  SgLabelStatement* end_of_inline_label;
  SgStatement* funbody_copy;

  public:
  ChangeReturnsToGotosPrevisitor(SgLabelStatement* end, SgStatement* body):
    end_of_inline_label(end), funbody_copy(body) {}

  virtual SgStatement* generate(SgExpression* where_to_write_answer) {
    // std::cout << "ChangeReturnsToGotosPrevisitor.generate: " << end_of_inline_label->get_name().getString() << " returning into " << where_to_write_answer->unparseToString() << std::endl;
    ChangeReturnsToGotosVisitor(end_of_inline_label, where_to_write_answer).
      traverse(funbody_copy, postorder);
    return funbody_copy;
  }
};

// This class replaces all uses of this to references to a specified
// variable.  Used as part of inlining non-static member functions.
class ReplaceThisWithRefVisitor: public AstSimpleProcessing {
  SgVariableSymbol* sym;
  
  public:
  ReplaceThisWithRefVisitor(SgVariableSymbol* sym): sym(sym) {}
    
  virtual void visit(SgNode* n) {
    if (isSgThisExp(n)) {
      SgVarRefExp* vr = new SgVarRefExp(SgNULL_FILE, sym);
      vr->set_endOfConstruct(SgNULL_FILE);
      isSgExpression(n->get_parent())->
        replace_expression(isSgExpression(n), vr);
    }
  }
};

// This class replaces all variable references to point to new symbols
// based on a map.  It is used to replace references to the parameters
// of an inlined procedure with new variables.
class ReplaceParameterUseVisitor: public AstSimpleProcessing {
  public:
  typedef std::map<SgInitializedName*, SgVariableSymbol*> paramMapType;

  private:
  const paramMapType& paramMap;

  public:
  ReplaceParameterUseVisitor(const paramMapType& paramMap):
    paramMap(paramMap) {}

  virtual void visit(SgNode* n) {
    SgVarRefExp* vr = isSgVarRefExp(n);
    if (!vr) return;
    SgInitializedName* in = vr->get_symbol()->get_declaration();
    paramMapType::const_iterator iter = paramMap.find(in);
    if (iter == paramMap.end()) return; // This is not a parameter use
    vr->set_symbol(iter->second);
  }
};

// Convert a declaration such as "A x = A(1, 2);" into "A x(1, 2);".  This is
// always (IIRC) safe to do by C++ language rules, even if A has a nontrivial
// copy constructor and/or destructor.
// FIXME (bug in another part of ROSE) -- the output from this routine unparses
// as if no changes had occurred, even though the PDF shows the transformation
// correctly.
void removeRedundantCopyInConstruction(SgInitializedName* in) {
  SgAssignInitializer* ai = isSgAssignInitializer(in->get_initializer());
  ROSE_ASSERT (ai);
  SgInitializer* realInit = isSgInitializer(ai->get_operand());
  ROSE_ASSERT (realInit);
  ROSE_ASSERT (isSgConstructorInitializer(realInit));
  in->set_initializer(realInit);
  realInit->set_parent(in);
  // FIXME -- do we need to delete ai?
}

// Main inliner code.  Accepts a function call as a parameter, and inlines
// only that single function call.  Returns true if it succeeded, and false
// otherwise.  The function call must be to a named function, static member
// function, or non-virtual non-static member function, and the function
// must be known (not through a function pointer or member function
// pointer).  Also, the body of the function must already be visible.
// Recursive procedures are handled properly (when allowRecursion is set), by
// inlining one copy of the procedure into itself.  Any other restrictions on
// what can be inlined are bugs in the inliner code.
bool
doInline(SgFunctionCallExp* funcall, bool allowRecursion)
   {
     SgExpression* funname = funcall->get_function();
     SgExpression* funname2 = isSgFunctionRefExp(funname);
     SgDotExp* dotexp = isSgDotExp(funname);
     SgArrowExp* arrowexp = isSgArrowExp(funname);
     SgExpression* thisptr = 0;
     if (dotexp || arrowexp)
        {
          funname2 = isSgBinaryOp(funname)->get_rhs_operand();
          if (dotexp) {
	    SgExpression* lhs = dotexp->get_lhs_operand();

	    // FIXME -- patch this into p_lvalue
	    bool is_lvalue = lhs->get_lvalue();
	    if (isSgInitializer(lhs)) is_lvalue = false;

	    if (!is_lvalue) {
	      SgAssignInitializer* ai = SageInterface::splitExpression(lhs);
	      ROSE_ASSERT (isSgInitializer(ai->get_operand()));
	      SgInitializedName* in = isSgInitializedName(ai->get_parent());
	      ROSE_ASSERT (in);
	      removeRedundantCopyInConstruction(in);
	      lhs = dotexp->get_lhs_operand(); // Should be a var ref now
	    }
	    thisptr = new SgAddressOfOp(SgNULL_FILE, lhs);
	  } else if (arrowexp) {
	    thisptr = arrowexp->get_lhs_operand();
	  } else {
	    assert (false);
	  }
        }

     if (!funname2)
        {
       // std::cout << "Inline failed: not a call to a named function" << std::endl;
          return false; // Probably a call through a fun ptr
        }

     SgFunctionSymbol* funsym = 0;
     if (isSgFunctionRefExp(funname2))
          funsym = isSgFunctionRefExp(funname2)->get_symbol();
       else
          if (isSgMemberFunctionRefExp(funname2))
               funsym = isSgMemberFunctionRefExp(funname2)->get_symbol();
            else
               assert (false);

     assert (funsym);
     if (isSgMemberFunctionSymbol(funsym) && isSgMemberFunctionSymbol(funsym)->get_declaration()->get_functionModifier().isVirtual())
        {
       // std::cout << "Inline failed: cannot inline virtual member functions" << std::endl;
          return false;
        }

     SgFunctionDeclaration* fundecl = funsym->get_declaration();
     SgFunctionDefinition* fundef = fundecl->get_definition();
     if (!fundef)
        {
       // std::cout << "Inline failed: no definition is visible" << std::endl;
          return false; // No definition of the function is visible
        }
     if (!allowRecursion)
        {
          SgNode* my_fundef = funcall;
          while (my_fundef && !isSgFunctionDefinition(my_fundef))
             {
            // printf ("Before reset: my_fundef = %p = %s \n",my_fundef,my_fundef->class_name().c_str());
               my_fundef = my_fundef->get_parent();
               ROSE_ASSERT(my_fundef != NULL);
            // printf ("After reset: my_fundef = %p = %s \n",my_fundef,my_fundef->class_name().c_str());
             }
       // printf ("After reset: my_fundef = %p = %s \n",my_fundef,my_fundef->class_name().c_str());
          assert (isSgFunctionDefinition(my_fundef));
          if (isSgFunctionDefinition(my_fundef) == fundef)
             {
               std::cout << "Inline failed: trying to inline a procedure into itself" << std::endl;
               return false;
             }
        }

     SgVariableDeclaration* thisdecl = 0;
     SgName thisname("this__");
     thisname << ++gensym_counter;
     SgInitializedName* thisinitname = 0;
     if (isSgMemberFunctionSymbol(funsym) && !fundecl->get_declarationModifier().get_storageModifier().isStatic())
        {
          SgType* thisptrtype = thisptr->get_type();
	  const SgSpecialFunctionModifier& specialMod = 
	    funsym->get_declaration()->get_specialFunctionModifier();
	  if (specialMod.isConstructor()) {
	    SgFunctionType* ft = funsym->get_declaration()->get_type();
	    ROSE_ASSERT (ft);
	    SgMemberFunctionType* mft = isSgMemberFunctionType(ft);
	    ROSE_ASSERT (mft);
	    SgType* ct = mft->get_class_type();
	    thisptrtype = new SgPointerType(ct);
	  }
          SgConstVolatileModifier& thiscv = fundecl->get_declarationModifier().get_typeModifier().get_constVolatileModifier();
       // if (thiscv.isConst() || thiscv.isVolatile()) { FIXME
          thisptrtype = new SgModifierType(thisptrtype);
          isSgModifierType(thisptrtype)->get_typeModifier().get_constVolatileModifier() = thiscv;
       // }
       // cout << thisptrtype->unparseToString() << " --- " << thiscv.isConst() << " " << thiscv.isVolatile() << endl;
          SgAssignInitializer* assignInitializer = new SgAssignInitializer(SgNULL_FILE, thisptr);
          assignInitializer->set_endOfConstruct(SgNULL_FILE);
       // thisdecl = new SgVariableDeclaration(SgNULL_FILE, thisname, thisptrtype, new SgAssignInitializer(SgNULL_FILE, thisptr));
          thisdecl = new SgVariableDeclaration(SgNULL_FILE, thisname, thisptrtype, assignInitializer);
          thisdecl->set_endOfConstruct(SgNULL_FILE);
	  thisdecl->get_definition()->set_endOfConstruct(SgNULL_FILE);
	  thisdecl->set_definingDeclaration(thisdecl);

          thisinitname = (thisdecl->get_variables()).back();
          //thisinitname = lastElementOfContainer(thisdecl->get_variables());
          // thisinitname->set_endOfConstruct(SgNULL_FILE);
	  assignInitializer->set_parent(thisinitname);

       // printf ("Built new SgVariableDeclaration #1 = %p \n",thisdecl);

       // DQ (6/23/2006): New test
          ROSE_ASSERT(assignInitializer->get_parent() != NULL);
        }

     std::cout << "Trying to inline function " << fundecl->get_name().str() << std::endl;
     SgBasicBlock* funbody_raw = fundef->get_body();
     SgInitializedNamePtrList& params = fundecl->get_args();
     SgInitializedNamePtrList::iterator i;
     SgExpressionPtrList& funargs = funcall->get_args()->get_expressions();
     SgExpressionPtrList::iterator j;
     //int ctr; // unused variable, Liao
     std::vector<SgInitializedName*> inits;
     SgTreeCopy tc;
     SgFunctionDefinition* function_copy = isSgFunctionDefinition(fundef->copy(tc));
     ROSE_ASSERT (function_copy);
     SgBasicBlock* funbody_copy = function_copy->get_body();


     SgFunctionDefinition* targetFunction = PRE::getFunctionDefinition(funcall);

     renameLabels(funbody_copy, targetFunction);
     std::cout << "Original symbol count: " << funbody_raw->get_symbol_table()->size() << std::endl;
     std::cout << "Copied symbol count: " << funbody_copy->get_symbol_table()->size() << std::endl;
     // std::cout << "Original symbol count f: " << fundef->get_symbol_table()->size() << std::endl;
     // std::cout << "Copied symbol count f: " << function_copy->get_symbol_table()->size() << std::endl;
     // We don't need to keep the copied function definition now that the
     // labels in it have been moved to the target function.  Having it in the
     // memory pool confuses the AST tests.
     function_copy->set_declaration(NULL);
     function_copy->set_body(NULL);
     delete function_copy;
     function_copy = NULL;
#if 0
     SgPragma* pragmaBegin = new SgPragma("start_of_inline_function", SgNULL_FILE);
     SgPragmaDeclaration* pragmaBeginDecl = new SgPragmaDeclaration(SgNULL_FILE, pragmaBegin);
     pragmaBeginDecl->set_endOfConstruct(SgNULL_FILE);
     pragmaBegin->set_parent(pragmaBeginDecl);
     pragmaBeginDecl->set_definingDeclaration(pragmaBeginDecl);
     funbody_copy->prepend_statement(pragmaBeginDecl);
     pragmaBeginDecl->set_parent(funbody_copy);
#endif
     ReplaceParameterUseVisitor::paramMapType paramMap;
     for (i = params.begin(), j = funargs.begin(); i != params.end() && j != funargs.end(); ++i, ++j)
        {
          SgAssignInitializer* ai = new SgAssignInitializer(SgNULL_FILE, *j, (*i)->get_type());
          ROSE_ASSERT(ai != NULL);
          ai->set_endOfConstruct(SgNULL_FILE);
          SgName shadow_name((*i)->get_name());
          shadow_name << "__" << ++gensym_counter;
          SgVariableDeclaration* vardecl = new SgVariableDeclaration(SgNULL_FILE,shadow_name,(*i)->get_type(),ai);
	  vardecl->set_definingDeclaration(vardecl);
	  vardecl->set_endOfConstruct(SgNULL_FILE);
	  vardecl->get_definition()->set_endOfConstruct(SgNULL_FILE);

          printf ("Built new SgVariableDeclaration #2 = %p = %s initializer = %p \n",vardecl,shadow_name.str(),(*(vardecl->get_variables().begin()))->get_initializer());

          vardecl->set_parent(funbody_copy);
          SgInitializedName* init = (vardecl->get_variables()).back();
          // init->set_endOfConstruct(SgNULL_FILE);
          inits.push_back(init);
	  ai->set_parent(init);
	  init->set_scope(funbody_copy);
          funbody_copy->get_statements().insert(funbody_copy->get_statements().begin() + (i - params.begin()), vardecl);
	  SgVariableSymbol* sym = new SgVariableSymbol(init);
	  paramMap[*i] = sym;
          funbody_copy->insert_symbol(shadow_name, sym);
          sym->set_parent(funbody_copy->get_symbol_table());
        }

     if (thisdecl)
        {
          thisdecl->set_parent(funbody_copy);
	  thisinitname->set_scope(funbody_copy);
          funbody_copy->get_statements().insert(funbody_copy->get_statements().begin(), thisdecl);
          SgVariableSymbol* thisSym = new SgVariableSymbol(thisinitname);
          funbody_copy->insert_symbol(thisname, thisSym);
          thisSym->set_parent(funbody_copy->get_symbol_table());
          ReplaceThisWithRefVisitor(thisSym).traverse(funbody_copy, postorder);
        }
     ReplaceParameterUseVisitor(paramMap).traverse(funbody_copy, postorder);

     SgName end_of_inline_name = "rose_inline_end__";
     end_of_inline_name << ++gensym_counter;
     SgLabelStatement* end_of_inline_label = new SgLabelStatement(SgNULL_FILE, end_of_inline_name);
     end_of_inline_label->set_endOfConstruct(SgNULL_FILE);

#if 0
     printf ("\n\nCalling AST copy mechanism on a SgBasicBlock \n");

  // Need to set the parent of funbody_copy to avoid error.
     funbody_copy->set_parent(funbody_raw->get_parent());

     printf ("This is a copy of funbody_raw = %p to build funbody_copy = %p \n",funbody_raw,funbody_copy);

     printf ("funbody_raw->get_statements().size()  = %zu \n",funbody_raw->get_statements().size());
     printf ("funbody_copy->get_statements().size() = %zu \n",funbody_copy->get_statements().size());

     printf ("funbody_raw->get_symbol_table()->size()  = %d \n",(int)funbody_raw->get_symbol_table()->size());
     printf ("funbody_copy->get_symbol_table()->size() = %d \n",(int)funbody_copy->get_symbol_table()->size());

     printf ("Output the symbol table for funbody_raw \n");
     funbody_raw->get_symbol_table()->print("debugging copy problem");

  // printf ("Output the symbol table for funbody_copy \n");
  // funbody_copy->get_symbol_table()->print("debugging copy problem");

     SgProject* project_copy = TransformationSupport::getProject(funbody_raw);
     ROSE_ASSERT(project_copy != NULL);

     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 4000;
     generateAstGraph(project_copy,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

  // printf ("Exiting as a test after testing the symbol table \n");
  // ROSE_ASSERT(false);

     funbody_copy->append_statement(end_of_inline_label);
     end_of_inline_label->set_scope(targetFunction);
     SgLabelSymbol* end_of_inline_label_sym = new SgLabelSymbol(end_of_inline_label);
     end_of_inline_label_sym->set_parent(targetFunction->get_symbol_table());
     targetFunction->get_symbol_table()->insert(end_of_inline_label->get_name(), end_of_inline_label_sym);
  // To ensure that there is some statement after the label
     SgExprStatement* dummyStatement = SageBuilder::buildExprStatement(SageBuilder::buildNullExpression());
     dummyStatement->set_endOfConstruct(SgNULL_FILE);
     funbody_copy->append_statement(dummyStatement);
     dummyStatement->set_parent(funbody_copy);
#if 0
     SgPragma* pragmaEnd = new SgPragma("end_of_inline_function", SgNULL_FILE);
     SgPragmaDeclaration* pragmaEndDecl = new SgPragmaDeclaration(SgNULL_FILE, pragmaEnd);
     pragmaEndDecl->set_endOfConstruct(SgNULL_FILE);
     pragmaEnd->set_parent(pragmaEndDecl);
     pragmaEndDecl->set_definingDeclaration(pragmaEndDecl);
     funbody_copy->append_statement(pragmaEndDecl);
     pragmaEndDecl->set_parent(funbody_copy);
#endif
     // std::cout << "funbody_copy is " << funbody_copy->unparseToString() << std::endl;

     ChangeReturnsToGotosPrevisitor previsitor = ChangeReturnsToGotosPrevisitor(end_of_inline_label, funbody_copy);
     // std::cout << "funbody_copy 2 is " << funbody_copy->unparseToString() << std::endl;
     replaceExpressionWithStatement(funcall, &previsitor);
  // std::cout << "Inline succeeded " << funcall->get_parent()->unparseToString() << std::endl;
     return true;
   }
