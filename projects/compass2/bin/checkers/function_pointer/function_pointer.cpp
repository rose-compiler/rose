/**
 * \file    functionPointer.C
 * \author  Justin Too <too1@llnl.gov>
 * \date    September 9, 2011
 */

#include "rose.h"
#include "string_functions.h"

#include "compass2/compass.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const functionPointerChecker;

Compass::ProjectPrerequisite Compass::projectPrerequisite;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_FUNCTION_POINTER_H
#define COMPASS_FUNCTION_POINTER_H

namespace CompassAnalyses
{
/**
 * \brief Detect function pointers and function references.
 */
namespace FunctionPointer
{
  extern const string checker_name;
  extern const string short_description;
  extern const string long_description;

  /**
   * \brief Specificaiton of checker results.
   */
  class CheckerOutput: public Compass::OutputViolationBase {
   public:
    explicit CheckerOutput(SgNode *const node);

   private:
    DISALLOW_COPY_AND_ASSIGN(CheckerOutput);
  };

  /**
   * \brief Specification of AST traversal.
   */
  class Traversal : public Compass::AstSimpleProcessingWithRunFunction {
   public:
    Traversal(Compass::Parameters inputParameters,
              Compass::OutputObject *output);

    /**
     * \brief Get inherited attribute for traversal.
     *
     * We are not using an inherited attribute for this checker.
     *
     * \returns NULL
     */
    void* initialInheritedAttribute() const
      {
        return NULL;
      }

    void run(SgNode *n)
      {
        this->traverse(n, preorder);
      }

    void visit(SgNode *n);

    /*-----------------------------------------------------------------------
     * Utilities
     *---------------------------------------------------------------------*/
    bool IsFunctionPointer(SgNode *const node);
    bool IsFunctionPointerType(const SgType *const type);

    /*-----------------------------------------------------------------------
     * Accessors / Mutators
     *---------------------------------------------------------------------*/
    string source_directory() const { return source_directory_; }
    void set_source_directory(const string &source_directory)
      {
        source_directory_ = source_directory;
      }

    Compass::OutputObject* output() const { return output_; }
    void set_output(Compass::OutputObject *const output)
      {
        output_ = output;
      }

   private:
    /*-----------------------------------------------------------------------
     * Attributes
     *---------------------------------------------------------------------*/
    string source_directory_; ///< Restrict analysis to user input files.
    Compass::OutputObject* output_;

    /*-----------------------------------------------------------------------
     * Utilities
     *---------------------------------------------------------------------*/
    DISALLOW_COPY_AND_ASSIGN(Traversal);
  };
} // ::CompassAnalyses
} // ::FunctionPointer
#endif // COMPASS_FUNCTION_POINTER_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace FunctionPointer
 {
  const string checker_name      = "FunctionPointer";
  const string short_description = "Function pointer detected.";
  const string long_description  = "This analysis looks for function \
      pointers (includes function dereferences)";
 }
}

CompassAnalyses::FunctionPointer::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::functionPointerChecker->checkerName,
                          ::functionPointerChecker->shortDescription) {}

CompassAnalyses::FunctionPointer::Traversal::
Traversal(Compass::Parameters a_parameters, Compass::OutputObject* output)
    : output_(output)
  {
    try
    {
        string target_directory = a_parameters["general::target_directory"].front();
        source_directory_.assign(target_directory);
    }
    catch (Compass::ParameterNotFoundException e)
    {
        std::cout << "ParameterNotFoundException: " << e.what() << std::endl;
        homeDir(source_directory_);
    }
  }

//////////////////////////////////////////////////////////////////////////////

bool
CompassAnalyses::FunctionPointer::Traversal::
IsFunctionPointerType(const SgType *const type)
  {
    ROSE_ASSERT( type != NULL );

    bool isBaseTypeFunction = isSgFunctionType(type->findBaseType()) != NULL;
    return isBaseTypeFunction       &&
           (isSgReferenceType(type) ||
            isSgPointerType(type)   ||
            isSgTypedefType(type)   ||
            isSgArrayType(type)
           );
  }

bool
CompassAnalyses::FunctionPointer::Traversal::
IsFunctionPointer(SgNode *const node)
  {
    //std::cout << "== " << node->class_name() << std::endl;
    switch( node->variantT() )
    {
      //=======================================================================
      case V_SgVarRefExp:
      //=======================================================================
      // Function-pointer-type variable reference
       {
         if (IsFunctionPointerType(isSgVarRefExp(node)->get_type()))
         {
             //std::cout << "OK SgVarRefExp = function pointer type" << std::endl;
             return true;
         }
       } break;
      //=======================================================================
      case V_SgInitializedName:
      //=======================================================================
       {
         SgInitializedName* initialized_name = isSgInitializedName(node);

         // NOTE: only if we use the case V_SgTemplateFunctionInstantiationDecl
         // Skip template instantiations
         //SgFunctionParameterList* function_param_list =
         //  isSgFunctionParameterList(initialized_name->get_parent());
         //if ( function_param_list != NULL )
         //{
         //    if (isSgTemplateInstantiationFunctionDecl(
         //         function_param_list->get_parent()))
         //    {
         //        //std::cout << "SKIP SgInitializedName in template instantiation" << std::endl;
         //        return false;
         //    }
         //}

         SgType* type = initialized_name->get_type();
         if (IsFunctionPointerType(type))
         {
             //std::cout << "OK SgInitializedName" << std::endl;
             return true;
         }
       } break;
      //=======================================================================
      case V_SgAssignInitializer:
      //=======================================================================
       {
         SgNode* operand_i = isSgAssignInitializer(node)->get_operand_i();
         if (isSgFunctionRefExp(operand_i))
         {
             //std::cout << "OK SgAssignInitializer" << std::endl;
             return true;
         }
         else if (isSgCastExp(operand_i))
         {
             //std::cout << "OK SgAssignInitializer::operand_i=SgCastExp" << std::endl;
             return IsFunctionPointer(operand_i);
         }
       } break;
      //=======================================================================
      case V_SgCastExp:
      //=======================================================================
       {
         SgCastExp* cast = isSgCastExp(node);
         //std::cout << "SgCastExp operand_i = " << cast->get_operand_i()->class_name() << std::endl;
         if (isSgFunctionRefExp(cast->get_operand_i()))
         {
             //std::cout << "OK SgCastExp::SgFunctionRefExp" << std::endl;
             return true;
         }
       } break;
      //=======================================================================
      case V_SgExprStatement:
      //=======================================================================
       {
         SgExpression* expression = isSgExprStatement(node)->get_expression();
         if (isSgFunctionRefExp(expression))
         {
             //std::cout << "OK SgExprStatement (expression)> SgFunctionRefExp" << std::endl;
             return true;
         }
       } break;
      //=======================================================================
      case V_SgReturnStmt:
      //=======================================================================
       {
         SgExpression* expression = isSgReturnStmt(node)->get_expression();
         if (isSgFunctionRefExp(expression))
         {
             //std::cout << "OK SgReturnStmt (expression)> SgFunctionRefExp" << std::endl;
             return true;
         }
         else if (isSgCastExp(expression))
         {
             //std::cout << "OK SgReturnStmt::expression=SgCastExp" << std::endl;
             return IsFunctionPointer(expression);
         }
       } break;
      //=======================================================================
      case V_SgTypedefDeclaration:
      //=======================================================================
       {
         SgType* base_type = isSgTypedefDeclaration(node)->get_base_type();
         if (IsFunctionPointerType(base_type))
         {
             //std::cout << "OK SgTypedefDeclaration" << std::endl;
             return true;
         }
       } break;
      // NOTE: picked up by SgInitializedName
      //=======================================================================
      //case V_SgTemplateInstantiationFunctionDecl:
      ////=======================================================================
      //// TODO: isForward and !isForward are both detected => duplicate.
      ////       A template instantiation generates both a forward and defining
      ////       function definition.
      //// TODO: each instantiation of a single templated function will
      //// be traversed => "duplicates" will be detected for each instantation.
      // {
      //   SgTemplateArgumentPtrList template_args =
      //     isSgTemplateInstantiationFunctionDecl(node)->get_templateArguments();

      //   bool match = false;
      //   SgTemplateArgumentPtrList::iterator it;
      //   for ( it = template_args.begin();
      //         it != template_args.end();
      //         ++it )
      //   {
      //       SgTemplateArgument* template_arg = (*it);
      //       if (IsFunctionPointerType(template_arg->get_type()))
      //       {
      //           std::cout << "OK SgTemplateArgument" << std::endl;
      //           match = true;
      //       }
      //       return match;
      //   }
      // } break;
      //=======================================================================
      case V_SgFunctionCallExp:
      //=======================================================================
      // TODO: multiple casts, ... ?
      // TODO: could there be something in-between the casts
      // TODO: ideally, we should check for PointerDerefExp > ... > FunctionCallExp
       {
         SgExpression* fcall_exp = isSgFunctionCallExp(node)->get_function();
         SgPointerDerefExp* ptr_deref_exp = isSgPointerDerefExp(fcall_exp);
         if (ptr_deref_exp != NULL )
         {
             SgNode* operand_i = ptr_deref_exp->get_operand_i();
             if (isSgFunctionCallExp(operand_i) || isSgCastExp(operand_i))
             {
                 //std::cout << "OK SgFunctionCallExp (function)> SgPointerDerefExp" << std::endl;
                 return true;
             }
         }
       } break;
      //=======================================================================
      default:
      //=======================================================================
       {
         //====================================================================
         SgFunctionDeclaration* func_decl = isSgFunctionDeclaration(node);
         //====================================================================
         if ( func_decl != NULL )
         {
             SgType* return_type = func_decl->get_type()->get_return_type();
             if (IsFunctionPointerType(return_type))
             {
                 //std::cout << "OK SgFunctionDeclaration = function pointer Return type" << std::endl;
                 return true;
             }
         }
       } break;
    }// switch node->variantT
   return false;
  }// IsFunctionPointer

void
CompassAnalyses::FunctionPointer::Traversal::
visit(SgNode* node)
  {
    SgLocatedNode* located_node = isSgLocatedNode(node);
    if (located_node != NULL &&
        Compass::IsNodeInUserLocation(located_node, source_directory_))
    {
        if (IsFunctionPointer(node) == true)
        {
            output_->addOutput(new CheckerOutput(node));
        }
    }
  } //End of the visit function.

// Checker main run function and metadata

static void
run(Compass::Parameters params, Compass::OutputObject* output)
  {
    CompassAnalyses::FunctionPointer::Traversal(params, output).run(
      Compass::projectPrerequisite.getProject());
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return new CompassAnalyses::FunctionPointer::Traversal(params, output);
  }

extern const Compass::Checker* const functionPointerChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::FunctionPointer::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::FunctionPointer::short_description,
      CompassAnalyses::FunctionPointer::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

