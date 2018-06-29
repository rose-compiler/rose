#ifndef CONSTANT_VALUE
#define CONSTANT_VALUE

#include <string>
#include <math.h>

using namespace std;

class SgStringVal;
class SgFunctionRefExp;
class SgExpression;

class ConstantValue {
    bool hasIntValue_;
    bool hasFloatValue_;
    bool hasDoubleValue_;
    bool hasLongDoubleValue_;
    string code;

public:

    ConstantValue() {
        setNoArithmeticValue();

        code = "";

        string_literal = NULL;
        function_reference = NULL;
        other_expression = NULL;
    }

    long long int_value;
    float float_value;
    double double_value;
    long double long_double_value;
    SgStringVal *string_literal;
    SgFunctionRefExp *function_reference;
    SgExpression *other_expression;

    void setNoArithmeticValue() {
        int_value = -1;
        float_value = NAN;
        double_value = NAN;
        long_double_value = NAN;
        hasIntValue_ = false;
        hasFloatValue_ = false;
        hasDoubleValue_ = false;
        hasLongDoubleValue_ = false;
    }

    bool hasArithmeticValue() {
        return (hasIntValue_ || hasFloatValue_ || hasDoubleValue_ || hasLongDoubleValue_);
    }

    bool hasIntValue() {
        return hasIntValue_;
    }

    bool hasFloatValue() {
        return hasFloatValue_;
    }

    bool hasDoubleValue() {
        return hasDoubleValue_;
    }

    bool hasLongDoubleValue() {
        return hasLongDoubleValue_;
    }

    bool hasValue() {
        return hasArithmeticValue() || string_literal != NULL || function_reference != NULL || other_expression != NULL;
    }

    void setIntValue(long long value_) {
        int_value = value_;
        hasIntValue_ = true;

        hasFloatValue_ = false;
        float_value = NAN;
        hasDoubleValue_ = false;
        double_value = NAN;
        hasLongDoubleValue_ = false;
        long_double_value = NAN;
    }

    void setFloatValue(float value_) {
        float_value = value_;
        hasFloatValue_ = true;

        hasDoubleValue_ = false;
        double_value = NAN;
        hasLongDoubleValue_ = false;
        long_double_value = NAN;
        hasIntValue_ = false;
        int_value = -1;
    }

    void setDoubleValue(double value_) {
        double_value = value_;
        hasDoubleValue_ = true;

        hasFloatValue_ = false;
        float_value = NAN;
        hasLongDoubleValue_ = false;
        long_double_value = NAN;
        hasIntValue_ = false;
        int_value = -1;
    }

    void setLongDoubleValue(long double value_) {
        long_double_value = value_;
        hasLongDoubleValue_ = true;

        hasFloatValue_ = false;
        float_value = NAN;
        hasDoubleValue_ = false;
        double_value = NAN;
        hasIntValue_ = false;
        int_value = -1;
    }

    void setStringValue(SgStringVal *literal) {
        setNoArithmeticValue();
        string_literal = literal;
    }

  void setFunctionReference(SgFunctionRefExp *ref, string code_) {
        setNoArithmeticValue();
        this -> code = code_;
        function_reference = ref;
    }

  void setOtherExpression(SgExpression *exp, string code_) {
        setNoArithmeticValue();
        this -> code = code_;
        other_expression = exp;
    }

    string getCode() { return code; }
};


#endif
