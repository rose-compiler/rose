#include <complex>
#include <rosetollvm/LLVMAstAttributes.h>
#include <rosetollvm/CodeAttributesVisitor.h>
#include <rosetollvm/ConstantIntegerEvaluator.h>
#include <rosetollvm/Utf8.h>

#include <llvm/ADT/ArrayRef.h>
#include <llvm/BinaryFormat/Dwarf.h> // For LLVMDebugVersion
#include <llvm/IR/Metadata.h>

#ifdef HAVE_LOOPUNROLLER_BUNDLEATTRIBUTE_H
#  include <loopunroller/bundleAttribute.h>
#endif

#ifdef HAVE_LOOPUNROLLER_ALIGNMENTATTRIBUTE_H
#  include <loopunroller/alignmentAttribute.h>
#endif

#ifdef HAVE_POLYOPT_SAGENODEANNOTATION_HPP
#  include <polyopt/SageNodeAnnotation.hpp>
#endif

using namespace std;

long LLVMAstAttributes::tmp_count = 0,
     LLVMAstAttributes::tmp_int_count = 0;

const string LLVMAstAttributes::getTemp(TEMP_KIND k) {
    stringstream out;
    switch(k) {
        case TEMP_INT:
            out << "%";
            break;
        case TEMP_INCREMENT:
            out << "%.incr";
            break;
        case TEMP_DECREMENT:
            out << "%.decr";
            break;
        case TEMP_NEGATION:
            out << "%.neg";
            break;
        case TEMP_ADD:
            out << "%.add";
            break;
        case TEMP_ADD_PTR:
            out << "%.add.ptr";
            break;
        case TEMP_SUBTRACT:
            out << "%.sub";
            break;
        case TEMP_SUBTRACT_PTR:
            out << "%.sub.ptr";
            break;
        case TEMP_MULTIPLY:
            out << "%.mul";
            break;
        case TEMP_DIVIDE:
            out << "%.div";
            break;
        case TEMP_REMAINDER:
            out << "%.rem";
            break;
        case TEMP_COMPARE:
            out << "%.cmp";
            break;
        case TEMP_ARRAY:
            out << "%.arraydecay";
            break;
        case TEMP_ARRAY_INDEX:
            out << "%.arrayidx";
            break;
        case TEMP_CALL:
            out << "%.call";
            break;
        case TEMP_IF_TRUE: 
            out << "if.true";
            break;
        case TEMP_IF_FALSE:
            out << "if.false";
            break;
        case TEMP_IF_END:
            out << "if.end";
            break;
        case TEMP_WHILE_CONDITION:
            out << "while.cond";
            break;
        case TEMP_WHILE_BODY:
            out << "while.body";
            break;
        case TEMP_WHILE_END:
            out << "while.end";
            break;
        case TEMP_DO_CONDITION:
            out << "do.cond";
            break;
        case TEMP_DO_BODY:
            out << "do.body";
            break;
        case TEMP_DO_END:
            out << "do.end";
            break;
        case TEMP_FOR_CONDITION:
            out << "for.cond";
            break;
        case TEMP_FOR_BODY:
            out << "for.body";
            break;
        case TEMP_FOR_INCREMENT:
            out << "for.inc";
            break;
        case TEMP_FOR_END:
            out << "for.end";
            break;
        case TEMP_SWITCH_END_LABEL:
            out << "sw.end";
            break;
        case TEMP_CASE_LABEL:
            out << "sw.case";
            break;
        case TEMP_DEFAULT_LABEL:
            out << "sw.default";
            break;
        case TEMP_RETURN:
            out << "%.return";
            break;
        case TEMP_CONVERT:
            out << "%.conv";
            break;
        case TEMP_NOT:
            out << "%.not";
            break;
        case TEMP_AND:
            out << "%.and";
            break;
        case TEMP_OR:
            out << "%.or";
            break;
        case TEMP_XOR:
            out << "%.xor";
            break;
        case TEMP_COND:
            out << "%.cond";
            break;
        case TEMP_COND_TRUE: 
            out << "cond.true";
            break;
        case TEMP_COND_FALSE:
            out << "cond.false";
            break;
        case TEMP_COND_END:
            out << "cond.end";
            break;
        case TEMP_LOGICAL_AND_RHS:
            out << "land.rhs";
            break;
        case TEMP_LOGICAL_AND_END:
            out << "land.end";
            break;
        case TEMP_LOGICAL_OR_RHS:
            out << "lor.rhs";
            break;
        case TEMP_LOGICAL_OR_END:
            out << "lor.end";
            break;
        case TEMP_SHIFT_LEFT:
            out << "%.shl";
            break;
        case TEMP_SHIFT_RIGHT:
            out << "%.shr";
            break;
        case TEMP_ANONYMOUS:
            out << "%union.anon";
            break;
        case TEMP_LHS_POINTER_TO_INT:
            out << "%sub.ptr.lhs.cast";
            break;
        case TEMP_RHS_POINTER_TO_INT:
            out << "%sub.ptr.rhs.cast";
            break;
        case TEMP_COERCE:
            out << "%.coerce";
            break;
        case TEMP_AGGREGATE:
            out << "%.agg.tmp";
            break;
        case TEMP_POINTER_DIFFERENCE_ARITHMETIC_SHIFT_RIGHT:
            out << "%sub.ptr.shr";
            break;
        case TEMP_POINTER_DIFFERENCE_DIVISION:
            out << "%sub.ptr.div";
            break;
        case TEMP_LABEL: // just needs a number
            break;
        default:
            out << "%.tmp";
            break;
    }
    out << (k == TEMP_INT ? tmp_int_count++ : tmp_count++);
    return out.str();
}


/**
 * Check whether or not casting is required between type1 and type2.
 */
bool LLVMAstAttributes::isTrivialCast(SgType *type1, SgType *type2) {
    SgPointerType *pointer1 = isSgPointerType(type1),
                  *pointer2 = isSgPointerType(type2);
    if (pointer1 && pointer2) {
        return isTrivialCast(getSourceType(pointer1 -> get_base_type()),
                             getSourceType(pointer2 -> get_base_type()));
    }
    SgArrayType *array1 = isSgArrayType(type1),
                *array2 = isSgArrayType(type2);
    if (array1 && array2) {
      return (((IntAstAttribute *) array1 -> getAttribute(Control::LLVM_SIZE)) -> getValue() ==
              ((IntAstAttribute *) array2 -> getAttribute(Control::LLVM_SIZE)) -> getValue())
              &&
              ((IntAstAttribute *) array1 -> getAttribute(Control::LLVM_SIZE)) -> getValue() != 0
              &&
              ((IntAstAttribute *) array2 -> getAttribute(Control::LLVM_SIZE)) -> getValue() != 0 // this test is not needed...
              &&
              isTrivialCast(getSourceType(array1 -> get_base_type()),
                            getSourceType(array2 -> get_base_type()));
    }

    bool result = (type1 == type2 || // same class type?
                   (isSgTypeFloat(type1) && isSgTypeFloat(type2)) ||
                   (isSgTypeDouble(type1) && isSgTypeDouble(type2)) ||
                   (isSgTypeLongDouble(type1) && isSgTypeLongDouble(type2)) ||
                   (isSgTypeComplex(type1) && isSgTypeComplex(type2) && isTrivialCast(getSourceType(isSgTypeComplex(type1) -> get_base_type()),
                                                                                      getSourceType(isSgTypeComplex(type2) -> get_base_type()))) ||
                   (
                    (type1 -> isIntegerType() || isSgEnumType(type1)) &&
                    (type2 -> isIntegerType() || isSgEnumType(type2)) &&
                    ((IntAstAttribute *) type1 -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue() == ((IntAstAttribute *) type2 -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()
                   )
                  );

    return result;
}


/**
 * Rose has a function type -> isUnsignedType() that is supposed to yield the same result
 * as this function. However, it has a bug and does not include the type: unsigned long.
 */
bool LLVMAstAttributes::isUnsignedType(SgType *type) {
    return type -> isUnsignedType() || isSgTypeUnsignedLong(type);
}


/**
 * The type might be encapsulated in an SgModifierType.
 */
bool LLVMAstAttributes::isFloatType(SgType *type) {
    return this -> getSourceType(type) -> isFloatType();
}

/**
 * The type might be encapsulated in an SgModifierType.
 */
bool LLVMAstAttributes::isIntegerType(SgType *type) {
    type = this -> getSourceType(type);
    return (type -> isIntegerType() || isSgEnumType(type));
}

/**
 * The type might be encapsulated in an SgModifierType.
 */
bool LLVMAstAttributes::isBooleanType(SgType *type) {
    type = this -> getSourceType(type);
    return isSgTypeBool(type);
}



/**
 * Checks whether the type is a typedef whose name starts with "valign_"
 */
bool LLVMAstAttributes::isValignType(SgType *type) {
    SgTypedefType * t = dynamic_cast<SgTypedefType *> (type);
    return (t && (t -> get_name().getString().find("valign_") == 0));
}


/**
 *
 */
string LLVMAstAttributes::findLabel(SgLabelStatement *n) {
    string label = ((SgName) n -> get_label()).getString();

    map<string, int>::iterator itr = label_map.find(label);
    int label_index;
    if (itr == label_map.end()) {
        label_index = tmp_count++;
        label_map[label] = label_index;
    }
    else label_index = itr->second;

    stringstream out;
    out << "label." << label_index;

    return out.str();
}

const string LLVMAstAttributes::getFunctionTemp(string function_name, string var_name) {
    stringstream out;
    out << "@" << function_name << "." << var_name << tmp_count++;
    return out.str();
}

const string LLVMAstAttributes::getFunctionName(SgFunctionSymbol *sym) {
     string function_name = "@";
     function_name.append(sym -> get_name().getString());
     return function_name;
}

const string LLVMAstAttributes::getFunctionSignature(SgFunctionSymbol *sym) {
     stringstream out;
     SgFunctionType *function_type = isSgFunctionType(sym -> get_type());
     ROSE2LLVM_ASSERT(function_type);
     SgType *return_type = function_type -> get_return_type();
     if (isSgClassType(getSourceType(return_type))) {
         int size = integralStructureType(return_type);
         if (size == 0) {
             out << "void";
         }
         else {
             out << "i" << (size * 8);
         }
     }
     else {
         out << ((StringAstAttribute *) return_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
     }
     out << " " << getFunctionName(sym);
     out << ((StringAstAttribute *) function_type -> getAttribute(Control::LLVM_FUNCTION_ARGUMENTS)) -> getValue();

     return out.str();
}

const string LLVMAstAttributes::getGlobalStringConstantName(int string_index) {
    stringstream out;
    out << "@\"\\01LC" << string_index << "\"";
    return out.str();
}

const string LLVMAstAttributes::getGlobalStringReference(int string_index) {
    const char *data = getString(string_index);
    stringstream out;
    out << "getelementptr inbounds ([" << getStringLength(string_index) << " x i8], [" << getStringLength(string_index) << " x i8]* @\"\\01LC" << string_index << "\", i32 0, i32 0)";
    return out.str();
}

const string LLVMAstAttributes::getGlobalStringReference(int string_index, string array_index) {
    const char *data = getString(string_index);
    stringstream out;
    out << "getelementptr inbounds [" << getStringLength(string_index) << " x i8], [" << getStringLength(string_index) << " x i8]* @\"\\01LC" << string_index << "\", i32 0, i32 " << array_index;
    return out.str();
}

#include <codecvt>
LLVMAstAttributes::StringLiteral LLVMAstAttributes::preprocessString(SgStringVal *string_val, int string_size) {
    stringstream out;
    int length = 0,
        size = 0;

    if (string_val -> get_is16bitString()) {
/*      
cout << "*** Encountered "
     << (string_val -> get_wcharString() ? "Wchar " : "ASCII ")
     << (string_val -> get_is16bitString() ? "16 bits " : "")
     << (string_val -> get_is32bitString() ? "32 bits " : "")
     << (string_val -> get_isRawString() ? "raw " : "")
     << "string : \""
     << string_val -> get_value()
     << "\" with raw value \""
     << string_val -> get_raw_string_value()
     << "\""
     << endl;
cout.flush();

Utf8 my_str(string_val -> get_value(), Utf8::CHAR16);
u32string my_s = my_str.getChar32();

std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
std::u32string utf32 = cvt.from_bytes(string_val -> get_value());
cout << "*** Outputting CHAR32 from source with "
     << string_val -> get_value().size()
     << " characters: " << endl;
    for (int k = 0; k < utf32.size(); k++) {
        int value = utf32[k];
cout << " 0x" << hex << value  << "  => "
     << " 0x" << hex << (k < my_s.size() ? my_s[k] : ' ')
     << endl;
    }

    for (int k = 0; k < utf32.size(); k++) {
        int value = utf32[k];
cout << (char) value;
    }
cout << endl;
*/    
        ROSE2LLVM_ASSERT(! "yet support wchar_t (16) strings");
    }
    else if (string_val -> get_is32bitString()) {
    /*
        Utf8 my_str(string_val -> get_value(), Utf8::CHAR32);
        u32string my_s = my_str.getChar32();

        //std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
        //std::u32string utf32 = cvt.from_bytes(string_val -> get_value());

        string s = my_str.getUtf8();
        cout << endl
             << "The utf8 codes for string \"" << s << "\" are: "
             << endl;
        for (int k = 0; k < s.size(); k++) {
            cout << " 0x" << hex << s[k] << " ";
        }
        cout << endl
             << endl;

       cout << endl
            << "The utf32 codes for string \"" << s << "\" are: "
            << endl;
       for (int k = 0; k < my_s.size(); k++) {
           cout << " 0x" << hex << ((int) my_s[k]) << " ";
       }
       cout << endl;
    */

        ROSE2LLVM_ASSERT(! "yet support wchar_t (32) strings");
    }
    else {
        const string in = string_val -> get_value();
// TODO: Remove this !!!
      
        Utf8 my_str(in, Utf8::CHAR8);
/*
cout << "**** Encountered "
     << (string_val -> get_wcharString() ? "Wchar " : "ASCII ")
     << (string_val -> get_is16bitString() ? "16 bits " : "")
     << (string_val -> get_is32bitString() ? "32 bits " : "")
     << (string_val -> get_isRawString() ? "raw " : "")
     << " string : \"" << in << "\"" << endl
     << "converted to \"" << my_str.getChar() << "\"" << endl;
cout.flush();
*/
/*
std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
std::u32string utf32 = cvt.from_bytes(string_val -> get_value());

u32string my_s = my_str.getChar32();

cout << "*** Outputting CHAR32 from source with "
     << string_val -> get_value().size()
     << " characters: " << endl;
    for (int k = 0; k < utf32.size(); k++) {
        int value = utf32[k];
cout << " 0x" << hex << value  << "  => "
       << " 0x" << hex << (k < my_s.size() ? my_s[k] : ' ')
     << endl;
    }

    for (int k = 0; k < utf32.size(); k++) {
        int value = utf32[k];
cout << (char) value;
    }
cout << endl;
*/
        for (int i = 0; size < string_size && i < in.length(); i++) {
            length++;
            size++;
            out << in[i];
            if (in[i] == '\\') {
                length += 2; // There will be 2 more characters

                // LLVM escape sequences always look like \xx where each x
                // is a hexadecimal digit.
                if (in[i + 1] == 'n') {
                    i++;
                    prepFor2DigitHex(out);
                    out << ((int) '\n');
                }
                else if (in[i + 1] == 't') {
                    i++;
                    prepFor2DigitHex(out);
                    out << ((int) '\t');
                }
                else if (in[i + 1] == 'b') {
                    i++;
                    prepFor2DigitHex(out);
                    out << ((int) '\b');
                }
                else if (in[i + 1] == 'r') {
                    i++;
                    prepFor2DigitHex(out);
                    out << ((int) '\r');
                }
                else if (in[i + 1] == 'f') {
                    i++;
                    prepFor2DigitHex(out);
                    out << ((int) '\f');
                }
                else if (in[i + 1] == 'v') {
                    i++;
                    prepFor2DigitHex(out);
                    out << ((int) '\v');
                }
                else if (in[i + 1] == 'a') {
                    i++;
                    prepFor2DigitHex(out);
                    out << ((int) '\a');
                }
                else if (in[i + 1] == '?') {
                    i++;
                    prepFor2DigitHex(out);
                    out << ((int) '\?');
                }
                else if (in[i + 1] == '\"' || in[i + 1] == '\'' || in[i + 1] == '\\') {
                    i++;
                    prepFor2DigitHex(out);
                    out << ((int) in[i]);
                }
                //
                //            Rose emits a 3 digit sequence for '\0'.
                //
                else if (isdigit(in[i + 1]) && isdigit(in[i + 2]) && isdigit(in[i + 3])) {
                    istringstream strm(in.substr(i + 1, 3));
                    int v;
                    strm >> std::oct >> v;
                    prepFor2DigitHex(out);
                    out << v;
                    i += 3;
                }
            }
        }
    }

// TODO: Remove this !!!
/*
cout << "*** Outputting string : \"" << out.str() << "\" with size " << size << endl;
cout.flush();
*/
    ROSE2LLVM_ASSERT(size <= string_size);
      
    return (StringLiteral) {out.str(), length, size};
}

int LLVMAstAttributes::insertString(SgStringVal *string_val) {
    StringLiteral literal = preprocessString(string_val, string_val -> get_value().size());
    literal.value += "\\00";
    literal.length += 3;
    literal.size++;
    return string_table.insert(literal.value.c_str(), literal.size);
}

int LLVMAstAttributes::insertString(SgStringVal *string_val, int size) {
    StringLiteral literal = preprocessString(string_val, size);
/*
if (size < literal.size) {
cout << "** Request to insert string \""
     << string_val -> get_value() << "\" which was preprocessed as \""
     << literal.value << "\" with size " << literal.size
     << "; The requested final output size is " << size
    << endl;
cout.flush();
}
*/
    for (int i = literal.size; i < size; i++) {
        literal.value += "\\00";
        literal.length += 3;
        literal.size++;
    }
 
    return string_table.insert(literal.value.c_str(), size);
}

/*
int LLVMAstAttributes::getLength(const char *in) {
    int length = 0;
    for (const char *p = in; *p; p++) {
        length++;
        if (*p == '\\' && isHex(p[1]) && isHex(p[2]))
            p += 2;
    }

    return length;
}
*/

const string LLVMAstAttributes::setLLVMTypeName(SgType *type) {
    ROSE2LLVM_ASSERT(type != NULL);

    StringAstAttribute *attr = (StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE);
    string str;
    int size = -1;

    if (attr != NULL) {
        str = attr -> getValue();
    }
    else {
        if (dynamic_cast<SgTypeVoid *>(type)) {
            str = "void";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(0));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
        }
        else if (dynamic_cast<SgTypeBool *>(type)) {
            // Like llvm-clang and llvm-gcc, we use i8 not i1 for bool.
            // Our justification is that sizeof returns a byte count not
            // a bit count, which affects malloc, memcpy, pointer
            // arithmetic in some forms of memory pooling, etc.
            str = "i8";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(1));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(1));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0"));
        }
        else if (dynamic_cast<SgTypeChar *>(type) || dynamic_cast<SgTypeSignedChar *>(type) || dynamic_cast<SgTypeUnsignedChar *>(type)) {
            size = sizeof(char); // compute the size of a char on this machine.
            str = getIntegerBitSize(size);
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0"));
        }
        else if (dynamic_cast<SgTypeShort *>(type) || dynamic_cast<SgTypeSignedShort *>(type) || dynamic_cast<SgTypeUnsignedShort *>(type)) {
            size = sizeof(short); // compute the size of a short on this machine.
            str = getIntegerBitSize(size);
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0"));
        }
        else if (dynamic_cast<SgTypeInt *>(type) || dynamic_cast<SgTypeSignedInt *>(type) || dynamic_cast<SgTypeUnsignedInt *>(type) || dynamic_cast<SgEnumType *>(type)) {
            size = sizeof(int);
            str = getIntegerBitSize(size);
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0"));
        }
        else if (dynamic_cast<SgTypeLong *>(type) || dynamic_cast<SgTypeSignedLong *>(type) || dynamic_cast<SgTypeUnsignedLong *>(type)) {
            size = sizeof(long);
            str = getIntegerBitSize(size);
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0"));
        }
        else if (dynamic_cast<SgTypeLongLong *>(type) || dynamic_cast<SgTypeSignedLongLong *>(type) || dynamic_cast<SgTypeUnsignedLongLong *>(type)) {
            size = sizeof(long long);
            str = getIntegerBitSize(size);
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0"));
        }
        else if (dynamic_cast<SgTypeFloat *>(type)) {
            size = sizeof(float);
            str = "float";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute(Control::FloatToString(0.0)));
        }
        else if (dynamic_cast<SgTypeDouble *>(type)) {
            size = sizeof(double);
            str = "double";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute(Control::DoubleToString(0.0)));
        }
        else if (dynamic_cast<SgTypeLongDouble *>(type)) {
            size = sizeof(long double);
            str = "x86_fp80";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute(Control::LongDoubleToString(0.0)));
        }
        else if (dynamic_cast<SgTypeComplex *>(type)) {
            SgTypeComplex* complex_type = isSgTypeComplex(type);
            SgType *component_type = getSourceType(complex_type -> get_base_type());
            string component_type_name = setLLVMTypeName(component_type);
            int align_type = ((IntAstAttribute *) component_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
// TODO: Remove this !!!
/*      
cout << "***Processing ";
*/
            if (isSgTypeFloat(component_type)) {
// TODO: Remove this !!!
/*      
cout << "Float Complex type of size ";
*/
                str = "{ float, float }";
                align_type = sizeof(float);
                size = sizeof(complex<float>);
            }
            else if (isSgTypeDouble(component_type)) {
// TODO: Remove this !!!
/*      
cout << "Double Complex type of size ";
*/
                str = "{ double, double }";
                align_type = sizeof(double);
                size = sizeof(complex<double>);
            }
            else if (isSgTypeLongDouble(component_type)) {
// TODO: Remove this !!!
/*      
cout << "Long Double Complex type of size ";
*/
                str = "{ x86_fp80, x86_fp80 }";
                align_type = sizeof(long double);
                size = sizeof(complex<long double>);
            }
            else {
                ROSE2LLVM_ASSERT(! "know how to process this kind of complex type");
            }
// TODO: Remove this !!!
/*      
cout << size
     << endl;
cout.flush();
*/
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(align_type));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0.0e+00"));
        }
        else if (dynamic_cast<SgArrayType *>(type)) {
            SgArrayType* array_type = isSgArrayType(type);
            SgType *element_type = array_type -> get_base_type();
            string element_type_name = setLLVMTypeName(element_type);

            SgExpression *array_bound_expression = array_type -> get_index();

            /**
             * Note that the expression:
             *
             *      size_t array_size = SageInterface::getArrayElementCount(array_type);
             *
             * yields the total number of elements in an array.  Thus if the ArrayType in question
             * is, for example, a matrix of size 8X4 then the expression above will yield 32 instead
             * of 8.
             */
            // TODO: Remove this ... Old ROSE construct.  Now an SgIntVal is used.
            //
            //            SgUnsignedLongVal *specified_size = isSgUnsignedLongVal(array_bound_expression);
            //            size_t array_size = (specified_size ? specified_size -> get_value() : 1); // compute number of elements in this array.


            /**
             * The bound of the array may have been specified as a constant expression.
             * That expression must be attributed before we can attempt to evaluate it.
             * We use the ad hoc attributes visitor to preprocess the array bound expression here.
             */
            if (array_bound_expression) {
                if (option.isDebugPreTraversal() || option.isDebugPostTraversal()) {
                    cerr << "==> Start Traversing array index expression "
                         << array_bound_expression -> class_name()
                         << "; tmp_count = "
                         << tmp_count
                         << "; tmp_int_count = "
                         << tmp_int_count
                         << endl;
                    cerr.flush();
                }

                control.getAdHocAttributesVisitor(this) -> traverse(array_bound_expression);

                if (option.isDebugPreTraversal() || option.isDebugPostTraversal()) {
                    cerr << "==> Done Traversing array index expression "
                         << array_bound_expression -> class_name()
                         << endl;
                    cerr.flush();
                }
            }
            ConstantIntegerEvaluator evaluator(this);
            ConstantValue x = evaluator.traverse(array_bound_expression);
            size_t array_size = (x.hasIntValue() ? (size_t) x.int_value : 0);

// TODO: Remove this !!!
/*
cout
  << "*** The type of the dimension of array "
  <<  array_type -> get_name().getString()
  << " is "
  << (array_bound_expression ? array_bound_expression -> class_name() : " NULL???")
  << "; its value is "
  << array_size
  << endl;
cout.flush();
*/
            if (x.hasIntValue() && x.int_value > 0) {
                control.SetAttribute(array_bound_expression, Control::LLVM_CONSTANT_VALUE, new IntAstAttribute(x.int_value));
            }

            int element_size = ((IntAstAttribute *) element_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
            std::ostringstream out;
            out << "[" << array_size << " x " << element_type_name << "]";
            str = out.str();
            size = array_size * element_size;
            control.SetAttribute(type, Control::LLVM_AGGREGATE, new AggregateAstAttribute(array_type));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(((IntAstAttribute *) element_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()));
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("null"));
        }
        else if (dynamic_cast<SgPointerType *>(type)) {
            SgPointerType *pointer_type = isSgPointerType(type);
            size = sizeof(void *);
            str = (isSgTypeVoid(getSourceType(pointer_type -> get_base_type())) ? "i8" : setLLVMTypeName(pointer_type -> get_base_type()));
            str += "*";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("null"));
        }
        else if (dynamic_cast<SgFunctionType *>(type)) {
            SgFunctionType *n = isSgFunctionType(type);
// TODO: Remove this !!!
/*
cout
  << "*** Processing function type "
  << n -> get_mangled().getString()
  << endl;
cout.flush();
SgSymbol *symbol = n -> get_symbol_from_symbol_table();
if (symbol) {
  cout << "*** Processing function symbol "
       << symbol -> get_name().getString()
       << " with declaratiom "
       << (n -> getAssociatedDeclaration() ? n -> getAssociatedDeclaration() -> class_name() : "?")
       << endl;
}
else {
  cout << "*** Did not find function symbol but declaration is "
       << (n -> getAssociatedDeclaration() ? n -> getAssociatedDeclaration() -> class_name() : "?")
       << endl;
}
cout.flush();
*/
            SgType *return_type = n -> get_return_type();
            ROSE2LLVM_ASSERT(return_type);
            ROSE2LLVM_ASSERT(return_type != type);
            string return_type_name = setLLVMTypeName(return_type);
            if (isSgClassType(getSourceType(return_type))) {
                int size = integralStructureType(return_type);
                if (size == 0) {
                    str = "void";
                }
                else {
                    stringstream out;
                    out << "i" << (size * 8);
                    str = out.str();
                }
            }
            else {
                str = return_type_name;
            }

            string args_signature = "(";
            vector<SgType *> arg_type = n -> get_arguments();
            if (isSgClassType(getSourceType(return_type)) && integralStructureType(return_type) == 0) {
                args_signature.append(return_type_name);
                args_signature.append("* noalias sret");
                if (arg_type.size() > 0)
                    args_signature.append(", ");
            }
            for (int i = 0; i < arg_type.size(); i++) {
                 SgType *type = arg_type[i];
                 string parm_type_name = setLLVMTypeName(type);
                 SgArrayType *array_type = isSgArrayType(type);
                 if (array_type) { // convert array argument into a pointer.
                     parm_type_name = ((StringAstAttribute *) array_type -> get_base_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                     parm_type_name += "*";
                 }
                 else if (isSgClassType(type)) { // only a pointer to a structure is passed.
                     parm_type_name += "*";
                 }

                args_signature.append(parm_type_name);
                if (i + 1 < arg_type.size())
                    args_signature.append(", ");
            }

            /**
             * If a compiler-generated function type declaration does not have a final argument with type SgTypeEllipsis, add that final argument
             * so that any function call to that function will look legitimate.
             */
// TODO: Remove this !!!
/*
SgSymbol *symbol = n -> get_symbol_from_symbol_table();
if (n -> get_has_ellipses()) {    
  cout << "*** The function "
       << (symbol ? symbol -> get_name().getString() : "?")
       << " has ellipsis"
       << endl;
}
else {
  cout << "*** No ellipsis found for "
       << (symbol ? symbol -> get_name().getString() : "?")
       << endl;
}
if (n -> attributeExists(Control::LLVM_COMPILER_GENERATED)) { // this function was declared inside a block?
  cout << "*** Function "
       << (symbol ? symbol -> get_name().getString() : "?")
       << " is compiler-generated"
       << endl;
}
else { // this function was declared inside a block?
  cout << "*** Function "
       << (symbol ? symbol -> get_name().getString() : "?")
       << " is NOT compiler-generated"
       << endl;
}
cout.flush();
*/
            if ((n -> get_has_ellipses() || n -> attributeExists(Control::LLVM_COMPILER_GENERATED)) && (arg_type.size() == 0 || (! isSgTypeEllipse(arg_type[arg_type.size() - 1])))) {
                if (arg_type.size() > 0) {
                    args_signature.append(",");
                }
                args_signature.append("...");
            }

            args_signature.append(")");
            control.SetAttribute(type, Control::LLVM_FUNCTION_ARGUMENTS, new StringAstAttribute(args_signature));

            str.append(" ");
            str.append(args_signature);
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(sizeof(void *)));
        }
        else if (dynamic_cast<SgTypeString *>(type)) { // This type is only used internally by Rose.
            size = sizeof(void *);
            str = "i8 *";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
        }
        else if (dynamic_cast<SgTypeEllipse *>(type)) {
            str = "...";
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(sizeof(void *))); // Align it on the same boundary as a pointer.
        }
        else if (dynamic_cast<SgModifierType *>(type)) {
            SgModifierType *n = isSgModifierType(type);
            SgType *base_type = n -> get_base_type();
            str = setLLVMTypeName(base_type);
            /**
             * TODO: probably need a more systematic way to "inherit" all the type attributes from another type?
             */
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            if (base_type -> attributeExists(Control::LLVM_SIZE)) {
                control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_SIZE)) -> getValue()));
            }
            if (base_type -> attributeExists(Control::LLVM_ALIGN_TYPE)) {
                control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()));
            }
            if (base_type -> attributeExists(Control::LLVM_DEFAULT_VALUE)) {
                control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute(((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_DEFAULT_VALUE)) -> getValue()));
            }
        }
        else if (dynamic_cast<SgTypedefType *>(type)) {
            SgTypedefType *n = isSgTypedefType(type);
            SgType *base_type = n -> get_base_type();
            str = setLLVMTypeName(base_type);

            /**
             * TODO: probably need a more systematic way to "inherit" all the type attributes from another type?
             */
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            if (base_type -> attributeExists(Control::LLVM_SIZE)) {
                control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_SIZE)) -> getValue()));
            }
            if (base_type -> attributeExists(Control::LLVM_ALIGN_TYPE)) {
                control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()));
            }
            if (base_type -> attributeExists(Control::LLVM_DEFAULT_VALUE)) {
                control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute(((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_DEFAULT_VALUE)) -> getValue()));
            }
        }
        else if (dynamic_cast<SgTypeOfType *>(type)) {
            SgTypeOfType *n = isSgTypeOfType(type);
            SgType *base_type = n -> get_base_type();
            str = setLLVMTypeName(base_type);

            /**
             * TODO: probably need a more systematic way to "inherit" all the type attributes from another type?
             */
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            if (base_type -> attributeExists(Control::LLVM_SIZE)) {
                control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_SIZE)) -> getValue()));
            }
            if (base_type -> attributeExists(Control::LLVM_ALIGN_TYPE)) {
                control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()));
            }
            if (base_type -> attributeExists(Control::LLVM_DEFAULT_VALUE)) {
                control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute(((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_DEFAULT_VALUE)) -> getValue()));
            }
        }
        else if (dynamic_cast<SgClassType *>(type)) {
            SgClassType *n = isSgClassType(type);

            /**
             * TODO: Remove this when bug is fixed!
             *
             * There is a bug in Rose that causes multiple nodes to be allocated for a class type.  We test for the duplicate case here and produce the correct result for it.
             */
            if (class_map[n -> get_qualified_name().getString()] != NULL) {
                SgClassType *base_type = class_map[n -> get_qualified_name().getString()] -> getClassType();
                str = ((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
                control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_SIZE)) -> getValue()));
                control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()));
            }
            else {
                SgClassDeclaration *decl = (SgClassDeclaration *) n -> get_declaration();
                if (decl -> get_class_type() == SgClassDeclaration::e_union) {
                    if (decl -> get_isUnNamed())
                        str = getTemp(TEMP_ANONYMOUS);
                    else {
                        str = "%union.";
                        // TODO: Add name of enclosing scope here to make sure that there are no name collisions !
                        str += n -> get_name().getString();
                    }
                }
                else {
                    str = "%struct.";
                    // TODO: Add name of enclosing scope here to make sure that there are no name collisions !
                    str += n -> get_name().getString();
                }
                control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
                processClassDeclaration(n);
            }
        }
        else if (dynamic_cast<SgTypeDefault *>(type)) { // This type is only used internally by Rose.
            size = sizeof(void *);  // Align it on the same boundary as a pointer.
            SgTypeDefault *n = isSgTypeDefault(type);
            str = "..."; // TODO: Confirm that SgTypeDefault always maps into the ellipsis type!
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
        }
        else {
             cerr << "rosetollvm: Don't know how to process type: " << type -> class_name();
             SgNamedType *named_type = isSgNamedType(type);
             if (named_type)
                 cerr << " - " << named_type -> get_name().getString();
             cerr << endl;
             ROSE2LLVM_ASSERT(0);
        }
    }

    return str;
}


void LLVMAstAttributes::processClassDeclaration(SgClassType *n)
{
    DeclarationsAstAttribute *attribute = new DeclarationsAstAttribute(n);

    ROSE2LLVM_ASSERT(class_map[n -> get_qualified_name().getString()] == NULL);
    class_map[n -> get_qualified_name().getString()] = attribute;

    SgClassDeclaration *declaration = isSgClassDeclaration(n -> get_declaration()),
                       *defining_declaration = isSgClassDeclaration(declaration -> get_definingDeclaration());

    if (! defining_declaration) {
         insertGlobalDeclaration(declaration);
         control.SetAttribute(declaration, Control::LLVM_LOCAL_DECLARATIONS, attribute); // needed because of Rose bug!
         control.SetAttribute(n, Control::LLVM_SIZE, new IntAstAttribute(0)); // an (LLVM) opaque type!
         control.SetAttribute(n, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(sizeof(void *))); // Align it on the same boundary as a pointer.
    }
    else {
        /**
         * TODO: Need to compute size of structure (sizeof) and its alignment (alignment of first field for structure,
         * largest alignment for union.
         * NOTE: there is a function called get_packingAlignment(). However, it always returns 0.
         */
        int size = 0,
            first_field_size = 0,
            alignment = 1; // default alignment is 1

        vector<SgInitializedName *> pointer_decls;

        insertGlobalDeclaration(defining_declaration);
        control.SetAttribute(defining_declaration, Control::LLVM_LOCAL_DECLARATIONS, attribute); // needed because of Rose bug!
        SgClassDefinition *def = isSgClassDefinition(defining_declaration -> get_definition());
        ROSE2LLVM_ASSERT(def);
        vector<SgDeclarationStatement *> decls = def -> get_members();
        for (int i = 0; i < decls.size(); i++) {
            if (dynamic_cast<SgVariableDeclaration *>(decls[i])) {
                vector<SgInitializedName *> vars = isSgVariableDeclaration(decls[i]) -> get_variables();
                for (int k = 0; k < vars.size(); k++) {
                    SgInitializedName *var = vars[k];

                    size_t field_size,
                           field_alignment;

                    /**
                     * If we are dealing with a pointer, we compute its size here instead of making a recursive call so setLLVMType.
                     * That is because the pointer could have a base type that involves this class and since the definition of the
                     * type is pending, the base type cannot yet be processed.  One particular hard case is a pointer to a function
                     *  that takes this class as parameter or returns an object of this class type.
                     */
                    if (isSgPointerType(getSourceType(var -> get_type()))) {
                        pointer_decls.push_back(var);
                        field_size = sizeof(void *);
                        field_alignment = sizeof(void *);
                    }
                    else {
                        SgType *type = getSourceType(var -> get_type());
                        setLLVMTypeName(type);
                        field_size = ((IntAstAttribute *) type -> getAttribute(Control::LLVM_SIZE)) -> getValue(),
                        field_alignment = ((IntAstAttribute *) type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();

                        string type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                        control.SetAttribute(var, Control::LLVM_TYPE, new StringAstAttribute(type_name)); // Tag this declaration with its bounded type name.
                        control.SetAttribute(var, Control::LLVM_DECLARATION_TYPE, new SgTypeAstAttribute(type));
                    }

                    /**
                     * The structure inherits the alignment of its largest element
                     */
                    if (field_alignment > alignment) {
                        alignment = field_alignment;
                    }

                    /**
                     * The structure inherits the alignment of its largest element
                     */
                    if (defining_declaration -> get_class_type() == SgClassDeclaration::e_union) {
                        if (field_size > size) {
                            size = field_size;
                        }
                        if (i == 0 && k == 0) {
                            first_field_size = field_size;
                        }
                        control.SetAttribute(var, Control::LLVM_FIELD_OFFSET, new IntAstAttribute(0));
                    }
                    else {
                        /**
                         * If the current prefix is not aligned with the new field, pad it!
                         */
                        if (size % field_alignment) {
/**/
// TODO: Remove all internall padding for now!!! 12/13/2018
//
                            int pad_size = alignment - (size % alignment);
                            control.SetAttribute(var, Control::LLVM_STRUCTURE_PADDING, new IntAstAttribute(pad_size));
                            size += pad_size;
                            attribute -> insertPadding(pad_size);
/**/
                        }
                        control.SetAttribute(var, Control::LLVM_FIELD_OFFSET, new IntAstAttribute(size));
                        size += field_size;
                    }
                    attribute -> addSgInitializedName(var);
                    string name = "%";
                    name += var -> get_name().getString();
                    attribute -> insertName(name);
                    control.SetAttribute(var, Control::LLVM_NAME, new StringAstAttribute(name));
                }
            }
        }


        control.SetAttribute(n, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(alignment));

        /**
         * Always pad the structure so that its size is a multiple of the alignment.
         */
        int pad_size = 0;
        if (size % alignment) {
            pad_size = alignment - (size % alignment);
            size += pad_size;
        }

        if (defining_declaration -> get_class_type() == SgClassDeclaration::e_union) {
            pad_size = size - first_field_size;
        }
        if (pad_size) {
            control.SetAttribute(n, Control::LLVM_STRUCTURE_PADDING, new IntAstAttribute(pad_size));
        }
        control.SetAttribute(n, Control::LLVM_SIZE, new IntAstAttribute(size));

        /**
         * Now that we have a size for the structure, we can process pointer types that might depend on it.
         */
        for (int i = 0; i< pointer_decls.size(); i++) {
            SgInitializedName *var = pointer_decls[i];
            SgType *type = getSourceType(var -> get_type());
            setLLVMTypeName(type);

            string type_name = ((StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
            control.SetAttribute(var, Control::LLVM_TYPE, new StringAstAttribute(type_name)); // Tag this declaration with its bounded type name.
            control.SetAttribute(var, Control::LLVM_DECLARATION_TYPE, new SgTypeAstAttribute(type));
        }
    }

    return;
}

int LLVMAstAttributes::addMetadata(llvm::MDNode *node) {
    map<llvm::MDNode *, int>::iterator itr = mdIndices.find(node);
    if (itr == mdIndices.end()) {
        mdIndices[node] = mdNodes.size();
        mdNodes.push_back(node);
        return mdNodes.size() - 1;
    }
    return itr->second;
}

std::string LLVMAstAttributes::addBundleMetadata(SgNode *node) {
#ifdef HAVE_LOOPUNROLLER_BUNDLEATTRIBUTE_H
  bundleAttribute *B =
    dynamic_cast<bundleAttribute *>(node->getAttribute(bundleAttribute::name()));
  // Bundle metadata
  // ... = load double* %foo, !bun !54
  // !54 = metadata !{i64 12, i32 0} ; 12 = bundle id, 0 = bundle idx
  if (B) {
    uint64_t bundle_id = B->getBundleId();
    uint32_t bundle_idx = B->getIdx();
    llvm::Value *const bundle_values[] = {
      llvm::ConstantInt::get( llvm::IntegerType::get(context, 64), bundle_id),
      llvm::ConstantInt::get( llvm::IntegerType::get(context, 32), bundle_idx),
    };
    llvm::MDNode *bundle = llvm::MDNode::get(context, bundle_values, 2);
    std::ostringstream bundle_str;
    bundle_str << ", !bun !";
    bundle_str << addMetadata(bundle);
// TODO: Remove this !!!
/*      
cout << "Returning bundle: "
     << bundle_str.str()
     << endl;
cout.flush();
*/
    return bundle_str.str();
  }
#endif
  return std::string();
}

std::string LLVMAstAttributes::getVectorAlignmentStr() {
    return getAlignmentStr(vectorMemAlignment());
}

std::string LLVMAstAttributes::getAlignmentStr(int alignment) {
    std::stringstream str;
    str << ", align " << alignment;
    return str.str();
}

// If the alignment is specified by the loop unroller/vectorizer, use that.
// Otherwise, if alignment is specified through the GCC attribute, use that.
// Otherwise, don't explicitly set alignment.
std::string LLVMAstAttributes::addVectorAlignment(SgNode *node) {
#ifdef HAVE_LOOPUNROLLER_ALIGNMENTATTRIBUTE_H
  alignmentAttribute *Align =
    dynamic_cast<alignmentAttribute *>
    (node->getAttribute(alignmentAttribute::name()));
  if (Align) {
    return getVectorAlignmentStr();
  }
  return std::string();
#else
  if (node->attributeExists(Control::LLVM_ALIGN_VAR)) {
    int alignment = ((IntAstAttribute*) node->getAttribute(Control::LLVM_ALIGN_VAR))->getValue();
    return getAlignmentStr(alignment);
  }
  else return std::string();
#endif
}

std::string LLVMAstAttributes::addIsParallelMetadata(SgNode *node) {
#ifdef HAVE_POLYOPT_SAGENODEANNOTATION_HPP
  SageForInfo *FI =
    dynamic_cast<SageForInfo *>(node->getAttribute(SageForInfo::name()));
  if (FI && FI->isParallel) {
    llvm::Value *const vals[] = {
      llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 1)
    };
    llvm::MDNode *noivdep_md = llvm::MDNode::get(context, vals, 1);
    std::ostringstream noivdep;
    noivdep << ", !noivdep !" << addMetadata(noivdep_md);
// TODO: Remove this !!!
/*      
cout << "Returning Parallel metadata: "
     << noivdep.str()
     << endl;
cout.flush();
*/
    return noivdep.str();
  }
#endif
  return std::string();
}

std::string LLVMAstAttributes::addDebugMetadata(SgNode const *node, FunctionAstAttribute *curr_fun_decls, bool just_index) {
    // TODO: For now, the debug metadata attached to each instruction
    // takes the form:
    //
    //   metadata !{i32 LINE, i32 COLUMN, metadata !INDEX, null}
    //
    // where the metadata node !INDEX is of the form described at
    //
    //   http://llvm.org/docs/SourceLevelDebugging.html#format_compile_units
    //
    // Together, these are exactly the LLVM form described at
    // http://llvm.org/docs/SourceLevelDebugging.html except that all
    // scope info between the line/column level and the compile unit
    // level are missing.  We might later extend our approach to include
    // this info.
    Sg_File_Info const *file_info = node->get_file_info();
    string file_name = file_info->get_filename();
    string directory_name;
    {
        size_t slash = file_name.rfind('/');
        if (slash == string::npos) {
            // If LLVM is compiled with assertions enabled, then debug
            // metadata that contains a file name without a directory
            // component will produce an assertion failure.  There are
            // two cases so far: Rose generates some bogus file names
            // like "compilerGenerated" and "transformation" (see
            // PACE-15), and #line directives can also omit the
            // directory.  Thus, we must add a directory.  FIXME: The
            // goal was to choose an impossible directory on POSIX
            // systems, but can we think of anything better than this?
            directory_name = "/dev/null/UNSPECIFIED-DIRECTORY/";
        }
        else {
            directory_name = file_name.substr(0, slash + 1);
            file_name = file_name.substr(slash + 1);
        }
    }

    // To prototype feedback directed optimization we need llvm to dump the line
    // information out (to DWARF) when it generates the object code. LLVM only
    // seems to dump that information if we include the following context.
    // Begin context information
    llvm::Metadata *dw_tag_compile_unit_values[] = {
      // Tag.  DW_TAG_compile_unit = 17 + LLVMDebugVersion
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 32), llvm::dwarf::DW_TAG_compile_unit)), // llvm::LLVMDebugVersion + 17),
      // Unused field.
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 32), 0)),
      // DWARF language identifier.  1 is what I always get from
      // "llvmc -S --emit-llvm -g".
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 32), 1)),
      // Source file name.
      llvm::MDString::get(context, file_name),
      // Source file directory.
      llvm::MDString::get(context, directory_name),
      // Producer.
      llvm::MDString::get(context, "roseToLLVM"),
      // True if this is a main compile unit.  TODO: Do we need to
      // be more careful with this one?
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), true)),
      // True if this is optimized.  TODO: Do we need to be more
      // careful with this one?
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), false)),
      // Flags.  "" is what I always get from
      // "llvmc -S --emit-llvm -g".
      llvm::MDString::get(context, ""),
      // Runtime version.  0 is what I always get from
      // "llvmc -S --emit-llvm -g".
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 32), 0))
    };
    llvm::ArrayRef<llvm::Metadata *> dw_tag_compile_unit_array(dw_tag_compile_unit_values, sizeof dw_tag_compile_unit_values / sizeof(llvm::Metadata*));
    llvm::MDNode *dw_tag_compile_unit_node = llvm::MDNode::get(context, dw_tag_compile_unit_array);
    ostringstream dw_tag_compile_unit_stream;
    dw_tag_compile_unit_stream << addMetadata(dw_tag_compile_unit_node);

    llvm::Metadata *dw_tag_file_values[] = {
      // Tag DW_TAG_file_type = 41 + LLVMDebugVersion
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 32), llvm::dwarf::DW_TAG_file_type)), // llvm::LLVMDebugVersion + 41),
      // Source file name.
      llvm::MDString::get(context, file_name),
      // Source file directory.
      llvm::MDString::get(context, directory_name),
      // Compile unit.
      llvm::MDString::get(context, "MD_REF:" + dw_tag_compile_unit_stream.str())
    };
    llvm::ArrayRef<llvm::Metadata *> dw_tag_file_array(dw_tag_file_values, sizeof dw_tag_file_values / sizeof(llvm::Metadata*));
    llvm::MDNode *dw_tag_file_node = llvm::MDNode::get(context, dw_tag_file_array);
    ostringstream dw_tag_file_stream;
    dw_tag_file_stream << addMetadata(dw_tag_file_node);

    // Get the current function's name.
    std::string fun_name;
    if (curr_fun_decls) {
        if (SgFunctionDeclaration *fun_decl = curr_fun_decls->getFunctionDeclaration()) {
            fun_name = fun_decl->get_name().getString();
        }
    }

    // A lot of the values in the following structure are dummy value. Lets hope
    // we get away with that. Fixing the FIXMEs would require having the current
    // function available.
    llvm::Metadata *dw_tag_subprogram_values[] = {
      // Tag DW_TAG_subprogram = 46 + LLVMDebugVersion
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 32), llvm::dwarf::DW_TAG_subprogram)), // llvm::LLVMDebugVersion + 46),
      // Unused field.
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 32), 0)),
      // Reference to context descriptor
      llvm::MDString::get(context, "MD_REF:" + dw_tag_file_stream.str()),
      // Name
      llvm::MDString::get(context, fun_name),
      // Display name (fully qualified C++ name)
      llvm::MDString::get(context, fun_name),
      // MIPS linkage name (for C++)
      llvm::MDString::get(context, fun_name),
      // Reference to file where defined
      llvm::MDString::get(context, "MD_REF:" + dw_tag_file_stream.str()),
      // Line number where defined
      // FIXME: dummy.
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 32), 0)),
      // Reference to type descriptor
      NULL,
      // True if the global is local to compile unit (static)
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 0)),
      // True if the global is defined in the compile unit (not extern)
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 1)),
      // Virtuality, e.g. dwarf::DW_VIRTUALITY__virtual
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 0)),
      // Index into a virtual function
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 0)),
      // indicates which base type contains the vtable pointer for the
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 1)),
      // derived class
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 0)),
      // isArtificial
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 0)),
      // isOptimized
      llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 0)),
      // Pointer to LLVM function
      NULL
    };
    llvm::ArrayRef<llvm::Metadata *> dw_tag_subprogram_array(dw_tag_subprogram_values, sizeof dw_tag_subprogram_values / sizeof(llvm::Metadata*));
    llvm::MDNode *dw_tag_subprogram_node = llvm::MDNode::get(context, dw_tag_subprogram_array);
    ostringstream dw_tag_subprogram_stream;
    dw_tag_subprogram_stream << addMetadata(dw_tag_subprogram_node);
    // End context information

    llvm::Metadata *position_values[] = {
        llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 32), file_info -> get_line())),
        llvm::ConstantAsMetadata::getConstant(llvm::ConstantInt::get(llvm::IntegerType::get(context, 32), file_info->get_col())),
        // TODO: After switching to IRBuilder, uncomment the next line,
        // and remove the line after that.  That is, let IRBuilder
        // handle metadata node indices instead of hacking it ourselves.
        //
        //file_node,
        llvm::MDString::get(context, "MD_REF:" + dw_tag_subprogram_stream.str()),
        NULL
    };
    // TODO: There is also an llvm::DebugLoc that can be attached to
    // instructions.  However, many of the associated functions are not
    // yet available in LLVM 2.7.  For example, Instruction::setDebugLoc
    // and DebugLoc::getAsMDNode.
    //
    // LLVM makes sure we don't end up with duplicate copies of the same
    // metadata within a single context.
    llvm::ArrayRef<llvm::Metadata *> position_array(position_values, sizeof position_values / sizeof(llvm::Metadata*));
    llvm::MDNode *position_node = llvm::MDNode::get(context, position_array);
    ostringstream strm;
    if (!just_index) {
      strm << ", !dbg !";
    }
    strm << addMetadata(position_node);
// TODO: Remove this !!!
/*      
cout << "Returning Debug metadata: "
     << strm.str()
     << endl;
cout.flush();
*/
    return strm.str();
}

void LLVMAstAttributes::addFunctionPragmaMetadata(string const &name, vector<SgPragma *> const &pragmas) {
    ROSE2LLVM_ASSERT(functionPragmaMetadataIndices.find(name) == functionPragmaMetadataIndices.end());
    size_t nvalues = 2 + 2 * pragmas.size();
    llvm::Metadata **values = new llvm::Metadata*[nvalues];
    values[0] = llvm::MDString::get(context, name);
    for (vector<SgPragma*>::size_type i = 0; i < pragmas.size(); ++i) {
        values[1 + 2*i] = llvm::MDString::get(context, pragmas[i]->get_pragma());
        // TODO: After switching to IRBuilder, this needs to store the
        // actual metadata node instead of this hacked reference.
        values[1 + 2*i + 1] = llvm::MDString::get(context, "MD_REF:" + addDebugMetadata(pragmas[i], NULL,  true));
    };
    values[nvalues - 1] = NULL;
    llvm::ArrayRef<llvm::Metadata *> values_array(values, nvalues);
    int index = addMetadata(llvm::MDNode::get(context, values_array));
    functionPragmaMetadataIndices[name] = index;
}

void LLVMAstAttributes::generateMetadataNodes() {
    if (functionPragmaMetadataIndices.size()) {
        codeOut << "!function_pragmas = !{";
        size_t i = 0;
        for (
            map<string, int>::const_iterator
                itr = functionPragmaMetadataIndices.begin(),
                end = functionPragmaMetadataIndices.end();
            itr != end; ++i
        ) {
            codeOut << "!" << itr->second;
            if (++itr != end) {
                codeOut << ", ";
            }
        };
        codeOut << "}" << endl;
    }
    for (vector<llvm::MDNode *>::size_type i = 0; i < mdNodes.size(); ++i) {
        string str;
        {
            llvm::raw_string_ostream strm(str);
            strm << *mdNodes[i];
        }

// TODO: Remove this !!!
/*      
cout << "Original Debug String is: "
     << str
     << endl;
cout.flush();
*/
        int k = str.find("= ");
        str = str.substr(k + 2);

// TODO: Remove this !!!
/*      
cout << "After clean up, k = " << k
     << "; Debug String is: "
     << str
     << endl;
cout.flush();
*/

        // Hopefully, "MD_REF:" will not be the start of any legitimate
        // metadata string before the switch to IRBuilder.
        size_t md_ref;
        while (string::npos != (md_ref = str.find("metadata !\"MD_REF:"))) {
            md_ref += 10; // skip to open quote
            str.replace(md_ref, 8, ""); // remove string before index
            md_ref = str.find('"', md_ref); // skip to close quote
            ROSE2LLVM_ASSERT(md_ref != string::npos);
            str.replace(md_ref, 1, ""); // remove close quote
        }

// TODO: Remove this !!!
/*      
cout << "Emitting Debug String: "
     << str
     << endl;
cout.flush();
*/
        codeOut << "!" << i << " = " << str << endl;
    }
}
