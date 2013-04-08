
#ifndef __ANNOT_COLLECT_H
#define __ANNOT_COLLECT_H

#include "OperatorDescriptors.h"

typedef AnnotCollectionBase<OperatorDeclaration> OperatorCollectionBase;
typedef ReadAnnotCollection<OperatorDeclaration, '{', ';','}'> ReadOperatorCollection;

//! An interface to read into type(class) and operator(function) annotations
// Each type of 
class ROSE_DLL_API ReadAnnotation {
  ReadTypeCollection typeInfo;
  ReadOperatorCollection opInfo;
  static ReadAnnotation* inst;
  ReadAnnotation() {}
 public:
  void add_TypeCollection (TypeCollectionBase* c) 
    { typeInfo.push_back(c); }
  void add_OperatorCollection (OperatorCollectionBase *c)
    { opInfo.push_back(c); }

  void read( std::istream& in);
  void read();

  static ReadAnnotation* get_inst();
  static std::string OptionString()
    {
      return "-annot <filename> ";
    }
};


#endif
