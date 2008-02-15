
#ifndef ANNOT_COLLECT_H
#define ANNOT_COLLECT_H

//#include <TypeAnnotation.h>
#include <OperatorDescriptors.h>

typedef AnnotCollectionBase<OperatorDeclaration> OperatorCollectionBase;
typedef ReadAnnotCollection<OperatorDeclaration, '{', ';','}'> 
ReadOperatorCollection;

class ReadAnnotation {
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
