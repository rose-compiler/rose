#ifndef LABEL_H
#define LABEL_H

#include <limits>
#include <set>
#include <string>

static constexpr std::size_t NO_LABEL_ID = std::numeric_limits<std::size_t>::max();

namespace CodeThorn {
  /*!
   * \author Markus Schordan
   * \date 2012, 2014.
   */
  class Label {
  public:
    Label();
    Label(size_t labelId);
    //Copy constructor
    Label(const Label& other);
    //Copy assignment operator
    Label& operator=(const Label& other);
    bool operator<(const Label& other) const;
    bool operator==(const Label& other) const;
    bool operator!=(const Label& other) const;
    bool operator>(const Label& other) const;
    bool operator>=(const Label& other) const;
    Label& operator+(int num);
    // prefix inc operator
    Label& operator++();
    // postfix inc operator
    Label operator++(int);
    size_t getId() const;
    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& os, const Label& label);
    bool isValid() const;
  protected:
    size_t _labelId;
  };

  std::ostream& operator<<(std::ostream& os, const Label& label);

  class LabelSet : public std::set<Label> {
  public:
    LabelSet operator+(LabelSet& s2);
    LabelSet& operator+=(LabelSet& s2);
    LabelSet operator-(LabelSet& s2);
    LabelSet& operator-=(LabelSet& s2);

    std::string toString();
    bool isElement(Label lab);
  };

  typedef std::set<LabelSet> LabelSetSet;

} // end namespace


// support hashing labels (PP 20/11/24)
namespace std {

  /// Add hash specialization for Labels
  template <>
  struct hash<CodeThorn::Label> {
    std::size_t operator()(CodeThorn::Label lbl) const noexcept {
      return std::hash<decltype(CodeThorn::Label{}.getId())>{}(lbl.getId());
    }
  };
}

#endif
