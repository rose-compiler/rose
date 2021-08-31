#include "sage3basic.h"
#include "CodeThornException.h"
#include "Label.h"
#include <sstream>
#include <iostream>

using namespace std;

namespace CodeThorn {
  Label::Label() {
    _labelId=NO_LABEL_ID;
  }

  Label::Label(size_t labelId) {
    ROSE_ASSERT(labelId!=NO_LABEL_ID);
    _labelId=labelId;
  }

  //Copy constructor
  Label::Label(const Label& other) {
    _labelId=other._labelId;
  }

  bool Label::isValid() const {
    return _labelId!=NO_LABEL_ID;
  }

  //Copy assignemnt operator
  Label& Label::operator=(const Label& other) {
    // prevent self-assignment
    if (this != &other) {
      _labelId = other._labelId;
    }
    return *this;
  }

  bool Label::operator<(const Label& other) const {
    return _labelId<other._labelId;
  }
  bool Label::operator==(const Label& other) const {
    return _labelId==other._labelId;
  }
  bool Label::operator!=(const Label& other) const {
    return !(*this==other);
  }
  bool Label::operator>(const Label& other) const {
    return !(*this<other||*this==other);
  }
  bool Label::operator>=(const Label& other) const {
    return !(*this<other);
  }
  Label& Label::operator+(int num) {
    _labelId+=num;
    return *this;
  }
  // prefix inc operator
  Label& Label::operator++() {
    ++_labelId;
    return *this;
  }
  // postfix inc operator
  Label Label::operator++(int) {
    Label tmp(*this);
    ++(*this);
    return tmp;
  }

  size_t Label::getId() const {
    return _labelId;
  }

  std::string Label::toString() const {
    if(_labelId==NO_LABEL_ID) {
      return "NO_LABEL_ID";
    } else {
      std::stringstream ss;
      ss<<_labelId;
      return ss.str();
    }
  }
} // end of namespace

// friend function
ostream& CodeThorn::operator<<(ostream& os, const Label& label) {
  os<<label.toString();
  return os;
}
