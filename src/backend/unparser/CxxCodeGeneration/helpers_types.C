
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

#define DEBUG__get_type_name 0

std::vector<const char*> memberFunctionQualifiers(const SgMemberFunctionType* mfnFype, bool trailingSpace) {
  static const char* TEXT_CONST      = " const";
  static const char* TEXT_VOLATILE   = " volatile";
  static const char* TEXT_LVALUE_REF = " &";
  static const char* TEXT_RVALUE_REF = " &&";
  static const char* TEXT_SPACE      = " ";

  ASSERT_not_null(mfnFype);

  std::vector<const char*> res;
  bool addSpace = false;

  if (mfnFype->isConstFunc()) {
    res.push_back(TEXT_CONST);
    addSpace = trailingSpace;
  }

  if (mfnFype->isVolatileFunc()) {
    res.push_back(TEXT_VOLATILE);
    addSpace = trailingSpace;
  }

  if (mfnFype->isLvalueReferenceFunc()) {
    res.push_back(TEXT_LVALUE_REF);
    addSpace = false;
  }

  if (mfnFype->isRvalueReferenceFunc()) {
    res.push_back(TEXT_RVALUE_REF);
    addSpace = false;
  }

  if (addSpace)
    res.push_back(TEXT_SPACE);

  return res;
}

std::string get_type_name(SgType* t) {
  ASSERT_not_null(t);
#if DEBUG__get_type_name
  printf ("In get_type_name()\n");
  printf ("  t = %p = %s\n", t, t->class_name().c_str());
#endif

  switch (t->variant()) {
    case T_DEFAULT:                 return "int";
    case T_UNKNOWN:                 return "UNKNOWN";
    case T_CHAR:                    return "char";
    case T_SIGNED_CHAR:             return "signed char";
    case T_UNSIGNED_CHAR:           return "unsigned char";
    case T_SHORT:                   return "short";
    case T_SIGNED_SHORT:            return "signed short";
    case T_UNSIGNED_SHORT:          return "unsigned short";
    case T_INT:                     return "int";
    case T_SIGNED_INT:              return "signed int";
    case T_UNSIGNED_INT:            return "unsigned int";
    case T_LONG:                    return "long";
    case T_SIGNED_LONG:             return "signed long";
    case T_UNSIGNED_LONG:           return "unsigned long";
    case T_VOID:                    return "void";
    case T_GLOBAL_VOID:             return "global void";
    case T_WCHAR:                   return "wchar_t";
    case T_AUTO:                    return "auto";
    case T_NULLPTR:                 return "null";
    case T_CHAR16:                  return "char16_t";
    case T_CHAR32:                  return "char32_t";
    case T_FLOAT:                   return "float";
    case T_DOUBLE:                  return "double";
    case T_LONG_LONG:               return "long long";
    case T_SIGNED_LONG_LONG:        return "signed long long";
    case T_UNSIGNED_LONG_LONG:      return "unsigned long long";
    case T_FLOAT80:                 return "__float80";
    case T_FLOAT128:                return "__float128";
    case T_SIGNED_128BIT_INTEGER:   return "__int128";
    case T_UNSIGNED_128BIT_INTEGER: return "unsigned __int128";
    case T_LONG_DOUBLE:             return "long double";
    case T_STRING:                  return "char*";

    case T_BOOL:
      return (SageInterface::is_C99_language() || SageInterface::is_C_language()) ? "_Bool" : "bool";

    case T_MATRIX:
      return "Matrix<" + get_type_name(isSgTypeMatrix(t)->get_base_type()) + ">";

    case T_DECLTYPE:
      return get_type_name(isSgDeclType(t)->get_base_type()) + "<" + isSgDeclType(t)->get_base_expression()->unparseToString() + ">";

    case T_TUPLE: {
      SgTypeTuple *typeTuple = isSgTypeTuple(t);
      SgTypePtrList typeList = typeTuple->get_types();

      SgTypePtrList::iterator typeIterator = typeList.begin();

      std::string typeString = "std::tuple<";
      if (typeList.size() != 0) {
        typeString += get_type_name(*typeIterator);
        ++typeIterator;
      }

      for(; typeIterator != typeList.end(); ++typeIterator) {
        typeString += "," + get_type_name(*typeIterator);
      }

      typeString += ">";

      return typeString;
    }

    case T_COMPLEX:             {
      SgTypeComplex* complexType = isSgTypeComplex(t);
      ASSERT_not_null(complexType);
      std::string returnString = get_type_name(complexType->get_base_type()) + " _Complex";
      return returnString;
    }

    case T_IMAGINARY: {
      SgTypeImaginary* imaginaryType = isSgTypeImaginary(t);
      ASSERT_not_null(imaginaryType);
      std::string returnString = get_type_name(imaginaryType->get_base_type());

      bool usingGcc = false;
#ifdef USE_CMAKE
#  ifdef CMAKE_COMPILER_IS_GNUCC
      usingGcc = true;
#  endif
#else
#  if BACKEND_CXX_IS_GNU_COMPILER
      usingGcc = true;
#  endif
#endif
      if (usingGcc) {
        // Handle special case of GNU compilers
      } else {
        returnString + " _Imaginary";
      }
      return returnString;
    }

    case T_POINTER: {
      SgPointerType* pointer_type = isSgPointerType(t);
      ASSERT_not_null(pointer_type);
      return get_type_name(pointer_type->get_base_type()) + "*";
    }

    case T_REFERENCE: {
      SgReferenceType* ref_type = isSgReferenceType(t);
      ASSERT_not_null(ref_type);
      return get_type_name(ref_type->get_base_type()) + "&";
    }

    case T_MEMBER_POINTER: {
      SgPointerMemberType* mpointer_type = isSgPointerMemberType(t);
      ASSERT_not_null(mpointer_type);

      SgType *btype = mpointer_type->get_base_type();
      SgMemberFunctionType *ftype = isSgMemberFunctionType(btype);
      if (ftype != NULL) {
        std::string res = get_type_name(ftype->get_return_type()) +  "("
                          + get_type_name(mpointer_type->get_class_type())
                          + "::*" + ")" + "(" ;
        SgTypePtrList::iterator p = ftype->get_arguments().begin();
        while ( p != ftype->get_arguments().end() ) {
          res = res + get_type_name(*p);
          p++;
          if (p != ftype->get_arguments().end()) { res = res + ","; }
        }
        res = res + ")";

        for (auto qual : memberFunctionQualifiers(ftype)) res = res + qual;
        return res;
      } else {
        return get_type_name(btype) + "(" + get_type_name(mpointer_type->get_class_type()) + "::*" + ")";
      }
    }

    case T_CLASS: {
      SgClassType* class_type = isSgClassType(t);
      ASSERT_not_null(class_type);
      SgClassDeclaration* decl = isSgClassDeclaration(class_type->get_declaration());
      SgName nm = decl->get_name();
      if (nm.getString() != "")
        return nm.getString();
      else
        return std::string(class_type->get_name().str());
    }

    case T_ENUM: {
      SgEnumType* enum_type = isSgEnumType(t);
      ROSE_ASSERT(enum_type);
      SgEnumDeclaration *edecl = isSgEnumDeclaration(enum_type->get_declaration());

      SgName nm = edecl->get_name();
      if (nm.getString() != "") {
        return nm.getString();
      } else {
        return std::string(enum_type->get_name().str());
      }
    }

    case T_TYPEDEF: {
      SgTypedefType* typedef_type = isSgTypedefType(t);
      ASSERT_not_null(typedef_type);

      SgName nm = typedef_type->get_name();
      return std::string(nm.str());
    }

    case T_MODIFIER: {
      SgModifierType* mod_type = isSgModifierType(t);
      ASSERT_not_null(mod_type);
      std::string res;
      bool unparse_base = true;
      if (mod_type->get_typeModifier().isOpenclGlobal())
        res = "__global " + res;
      if (mod_type->get_typeModifier().isOpenclLocal())
        res = "__local " + res;
      if (mod_type->get_typeModifier().isOpenclConstant())
        res = "__constant " + res;
      if ( isSgReferenceType(mod_type->get_base_type()) ||
           isSgPointerType(mod_type->get_base_type()) ) {
        res = get_type_name(mod_type->get_base_type());
        unparse_base = false;
      }
      if (mod_type->get_typeModifier().haveAddressSpace()) {
        std::ostringstream outstr;
        outstr << mod_type->get_typeModifier().get_address_space_value();
        res = res + "__attribute__((address_space(" + outstr.str() + ")))";
      }

      if (mod_type->get_typeModifier().get_constVolatileModifier().isConst())
        res = res + "const ";
      if (mod_type->get_typeModifier().get_constVolatileModifier().isVolatile())
        res = res + "volatile ";

      if (mod_type->get_typeModifier().isRestrict()) {
        res = res + Unparse_Type::unparseRestrictKeyword();
      }
      if (unparse_base)
        res = res + get_type_name(mod_type->get_base_type());

      return res;
    }

    case T_PARTIAL_FUNCTION:
    case T_FUNCTION: {
      SgFunctionType* func_type = isSgFunctionType(t);
      ASSERT_not_null(func_type);
      std::string res = get_type_name(func_type->get_return_type()) + "(";
      SgTypePtrList::iterator p = func_type->get_arguments().begin();
      while (p != func_type->get_arguments().end()) {
        res = res + get_type_name(*p);
        p++;
        if (p != func_type->get_arguments().end())
          res = res + ",";
      }
      res = res + ")";
      return res;
    }

    case T_MEMBERFUNCTION: {
      SgMemberFunctionType* mfunc_type = isSgMemberFunctionType(t);
      ASSERT_not_null(mfunc_type);

      std::string res = get_type_name(mfunc_type->get_return_type()) + "(";
      SgTypePtrList::iterator p = mfunc_type->get_arguments().begin();
      while (p != mfunc_type->get_arguments().end()) {
        res = res + get_type_name(*p);
        p++;
        if (p != mfunc_type->get_arguments().end())
          res = res + ",";
      }
      res = res + ")";
      for (auto qual : memberFunctionQualifiers(mfunc_type))
        res = res + qual;

      return res;
    }

    case T_ARRAY: {
      SgArrayType* array_type = isSgArrayType(t);
      ASSERT_not_null(array_type);

      std::string res = get_type_name(array_type->get_base_type()) + "[";
      if (array_type->get_index())
        res = res + array_type->get_index()->unparseToString();
      res = res + "]";
      return res;
    }

    case T_ELLIPSE: return "...";

    case T_TEMPLATE: {
      SgTemplateType * tpl_type = isSgTemplateType(t);
      ASSERT_not_null(tpl_type);
      return tpl_type->get_name();
    }

    case T_NONREAL: {
      SgNonrealType * nrtype = isSgNonrealType(t);
      ASSERT_not_null(nrtype);
      return nrtype->get_name();
    }

    case T_RVALUE_REFERENCE: {
      SgRvalueReferenceType* rref_type = isSgRvalueReferenceType(t);
      ASSERT_not_null(rref_type);
      return get_type_name(rref_type->get_base_type()) + "&&";
    }

    default: {
      printf("Error: unparse_type.C get_type_name(): Default case reached in switch: %s\n", t->class_name().c_str());
      ROSE_ABORT();
    }
  }

#if DEBUG__get_type_name
  printf ("Leaving get_type_name()\n");
#endif
}
