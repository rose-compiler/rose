#ifndef ABSTRACT_VALUE
#define ABSTRACT_VALUE

#include "AType.h"

namespace CodeThorn {
  template<typename AbstractVariableType,typename AbstractIndexType,typename AbstractHeapLocType=bool>
  class AbstractMemoryLocation {
  public:
    AbstractMemoryLocation();
    AbstractMemoryLocation(AbstractVariableType var);
    AbstractMemoryLocation(AbstractVariableType var, AbstractIndexType index);
    AType::BoolLattice isEqual(AbstractMemoryLocation other) const;
    AType::BoolLattice isNotEqual(AbstractMemoryLocation other) const;
    bool operator==(AbstractMemoryLocation other) const; // for STL container
    bool operator!=(AbstractMemoryLocation other) const; // for STL container
    bool operator<(AbstractMemoryLocation other) const; // for STL container
    //size_t hashValue();
  private:
    AbstractVariableType _var;
    AbstractIndexType _index;
    AbstractHeapLocType _heapLoc;
  };
  
  class AbstractValue {
  };

  template <typename AbstractMemoryLocationType>
  class AbstractAddressValue : public AbstractValue {
  public:
    AbstractAddressValue(AbstractMemoryLocationType memLoc):_memLoc(memLoc) {}
  private:
    AbstractMemoryLocationType _memLoc;
  };

  template <typename AbstractNumberType>
  class AbstractNumberValue : public AbstractValue {
  public:
    AbstractNumberValue(AbstractNumberType number):_number(number) {}
  private:
    AbstractNumberType _number;
  };

  // surrogate class
  template<typename AbstractNumberType,typename AbstractMemoryLocationType>
  class AbstractValueSurrogate {
  public:
    AbstractValueSurrogate();
    AbstractValueSurrogate(AbstractNumberValue<AbstractNumberType> number);
    AbstractValueSurrogate(AbstractAddressValue<AbstractMemoryLocationType> memLoc);
    ~AbstractValueSurrogate();
    bool operator<(AbstractValueSurrogate avs) const;
  private:
    AbstractValue* _val;
  };

} // end of namespace CodeThorn

#endif
