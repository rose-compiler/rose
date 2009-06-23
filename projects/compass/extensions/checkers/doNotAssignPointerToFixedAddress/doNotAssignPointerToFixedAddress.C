// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2:foldmethod=syntax

// Pointer Fixed Address Assignment Detector
// Author: David J. Hamilton
// Date: 15-June-2009

#include "rose.h"
#include "compass.h"

#define DEBUG 0


#define VISIT(x) if(x) visit(x)
#define IS_ILLEGAL_CONSTANT(x) (x && is_illegal_constant(x))
#define HAS_ILLEGAL_CONSTANT(x) (x && has_illegal_constant(x))
#define IS_ZERO(x) (x && 0 == x->get_value())


extern const Compass::Checker* const doNotAssignPointerToFixedAddressChecker;

namespace CompassAnalyses
   { 
     namespace DoNotAssignPointerToFixedAddress
        {
            bool is_zero_value( SgValueExp* n);
            bool is_illegal_constant( SgInitializer* n);
            bool is_illegal_constant( SgValueExp* n);
            bool is_illegal_constant( SgExpression* n);
            bool is_illegal_constant( SgCastExp* n);
            bool is_illegal_constant( SgAssignInitializer* n);
            bool is_illegal_constant( SgBinaryOp* n);
            bool has_illegal_constant( SgAggregateInitializer* n);

            bool is_constant( SgExpression* n);
            bool is_pointer( SgType* n);

        /*! \brief Pointer Fixed Address Assignment: Common Weakness Enumeration
         * (CWE) 587, pointers should not be assigned fixed addresses.
         */

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node, std::string message);
             };

       // Specification of Checker Traversal Implementation
          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction  
             {
                    Compass::OutputObject* output;
            // Checker specific parameters should be allocated here.

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                    void *initialInheritedAttribute() const { return NULL; }

                    void run(SgNode* n) { 
#if DEBUG
                      generateDOT( *isSgProject( n));
#endif // debug
                      this->traverse(n, preorder); 
                    }

                    void visit(SgNode* n);

                protected:
                    void visit( SgInitializedName* n);
                    void visit( SgAssignOp* n);
             };
        }
   }

CompassAnalyses::DoNotAssignPointerToFixedAddress::
CheckerOutput::CheckerOutput ( SgNode* node, std::string message )
   : OutputViolationBase(node,::doNotAssignPointerToFixedAddressChecker->checkerName, message)
   {}

CompassAnalyses::DoNotAssignPointerToFixedAddress::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
   }


bool
CompassAnalyses::DoNotAssignPointerToFixedAddress::
is_pointer( SgType* node)
  {
    SgTypedefType* typedef_node = isSgTypedefType( node);

    return node && (
      isSgPointerType( node)
        || (typedef_node && is_pointer( typedef_node->get_base_type()))
    );
  }


//----------------------------------------------------------------------------
// visit
void
CompassAnalyses::DoNotAssignPointerToFixedAddress::Traversal::
visit( SgNode* node)
   { 
     VISIT( isSgInitializedName( node));
     VISIT( isSgAssignOp( node));
   } //End of the visit function.


void
CompassAnalyses::DoNotAssignPointerToFixedAddress::Traversal::
visit( SgInitializedName* node)
   {
      if(
          is_pointer( node->get_type()) 
            && is_illegal_constant( node->get_initializer())
      ) {
        output->addOutput( new CheckerOutput( 
          node, 
          std::string(
            "Pointer " + node->get_name() + " is initialized to a fixed address."
          )
        ));
        return;
      }

      // check for array of pointers
      SgArrayType* arr = isSgArrayType( node->get_type());
      if(
          arr
            && is_pointer( arr->get_base_type())
            && HAS_ILLEGAL_CONSTANT( isSgAggregateInitializer( node->get_initializer()))
      ) {
        output->addOutput( new CheckerOutput( 
          node, 
          std::string(
            "Array of pointers " 
              + node->get_name() 
              + " has an element initialized to a fixed address."
          )
        ));
        return;
      }
   } // end visit


void
CompassAnalyses::DoNotAssignPointerToFixedAddress::Traversal::
visit( SgAssignOp* node)
   {
      if(
          is_pointer( node->get_type()) 
            && is_illegal_constant( node->get_rhs_operand())
      ) {
        output->addOutput( new CheckerOutput( 
          node, 
          std::string(
            "Pointer is assigned to a fixed address."
          )
        ));
      }
   } // end visit

//----------------------------------------------------------------------------
// is_illegal_constant

bool
CompassAnalyses::DoNotAssignPointerToFixedAddress::
is_zero_value( SgValueExp* value_node)
  {
    return (
      IS_ZERO( isSgBoolValExp( value_node))
      || IS_ZERO( isSgCharVal( value_node))
      // Complex
      || IS_ZERO( isSgDoubleVal( value_node))
      // Enum
      || IS_ZERO( isSgFloatVal( value_node))
      || IS_ZERO( isSgIntVal( value_node))
      || IS_ZERO( isSgLongDoubleVal( value_node))
      || IS_ZERO( isSgLongIntVal( value_node))
      || IS_ZERO( isSgLongLongIntVal( value_node))
      || IS_ZERO( isSgShortVal( value_node))
      // String
      || IS_ZERO( isSgUnsignedCharVal( value_node))
      || IS_ZERO( isSgUnsignedIntVal( value_node))
      || IS_ZERO( isSgUnsignedLongLongIntVal( value_node))
      || IS_ZERO( isSgUnsignedLongVal( value_node))
      // UpcMyThread
      // UpcThreads
      || IS_ZERO( isSgWcharVal( value_node))
    );
  }


bool
CompassAnalyses::DoNotAssignPointerToFixedAddress::
is_illegal_constant( SgInitializer* node)
  {
    return IS_ILLEGAL_CONSTANT( isSgAssignInitializer( node));
  }


bool
CompassAnalyses::DoNotAssignPointerToFixedAddress::
is_illegal_constant( SgValueExp* node)
  {
    return !is_zero_value( node);
  }


  
bool
CompassAnalyses::DoNotAssignPointerToFixedAddress::
is_constant( SgExpression* node)
  {
    return isSgValueExp( node)
      || ( isSgCastExp( node) 
              && is_constant( isSgCastExp( node)->get_operand()));
  }

  
bool
CompassAnalyses::DoNotAssignPointerToFixedAddress::
is_illegal_constant( SgBinaryOp* node)
  {
    // This won't catch things like 2 + 0, e.g.
    //    int* bad = ((int*) 2) + 0;
  
    SgExpression* l_op = node->get_lhs_operand();
    SgExpression* r_op = node->get_rhs_operand();

    return is_constant( l_op)
      && is_constant( r_op)
      && (
        is_illegal_constant( l_op)
          || is_illegal_constant( r_op)
      );
  }


bool
CompassAnalyses::DoNotAssignPointerToFixedAddress::
is_illegal_constant( SgExpression* node)
  {
    return (
      IS_ILLEGAL_CONSTANT( isSgValueExp( node))
        || IS_ILLEGAL_CONSTANT( isSgCastExp( node))
        || IS_ILLEGAL_CONSTANT( isSgBinaryOp( node))
    );
  }


bool
CompassAnalyses::DoNotAssignPointerToFixedAddress::
is_illegal_constant( SgCastExp* node)
  {
    return is_illegal_constant( node->get_operand());
  }


bool
CompassAnalyses::DoNotAssignPointerToFixedAddress::
is_illegal_constant( SgAssignInitializer* node)
  {
    return is_illegal_constant( node->get_operand());
  }


bool
CompassAnalyses::DoNotAssignPointerToFixedAddress::
has_illegal_constant( SgAggregateInitializer* node)
  {
    Rose_STL_Container<SgExpression*>::iterator i;
    Rose_STL_Container<SgExpression*> list 
      = node->get_initializers()->get_expressions();

    for( i=list.begin(); i != list.end(); ++i) {
      if( IS_ILLEGAL_CONSTANT( isSgAssignInitializer( *i)))
        return true;
    }

    return false;
  }
//----------------------------------------------------------------------------


// Checker main run function and metadata
static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::DoNotAssignPointerToFixedAddress::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::DoNotAssignPointerToFixedAddress::Traversal(params, output);
}

extern const Compass::Checker* const doNotAssignPointerToFixedAddressChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        "DoNotAssignPointerToFixedAddress",
     // Descriptions should not include the newline character "\n".
        "Warning: Pointer assigned fixed address.",
        "Warning: Pointer assigned fixed address.  See MITRE CWE-587 at http://cwe.mitre.org/data/definitions/587.html.",
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
   
