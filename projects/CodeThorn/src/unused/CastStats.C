#include "sage3basic.h"
#include "CastStats.h"
#include "RoseAst.h"
#include <sstream>
#include <string>
#include "SgNodeHelper.h"

using namespace std;

void CastStats::addCast(SgType* from, SgType* to) {
  tuple<SgType*,SgType*> t(from,to);
  if(casts.find(t)==casts.end()) {
    casts[t]=0;
  }
  casts[t]++;
}

std::string CastStats::typeName(SgType* type) {
  if(SgNamedType* nameType=isSgNamedType(type)) {
    return nameType->get_name();
  } else {
    SgUnparse_Info info;
    info.set_SkipEnumDefinition();
    info.set_SkipClassDefinition();
    return type->unparseToString(&info);
  }
}

std::string CastStats::toString(CastTypes ct, CastCountType num) {
  SgType* castFromType=std::get<0>(ct);
  SgType* castToType=std::get<1>(ct);
  return typeName(castFromType)+" => "+typeName(castToType)+" : "+to_string(num);
}

std::string CastStats::toString() {
  stringstream ss;
  for(auto t : casts) {
    ss<<toString(t.first,t.second)<<endl;
  }
  return ss.str();
}

void CastStats::computeStats(SgNode* node) {
  RoseAst ast(node);
  for(auto node : ast) {
    if(SgCastExp* castExp=isSgCastExp(node)) {
      SgType* toType=castExp->get_type();
      SgExpression* operand=castExp->get_operand();
      SgType* fromType=operand->get_type();
      // only add casts of builit-in floating point types
      if(SgNodeHelper::isFloatingPointType(fromType) 
         && SgNodeHelper::isFloatingPointType(toType)) {
        addCast(fromType,toType);
      }
    }
  }
}
