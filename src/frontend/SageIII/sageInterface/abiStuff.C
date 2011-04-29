// tps (01/14/2010) : Switching from rose.h to sage3
#include "sage3basic.h"
#include "sageBuilder.h"
#include "abiStuff.h"
#include <vector>

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

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
  // DQ (12/1/2009): MSVC reports: warning C4715: 'ChainableTypeLayoutGenerator::layoutType' : not all control paths return a value
  // but it not clear how to modify this code to avoid that warning.
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
      StructLayoutInfo layout;
      size_t currentOffset = 0;
      const SgBaseClassPtrList& bases = def->get_inheritances();
      for (SgBaseClassPtrList::const_iterator i = bases.begin();
           i != bases.end(); ++i) {
        SgBaseClass* base = *i;
        SgClassDeclaration* basecls = base->get_base_class();
        layoutOneField(basecls->get_type(), base, false, currentOffset, layout);
      }
      const SgDeclarationStatementPtrList& body = def->get_members();
      bool isUnion = (decl->get_class_type() == SgClassDeclaration::e_union);
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
      if (layout.alignment != 0 && layout.size % layout.alignment != 0) {
        size_t paddingNeeded = layout.alignment - (layout.size % layout.alignment);
        if (!isUnion) {
          layout.fields.push_back(StructLayoutEntry(NULL, layout.size, paddingNeeded));
        }
        layout.size += paddingNeeded;
      }
      return layout;
    }
    case V_SgArrayType: {
      StructLayoutInfo layout = this->beginning->layoutType(isSgArrayType(t)->get_base_type());
      layout.fields.clear();
      SgExpression* numElements = isSgArrayType(t)->get_index();

      //Adjustment for UPC array like a[100*THREADS],treat it as a[100]
      // Liao, 8/7/2008
      if (isUpcArrayWithThreads(isSgArrayType(t)))
      {
        SgMultiplyOp* multiply = isSgMultiplyOp(isSgArrayType(t)->get_index());
        ROSE_ASSERT(multiply);
        numElements = multiply->get_lhs_operand();
      }  
      if (!isSgValueExp(numElements)) {
        cerr << "Error: trying to compute static size of an array with non-constant size" << endl;
        abort();
      }
      layout.size *= SageInterface::getIntegerConstantValue(isSgValueExp(numElements));
      return layout;
    }
    case V_SgTypeComplex: {
    //"Each complex type has the same representation and alignment requirements as 
    //an array type containing exactly two elements of the corresponding real type"
      StructLayoutInfo layout = this->beginning->layoutType(isSgTypeComplex(t)->get_base_type());
      layout.size *= 2;
      return layout;
    }
    case V_SgTypeImaginary: {
      StructLayoutInfo layout = this->beginning->layoutType(isSgTypeImaginary(t)->get_base_type());
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
      ROSE_ASSERT(layout.size!=0);
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
      // pointer to shared: phaseless or not
      SgType * pointed_type = isSgPointerType(t)->get_base_type();
      if(isSgModifierType(pointed_type))
      {
      if (isUpcSharedModifierType(isSgModifierType(pointed_type))){
        if (isUpcPhaseLessSharedType(pointed_type))
        {
//cout<<"Found a pointer to  phaseless UPC shared type."<<endl;          
          layout.size = custom_sizes->sz_pshared_ptr;
          layout.alignment= custom_sizes->sz_alignof_pshared_ptr;
        }
        else
        {
//cout<<"Found a pointer to  phased UPC shared type."<<endl;          
          layout.size = custom_sizes->sz_shared_ptr;
          layout.alignment= custom_sizes->sz_alignof_shared_ptr;
          ROSE_ASSERT(layout.alignment!=0);
        }
       }
      }
      else
      {
        layout.size = custom_sizes->sz_void_ptr; 
        layout.alignment = custom_sizes->sz_alignof_void_ptr; 
      }
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

    default: return ChainableTypeLayoutGenerator::layoutType(t);
  }
  return layout;
}

template <typename T>
struct alignof {

     struct test {
          char c;
          T t;
     };

#ifndef USE_ROSE
  // DQ (1/26/2010): this fails in EDG (so a problem for ROSE compiling ROSE).
     static const size_t v = offsetof(struct test, t);
#else
     static const size_t v = 0;
#endif
};

enum testenum { testenum_x = INT_MAX }; /* NO_STRINGIFY */

StructLayoutInfo SystemPrimitiveTypeLayoutGenerator::layoutType(SgType* t) const {
  StructLayoutInfo layout;
  switch (t->variantT()) {
    case V_SgTypeBool: {layout.size = sizeof(bool); layout.alignment = alignof<bool>::v; break;}

    case V_SgTypeChar: {layout.size = sizeof(char); layout.alignment = alignof<char>::v; break;}
    case V_SgTypeSignedChar: {layout.size = sizeof(signed char); layout.alignment = alignof<signed char>::v; break;}
    case V_SgTypeUnsignedChar: {layout.size = sizeof(unsigned char); layout.alignment = alignof<unsigned char>::v; break;}

    case V_SgTypeShort: {layout.size = sizeof(short); layout.alignment = alignof<short>::v; break;}
    case V_SgTypeSignedShort: {layout.size = sizeof(signed short); layout.alignment = alignof<signed short>::v; break;}
    case V_SgTypeUnsignedShort: {layout.size = sizeof(unsigned short); layout.alignment = alignof<unsigned short>::v; break;}

    case V_SgTypeInt: {layout.size = sizeof(int); layout.alignment = alignof<int>::v; break;}
    case V_SgEnumType: {layout.size = sizeof(testenum); layout.alignment = alignof<testenum>::v; break;} // strictly, this depends on the members of the enumeration (see C99 s6.7.2.2, para 4)
    case V_SgTypeSignedInt: {layout.size = sizeof(signed int); layout.alignment = alignof<signed int>::v; break;}
    case V_SgTypeUnsignedInt: {layout.size = sizeof(unsigned int); layout.alignment = alignof<unsigned int>::v; break;}

    case V_SgTypeLong: {layout.size = sizeof(long); layout.alignment = alignof<long>::v; break;}
    case V_SgTypeSignedLong: {layout.size = sizeof(signed long); layout.alignment = alignof<signed long>::v; break;}
    case V_SgTypeUnsignedLong: {layout.size = sizeof(unsigned long); layout.alignment = alignof<unsigned long>::v; break;}

    case V_SgTypeLongLong: {layout.size = sizeof(long long); layout.alignment = alignof<long long>::v; break;}
    //case V_SgTypeSignedLongLong: {layout.size = sizeof(); layout.alignment = alignof<>::v; break;}
    case V_SgTypeUnsignedLongLong: {layout.size = sizeof(unsigned long long); layout.alignment = alignof<unsigned long long>::v; break;}

    case V_SgTypeFloat: {layout.size = sizeof(float); layout.alignment = alignof<float>::v; break;}
    case V_SgTypeDouble: {layout.size = sizeof(double); layout.alignment = alignof<double>::v; break;}
    case V_SgTypeLongDouble: {layout.size = sizeof(long double); layout.alignment = alignof<long double>::v; break;}

    case V_SgPointerType: {layout.size = sizeof(void *); layout.alignment = alignof<void *>::v; break;}
    case V_SgReferenceType: {layout.size = sizeof(void *); layout.alignment = alignof<void *>::v; break;}
#if 1    
    //case V_SgTypeVoid: {layout.size = 1; layout.alignment = 1; break;}
    //case V_SgTypeWchar: {layout.size = 4; layout.alignment = 4; break;}
#endif

    default: return ChainableTypeLayoutGenerator::layoutType(t);
  }
  return layout;
}

