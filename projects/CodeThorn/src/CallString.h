#ifndef CALL_STRING_H
#define CALL_STRING_H

#include <vector>
#include <string>
#include "Labeler.h"

namespace CodeThorn {
  /**
     A CallString is used as context in inter-procedural analysis. It
     consists of Labels that are associated with the function calls in
     an analyzed program.
  */
  class CallString {
  public:
    /** Adds a label to the call string if it is not longer than getLength().
        Returns true if the label is added, otherwise false.
    */
    bool isEmpty();
    bool addLabel(CodeThorn::Label lab);

    /** Removes the last label of the call string if it matches with the argument 'lab'.
        In case of success this function returns true, otherwise false.
        On a call string of length 0 it always returns false and no operation is performed.
    */
    bool removeIfLastLabel(CodeThorn::Label lab);

    /** true if the provided label is the last label in the call
        string. Always false for the empty call string.
     */
    bool isLastLabel(CodeThorn::Label lab);

    /** removes last label. 
        If call string is of length 0 no operation is performed.
    */
    void removeLastLabel();


    /** returns the length of the call string.
     */
    size_t getLength() const;
    static size_t getMaxLength();
    static void setMaxLength(size_t maxLength);

    /** generates a string showing the labels of the call string
        within brackets. For each label the label-id (an integer) is
        shown. No white space is generated. Labels are separated by
        commas.
        Example: [2,1,5]
    */
    std::string toString() const;

    /** Establish an ordering of call strings as required for ordered STL containers.
        Compares this string with 'other'.
        Returns true if the length is shorter or if same length then all values are lower
        Examples: [1,2] < [2,3]; [1,3]<[1,2,3]; 
    */
    bool operator<(const CallString& other) const;
    bool operator==(const CallString& other) const;
    bool operator!=(const CallString& other) const;

    // hash value for this callstring
    size_t hash() const;

  private:
    static size_t _maxLength;
    typedef std::vector<CodeThorn::Label> CallStringType;
    CallStringType _callString;
  };

}

// custom specialization of std::hash injected in namespace std
namespace std
{
  template<> struct hash<CodeThorn::CallString> {
    typedef CodeThorn::CallString argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& cs) const noexcept {
      return cs.hash();
    }
  };
}


#endif
