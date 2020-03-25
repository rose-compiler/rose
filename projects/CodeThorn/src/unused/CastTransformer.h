#ifndef CAST_TRANSFORMER_H
#define CAST_TRANSFORMER_H

typedef bool CTInheritedAttributeType;
typedef SgType* CTSynthesizedAttributeType;

class CastTransformer : public AstTopDownBottomUpProcessing<CTInheritedAttributeType,CTSynthesizedAttributeType> {
 public:
  virtual CTInheritedAttributeType   evaluateInheritedAttribute (SgNode* astNode, CTInheritedAttributeType inheritedValue);
  virtual CTSynthesizedAttributeType evaluateSynthesizedAttribute (SgNode*, CTInheritedAttributeType, SubTreeSynthesizedAttributes);
  CTSynthesizedAttributeType defaultSynthesizedAttribute(CTInheritedAttributeType inh);

  void transformCommandLineFiles(SgProject* project);
  
 private:
  void traverseWithinCommandLineFiles(SgProject* project);
  void traverseWithinCommandLineFiles(SgProject* project, CTInheritedAttributeType inh);
  
  bool isOutsideTemplate(CTInheritedAttributeType inh);
  bool castIsNecessary(SgType* source, SgType* target);
  void eliminateCast(SgCastExp*);
  void introduceCast(SgExpression*,SgType*);
  void changeCast(SgCastExp*,SgType* newType);
  bool isFloatingPointType(SgType*);
  bool isLessPrecise(SgType*,SgType*);
  bool isEqualPrecise(SgType*,SgType*);
  bool isLessOrEqualPrecise(SgType* t1, SgType* t2);
};

#endif
