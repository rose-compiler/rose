/* unparse_type_fort.C 
 * 
 * Code to unparse Sage/Fortran type nodes.
 * 
 */
#include "sage3basic.h"
#include "unparser.h"

//----------------------------------------------------------------------------
//  void UnparserFort::unparseType
//
//  General unparse function for types. Routes work to the appropriate
//  helper function.
// 
//  NOTE: Unparses the type as a *declaration*. In Fortran, types only
//  appear in the declaration sections (not in casts, etc.)
//----------------------------------------------------------------------------

void
UnparseFortran_type::unparseType(SgType* type, SgUnparse_Info& info, bool printAttrs)
   {
     ASSERT_not_null(type);

     switch (type->variantT())
        {
          case V_SgTypeUnknown:
             {
               printf ("Error: SgTypeUnknown should not be found in AST \n");
               ROSE_ABORT();
             }

          case V_SgTypeDefault:
             {
               curprint("integer");

               if ( SgProject::get_verbose() > 1 )
                  {
                    printf ("Warning: SgTypeDefault should not be found in AST \n");
                  }
               break;
             }

       // SgTypeVoid might be required for function handling, but for Fortran we don't unparse anything
          case V_SgTypeVoid:              break;

          case V_SgTypeString:           unparseStringType(type, info, printAttrs); break;

       // scalar integral types
          case V_SgTypeChar:             unparseBaseType(type,"CHARACTER",info); break;
          case V_SgTypeInt:              unparseBaseType(type,"INTEGER",info); break;
          case V_SgTypeSignedInt:        unparseBaseType(type,"INTEGER",info); break;
          case V_SgTypeUnsignedInt:      unparseBaseType(type,"INTEGER",info); break;

       // scalar floating point types
          case V_SgTypeFloat:            unparseBaseType(type,"REAL",info); break;
          case V_SgTypeDouble:           unparseBaseType(type,"DOUBLE PRECISION",info); break;

       // scalar boolean type
          case V_SgTypeBool:             unparseBaseType(type,"LOGICAL",info); break;

       // complex type
          case V_SgTypeComplex:
             {
                SgTypeComplex* complexType = isSgTypeComplex(type);
                ASSERT_not_null(complexType);
                if (isSgTypeDouble(complexType->get_base_type()))
                   {
                      unparseBaseType(type,"DOUBLE COMPLEX",info);
                   }
                else
                   {
                      unparseBaseType(type,"COMPLEX",info);
                   }
                break;
             }

       // Rice coarrays
          case V_SgTypeCAFTeam:          unparseBaseType(type,"TEAM",info); break;

          case V_SgTypeCrayPointer:      unparseBaseType(type,"POINTER",info); break;

       // array type
          case V_SgArrayType:            unparseArrayType(type, info, printAttrs); break;

       // pointer and reference support    
          case V_SgPointerType:          unparsePointerType(type, info, printAttrs); break;
          case V_SgReferenceType:        unparseReferenceType(type, info); break;

          case V_SgClassType:            unparseClassType(type, info); break;

       // unparse kind and type parameters
          case V_SgModifierType:         unparseModifierType(type, info); break;

          case V_SgFunctionType:         unparseFunctionType(type, info); break;

          default: 
             {
               printf("UnparserFort::unparseType: Error: No handler for %s (variant: %d)\n",type->sage_class_name(), type->variantT());
               ROSE_ABORT();
             }
        }
   }

//----------------------------------------------------------------------------
//  UnparserFort::<>
//----------------------------------------------------------------------------

void 
UnparseFortran_type::unparseTypeKind(SgType* type, SgUnparse_Info& info)
   {
     SgExpression* kindExpression = type->get_type_kind();
     if (kindExpression != nullptr)
        {
          curprint("(");
          curprint("kind=");
          unp->u_fortran_locatedNode->unparseExpression(kindExpression,info);
          curprint(")");
        }
   }

void
UnparseFortran_type::unparseTypeLengthAndKind(SgType* type, SgExpression* lengthExpression, SgUnparse_Info & info)
   {
     SgExpression* kindExpression = type->get_type_kind();
     if (lengthExpression != nullptr || kindExpression != nullptr)
        {
          curprint("(");

          if (lengthExpression != NULL)
             {
               curprint("len=");
               unp->u_fortran_locatedNode->unparseExpression(lengthExpression,info);

            // Check if there will be a kind paramter.
               if (kindExpression != NULL)
                  {
                    curprint(",");
                  }
             }

          if (kindExpression != NULL)
             {
               curprint("kind=");
               unp->u_fortran_locatedNode->unparseExpression(kindExpression,info);
             }

          curprint(")");
        }
   }

void 
UnparseFortran_type::unparseBaseType(SgType* type, const std::string & nameOfType, SgUnparse_Info & info)
   {
     curprint(nameOfType);
     unparseTypeKind(type,info);
   }

void 
UnparseFortran_type::unparseStringType(SgType* type, SgUnparse_Info& info, bool printAttrs)
   {
     SgTypeString* string_type = isSgTypeString(type);
     ASSERT_not_null(string_type);
     curprint ("CHARACTER");
     if (printAttrs)
        unparseTypeLengthAndKind(string_type,string_type->get_lengthExpression(),info);
     else //   the length will be printed as part of the entity_decl.
        unparseTypeKind(type, info);
   }


void 
UnparseFortran_type::unparseArrayType(SgType* type, SgUnparse_Info& info, bool printDim)
   {
  // Examples: 
  //   real, dimension(10, 10) :: A1, A2
  //   real, dimension(:) :: B1
  //   character(len=*) :: s1

     SgArrayType* array_type = isSgArrayType(type);
     ASSERT_not_null(array_type);

     if (info.supressStrippedTypeName() == false)
        {
       // only output the name of the stripped type once
          SgType* stripType = array_type->stripType();
          unparseType(stripType, info);
          info.set_supressStrippedTypeName();
        }

     if (printDim)
     {
        ROSE_ASSERT(array_type->get_rank() >= 1);
        curprint(array_type->get_isCoArray()? ", CODIMENSION": ", DIMENSION");

        ASSERT_not_null(unp);
        ASSERT_not_null(unp->u_fortran_locatedNode);

        if (array_type->get_isCoArray())
        {  // print codimension info
           curprint("[");
           unp->u_fortran_locatedNode->unparseExprList(array_type->get_dim_info(),info);
           curprint("]");
        }
        else  // print dimension info
        {
           curprint("(");
           unp->u_fortran_locatedNode->unparseExprList(array_type->get_dim_info(),info);
           curprint(")");
        }
     }

     if (array_type->get_base_type()->containsInternalTypes() == true)
        {
          unparseType(array_type->get_base_type(), info, printDim);
        }
   }

void 
UnparseFortran_type::unparsePointerType(SgType* type, SgUnparse_Info& info, bool printAttrs)
   {
     SgPointerType* pointer_type = isSgPointerType(type);
     ASSERT_not_null(pointer_type);

     if (info.supressStrippedTypeName() == false)
        {
       // only output the name of the stripped type once!
          SgType* stripType = pointer_type->stripType();
          unparseType(stripType, info);
          info.set_supressStrippedTypeName();
        }

     curprint(type->get_isCoArray()? ", COPOINTER": ", POINTER");

     if (pointer_type->get_base_type()->containsInternalTypes() == true)
        {
          unparseType(pointer_type->get_base_type(), info, printAttrs);
        }
   }

void 
UnparseFortran_type::unparseReferenceType(SgType* type, SgUnparse_Info& info)
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
        {
          ninfo.set_isReferenceToSomething();
        }

     if (ninfo.isTypeFirstPart())
        {
          unparseType(ref_type->get_base_type(), ninfo);
          curprint("&");
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

void
UnparseFortran_type::unparseClassType(SgType* type, SgUnparse_Info& info)
   {
     SgClassType* class_type = isSgClassType(type);
     ASSERT_not_null(class_type);

     if (info.isTypeSecondPart() == false)
        {
       // Fortran is not as complex as C++, so we can, at least for now, skip the name qualification!
          ASSERT_not_null(class_type);
          curprint("TYPE ( ");
          curprint(class_type->get_name().str());
          curprint(" ) ");
        }
   }


void 
UnparseFortran_type::unparseModifierType(SgType* type, SgUnparse_Info& info) 
   {
     SgModifierType* mod_type = isSgModifierType(type);
     ASSERT_not_null(mod_type);

     unparseType(mod_type->get_base_type(), info);

     SgExpression* kindExpression = mod_type->get_type_kind();
     if (kindExpression != nullptr)
        {
          curprint("(");
          if (kindExpression != NULL)
             {
               curprint("kind=");
               unp->u_fortran_locatedNode->unparseExpression(kindExpression,info);
             }
          curprint(")");
        }
   }


void
UnparseFortran_type::unparseFunctionType(SgType* type, SgUnparse_Info& info)
   {
     SgFunctionType* func_type = isSgFunctionType(type);
     ASSERT_not_null(func_type);

     SgUnparse_Info ninfo(info);

     curprint("procedure(), pointer");
   }


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

void
UnparseFortran_type::curprint (const std::string & str) const
   {
     unp->u_sage->curprint(str);
   }

bool
UnparseFortran_type::isCharType(SgType* type) 
   {
     switch (type->variantT()) 
        {
          case V_SgTypeChar:
          case V_SgTypeSignedChar:
          case V_SgTypeUnsignedChar:
               return true;
      
          default:
               return false;
        }
   }
