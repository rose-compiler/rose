// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "rose_config.h"

#ifndef USE_ROSE_PHP_SUPPORT // If PHP is disabled then compile a stub


#include <iostream>

int php_main(std::string, SgFile*)
{
    std::cerr << "ROSE was built without PHP support" << std::endl;
    return -1;
}

#else // Else if PHP *is enabled*


#include <AST.h>
#include <AST_fold.h>
#include <parsing/parse.h>
#include <cmdline.h>
#include <embed/embed.h>
#include <pass_manager/Pass_manager.h>

using namespace AST;
struct gengetopt_args_info args_info;
Pass_manager* pm = 0;

using namespace SageBuilder;
using namespace SageInterface;

class ConvertMethodMod : public Method_mod, public virtual SgNode
{  
};

class AST_to_ROSE : public AST::Fold
<
 SgExpression*,                 // Actual_parameter*
 SgExpression*,                                 // Array*
 SgNode*,                       // Array_elem*
 SgExpression*,                                 // Assignment*
 SgNode*,                       // Attr_mod*
 SgStatement*,                  // Attribute*
 SgBoolValExp*,                                 // BOOL*
 SgExpression*,                                 // Bin_op*
 SgBreakStmt*,                                  // Break*
 SgName*,                       // CAST*
 SgName*,                       // CLASS_NAME*
 SgName*,                       // CONSTANT_NAME*
 SgExpression*,                                 // Cast*
 SgStatement*,                  // Catch*
 SgClassDefinition*,            // Class_def*
 SgNode*,                       // Class_mod*
 SgName*,                       // Class_name*
 SgStatement*,                  // Commented_node*
 SgExpression*,                                 // Conditional_expr*
 SgExpression*,                 // Constant*
 SgContinueStmt*,                               // Continue*
 SgName*,                       // DIRECTIVE_NAME*
 SgStatement*,                  // Declare*  TODO part of directive, probably not needed
 SgNode*,                       // Directive*  TODO probably not needed
 SgDoWhileStmt*,                                // Do*
 SgExprStatement*,                              // Eval_expr*
 SgExpression*,                                 // Expr*
 SgForStatement*,                               // For*
 SgForStatement*,                               // Foreach*  TODO See if mappable to Fortran foreach
 SgExpression*,                 // Foreign*   TODO Not sure what to do with this to allow PHC's template for the superclass to compile cleanly
 SgInitializedName*,            // Formal_parameter*
 SgStatement*,                                  // Global*
 SgIntVal*,                                             // INT*
 SgName*,                       // INTERFACE_NAME*
 SgName*,                       // Identifier*
 SgIfStmt*,                                             // If*
 SgExpression*,                                 // Ignore_errors*
 SgExpression*,                 // Instanceof*
 SgClassDefinition*,            // Interface_def*
 SgExpression*,                                 // List_assignment*
 SgNode*,                       // List_element*
 SgNode*,                       // Literal*
 SgName*,                       // METHOD_NAME*
 SgNode*,                       // Member*
 SgFunctionDeclaration*,        // Method*
 SgFunctionCallExp*,            // Method_invocation*
 ConvertMethodMod*,             // Method_mod*
 SgName*,                       // Method_name*
 SgNullExpression*,             // NIL*
 SgName*,                       // Name_with_default*
 SgNode*,                       // Nested_list_elements*
 SgExpression*,                                 // New*
 SgNode*,                       // Node*
 SgStatement*,                  // Nop*
 SgName*,                                               // OP*
 SgExpression*,                                 // Op_assignment*
 SgFile*,                       // PHP_script*
 SgExpression*,                                 // Post_op*
 SgExpression*,                                 // Pre_op*
 SgDoubleVal*,                                  // REAL*
 SgName*,                       // Reflection*
 SgReturnStmt*,                                 // Return*
 SgStringVal*,                                  // STRING*
 SgFunctionParameterList*,      // Signature*
 SgNode*,                       // Source_rep*
 SgStatement*,                  // Statement*
 SgStatement*,                  // Static_declaration*
 SgSwitchStatement*,            // Switch*
 SgBasicBlock*,                 // Switch_case*
 SgNode*,                       // Target* - HIR::Targets have VARIABLE_NAME expr, so we cant fold to them directly
 SgStatement*,                                  // Throw*
 SgStatement*,                  // Try*
 SgType*,                       // Type*
 SgExpression*,                 // Unary_op*
 SgName*,                       // VARIABLE_NAME*
 SgVarRefExp*,                  // Variable*
 SgName*,                       // Variable_name*
 SgWhileStmt*,                  // While*
 list                           // Use std::list instead of PHC's list type
>
{
public:
    SgFile *file;

    AST_to_ROSE(SgFile *fileToModify) : file(fileToModify)
    {
    }

private:
    SgBasicBlock* statementListToBasicBlock(list<SgStatement*>* sl)
    {
        SgBasicBlock* blk = buildBasicBlock();
        for (list<SgStatement*>::iterator i = sl->begin();
             i != sl->end(); i++)
            blk->append_statement(*i);
        return blk;
    }
    
public:

    // TODO Despite being able to override this to fix the "foreign"
    // issue, we still have to fix the base class because the template
    // instantiation still violates the return types
        /*
    virtual SgExpression* fold_expr(Expr* in)
    {
        ROSE_ASSERT(0);
        return 0;
    }
    */

    virtual SgExpression* fold_foreign_impl(Foreign* foreign)
    {
        ROSE_ASSERT(0);
        return NULL;
    }

        virtual SgFile* fold_php_script(PHP_script* in)
        {
        ROSE_ASSERT(file);
        pushScopeStack(file->get_globalScope());

                list<SgStatement*>* statements = 0;
                {
                        statements = new list<SgStatement*>;
                        list<Statement*>::const_iterator i;
                        for(i = in->statements->begin(); i != in->statements->end(); i++)
                                if(*i) statements->push_back(fold_statement(*i));
                                else statements->push_back(0);
                }

        list<SgStatement*>::const_iterator i;
        for(i = statements->begin(); i != statements->end(); i++)
        {
            SgDeclarationStatement* decl =
                dynamic_cast<SgDeclarationStatement*>(*i);
            if (decl) {
                appendStatement(decl);
            }
            // TODO what else is in here that we are skipping?
            // PHP can have more than declarations at the top level
        }
        
        popScopeStack();
                return file;
        }

    virtual ConvertMethodMod* fold_impl_method_mod(Method_mod* orig,
                                                   bool is_public,
                                                   bool is_protected,
                                                   bool is_private,
                                                   bool is_static,
                                                   bool is_abstract,
                                                   bool is_final)
    {
        ConvertMethodMod* m = new ConvertMethodMod();
        m->is_public = is_public;
        m->is_protected = is_protected;
        m->is_private = is_private;
        // TODO FINISH THIS
        ROSE_ASSERT(m);
        return m;
    }

        virtual SgName* fold_method_name(METHOD_NAME* orig)
    {
        // NOTICE: Any method accepts a SgName that has been folded
        // needs to delete it.  It would be ideal to return the SgName
        // by value here, but the template requires that a pointer is
        // returned.
        return new SgName(*orig->value);
    }

        virtual SgName* fold_variable_name(AST::VARIABLE_NAME* orig)
    {
        // NOTICE: Any method accepts a SgName that has been folded
        // needs to delete it.  It would be ideal to return the SgName
        // by value here, but the template requires that a pointer is
        // returned.

        // TODO Just prepend $ to all var names to fix
        // conflicts with function names?
        return new SgName(*orig->value);
    }

        virtual SgVarRefExp* fold_impl_variable(Variable* orig,
                                            SgNode* target,
                                            SgName* variable_name,
                                            list<SgExpression*>* array_indices)
    {
        // NOTE in PHP an assignment is essentially also a declaration
        // if it has not been seen before, we need to add an explicit
        // declaration to make ROSE happy otherwise it can't unparse

        // TODO What is to be done with the target?
        if (!topScopeStack()->symbol_exists(*variable_name))
        {
            SgAssignInitializer* init = buildAssignInitializer(buildIntVal(0));
            // TODO We could easily just return with with the expr as the
            // initializer here and cut out the extra statement
            SgVariableDeclaration* decl =
                buildVariableDeclaration(*variable_name,
                                         SgTypeDefault::createType(),
                                         init,
                                         topScopeStack());
            decl->setCompilerGenerated();
            appendStatement(decl);
        }

        SgVarRefExp* var = buildVarRefExp(*variable_name, topScopeStack());
        // TODO handle array_indicies
        return var;
    }

        virtual SgIntVal* fold_int(INT* orig)
    {
        return buildIntVal(orig->value);
    }

        virtual SgExpression* fold_impl_assignment(Assignment* orig,
                                               SgVarRefExp* variable,
                                               bool is_ref,
                                               SgExpression* expr)
    {
        SgName name = variable->get_symbol()->get_name();
        // TODO Unfortunately for us Assignment is an SgExprStatement
        SgExprStatement* s = buildAssignStatement(variable, expr);
        return s->get_expression();
    }

        virtual SgExprStatement* fold_impl_eval_expr(Eval_expr* orig,
                                                 SgExpression* expr)
    {
        return buildExprStatement(expr);
    }

        virtual SgReturnStmt* fold_impl_return(Return* orig,
                                           SgExpression* expr)
    {
        return buildReturnStmt(expr);
    }

    // This fold is overridden *just* to wrap with scope
        virtual SgFunctionDeclaration* fold_method(Method* in)
        {
                SgFunctionParameterList* signature = 0;
                if(in->signature) signature = fold_signature(in->signature);

        SgFunctionDeclaration* decl =
            buildDefiningFunctionDeclaration(
                                         *in->signature->method_name->value,
                                         SgTypeDefault::createType(),
                                         signature,
                                         topScopeStack());
        pushScopeStack(decl->get_definition()->get_body());

                list<SgStatement*>* statements = 0;
        if (in->statements)
                {
                        statements = new list<SgStatement*>;
                        list<Statement*>::const_iterator i;
                        for(i = in->statements->begin(); i != in->statements->end(); i++)
                                if(*i) statements->push_back(fold_statement(*i));
                                else statements->push_back(0);
                }

        for (list<SgStatement*>::iterator i = statements->begin();
             i != statements->end(); i++)
        {
            if (!*i)     // TODO change to an assert once cleaned up
                continue;
            appendStatement(*i);
        }

        popScopeStack();
        return decl;
        }

        virtual SgFunctionParameterList*
    fold_impl_signature(Signature* orig,
                        ConvertMethodMod* method_mod,
                        bool is_ref,
                        SgName* method_name,
                        list<SgInitializedName*>* formal_parameters)
    {
        // TODO use all modifiers, is_ref, etc
        SgFunctionParameterList* pl = buildFunctionParameterList();
        for (list<SgInitializedName*>::iterator i = formal_parameters->begin();
             i != formal_parameters->end(); i++)
        {
            appendArg(pl, *i);
        }
        return pl;
    }

        virtual SgStatement* fold_impl_nop(Nop* orig)
    {
        return 0;
    }

        virtual SgFunctionCallExp*
    fold_impl_method_invocation(Method_invocation* orig,
                                SgNode* target,
                                SgName* method_name,
                                list<SgExpression*>* actual_parameters)
    {
        SgExprListExp* el = new SgExprListExp();
        for (list<SgExpression*>::iterator i = actual_parameters->begin();
             i != actual_parameters->end(); i++)
        {
            el->append_expression(*i);
        }
        SgFunctionCallExp* fc = buildFunctionCallExp(*method_name,
                                                     SgTypeDefault::createType(),
                                                     el,
                                                     topScopeStack());
        return fc;
    }

        virtual SgName* fold_op(OP* orig)
    {
        return new SgName(*orig->value);
    }

        virtual SgExpression* fold_impl_op_assignment(Op_assignment* orig,
                                                  SgVarRefExp* variable,
                                                  SgName* op,
                                                  SgExpression* expr)
    {
        SgName name = variable->get_symbol()->get_name();
        // TODO Unfortunately for us Assignment is an SgExprStatement
        SgExpression* e = 0;
        if (op->getString() == "+")
        {
            e = buildPlusAssignOp(variable, expr);
        }
        else if (op->getString() == "-")
        {
            e = buildMinusAssignOp(variable, expr);
        }
        else if (op->getString() == "*")
        {
            e = buildMultAssignOp(variable, expr);
        }
        else if (op->getString() == "/")
        {
            e = buildDivAssignOp(variable, expr);
        }
        else
        {
            ROSE_ASSERT(0);
        }
        ROSE_ASSERT(e);
        return e;
    }

        virtual SgExpression* fold_impl_bin_op(Bin_op* orig,
                                           SgExpression* left,
                                           SgName* op,
                                           SgExpression* right)
    {
        SgExpression* e = 0;
        if (op->getString() == "+")
        {
            e = buildAddOp(left, right);
        }
        else if (op->getString() == "-")
        {
            e = buildSubtractOp(left, right);
        }
        else if (op->getString() == "*")
        {
            e = buildMultiplyOp(left, right);
        }
        else if (op->getString() == "/")
        {
            // TODO where is this function - there is no buildDivideOp!?
            //e = buildDivideOp(left, right);
            ROSE_ASSERT(0);
        }
        else if (op->getString() == "<")
        {
            e = buildLessThanOp(left, right);
        }
        else if (op->getString() == ">")
        {
            e = buildGreaterThanOp(left, right);
        }
        else if (op->getString() == "<=")
        {
            e = buildLessOrEqualOp(left, right);
        }
        else if (op->getString() == ">=")
        {
            e = buildGreaterOrEqualOp(left, right);
        }
        else if (op->getString() == "==")
        {
            e = buildEqualityOp(left, right);
        }
        else
        {
            // TODO "===" needs to be implemented
            ROSE_ASSERT(0);
        }
        ROSE_ASSERT(e);
        return e;
    }

        virtual SgName* fold_interface_name(INTERFACE_NAME* orig)
    {
        return new SgName(*orig->value);
    }

        virtual SgStringVal* fold_string(STRING* orig)
    {
        return buildStringVal(*orig->value);
    }

        virtual SgExpression* fold_impl_actual_parameter(Actual_parameter* orig,
                                                     bool is_ref,
                                                     SgExpression* expr)
    {
        // TODO deal with is_ref
        return expr;
    }

        virtual SgType* fold_impl_type(Type* orig,
                                   SgName* class_name)
    {
        SgType *t = 0;
        if (!class_name)
        {
            t = SgTypeDefault::createType();
        }
        else if (*class_name == "int")
        {
            t = buildIntType();
        }
        else
        {
            t = SgTypeDefault::createType();
        }
        ROSE_ASSERT(t);
        return t;
    }

        virtual SgName* fold_impl_name_with_default(Name_with_default* orig,
                                                SgName* variable_name,
                                                SgExpression* expr)
    {
        // TODO handle default value
        return variable_name;
    }

        virtual SgInitializedName*
    fold_impl_formal_parameter(Formal_parameter* orig,
                               SgType* type,
                               bool is_ref,
                               SgName* var)
    {
       return buildInitializedName(*var, type);
    }

        virtual SgIfStmt* fold_impl_if(If* orig,
                                   SgExpression* expr,
                                   list<SgStatement*>* iftrue,
                                   list<SgStatement*>* iffalse)
    {
        SgBasicBlock* trueBranch = statementListToBasicBlock(iftrue);
        SgBasicBlock* falseBranch = statementListToBasicBlock(iffalse);
        SgExprStatement* exprStmt = buildExprStatement(expr);
        return buildIfStmt(exprStmt, trueBranch, falseBranch);
    }

        virtual SgExpression* fold_impl_post_op(Post_op* orig,
                                            SgVarRefExp* variable,
                                            SgName* op)
    {
        SgExpression* e = 0;
        if (op->getString() == "++")
        {
            e = buildPlusPlusOp(variable, SgUnaryOp::postfix);
        }
        else if (op->getString() == "--")
        {
            e = buildMinusMinusOp(variable, SgUnaryOp::postfix);
        }
        else
        {
            ROSE_ASSERT(0);
        }
        ROSE_ASSERT(e);
        return e;
    }

        virtual SgExpression* fold_impl_pre_op(Pre_op* orig,
                                           SgVarRefExp* variable,
                                           SgName* op)
    {
        SgExpression* e = 0;
        if (op->getString() == "++")
        {
            e = buildPlusPlusOp(variable, SgUnaryOp::prefix);
        }
        else if (op->getString() == "--")
        {
            e = buildMinusMinusOp(variable, SgUnaryOp::prefix);
        }
        else
        {
            ROSE_ASSERT(0);
        }
        ROSE_ASSERT(e);
        return e;
    }

        virtual SgForStatement* fold_impl_for(For* orig,
                                          SgExpression* init,
                                          SgExpression* cond,
                                          SgExpression* incr,
                                          list<SgStatement*>* statements)
    {
        //SgBasicBlock* blk = statementListToBasicBlock(statements);
        // TODO what to do with init?
        //return buildForStatement(cond, incr, blk);
        return 0;
    }

        virtual SgWhileStmt* fold_impl_while(While* orig,
                                         SgExpression* expr,
                                         list<SgStatement*>* statements)
    {
        SgBasicBlock* blk = statementListToBasicBlock(statements);
        return buildWhileStmt(expr, blk);
    }

};


int php_main(string filename, SgFile *file) {    
    ROSE_ASSERT(file);

    PHP::startup_php();

    String sfilename(filename);
    PHP_script* ir = parse(&sfilename, 0);
    ROSE_ASSERT(ir);

    AST_to_ROSE* trans = new AST_to_ROSE(file);
    trans->fold_php_script(ir);

    return 0;
}
#endif // else (ROSE_PHP is defined)
