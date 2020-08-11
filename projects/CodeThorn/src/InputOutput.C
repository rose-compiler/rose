/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"

#include "InputOutput.h"
#include "AbstractValue.h"
#include "CodeThornException.h"

using namespace std;
using namespace CodeThorn;
using namespace CodeThorn;

namespace CodeThorn {

  bool InputOutput::isBot() const { return op==BOT; }
  bool InputOutput::isNone() const { return op==NONE; }
  bool InputOutput::isStdInIO() const { return op==STDIN_VAR; }
  bool InputOutput::isStdOutIO() const { return op==STDOUT_VAR || op==STDOUT_CONST; }
  bool InputOutput::isStdErrIO() const { return op==STDERR_VAR || op==STDERR_CONST; }
  bool InputOutput::isFailedAssertIO() const { return op==FAILED_ASSERT; }
  bool InputOutput::isVerificationError() const { return op==VERIFICATION_ERROR; }
  bool InputOutput::isNonIO() const { return !(isStdInIO()||isStdOutIO()||isStdErrIO()); }
  
  void InputOutput::initVarValValues() {
    var=CodeThorn::AbstractValue();
    val=CodeThorn::Bot();
  }

  void InputOutput::recordBot() {
    initVarValValues();
    op=BOT;
  }
  void InputOutput::recordNone() {
    initVarValValues();
    op=NONE;
  }

  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  void InputOutput::recordVariable(OpType op0,AbstractValue varId) {
    switch(op0) {
    case STDIN_VAR:
    case STDOUT_VAR:
    case STDERR_VAR:
      break;
    default:
      throw CodeThorn::Exception("InputOutput::recordVariable: wrong IO operation type.");
    }
    op=op0;
    var=varId;
    val=CodeThorn::Bot();
  }

  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  void InputOutput::recordFailedAssert() {
    op=FAILED_ASSERT;
  }

  /*! 
   * \author Markus Schordan
   * \date 2015.
   */
  void InputOutput::recordVerificationError() {
    op=VERIFICATION_ERROR;
  }

  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  void InputOutput::recordConst(OpType op0,AbstractValue constvalue) {
    ROSE_ASSERT(op0==STDOUT_CONST || op0==STDERR_CONST);
    op=op0;
    var=AbstractValue();
    ROSE_ASSERT(constvalue.isConstInt());
    val=constvalue;
  }
  void InputOutput::recordConst(OpType op0,int value) {
    ROSE_ASSERT(op0==STDOUT_CONST || op0==STDERR_CONST);
    AbstractValue abstractConstValue(value);
    recordConst(op0,abstractConstValue);
  }
  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  string InputOutput::toString() const {
    string str;
    switch(op) {
    case BOT: str="bot";break;
    case NONE: str="none";break;
    case STDIN_VAR: str="stdin:"+var.toString();break;
    case STDOUT_VAR: str="stdout:"+var.toString();break;
    case STDERR_VAR: str="stderr:"+var.toString();break;
    case STDOUT_CONST: str="stdoutconst:"+val.toString();break;
    case STDERR_CONST: str="stderrconst:"+val.toString();break;
    case FAILED_ASSERT: str="failedassert";break;
    case VERIFICATION_ERROR: str="verificationerror";break;
    default:
      CodeThorn::Exception("InputOutput::toString: unknown IO operation abstraction.");
    }
    return str;
  }

  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  string InputOutput::toString(VariableIdMapping* variableIdMapping) const {
    string str;
    string varName=var.toString(variableIdMapping);
    switch(op) {
    case BOT: str="bot";break;
    case NONE: str="none";break;
    case STDIN_VAR: str="stdin:"+varName;break;
    case STDOUT_VAR: str="stdout:"+varName;break;
    case STDERR_VAR: str="stderr:"+varName;break;
    case STDOUT_CONST: str="stdoutconst:"+val.toString();break;
    case STDERR_CONST: str="stderrconst:"+val.toString();break;
    case FAILED_ASSERT: str="failedassert";break;
    case VERIFICATION_ERROR: str="verificationerror";break;
    default:
      CodeThorn::Exception("InputOutput::toString: unknown IO operation abstraction.");
      exit(1);
    }
    return str;
  }

  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  bool operator<(const InputOutput& c1, const InputOutput& c2) {
    if(c1.op!=c2.op)
      return c1.op<c2.op;
    if(!(c1.var==c2.var))
      return c1.var<c2.var;
    return CodeThorn::strictWeakOrderingIsSmaller(c1.val,c2.val);
  }

  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  bool operator==(const InputOutput& c1, const InputOutput& c2) {
    return c1.op==c2.op && c1.var==c2.var && (CodeThorn::strictWeakOrderingIsEqual(c1.val,c2.val));
  }

  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  bool operator!=(const InputOutput& c1, const InputOutput& c2) {
    return !(c1==c2);
  }

} // end of namespace CodeThorn
