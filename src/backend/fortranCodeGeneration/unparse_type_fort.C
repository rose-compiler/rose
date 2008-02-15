/* unparse_type_fort.C 
 * 
 * Code to unparse Sage/Fortran type nodes.
 * 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <rose.h>
#include "unparser_fort.h"


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
UnparserFort::unparseType(SgType* type, SgUnparse_Info& info)
{
  ROSE_ASSERT(type != NULL);

#if 0
  printf("Beg unparseType: %s\n", type->sage_class_name());
  cur << "\n/* Beg unparseType: " << type->sage_class_name() << " */\n";
#endif

  if (isDebug()) {
    cout << "entering case for %s" << type->sage_class_name() << endl;
  }
  
  switch (type->variantT())
    {
    case V_SgTypeUnknown:          cur << type->sage_class_name(); break;
    case V_SgTypeDefault:          cur << type->sage_class_name(); break;

    case V_SgTypeVoid:             cur << type->sage_class_name(); break;
    case V_SgTypeGlobalVoid:       cur << type->sage_class_name(); break;

      // character, string types
    case V_SgTypeChar:             cur << "CHARACTER"; break;
    case V_SgTypeSignedChar:       cur << "CHARACTER"; break;
    case V_SgTypeUnsignedChar:     cur << "CHARACTER"; break;

    case V_SgTypeWchar:            cur << type->sage_class_name(); break;
    case V_SgTypeString:           cur << "CHARACTER(LEN=*)"; break;

      // scalar integral types
    case V_SgTypeShort:            cur << "INTEGER(ROSE_TY_I2)"; break;
    case V_SgTypeSignedShort:      cur << "INTEGER(ROSE_TY_I2)"; break;
    case V_SgTypeUnsignedShort:    cur << "INTEGER(ROSE_TY_I2)"; break;

    case V_SgTypeInt:              cur << "INTEGER(ROSE_TY_I4)"; break;
    case V_SgTypeSignedInt:        cur << "INTEGER(ROSE_TY_I4)"; break;
    case V_SgTypeUnsignedInt:      cur << "INTEGER(ROSE_TY_I4)"; break;

    case V_SgTypeLong:             cur << "INTEGER(ROSE_TY_I4)"; break;
    case V_SgTypeSignedLong:       cur << "INTEGER(ROSE_TY_I4)"; break;
    case V_SgTypeUnsignedLong:     cur << "INTEGER(ROSE_TY_I4)"; break;

    case V_SgTypeLongLong:         cur << "INTEGER(ROSE_TY_I8)"; break;
    case V_SgTypeUnsignedLongLong: cur << "INTEGER(ROSE_TY_I8)"; break;

      // scalar floating point types
    case V_SgTypeFloat:            cur << "REAL(ROSE_TY_R4)"; break;
    case V_SgTypeDouble:           cur << "REAL(ROSE_TY_R8)"; break;
    case V_SgTypeLongDouble:       cur << "REAL(ROSE_TY_R8)"; break;

      // scalar boolean type
    case V_SgTypeBool:             cur << "LOGICAL"; break;

      // complex type
    case V_SgComplex:              cur << "COMPLEX"; break;

      // 
    case V_SgArrayType:          unparseArrayType(type, info); break;
    
      // FIXME:eraxxon
    case V_SgPointerType:        unparsePointerType(type, info); break;
    case V_SgReferenceType:      unparseReferenceType(type, info); break;
    case V_SgNamedType:          unparseNameType(type, info); break;
    case V_SgClassType:          unparseClassType(type, info); break;
    case V_SgEnumType:           unparseEnumType(type, info); break;
    case V_SgTypedefType:        unparseTypedefType(type, info); break;
    case V_SgModifierType:       unparseModifierType(type, info); break;
    case V_SgFunctionType:       unparseFunctionType(type, info); break;
    case V_SgMemberFunctionType: unparseMemberFunctionType(type, info); break;

    default: 
      printf("UnparserFort::unparseType: Error: No handler for %s (variant: %d)\n", 
	     type->sage_class_name(), type->variantT());
      ROSE_ASSERT(false);
      break;
    }

#if 0
  printf ("End unparseType: %s\n", type->sage_class_name());
  cur << "\n/* End unparseType: "  << type->sage_class_name() << " */\n";
#endif
}

//----------------------------------------------------------------------------
//  void UnparserFort::unparseTypeForConstExprs
//
//  Unparse function for types.  Instead of using ROSE__TYPES, uses
//  explicit (ugly) REAL*8, etc.  Routes work to the appropriate
//  helper function.  This function should only be used in those cases
//  where mfef90 will generate an error message if it sees a REAL(ROSE_TY_R8)
//  instead of REAL*8.
// 
//  NOTE: Unparses the type as a *declaration*. In Fortran, types only
//  appear in the declaration sections (not in casts, etc.)
//----------------------------------------------------------------------------

void
UnparserFort::unparseTypeForConstExprs(SgType* type, SgUnparse_Info& info)
{
  ROSE_ASSERT(type != NULL);

#if 0
  printf("Beg unparseTypeForConstExprs: %s\n", type->sage_class_name());
  cur << "\n/* Beg unparseTypeForConstExprs: " << type->sage_class_name() << " */\n";
#endif

  if (isDebug()) {
    cout << "entering case for %s" << type->sage_class_name() << endl;
  }
  
  switch (type->variantT())
    {
    case V_SgTypeUnknown:          cur << type->sage_class_name(); break;
    case V_SgTypeDefault:          cur << type->sage_class_name(); break;

    case V_SgTypeVoid:             cur << type->sage_class_name(); break;
    case V_SgTypeGlobalVoid:       cur << type->sage_class_name(); break;

      // character, string types
    case V_SgTypeChar:             cur << "CHARACTER"; break;
    case V_SgTypeSignedChar:       cur << "CHARACTER"; break;
    case V_SgTypeUnsignedChar:     cur << "CHARACTER"; break;

    case V_SgTypeWchar:            cur << type->sage_class_name(); break;
    case V_SgTypeString:           cur << "CHARACTER(LEN=*)"; break;

      // scalar integral types
    case V_SgTypeShort:            cur << "INTEGER*2"; break;
    case V_SgTypeSignedShort:      cur << "INTEGER*2"; break;
    case V_SgTypeUnsignedShort:    cur << "INTEGER*2"; break;

    case V_SgTypeInt:              cur << "INTEGER*4"; break;
    case V_SgTypeSignedInt:        cur << "INTEGER*4"; break;
    case V_SgTypeUnsignedInt:      cur << "INTEGER*4"; break;

    case V_SgTypeLong:             cur << "INTEGER*4"; break;
    case V_SgTypeSignedLong:       cur << "INTEGER*4"; break;
    case V_SgTypeUnsignedLong:     cur << "INTEGER*4"; break;

    case V_SgTypeLongLong:         cur << "INTEGER*8"; break;
    case V_SgTypeUnsignedLongLong: cur << "INTEGER*8"; break;

      // scalar floating point types
    case V_SgTypeFloat:            cur << "REAL*4"; break;
    case V_SgTypeDouble:           cur << "REAL*8"; break;
    case V_SgTypeLongDouble:       cur << "REAL*8"; break;

      // scalar boolean type
    case V_SgTypeBool:             cur << "LOGICAL"; break;

      // complex type
    case V_SgComplex:              cur << "COMPLEX"; break;

      // 
    case V_SgArrayType:          unparseArrayType(type, info); break;
    
      // FIXME:eraxxon
    case V_SgPointerType:        unparsePointerType(type, info); break;
    case V_SgReferenceType:      unparseReferenceType(type, info); break;
    case V_SgNamedType:          unparseNameType(type, info); break;
    case V_SgClassType:          unparseClassType(type, info); break;
    case V_SgEnumType:           unparseEnumType(type, info); break;
    case V_SgTypedefType:        unparseTypedefType(type, info); break;
    case V_SgModifierType:       unparseModifierType(type, info); break;
    case V_SgFunctionType:       unparseFunctionType(type, info); break;
    case V_SgMemberFunctionType: unparseMemberFunctionType(type, info); break;

    default: 
      printf("UnparserFort::unparseTypeForConstExprs: Error: No handler for %s (variant: %d)\n", 
	     type->sage_class_name(), type->variantT());
      ROSE_ASSERT(false);
      break;
    }

#if 0
  printf ("End unparseTypeForConstExprs: %s\n", type->sage_class_name());
  cur << "\n/* End unparseTypeForConstExprs: "  << type->sage_class_name() << " */\n";
#endif
}

//----------------------------------------------------------------------------
//  UnparserFort::<>
//----------------------------------------------------------------------------

void 
UnparserFort::unparseArrayType(SgType* type, SgUnparse_Info& info) 
{
  // Examples: 
  //   real, dimension(10, 10) :: A1, A2
  //   real, dimension(:) :: B1
  //   character(len=*) :: s1
  
  SgArrayType* array_type = isSgArrayType(type);
  ROSE_ASSERT(array_type != NULL);

  // element type
  unparseType(array_type->get_base_type(), info);
    
  // dimension information
  SgExprListExp* dim = array_type->get_dim_info();
  if (isCharType(array_type->get_base_type())) {
    // a character type: must be treated specially
    ROSE_ASSERT(array_type->get_rank() == 1);
    cur << "(len=";

    SgExpressionPtrList::iterator it = dim->get_expressions().begin();
    if (it != dim->get_expressions().end()) {  
      SgExpression* expr = *it;
      if (expr->variantT() == V_SgSubscriptExpression) {

	// this is a subscript expression but all we want to unparse is the length
	// of the string, which should be the upper bound of the subscript expression
	SgSubscriptExpression* sub_expr = isSgSubscriptExpression(expr);
	ROSE_ASSERT(sub_expr != NULL);
	unparseExpression(sub_expr->get_upperBound(), info);
      }
      else {  // unparse the entire expression
         unparseExpression(*it, info);
      }
    }
    else {
      cur << "*";
    }

    cur << ")";
  }
  else {
    // a non-character type
    
    // explicit-shape (explicit rank and bounds/extents)
    // assumed-shape (explicit rank; unspecified bounds/extents)
    // deferred-shape (explicit rank; unspecified bounds/extents)
    // assumed-size (explicit ranks, explicit bounds/extents except last dim)
    ROSE_ASSERT(array_type->get_rank() >= 1);
    cur << ", DIMENSION";
    unparseExprList(dim, info); // adds parens
  }
}

void 
UnparserFort::unparsePointerType(SgType* type, SgUnparse_Info& info)
{
  // printf ("Inside of UnparserFort::unparsePointerType \n");
  // cur << "\n/* Inside of UnparserFort::unparsePointerType */\n";
  
  SgPointerType* pointer_type = isSgPointerType(type);
  ROSE_ASSERT(pointer_type != NULL);

  /* special cases: ptr to array, int (*p) [10] */ 
  /*                ptr to function, int (*p)(int) */
  /*                ptr to ptr to .. int (**p) (int) */

  if (isSgReferenceType(pointer_type->get_base_type()) || 
      isSgPointerType(pointer_type->get_base_type()) ||
      isSgArrayType(pointer_type->get_base_type()) ||
      isSgFunctionType(pointer_type->get_base_type()) ||
      isSgMemberFunctionType(pointer_type->get_base_type()) ||
      isSgModifierType(pointer_type->get_base_type()) ) {
    info.set_isPointerToSomething();
  }
  
  // If not isTypeFirstPart nor isTypeSecondPart this unparse call
  // is not controlled from the statement level but from the type level
  
  if (info.isTypeFirstPart() == true) {
    unparseType(pointer_type->get_base_type(), info);
    
    // DQ (9/21/2004): Moved this conditional into this branch (to fix test2004_93.C)
    // DQ (9/21/2004): I think we can assert this, and if so we can simplify the logic below
    ROSE_ASSERT(info.isTypeSecondPart() == false);
    
    cur << "*";
  }
  else {
    if (info.isTypeSecondPart() == true) {
      unparseType(pointer_type->get_base_type(), info);
    }
    else {
      SgUnparse_Info ninfo(info);
      ninfo.set_isTypeFirstPart();
      unparseType(pointer_type, ninfo);
      ninfo.set_isTypeSecondPart();
      unparseType(pointer_type, ninfo);
    }
  }
  
  // printf ("Leaving of UnparserFort::unparsePointerType \n");
  // cur << "\n/* Leaving of UnparserFort::unparsePointerType */\n";
}

void 
UnparserFort::unparseReferenceType(SgType* type, SgUnparse_Info& info)
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
    ninfo.set_isReferenceToSomething();

  if (ninfo.isTypeFirstPart()) {
    unparseType(ref_type->get_base_type(), ninfo);
    // cur << "& /* reference */ ";
    cur << "&";
  }
  else {
    if (ninfo.isTypeSecondPart()) {
      unparseType(ref_type->get_base_type(), ninfo);
    }
    else {
      SgUnparse_Info ninfo2(ninfo);
      ninfo2.set_isTypeFirstPart();
      unparseType(ref_type, ninfo2);
      ninfo2.set_isTypeSecondPart();
      unparseType(ref_type, ninfo2);
    }
  }
}

void 
UnparserFort::unparseNameType(SgType* type, SgUnparse_Info& info)
{
  // DQ (10/7/2004): This should not exist! anything that is a SgNamedType is handled by the unparse 
  // functions for the types that are derived from the SgNamedType (thus this function should not be here)

  printf ("Error: It should be impossible to call this unparseNameType() function (except directly which should not be done!) \n");
  ROSE_ASSERT(false);

  SgNamedType* named_type = isSgNamedType(type);
  ROSE_ASSERT(named_type != NULL);

  if((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart())
    /* do nothing */;
  else {
    // DQ (10/15/2004): Output the qualified name instead of the name (I think) Opps! this function is never called!
    // cur << named_type->get_name().str() << " ";
    printf ("In unparseNameType(): named_type->get_qualified_name() = %s \n",named_type->get_qualified_name().str());
    cur << named_type->get_qualified_name().str() << " "; 
  }
}

void
UnparserFort::unparseClassType(SgType* type, SgUnparse_Info& info)
{
  // printf ("Inside of UnparserFort::unparseClassType \n");

  SgClassType* class_type = isSgClassType(type);
  ROSE_ASSERT(class_type != NULL);

  SgClassDeclaration *cdecl = isSgClassDeclaration(class_type->get_declaration());
  ROSE_ASSERT(cdecl != NULL);

#if 0
  printf ("info.isWithType() = %s \n",(info.isWithType() == true) ? "true" : "false");
  printf ("info.SkipBaseType() = %s \n",(info.SkipBaseType() == true) ? "true" : "false");
  printf ("info.isTypeSecondPart() = %s \n",(info.isTypeSecondPart() == true) ? "true" : "false");
#endif

  if (info.isTypeFirstPart() == true) {
    /* print the class specifiers */
    if (!info.SkipClassSpecifier()) {
	  switch (cdecl->get_class_type())
	    {
	    case SgClassDeclaration::e_class :
	      {
		cur << "class ";
		break;
	      }
	    case SgClassDeclaration::e_struct :
	      {
		cur << "struct ";
		break;
	      }
	    case SgClassDeclaration::e_union :
	      {
		cur << "union ";
		break;
	      }
	    default:
	      {
		printf ("Error: default reached \n");
		ROSE_ASSERT(false);
		break;
	      }
	    }
	}
    }

  // DQ (10/7/2004): We need to output just the name when isTypeFirstPart == false and isTypeSecondPart == false
  // this allows us to handle: "doubleArray* arrayPtr2 = new doubleArray();"
  //                                                         ^^^^^^^^^^^
  if (info.isTypeSecondPart() == false) {
      // DQ (10/14/2004): Or this could be a declaration in a namespace
      SgClassDefinition*              cdefn         = NULL;
      SgNamespaceDefinitionStatement* namespaceDefn = NULL;

      SgClassDeclaration *nondefiningClassDeclaration = isSgClassDeclaration(class_type->get_declaration());
      ROSE_ASSERT(nondefiningClassDeclaration != NULL);

      SgClassDefinition  *nondefiningClassDefinition  = nondefiningClassDeclaration->get_definition();
      ROSE_ASSERT(nondefiningClassDefinition != NULL);

      SgClassDeclaration *definingClassDeclaration    = isSgClassDeclaration(nondefiningClassDefinition->get_declaration());
      ROSE_ASSERT(definingClassDeclaration != NULL);

      SgTemplateInstantiationDecl *instantiatedTemplateClass = isSgTemplateInstantiationDecl(definingClassDeclaration);

      SgDeclarationStatement* declaration = NULL;

      if (instantiatedTemplateClass != NULL)
	{
	  ROSE_ASSERT(instantiatedTemplateClass != NULL);

	  SgTemplateDeclaration *templateDeclaration = instantiatedTemplateClass->get_templateDeclaration();
	  ROSE_ASSERT(templateDeclaration != NULL);
	  declaration = templateDeclaration;
	}
      else
	{
	  declaration = definingClassDeclaration;
	}

      ROSE_ASSERT(declaration != NULL);

#if 0
      // Build reference to any possible enclosing scope represented by a SgClassDefinition or SgNamespaceDefinition
      // to be used check if name qualification is required.
      initializeDeclarationsFromParent ( declaration, cdefn, namespaceDefn );
#endif

      SgType* ptype = NULL;

      if (cdefn != NULL) {
	  ptype = isSgNamedType(cdefn->get_declaration()->get_type());
	}

      // DQ (10/8/2004): I had to make this (ptype != NULL) to make sure that the get_qualified_name()
      // function would not be called for IR nodes where the parent of the class declaration was not 
      // set consistantly.  It seems likely that this might be a bug in the future!
      // DQ (10/8/2004): If the parent nodes are not set then avoid calling 
      // get_qualified_name() (which requires valid parent pointers).

      // DQ 10/10/2004): This version with (ptype != NULL) works in the sense that it allows 
      // the EDG/Sage connection code to avoid calling the get_qualified_name() function, but 
      // sometimes the qualified name is required so it is ultimately note correct.

      // DQ (10/14/2004): If we are going to output the definition (below) then we don't need the qualified name!
      bool definitionWillBeOutput = ( (info.isTypeFirstPart() == true) && !info.SkipClassDefinition() );
      // DQ (10/14/2004): This code take the namespace into account when a qualified name is required!
      ROSE_ASSERT(namespaceDefn == NULL || namespaceDefn->get_namespaceDeclaration() != NULL);
      bool outputQualifiedName = ((ptype != NULL) && (info.get_current_context() != ptype)) ||
	((namespaceDefn != NULL) && (info.get_current_namespace() != namespaceDefn->get_namespaceDeclaration()));
      outputQualifiedName = outputQualifiedName && (definitionWillBeOutput == false);

      if ( outputQualifiedName == false ) {
	  // (10/15/2001): Bugfix
	  // Note that the name of the class is allowed to be empty in the case of: 
	  //      typedef struct <no tag required here> { int x; } y;
	  if (class_type->get_name().str() != NULL) {
	      cur << class_type->get_name().str() << " ";
	    }
	  else {
	      // printf ("class_type->get_name().str() == NULL \n");
	    }
	}
      else {
	  // add qualifier of current types to the name
	  SgName nm = cdecl->get_qualified_name();
	  if (nm.str() != NULL) {
	      cur << nm.str() << " ";
	    }
	}
    }


  if (info.isTypeFirstPart() == true) {
      if ( !info.SkipClassDefinition() ) {
	  SgUnparse_Info ninfo(info);
	  ninfo.unset_SkipSemiColon();

	  ninfo.set_isUnsetAccess();
	  cur << "{";
	  SgClassDefinition* classdefn_stmt = cdecl->get_definition();
	  ROSE_ASSERT(classdefn_stmt != NULL);
	  SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();
	  while (pp != classdefn_stmt->get_members().end()) {
	      unparseStatement((*pp), ninfo);
	      pp++;
	    }
	  cur << "}";
	}
    }
}

void
UnparserFort::unparseEnumType(SgType* type, SgUnparse_Info& info)
{
  SgEnumType* enum_type = isSgEnumType(type);
  ROSE_ASSERT(enum_type);

  // printf ("Inside of unparseEnumType() \n");

  // DQ (10/7/2004): We need to output just the name when isTypeFirstPart == false and isTypeSecondPart == false
  // this allows us to handle: "doubleArray* arrayPtr2 = new doubleArray();"
  //                                                         ^^^^^^^^^^^
  if (info.isTypeSecondPart() == false) {
      SgEnumDeclaration *edecl = isSgEnumDeclaration(enum_type->get_declaration());
      SgClassDefinition *cdefn = NULL;
      SgNamespaceDefinitionStatement* namespaceDefn = NULL;

      ROSE_ASSERT(edecl != NULL);


#if 0
      // Build reference to any possible enclosing scope represented by a SgClassDefinition or SgNamespaceDefinition
      // to be used check if name qualification is required.
      initializeDeclarationsFromParent ( edecl, cdefn, namespaceDefn );
#endif


      // DQ (5/22/2003) Added output of "enum" string
      cur << "enum ";

      // DQ (10/16/2004): Handle name qualification the same as in the unparseClassType function (we could factor common code later!)
      SgNamedType *ptype = NULL;
      if (cdefn != NULL)
	{
	  ptype = isSgNamedType(cdefn->get_declaration()->get_type());
	}

      // DQ (10/14/2004): If we are going to output the definition (below) then we don't need the qualified name!
      bool definitionWillBeOutput = ( (info.isTypeFirstPart() == true) && !info.SkipClassDefinition() );

      // DQ (10/14/2004): This code take the namespace into account when a qualified name is required!
      ROSE_ASSERT(namespaceDefn == NULL || namespaceDefn->get_namespaceDeclaration() != NULL);
      bool outputQualifiedName = ((ptype != NULL) && (info.get_current_context() != ptype)) ||
	((namespaceDefn != NULL) && (info.get_current_namespace() != namespaceDefn->get_namespaceDeclaration()));
      outputQualifiedName = outputQualifiedName && (definitionWillBeOutput == false);

      if ( outputQualifiedName == false ) {
	  // (10/15/2001): Bugfix
	  // Note that the name of the class is allowed to be empty in the case of: 
	  //      typedef struct <no tag required here> { int x; } y;
	  if (enum_type->get_name().str() != NULL) {
	      cur << enum_type->get_name().str() << " ";
	    }
	  else {
	      // printf ("enum_type->get_name().str() == NULL \n");
	    }
	}
      else {
	  // add qualifier of current types to the name
	  SgName nm = edecl->get_qualified_name();
	  if (nm.str() != NULL) {
	      cur << nm.str() << " ";
	    }
	}
    }

  if (info.isTypeFirstPart() == true) {
      SgUnparse_Info ninfo(info);

      // don't skip the semicolon in the output of the statement in the class definition
      ninfo.unset_SkipSemiColon();

      ninfo.set_isUnsetAccess();

      if ( info.SkipEnumDefinition() == false) {
	  SgUnparse_Info ninfo(info);
	  ninfo.set_inEnumDecl();
	  SgInitializer *tmp_init = NULL;
	  SgName tmp_name;

	  SgEnumDeclaration *enum_stmt = isSgEnumDeclaration(enum_type->get_declaration());
	  ROSE_ASSERT(enum_stmt != NULL);

	  SgInitializedNamePtrList::iterator p = enum_stmt->get_enumerators().begin();

	  if (p != enum_stmt->get_enumerators().end()) {
	      cur << "{"; 
	      while (1) {
		  tmp_name=(*p)->get_name();
		  tmp_init=(*p)->get_initializer();
		  cur << tmp_name.str();
		  if(tmp_init) {
		      cur << "=";
		      unparseExpression(tmp_init, ninfo);
		    }
		  p++;
		  if (p != enum_stmt->get_enumerators().end())
		    {
		      cur << ",";
		    }
		  else
		    break; 
		}
	      cur << "}";
	    }
	}
    }
}

void
UnparserFort::unparseTypedefType(SgType* type, SgUnparse_Info& info)
{
  // printf ("Inside of UnparserFort::unparseTypedefType \n");
  // cur << "\n/* Inside of UnparserFort::unparseTypedefType */\n";

  SgTypedefType* typedef_type = isSgTypedefType(type);
  ROSE_ASSERT(typedef_type != NULL);

  if((info.isWithType() && info.SkipBaseType()) || info.isTypeSecondPart()) {
      /* do nothing */;
      // printf ("Inside of UnparserFort::unparseTypedefType (do nothing) \n");
    }
  else
    {
      // could be a scoped typedef type
      // check if currrent type's parent type is the same as the context type */
      // SgNamedType *ptype = NULL;

      SgTypedefDeclaration *tdecl = isSgTypedefDeclaration(typedef_type->get_declaration());
      ROSE_ASSERT (tdecl != NULL);

      // DQ (10/16/2004): Keep this error checking for now!
      ROSE_ASSERT(typedef_type != NULL);
      ROSE_ASSERT(typedef_type->get_declaration() != NULL);
      // DQ (10/17/2004): This assertion force me to set the parents of typedef in the EDG/Sage connection code 
      // since I could not figure out why it was not being set in the post processing which sets parents.
      ROSE_ASSERT(typedef_type->get_declaration()->get_parent() != NULL);

      cur << typedef_type->get_qualified_name().str() << " ";
    }

  // printf ("Leaving UnparserFort::unparseTypedefType \n");
  // cur << "\n/* Leaving UnparserFort::unparseTypedefType */\n";
}

void 
UnparserFort::unparseModifierType(SgType* type, SgUnparse_Info& info) 
{
  SgModifierType* mod_type = isSgModifierType(type);
  ROSE_ASSERT(mod_type != NULL);

  // printf ("Top of UnparserFort::unparseModifierType \n");
  // cur << "\n/* Top of UnparserFort::unparseModifierType */\n";

  // Determine if we have to print the base type first (before printing the modifier).
  // This is true in case of a pointer (e.g., int * a) or a reference (e.g., int & a)
  bool btype_first = false;
  if ( isSgReferenceType(mod_type->get_base_type()) || isSgPointerType(mod_type->get_base_type()) )
    btype_first = true;

  if (info.isTypeFirstPart())
    {
      // Print the base type if this has to come first
      if (btype_first)
	unparseType(mod_type->get_base_type(), info);

      if (mod_type->get_typeModifier().get_constVolatileModifier().isConst())    { cur << "const "; }
      if (mod_type->get_typeModifier().get_constVolatileModifier().isVolatile()) { cur << "volatile "; }


      if (mod_type->get_typeModifier().isRestrict())
	{
	  if ( (string(CXX_COMPILER_NAME) == "g++") || (string(CXX_COMPILER_NAME) == "gcc") )
	    {
	      // GNU uses a string variation on the C99 spelling of the "restrict" keyword
	      cur << "__restrict__ ";
	    }
	  else
	    {
	      cur << "restrict ";
	    }
	}

      // Microsoft extension
      // xxx_unaligned   // Microsoft __unaligned qualifier

      // Support for near and far pointers (a microsoft extension)
      // xxx_near        // near pointer
      // xxx_far         // far pointer

      // Support for UPC
      if (mod_type->get_typeModifier().get_upcModifier().isUPC_Shared())
	{ cur << "/* shared: upc not supported by vendor compiler (ignored) */ "; }
      if (mod_type->get_typeModifier().get_upcModifier().isUPC_Strict())
	{ cur << "/* strict: upc not supported by vendor compiler (ignored) */ "; }
      if (mod_type->get_typeModifier().get_upcModifier().isUPC_Relaxed())
	{ cur << "/* relaxed: upc not supported by vendor compiler (ignored) */ "; }

      // Print the base type unless it has been printed up front
      if (!btype_first)
	unparseType(mod_type->get_base_type(), info);
    }
  else {
      if (info.isTypeSecondPart()) {
	  unparseType(mod_type->get_base_type(), info);
	}
      else {
	  SgUnparse_Info ninfo(info);
	  ninfo.set_isTypeFirstPart();
	  unparseType(mod_type, ninfo);
	  ninfo.set_isTypeSecondPart();
	  unparseType(mod_type, ninfo);
	}
    }

  // printf ("Leaving UnparserFort::unparseModifierType \n");
  // cur << "\n/* Leaving UnparserFort::unparseModifierType */\n";
}

void
UnparserFort::unparseFunctionType(SgType* type, SgUnparse_Info& info)
{
  SgFunctionType* func_type = isSgFunctionType(type);
  ROSE_ASSERT (func_type != NULL);

  SgUnparse_Info ninfo(info);
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
	  cur << "(";
	}
      else {
	  unparseType(func_type->get_return_type(), ninfo);
	}
    }
  else {
      if (ninfo.isTypeSecondPart()) {
	  if (needParen) {
	      cur << ")";
	      info.unset_isReferenceToSomething();
	      info.unset_isPointerToSomething();
	    }
	  // print the arguments
	  SgUnparse_Info ninfo2(info); 
	  ninfo2.unset_SkipBaseType();
	  ninfo2.unset_isTypeSecondPart();
	  ninfo2.unset_isTypeFirstPart();

	  cur << "(";
	  SgTypePtrList::iterator p = func_type->get_arguments().begin();
	  while(p != func_type->get_arguments().end())
	    {
	      // printf ("Output function argument ... \n");
	      unparseType(*p, ninfo2);
	      p++;
	      if (p != func_type->get_arguments().end())
		{ cur << ", "; }
	    }
	  cur << ")";
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
}

void 
UnparserFort::unparseMemberFunctionType(SgType* type, SgUnparse_Info& info)
{
  SgMemberFunctionType* mfunc_type = isSgMemberFunctionType(type);
  ROSE_ASSERT(mfunc_type != NULL);

  SgUnparse_Info ninfo(info);
  int needParen = 0;
  if (ninfo.isReferenceToSomething() || ninfo.isPointerToSomething()) {
    needParen=1;
  }

  // DQ (10/7/2004): Skip output of class definition for return type! C++ standard does not permit 
  // a defining declaration within a return type, function parameter, or sizeof expression. 
  ninfo.set_SkipClassDefinition();

  if (ninfo.isTypeFirstPart()) {
      if (needParen) {
	  ninfo.unset_isReferenceToSomething();
	  ninfo.unset_isPointerToSomething();
	  unparseType(mfunc_type->get_return_type(), ninfo);
	  cur << "(";
	}
      else
	unparseType(mfunc_type->get_return_type(), ninfo);
    }
  else {
      if (ninfo.isTypeSecondPart()) {
	  if (needParen) {
	      cur << ")";
	      info.unset_isReferenceToSomething();
	      info.unset_isPointerToSomething();
	    }
	  // print the arguments
	  SgUnparse_Info ninfo2(info); 
	  ninfo2.unset_SkipBaseType();
	  ninfo2.unset_isTypeFirstPart();
	  ninfo2.unset_isTypeSecondPart();
    
	  cur << "(";
	  SgTypePtrList::iterator p = mfunc_type->get_arguments().begin();
	  while (p != mfunc_type->get_arguments().end()) {
	      printf ("In unparseMemberFunctionType: output the arguments \n");
	      unparseType(*p, ninfo2);
	      p++;
	      if (p != mfunc_type->get_arguments().end()) {
		  cur << ", ";
		}
	    }
	  cur << ")";
	  unparseType(mfunc_type->get_return_type(), info); // catch the 2nd part of the rtype
	}
      else {
	  ninfo.set_isTypeFirstPart();
	  unparseType(mfunc_type, ninfo);
	  ninfo.set_isTypeSecondPart();
	  unparseType(mfunc_type, ninfo);
	}
    }
}


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------


bool
UnparserFort::isCharType(SgType* type) 
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

