#ifndef FICONSTANALYSIS_H
#define FICONSTANALYSIS_H

// does not support -inf, +inf yet
class VariableValueRangeInfo {
public:
  VariableValueRangeInfo(ConstIntLattice min, ConstIntLattice max);
  VariableValueRangeInfo(ConstIntLattice value);
  bool isTop() const { return _width.isTop(); }
  bool isBot() const { return _width.isBot(); }
  bool isEmpty() const { return (_width==0).isTrue(); }
  ConstIntLattice minValue() const { return _min; }
  ConstIntLattice maxValue() const { return _max; }
  int minIntValue() const { assert(_min.isConstInt()); return _min.getIntValue(); }
  int maxIntValue() const { assert(_max.isConstInt()); return _max.getIntValue(); }
  ConstIntLattice width() const { return _width; }
  string toString() const {
    if(isBot())
      return "bot";
    if(isTop())
      return "top";
    return string("[")+_min.toString()+","+_max.toString()+"]";
  }
  void setArraySize(int asize);
  int arraySize();
private:
  ConstIntLattice _width;
  ConstIntLattice _min;
  ConstIntLattice _max;
  int _asize;
};

typedef map<VariableId, set<CppCapsuleConstIntLattice> > VarConstSetMap;

class VariableConstInfo {
public:
  VariableConstInfo(VariableIdMapping* variableIdMapping, VarConstSetMap* map);
  bool isAny(VariableId);
  bool isUniqueConst(VariableId);
  bool isMultiConst(VariableId);
  size_t width(VariableId);
  bool isInConstSet(VariableId varId, int varVal);
  int uniqueConst(VariableId);
  int minConst(VariableId);
  int maxConst(VariableId);
  int arraySize(VariableId);
private:
  VariableIdMapping* _variableIdMapping;
  VarConstSetMap* _map;
};

class VariableValuePair {
public:
  VariableValuePair(){}
  VariableValuePair(VariableId varId, ConstIntLattice varValue):varId(varId),varValue(varValue){}
  VariableId varId;
  ConstIntLattice varValue;
  string toString(VariableIdMapping& varIdMapping) {
    string varNameString=varIdMapping.uniqueShortVariableName(varId);
    string varValueString=varValue.toString();
    return varNameString+"="+varValueString;
  }
};

#endif
