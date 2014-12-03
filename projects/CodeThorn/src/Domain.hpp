#ifndef DOMAIN_HPP
#define DOMAIN_HPP

template<typename AbstractValue>
class Domain {
public:
  virtual AbstractValue add(AbstractValue& e1, AbstractValue& e2)=0;
  virtual AbstractValue sub(AbstractValue& e1, AbstractValue& e2)=0;
  virtual AbstractValue mul(AbstractValue& e1, AbstractValue& e2)=0;
  virtual AbstractValue div(AbstractValue& e1, AbstractValue& e2)=0;
  virtual AbstractValue neg(AbstractValue& e2)=0;
  virtual AbstractValue intValue(AbstractValue& e0)=0;
};

#endif
