#include "sage3.h"
#include <sstream>
#include <iostream>
#include "sageInterface.h"

using namespace std;


// originally from src/midend/astInlining/typeTraits.C
// src/midend/astUtil/astInterface/AstInterface.C
namespace SageInterface
{

bool isScalarType( SgType* t)
{
  ROSE_ASSERT(t);
  switch(t->variantT()) {
  case V_SgTypeChar :
  case V_SgTypeSignedChar :
  case V_SgTypeUnsignedChar :

  case V_SgTypeShort :
  case V_SgTypeSignedShort :
  case V_SgTypeUnsignedShort :

  case V_SgTypeInt :
  case V_SgTypeSignedInt :
  case V_SgTypeUnsignedInt :

  case V_SgTypeLong :
  case V_SgTypeSignedLong :
  case V_SgTypeUnsignedLong :

  case V_SgTypeVoid :
  case V_SgTypeWchar:
  case V_SgTypeFloat:
  case V_SgTypeDouble:

  case V_SgTypeLongLong:
  case V_SgTypeUnsignedLongLong:
  case V_SgTypeLongDouble:

  case V_SgTypeString:
  case V_SgTypeBool:
// DQ (8/27/2006): change name of SgComplex to SgTypeComplex (for consistency) and added SgTypeImaginary (C99 type).
  case V_SgTypeComplex:
  case V_SgTypeImaginary:
     return true;
  default: {}
  }
  return false;
}


  SgType* getBaseFromType(SgType* t) {

  SgType* deref = t->dereference();
  if (deref && deref != t)
    return getBaseFromType(deref);
  return t;
}

// Is this a const type?
bool isConstType(SgType* t) {
//  if (isSgReferenceType(t)) // reference type cannot be cv-qualified usually, 
//    return isConstType(isSgReferenceType(t)->get_base_type());
  if (isSgTypedefType(t))
    return isConstType(isSgTypedefType(t)->get_base_type());
  if (isSgModifierType(t)) {
    SgTypeModifier& modifier = isSgModifierType(t)->get_typeModifier();
    SgConstVolatileModifier& cv = modifier.get_constVolatileModifier();
    if (cv.isConst())
      return true;
    else
      return isConstType(isSgModifierType(t)->get_base_type());
  }
  return false;
}

// Is this a volatile type?
bool isVolatileType(SgType* t) {
  if (isSgTypedefType(t))
    return isVolatileType(isSgTypedefType(t)->get_base_type());
  if (isSgModifierType(t)) {
    SgTypeModifier& modifier = isSgModifierType(t)->get_typeModifier();
    SgConstVolatileModifier& cv = modifier.get_constVolatileModifier();
    if (cv.isVolatile())
      return true;
    else
      return isVolatileType(isSgModifierType(t)->get_base_type());
  }
  return false;
}

// Is this a restrict type?
bool isRestrictType(SgType* t) {
  if (isSgTypedefType(t))
    return isRestrictType(isSgTypedefType(t)->get_base_type());
  if (isSgModifierType(t)) {
    SgTypeModifier& modifier = isSgModifierType(t)->get_typeModifier();
    if (modifier.isRestrict())
      return true;
    else
      return isRestrictType(isSgModifierType(t)->get_base_type());
  }
  return false;
}


// Is this type a non-constant reference type?
bool isNonconstReference(SgType* t) {
  if (isSgReferenceType(t))
    return !isConstType(isSgReferenceType(t)->get_base_type());
  if (isSgTypedefType(t))
    return isNonconstReference(isSgTypedefType(t)->get_base_type());
  if (isSgModifierType(t)) {
    SgTypeModifier& modifier = isSgModifierType(t)->get_typeModifier();
    SgConstVolatileModifier& cv = modifier.get_constVolatileModifier();
    if (cv.isConst())
      return false;
    else
      return isNonconstReference(isSgModifierType(t)->get_base_type());
  }
  return false;
}

// Is this type a constant or non-constant reference type?
bool isReferenceType(SgType* t) {
  if (isSgReferenceType(t))
    return true;
  if (isSgTypedefType(t))
    return isReferenceType(isSgTypedefType(t)->get_base_type());
  if (isSgModifierType(t)) {
    return isReferenceType(isSgModifierType(t)->get_base_type());
  }
  return false;
}

  //! Calculate the number of elements of an array type
  
  size_t getArrayElementCount(SgArrayType* t)
  {
    ROSE_ASSERT(t);
    size_t result=1; 
    SgExpression * indexExp =  t->get_index();

    //strip off THREADS for UPC array with a dimension like dim*THREADS
    if (isUpcArrayWithThreads(t))
     {
       SgMultiplyOp* multiply = isSgMultiplyOp(indexExp);
       ROSE_ASSERT(multiply);
       indexExp = multiply->get_lhs_operand();
     }

    // assume dimension default to 1 if not specified ,such as a[] 
    if (indexExp == NULL) 
      result = 1;
    else 
    { 
      //Take advantage of the fact that the value expression is always SgUnsignedLongVal in AST
      SgUnsignedLongVal * valExp = isSgUnsignedLongVal(indexExp);
      ROSE_ASSERT(valExp); // TODO: return -1 is better ?
      result = valExp->get_value(); 
    }

    // consider multi dimensional case 
    SgArrayType* arraybase = isSgArrayType(t->get_base_type());
    if (arraybase)
      result = result * getArrayElementCount(arraybase);

    return result;
  } // getArrayElementCount()

// Returns true if args is a valid argument type list for decl
// Requires the exact argument list given -- conversions not handled
// Good enough for default and copy constructors, but not other functions
static bool acceptsArguments(SgFunctionDeclaration* decl, 
			     std::list<SgType*> args) {
  SgInitializedNamePtrList& params = decl->get_args();
  SgInitializedNamePtrList::iterator p = params.begin();
  std::list<SgType*>::iterator a = args.begin();
  for (; p != params.end() && a != args.end(); ++p, ++a) {
    if (getBaseFromType((*p)->get_type()) != getBaseFromType(*a))
      return false; // Type mismatch
      // Good enough for copy constructors, though
  }
  if (p == params.end() && a != args.end())
    return false; // Too many arguments
  for (; p != params.end(); ++p) {
    if ((*p)->get_initializer() == 0)
      return false; // Non-defaulted parameter after end of arg list
  }
  return true;
}

// Is a type default constructible?  This may not quite work properly.
bool isDefaultConstructible(SgType* type) {
  switch (type->variantT()) {
    case V_SgArrayType:
    return isDefaultConstructible(isSgArrayType(type)->get_base_type());
    break;

    case V_SgModifierType:
    return isDefaultConstructible(isSgModifierType(type)->get_base_type()); 
    break;

    case V_SgClassType: {
      SgClassDeclaration* decl = 
	isSgClassDeclaration(isSgClassType(type)->get_declaration());
      assert (decl);
      SgClassDefinition* defn = decl->get_definition();
      assert (defn);
      bool hasDefaultConstructor = false, hasAnyConstructor = false;
      // Look for user-defined constructors
      SgDeclarationStatementPtrList& classMembers = defn->get_members();
      for (SgDeclarationStatementPtrList::iterator i = classMembers.begin();
	   i != classMembers.end(); ++i) {
	if (isSgFunctionDeclaration(*i)) {
	  SgFunctionDeclaration* fundecl = isSgFunctionDeclaration(*i);
	  bool thisIsAConstructor = 
	    fundecl->get_specialFunctionModifier().isConstructor();
	  if (thisIsAConstructor)
	    hasAnyConstructor = true;
	  bool allArgsAreDefaulted = 
	    acceptsArguments(fundecl, std::list<SgType*>());
	  if (thisIsAConstructor && allArgsAreDefaulted &&
	      (*i)->get_declarationModifier().get_accessModifier().isPublic())
	    hasDefaultConstructor = true;
	}
      }
      if (hasDefaultConstructor) return true;
      if (hasAnyConstructor) return false;
      // Return true if all non-static data members are default constructible
      for (SgDeclarationStatementPtrList::iterator i = classMembers.begin();
	   i != classMembers.end(); ++i) {
	if (isSgVariableDeclaration(*i)) {
	  SgVariableDeclaration* decl = isSgVariableDeclaration(*i);
	  if (decl->get_declarationModifier().get_storageModifier().isStatic())
	    continue;
	  SgInitializedNamePtrList& vars = decl->get_variables();
	  for (SgInitializedNamePtrList::iterator j = vars.begin();
	       j != vars.end(); ++j) {
	    if (isDefaultConstructible((*j)->get_type()))
	      return true;
	  }
	}
      }
      return false;
    }
    break;

    case V_SgTypedefType:
    return isDefaultConstructible(isSgTypedefType(type)->get_base_type());
    break;

    case V_SgReferenceType:
    return false;
    break;

 // DQ (8/27/2006): Changed name of SgComplex to make it more consistant with other 
 // type names and added SgTypeImaginary IR node (for C99 complex support).
    case V_SgTypeComplex:
    case V_SgTypeImaginary:
    case V_SgEnumType:
    case V_SgPointerType:
    case V_SgPointerMemberType:
    case V_SgTypeBool:
    case V_SgTypeChar:
    case V_SgTypeDefault:
    case V_SgTypeDouble:
    case V_SgTypeFloat:
    case V_SgTypeGlobalVoid:
    case V_SgTypeInt:
    case V_SgTypeLong:
    case V_SgTypeLongDouble:
    case V_SgTypeLongLong:
    case V_SgTypeShort:
    case V_SgTypeSignedChar:
    case V_SgTypeSignedInt:
    case V_SgTypeSignedLong:
    case V_SgTypeSignedShort:
    case V_SgTypeString:
    case V_SgTypeUnsignedChar:
    case V_SgTypeUnsignedInt:
    case V_SgTypeUnsignedLong:
    case V_SgTypeUnsignedLongLong:
    case V_SgTypeUnsignedShort:
    case V_SgTypeVoid:
    case V_SgTypeWchar:
    return true;
    break;

    default:
    assert (!"Unknown type in isDefaultConstructible()");
    return true;
  }
}

// Is a type copy constructible?  This may not quite work properly.
bool isCopyConstructible(SgType* type) {
  switch (type->variantT()) {
    case V_SgArrayType:
    return isCopyConstructible(isSgArrayType(type)->get_base_type());
    break;

    case V_SgModifierType:
    return isCopyConstructible(isSgModifierType(type)->get_base_type()); 
    break;

    case V_SgFunctionType:
    case V_SgMemberFunctionType:
    return false;
    break;

    case V_SgClassType: {
      SgClassDeclaration* decl = 
	isSgClassDeclaration(isSgClassType(type)->get_declaration());
      assert (decl);
      SgClassDefinition* defn = decl->get_definition();
      assert (defn);
      bool hasPublicCopyConstructor = false, hasAnyCopyConstructor = false;
      // Look for user-defined constructors
      SgDeclarationStatementPtrList& classMembers = defn->get_members();
      for (SgDeclarationStatementPtrList::iterator i = classMembers.begin();
	   i != classMembers.end(); ++i) {
	if (isSgFunctionDeclaration(*i)) {
	  SgFunctionDeclaration* fundecl = isSgFunctionDeclaration(*i);
	  bool thisIsAConstructor = 
	    fundecl->get_specialFunctionModifier().isConstructor();
	  std::list<SgType*> args;
	  args.push_back(new SgReferenceType(type));
	  bool isCopyConstructor = acceptsArguments(fundecl, args);
	  if (thisIsAConstructor && isCopyConstructor) {
	    hasAnyCopyConstructor = true;
	    if ((*i)->get_declarationModifier().
			get_accessModifier().isPublic())
	      hasPublicCopyConstructor = true;
	  }
	}
      }
      if (hasPublicCopyConstructor) return true;
      if (hasAnyCopyConstructor) return false;
      // Return true if all non-static data members are copy constructible
      for (SgDeclarationStatementPtrList::iterator i = classMembers.begin();
	   i != classMembers.end(); ++i) {
	if (isSgVariableDeclaration(*i)) {
	  SgVariableDeclaration* decl = isSgVariableDeclaration(*i);
	  if (decl->get_declarationModifier().get_storageModifier().isStatic())
	    continue;
	  SgInitializedNamePtrList& vars = decl->get_variables();
	  for (SgInitializedNamePtrList::iterator j = vars.begin();
	       j != vars.end(); ++j) {
	    if (isCopyConstructible((*j)->get_type()))
	      return true;
	  }
	}
      }
      return false;
    }
    break;

    case V_SgTypedefType:
    return isCopyConstructible(isSgTypedefType(type)->get_base_type());
    break;

    case V_SgReferenceType:
 // DQ (8/27/2006): Changed name of SgComplex to make it more consistant with other 
 // type names and added SgTypeImaginary IR node (for C99 complex support).
    case V_SgTypeComplex:
    case V_SgTypeImaginary:
    case V_SgEnumType:
    case V_SgPointerType:
    case V_SgPointerMemberType:
    case V_SgTypeBool:
    case V_SgTypeChar:
    case V_SgTypeDefault:
    case V_SgTypeDouble:
    case V_SgTypeFloat:
    case V_SgTypeGlobalVoid:
    case V_SgTypeInt:
    case V_SgTypeLong:
    case V_SgTypeLongDouble:
    case V_SgTypeLongLong:
    case V_SgTypeShort:
    case V_SgTypeSignedChar:
    case V_SgTypeSignedInt:
    case V_SgTypeSignedLong:
    case V_SgTypeSignedShort:
    case V_SgTypeString:
    case V_SgTypeUnsignedChar:
    case V_SgTypeUnsignedInt:
    case V_SgTypeUnsignedLong:
    case V_SgTypeUnsignedLongLong:
    case V_SgTypeUnsignedShort:
    case V_SgTypeVoid:
    case V_SgTypeWchar:
    return true;
    break;

    default:
    std::cerr << "In isCopyConstructible(), type is a " << type->class_name() << std::endl;
    assert (!"Unknown type in isCopyConstructible()");
  }
  return true;
}

// Does a type have a trivial (built-in) destructor?
bool hasTrivialDestructor(SgType* t) {
  switch (t->variantT()) {
 // DQ (8/27/2006): Changed name of SgComplex to make it more consistant with other 
 // type names and added SgTypeImaginary IR node (for C99 complex support).
    case V_SgTypeComplex:
    case V_SgTypeImaginary:
    case V_SgPointerType:
    case V_SgReferenceType:
    case V_SgTypeBool:
    case V_SgTypeChar:
    case V_SgTypeDouble:
    case V_SgTypeEllipse:
    case V_SgTypeFloat:
    case V_SgTypeGlobalVoid:
    case V_SgTypeInt:
    case V_SgTypeLong:
    case V_SgTypeLongDouble:
    case V_SgTypeLongLong:
    case V_SgTypeShort:
    case V_SgTypeSignedChar:
    case V_SgTypeSignedInt:
    case V_SgTypeSignedLong:
    case V_SgTypeSignedShort:
    case V_SgTypeUnsignedChar:
    case V_SgTypeUnsignedInt:
    case V_SgTypeUnsignedLong:
    case V_SgTypeUnsignedLongLong:
    case V_SgTypeUnsignedShort:
    case V_SgTypeVoid:
    case V_SgTypeWchar:
    case V_SgFunctionType:
    case V_SgTypeString:
    case V_SgEnumType:
      return true;

    case V_SgArrayType:
      return hasTrivialDestructor(isSgArrayType(t)->get_base_type());

    case V_SgModifierType: 
      return hasTrivialDestructor(isSgModifierType(t)->get_base_type());

    case V_SgTypedefType:
      return hasTrivialDestructor(isSgTypedefType(t)->get_base_type());

    default: 
      SgClassType* ct = isSgClassType(t);
      if (!ct) return false;
      SgDeclarationStatement* decl = ct->get_declaration();
      if (isSgClassDeclaration(decl)) {
	SgClassDeclaration* cd = isSgClassDeclaration(decl);
	SgClassDefinition* cdef = cd->get_definition();
	if (!cdef) return false;
	bool hasDestructor = false;
	SgDeclarationStatementPtrList& decls = cdef->get_members();
	for (SgDeclarationStatementPtrList::iterator i = decls.begin();
	     i != decls.end(); ++i) {
	  if (isSgFunctionDeclaration(*i)) {
	    SgFunctionDeclaration* fundecl = isSgFunctionDeclaration(*i);
	    if (fundecl->get_specialFunctionModifier().isDestructor())
	      hasDestructor = true;
	  }
	}
	if (hasDestructor)
	  return false;
	else
	  return true;
      } else if (isSgFunctionDeclaration(decl)) {
	return true;
      } else if (isSgEnumDeclaration(decl)) {
	return true;
      } else {
	return false;
      }
  }
}

  //! Is UPC shared type?
  bool isUpcSharedType(SgType* sg_type,SgModifierType ** mod_type_out/*=NULL*/)
  {
    ROSE_ASSERT(sg_type);
    bool result = false;

    if (isSgModifierType(sg_type))
    {
      SgModifierType * mod_type = isSgModifierType(sg_type);
      if (mod_type->get_typeModifier().get_upcModifier().get_isShared())
      {  
        if (mod_type_out) *mod_type_out = mod_type;
        result = true;
      }  
      else 
        result = isUpcSharedType( (isSgModifierType(sg_type))->get_base_type(), mod_type_out);
    }  
    else if (isSgPointerType(sg_type))
      result = isUpcSharedType( (isSgPointerType(sg_type))->get_base_type(),mod_type_out );
    else if (isSgReferenceType(sg_type))
      result = isUpcSharedType( (isSgReferenceType(sg_type))->get_base_type(),mod_type_out );
    else if (isSgArrayType(sg_type))
      result = isUpcSharedType( (isSgArrayType(sg_type))->get_base_type(),mod_type_out );
    else if (isSgTypedefType(sg_type))
      result = isUpcSharedType( (isSgTypedefType(sg_type))->get_base_type(), mod_type_out);

    if ((result == false) &&(mod_type_out))  *mod_type_out = NULL;
    return result;
  } //isUpcSharedType

  //! Is UPC private-to-shared type? Judge the order of SgPointer and SgUPC_AccessModifier
  bool isUpcPrivateToSharedType(SgType* t)
  {
    bool check = isUpcSharedType(t);
    ROSE_ASSERT(check);
    bool pointerFirst = false;
    SgType* currentType = t;

    while (true) 
    {
       if (isSgModifierType(currentType))
       {
          SgModifierType * mod_type = isSgModifierType(currentType);
         if (mod_type->get_typeModifier().get_upcModifier().get_isShared())
           break; //Reaches SgUPC_AccessModifier,exit the while loop
          else 
            currentType = isSgModifierType(currentType)->get_base_type();
       } // if SgModifierType
       else if (isSgPointerType(currentType))
       {
         pointerFirst = true; // reach pointer before reaching UPC modifier!
         currentType = isSgPointerType(currentType)->get_base_type();
       } // if Pointer
      else if (isSgReferenceType(currentType))
         currentType = isSgReferenceType(currentType)->get_base_type();
      else if (isSgArrayType(currentType))
         currentType =  isSgArrayType(currentType)->get_base_type();
      else if (isSgTypedefType(currentType))
          currentType =  isSgTypedefType(currentType)->get_base_type();
    } // while

   return pointerFirst;
  }// isUpcPrivateToSharedType()

  //! Phase-less means block size ==1, or 0, or unspecified(-1). 
  bool isUpcPhaseLessSharedType (SgType* t)
  {
    SgModifierType * mod_type_out;
    int block_size;

    bool check = isUpcSharedType(t,&mod_type_out);
    //ROSE_ASSERT(isUpcSharedType(t,mod)); // avoid side effect for assertion!!
    ROSE_ASSERT(check&&mod_type_out); 

    block_size =  mod_type_out->get_typeModifier().get_upcModifier().get_layout();
   // cout<<"block size is "<<block_size<<endl;
    if ((block_size==1)||(block_size == -1)|| (block_size == 0))
      return true;
    else 
      return false;
  } // isUpcPhaseLessSharedType

  //! Is an UPC array with dimension of X*THREADS
  /*!
   * EDG-SAGE connection ensures that UPC array using THREADS dimension 
   *  has an index expression of type SgMultiplyOp
   * and operands (X and SgUpcThreads)
   * TODO multi dimensional arrays
   */
  bool isUpcArrayWithThreads(SgArrayType* t)
  {
    ROSE_ASSERT(t!=NULL);
    bool result = false;
    SgExpression * exp = t->get_index();
    SgMultiplyOp* multiply = isSgMultiplyOp(exp);
    if (multiply)
      if (isSgUpcThreads(multiply->get_rhs_operand()))
        result = true;
    return result;  
  }

  SgType* getFirstVarType(SgVariableDeclaration* decl)
  {
    ROSE_ASSERT(decl);
    SgVariableSymbol * sym = getFirstVarSym(decl);
    ROSE_ASSERT(sym);
    return sym->get_type();
  }
} //end of namespace 

