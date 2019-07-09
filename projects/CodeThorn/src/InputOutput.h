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
  InputOutput():op(BOT),var(CodeThorn::AbstractValue()){ val=CodeThorn::Bot();}
    // BOT is used to indicate the bottom element of the EState
    // lattice, default is NONE. BOT is used explicitly when setting a
    // bottom element in the analysis intialization such that the
    // bottom element and extremal value are guaranteed to be
    // different.
    enum OpType {NONE,BOT,STDIN_VAR,STDOUT_VAR,STDOUT_CONST,STDERR_VAR,STDERR_CONST, FAILED_ASSERT,VERIFICATION_ERROR};
    OpType op;
    CodeThorn::AbstractValue var;
    CodeThorn::AbstractValue val;
    string toString() const;
    string toString(CodeThorn::VariableIdMapping* variableIdMapping) const;
    string toDetailedString(CodeThorn::VariableIdMapping* variableIdMapping) const;
    void recordBot();
    void recordNone();
    void recordVariable(OpType op, CodeThorn::AbstractValue varId);
    void recordConst(OpType op, CodeThorn::AbstractValue val);
    void recordConst(OpType op, int val);
    void recordFailedAssert();
    void recordVerificationError();
    bool isBot() const;
    bool isNone() const;
    bool isStdInIO() const;
    bool isStdOutIO() const;
    bool isStdErrIO() const;
    bool isFailedAssertIO() const;
    bool isVerificationError() const;
    bool isNonIO() const;
  private:
    void initVarValValues();
  };
  
  bool operator<(const InputOutput& c1, const InputOutput& c2);
  bool operator==(const InputOutput& c1, const InputOutput& c2);
  bool operator!=(const InputOutput& c1, const InputOutput& c2);
  
} // end of namespace CodeThorn

#endif
