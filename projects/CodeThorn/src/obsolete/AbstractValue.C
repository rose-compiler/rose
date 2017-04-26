#include "AbstractValue.h"

using namespace CodeThorn;

template<typename AbstractNumberType,typename AbstractMemoryLocationType>
AbstractValueSurrogate<AbstractNumberType,AbstractMemoryLocationType>::AbstractValueSurrogate():_val(0) {
}

template<typename AbstractNumberType,typename AbstractMemoryLocationType>
AbstractValueSurrogate<AbstractNumberType,AbstractMemoryLocationType>::AbstractValueSurrogate(AbstractNumberValue<AbstractNumberType> number) {
  _val=new AbstractNumberValue<AbstractNumberType>(number);
}

template<typename AbstractNumberType,typename AbstractMemoryLocationType>
AbstractValueSurrogate<AbstractNumberType,AbstractMemoryLocationType>::AbstractValueSurrogate(AbstractAddressValue<AbstractMemoryLocationType> abstractAddress) {
  _val=new AbstractAddressValue<AbstractMemoryLocationType>(abstractAddress);
}

template<typename AbstractNumberType,typename AbstractMemoryLocationType>
AbstractValueSurrogate<AbstractNumberType,AbstractMemoryLocationType>::~AbstractValueSurrogate() {
  delete _val;
}

template<typename AbstractNumberType,typename AbstractMemoryLocationType>
bool AbstractValueSurrogate<AbstractNumberType,AbstractMemoryLocationType>::operator<(AbstractValueSurrogate other) const {
  // TODO
  return false;
}

template<typename AbstractVariableType,typename AbstractIndexType,typename AbstractHeapLocType>    
AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType>::AbstractMemoryLocation() {
}

template<typename AbstractVariableType,typename AbstractIndexType,typename AbstractHeapLocType>    
AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType>::AbstractMemoryLocation(AbstractVariableType var) {
  _var=var;
}

template<typename AbstractVariableType,typename AbstractIndexType,typename AbstractHeapLocType>    
AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType>::AbstractMemoryLocation(AbstractVariableType var, AbstractIndexType index){
  _var=var;
  _index=index;
}

template<typename AbstractVariableType,typename AbstractIndexType,typename AbstractHeapLocType>    
bool AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType>::operator==(AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType>  other) const {
  return _var==other._var && _index==other._index && _heapLoc==other._heapLoc;
}

template<typename AbstractVariableType,typename AbstractIndexType,typename AbstractHeapLocType>    
bool AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType>::operator!=(AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType> other) const {
  return !(*this==other);
}

template<typename AbstractVariableType,typename AbstractIndexType,typename AbstractHeapLocType>    
bool AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType>::operator<(AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType>  other) const {
  if(_var!=other._var)
    return _var<other._var;
  else if(_index!=other._index)
    return _index<other._index;
  else
    return _heapLoc<other._heapLoc;
}

template<typename AbstractVariableType,typename AbstractIndexType,typename AbstractHeapLocType>    
AType::BoolLattice AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType>::isEqual(AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType>  other) const {
  return AType::BoolLattice(_var==other._var) && AType::BoolLattice(_index==other._index) && AType::BoolLattice(_heapLoc==other._heapLoc);
}

template<typename AbstractVariableType,typename AbstractIndexType,typename AbstractHeapLocType>    
AType::BoolLattice AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType>::isNotEqual(AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType> other) const {
  return !(AType::BoolLattice(*this==other));
}

#if 0
template<typename AbstractVariableType,typename AbstractIndexType,typename AbstractHeapLocType>    
size_t AbstractMemoryLocation<AbstractVariableType,AbstractIndexType,AbstractHeapLocType>::hashValue(){
  return 0;
}
#endif
