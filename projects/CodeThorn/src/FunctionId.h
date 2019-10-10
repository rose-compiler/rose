#ifndef FUNCTION_ID_H
#define FUNCTION_ID_H

#include <string>
#include <set>

namespace CodeThorn {

class FunctionIdMapping;

/*!
  * \author Simon Schroder (based on Markus Schordan's VariableId)
  * \date 2012.
 */
class FunctionId {
  friend class FunctionIdMapping;
  friend class ConstraintSetHashFun;
  friend bool operator<(FunctionId id1, FunctionId id2);
 public:
  FunctionId();
  static const char * const idKindIndicator;
  // Returns the id code as a string with the given prefix
  std::string toString(const char* prefix = idKindIndicator) const;
  // Uses the given mapping to return a more detailed string representation
  std::string toString(const CodeThorn::FunctionIdMapping& vid) const;
  // Returns the id code
  int getIdCode() const { return _id; }
  // we intentionally do not provide a constructor for int because this would clash
  // with overloaded functions that are using AbstractValue (which has an implicit
  // type conversion for int)
  void setIdCode(int id);
  // Returns whether this id is valid
  bool isValid() const { return _id >= 0; }

  bool operator==(const FunctionId& other) const;
  bool operator!=(const FunctionId& other) const;
 private:
  // The id code
  int _id;
};

 bool operator<(FunctionId id1, FunctionId id2);
  
 class FunctionIdHashFunction {
 public:
   std::size_t operator()(const FunctionId& p) const;
 };

 typedef std::set<FunctionId> FunctionIdSet;
 FunctionIdSet& operator+=(FunctionIdSet& s1, const FunctionIdSet& s2);
} // end of namespace

#endif
