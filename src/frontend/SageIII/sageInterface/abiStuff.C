#include "rose.h"
#include <vector>

using namespace std;
using namespace SageBuilder;

StructLayoutInfo ChainableTypeLayoutGenerator::layoutType(SgType* t) const {
  // Default implementation just passes everything (except typedefs and
  // modifiers) on to next
  switch (t->variantT()) {
    case V_SgTypedefType: {
      SgType* t2 = isSgTypedefType(t)->get_base_type();
      return this->beginning->layoutType(t2);
    }
    case V_SgModifierType: {
      SgType* t2 = isSgModifierType(t)->get_base_type();
      return this->beginning->layoutType(t2);
    }
    default: {
      if (this->next) {
        return this->next->layoutType(t);
      } else {
        std::cerr << "Reached end of chain when trying to lay out a " << t->class_name() << std::endl;
        abort();
      }
    }
  }
}

void NonpackedTypeLayoutGenerator::layoutOneField(SgType* fieldType, SgNode* decl, bool isUnion, size_t& currentOffset, StructLayoutInfo& layout) const {
  StructLayoutInfo fieldInfo = this->beginning->layoutType(fieldType);
  if (fieldInfo.alignment > layout.alignment) {
    layout.alignment = fieldInfo.alignment;
  }
  if (currentOffset % fieldInfo.alignment != 0) {
    size_t paddingNeeded = fieldInfo.alignment - (currentOffset % fieldInfo.alignment);
    if (!isUnion) {
      layout.fields.push_back(StructLayoutEntry(NULL, currentOffset, paddingNeeded));
    }
    currentOffset += paddingNeeded;
  }
  layout.fields.push_back(StructLayoutEntry(decl, currentOffset, fieldInfo.size));
  currentOffset += fieldInfo.size; // We need to do this even for unions to set the object size
  if (currentOffset > layout.size) {
    layout.size = currentOffset;
  }
  if (isUnion) {
    ROSE_ASSERT (currentOffset == fieldInfo.size);
    currentOffset = 0;
  }
}

StructLayoutInfo NonpackedTypeLayoutGenerator::layoutType(SgType* t) const {
  switch (t->variantT()) {
    case V_SgClassType: { // Also covers structs and unions
      SgClassDeclaration* decl = isSgClassDeclaration(isSgClassType(t)->get_declaration());
      ROSE_ASSERT (decl);
      decl = isSgClassDeclaration(decl->get_definingDeclaration());
      ROSE_ASSERT (decl);
      SgClassDefinition* def = decl->get_definition();
      ROSE_ASSERT (def);
      const SgDeclarationStatementPtrList& body = def->get_members();
      bool isUnion = (decl->get_class_type() == SgClassDeclaration::e_union);
      StructLayoutInfo layout;
      size_t currentOffset = 0;
      for (SgDeclarationStatementPtrList::const_iterator i = body.begin();
           i != body.end(); ++i) {
        SgDeclarationStatement* mem = *i;
        SgVariableDeclaration* vardecl = isSgVariableDeclaration(mem);
        SgClassDeclaration* classdecl = isSgClassDeclaration(mem);
        bool isUnnamedUnion = classdecl ? classdecl->get_isUnNamed() : false;
        if (vardecl) {
          if (!vardecl->get_declarationModifier().isDefault()) continue; // Static fields and friends
          ROSE_ASSERT (!vardecl->get_bitfield());
          const SgInitializedNamePtrList& vars = isSgVariableDeclaration(mem)->get_variables();
          for (SgInitializedNamePtrList::const_iterator j = vars.begin();
               j != vars.end(); ++j) {
            SgInitializedName* var = *j;
            layoutOneField(var->get_type(), var, isUnion, currentOffset, layout);
          }
        } else if (isUnnamedUnion) {
          layoutOneField(classdecl->get_type(), classdecl, isUnion, currentOffset, layout);
        } // else continue;
      }
      if (layout.size % layout.alignment != 0) {
        size_t paddingNeeded = layout.alignment - (layout.size % layout.alignment);
        if (!isUnion) {
          layout.fields.push_back(StructLayoutEntry(NULL, layout.size, paddingNeeded));
        }
        layout.size += paddingNeeded;
      }
      return layout;
    }
    case V_SgArrayType: {
      StructLayoutInfo layout = beginning->layoutType(isSgArrayType(t)->get_base_type());
      layout.fields.clear();
      SgExpression* numElements = isSgArrayType(t)->get_index();
      if (!isSgValueExp(numElements)) {
        cerr << "Error: trying to compute static size of an array with non-constant size" << endl;
        abort();
      }
      layout.size *= SageInterface::getIntegerConstantValue(isSgValueExp(numElements));
      return layout;
    }
    default: return ChainableTypeLayoutGenerator::layoutType(t);
  }
}

//! There is a nice summary for ABI from
// http://developers.sun.com/solaris/articles/about_amd64_abi.html
// Liao, 7/22/2008
StructLayoutInfo I386PrimitiveTypeLayoutGenerator::layoutType(SgType* t) const {
  StructLayoutInfo layout;
  switch (t->variantT()) {
    case V_SgTypeBool: {layout.size = 1; layout.alignment = 1; break;}

    case V_SgTypeChar: {layout.size = 1; layout.alignment = 1; break;}
    case V_SgTypeSignedChar: {layout.size = 1; layout.alignment = 1; break;}
    case V_SgTypeUnsignedChar: {layout.size = 1; layout.alignment = 1; break;}

    case V_SgTypeShort: {layout.size = 2; layout.alignment = 2; break;}
    case V_SgTypeSignedShort: {layout.size = 2; layout.alignment = 2; break;}
    case V_SgTypeUnsignedShort: {layout.size = 2; layout.alignment = 2; break;}

    case V_SgTypeInt: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgEnumType: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgTypeSignedInt: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgTypeUnsignedInt: {layout.size = 4; layout.alignment = 4; break;}

    case V_SgTypeLong: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgTypeSignedLong: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgTypeUnsignedLong: {layout.size = 4; layout.alignment = 4; break;}

    case V_SgTypeLongLong: {layout.size = 8; layout.alignment = 4; break;}
    // No this node in ROSE yet
    //case V_SgTypeSignedLongLong: {layout.size = 8; layout.alignment = 4; break;}
    case V_SgTypeUnsignedLongLong: {layout.size = 8; layout.alignment = 4; break;}

    case V_SgTypeFloat: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgTypeDouble: {layout.size = 8; layout.alignment = 4; break;}
    case V_SgTypeLongDouble: {layout.size = 12; layout.alignment = 4; break;}

    case V_SgPointerType: {layout.size = 4; layout.alignment = 4; break;} 
    case V_SgReferenceType: {layout.size = 4; layout.alignment = 4; break;}
#if 1    
    //TODO what is the specification for them?
    //case V_SgTypeVoid: {layout.size = 1; layout.alignment = 1; break;}
    //case V_SgTypeWchar: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgTypeComplex: 
    //"Each complex type has the same representation and alignment requirements as 
    //an array type containing exactly two elements of the corresponding real type"
    {
      SgTypeComplex::floating_point_precision_enum precision = 
                       isSgTypeComplex(t)->get_precision();
      if (precision == SgTypeComplex::e_floatPrecision)
      { 
        layout.size = 2*I386PrimitiveTypeLayoutGenerator::layoutType(buildFloatType()).size; 
       layout.alignment = 
             I386PrimitiveTypeLayoutGenerator::layoutType(buildFloatType()).alignment;
      }
      else if (precision == SgTypeComplex::e_doublePrecision)
      {
        layout.size = 2*I386PrimitiveTypeLayoutGenerator::layoutType(buildDoubleType()).size; 
        layout.alignment =I386PrimitiveTypeLayoutGenerator::layoutType(buildDoubleType()).alignment;
      }
      else if (precision ==SgTypeComplex::e_longDoublePrecision)
      {
        layout.size = 2* I386PrimitiveTypeLayoutGenerator::layoutType(buildLongDoubleType()).size; 
        layout.alignment =I386PrimitiveTypeLayoutGenerator::layoutType(buildLongDoubleType()).alignment;
      }
      else 
      {
        cerr<<"Unrecognized Complex type, must be one of float, double, and long double Complex"<<endl;
        ROSE_ASSERT(false);  
      }
      break;
    }
    case V_SgTypeImaginary: 
    {
      SgTypeImaginary::floating_point_precision_enum precision = 
                       isSgTypeImaginary(t)->get_precision();
      if (precision == SgTypeImaginary::e_floatPrecision)
      {
        layout.size = I386PrimitiveTypeLayoutGenerator::layoutType(buildFloatType()).size; 
        layout.alignment = I386PrimitiveTypeLayoutGenerator::layoutType(buildFloatType()).alignment;
      }
      else if (precision == SgTypeImaginary::e_doublePrecision)
      {
        layout.size = I386PrimitiveTypeLayoutGenerator::layoutType(buildDoubleType()).size; 
        layout.alignment = I386PrimitiveTypeLayoutGenerator::layoutType(buildDoubleType()).alignment;
      }
      else if (precision ==SgTypeImaginary::e_longDoublePrecision)
      {
        layout.size = I386PrimitiveTypeLayoutGenerator::layoutType(buildLongDoubleType()).size; 
        layout.alignment = I386PrimitiveTypeLayoutGenerator::layoutType(buildLongDoubleType()).alignment;
      }
      else 
      {
        cerr<<"Unrecognized Imaginary type, must be one of float, double, and long double _Imaginary"<<endl;
        ROSE_ASSERT(false);  
      }
      break;
    }

#endif
    default: return ChainableTypeLayoutGenerator::layoutType(t);
  }
  return layout;
}

StructLayoutInfo I386_VSPrimitiveTypeLayoutGenerator::layoutType(SgType* t) const {
  StructLayoutInfo layout;
  switch (t->variantT()) {
    case V_SgTypeDouble: {layout.size = 8; layout.alignment = 8; break;}
    case V_SgTypeLongDouble: {layout.size = 8; layout.alignment = 8; break;}
    case V_SgTypeComplex: 
    //"Each complex type has the same representation and alignment requirements as 
    //an array type containing exactly two elements of the corresponding real type"
    {
      SgTypeComplex::floating_point_precision_enum precision = 
                       isSgTypeComplex(t)->get_precision();
      if (precision == SgTypeComplex::e_floatPrecision)
      {  
        layout.size = 2*4;
       layout.alignment = 4;
      }
      else if (precision == SgTypeComplex::e_doublePrecision)
      {
        layout.size = 2*8;
        layout.alignment =8;
      }
      else if (precision ==SgTypeComplex::e_longDoublePrecision)
      {
        layout.size = 2*8 ;
        layout.alignment =8;
      }
      else 
      {
        cerr<<"Unrecognized Complex type, must be one of float, double, and long double Complex"<<endl;
        ROSE_ASSERT(false);  
      }
      break;
    }
    case V_SgTypeImaginary: 
    {
      SgTypeImaginary::floating_point_precision_enum precision = 
                       isSgTypeImaginary(t)->get_precision();
      if (precision == SgTypeImaginary::e_floatPrecision)
      {
        layout.size = 4;
        layout.alignment =4 ;
      }
      else if (precision == SgTypeImaginary::e_doublePrecision)
      {
        layout.size = 8;
        layout.alignment = 8;
      }
      else if (precision ==SgTypeImaginary::e_longDoublePrecision)
      {
        layout.size = 8;
        layout.alignment = 8;
      }
      else 
      {
        cerr<<"Unrecognized Imaginary type, must be one of float, double, and long double _Imaginary"<<endl;
        ROSE_ASSERT(false);  
      }
      break;
    }
    default: return I386PrimitiveTypeLayoutGenerator::layoutType(t);
  }
  return layout;
}

StructLayoutInfo X86_64PrimitiveTypeLayoutGenerator::layoutType(SgType* t) const {
  StructLayoutInfo layout;
  switch (t->variantT()) {
    case V_SgTypeBool: {layout.size = 1; layout.alignment = 1; break;}

    case V_SgTypeChar: {layout.size = 1; layout.alignment = 1; break;}
    case V_SgTypeSignedChar: {layout.size = 1; layout.alignment = 1; break;}
    case V_SgTypeUnsignedChar: {layout.size = 1; layout.alignment = 1; break;}

    case V_SgTypeShort: {layout.size = 2; layout.alignment = 2; break;}
    case V_SgTypeSignedShort: {layout.size = 2; layout.alignment = 2; break;}
    case V_SgTypeUnsignedShort: {layout.size = 2; layout.alignment = 2; break;}

    case V_SgTypeInt: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgEnumType: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgTypeSignedInt: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgTypeUnsignedInt: {layout.size = 4; layout.alignment = 4; break;}

    case V_SgTypeLong: {layout.size = 8; layout.alignment = 8; break;}
    case V_SgTypeSignedLong: {layout.size = 8; layout.alignment = 8; break;}
    case V_SgTypeUnsignedLong: {layout.size = 8; layout.alignment = 8; break;}

    case V_SgTypeLongLong: {layout.size = 8; layout.alignment = 8; break;}
    //case V_SgTypeSignedLongLong: {layout.size = 8; layout.alignment = 8; break;}
    case V_SgTypeUnsignedLongLong: {layout.size = 8; layout.alignment = 8; break;}

    case V_SgTypeFloat: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgTypeDouble: {layout.size = 8; layout.alignment = 8; break;}
    case V_SgTypeLongDouble: {layout.size = 16; layout.alignment = 16; break;}

    case V_SgPointerType: {layout.size = 8; layout.alignment = 8; break;}
    case V_SgReferenceType: {layout.size = 8; layout.alignment = 8; break;}
#if 1    
    //case V_SgTypeVoid: {layout.size = 1; layout.alignment = 1; break;}
    //case V_SgTypeWchar: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgTypeComplex: 
    //"Each complex type has the same representation and alignment requirements as 
    //an array type containing exactly two elements of the corresponding real type"
    {
      SgTypeComplex::floating_point_precision_enum precision = 
                       isSgTypeComplex(t)->get_precision();
      if (precision == SgTypeComplex::e_floatPrecision)
      { 
        layout.size = 2*X86_64PrimitiveTypeLayoutGenerator::layoutType(buildFloatType()).size; 
       layout.alignment = 
             X86_64PrimitiveTypeLayoutGenerator::layoutType(buildFloatType()).alignment;
      }
      else if (precision == SgTypeComplex::e_doublePrecision)
      {
        layout.size = 2*X86_64PrimitiveTypeLayoutGenerator::layoutType(buildDoubleType()).size; 
        layout.alignment =X86_64PrimitiveTypeLayoutGenerator::layoutType(buildDoubleType()).alignment;
      }
      else if (precision ==SgTypeComplex::e_longDoublePrecision)
      {
        layout.size = 2* X86_64PrimitiveTypeLayoutGenerator::layoutType(buildLongDoubleType()).size; 
        layout.alignment =X86_64PrimitiveTypeLayoutGenerator::layoutType(buildLongDoubleType()).alignment;
      }
      else 
      {
        cerr<<"Unrecognized Complex type, must be one of float, double, and long double Complex"<<endl;
        ROSE_ASSERT(false);  
      }
      break;
    }
    case V_SgTypeImaginary: 
    {
      SgTypeImaginary::floating_point_precision_enum precision = 
                       isSgTypeImaginary(t)->get_precision();
      if (precision == SgTypeImaginary::e_floatPrecision)
      {
        layout.size = X86_64PrimitiveTypeLayoutGenerator::layoutType(buildFloatType()).size; 
        layout.alignment = X86_64PrimitiveTypeLayoutGenerator::layoutType(buildFloatType()).alignment;
      }
      else if (precision == SgTypeImaginary::e_doublePrecision)
      {
        layout.size = X86_64PrimitiveTypeLayoutGenerator::layoutType(buildDoubleType()).size; 
        layout.alignment = X86_64PrimitiveTypeLayoutGenerator::layoutType(buildDoubleType()).alignment;
      }
      else if (precision ==SgTypeImaginary::e_longDoublePrecision)
      {
        layout.size = X86_64PrimitiveTypeLayoutGenerator::layoutType(buildLongDoubleType()).size; 
        layout.alignment = X86_64PrimitiveTypeLayoutGenerator::layoutType(buildLongDoubleType()).alignment;
      }
      else 
      {
        cerr<<"Unrecognized Imaginary type, must be one of float, double, and long double _Imaginary"<<endl;
        ROSE_ASSERT(false);  
      }
      break;
    }

#endif

    default: return ChainableTypeLayoutGenerator::layoutType(t);
  }
  return layout;
}

StructLayoutInfo X86_64_VSPrimitiveTypeLayoutGenerator::layoutType(SgType* t) const {
  StructLayoutInfo layout;
  switch (t->variantT()) {
    case V_SgTypeLong: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgTypeUnsignedLong: {layout.size = 4; layout.alignment = 4; break;}
    case V_SgTypeComplex: 
    //"Each complex type has the same representation and alignment requirements as 
    //an array type containing exactly two elements of the corresponding real type"
    {
      SgTypeComplex::floating_point_precision_enum precision = 
                       isSgTypeComplex(t)->get_precision();
      if (precision == SgTypeComplex::e_floatPrecision)
      { 
       layout.size = 2*4;
       layout.alignment = 4;
      }
      else if (precision == SgTypeComplex::e_doublePrecision)
      {
        layout.size = 2*4;
        layout.alignment =4;
      }
      else if (precision ==SgTypeComplex::e_longDoublePrecision)
      {
        layout.size = 2*4 ;
        layout.alignment =4;
      }
      else 
      {
        cerr<<"Unrecognized Complex type, must be one of float, double, and long double Complex"<<endl;
        ROSE_ASSERT(false);  
      }
      break;
    }
    case V_SgTypeImaginary: 
    {
      SgTypeImaginary::floating_point_precision_enum precision = 
                       isSgTypeImaginary(t)->get_precision();
      if (precision == SgTypeImaginary::e_floatPrecision)
      {
        layout.size = 4;
        layout.alignment = 4;
      }
      else if (precision == SgTypeImaginary::e_doublePrecision)
      {
        layout.size = 4;
        layout.alignment = 4;
      }
      else if (precision ==SgTypeImaginary::e_longDoublePrecision)
      {
        layout.size = 4;
        layout.alignment = 4;
      }
      else 
      {
        cerr<<"Unrecognized Imaginary type, must be one of float, double, and long double _Imaginary"<<endl;
        ROSE_ASSERT(false);  
      }
      break;
    }
 
    default: return X86_64PrimitiveTypeLayoutGenerator::layoutType(t);
  }
  return layout;
}

ostream& operator<<(ostream& o, const StructLayoutEntry& e) {
  string label;
  if (isSgInitializedName(e.decl)) {
    label = "Field " + isSgInitializedName(e.decl)->get_name().getString();
  } else if (isSgClassDeclaration(e.decl)) {
    label = "Anonymous union";
  } else if (!e.decl) {
    label = "Padding";
  } else {
    cerr << "Bad decl kind " << e.decl->class_name() << endl;
    abort();
  }
  o << label << " at " << e.byteOffset << " size " << e.fieldSize;
  if (e.bitFieldContainerSize != 0) {
    o << " (bit field at bit " << e.bitOffset << " of word of size " << e.bitFieldContainerSize << ")";
  }
  return o;
}

ostream& operator<<(ostream& o, const StructLayoutInfo& i) {
  o << "Type with size " << i.size << " alignment " << i.alignment;
  if (!i.fields.empty()) {
    o << " fields: {\n";
    for (size_t j = 0; j < i.fields.size(); ++j) {
      o << "  " << i.fields[j] << "\n";
    }
    o << "}";
  }
  return o;
}

//! Get size and alignment from the member: custom_sizes. 
StructLayoutInfo CustomizedPrimitiveTypeLayoutGenerator::layoutType(SgType* t) const {
  StructLayoutInfo layout;
  switch (t->variantT()) {
    case V_SgTypeBool: 
    {
      layout.size = custom_sizes->sz_bool; 
      layout.alignment = custom_sizes->sz_alignof_bool; 
      break;
    }

    case V_SgTypeChar: 
    case V_SgTypeSignedChar: 
    case V_SgTypeUnsignedChar: 
    {
      layout.size = custom_sizes->sz_char; 
      layout.alignment = custom_sizes->sz_alignof_char; 
      break;
    }

    case V_SgTypeShort: 
    case V_SgTypeSignedShort: 
    case V_SgTypeUnsignedShort: 
    {
      layout.size = custom_sizes->sz_short; 
      layout.alignment = custom_sizes->sz_alignof_short; 
      break;
    }
    case V_SgTypeInt: 
    case V_SgEnumType: 
    case V_SgTypeSignedInt: 
    case V_SgTypeUnsignedInt: 
    {
      layout.size = custom_sizes->sz_int; 
      layout.alignment = custom_sizes->sz_alignof_int; 
      break;
    }
    case V_SgTypeLong: 
    case V_SgTypeSignedLong: 
    case V_SgTypeUnsignedLong: 
    {
      layout.size = custom_sizes->sz_long; 
      layout.alignment = custom_sizes->sz_alignof_long; 
      break;
    }

    case V_SgTypeLongLong: 
    //case V_SgTypeSignedLongLong: // No this node in ROSE yet
    case V_SgTypeUnsignedLongLong: 
    {
      layout.size = custom_sizes->sz_longlong; 
      layout.alignment = custom_sizes->sz_alignof_longlong; 
      break;
    }
    case V_SgTypeFloat: 
    {
      layout.size = custom_sizes->sz_float; 
      layout.alignment = custom_sizes->sz_alignof_float; 
      break;
    }

    case V_SgTypeDouble: 
    {
      layout.size = custom_sizes->sz_double; 
      layout.alignment = custom_sizes->sz_alignof_double; 
      break;
    }
    case V_SgTypeLongDouble: 
    {
      layout.size = custom_sizes->sz_longdouble; 
      layout.alignment = custom_sizes->sz_alignof_longdouble; 
      break;
    }
    // extended types
    case V_SgPointerType:  // UPC extension
    {
      layout.size = custom_sizes->sz_void_ptr; 
      layout.alignment = custom_sizes->sz_alignof_void_ptr; 
      break;
    } 
    case V_SgReferenceType: //{layout.size = 4; layout.alignment = 4; break;}
    {
      layout.size = custom_sizes->sz_reference;
      layout.alignment = custom_sizes->sz_alignof_reference;
      break;
    }

    case V_SgTypeVoid: 
    {
      layout.size = custom_sizes->sz_void_ptr; 
      layout.alignment = custom_sizes->sz_alignof_void_ptr; 
      break;
    }
    case V_SgTypeWchar: 
    {
      layout.size = custom_sizes->sz_wchar; 
      layout.alignment = custom_sizes->sz_alignof_wchar; 
      break;
    }
    //TODO refactor this, use this pointer !
    case V_SgTypeComplex: 
    //"Each complex type has the same representation and alignment requirements as 
    //an array type containing exactly two elements of the corresponding real type"
    {
      SgTypeComplex::floating_point_precision_enum precision = 
                       isSgTypeComplex(t)->get_precision();
      if (precision == SgTypeComplex::e_floatPrecision)
      { 
        layout.size = 2*CustomizedPrimitiveTypeLayoutGenerator::layoutType(buildFloatType()).size; 
       layout.alignment = 
             CustomizedPrimitiveTypeLayoutGenerator::layoutType(buildFloatType()).alignment;
      }
      else if (precision == SgTypeComplex::e_doublePrecision)
      {
        layout.size = 2*CustomizedPrimitiveTypeLayoutGenerator::layoutType(buildDoubleType()).size; 
        layout.alignment =CustomizedPrimitiveTypeLayoutGenerator::layoutType(buildDoubleType()).alignment;
      }
      else if (precision ==SgTypeComplex::e_longDoublePrecision)
      {
        layout.size = 2* CustomizedPrimitiveTypeLayoutGenerator::layoutType(buildLongDoubleType()).size; 
        layout.alignment =CustomizedPrimitiveTypeLayoutGenerator::layoutType(buildLongDoubleType()).alignment;
      }
      else 
      {
        cerr<<"Unrecognized Complex type, must be one of float, double, and long double Complex"<<endl;
        ROSE_ASSERT(false);  
      }
      break;
    }
    case V_SgTypeImaginary: 
    {
      SgTypeImaginary::floating_point_precision_enum precision = 
                       isSgTypeImaginary(t)->get_precision();
      if (precision == SgTypeImaginary::e_floatPrecision)
      {
        layout.size = CustomizedPrimitiveTypeLayoutGenerator::layoutType(buildFloatType()).size; 
        layout.alignment = CustomizedPrimitiveTypeLayoutGenerator::layoutType(buildFloatType()).alignment;
      }
      else if (precision == SgTypeImaginary::e_doublePrecision)
      {
        layout.size = CustomizedPrimitiveTypeLayoutGenerator::layoutType(buildDoubleType()).size; 
        layout.alignment = CustomizedPrimitiveTypeLayoutGenerator::layoutType(buildDoubleType()).alignment;
      }
      else if (precision ==SgTypeImaginary::e_longDoublePrecision)
      {
        layout.size = CustomizedPrimitiveTypeLayoutGenerator::layoutType(buildLongDoubleType()).size; 
        layout.alignment = CustomizedPrimitiveTypeLayoutGenerator::layoutType(buildLongDoubleType()).alignment;
      }
      else 
      {
        cerr<<"Unrecognized Imaginary type, must be one of float, double, and long double _Imaginary"<<endl;
        ROSE_ASSERT(false);  
      }
      break;
    }

    default: return ChainableTypeLayoutGenerator::layoutType(t);
  }
  return layout;
}

