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
    friend bool operator==(const CallString& c1, const CallString& c2);
    std::string toString() const;
    // is true if this call string is a lower value-substring of the string 'other'
    // examples: [1,2] < [1,3]; [1,3]<[1,2,3] (based on size);
    bool isLowerValueSubstring(const CodeThorn::CallString& other) const;

  private:
    typedef std::vector<SPRAY::Label> CallStringType;
    CallStringType _callString;
  };

  bool operator<(const CodeThorn::CallString& c1, const CodeThorn::CallString& c2);
  bool operator==(const CallString& c1, const CallString& c2);
  bool operator!=(const CallString& c1, const CallString& c2);
}

#endif
