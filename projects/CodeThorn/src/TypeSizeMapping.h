#ifndef TYPE_SIZE_MAPPING_H
#define TYPE_SIZE_MAPPING_H

#include <vector>
#include <unordered_map>

namespace CodeThorn {
enum BuiltInType {
    BITYPE_BOOL,
    BITYPE_CHAR,   BITYPE_CHAR16,   BITYPE_CHAR32,
    BITYPE_SHORT,  BITYPE_INT,      BITYPE_LONG,       BITYPE_LONG_LONG,
    BITYPE_FLOAT,  BITYPE_DOUBLE,   BITYPE_LONG_DOUBLE,
    BITYPE_POINTER,BITYPE_REFERENCE,
    BITYPE_SIZE
  };
}

namespace CodeThorn {

  class VariableIdMappingExtended;
  class VariableId;
  
  // typesize in bytes
  typedef long int TypeSize;
  
  class TypeSizeMapping {
  public:
    TypeSizeMapping();
    ~TypeSizeMapping();
    // sets sizes of all types (same as reported by sizeof on respective architecture)
    void setBuiltInTypeSizes(std::vector<CodeThorn::TypeSize> mapping);
    // sets size of one type (same as reported by sizeof on respective architecture)
    void setTypeSize(BuiltInType bitype, CodeThorn::TypeSize size);
    CodeThorn::TypeSize getTypeSize(CodeThorn::BuiltInType bitype);
    bool isUndefinedTypeSize(CodeThorn::TypeSize size);
    std::size_t sizeOfOp(BuiltInType bitype);
    bool isCpp11StandardCompliant();
    std::string toString();
    /* determine size of type in bytes from SgType and stored mapping
       of builtin types. The computation of the type size uses only
       type sizes provided by the type size mapping (it is independent
       of the system the analyzer is running on).
    */
    CodeThorn::TypeSize determineTypeSize(SgType* sgType);

    // returns the element type size of an array
    CodeThorn::TypeSize determineElementTypeSize(SgArrayType* sgType);

    // returns the element type size of an array
    CodeThorn::TypeSize determineNumberOfElements(SgArrayType* sgType);

    // returns the size of the type pointed to.
    CodeThorn::TypeSize determineTypeSizePointedTo(SgPointerType* sgType);

    void computeOffsets(SgProject* project,CodeThorn::VariableIdMappingExtended* vim);
    std::list<SgVariableDeclaration*> getDataMembers(SgClassDefinition* classDef);
    int getOffset(CodeThorn::VariableId varId);
    // returns true if the variable is a member of a struct/class/union.
    bool isStructMember(CodeThorn::VariableId varId);
    bool isUnionDeclaration(SgNode* node);
  protected:
    std::vector<CodeThorn::TypeSize> _mapping={1,
                                               1,2,4,
                                               2,4,8,8,
                                               4,8,16,
                                               8,8
    };
    std::unordered_map<SgType*,unsigned int> _typeToSizeMapping;
  };
}

#endif
