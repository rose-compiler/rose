#ifndef CONSTANT_VALUE
#define CONSTANT_VALUE

#include <math.h>
#include <complex>
#include <string>

using namespace std;

class SgStringVal;
class SgFunctionRefExp;
class SgExpression;

class ConstantValue {
    bool hasIntValue_;
    bool hasFloatValue_;
    bool hasDoubleValue_;
    bool hasLongDoubleValue_;
    bool hasFloatComplexValue_;
    bool hasDoubleComplexValue_;
    bool hasLongDoubleComplexValue_;
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
    std::complex<float> float_complex_value;
    std::complex<double> double_complex_value;
    std::complex<long double> long_double_complex_value;
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
        hasFloatComplexValue_ = false;
        hasDoubleComplexValue_ = false;
        hasLongDoubleComplexValue_ = false;
    }

    bool hasArithmeticValue() {
        return (hasIntValue_ || hasFloatValue_ || hasDoubleValue_ || hasLongDoubleValue_ || hasFloatComplexValue_ || hasDoubleComplexValue_ || hasLongDoubleComplexValue_);
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

    bool hasFloatComplexValue() {
        return hasFloatComplexValue_;
    }

    bool hasDoubleComplexValue() {
        return hasDoubleComplexValue_;
    }

    bool hasLongDoubleComplexValue() {
        return hasLongDoubleComplexValue_;
    }

    bool hasValue() {
        return hasArithmeticValue() || string_literal != NULL || function_reference != NULL || other_expression != NULL;
    }

    void setIntValue(long long value_) {
        setNoArithmeticValue(); // Clear any prior value;

        int_value = value_;
        hasIntValue_ = true;
    }

    void setFloatValue(float value_) {
        setNoArithmeticValue(); // Clear any prior value;

        float_value = value_;
        hasFloatValue_ = true;
    }

    void setDoubleValue(double value_) {
        setNoArithmeticValue(); // Clear any prior value;

        double_value = value_;
        hasDoubleValue_ = true;
    }

    void setLongDoubleValue(long double value_) {
        setNoArithmeticValue(); // Clear any prior value;

        long_double_value = value_;
        hasLongDoubleValue_ = true;
    }

    void setFloatComplexValue(std::complex<float> value_) {
        setNoArithmeticValue(); // Clear any prior value;

        float_complex_value = value_;
        hasFloatComplexValue_ = true;
    }

    void setDoubleComplexValue(std::complex<double> value_) {
        setNoArithmeticValue(); // Clear any prior value;

        double_complex_value = value_;
        hasDoubleComplexValue_ = true;
    }

    void setLongDoubleComplexValue(std::complex<long double> value_) {
        setNoArithmeticValue(); // Clear any prior value;

        long_double_complex_value = value_;
        hasLongDoubleComplexValue_ = true;
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
