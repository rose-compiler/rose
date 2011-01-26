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
UnparseFortran_type::unparseType(SgType* type, SgUnparse_Info& info)
   {
     ROSE_ASSERT(type != NULL);

#if 0
     printf("In unparseType: %s\n", type->class_name().c_str());
  // cur << "\n/* Begin unparseType: " << type->class_name() << " */\n";
     curprint("\n! Begin unparseType:\n");
#endif
#if 0
     if (isDebug())
        {
          cout << "entering case for %s" << type->class_name() << endl;
        }
#endif

     switch (type->variantT())
        {
       // case V_SgTypeUnknown:          curprint(type->sage_class_name()); break;
       // case V_SgTypeDefault:          curprint(type->sage_class_name()); break;
          case V_SgTypeUnknown:
             {
               printf ("Error: SgTypeUnknown should not be found in AST \n");
               ROSE_ASSERT(false);
               break;
             }

          case V_SgTypeDefault:
             {
#if 0
            // DQ (12/29/2010): This provides a more obvious way to spot where default types are used (where we used to 
            // instead use SgTypeInt as the default type it would output "integer".
               curprint("ROSE_DEFAULT_TYPE");
#else
            // DQ (12/29/2010): We used to store an SgTypeInt in the AST for the default type, now we more correctly store 
            // the SgDefaultType, but we don't fix them all up yet (but at least they are more explicit in the AST).
            // To make the generated code more equivalent we output "integer" for the SgTypeDefault, until it is fixed
            // better.  Note that all of the test code in Fortran_tests/*.f,f90,f03 pass without this translation via 
            // the backend.
               curprint("integer");
#endif

            // DQ (1/25/2011): We now try to to translations of the use of SgTypeDefault when we see it in the AST.  This work in incomplete, so we issue a warning at the moment.
               if ( SgProject::get_verbose() > 1 )
                  {
                    printf ("Warning: SgTypeDefault should not be found in AST \n");
                  }
            // ROSE_ASSERT(false);
               break;
             }

       // case V_SgTypeVoid:             curprint(type->sage_class_name()); break;
       // case V_SgTypeVoid:             curprint("void"); break;

       // DQ (10/5/2010): Comment added: SgTypeVoid might be required for function handling, but for Fortran we don't unparse anything here.
       // FMZ 6/17/2009 
          case V_SgTypeVoid:              break;

       // DQ (8/16/2007): I don't think that SgGlobalVoid is used!
       // case V_SgTypeGlobalVoid:       curprint(type->sage_class_name()); break;

       // DQ (10/5/2010): I don't think that SgTypeWchar is used!
       // case V_SgTypeWchar:            curprint(type->sage_class_name()); break;

       // DQ (8/15/2010): I think we were not using the SgStringType before now.
       // case V_SgTypeString:           curprint("CHARACTER(LEN=*)"); break;
          case V_SgTypeString:           unparseStringType(type, info); break;

       // scalar integral types
          case V_SgTypeChar:             unparseBaseType(type,"CHARACTER",info); break;
       // case V_SgTypeSignedChar:       unparseBaseType(type,"CHARACTER",info); break;
       // case V_SgTypeUnsignedChar:     unparseBaseType(type,"CHARACTER",info); break;

       // case V_SgTypeShort:            unparseBaseType(type,"INTEGER",info); break;
       // case V_SgTypeSignedShort:      unparseBaseType(type,"INTEGER",info); break;
       // case V_SgTypeUnsignedShort:    unparseBaseType(type,"INTEGER",info); break;

          case V_SgTypeInt:              unparseBaseType(type,"INTEGER",info); break;
          case V_SgTypeSignedInt:        unparseBaseType(type,"INTEGER",info); break;
          case V_SgTypeUnsignedInt:      unparseBaseType(type,"INTEGER",info); break;

       // case V_SgTypeLong:             unparseBaseType(type,"INTEGER",info); break;
       // case V_SgTypeSignedLong:       unparseBaseType(type,"INTEGER",info); break;
       // case V_SgTypeUnsignedLong:     unparseBaseType(type,"INTEGER",info); break;

       // case V_SgTypeLongLong:         unparseBaseType(type,"INTEGER",info); break;
       // case V_SgTypeUnsignedLongLong: unparseBaseType(type,"INTEGER",info); break;

       // scalar floating point types
          case V_SgTypeFloat:            unparseBaseType(type,"REAL",info); break;
          case V_SgTypeDouble:           unparseBaseType(type,"DOUBLE PRECISION",info); break;
       // case V_SgTypeLongDouble:       unparseBaseType(type,"QUAD PRECISION",info); break;

       // scalar boolean type
          case V_SgTypeBool:             unparseBaseType(type,"LOGICAL",info); break;

       // complex type
          case V_SgTypeComplex:          unparseBaseType(type,"COMPLEX",info); break;

       // FMZ (2/2/2009): Add image_team for co-array team declaration
          case V_SgTypeCAFTeam:          unparseBaseType(type,"TEAM",info); break;

       // FMZ (4/14/2009): Added cray pointer
          case V_SgTypeCrayPointer:      unparseBaseType(type,"POINTER",info); break;

       // array type
          case V_SgArrayType:            unparseArrayType(type, info); break;

       // pointer and reference support    
          case V_SgPointerType:          unparsePointerType(type, info); break;
          case V_SgReferenceType:        unparseReferenceType(type, info); break;

       // DQ (8/26/2007): This is relavant to derived types
          case V_SgClassType:            unparseClassType(type, info); break;

       // DQ (12/1/2007): We need to unparse the kind and type parameters
          case V_SgModifierType:         unparseModifierType(type, info); break;

       // DQ (1/24/2011): Added to support procedure pointers (see test2011_28.f90).
          case V_SgFunctionType:         unparseFunctionType(type, info); break;

#if 0
       // DQ (8/15/2007): I don't think these apply to Fortran.
          case V_SgNamedType:            unparseNameType(type, info); break;
       // case V_SgEnumType:             unparseEnumType(type, info); break;
          case V_SgTypedefType:          unparseTypedefType(type, info); break;
          case V_SgMemberFunctionType:   unparseMemberFunctionType(type, info); break;
#endif
          default: 
             {
               printf("UnparserFort::unparseType: Error: No handler for %s (variant: %d)\n",type->sage_class_name(), type->variantT());
               ROSE_ASSERT(false);
               break;
             }
        }

#if 0
  // DQ (12/1/2007): This has been moved to the SgModifierType
     SgExpression* kindExpression = type->get_type_kind();
  // printf ("In UnparseFortran_type::unparseType(): type->get_type_kind() = %p \n",type->get_type_kind());
     if (kindExpression != NULL)
        {
          curprint("(");
          curprint("kind=");
          unp->u_fortran_locatedNode->unparseExpression(kindExpression,info);
          curprint(")");
        }
#endif

#if 0
     printf ("End unparseType: %s\n",type->class_name().c_str());
  // curprint ("\n/* End unparseType: "  << type->class_name() << " */\n");
     curprint("\n! End unparseType: \n");
#endif
   }


//----------------------------------------------------------------------------
//  UnparserFort::<>
//----------------------------------------------------------------------------


void 
UnparseFortran_type::unparseTypeKind(SgType* type, SgUnparse_Info& info)
   {
  // DQ (12/1/2007): This has been moved to the SgModifierType
     SgExpression* kindExpression = type->get_type_kind();
  // printf ("In UnparseFortran_type::unparseType(): type->get_type_kind() = %p \n",type->get_type_kind());
     if (kindExpression != NULL)
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
  // DQ (12/1/2007): This has been moved to the SgModifierType
     SgExpression* kindExpression = type->get_type_kind();
  // printf ("In UnparseFortran_type::unparseType(): type->get_type_kind() = %p \n",type->get_type_kind());

     if (lengthExpression != NULL || kindExpression != NULL)
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
  // printf ("Inside of UnparserFort::unparseBaseType \n");
  // curprint ("\n! Inside of UnparserFort::unparseBaseType \n");
     curprint(nameOfType);
     unparseTypeKind(type,info);
  // curprint ("\n! Leaving UnparserFort::unparseBaseType \n");
   }

void 
UnparseFortran_type::unparseStringType(SgType* type, SgUnparse_Info& info)
   {
  // printf ("Inside of UnparserFort::unparseStringType \n");
  // cur << "\n/* Inside of UnparserFort::unparseStringType */\n";
  
     SgTypeString* string_type = isSgTypeString(type);
     ROSE_ASSERT(string_type != NULL);
#if 0
  // curprint("CHARACTER(LEN=*)");
     curprint("CHARACTER(LEN=");

  // DQ (8/17/2010): Changed the name of this variable (only used in Fortran codes).
  // SgExpression* lengthExpression = string_type->get_index();
     if (string_type->get_definedUsingScalarLength())
        {
       // Output the scalar integer value
          curprint(StringUtility::numberToString(string_type->get_lengthScalar()));
        }
       else
        {
       // Output the integer expression.
          SgExpression* lengthExpression = string_type->get_lengthExpression();
          ROSE_ASSERT(lengthExpression != NULL);

       // UnparseLanguageIndependentConstructs::unparseExpression(lengthExpression,info);
          unp->u_fortran_locatedNode->unparseExpression(lengthExpression,info);
        }

     curprint(")");
#else
     curprint ("character");
     unparseTypeLengthAndKind(string_type,string_type->get_lengthExpression(),info);
#endif

  // printf ("Leaving of UnparserFortran_type::unparseStringType \n");
  // cur << "\n/* Leaving of UnparserFort::unparseStringType */\n";
   }


void 
UnparseFortran_type::unparseArrayType(SgType* type, SgUnparse_Info& info) 
   {
  // Examples: 
  //   real, dimension(10, 10) :: A1, A2
  //   real, dimension(:) :: B1
  //   character(len=*) :: s1
#if 0
     curprint ("\n! Inside of UnparserFort::unparseArrayType \n");
#endif

     SgArrayType* array_type = isSgArrayType(type);
     ROSE_ASSERT(array_type != NULL);

  // element type
#if 0
     unparseType(array_type->get_base_type(), info);
#else
  // I think that supressStrippedTypeName() and SkipBaseType() are redundant...
     if (info.supressStrippedTypeName() == false)
        {
       // DQ (1/16/2011): We only want to output the name of the stripped type once!
          SgType* stripType = array_type->stripType();
          unparseType(stripType, info);
          info.set_supressStrippedTypeName();
        }
#endif

  // DQ (8/5/2010): It is an error to treat an array of char as a string (see test2010_16.f90).
#if 0
  // dimension information
     SgExprListExp* dim = array_type->get_dim_info();

  // if (isCharType(array_type->get_base_type()))
  // if (false && isCharType(array_type->get_base_type()))
     if (isCharType(array_type->get_base_type()))
        {
       // a character type: must be treated specially
          ROSE_ASSERT(array_type->get_rank() == 1);
          curprint("(len=");

          SgExpressionPtrList::iterator it = dim->get_expressions().begin();
          if (it != dim->get_expressions().end())
             {
               SgExpression* expr = *it;
               if (expr->variantT() == V_SgSubscriptExpression)
                  {
                 // this is a subscript expression but all we want to unparse is the length
                 // of the string, which should be the upper bound of the subscript expression
                    SgSubscriptExpression* sub_expr = isSgSubscriptExpression(expr);
                    ROSE_ASSERT(sub_expr != NULL);

                    ROSE_ASSERT(unp != NULL);
                    ROSE_ASSERT(unp->u_fortran_locatedNode != NULL);
                    unp->u_fortran_locatedNode->unparseExpression(sub_expr->get_upperBound(), info);
                  }
                 else
                  {
                 // unparse the entire expression
                    ROSE_ASSERT(unp != NULL);
                    ROSE_ASSERT(unp->u_fortran_locatedNode != NULL);
                    unp->u_fortran_locatedNode->unparseExpression(*it, info);
                  }
             }
            else
             {
               curprint("*");
             }
          curprint(")");
        }
       else
        {
       // a non-character type

       // explicit-shape (explicit rank and bounds/extents)
       // assumed-shape (explicit rank; unspecified bounds/extents)
       // deferred-shape (explicit rank; unspecified bounds/extents)
       // assumed-size (explicit ranks, explicit bounds/extents except last dim)
          ROSE_ASSERT(array_type->get_rank() >= 1);
          curprint(", DIMENSION");

          ROSE_ASSERT(unp != NULL);
          ROSE_ASSERT(unp->u_fortran_locatedNode != NULL);
       // unp->u_fortran_locatedNode->unparseExprList(dim, info); // adds parens
       // unp->u_fortran_locatedNode->UnparseLanguageIndependentConstructs::unparseExprList(dim, info); // adds parens

       // curprint("(");
       // curprint( StringUtility::numberToString(array_type->get_rank()) );
       // curprint(")");

       // unp->u_fortran_locatedNode->unparseExpression(array_type->get_dim_info(),info);
          unp->u_fortran_locatedNode->unparseExprList(array_type->get_dim_info(),info,/* output parens */ true);
        }
#else
     ROSE_ASSERT(array_type->get_rank() >= 1);
     curprint(", DIMENSION");

     ROSE_ASSERT(unp != NULL);
     ROSE_ASSERT(unp->u_fortran_locatedNode != NULL);

     unp->u_fortran_locatedNode->unparseExprList(array_type->get_dim_info(),info,/* output parens */ true);

  // DQ (1/16/2011): Plus unparse the base type...(unless it will just output the stripped types name).
     if (array_type->get_base_type()->containsInternalTypes() == true)
        {
          unparseType(array_type->get_base_type(), info);
        }
#endif

#if 0
     curprint ("\n! Leaving UnparserFort::unparseArrayType \n");
#endif
   }

void 
UnparseFortran_type::unparsePointerType(SgType* type, SgUnparse_Info& info)
   {
#if 0
  // printf ("Inside of UnparserFort::unparsePointerType \n");
  // cur << "\n/* Inside of UnparserFort::unparsePointerType */\n";
     curprint ("\n! Inside of UnparserFort::unparsePointerType \n");
#endif

  // DQ (1/16/2011): Note that pointers in fortran are not expressed the same as in C/C++, are are
  // only a part of the type which is managed more directly using attributes in the variable declaration.
  // Not clear that we want to do anything here in the unparser...

     SgPointerType* pointer_type = isSgPointerType(type);
     ROSE_ASSERT(pointer_type != NULL);

#if 0
  /* special cases: ptr to array, int (*p) [10] */ 
  /*                ptr to function, int (*p)(int) */
  /*                ptr to ptr to .. int (**p) (int) */

     if (isSgReferenceType(pointer_type->get_base_type()) || 
         isSgPointerType(pointer_type->get_base_type()) ||
         isSgArrayType(pointer_type->get_base_type()) ||
         isSgFunctionType(pointer_type->get_base_type()) ||
         isSgMemberFunctionType(pointer_type->get_base_type()) ||
         isSgModifierType(pointer_type->get_base_type()) )
        {
          info.set_isPointerToSomething();
        }
  
  // If not isTypeFirstPart nor isTypeSecondPart this unparse call
  // is not controlled from the statement level but from the type level
  
     if (info.isTypeFirstPart() == true)
        {
          unparseType(pointer_type->get_base_type(), info);
    
       // DQ (9/21/2004): Moved this conditional into this branch (to fix test2004_93.C)
       // DQ (9/21/2004): I think we can assert this, and if so we can simplify the logic below
          ROSE_ASSERT(info.isTypeSecondPart() == false);
    
          curprint("*");
        }
       else
        {
          if (info.isTypeSecondPart() == true)
             {
               unparseType(pointer_type->get_base_type(), info);
             }
            else
             {
               SgUnparse_Info ninfo(info);
               ninfo.set_isTypeFirstPart();
               unparseType(pointer_type, ninfo);
               ninfo.set_isTypeSecondPart();
               unparseType(pointer_type, ninfo);
             }
        }
#else
     if (info.supressStrippedTypeName() == false)
        {
       // DQ (1/16/2011): We only want to output the name of the stripped type once!
          SgType* stripType = pointer_type->stripType();
          unparseType(stripType, info);
          info.set_supressStrippedTypeName();
        }

     curprint(", POINTER");

  // DQ (1/16/2011): Plus unparse the base type...(unless it will just output the stripped types name).
     if (pointer_type->get_base_type()->containsInternalTypes() == true)
        {
          unparseType(pointer_type->get_base_type(), info);
        }
#endif

#if 0
  // printf ("Leaving of UnparserFort::unparsePointerType \n");
  // cur << "\n/* Leaving of UnparserFort::unparsePointerType */\n";
     curprint ("\n! Leaving UnparserFort::unparsePointerType \n");
#endif
   }

void 
UnparseFortran_type::unparseReferenceType(SgType* type, SgUnparse_Info& info)
   {
     SgReferenceType* ref_type = isSgReferenceType(type);
     ROSE_ASSERT(ref_type != NULL);
  
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
       // curprint("& /* reference */ ");
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


// DQ (8/14/2007): This function does not make sense for Fortran.
void
UnparseFortran_type::unparseClassType(SgType* type, SgUnparse_Info& info)
   {
  // printf ("Inside of UnparserFortran::unparseClassType \n");

     SgClassType* class_type = isSgClassType(type);
     ROSE_ASSERT(class_type != NULL);

  // DQ (10/7/2004): We need to output just the name when isTypeFirstPart == false and isTypeSecondPart == false
  // this allows us to handle: "doubleArray* arrayPtr2 = new doubleArray();"
     if (info.isTypeSecondPart() == false)
        {
       // Fortran is not as complex as C++, so we can, at least for now, skip the name qualification!
          ROSE_ASSERT(class_type != NULL);
          curprint("TYPE ( ");
          curprint(class_type->get_name().str());
          curprint(" ) ");
        }
   }


void 
UnparseFortran_type::unparseModifierType(SgType* type, SgUnparse_Info& info) 
   {
     SgModifierType* mod_type = isSgModifierType(type);
     ROSE_ASSERT(mod_type != NULL);

  // printf ("Not clear what the Fortran specific type modifiers will be, UnparseFortran_type::unparseModifierType() not implemented! \n");

     unparseType(mod_type->get_base_type(), info);

  // DQ (12/1/2007): This has been moved from SgType to the SgModifierType
     SgExpression* kindExpression          = mod_type->get_type_kind();

  // DQ (10/2/2010): The type parameter is not longer used since the length is represented explicitly for strings in SgTypeString.
  // SgExpression* typeParameterExpression = mod_type->get_type_parameter();

  // printf ("In UnparseFortran_type::unparseModifierType(): mod_type->get_type_kind() = %p mod_type->get_type_parameter() = %p \n",mod_type->get_type_kind(),mod_type->get_type_parameter());

  // DQ (10/2/2010): The type parameter is not longer used since the length is represented explicitly for strings in SgTypeString.
  // if (kindExpression != NULL || typeParameterExpression != NULL)
     if (kindExpression != NULL)
        {
          curprint("(");
          if (kindExpression != NULL)
             {
               curprint("kind=");
               unp->u_fortran_locatedNode->unparseExpression(kindExpression,info);
             }
#if 0
        // DQ (10/2/2010): The type parameter is not longer used since the length is represented explicitly for strings in SgTypeString.
          if (typeParameterExpression != NULL)
             {
               if (kindExpression != NULL)
                  {
                    curprint(",");
                  }
               curprint("len=");
               unp->u_fortran_locatedNode->unparseExpression(typeParameterExpression,info);
             }
#endif
          curprint(")");
        }
   }


void
UnparseFortran_type::unparseFunctionType(SgType* type, SgUnparse_Info& info)
   {
     SgFunctionType* func_type = isSgFunctionType(type);
     ROSE_ASSERT (func_type != NULL);

     SgUnparse_Info ninfo(info);

  // DQ (1/24/2011): The case of a procedure type in Fortran is quite simple.
  // Note that test2011_28.f90 demonstrates an example of this.
  // curprint("procedure()");
     curprint("procedure(), pointer");

#if 0
  int needParen = 0;
  if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething()) {
      needParen=1;
    }

  // DQ (10/8/2004): Skip output of class definition for return type! C++ standard does not permit 
  // a defining declaration within a return type, function parameter, or sizeof expression. 
  ninfo.set_SkipClassDefinition();

  if (ninfo.isTypeFirstPart()) {
      if (needParen) {
	  ninfo.unset_isReferenceToSomething();
	  ninfo.unset_isPointerToSomething();
	  unparseType(func_type->get_return_type(), ninfo);
	  curprint("(");
	}
      else {
	  unparseType(func_type->get_return_type(), ninfo);
	}
    }
  else {
      if (ninfo.isTypeSecondPart()) {
	  if (needParen) {
	      curprint(")");
	      info.unset_isReferenceToSomething();
	      info.unset_isPointerToSomething();
	    }
	  // print the arguments
	  SgUnparse_Info ninfo2(info); 
	  ninfo2.unset_SkipBaseType();
	  ninfo2.unset_isTypeSecondPart();
	  ninfo2.unset_isTypeFirstPart();

	  curprint("(");
	  SgTypePtrList::iterator p = func_type->get_arguments().begin();
	  while(p != func_type->get_arguments().end())
	    {
	      // printf ("Output function argument ... \n");
	      unparseType(*p, ninfo2);
	      p++;
	      if (p != func_type->get_arguments().end())
		{ curprint(", "); }
	    }
	  curprint(")");
	  unparseType(func_type->get_return_type(), info); // catch the 2nd part of the rtype
	}
      else
	{
	  ninfo.set_isTypeFirstPart();
	  unparseType(func_type, ninfo);
	  ninfo.set_isTypeSecondPart();
	  unparseType(func_type, ninfo);
	}
    }
#endif

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

