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
    void addLabel(SPRAY::Label lab);
    void removeLabel();
    size_t getLength() const;
    std::string toString() const;

    /** Establish an ordering of call strings as required for ordered STL containers.
        For same length all values must be lower, otherwise the length must be shorter.
        Examples: [1,2] < [2,3]; [1,3]<[1,2,3]; 
        Compares this string with 'other'.
    */
    bool operator<(const CallString& other) const;
    bool operator==(const CallString& other) const;
    bool operator!=(const CallString& other) const;

  private:
    typedef std::vector<SPRAY::Label> CallStringType;
    CallStringType _callString;
  };

}

#endif
