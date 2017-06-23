#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H

#include "VariableIdMapping.h"
#include "AbstractValue.h"

/*! 
  * \author Markus Schordan
  * \date 2012.
 * \brief Input: a value val is read into a variable var
 * Output: either a variable or a value is written
 */

namespace CodeThorn {

  class InputOutput {
  public:
  InputOutput():op(NONE),var(CodeThorn::AbstractValue()){ val=CodeThorn::Bot();}
    enum OpType {NONE,STDIN_VAR,STDOUT_VAR,STDOUT_CONST,STDERR_VAR,STDERR_CONST, FAILED_ASSERT,VERIFICATION_ERROR};
    OpType op;
    CodeThorn::AbstractValue var;
    CodeThorn::AbstractValue val;
    string toString() const;
    string toString(SPRAY::VariableIdMapping* variableIdMapping) const;
    void recordVariable(OpType op, CodeThorn::AbstractValue varId);
    void recordConst(OpType op, CodeThorn::AbstractValue val);
    void recordConst(OpType op, int val);
    void recordFailedAssert();
    void recordVerificationError();
    bool isStdInIO() const { return op==STDIN_VAR; }
    bool isStdOutIO() const { return op==STDOUT_VAR || op==STDOUT_CONST; }
    bool isStdErrIO() const { return op==STDERR_VAR || op==STDERR_CONST; }
    bool isFailedAssertIO() const { return op==FAILED_ASSERT; }
    bool isVerificationError() const { return op==VERIFICATION_ERROR; }
    bool isNonIO() const { return op==NONE; }
  };
  
  bool operator<(const InputOutput& c1, const InputOutput& c2);
  bool operator==(const InputOutput& c1, const InputOutput& c2);
  bool operator!=(const InputOutput& c1, const InputOutput& c2);
  
} // end of namespace CodeThorn

#endif
