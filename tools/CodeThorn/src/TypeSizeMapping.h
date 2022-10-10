#ifndef TYPE_SIZE_MAPPING_H
#define TYPE_SIZE_MAPPING_H

#include <vector>
#include <unordered_map>

#include "VariableIdMapping.h"

namespace CodeThorn {
enum BuiltInType {
		  BITYPE_UNKNOWN,
		  BITYPE_VOID,
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
  
  class TypeSizeMapping {
  public:
    TypeSizeMapping();
    ~TypeSizeMapping();
    static BuiltInType determineBuiltInTypeId(SgType* sgType);
    CodeThorn::TypeSize getBuiltInTypeSize(CodeThorn::BuiltInType bitype);
    // sets sizes of all types (same as reported by sizeof on respective architecture)
    void setBuiltInTypeSizes(std::vector<CodeThorn::TypeSize> mapping);
    // sets size of one type (same as reported by sizeof on respective architecture)
    void setTypeSize(BuiltInType bitype, CodeThorn::TypeSize size);
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
    static CodeThorn::TypeSize determineNumberOfElements(SgArrayType* sgType);

    // returns the size of the type pointed to.
    CodeThorn::TypeSize determineTypeSizePointedTo(SgPointerType* sgType);

    std::list<SgVariableDeclaration*> getDataMembers(SgClassDefinition* classDef);
    //int getOffset(CodeThorn::VariableId varId);
    // returns true if the variable is a member of a struct/class/union.
    static bool isUnionDeclaration(SgNode* node);
    static bool isClassOrStructDeclaration(SgNode* node);
  protected:
    std::vector<CodeThorn::TypeSize> _mapping={0,
					       0,1,
                                               1,2,4,
                                               2,4,8,8,
                                               4,8,16,
                                               8,8
    };
    std::unordered_map<SgType*, TypeSize> _typeToSizeMapping;
  };
}

#endif
