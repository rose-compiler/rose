/* unparse_type.C
1;95;0c * This C file contains the general function to unparse types as well as
 * functions to unparse every kind of type.
 */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// If this is turned on then we get the message to the
// generted code showing up in the mangled names!
#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_DEBUGGING_FUNCTION_INTERNALS 0
#define OUTPUT_DEBUGGING_UNPARSE_INFO 0

Unparse_Type::Unparse_Type(Unparser* unp)
   : unp(unp)
   {
  // Nothing to do here!
   }

Unparse_Type::~Unparse_Type()
   {
  // Nothing to do here!
   }

void Unparse_Type::curprint (std::string str) {
  unp->u_sage->curprint(str);
}

bool
Unparse_Type::generateElaboratedType(SgDeclarationStatement*, const SgUnparse_Info&)
   {
  // For now we just return true, later we will check the scopeStatement->get_type_elaboration_list();
     bool useElaboratedType = true;

  // unp-> cur << "\n /* In generateElaboratedType = " << (useElaboratedType ? "true" : "false") << " */ \n ";

     return useElaboratedType;
   }

void
Unparse_Type::unparseNullptrType(SgType*, SgUnparse_Info &)
   {
     curprint("std::nullptr_t");
   }


void
Unparse_Type::unparseDeclType(SgType* type, SgUnparse_Info& info)
   {
     SgDeclType* decltype_node = isSgDeclType(type);
     ASSERT_not_null(decltype_node);
     ASSERT_not_null(decltype_node->get_base_expression());

#if 0
     printf ("In Unparse_Type::unparseDeclType(): decltype_node = %p \n",decltype_node);
     printf ("   --- info.isTypeFirstPart()             = %s \n",info.isTypeFirstPart() ? "true" : "false");
     printf ("   --- info.isTypeSecondPart()            = %s \n",info.isTypeSecondPart() ? "true" : "false");
#endif

     if (!info.isTypeSecondPart())
        {
#if 0
          printf ("decltype_node->get_base_expression() = %p = %s \n",decltype_node->get_base_expression(),decltype_node->get_base_expression()->class_name().c_str());
#endif
          SgFunctionParameterRefExp* functionParameterRefExp = isSgFunctionParameterRefExp(decltype_node->get_base_expression());
          if (functionParameterRefExp != NULL)
             {
            // In this case just use the type directly.
               ASSERT_not_null(decltype_node->get_base_type());
#if 0
               printf ("In unparseDeclType(): detected SgFunctionParameterRefExp: using decltype_node->get_base_type() = %p = %s \n",decltype_node->get_base_type(),decltype_node->get_base_type()->class_name().c_str());
#endif
               unparseType(decltype_node->get_base_type(),info);
             }
            else
             {
               curprint("decltype(");
               unp->u_exprStmt->unparseExpression(decltype_node->get_base_expression(),info);
               curprint(") ");
             }
        }
   }


void
Unparse_Type::unparseTypeOfType(SgType* type, SgUnparse_Info& info)
   {
  // DQ (3/28/2015): Adding support for GNU C typeof language extension.

     SgTypeOfType* typeof_node = isSgTypeOfType(type);
     ASSERT_not_null(typeof_node);

#define DEBUG_TYPEOF_TYPE 0

#if DEBUG_TYPEOF_TYPE || 0
     printf ("In unparseTypeOfType(): typeof_node       = %p \n",typeof_node);
     printf ("   --- typeof_node->get_base_expression() = %p \n",typeof_node->get_base_expression());
     if (typeof_node->get_base_expression() != NULL)
        {
          printf ("   --- typeof_node->get_base_expression() = %p = %s \n",typeof_node->get_base_expression(),typeof_node->get_base_expression()->class_name().c_str());
        }
     printf ("   --- typeof_node->get_base_type()            = %p \n",typeof_node->get_base_type());
     if (typeof_node->get_base_type() != NULL)
        {
          printf ("   --- typeof_node->get_base_type() = %p = %s \n",typeof_node->get_base_type(),typeof_node->get_base_type()->class_name().c_str());
        }
     printf ("   --- info.isTypeFirstPart()             = %s \n",info.isTypeFirstPart() ? "true" : "false");
     printf ("   --- info.isTypeSecondPart()            = %s \n",info.isTypeSecondPart() ? "true" : "false");
#endif

  // ASSERT_not_null(typeof_node->get_base_expression());

  // DQ (3/31/2015): I think we can assert this.
  // ROSE_ASSERT (info.isTypeFirstPart() == true  || info.isTypeSecondPart() == true);
     ROSE_ASSERT (info.isTypeFirstPart() == false || info.isTypeSecondPart() == false);


  // DQ (3/31/2015): We can't use the perenthesis in this case (see test2015_49.c).
#if 0
     printf("################ In unparseTypeOfType(): handle special case of SgTypeOfType \n");
#endif

  // DQ (3/31/2015): Note that (info.isTypeFirstPart() == false && info.isTypeSecondPart() == false) is required because
  // we have implemented some special case handling for SgTypeOfType in SgArrayType, SgPointerType, etc.  With this
  // implementation below, we might not need this special case handling.
  // if (info.isTypeFirstPart() == true)
     if (info.isTypeFirstPart() == true || (info.isTypeFirstPart() == false && info.isTypeSecondPart() == false) )
        {
          curprint("__typeof(");
          if (typeof_node->get_base_expression() != NULL)
             {
               unp->u_exprStmt->unparseExpression(typeof_node->get_base_expression(),info);
             }
            else
             {
               SgUnparse_Info ninfo1(info);

               ninfo1.set_SkipClassDefinition();
            // ninfo1.set_SkipClassSpecifier();
               ninfo1.set_SkipEnumDefinition();

               ninfo1.unset_isTypeSecondPart();
               ninfo1.unset_isTypeFirstPart();

// DQ (5/10/2015): Changing this back to calling "unparseType(typeof_node->get_base_type(), ninfo1);" once.
#if 1

// #error "DEAD CODE!"

               unparseType(typeof_node->get_base_type(), ninfo1);
#else
               ninfo1.set_isTypeFirstPart();
#if DEBUG_TYPEOF_TYPE
               printf("In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 1) \n");
               curprint("\n/* In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 1) */ \n");
#endif
               unparseType(typeof_node->get_base_type(), ninfo1);
               ninfo1.set_isTypeSecondPart();
#if DEBUG_TYPEOF_TYPE
               printf("In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 2) \n");
               curprint("\n/* In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 2) */ \n");
#endif
               unparseType(typeof_node->get_base_type(), ninfo1);
#endif
             }
       // curprint("/* end of typeof */ )");
          curprint(") ");
        }

#if 0
  // DQ (3/31/2015): This is an inferior implementation.

     if (info.isTypeFirstPart() == true)
        {
          SgFunctionParameterRefExp* functionParameterRefExp = isSgFunctionParameterRefExp(typeof_node->get_base_expression());
          if (functionParameterRefExp != NULL)
             {
            // DQ (3/31/2015): I am not sure I understand this case well enough and I want to debug it seperately.

            // In this case just use the type directly.
               ASSERT_not_null(typeof_node->get_base_type());
#if 0
               printf ("In unparseTypeOfType(): detected SgFunctionParameterRefExp: using typeof_node->get_base_type() = %p = %s \n",typeof_node->get_base_type(),typeof_node->get_base_type()->class_name().c_str());
#endif

#error "DEAD CODE!"

               printf ("Exiting as a test: debug this case seperately for unparseTypeOfType() \n");
               ROSE_ABORT();

               unparseType(typeof_node->get_base_type(),info);
             }
            else
             {
               curprint("typeof(");

            // unp->u_exprStmt->unparseExpression(typeof_node->get_base_expression(),info);
               if (typeof_node->get_base_expression() != NULL)
                  {
                    unp->u_exprStmt->unparseExpression(typeof_node->get_base_expression(),info);
#error "DEAD CODE!"

                  }
                 else
                  {
                    SgUnparse_Info newinfo(info);
#if 0
                    printf ("typeof_node->get_base_type() = %p = %s \n",typeof_node->get_base_type(),typeof_node->get_base_type()->class_name().c_str());
#endif
                    newinfo.set_SkipClassDefinition();
                 // newinfo.set_SkipClassSpecifier();
                    newinfo.set_SkipEnumDefinition();
#if 0
                 // DQ (3/30/2015): We need to unset these to support when the base type is a SgArrayType.
                 // if (isSgArrayType(typeof_node->get_base_type()) != NULL)
                    if (isSgModifierType(typeof_node->get_base_type()) != NULL)
                       {
                         newinfo.unset_isReferenceToSomething();
                         newinfo.unset_isPointerToSomething();
                       }
#endif
#if 0
                    newinfo.unset_isTypeFirstPart();
                    newinfo.unset_isTypeSecondPart();
#endif

#error "DEAD CODE!"

#if 1
                 // unparseType(typeof_node->get_base_type(),info);
                    unparseType(typeof_node->get_base_type(),newinfo);
#else
                    SgType* type = typeof_node->get_base_type();

#error "DEAD CODE!"

                 // SgUnparse_Info newinfo(info);
                 // newinfo.set_reference_node_for_qualification(operatorExp);
                    if (newinfo.get_reference_node_for_qualification() == NULL)
                       {
                         printf ("Note: In unparseTypeOfType(): newinfo.get_reference_node_for_qualification() == NULL \n");
                       }
                 // ASSERT_not_null(newinfo.get_reference_node_for_qualification());

                    newinfo.set_isTypeFirstPart();
#if 0
                    printf ("In unparseTypeOfType(): isTypeFirstPart: sizeof_op->get_operand_type() = %p = %s \n",type,type->class_name().c_str());
                    curprint ("/* In unparseTypeOfType(): isTypeFirstPart \n */ ");
#endif
                    unp->u_type->unparseType(type, newinfo);

                    newinfo.unset_isTypeFirstPart();
#if 0
                    newinfo.set_isTypeSecondPart();
#if 0
                    printf ("In unparseTypeOfType(): isTypeSecondPart: type = %p = %s \n",type,type->class_name().c_str());
                    curprint ("/* In unparseTypeOfType(): isTypeSecondPart \n */ ");
#endif
                    unp->u_type->unparseType(type, newinfo);
#endif

                    newinfo.unset_isTypeFirstPart();
                    newinfo.unset_isTypeSecondPart();
#endif
                  }

#error "DEAD CODE!"

            // curprint(") ");
               curprint(" /* closing typeof paren: first part */ ) ");
             }
        }
       else
        {
       // DQ (3/31/2015): Implemented this as an else-case of when first-part is false.

       // DQ (3/29/2015): We need to handle the 2nd part of the type when the types are more complex (see case of array type used in test2015_49.c).
          if (info.isTypeSecondPart() == true)
             {
            // curprint(" /* closing typeof paren: second part */ ) ");

               if (typeof_node->get_base_expression() != NULL)
                  {
#if 0
                    printf ("Nothing to do for info.isTypeSecondPart() == true and typeof_node->get_base_expression() != NULL \n");
#endif
                 // unp->u_exprStmt->unparseExpression(typeof_node->get_base_expression(),info);
                  }
                 else
                  {
                    SgUnparse_Info newinfo(info);
#if 0
                 // DQ (3/30/2015): We need to unset these to support when the base type is a SgArrayType.
                 // if (isSgArrayType(typeof_node->get_base_type()) != NULL)
                    if (isSgModifierType(typeof_node->get_base_type()) != NULL)
                       {
                         newinfo.unset_isReferenceToSomething();
                         newinfo.unset_isPointerToSomething();
                       }
#endif
                    newinfo.set_SkipClassDefinition();
                 // newinfo.set_SkipClassSpecifier();
                    newinfo.set_SkipEnumDefinition();

                 // unparseType(typeof_node->get_base_type(),info);
                    unparseType(typeof_node->get_base_type(),newinfo);
                  }

#error "DEAD CODE!"

            // curprint(") ");
            // curprint(" /* closing typeof paren: second part */ ) ");
             }
            else
             {
            // DQ (3/31/2015): Added general case for for when neither first-part nor second-part flags are set.
            // This is done to support more general simplified use with a sing call to the unpars function for
            // cases the type is a SgTypeOfType.

               SgUnparse_Info ninfo1(info);
               ninfo1.unset_isTypeSecondPart();
               ninfo1.unset_isTypeFirstPart();

               ninfo1.set_isTypeFirstPart();
#if 0
               printf("In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 1) \n");
               curprint("\n/* In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 1) */ \n");
#endif
               unparseType(typeof_node, ninfo1);
               ninfo1.set_isTypeSecondPart();
#if 0
               printf("In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 2) \n");
               curprint("\n/* In Unparse_Type::unparseTypeOfType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 2) */ \n");
#endif

#error "DEAD CODE!"

               unparseType(typeof_node, ninfo1);
             }
        }
#endif
   }



#if 0
void
Unparse_Type::unparseQualifiedNameType ( SgType* type, SgUnparse_Info& info )
   {
  // DQ (10/11/2006): Reactivated this design of support for name qualification in ROSE
  // (I hope it works out better this time). This time we handle some internal details
  // differently and attach the qualified name list to the SgUnparse_Info so that the lower
  // level unparse functions can have the control required to generate the names more precisely.

  // DQ (12/21/2005): Added to support name qualification when explicitly stored in the AST
  // (rather than generated).  However, it appears not possible to get the qualified name
  // inbetween the "enum" and the "X" in "enum ::X", so we have to add the support for
  // qualified names more directly to the SgNamedType object to get this level of control
  // in the unparsing.

#if 1
     printf ("Error: This function should not be called, it represents the wrong approach to the design of the IR \n");
     ROSE_ABORT();
#endif

     SgQualifiedNameType* qualifiedNameType = isSgQualifiedNameType(type);
     ASSERT_not_null(qualifiedNameType);

     SgUnparse_Info ninfo(info);

#if 0
     printf ("In unparseQualifiedNameType(): info.isTypeFirstPart()  = %s \n",info.isTypeFirstPart()  ? "true" : "false");
     printf ("In unparseQualifiedNameType(): info.isTypeSecondPart() = %s \n",info.isTypeSecondPart() ? "true" : "false");

     ninfo.unset_isTypeFirstPart();
     ninfo.set_isTypeSecondPart();
     unparseType(qualifiedNameType->get_base_type(),ninfo);

     if (info.isTypeFirstPart() == true)
        {
       // This is copy by value (we might do something better if ROSETTA could generate references).
          unparseQualifiedNameList(qualifiedNameType->get_qualifiedNameList());
          unparseType(qualifiedNameType->get_base_type(),info);
        }
#else
  // DQ (10/10/2006): New support for qualified names for types.
     SgType* internalType = qualifiedNameType->get_base_type();

  // Note that this modifies the list and that SgUnparse_Info& info is passed by reference.  This could be a problem!
     ninfo.set_qualifiedNameList(qualifiedNameType->get_qualifiedNameList());

     ASSERT_not_null(internalType);
  // ASSERT_not_null(ninfo.get_qualifiedNameList());

  // printf ("Processing a SgQualifiedNameType IR node base_type = %p = %s qualified name list size = %ld \n",
  //      type,type->class_name().c_str(),qualifiedNameType->get_qualifiedNameList().size());

  // Call unparseType on the base type with a modified SgUnparse_Info
     unparseType(internalType,ninfo);
#endif
   }
#endif


void Unparse_Type::unparsePointerType(SgType* type, SgUnparse_Info& info)
   {

#define DEBUG_UNPARSE_POINTER_TYPE 0

#if DEBUG_UNPARSE_POINTER_TYPE
     printf("Inside of Unparse_Type::unparsePointerType \n");
     curprint("\n/* Inside of Unparse_Type::unparsePointerType */ \n");
#endif

#if 0
     info.display("Inside of Unparse_Type::unparsePointerType");
#endif

#if DEBUG_UNPARSE_POINTER_TYPE
     printf ("In unparsePointerType(): info.isWithType()       = %s \n",(info.isWithType()       == true) ? "true" : "false");
     printf ("In unparsePointerType(): info.SkipBaseType()     = %s \n",(info.SkipBaseType()     == true) ? "true" : "false");
     printf ("In unparsePointerType(): info.isTypeFirstPart()  = %s \n",(info.isTypeFirstPart()  == true) ? "true" : "false");
     printf ("In unparsePointerType(): info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif

     SgPointerType* pointer_type = isSgPointerType(type);
     ASSERT_not_null(pointer_type);

#if DEBUG_UNPARSE_POINTER_TYPE
     printf ("In unparsePointerType(): isSgReferenceType(pointer_type->get_base_type())      = %s \n",(isSgReferenceType(pointer_type->get_base_type())      != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgPointerType(pointer_type->get_base_type())        = %s \n",(isSgPointerType(pointer_type->get_base_type())        != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgArrayType(pointer_type->get_base_type())          = %s \n",(isSgArrayType(pointer_type->get_base_type())          != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgFunctionType(pointer_type->get_base_type())       = %s \n",(isSgFunctionType(pointer_type->get_base_type())       != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgMemberFunctionType(pointer_type->get_base_type()) = %s \n",(isSgMemberFunctionType(pointer_type->get_base_type()) != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgModifierType(pointer_type->get_base_type())       = %s \n",(isSgModifierType(pointer_type->get_base_type())       != NULL) ? "true" : "false");
     printf ("In unparsePointerType(): isSgTypeOfType(pointer_type->get_base_type())         = %s \n",(isSgTypeOfType(pointer_type->get_base_type())         != NULL) ? "true" : "false");
#endif

  // DQ (3/31/2015): I think this TypeOf GNU extension needs to be supported as a special case.
  // if (isSgTypeOfType(pointer_type->get_base_type()) != NULL && (info.isTypeFirstPart() == true) )
     if (isSgTypeOfType(pointer_type->get_base_type()) != NULL)
        {
#if DEBUG_UNPARSE_POINTER_TYPE
          printf ("############### Warning: unparsePointerType(): pointer_type->get_base_type() is SgTypeOfType \n");
#endif
          if (info.isTypeFirstPart() == true)
             {
               SgUnparse_Info ninfo1(info);
               ninfo1.unset_isTypeSecondPart();
               ninfo1.unset_isTypeFirstPart();

               ninfo1.set_isTypeFirstPart();
#if DEBUG_UNPARSE_POINTER_TYPE
               printf("In Unparse_Type::unparsePointerType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 1) \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 1) */ \n");
#endif
               unparseType(pointer_type->get_base_type(), ninfo1);
               ninfo1.set_isTypeSecondPart();
#if DEBUG_UNPARSE_POINTER_TYPE
               printf("In Unparse_Type::unparsePointerType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 2) \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): TypeOf GNU extension needs to be supported as a special case: (call on base type: part 2) */ \n");
#endif
               unparseType(pointer_type->get_base_type(), ninfo1);

               curprint("*");
#if DEBUG_UNPARSE_POINTER_TYPE
               curprint(" /* unparsePointerType(): typeof: first part */ ");
#endif
             }
            else
             {
#if DEBUG_UNPARSE_POINTER_TYPE
               printf ("TypeofType not output because info.isTypeFirstPart() == false \n");
#endif
            // DQ (4/19/2015): We need to output the typeof operator when it appears as a parameter in function type arguments.
            // See test2015_110.c for an example.
               if (info.isTypeFirstPart() == false && info.isTypeSecondPart() == false)
                  {
#if DEBUG_UNPARSE_POINTER_TYPE
                    printf ("info.isTypeFirstPart() == false && info.isTypeSecondPart() == false (need to output typeof type) \n");
#endif
                    SgUnparse_Info ninfo1(info);
                    ninfo1.unset_isTypeSecondPart();
                    ninfo1.unset_isTypeFirstPart();

                    unparseType(pointer_type->get_base_type(), ninfo1);

                    curprint("*");
#if DEBUG_UNPARSE_POINTER_TYPE
                    curprint(" /* unparsePointerType(): typeof: first and second part false */ ");
#endif
                  }
             }

          return;
        }

  /* special cases: ptr to array, int (*p) [10] */
  /*                ptr to function, int (*p)(int) */
  /*                ptr to ptr to .. int (**p) (int) */

     if (isSgReferenceType(pointer_type->get_base_type()) ||
         isSgPointerType(pointer_type->get_base_type()) ||
         isSgArrayType(pointer_type->get_base_type()) ||
         isSgFunctionType(pointer_type->get_base_type()) ||
         isSgMemberFunctionType(pointer_type->get_base_type()) ||

      // DQ (1/8/2014): debugging test2014_25.c.
      // isSgModifierType(pointer_type->get_base_type()) ||

         false)
        {
#if DEBUG_UNPARSE_POINTER_TYPE
          printf ("In unparsePointerType(): calling info.set_isPointerToSomething() \n");
#endif
          info.set_isPointerToSomething();
        }

  // If not isTypeFirstPart nor isTypeSecondPart this unparse call
  // is not controlled from the statement level but from the type level

#if OUTPUT_DEBUGGING_UNPARSE_INFO
  // curprint ( "\n/* " + info.displayString("unparsePointerType") + " */ \n";
#endif

     if (info.isTypeFirstPart() == true)
        {
#if DEBUG_UNPARSE_POINTER_TYPE
          curprint("\n /* Calling unparseType from unparsePointerType (1st part) */ \n");
#endif
       // DQ (5/3/2013): The base type can not be unparsed if this is part of a list of types in a SgForInitStmt.
       // Here the SkipBaseType() flag is set and it must be respected in the unparsing.
       // unparseType(pointer_type->get_base_type(), info);
          if (info.SkipBaseType() == false)
             {
#if 0
               printf ("In unparsePointerType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparsePointerType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

               unparseType(pointer_type->get_base_type(), info);
             }
#if 0
          curprint("\n /* DONE: Calling unparseType from unparsePointerType (1st part) */ \n");
#endif
       // DQ (9/21/2004): Moved this conditional into this branch (to fix test2004_93.C)
       // DQ (9/21/2004): I think we can assert this, and if so we can simplify the logic below
          ROSE_ASSERT(info.isTypeSecondPart() == false);
       // if ( ( info.isWithType() && info.SkipBaseType() ) || info.isTypeSecondPart() )

#if DEBUG_UNPARSE_POINTER_TYPE
          printf ("info.isWithType()   = %s \n",info.isWithType()   ? "true" : "false");
          printf ("info.SkipBaseType() = %s \n",info.SkipBaseType() ? "true" : "false");
          curprint ( string("\n/* info.isWithType()           = ") + (info.isWithType()   ? "true" : "false") + " */ \n");
          curprint ( string("\n/* info.SkipBaseType()         = ") + (info.SkipBaseType() ? "true" : "false") + " */ \n");
          curprint ( string("\n/* info.isPointerToSomething() = ") + (info.isPointerToSomething() ? "true" : "false") + " */ \n");
#endif

#if 0
       // DQ (9/21/2004): This is currently an error!
          if ( info.isWithType() && info.SkipBaseType() )
             {
            // Do nothing (don't unparse the "*" using the same logic for which we don't unparse the type name)
            // curprint ( "\n/* $$$$$ In unparsePointerType: Do Nothing Case for output of type $$$$$  */ \n";
             }
            else
#error "DEAD CODE!"
             {
            // curprint ( "\n/* $$$$$ In unparsePointerType: Unparse Type Case for output of type $$$$$  */ \n";
            // curprint ( "* /* pointer */ ";
               curprint ( "*");
             }
#else
       // if (info.SkipDefinition() == true)
          curprint("*");
       // curprint(" /* unparsePointerType(): first part */ ");
#endif
        }
       else
        {
          if (info.isTypeSecondPart() == true)
             {
#if DEBUG_UNPARSE_POINTER_TYPE
               printf("In Unparse_Type::unparsePointerType(): unparse 2nd part of type \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): unparse 2nd part of type */ \n");
#endif
               unparseType(pointer_type->get_base_type(), info);

#if DEBUG_UNPARSE_POINTER_TYPE
               printf("DONE: Unparse_Type::unparsePointerType(): unparse 2nd part of type \n");
               curprint("\n/* DONE: Unparse_Type::unparsePointerType(): unparse 2nd part of type */ \n");
#endif
             }
            else
             {
            // DQ (11/27/2004): I think that this is important for unparing functions or function pointers
               SgUnparse_Info ninfo(info);
               ninfo.set_isTypeFirstPart();

#if DEBUG_UNPARSE_POINTER_TYPE
               printf("In Unparse_Type::unparsePointerType(): (call on base type: part 1) \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): (call on base type: part 1) */ \n");
#endif
               unparseType(pointer_type, ninfo);
               ninfo.set_isTypeSecondPart();

#if DEBUG_UNPARSE_POINTER_TYPE
               printf("In Unparse_Type::unparsePointerType(): (call on base type: part 2) \n");
               curprint("\n/* In Unparse_Type::unparsePointerType(): (call on base type: part 2) */ \n");
#endif
               unparseType(pointer_type, ninfo);
             }
        }

#if DEBUG_UNPARSE_POINTER_TYPE
     printf("Leaving of Unparse_Type::unparsePointerType \n");
     curprint("\n /* Leaving of Unparse_Type::unparsePointerType */ \n");
#endif
   }


void Unparse_Type::unparseMemberPointerType(SgType* type, SgUnparse_Info& info)
   {
     SgPointerMemberType* mpointer_type = isSgPointerMemberType(type);
     ASSERT_not_null(mpointer_type);

#define DEBUG_MEMBER_POINTER_TYPE 0
#define CURPRINT_MEMBER_POINTER_TYPE 0

#if DEBUG_MEMBER_POINTER_TYPE || 0
     printf ("In unparseMemberPointerType: mpointer_type = %p \n",mpointer_type);
#endif

#if 0
     printf ("In unparseMemberPointerType: info.inTypedefDecl() = %s \n",info.inTypedefDecl() ? "true" : "false");
     printf ("In unparseMemberPointerType: info.inArgList()     = %s \n",info.inArgList() ? "true" : "false");
#endif

  // plain type :  int (P::*)
  // type with name:  int P::* pmi = &X::a;
  // use: obj.*pmi=7;
     SgType *btype = mpointer_type->get_base_type();
     SgMemberFunctionType* mfnType = nullptr;

#if DEBUG_MEMBER_POINTER_TYPE
     printf ("In unparseMemberPointerType(): btype = %p = %s \n",btype,(btype != NULL) ? btype->class_name().c_str() : "NULL" );
#endif
#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
     curprint("\n/* In unparseMemberPointerType() */ \n");
#endif

  // if ( (mfnType = isSgMemberFunctionType(btype)) != NULL)
     mfnType = isSgMemberFunctionType(btype);
#if 0
     printf ("In unparseMemberPointerType(): mfnType = %p \n",mfnType);
#endif
     if (mfnType != NULL)
        {
       // pointer to member function data
#if DEBUG_MEMBER_POINTER_TYPE
          printf ("In unparseMemberPointerType(): pointer to member function \n");
#endif
#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
          curprint("\n/* In unparseMemberPointerType(): pointer to member function */ \n");
#endif
          if (info.isTypeFirstPart())
             {
#if DEBUG_MEMBER_POINTER_TYPE
               printf ("In unparseMemberPointerType(): pointer to member function:  first part of type \n");
#endif
#if DEBUG_MEMBER_POINTER_TYPE
               printf ("In unparseMemberPointerType(): pointer to member function: unparse return type \n");
#endif
#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
               curprint("\n/* In unparseMemberPointerType(): pointer to member function: first part of type */ \n");
#endif

            // DQ (4/28/2019): Adding name qualification to the base type unparsing for the SgPointerMemberType when it is a member function pointer.
               SgName nameQualifierForBaseType = mpointer_type->get_qualified_name_prefix_for_base_type();

#if DEBUG_MEMBER_POINTER_TYPE || 0
               printf ("In unparseMemberPointerType(): pointer to member data: nameQualifierForBaseType = %s \n",nameQualifierForBaseType.str());
#endif

            // DQ (4/21/2019): Output the associated name qualification.
               curprint(nameQualifierForBaseType);

            // DQ (1/20/2019): Suppress the definition (for enum, function, and class types).
            // unparseType(mfnType->get_return_type(), info); // first part
               SgUnparse_Info ninfo(info);
               ninfo.set_SkipDefinition();

            // DQ (5/19/2019): If there was name qualification, then we didn't need the class specifier (and it would be put in the wrong place anyway).
            // SgUnparse_Info ninfo(info);
            // I don't like that we are checking the name qualificaiton string here.
               if (nameQualifierForBaseType.is_null() == false)
                  {
                 // DQ (5/18/2019): when in the SgAggregateInitializer, don't output the class specifier.
                    ninfo.set_SkipClassSpecifier();
#if 0
                    curprint ( "\n/* set SkipClassSpecifier */ \n");
#endif
                  }

               unparseType(mfnType->get_return_type(), ninfo); // first part

#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
               curprint("\n/* In unparseMemberPointerType(): pointer to member function: DONE unparse return type */ \n");
#endif
#if DEBUG_MEMBER_POINTER_TYPE
               printf ("In unparseMemberPointerType(): pointer to member function: DONE unparse return type \n");
#endif

#if 0
               printf ("Leading paren: info.inTypedefDecl() = %s \n",info.inArgList() ? "true" : "false");
               printf ("Leading paren: info.inArgList() = %s \n",info.inArgList() ? "true" : "false");
#endif
            // DQ (2/3/2019): Suppress parenthesis (see Cxx11_tests/test2019_76.C)
            // Not clear yet where this was required in the first place.
            // DQ (4/27/2019): I think we always need this syntax for pointer to member functions.
               curprint ("(");

#if DEBUG_MEMBER_POINTER_TYPE || 0
               printf ("In unparseMemberPointerType(): pointer to member function: info.get_reference_node_for_qualification() = %p \n",info.get_reference_node_for_qualification());
#endif
            // DQ (4/27/2019): Actually where this is a valid pointer we will have used it to get the fully
            // generated string of the type in unparseType() and we will not have called this function.
            // DQ (4/20/2019): This should be a valid pointer, but we will not be using it.
            // ASSERT_not_null(info.get_reference_node_for_qualification());

            // DQ (4/20/2019): Get the name qualification directly using the SgPointerMemberType.
               SgName nameQualifier = mpointer_type->get_qualified_name_prefix_for_class_of();
               curprint(nameQualifier.str());

#if DEBUG_MEMBER_POINTER_TYPE
               printf ("nameQualifier (from xxx->get_qualified_name_prefix_for_class_of() function) = %s \n",nameQualifier.str());
#endif


#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
               curprint (" /* Calling get_type_name() */ ");
#endif
            // curprint ( "\n/* mpointer_type->get_class_of() = " + mpointer_type->get_class_of()->sage_class_name() + " */ \n";
               curprint ( get_type_name(mpointer_type->get_class_type()) );
#if DEBUG_MEMBER_POINTER_TYPE
               printf ("In unparseMemberPointerType(): pointer to member function: mpointer_type->get_class_type()                = %s \n",mpointer_type->get_class_type()->class_name().c_str());
               printf ("In unparseMemberPointerType(): pointer to member function: get_type_name(mpointer_type->get_class_type()) = %s \n",get_type_name(mpointer_type->get_class_type()).c_str());
#endif
               curprint ( "::*");
             }
            else
             {
               if (info.isTypeSecondPart())
                  {
#if DEBUG_MEMBER_POINTER_TYPE
                    printf ("In unparseMemberPointerType(): pointer to member function data: second part of type \n");
#endif
#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
                    curprint("\n/* In unparseMemberPointerType(): pointer to member function data: second part of type */ \n");
#endif
                    curprint(")");

                 // argument list
                    SgUnparse_Info ninfo(info);
                    ninfo.unset_SkipBaseType();
                    ninfo.unset_isTypeSecondPart();
                    ninfo.unset_isTypeFirstPart();

                 // AAAAAAAAAAAAAAAAA
                 // DQ (4/27/2019): Need to unset the reference node used as a key to lookup the generated string,
                 // else the pointer to member syntax will be output for each function argument type).
                 // ROSE_ASSERT(ninfo.get_reference_node_for_qualification() == NULL);
                    ninfo.set_reference_node_for_qualification(NULL);
                    ROSE_ASSERT(ninfo.get_reference_node_for_qualification() == NULL);
                 // AAAAAAAAAAAAAAAAA

#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
                    curprint("\n/* In unparseMemberPointerType(): start of argument list */ \n");
#endif
                    curprint("(");

                    SgTypePtrList::iterator p = mfnType->get_arguments().begin();
                    while ( p != mfnType->get_arguments().end() )
                       {
#if DEBUG_MEMBER_POINTER_TYPE
                         printf ("In unparseMemberPointerType: output the arguments *p = %p = %s \n",*p,(*p)->class_name().c_str());
#endif
#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
                         curprint("\n/* In unparseMemberPointerType(): output function argument type */ \n");
#endif
                      // DQ (1/20/2019): Supress the definition (for enum, function, and class types.
                      // unparseType(*p, ninfo);
                         SgUnparse_Info ninfo2(ninfo);
                         ninfo2.set_SkipDefinition();

                         unparseType(*p, ninfo2);
#if DEBUG_MEMBER_POINTER_TYPE && CURPRINT_MEMBER_POINTER_TYPE
                         curprint("\n/* In unparseMemberPointerType(): DONE: output function argument type */ \n");
#endif
                         p++;
                         if (p != mfnType->get_arguments().end()) { curprint ( ", "); }
                       }
                    curprint(")");
                 // curprint("\n/* In unparseMemberPointerType(): end of argument list */ \n";

                    unparseType(mfnType->get_return_type(), info); // second part
#if 0
                    printf ("In unparseMemberPointerType(): after unparseType() second part: unparse modifiers \n");
#endif
                 // add member function type qualifiers (&, &&, const, volatile)
                    for (auto qual : memberFunctionQualifiers(mfnType, true /* trailing space after keyword */))
                      curprint(qual);

                  }
                 else
                  {
#if DEBUG_MEMBER_POINTER_TYPE
                    printf ("In unparseMemberPointerType(): pointer to member function data: neither first not second part of type??? \n");
#endif
                    SgUnparse_Info ninfo(info);
                    ninfo.set_isTypeFirstPart();
                    unparseType(mpointer_type, ninfo);
                    ninfo.set_isTypeSecondPart();
                    unparseType(mpointer_type, ninfo);
                  }
             }
        }
       else
        {
       /* pointer to member data */
#if DEBUG_MEMBER_POINTER_TYPE || 0
          printf ("In unparseMemberPointerType(): pointer to member data \n");
#endif
          if (info.isTypeFirstPart())
             {
#if DEBUG_MEMBER_POINTER_TYPE || 0
               printf ("In unparseMemberPointerType(): pointer to member data: first part of type \n");
#endif
            // DQ (9/16/2004): This appears to be an error, btype should not be unparsed here (of maybe btype is not set properly)!
            // printf ("Handling the first part \n");
#if 0
               curprint ( "\n/* start of btype (before name qualification) */ \n");
#endif
#if 0
               curprint ( "\n/* here is where the name qualification goes */ \n");
#endif
#if 0
               if (info.get_reference_node_for_qualification() != NULL)
                  {
                    printf ("info.get_reference_node_for_qualification() = %p = %s \n",info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
                  }
#endif
               SgName nameQualifierForBaseType = mpointer_type->get_qualified_name_prefix_for_base_type();
#if 0
               printf ("In unparseMemberPointerType(): pointer to member data: nameQualifierForBaseType = %s \n",nameQualifierForBaseType.str());
#endif

            // DQ (4/21/2019): Output the associated name qualification.
               curprint(nameQualifierForBaseType);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ABORT();
#endif

#if 0
               curprint ( "\n/* start of btype (after name qualification) */ \n");
#endif

            // DQ (5/19/2019): If there was name qualification, then we didn't need the class specifier (and it would be put in the wrong place anyway).
               SgUnparse_Info ninfo(info);
            // I don't like that we are checking the name qualificaiton string here.
               if (nameQualifierForBaseType.is_null() == false)
                  {
                 // DQ (5/18/2019): when in the SgAggregateInitializer, don't output the class specifier.
                    ninfo.set_SkipClassSpecifier();
#if 0
                    curprint ( "\n/* set SkipClassSpecifier */ \n");
#endif
                  }

            // unparseType(btype, info);
               unparseType(btype, ninfo);
#if 0
               curprint ( "\n/* end of btype */ \n");
#endif
#if 0
               printf ("Leading paren: info.inTypedefDecl() = %s \n",info.inTypedefDecl() ? "true" : "false");
               printf ("Leading paren: info.inArgList()     = %s \n",info.inArgList() ? "true" : "false");
#endif
            // DQ (2/3/2019): Suppress parenthesis (see Cxx11_tests/test2019_76.C)
            // Not clear yet where this was required in the first place.
            // curprint ( "(");
            // if ( info.inTypedefDecl() == true)
               if ( info.inTypedefDecl() == true || info.inArgList() == true)
                  {
#if 0
                    curprint(" /* leading paren */ ");
#endif
                    curprint ("(");
                  }

            // DQ (3/31/2019): Need to unparse the name qualification for the class used in the pointer member type.
#if 0
            // DQ (3/31/2019): I think this is where the added name qualification for the associated class must be output.
               printf ("Additional name qualification for the associated class_type must be output here (inbetween the base type and the class type) \n");
#endif

#define DEBUG_UNPARSE_POINTER_MEMBER_TYPE 0

            // DQ (4/20/2019): Actually where this is a valid pointer we will have used it to get the fully
            // generated string of the type in unparseType() and we will not have called this function.
            // DQ (4/20/2019): This should be a valid pointer, but we will not be using it.
            // ASSERT_not_null(info.get_reference_node_for_qualification());

            // DQ (4/20/2019): Get the name qualification directly using the SgPointerMemberType.
               SgName nameQualifier = mpointer_type->get_qualified_name_prefix_for_class_of();
               curprint(nameQualifier.str());

#if 0
               curprint ( "\n/* calling get_type_name */ \n");
#endif
               curprint (" ");
               curprint ( get_type_name(mpointer_type->get_class_type()) );
               curprint ( "::*");
#if 0
               curprint ( "\n/* DONE: calling get_type_name */ \n");
#endif
             }
            else
             {
               if (info.isTypeSecondPart())
                  {
#if DEBUG_MEMBER_POINTER_TYPE || 0
                //  printf ("In unparseMemberPointerType(): Handling the second part \n");
                    printf ("In unparseMemberPointerType(): pointer to member data: second part of type \n");
#endif
#if 0
                    curprint ( "\n/* start of second type part processing */ \n");
#endif
#if 0
                    printf ("Trailing paren: info.inTypedefDecl() = %s \n",info.inTypedefDecl() ? "true" : "false");
                    printf ("Trailing paren: info.inArgList() = %s \n",info.inArgList() ? "true" : "false");
#endif
                // DQ (2/3/2019): Suppress parenthesis (see Cxx11_tests/test2019_76.C)
                 // curprint(")");
                 // if ( info.inTypedefDecl() == true)
                 // if ( info.inTypedefDecl() == true || info.inArgList() == true)
                    if ( info.inTypedefDecl() == true || info.inArgList() == true)
                       {
#if 0
                         curprint(" /* trailing paren */ ");
#endif
                         curprint(")");
                       }
                      else
                       {
#if 0
                         curprint(" /* skip output of trailing paren */ ");
#endif
                       }

                 // DQ (8/19/2014): Handle array types (see test2014_129.C).
                    SgArrayType* arrayType = isSgArrayType(btype);
                    if (arrayType != NULL)
                       {
#if DEBUG_MEMBER_POINTER_TYPE
                         printf ("In unparseMemberPointerType(): Handling the array type \n");
#endif
                         SgUnparse_Info ninfo(info);
                         curprint("[");
                         unp->u_exprStmt->unparseExpression(arrayType->get_index(),ninfo);
                         curprint("]");
                       }
                  }
                 else
                  {
                 // printf ("What is this 3rd case of neither 1st part nor 2nd part \n");
#if 0
                    printf ("What is this 3rd case! \n");
#endif
#if 0
                    curprint ( "\n/* What is this 3rd case! */ \n");
#endif
                    SgUnparse_Info ninfo(info);
                    ninfo.set_isTypeFirstPart();
                    unparseType(mpointer_type, ninfo);
                    ninfo.set_isTypeSecondPart();
                    unparseType(mpointer_type, ninfo);
                  }
             }
        }

#if DEBUG_MEMBER_POINTER_TYPE || CURPRINT_MEMBER_POINTER_TYPE || 0
     printf ("Leaving unparseMemberPointerType() \n");
     curprint("\n/* Leaving unparseMemberPointerType() */ \n");
#endif
   }


void Unparse_Type::unparseReferenceType(SgType* type, SgUnparse_Info& info)
   {
     SgReferenceType* ref_type = isSgReferenceType(type);
     ASSERT_not_null(ref_type);

  /* special cases: ptr to array, int (*p) [10] */
  /*                ptr to function, int (*p)(int) */
  /*                ptr to ptr to .. int (**p) (int) */
     SgUnparse_Info ninfo(info);

     if (isSgReferenceType(ref_type->get_base_type()) ||
         isSgPointerType(ref_type->get_base_type()) ||
         isSgArrayType(ref_type->get_base_type()) ||
         isSgFunctionType(ref_type->get_base_type()) ||
         isSgMemberFunctionType(ref_type->get_base_type()) ||
         isSgModifierType(ref_type->get_base_type()) )
          ninfo.set_isReferenceToSomething();

     if (ninfo.isTypeFirstPart())
        {
          unparseType(ref_type->get_base_type(), ninfo);
       // curprint ( "& /* reference */ ";
          curprint ( "&");
        }
       else
        {
          if (ninfo.isTypeSecondPart())
             {
               unparseType(ref_type->get_base_type(), ninfo);
             }
            else
             {
               SgUnparse_Info ninfo2(ninfo);
               ninfo2.set_isTypeFirstPart();
               unparseType(ref_type, ninfo2);
               ninfo2.set_isTypeSecondPart();
               unparseType(ref_type, ninfo2);
             }
        }
   }

void Unparse_Type::unparseRvalueReferenceType(SgType* type, SgUnparse_Info& info)
   {
     SgRvalueReferenceType* rvalue_ref_type = isSgRvalueReferenceType(type);
     ASSERT_not_null(rvalue_ref_type);

  /* special cases: ptr to array, int (*p) [10] */
  /*                ptr to function, int (*p)(int) */
  /*                ptr to ptr to .. int (**p) (int) */
     SgUnparse_Info ninfo(info);

     if (isSgReferenceType(rvalue_ref_type->get_base_type()) ||
         isSgPointerType(rvalue_ref_type->get_base_type()) ||
         isSgArrayType(rvalue_ref_type->get_base_type()) ||
         isSgFunctionType(rvalue_ref_type->get_base_type()) ||
         isSgMemberFunctionType(rvalue_ref_type->get_base_type()) ||
         isSgModifierType(rvalue_ref_type->get_base_type()) )
        {
          ninfo.set_isReferenceToSomething();
        }

     if (ninfo.isTypeFirstPart())
        {
          unparseType(rvalue_ref_type->get_base_type(), ninfo);
          curprint ( "&&");
        }
       else
        {
          if (ninfo.isTypeSecondPart())
             {
               unparseType(rvalue_ref_type->get_base_type(), ninfo);
             }
            else
             {
               SgUnparse_Info ninfo2(ninfo);
               ninfo2.set_isTypeFirstPart();
               unparseType(rvalue_ref_type, ninfo2);
               ninfo2.set_isTypeSecondPart();
               unparseType(rvalue_ref_type, ninfo2);
             }
        }
   }

#if 0
void Unparse_Type::unparseNameType(SgType* type, SgUnparse_Info& info)
   {
  // DQ (10/7/2004): This should not exist! anything that is a SgNamedType is handled by the unparse
  // functions for the types that are derived from the SgNamedType (thus this function should not be here)

     printf ("Error: It should be impossible to call this unparseNameType() function (except directly which should not be done!) \n");
     ROSE_ABORT();

     SgNamedType* named_type = isSgNamedType(type);
     ASSERT_not_null(named_type);

#error "DEAD CODE!"

     if((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart())
    /* do nothing */;
       else
        {
       // DQ (10/15/2004): Output the qualified name instead of the name (I think) Opps! this function is never called!
       // curprint ( named_type->get_name().str() + " ";
          printf ("In unparseNameType(): named_type->get_qualified_name() = %s \n",named_type->get_qualified_name().str());
          curprint ( named_type->get_qualified_name().str() + " ");
        }
   }
#endif

void
Unparse_Type::unparseEnumType(SgType* type, SgUnparse_Info& info)
   {
     SgEnumType* enum_type = isSgEnumType(type);
     ROSE_ASSERT(enum_type);

#define DEBUG_ENUM_TYPE 0

#if DEBUG_ENUM_TYPE
     printf ("Inside of unparseEnumType(): info.isTypeFirstPart() = %s info.isTypeSecondPart() = %s \n",(info.isTypeFirstPart() == true) ? "true" : "false",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif
#if DEBUG_ENUM_TYPE
     printf ("Inside of unparseEnumType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("Inside of unparseEnumType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true)  ? "true" : "false");
     printf ("Inside of unparseEnumType(): info.SkipClassSpecifier()  = %s \n",(info.SkipClassSpecifier() == true) ? "true" : "false");
  // printf ("Inside of unparseEnumType(): info.SkipEnumSpecifier()   = %s \n",(info.SkipEnumSpecifier() == true)  ? "true" : "false");
#endif
#if 0
     info.display("Inside of unparseEnumType(): call to info.display()");
#endif

     SgEnumDeclaration *edecl = isSgEnumDeclaration(enum_type->get_declaration());
     ASSERT_not_null(edecl);

#if DEBUG_ENUM_TYPE
     printf ("Inside of unparseEnumType(): edecl = %p = %s \n",edecl,edecl ? edecl->class_name().c_str() : "");
#endif

  // DQ (10/7/2004): We need to output just the name when isTypeFirstPart == false and isTypeSecondPart == false
  // this allows us to handle: "doubleArray* arrayPtr2 = new doubleArray();"
  //                                                         ^^^^^^^^^^^
     if (info.isTypeSecondPart() == false)
        {
          SgClassDefinition *cdefn = NULL;
          SgNamespaceDefinitionStatement* namespaceDefn = NULL;

       // printf ("edecl->isForward()         = %s \n",(edecl->isForward() == true) ? "true" : "false");

       // Build reference to any possible enclosing scope represented by a SgClassDefinition or SgNamespaceDefinition
       // to be used check if name qualification is required.
          unp->u_exprStmt->initializeDeclarationsFromParent ( edecl, cdefn, namespaceDefn );

       // printf ("After initializeDeclarationsFromParent: cdefn = %p namespaceDefn = %p \n",cdefn,namespaceDefn);
       // printf ("In unparseEnumType: cdefn = %p \n",cdefn);

       // GB (09/19/2007): If the enum definition is unparsed, also unparse its
       // attached preprocessing info.
          if (info.isTypeFirstPart() == true && info.SkipEnumDefinition() == false)
             {
               unp->u_exprStmt->unparseAttachedPreprocessingInfo(edecl, info, PreprocessingInfo::before);
             }

       // DQ (4/7/2013): We want to skip the class specified (this include the enum specified also) (see test2013_92.C).
       // DQ (7/24/2011): Restrict where enum is used (to avoid output in template arguments after the name qualification).
       // if ( (info.isTypeFirstPart() == true) )
          if ( (info.isTypeFirstPart() == true) && (info.SkipClassSpecifier() == false) )
             {
            // DQ (5/22/2003) Added output of "enum" string
                curprint ("enum ");
#if DEBUG_ENUM_TYPE
                printf ("Inside of unparseEnumType(): output enum keyword \n");
                curprint ("/* enum from unparseEnumType() */ ");
#endif
            // DQ (2/14/2019): Adding support for C++11 scoped enums (syntax is "enum class ").
               if (edecl->get_isScopedEnum() == true)
                  {
                    curprint ("class ");
                  }
             }
            else
             {
#if DEBUG_ENUM_TYPE
               printf ("Inside of unparseEnumType(): DO NOT output enum keyword \n");
#endif
             }

#if DEBUG_ENUM_TYPE
     printf ("In unparseEnumType: info.inTypedefDecl() = %s \n",info.inTypedefDecl() ? "true" : "false");
     printf ("In unparseEnumType: info.inArgList()     = %s \n",info.inArgList() ? "true" : "false");
#endif

       // DQ (9/14/2013): For C language we need to output the "enum" keyword (see test2013_71.c).
          if ( (info.isTypeFirstPart() == false) && (info.SkipClassSpecifier() == false) && (SageInterface::is_C_language() == true || SageInterface::is_C99_language() == true) )
             {
            // DQ (1/6/2020): When this is used as a argument we only want to unparse the name (e.g. sizeof opterator).
            // Note: we need this for the C language support.
            // if (info.inArgList() == false)
            //    {
               curprint ("enum ");

            // DQ (2/14/2019): Adding support for C++11 scoped enums (syntax is "enum class ").
               if (edecl->get_isScopedEnum() == true)
                  {
                    curprint ("class ");
                  }
            //    }
             }
#if 0
       // DQ (7/30/2014): Commented out to avoid compiler warning about not being used.
       // DQ (10/16/2004): Handle name qualification the same as in the unparseClassType function (we could factor common code later!)
          SgNamedType *ptype = NULL;
          if (cdefn != NULL)
             {
               ptype = isSgNamedType(cdefn->get_declaration()->get_type());
             }
#endif
#if 0
       // DQ (10/14/2004): If we are going to output the definition (below) then we don't need the qualified name!
          bool definitionWillBeOutput = ( (info.isTypeFirstPart() == true) && !info.SkipClassDefinition() );
       // printf ("definitionWillBeOutput = %s \n",(definitionWillBeOutput == true) ? "true" : "false");

       // DQ (10/14/2004): This code takes the namespace into account when a qualified name is required!
          ROSE_ASSERT(namespaceDefn == NULL || namespaceDefn->get_namespaceDeclaration() != NULL);
          bool outputQualifiedName = ((ptype != NULL) && (info.get_current_context() != ptype)) ||
                                     ((namespaceDefn != NULL) && (info.get_current_namespace() != namespaceDefn->get_namespaceDeclaration()));
          outputQualifiedName = outputQualifiedName && (definitionWillBeOutput == false);
       // printf ("outputQualifiedName = %s \n",(outputQualifiedName == true) ? "true" : "false");
#endif

          if (SageInterface::is_C_language() == true || SageInterface::is_C99_language() == true)
             {
            // DQ (10/11/2006): I think that now that we fill in all empty name as a post-processing step, we can assert this now!
#if 0
               printf ("Output the name because this is C language support \n");
#endif
               curprint ( enum_type->get_name().getString() + " ");
             }
            else
             {
            // DQ (6/25/2011): Fixing name qualifiction to work with unparseToString().  In this case we don't
            // have an associated node to reference as a way to lookup the strored name qualification.  In this
            // case we return a fully qualified name.
               if (info.get_reference_node_for_qualification() == NULL)
                  {
                    SgName nameQualifierAndType = enum_type->get_qualified_name();
#if DEBUG_ENUM_TYPE
                    printf ("NOTE: In unparseEnumType(): info.get_reference_node_for_qualification() == NULL (assuming this is for unparseToString() nameQualifierAndType = %s \n",nameQualifierAndType.str());
#endif
                 // DQ (3/29/2019): In reviewing where we are using the get_qualified_name() function, this
                 // might be OK since it is likely only associated with the unparseToString() function.
                    curprint(nameQualifierAndType.str());
                  }
                 else
                  {
#if DEBUG_ENUM_TYPE
                    printf ("In unparseEnumType(): info.get_reference_node_for_qualification() = %p \n",info.get_reference_node_for_qualification());
                    if (info.get_reference_node_for_qualification() != NULL)
                       {
                         printf (" --- info.get_reference_node_for_qualification() = %s \n",info.get_reference_node_for_qualification()->class_name().c_str());
                       }
#endif
                 // DQ (6/2/2011): Newest support for name qualification...
                    SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
#if DEBUG_ENUM_TYPE
                    printf ("In unparseEnumType(): nameQualifier = %s \n",nameQualifier.str());
#endif
                    curprint (nameQualifier.str());

                 // DQ (7/28/2012): Added support for un-named types in typedefs.
                    SgName nm;
                    if (edecl->get_isUnNamed() == false)
                       {
                         nm = edecl->get_name();
                       }
                      else
                       {
                      // Else if this is a declaration in a variable declaration, then we do want to output a generated name.
                      // We could also mark the declaration for the cases where this is required. See test2012_141.C for this case.
                         if (edecl->get_parent() == NULL)
                            {
                              printf ("WARNING: edecl->get_parent() == NULL: edecl = %p \n",edecl);

                           // DQ (4/23/2017): Debugging Microsoft extensions.
                              edecl->get_file_info()->display("WARNING: edecl->get_parent() == NULL: debug");

                              printf ("edecl->get_firstNondefiningDeclaration() = %p \n",edecl->get_firstNondefiningDeclaration());
                              printf ("edecl->get_definingDeclaration() = %p \n",edecl->get_definingDeclaration());
                            }
                         ASSERT_not_null(edecl->get_parent());
                         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(edecl->get_parent());
                         if (typedefDeclaration != NULL)
                            {
                              nm = edecl->get_name();
                            }
                         SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(edecl->get_parent());
                         if (variableDeclaration != NULL)
                            {
                              nm = edecl->get_name();
                            }
                       }

                    if (nm.getString() != "")
                       {
#if DEBUG_ENUM_TYPE
                         printf ("In unparseEnumType(): Output qualifier of current types to the name = %s \n",nm.str());
#endif
                         curprint ( nm.getString() + " ");
                       }
                  }
             }

       // DQ (2/18/2019): Adding support for C++11 base type specification syntax.
       // DQ (1/6/2020): When this is used as a argument we only want to unparse the name (e.g. sizeof opterator).
       // TV: Should it ever be unparsed here? It seems it should only be when unparsing the decl. Added "not SkipEnumDefinition" condition to fix my use case...
          if (!info.inArgList() && !info.SkipEnumDefinition() && edecl->get_field_type() != NULL) {
            curprint(" : ");
            SgUnparse_Info ninfo(info);
            unp->u_type->unparseType(edecl->get_field_type(),ninfo);
          }
        }


#if DEBUG_ENUM_TYPE
     printf ("In unparseEnumType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseEnumType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition()  == true) ? "true" : "false");
#endif

  // DQ (1/7/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

     if (info.isTypeFirstPart() == true)
        {
       // info.display("info before constructing ninfo");
          SgUnparse_Info ninfo(info);

       // don't skip the semicolon in the output of the statement in the class definition
          ninfo.unset_SkipSemiColon();

          ninfo.set_isUnsetAccess();
#if DEBUG_ENUM_TYPE
          printf ("info.SkipEnumDefinition() = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif
          if ( info.SkipEnumDefinition() == false )
             {
               SgUnparse_Info ninfo(info);
               ninfo.set_inEnumDecl();
               SgInitializer *tmp_init = NULL;
               SgName tmp_name;

            // DQ (5/8/2013): Make sure this is a valid pointer.
               if (edecl->get_definingDeclaration() == NULL)
                  {
                    printf ("edecl = %p = %s \n",edecl,edecl->class_name().c_str());
                  }
               ASSERT_not_null(edecl->get_definingDeclaration());

            // DQ (11/21/2021): If the declaration of context is set then use that declaration instead of
            // the one from the shared type, this allows us to support multiple files on the command line.
            // DQ (4/22/2013): We need the defining declaration.
            // edecl = isSgEnumDeclaration(edecl->get_definingDeclaration());
               if (info.get_declaration_of_context() != NULL &&  info.get_declaration_of_context()->variantT() == V_SgEnumDeclaration)
                  {
                    edecl = isSgEnumDeclaration(info.get_declaration_of_context());
                    ROSE_ASSERT(edecl != NULL);
#if 0
                    printf ("In unparseClassType(): Using alternate declaration of context: edecl->get_name() = %s \n",edecl->get_name().str());
                    edecl->get_file_info()->display("In unparseClassType(): debug");
#endif
                  }
                 else
                  {
                 // DQ (4/22/2013): We need the defining declaration.
                    edecl = isSgEnumDeclaration(edecl->get_definingDeclaration());
                  }

            // This fails for test2007_140.C.
               ASSERT_not_null(edecl);

            // DQ (6/26/2005): Output the opend and closing braces even if there are no enumerators!
            // This permits support of the empty enum case! "enum x{};"
               curprint ("{");
#if DEBUG_ENUM_TYPE
               printf ("In unparseEnumType(): Output enumerators from edecl = %p \n",edecl);
               printf ("     --- edecl->get_firstNondefiningDeclaration() = %p \n",edecl->get_firstNondefiningDeclaration());
               printf ("     --- edecl->get_definingDeclaration() = %p \n",edecl->get_definingDeclaration());
#endif
               SgInitializedNamePtrList::iterator p = edecl->get_enumerators().begin();
               if (p != edecl->get_enumerators().end())
                  {
                    while (1)
                       {
                         unp->u_exprStmt->unparseAttachedPreprocessingInfo(*p, info, PreprocessingInfo::before);
                         tmp_name=(*p)->get_name();
                         tmp_init=(*p)->get_initializer();
                         curprint ( tmp_name.str());
                         if (tmp_init)
                            {
                              curprint ( "=");
                              unp->u_exprStmt->unparseExpression(tmp_init, ninfo);
                            }
                         p++;
                         if (p != edecl->get_enumerators().end())
                            {
                              curprint ( ",");
                            }
                           else
                            {
                              break;
                            }
                        }
                  // curprint ( "}";
                   }

            // GB (09/18/2007): If the enum definition is unparsed, also unparse its attached preprocessing info.
            // Putting the "inside" info right here is just a wild guess as to where it might really belong.
               unp->u_exprStmt->unparseAttachedPreprocessingInfo(edecl, info, PreprocessingInfo::inside);

            // DQ (6/26/2005): Support for empty enum case!
               curprint ("}");

            // GB (09/18/2007): If the enum definition is unparsed, also unparse its attached preprocessing info.
               unp->u_exprStmt->unparseAttachedPreprocessingInfo(edecl, info, PreprocessingInfo::after);
             }
        }

#if DEBUG_ENUM_TYPE
     printf ("Leaving unparseEnumType(): edecl = %p \n",edecl);
#endif
   }


void
Unparse_Type::unparseTypedefType(SgType* type, SgUnparse_Info& info)
   {
     SgTypedefType* typedef_type = isSgTypedefType(type);
     ASSERT_not_null(typedef_type);

#define DEBUG_TYPEDEF_TYPE 0

#if DEBUG_TYPEDEF_TYPE
     printf ("Inside of Unparse_Type::unparseTypedefType name = %p = %s \n",typedef_type,typedef_type->get_name().str());
  // curprint ( "\n/* Inside of Unparse_Type::unparseTypedefType */ \n";
#endif
#if DEBUG_TYPEDEF_TYPE
     curprint ( string("\n /* info.isWithType()       = ") + ((info.isWithType()       == true) ? "true" : "false") + " */ \n");
     curprint ( string("\n /* info.SkipBaseType()     = ") + ((info.SkipBaseType()     == true) ? "true" : "false") + " */ \n");
     curprint ( string("\n /* info.isTypeSecondPart() = ") + ((info.isTypeSecondPart() == true) ? "true" : "false") + " */ \n");
#endif
#if DEBUG_TYPEDEF_TYPE
     printf ("In unparseTypedefType(): info.isWithType()       = %s \n",(info.isWithType()       == true) ? "true" : "false");
     printf ("In unparseTypedefType(): info.SkipBaseType()     = %s \n",(info.SkipBaseType()     == true) ? "true" : "false");
     printf ("In unparseTypedefType(): info.isTypeFirstPart()  = %s \n",(info.isTypeFirstPart()  == true) ? "true" : "false");
     printf ("In unparseTypedefType(): info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif

     if ((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart())
        {
       /* do nothing */;
#if DEBUG_TYPEDEF_TYPE
          printf ("Inside of Unparse_Type::unparseTypedefType (do nothing) \n");
#endif
       // curprint ( "\n /* Inside of Unparse_Type::unparseTypedefType (do nothing) */ \n");
        }
       else
        {
       // could be a scoped typedef type
       // check if currrent type's parent type is the same as the context type */
       // SgNamedType *ptype = NULL;
#if DEBUG_TYPEDEF_TYPE
          printf ("Inside of Unparse_Type::unparseTypedefType (normal handling) \n");
#endif
       // curprint ( "\n /* Inside of Unparse_Type::unparseTypedefType (normal handling) */ \n";

          SgTypedefDeclaration *tdecl = isSgTypedefDeclaration(typedef_type->get_declaration());
          ASSERT_not_null(tdecl);

       // DQ (10/16/2004): Keep this error checking for now!
          ASSERT_not_null(typedef_type);
          ASSERT_not_null(typedef_type->get_declaration());
#if 0
          printf ("typedef_type->get_declaration() = %p = %s \n",typedef_type->get_declaration(),typedef_type->get_declaration()->sage_class_name());
#endif
       // DQ (10/17/2004): This assertion forced me to set the parents of typedef in the EDG/Sage connection code
       // since I could not figure out why it was not being set in the post processing which sets parents.
          ASSERT_not_null(typedef_type->get_declaration()->get_parent());

          if (SageInterface::is_C_language() == true || SageInterface::is_C99_language() == true)
             {
            // DQ (10/11/2006): I think that now that we fill in all enmpty name as a post-processing step, we can assert this now!
               ROSE_ASSERT (typedef_type->get_name().getString() != "");
               curprint ( typedef_type->get_name().getString() + " ");
             }
            else
             {

#if 0
            // DQ (4/14/2018): Check if this is associated with the template instantiation.
               SgTemplateInstantiationTypedefDeclaration* templateInstantiationTypedefDeclaration = isSgTemplateInstantiationTypedefDeclaration(tdecl);
               if (templateInstantiationTypedefDeclaration != NULL)
                  {
                    printf ("Part 1: We need to process the template arguments (and name qualification) for this SgTemplateInstantiationTypedefDeclaration \n");
                    curprint ("/* Part 1: We need to process the template arguments to include name qualification */ ");
                  }
               printf ("In unparseTypedefType(): info.get_reference_node_for_qualification() = %p \n",info.get_reference_node_for_qualification());
#endif
            // DQ (6/25/2011): Fixing name qualifiction to work with unparseToString().  In this case we don't
            // have an associated node to reference as a way to lookup the strored name qualification.  In this
            // case we return a fully qualified name.
               if (info.get_reference_node_for_qualification() == NULL)
                  {
                 // printf ("WARNING: In unparseTypedefType(): info.get_reference_node_for_qualification() == NULL (assuming this is for unparseToString() \n");
                    SgName nameQualifierAndType = typedef_type->get_qualified_name();
#if 0
                    printf ("In unparseTypedefType(): Output name nameQualifierAndType = %s \n",nameQualifierAndType.str());
#endif
                 // DQ (3/29/2019): In reviewing where we are using the get_qualified_name() function, this
                 // might be OK since it is likely only associated with the unparseToString() function.
                    curprint(nameQualifierAndType.str());
                  }
                 else
                  {
                    SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
                    curprint(nameQualifier.str());

                 // DQ (4/14/2018): This is not the correct way to handle the output of template instantations since this uses the internal name (with unqualified template arguments).

                 // DQ (4/15/2018): New code (which unparses the template name (without template arguments, and unparsed the template arguments with name qualification).
                 // DQ (4/2/2018): Adding support for alternative and more sophisticated handling of the function name
                 // (e.g. with template arguments correctly qualified, etc.).
                    SgTemplateInstantiationTypedefDeclaration* templateInstantiationTypedefDeclaration = isSgTemplateInstantiationTypedefDeclaration(tdecl);
                    if (templateInstantiationTypedefDeclaration != NULL)
                       {
#if 0
                         printf ("Calling unparseTemplateTypedefName() \n");
                         curprint(" /* In unparseTypedefType(): Calling unparseTemplateTypedefName() */ \n");
#endif
                         unparseTemplateTypedefName(templateInstantiationTypedefDeclaration,info);
#if 0
                         printf ("Done: unparseTemplateTypedefName() \n");
                         curprint(" /* In unparseTypedefType(): Done: unparseTemplateTypedefName() */ \n");
#endif
                       }
                      else
                       {
#if 0
                         printf ("typedef_type->get_name() for non-template instantiation typedef = %s \n",typedef_type->get_name().str());
#endif
                      // curprint ( typedef_type->get_name().str());
                         SgName nm = typedef_type->get_name();
                         if (nm.getString() != "")
                            {
#if 0
                              printf ("In unparseTypedefType(): Output qualifier of current types to the name = %s \n",nm.str());
#endif
                              curprint ( nm.getString() + " ");
                            }
                       }
                  }
             }
        }

#if DEBUG_TYPEDEF_TYPE
     printf ("Leaving Unparse_Type::unparseTypedefType \n");
#endif
#if DEBUG_TYPEDEF_TYPE
     curprint("\n/* Leaving Unparse_Type::unparseTypedefType */ \n");
#endif
   }


void
Unparse_Type::unparseTemplateTypedefName(SgTemplateInstantiationTypedefDeclaration* templateInstantiationTypedefDeclaration, SgUnparse_Info& info)
   {
  // DQ (6/21/2011): Generated this function from refactored call to unparseTemplateArgumentList
     ASSERT_not_null(templateInstantiationTypedefDeclaration);

#if 0
     printf ("In unparseTemplateTypedefName(): templateInstantiationTypedefDeclaration->get_name() = %s \n",templateInstantiationTypedefDeclaration->get_name().str());
     printf ("In unparseTemplateTypedefName(): templateInstantiationTypedefDeclaration->get_templateName() = %s \n",templateInstantiationTypedefDeclaration->get_templateName().str());
#endif

     unp->u_exprStmt->curprint(templateInstantiationTypedefDeclaration->get_templateName().str());

  // DQ (4/15/2018): Not clear if this level of information is available for template teypdef instantiations.
  // bool unparseTemplateArguments = templateInstantiationTypedefDeclaration->get_template_argument_list_is_explicit();
     bool unparseTemplateArguments = true;

#if 0
  // DQ (6/29/2013): This controls if the template arguments for the function should be unparsed.
  // printf ("In unparseTemplateFunctionName(): templateInstantiationFunctionDeclaration->get_template_argument_list_is_explicit() = %s \n",
  //      templateInstantiationFunctionDeclaration->get_template_argument_list_is_explicit() ? "true" : "false");
     printf ("In unparseTemplateTypedefName(): name = %s unparse template arguments = %s \n",
          templateInstantiationTypedefDeclaration->get_templateName().str(),unparseTemplateArguments ? "true" : "false");
#endif

  // DQ (6/29/2013): Use the information recorded in the AST as to if this function has been used with
  // template arguments in the original code.  If so then we always unparse the template arguments, if
  // not then we never unparse the template arguments.  See test2013_242.C for an example of where this
  // is significant in the generated code.  Note that this goes a long way toward making the generated
  // code look more like the original input code (where before we have always unparsed template arguments
  // resulting in some very long function calls in the generated code).  Note that if some template
  // arguments are specified and some are not then control over not unparsing template arguments that
  // where not explicit in the original code will be handled seperately in the near future (in the
  // SgTemplateArgument IR nodes).
  // unparseTemplateArgumentList(templateInstantiationFunctionDeclaration->get_templateArguments(),info);
     if (unparseTemplateArguments == true)
        {
#if 0
          SgTemplateArgumentPtrList & templateArgList = templateInstantiationTypedefDeclaration->get_templateArguments();
          printf ("In unparseTemplateTypedefName(): templateArgList.size() = %zu \n",templateArgList.size());
          for (size_t i = 0; i < templateArgList.size(); i++)
             {
               printf ("--- templateArgList[%zu] = %p \n",i,templateArgList[i]);
             }
#endif
          unp->u_exprStmt->unparseTemplateArgumentList(templateInstantiationTypedefDeclaration->get_templateArguments(),info);
        }
   }












string
Unparse_Type::unparseRestrictKeyword()
   {
  // DQ (12/11/2012): This isolates the logic for the output of the "restrict" keyword for different backend compilers.
     string returnString;

  // DQ (8/29/2005): Added support for classification of back-end compilers (independent of the name invoked to execute them)
     bool usingGcc = false;
     #ifdef USE_CMAKE
        #ifdef CMAKE_COMPILER_IS_GNUCC
           usingGcc = true;
        #endif
     #else
     // DQ (4/16/2016): The clang compiler also uses the GNU form of the restrict keyword.
     // DQ (2/1/2016): Make the behavior of ROSE independent of the exact name of the backend compiler (problem when packages name compilers such as "g++-4.8").
     // string compilerName = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
     // usingGcc = (compilerName == "g++" || compilerName == "gcc" || compilerName == "mpicc" || compilerName == "mpicxx" || compilerName == "mpiicpc");
     // #if BACKEND_CXX_IS_GNU_COMPILER
        #if BACKEND_CXX_IS_GNU_COMPILER || BACKEND_CXX_IS_CLANG_COMPILER
           usingGcc = true;
        #endif
        #if 0
           printf ("Processing restrict keyword: compilerName = %s \n",compilerName.c_str());
        #endif
     #endif

  // Liao 6/11/2008, Preserve the original "restrict" for UPC
  // regardless types of the backend compiler
     if (SageInterface::is_UPC_language() == true )
        {
       // DQ (12/12/2012): We need the white space before and after the keyword (fails for cherokee-util.c).
          returnString = " restrict ";
        }
       else
        {
          if ( usingGcc )
             {
            // GNU uses a string variation on the C99 spelling of the "restrict" keyword
            // DQ (12/12/2012): We need the white space before and after the keyword.
#if 0
               printf ("Using GNU form of restrict keyword! \n");
#endif
               returnString = " __restrict__ ";
             }
            else
             {
            // DQ (12/12/2012): We need the white space before and after the keyword.
#if 0
               printf ("Using non-GNU form of restrict keyword! \n");
#endif
               returnString = " restrict ";
             }
        }

     return returnString;
   }


void Unparse_Type::unparseModifierType(SgType* type, SgUnparse_Info& info)
   {
     SgModifierType* mod_type = isSgModifierType(type);
     ASSERT_not_null(mod_type);

#if 0
     printf ("Top of Unparse_Type::unparseModifierType \n");
  // curprint("\n/* Top of Unparse_Type::unparseModifierType */ \n");
#endif
#if 0
  // mod_type->get_typeModifier().display("called from Unparse_Type::unparseModifierType()");
     printf ("In unparseModifierType(): modifier values (at %p): %s \n",mod_type,mod_type->get_typeModifier().displayString().c_str());
#endif

#if 0
     printf ("In unparseModifierType(): info.isWithType()       = %s \n",(info.isWithType()       == true) ? "true" : "false");
     printf ("In unparseModifierType(): info.SkipBaseType()     = %s \n",(info.SkipBaseType()     == true) ? "true" : "false");
     printf ("In unparseModifierType(): info.isTypeFirstPart()  = %s \n",(info.isTypeFirstPart()  == true) ? "true" : "false");
     printf ("In unparseModifierType(): info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif

  // Determine if we have to print the base type first (before printing the modifier).
  // This is true in case of a pointer (e.g., int * a) or a reference (e.g., int & a)
     bool btype_first = false;
  // if ( isSgReferenceType(mod_type->get_base_type()) || isSgPointerType(mod_type->get_base_type()) )
     if ( isSgReferenceType(mod_type->get_base_type()) || isSgPointerType(mod_type->get_base_type()) )
        {
          btype_first = true;
        }
       else
        {
       // DQ (6/19/2013): Check for case or base_type being a modifier (comes up in complex templae argument handling for template arguments that are unavailable though typedef references.
          SgModifierType* inner_mod_type = isSgModifierType(mod_type->get_base_type());
#if 0
          printf ("inner_mod_type = %p \n",inner_mod_type);
#endif
          if (inner_mod_type != NULL)
             {
               ASSERT_not_null(inner_mod_type->get_base_type());
#if 0
               printf ("In Unparse_Type::unparseModifierType(): inner_mod_type->get_base_type() = %p = %s \n",inner_mod_type->get_base_type(),inner_mod_type->get_base_type()->class_name().c_str());
#endif
#if 0
               if ( isSgReferenceType(inner_mod_type->get_base_type()) || isSgPointerType(inner_mod_type->get_base_type()) )
                  {
#if 0
                    printf ("In Unparse_Type::unparseModifierType(): Detected rare case of modifier-modifier (due to complex template typedef references to unavailable template instantiations). \n");
#endif
                    btype_first = true;
                  }
#else
            // btype_first = true;
            // printf ("In Unparse_Type::unparseModifierType(): Make recursive call to unparseModifierType \n");
            // unparseModifierType(inner_mod_type,info);
#endif
             }
        }


#if 0
     printf ("btype_first            = %s \n",btype_first ? "true" : "false");
     printf ("info.isTypeFirstPart() = %s \n",info.isTypeFirstPart() ? "true" : "false");
#endif

     if (info.isTypeFirstPart())
        {
       // Print the base type if this has to come first
          if (mod_type->get_typeModifier().isOpenclGlobal())
              curprint ( "__global ");
          if (mod_type->get_typeModifier().isOpenclLocal())
              curprint ( "__local ");
          if (mod_type->get_typeModifier().isOpenclConstant())
              curprint ( "__constant ");

          if (btype_first)
               unparseType(mod_type->get_base_type(), info);

          if (mod_type->get_typeModifier().haveAddressSpace()) {
              std::ostringstream outstr;
              outstr << "__attribute__((address_space(" << mod_type->get_typeModifier().get_address_space_value() << ")))";
              curprint ( outstr.str().c_str() );
          }

          if (mod_type->get_typeModifier().get_constVolatileModifier().isConst())
             { curprint ( "const "); }
          if (mod_type->get_typeModifier().get_constVolatileModifier().isVolatile())
             { curprint ( "volatile "); }
#if 0
       // DQ (1/19/2019): Cxx_tests/test2019_04.C demonstrates that "const volatile" is output as "volatile"
          printf ("mod_type->get_typeModifier().get_constVolatileModifier().isConst()    = %s \n",mod_type->get_typeModifier().get_constVolatileModifier().isConst()    ? "true" : "false");
          printf ("mod_type->get_typeModifier().get_constVolatileModifier().isVolatile() = %s \n",mod_type->get_typeModifier().get_constVolatileModifier().isVolatile() ? "true" : "false");
          printf ("mod_type->get_typeModifier().isRestrict()                             = %s \n",mod_type->get_typeModifier().isRestrict() ? "true" : "false");
#endif
          if (mod_type->get_typeModifier().isRestrict())
             {
            // DQ (12/11/2012): Newer version of the code refactored.
               curprint(unparseRestrictKeyword());
             }

       // Microsoft extension
       // xxx_unaligned   // Microsoft __unaligned qualifier

       // Support for near and far pointers (a microsoft extension)
       // xxx_near        // near pointer
       // xxx_far         // far pointer

       // Support for UPC
       // Liao, 6/11/2008. Enable faithful unparsing for .upc files
          if (mod_type->get_typeModifier().get_upcModifier().isUPC_Strict())
             { curprint ("strict "); }
          if (mod_type->get_typeModifier().get_upcModifier().isUPC_Relaxed())
             { curprint ("relaxed "); }

          if (mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
             {
               long block_size = mod_type->get_typeModifier().get_upcModifier().get_layout();

               if (block_size == 0) // block size empty
                  {
                    curprint ("shared[] ") ;
                  }
               else if (block_size == -1) // block size omitted
                  {
                    curprint ("shared ") ;
                  }
               else if (block_size == -2) // block size is *
                  {
                    curprint ("shared[*] ") ;
                  }
               else
                  {
                    ROSE_ASSERT(block_size > 0);

                    stringstream ss;

                    ss << block_size;

                    curprint ("shared[" + ss.str() + "] ");
                  }
             }

       // Print the base type unless it has been printed up front
          if (!btype_first)
             {
#if 0
               printf ("In Unparse_Type::unparseModifierType(): Calling unparseType on mod_type->get_base_type() = %p = %s \n",mod_type->get_base_type(),mod_type->get_base_type()->class_name().c_str());
#endif
               unparseType(mod_type->get_base_type(), info);
             }
        }
       else
        {
#if 0
          printf ("info.isTypeSecondPart() = %s \n",info.isTypeSecondPart() ? "true" : "false");
#endif
          if (info.isTypeSecondPart())
             {
#if 0
               printf ("In Unparse_Type::unparseModifierType(): Calling unparseType on mod_type->get_base_type() = %p = %s \n",mod_type->get_base_type(),mod_type->get_base_type()->class_name().c_str());
#endif
               unparseType(mod_type->get_base_type(), info);
             }
            else
             {
#if 0
               printf ("In Unparse_Type::unparseModifierType(): Calling unparseType TWICE (1st and 2nd parts) on mod_type = %p = %s \n",mod_type,mod_type->class_name().c_str());
#endif
               SgUnparse_Info ninfo(info);
               ninfo.set_isTypeFirstPart();
               unparseType(mod_type, ninfo);
               ninfo.set_isTypeSecondPart();
               unparseType(mod_type, ninfo);
             }
        }

#if 0
     printf ("Leaving Unparse_Type::unparseModifierType \n");
  // curprint("\n/* Leaving Unparse_Type::unparseModifierType */ \n");
#endif
   }


void
Unparse_Type::unparseFunctionType(SgType* type, SgUnparse_Info& info)
   {
     SgFunctionType* func_type = isSgFunctionType(type);
     ASSERT_not_null(func_type);

#define DEBUG_FUNCTION_TYPE 0

  // DQ (1/8/2014): debugging test2014_25.c.
  // info.unset_isPointerToSomething();

     SgUnparse_Info ninfo(info);
     int needParen = 0;
     if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething())
        {
          needParen = 1;
        }

#if 0
     printf ("In unparseFunctionType(): reset needParen = 0 \n");
     curprint("\n/* In unparseFunctionType: set needParen = 0 */ \n");
#endif

  // needParen = 0;

#if DEBUG_FUNCTION_TYPE
     printf ("In unparseFunctionType(): needParen = %d \n",needParen);
     curprint("\n/* In unparseFunctionType: needParen = " + StringUtility::numberToString(needParen) + " */ \n");
#endif

#if DEBUG_FUNCTION_TYPE
     printf ("In unparseFunctionType(): info.isReferenceToSomething() = %s \n",info.isReferenceToSomething() ? "true" : "false");
     printf ("In unparseFunctionType(): info.isPointerToSomething()   = %s \n",info.isPointerToSomething()   ? "true" : "false");
#endif

     ROSE_ASSERT(info.isReferenceToSomething() == ninfo.isReferenceToSomething());
     ROSE_ASSERT(info.isPointerToSomething()   == ninfo.isPointerToSomething());

  // DQ (10/8/2004): Skip output of class definition for return type! C++ standard does not permit
  // a defining declaration within a return type, function parameter, or sizeof expression.
     ninfo.set_SkipClassDefinition();

  // DQ (1/7/2014): We also need to skip the enum definition (see test2014_24.c).
     ninfo.set_SkipEnumDefinition();


     if (ninfo.isTypeFirstPart())
        {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
          curprint ( "\n/* In unparseFunctionType: handling first part */ \n");
          curprint ( "\n/* Skipping the first part of the return type! */ \n");
#endif
          if (needParen)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
            // DQ (9/21/2004): we don't want this for typedefs of function pointers where the function return type is a pointer
               printf ("Skipping the first part of the return type (in needParen == true case)! \n");
               curprint ( "\n/* Skipping the first part of the return type (in needParen == true case)! */ \n");
#endif
#if OUTPUT_DEBUGGING_UNPARSE_INFO || DEBUG_FUNCTION_TYPE
               curprint ( string("\n/* ") + ninfo.displayString("Skipping the first part of the return type (in needParen == true case)") + " */ \n");
#endif
               unparseType(func_type->get_return_type(), ninfo);
               curprint("(");
            // curprint("/* unparseFunctionType */ (");
             }
            else
             {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
            // DQ (9/21/2004): we don't want this for typedefs of function pointers where the function return type is a pointer
               printf ("Skipping the first part of the return type (in needParen == false case)! \n");
               curprint ( "\n/* Skipping the first part of the return type (in needParen == false case)! */ \n");
#endif
               unparseType(func_type->get_return_type(), ninfo);
             }
        }
       else
        {
          if (ninfo.isTypeSecondPart())
             {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
               curprint ( "\n/* In unparseFunctionType: handling second part */ \n");
#endif
               if (needParen)
                  {
#if DEBUG_FUNCTION_TYPE
                    curprint ("/* needParen must be true */ \n ");
#endif
                    curprint(")");
                 // curprint("/* unparseFunctionType */ )");

                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
                  }
            // print the arguments
               SgUnparse_Info ninfo2(info);
               ninfo2.unset_SkipBaseType();
               ninfo2.unset_isTypeSecondPart();
               ninfo2.unset_isTypeFirstPart();

            // DQ (3/15/2005): Don't let typedef declarations (or enum or struct definitions) be
            // unparsed in the function parameter list type output (see test2005_16.C).
               ninfo2.set_SkipDefinition();

            // DQ (4/23/2013): Force the fully qualified name to be used instead (leave reference_node_for_qualification == NULL).
            // See test2013_130.C for an example of where this required.
            // DQ (6/12/2011): Added support for name qualification of types (but this is insufficent when types in teh list must be qualified differently).
            // We can use the func_type as the node to associate name qualifications, but this is also not the correct type of node to us since it should
            // be a node that is unique in the AST.  The type name would have to be saved more directly.  This is likely the support that is ALSO required.
            // ninfo2.set_reference_node_for_qualification(func_type);
               ninfo2.set_reference_node_for_qualification(NULL);
               ROSE_ASSERT(ninfo2.get_reference_node_for_qualification() == NULL);
#if DEBUG_FUNCTION_TYPE
               printf ("Setting reference_node_for_qualification to SgFunctionType, but this is not correct where name qualification is required. \n");
#endif
#if DEBUG_FUNCTION_TYPE
               curprint ("/* Output the type arguments (with parenthesis) */ \n ");
#endif
               curprint("(");
            // curprint("/* unparseFunctionType:parameters */ (");

               SgTypePtrList::iterator p = func_type->get_arguments().begin();
               while(p != func_type->get_arguments().end())
                  {
                 // printf ("Output function argument ... \n");
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
                    curprint ( "\n/* In unparseFunctionType(): Output the function type arguments */ \n");
#endif
#if DEBUG_FUNCTION_TYPE
                    printf ("In unparseFunctionType(): calling unparseType(): output function arguments = %p \n",*p);
                    printf ("   --- *p = %p = %s \n",*p,(*p)->class_name().c_str());
                    printf ("   --- ninfo2.isTypeFirstPart()  = %s \n",ninfo2.isTypeFirstPart() ? "true" : "false");
                    printf ("   --- ninfo2.isTypeSecondPart() = %s \n",ninfo2.isTypeSecondPart() ? "true" : "false");
#endif
                    unparseType(*p, ninfo2);

                    p++;

                    if (p != func_type->get_arguments().end())
                       {
                         curprint(", ");
                       }
                  }

               curprint(")");
            // curprint("/* unparseFunctionType:parameters */ )");

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
               curprint ("\n/* In unparseFunctionType(): AFTER parenthesis are output */ \n");
#endif
               unparseType(func_type->get_return_type(), info); // catch the 2nd part of the rtype

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
               curprint ("\n/* Done: In unparseFunctionType(): handling second part */ \n");
#endif
             }
            else
             {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
               curprint ("\n/* In unparseFunctionType: recursive call with isTypeFirstPart == true */ \n");
#endif
#if 0
            // DQ (8/19/2012): Temp code while debugging (test2012_192.C).
               printf ("Error: I think this should not be done \n");
               ROSE_ABORT();
#endif
               ninfo.set_isTypeFirstPart();
               curprint (" ");
               unparseType(func_type, ninfo);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
               curprint ( "\n/* In unparseFunctionType: recursive call with isTypeSecondPart == true */ \n");
#endif
               ninfo.set_isTypeSecondPart();
               unparseType(func_type, ninfo);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
               curprint ( "\n/* In unparseFunctionType: end of recursive call */ \n");
#endif
             }
        }

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || DEBUG_FUNCTION_TYPE
     printf("Leaving unparseFunctionType() \n");
     curprint("\n/* Leaving unparseFunctionType() */ \n");
#endif
   }


void
Unparse_Type::unparseMemberFunctionType(SgType* type, SgUnparse_Info& info)
   {
#if 0
     printf ("In unparseMemberFunctionType(type = %p (%s))\n", type, type ? type->class_name().c_str() : "");
#endif

     SgMemberFunctionType* mfunc_type = isSgMemberFunctionType(type);
     ASSERT_not_null(mfunc_type);

     SgUnparse_Info ninfo(info);
     int needParen=0;
     if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething())
        {
          needParen=1;
        }

  // DQ (10/7/2004): Skip output of class definition for return type! C++ standard does not permit
  // a defining declaration within a return type, function parameter, or sizeof expression.
     ninfo.set_SkipClassDefinition();

  // DQ (1/13/2014): Set the output of the enum defintion to match that of the class definition (consistancy is now inforced).
     ninfo.set_SkipEnumDefinition();

     if (ninfo.isTypeFirstPart())
        {
          if (needParen)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();
#if 0
               printf ("In unparseMemberFunctionType(): ninfo.SkipClassDefinition() = %s \n",(ninfo.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseMemberFunctionType(): ninfo.SkipEnumDefinition()  = %s \n",(ninfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(ninfo.SkipClassDefinition() == ninfo.SkipEnumDefinition());

               unparseType(mfunc_type->get_return_type(), ninfo);
               curprint ( "(");
             }
            else
             {
#if 0
               printf ("In unparseMemberFunctionType(): ninfo.SkipClassDefinition() = %s \n",(ninfo.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseMemberFunctionType(): ninfo.SkipEnumDefinition()  = %s \n",(ninfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(ninfo.SkipClassDefinition() == ninfo.SkipEnumDefinition());

               unparseType(mfunc_type->get_return_type(), ninfo);
             }
        }
       else
        {
          if (ninfo.isTypeSecondPart())
             {
               if (needParen)
                  {
                    curprint ( ")");
                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
                  }
            // print the arguments
               SgUnparse_Info ninfo2(info);
               ninfo2.unset_SkipBaseType();
               ninfo2.unset_isTypeFirstPart();
               ninfo2.unset_isTypeSecondPart();

               curprint ( "(");
               SgTypePtrList::iterator p = mfunc_type->get_arguments().begin();
               while (p != mfunc_type->get_arguments().end())
                  {
                 // printf ("In unparseMemberFunctionType: output the arguments \n");
                    unparseType(*p, ninfo2);
                    p++;
                    if (p != mfunc_type->get_arguments().end())
                       {
                         curprint ( ", ");
                       }
                  }
               curprint ( ")");
#if 0
               printf ("In unparseMemberFunctionType(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseMemberFunctionType(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

               unparseType(mfunc_type->get_return_type(), info); // catch the 2nd part of the rtype
#if 0
               printf ("In unparseMemberFunctionType(): after unparseType() second part: unparse modifiers \n");
#endif
               // add member function type qualifiers (&, &&, const, volatile)
               for (auto qual : memberFunctionQualifiers(mfunc_type))
                 curprint(qual);
             }
            else
             {
               ninfo.set_isTypeFirstPart();
#if 0
               printf ("In unparseMemberFunctionType(): ninfo.SkipClassDefinition() = %s \n",(ninfo.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseMemberFunctionType(): ninfo.SkipEnumDefinition()  = %s \n",(ninfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(ninfo.SkipClassDefinition() == ninfo.SkipEnumDefinition());

               unparseType(mfunc_type, ninfo);

               ninfo.unset_isTypeFirstPart();
               curprint(" ");
               ninfo.set_isTypeSecondPart();
#if 0
               printf ("In unparseMemberFunctionType(): ninfo.SkipClassDefinition() = %s \n",(ninfo.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseMemberFunctionType(): ninfo.SkipEnumDefinition()  = %s \n",(ninfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(ninfo.SkipClassDefinition() == ninfo.SkipEnumDefinition());

               unparseType(mfunc_type, ninfo);
             }
        }
   }

void
Unparse_Type::unparseArrayType(SgType* type, SgUnparse_Info& info)
   {
     SgArrayType* array_type = isSgArrayType(type);
     ASSERT_not_null(array_type);

#define DEBUG_ARRAY_TYPE 0

  // different cases to think about
  //    int (*) [10],  int (*var) [20]
  //    int *[10],  int *var[10]
  //    int [10][20], int var[10][20]
  // multidimensional,
  //    int [2][10] is built up as
  //      ArrayType(base_type, 2)
  //        ArrayType(int, 10), because of the front-end

#if 0
     info.display("In unparseArrayType()");
#endif

     bool is_variable_length_array = array_type->get_is_variable_length_array();
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(info.get_decl_stmt());
     bool isFunctionPrototype = ( (functionDeclaration != NULL) && (functionDeclaration == functionDeclaration->get_firstNondefiningDeclaration()) );

#if 0
     printf ("In unparseArrayType(): is_variable_length_array = %s \n",is_variable_length_array ? "true" : "false");
     printf ("In unparseArrayType(): isFunctionPrototype      = %s \n",isFunctionPrototype ? "true" : "false");
#endif

#if DEBUG_ARRAY_TYPE
     string firstPartString  = (info.isTypeFirstPart()  == true) ? "true" : "false";
     string secondPartString = (info.isTypeSecondPart() == true) ? "true" : "false";
     printf ("\nIn Unparse_Type::unparseArrayType(): type = %p type->class_name() = %s firstPart = %s secondPart = %s \n",type,type->class_name().c_str(),firstPartString.c_str(),secondPartString.c_str());
     printf ("In Unparse_Type::unparseArrayType(): array_type->get_base_type() = %p = %s \n",array_type->get_base_type(),array_type->get_base_type()->class_name().c_str());
#endif

#if 0
  // DQ (5/8/2013): Note that this will make the type name very long and can cause problems with nexted type generating nested comments.
     curprint("/* In unparseArrayType() */ \n ");
#endif
#if DEBUG_ARRAY_TYPE
  // DQ (5/8/2013): Note that this will make the type name very long and can cause problems with nexted type generating nested comments.
     curprint(string("\n/* Top of unparseArrayType() using generated type name string: ") + type->class_name() + " firstPart " + firstPartString + " secondPart " + secondPartString + " */ \n");
#endif

#if DEBUG_ARRAY_TYPE
     printf ("In Unparse_Type::unparseArrayType(): info.isReferenceToSomething() = %s \n",info.isReferenceToSomething() ? "true" : "false");
     printf ("In Unparse_Type::unparseArrayType(): info.isPointerToSomething()   = %s \n",info.isPointerToSomething()   ? "true" : "false");
#endif

#if 0
  // DQ (5/10/2015): Removing this special case now that other parts of the unparsing of
  // typeof operators are more refined.  This is done as part of debugging: test2015_115.c.

  // DQ (4/11/2015): Need to handle "[n]" part of array type (see test2015_83.c).
  // DQ (3/31/2015): We can't use the perenthesis in this case (see test2015_49.c).
  // if (isSgTypeOfType(array_type->get_base_type()) != NULL)
     if (isSgTypeOfType(array_type->get_base_type()) != NULL && info.isTypeFirstPart() == true)
        {
       // This is a special case (similar to that for SgPointerType, but not the same).
#if DEBUG_ARRAY_TYPE
          printf("################ In unparseArrayType(): handle special case of array_type->get_base_type() is SgTypeOfType \n");
#endif
          if (info.isTypeFirstPart() == true)
             {
#if DEBUG_ARRAY_TYPE
               printf ("In Unparse_Type::unparseArrayType(): base_type is SgTypeOfType: info.isTypeFirstPart() == true (unset first and second parts and call unparse on SgTypeOfType base type) \n");
#endif
               SgUnparse_Info ninfo1(info);
               ninfo1.unset_isTypeSecondPart();
               ninfo1.unset_isTypeFirstPart();

               unparseType(array_type->get_base_type(), ninfo1);
             }

#if 0
       // DQ (4/11/2015): This code cannot be executed since we modified the predicate to add "&& info.isTypeFirstPart() == true"
          if (info.isTypeSecondPart() == true)
             {
#if DEBUG_ARRAY_TYPE
               printf ("In Unparse_Type::unparseArrayType(): base_type is SgTypeOfType: info.isTypeSecondPart() == true (nothing implemented) \n");
#endif
             }
#endif
          return;
        }
#else
//   printf ("In Unparse_Type::unparseArrayType(): Commented out special case handling for SgTypeOf base type \n");
#endif

     SgUnparse_Info ninfo(info);
     bool needParen = false;
     if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething())
        {
          needParen = true;

#if 0
       // DQ (3/30/2015): We can't use the perenthesis in this case (see test2015_49.c).
          if (isSgTypeOfType(array_type->get_base_type()) != NULL)
             {
#if 1
               printf("In unparseArrayType(): if isSgTypeOfType(array_type->get_base_type()) != NULL, then set needParen = false \n");
#endif
               needParen = false;
             }
#endif
        }

#if DEBUG_ARRAY_TYPE || 0
     printf ("In unparseArrayType(): needParen = %s \n",(needParen == true) ? "true" : "false");
     curprint(string("/* In  unparseArrayType() needParen = ") + string((needParen == true) ? "true" : "false") + string(" */ \n "));
#endif

     if (ninfo.isTypeFirstPart() == true)
        {
          if(needParen == true)
             {
               ninfo.unset_isReferenceToSomething();
               ninfo.unset_isPointerToSomething();
#if DEBUG_ARRAY_TYPE
               printf ("ninfo.isTypeFirstPart() == true: needParen == true: Calling unparseType(array_type->get_base_type(), ninfo); \n");
#endif
               unparseType(array_type->get_base_type(), ninfo);
#if DEBUG_ARRAY_TYPE
               printf ("DONE: ninfo.isTypeFirstPart() == true: needParen == true: Calling unparseType(array_type->get_base_type(), ninfo); \n");
#endif
               curprint("(");
            // curprint(" /* unparseArrayType */ (");
             }
            else
             {
#if DEBUG_ARRAY_TYPE
               printf ("ninfo.isTypeFirstPart() == true: needParen == false: Calling unparseType(array_type->get_base_type(), ninfo); \n");
#endif
               unparseType(array_type->get_base_type(), ninfo);
#if DEBUG_ARRAY_TYPE
               printf ("DONE: ninfo.isTypeFirstPart() == true: needParen == false: Calling unparseType(array_type->get_base_type(), ninfo); \n");
#endif
             }
        }
       else
        {
          if (ninfo.isTypeSecondPart() == true)
             {
               if (needParen == true)
                  {
#if DEBUG_ARRAY_TYPE
                    printf ("ninfo.isTypeSecondPart() == true: needParen == true: output parenthisis and unparse the array index \n");
#endif
                    curprint(")");
                 // curprint(" /* unparseArrayType */ )");
#if 0
#error "DEAD CODE!"
                 // DQ (3/24/2015): Original code.
                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
#else
#if 1
                 // DQ (3/24/2015): Original code (also required to fix test2015_21.C).
                    info.unset_isReferenceToSomething();
                    info.unset_isPointerToSomething();
#endif
                 // DQ (3/24/2015): I think we want to unset ninfo (see test2015_30.c).
                    ninfo.unset_isReferenceToSomething();
                    ninfo.unset_isPointerToSomething();
#endif
                  }
                 else
                  {
#if DEBUG_ARRAY_TYPE
                    printf ("ninfo.isTypeSecondPart() == true: needParen == false: unparse the array index \n");
#endif
                  }

               curprint("[");

#if DEBUG_ARRAY_TYPE
            // DQ (6/3/2017): Added more debugging info.
               printf ("##### array_type = %p array_type->get_index() = %p = %s \n",array_type,array_type->get_index(),array_type->get_index() != NULL ? array_type->get_index()->class_name().c_str() : "null");
#endif
               if (array_type->get_index())
                  {
                 // JJW (12/14/2008): There may be types inside the size of an array, and they are not the second part of the type
                    SgUnparse_Info ninfo2(ninfo);
                    ninfo2.unset_isTypeSecondPart();
#if 0
                 // DQ (6/3/2017): I think a fix for multi-dimentional array types may be to disable this feature,
                 // sense we already build the array type with a SgNullExpression for the index.
                    printf ("Test disabling supressArrayBound in array type \n");
                    ninfo2.unset_supressArrayBound();
#endif
#if 0
                    printf ("In Unparse_Type::unparseArrayType(): ninfo2.isReferenceToSomething() = %s \n",ninfo2.isReferenceToSomething() ? "true" : "false");
                    printf ("In Unparse_Type::unparseArrayType(): ninfo2.isPointerToSomething()   = %s \n",ninfo2.isPointerToSomething()   ? "true" : "false");
#endif
#if 0
                 // DQ (3/24/2015): I think that the index needs to have these be unset!
                    ROSE_ASSERT(ninfo2.isReferenceToSomething() == false);
                    ROSE_ASSERT(ninfo2.isPointerToSomething() == false);
#endif
#if 0
                    printf ("In unparseArrayType(): ninfo2.SkipClassDefinition() = %s \n",(ninfo2.SkipClassDefinition() == true) ? "true" : "false");
                    printf ("In unparseArrayType(): ninfo2.SkipEnumDefinition()  = %s \n",(ninfo2.SkipEnumDefinition() == true) ? "true" : "false");
#endif
                 // DQ (1/9/2014): These should have been setup to be the same.
                    ROSE_ASSERT(ninfo2.SkipClassDefinition() == ninfo2.SkipEnumDefinition());
#if DEBUG_ARRAY_TYPE
                    printf ("In unparseArrayType(): ninfo2.supressArrayBound()  = %s \n",(ninfo2.supressArrayBound() == true) ? "true" : "false");
#endif
                 // DQ (2/2/2014): Allow the array bound to be subressed (e.g. in secondary declarations of array variable using "[]" syntax.
                 // unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                    if (ninfo2.supressArrayBound() == false)
                       {
                      // Unparse the array bound.

                      // DQ (3/28/2017): Eliminate warning about unused variable from Clang.
                      // DQ (2/12/2016): Adding support for variable length arrays.
                      // unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                      // SgExpression* indexExpression = array_type->get_index();

                      // DQ (3/28/2017): Eliminate warning about unused variable from Clang.
                      // SgNullExpression* nullExpression = isSgNullExpression(indexExpression);

                      // DQ (2/14/2016): Since the array type's index is updated after seeing the function definition, we need to always use the VLA syntax for reliabily.
                      // if (nullExpression != NULL && array_type->get_is_variable_length_array() == true)
                         if (is_variable_length_array == true && isFunctionPrototype == true)
                            {
                           // The is the canonical normalized form for a type specified in a function parameter list of a prototype function.
                              curprint("*");
                            }
                           else
                            {
                           // unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                              if (is_variable_length_array == true && isFunctionPrototype == false)
                                 {
#if 0
                                   printf ("We need to output the expression used in the defining declaration's array type \n");
                                   curprint(" /* We need to output the expression used in the defining declaration's array type */ ");
#endif
#if 0
                                   printf ("In unparseArrayType(): array_type->get_index() = %p \n",array_type->get_index());
                                   printf ("In unparseArrayType(): array_type->get_index() = %p = %s \n",array_type->get_index(),array_type->get_index()->class_name().c_str());
#endif
                                   unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                                 }
                             // TV (09/21/2018): ROSE-1391: TODO: That should only happens when array_type->get_index() refers to a symbol that is not accessible
                                else if (array_type->get_number_of_elements() > 0)
                                 {
                                     std::ostringstream oss;
                                     oss << array_type->get_number_of_elements();
                                     curprint(oss.str());
                                 }
                                else
                                 {
                                   unp->u_exprStmt->unparseExpression(array_type->get_index(), ninfo2); // get_index() returns an expr
                                 }
                            }
                       }
                      else
                       {
#if DEBUG_ARRAY_TYPE
                         printf ("In unparseArrayType(): Detected info_for_type.supressArrayBound() == true \n");
#endif
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ABORT();
#endif
                       }
                  }

               curprint("]");

#if DEBUG_ARRAY_TYPE
               printf ("ninfo.isTypeSecondPart() == true: needParen = %s Calling unparseType(array_type->get_base_type(), ninfo); \n",needParen ? "true" : "false");
#endif
               unparseType(array_type->get_base_type(), info); // second part
#if DEBUG_ARRAY_TYPE
               printf ("DONE: ninfo.isTypeSecondPart() == true: needParen = %s Calling unparseType(array_type->get_base_type(), ninfo); \n",needParen ? "true" : "false");
#endif
             }
            else
             {
#if DEBUG_ARRAY_TYPE
               printf ("Calling unparseType(array_type, ninfo); with ninfo.set_isTypeFirstPart(); \n");
#endif
               ninfo.set_isTypeFirstPart();
               unparseType(array_type, ninfo);

#if DEBUG_ARRAY_TYPE
               printf ("Calling unparseType(array_type, ninfo); with ninfo.set_isTypeSecondPart(); \n");
#endif
               ninfo.set_isTypeSecondPart();
               unparseType(array_type, ninfo);
             }
        }

#if DEBUG_ARRAY_TYPE || 0
  // DQ (5/8/2013): Note that this will make the type name very long and can cause problems with nexted type generating nested comments.
     printf ("Leaving unparseArrayType(): type = %p \n",type);
     curprint("/* Leaving unparseArrayType() */ \n ");
#endif
   }



void
Unparse_Type::unparseTemplateType(SgType* type, SgUnparse_Info &)
   {
     SgTemplateType* template_type = isSgTemplateType(type);
     ASSERT_not_null(template_type);
     ROSE_ABORT();
   }

void
Unparse_Type::unparseAutoType(SgType* type, SgUnparse_Info& info)
   {
     SgAutoType* auto_type = isSgAutoType(type);
     ASSERT_not_null(auto_type);
     bool unparse_type = info.isTypeFirstPart() || ( !info.isTypeFirstPart() && !info.isTypeSecondPart() );
     if (unparse_type) {
       curprint("auto ");
     }
   }

#define DEBUG_UNPARSE_NONREAL_TYPE 0

void
Unparse_Type::unparseNonrealType(SgType* type, SgUnparse_Info& info, bool is_first_in_nonreal_chain)
   {

  // TV (03/29/2018): either first part is requested, or neither if called from unparseToString.
     bool unparse_type = info.isTypeFirstPart() || ( !info.isTypeFirstPart() && !info.isTypeSecondPart() );
     if (!unparse_type) return;

     SgNonrealType * nrtype = isSgNonrealType(type);
     ASSERT_not_null(nrtype);

#if DEBUG_UNPARSE_NONREAL_TYPE
     printf("In unparseNonrealType(type = %p): name = %s\n", type, nrtype->get_name().str());
#endif

     SgNonrealDecl * nrdecl = isSgNonrealDecl(nrtype->get_declaration());
     ASSERT_not_null(nrdecl);

     if (nrdecl->get_templateDeclaration() == NULL) {
       SgNode * parent = nrdecl->get_parent();
       ASSERT_not_null(parent);
       SgDeclarationScope * nrscope = isSgDeclarationScope(parent);
       if (nrscope == NULL) {
         printf("FATAL: Found a SgNonrealDecl (%p) whose parent is a %s (%p)\n", nrdecl, parent->class_name().c_str(), parent);
       }
       ASSERT_not_null(nrscope);

       parent = nrscope->get_parent();
       SgNonrealDecl * nrparent_nrscope = isSgNonrealDecl(parent);
#if DEBUG_UNPARSE_NONREAL_TYPE
       printf(" --- nrparent_nrscope = %p (%s)\n", nrparent_nrscope, nrparent_nrscope != NULL ? nrparent_nrscope->class_name().c_str() : NULL);
#endif
       if (nrparent_nrscope != NULL) {
         if (is_first_in_nonreal_chain) curprint("typename ");
         unparseNonrealType(nrparent_nrscope->get_type(), info, false);
         curprint("::");
       }

     } else if (info.get_reference_node_for_qualification()) {
       SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
#if DEBUG_UNPARSE_NONREAL_TYPE
       printf(" --- nameQualifier = %s\n", nameQualifier.str());
#endif
       curprint(nameQualifier.str());
     }

     SgTemplateArgumentPtrList & tpl_args = nrdecl->get_tpl_args();

     // if template argument are provided then the "template" keyword has to be added
  // if (tpl_args.size() > 0) curprint("template ");

     // output the name of the non-real type
     curprint(nrtype->get_name());

     // unparse template argument list
     if (tpl_args.size() > 0) {
#if DEBUG_UNPARSE_NONREAL_TYPE
       printf("  tpl_args.size() = %d\n", tpl_args.size());
#endif
       SgUnparse_Info ninfo(info);
       ninfo.set_SkipClassDefinition();
       ninfo.set_SkipEnumDefinition();
       ninfo.set_SkipClassSpecifier();
       unp->u_exprStmt->unparseTemplateArgumentList(tpl_args, ninfo);
     }

     curprint(" ");
   }

// explicit instantiation of Unparse_Type::outputType
template void Unparse_Type::outputType(SgInitializedName*, SgType* , SgUnparse_Info &);
template void Unparse_Type::outputType(SgTemplateArgument*, SgType*, SgUnparse_Info &);
template void Unparse_Type::outputType(SgAggregateInitializer*, SgType*, SgUnparse_Info &);

template <typename T>
void
Unparse_Type::outputType( T* referenceNode, SgType* referenceNodeType, SgUnparse_Info & info)
   {
     SgUnparse_Info newInfo(info);
     newInfo.set_isTypeFirstPart();
     SgUnparse_Info ninfo_for_type(newInfo);

     if (referenceNode->get_requiresGlobalNameQualificationOnType() == true)
        {
          ninfo_for_type.set_requiresGlobalNameQualification();
        }

     SgTemplateArgument* templateArgument = isSgTemplateArgument(referenceNode);
     if (templateArgument != NULL)
        {
          templateArgument->set_name_qualification_length_for_type    (templateArgument->get_name_qualification_length());
          templateArgument->set_global_qualification_required_for_type(templateArgument->get_global_qualification_required());
          templateArgument->set_type_elaboration_required_for_type    (templateArgument->get_type_elaboration_required());
        }

     ninfo_for_type.set_name_qualification_length    (referenceNode->get_name_qualification_length_for_type());
     ninfo_for_type.set_global_qualification_required(referenceNode->get_global_qualification_required_for_type());
     ninfo_for_type.set_type_elaboration_required    (referenceNode->get_type_elaboration_required_for_type());

     ninfo_for_type.set_reference_node_for_qualification(referenceNode);

     if (ninfo_for_type.requiresGlobalNameQualification()) {
       ninfo_for_type.set_global_qualification_required(true);
       ninfo_for_type.set_reference_node_for_qualification(NULL);
     }
     ROSE_ASSERT(ninfo_for_type.SkipClassDefinition() == ninfo_for_type.SkipEnumDefinition());

     unp->u_type->unparseType(referenceNodeType, ninfo_for_type);

     SgInitializedName* initializedName = isSgInitializedName(referenceNode);
     if (initializedName != NULL)
        {
          SgName tmp_name  = initializedName->get_name();
          curprint(tmp_name.str());
        }

     newInfo.set_isTypeSecondPart();

     unp->u_type->unparseType(referenceNodeType, newInfo);
   }

template <>
void
Unparse_Type::outputType<SgConstructorInitializer>( SgConstructorInitializer* referenceNode, SgType* referenceNodeType, SgUnparse_Info & info)
   {
     SgUnparse_Info newInfo(info);
     newInfo.set_isTypeFirstPart();
     SgUnparse_Info ninfo_for_type(newInfo);

     ninfo_for_type.set_reference_node_for_qualification(referenceNode);

     if (ninfo_for_type.requiresGlobalNameQualification()) {
       ninfo_for_type.set_global_qualification_required(true);
       ninfo_for_type.set_reference_node_for_qualification(NULL);
     }
     ROSE_ASSERT(ninfo_for_type.SkipClassDefinition() == ninfo_for_type.SkipEnumDefinition());

     unp->u_type->unparseType(referenceNodeType, ninfo_for_type);

     SgInitializedName* initializedName = isSgInitializedName(referenceNode);
     if (initializedName != NULL)
        {
          SgName tmp_name  = initializedName->get_name();
          curprint(tmp_name.str());
        }

     newInfo.set_isTypeSecondPart();

     unp->u_type->unparseType(referenceNodeType, newInfo);
   }

