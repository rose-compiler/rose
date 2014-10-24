
#include "rose.h"
#include "RoseAst.h"


class TypeCounter : public AstSimpleProcessing {

public:
  size_t getNumberOfDistinctTypes();
  void visit(SgNode *n);
  void printDistinctTypes(std::ostream& outs);

private:
  std::vector<SgType *> typesVisitedSoFar, distTypes;
  size_t numberOfDistinctTypes;
  size_t scalarTypes, namedTypes, pointerTypes, referenceTypes;
};


void TypeCounter::printDistinctTypes(std::ostream& outs){
  for(std::vector<SgType *>::iterator i = distTypes.begin(); i != distTypes.end(); ++i){
//    outs << (*i)->unparseToString() << std::endl;
  }
  outs << "scalarTypes: " << scalarTypes << "\nnamedTypes: " << namedTypes << "\npointerTypes: "
    << pointerTypes << "\nreferenceTypes: " << referenceTypes << std::endl;
}

size_t TypeCounter::getNumberOfDistinctTypes(){
  std::cout << "Evaluating container" << std::endl;
//  std::vector<SgType *> distTypes;
  if(distTypes.empty()){
    std::cout << "DistTypes is empty" << std::endl;
    if((!typesVisitedSoFar.empty()) && (typesVisitedSoFar.front() != NULL)){
      SgType *_t = typesVisitedSoFar.front();
      distTypes.push_back(_t);
      std::cout << "pushed_back" << std::endl;
    }
  }
  scalarTypes = namedTypes = pointerTypes = referenceTypes = 0;
  std::cout << "counting " << std::endl;
  for(std::vector<SgType *>::iterator i = typesVisitedSoFar.begin(); i != typesVisitedSoFar.end(); ++i){
    bool flag = false;
    for(std::vector<SgType *>::iterator j = distTypes.begin(); j != distTypes.end(); ++j){
      if(SageInterface::checkTypesAreEqual(*j, *i)){
        flag = true;
      }
    }
    if(!flag){
      distTypes.push_back(*i);
      // we build some statistics
      if(SageInterface::isScalarType(*i)){
        scalarTypes += 1;
      } else if (isSgNamedType(*i)){
        namedTypes += 1;
      } else if (SageInterface::isPointerType(*i)){
        pointerTypes += 1;
      } else if (SageInterface::isReferenceType(*i)){
        referenceTypes += 1;
      }
    }
  }

  return distTypes.size();
}

void TypeCounter::visit(SgNode *n){
//  std::cout << "Node: " << n << std::endl;
  // This code is only to get all the different types
  if(isSgExpression(n)){
    SgExpression *_e = isSgExpression(n);
    if(_e){
      SgType *_t = _e->get_type();
      if(_t){
        typesVisitedSoFar.push_back(_t);
      }
    }
  }

  if(isSgStatement(n)){
    if(isSgStatement(n)->hasExplicitType()){
      if(isSgTypedefDeclaration(n)){
        SgType *_t = isSgTypedefDeclaration(n)->get_base_type();
        if(_t){
          typesVisitedSoFar.push_back(_t);
        }
      } else {
        std::cerr << "Documentation is wrong! Found: " << n->class_name() << std::endl;
      }
    }
  }

  if(isSgType(n)){
    std::cout << "Hit Type" << std::endl;
    SgType *_t = isSgType(n);
    if(_t){
      typesVisitedSoFar.push_back(_t);
    }
  }
}


int main(int argc, char** argv){

  std::cout << "Type Counter" << std::endl;

  SgProject *project = frontend(argc, argv);

  TypeCounter tc;
  tc.traverse(project, preorder);

  std::cout << "Distinct Types encountered: " << tc.getNumberOfDistinctTypes() << std::endl;
  tc.printDistinctTypes(std::cout);

}
