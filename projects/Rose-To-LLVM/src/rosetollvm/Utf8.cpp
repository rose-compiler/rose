#include <vector>
#include <assert.h>
#include "rosetollvm/Utf8.h"

// Define static member variables
Utf8::BadUnicodeException Utf8::bad_unicode_exception;
Utf8::BadUtf8CodeException Utf8::bad_utf8_code_exception;
Utf8::WrongCharKindException Utf8::wrong_char_kind_exception;

/**
 * Compute the code value of a Unicode character encoded in UTF8 format
 * in the array bytes starting.  "size" indicates the number of ASCII
 * characters that was required to represent the Unicode char in question.
 *
 * This is a PRIVATE method.
 *
 */
int Utf8::getUnicodeValue(const char *bytes, int size) {
    int code = bytes[0];
    switch(size) {
        case 0: // Bad Utf8 character sequence
             throw bad_utf8_code_exception;
             break;
        case 1: // ASCII character
             break;
        default: 
        {
             code &= (0xFF >> (size + 1));
             for (int k = 1; k < size; k++) {
                 int c = bytes[k];
                 if ((c & 0x000000C0) != 0x80) { // invalid UTF8 character?
                     throw bad_utf8_code_exception;
                 }
                 code = (code << 6) + (c & 0x0000003F);
             }
             break;
        }
    }

    return code;
}

/**
 * Compute the number of bytes that was required to store a UTF8 character
 * sequence that starts with the character c.
 */
int Utf8::getCharSize(int val) {
    unsigned char c = (val & 0x000000FF);

    //
    // The base Ascii characters
    //
    if (c >= 0 && c < 0x80)
        return 1;

    //
    // A character with a bit sequence in the range:
    // 
    //    0B10000000..0B10111111
    //
    // cannot be a leading UTF8 character.
    //
    if (c >= 0x80 && c < 0xC0) {
        throw bad_utf8_code_exception;
    }

    //
    // A leading character in the range 0xC0..0xDF
    //
    //    0B11000000..0B11011111
    //
    // identifies a two-bytes sequence
    //
    if (c >= 0xC0 && c < 0xE0)
        return 2;

    //
    // A leading character in the range 0xE0..0xEF
    //
    //    0B11100000..0B11101111
    //
    // identifies a three-bytes sequence
    //
    if (c >= 0xE0 && c < 0xF0)
        return 3;

    //
    // A leading character in the range 0xF0..0xF7
    //
    //    0B11110000..0B11110111
    //
    // identifies a four-bytes sequence
    //
    if (c >= 0xF0 && c < 0xF8)
        return 4;

    //
    // A leading character in the range 0xF8..0xFB
    //
    //    0B11111000..0B11111011
    //
    // identifies a five-bytes sequence
    //
    if (c >= 0xF8 && c < 0xFC)
        return 5;

    //
    // A leading character in the range 0xFC..0xFD
    //
    //    0B11111100..0B11111101
    //
    // identifies a six-bytes sequence
    //
    if (c >= 0xFC && c < 0xFE)
        return 6;

    //
    // The characters 
    //
    //    0B11111110 and 0B11111111
    //
    // are not valid leading UTF8 characters as they would indicate
    // a sequence of 7 characters which is not possible.
    //
    // c >= 0xFE && c < 0xFF
    throw bad_utf8_code_exception;
}

/**
 * Compute the code value of a Unicode character encoded in UTF8 format
 * in the array of characters "bytes".
 */
int Utf8::getUnicodeValue(const char *bytes) {
    return getUnicodeValue(bytes, getCharSize(bytes[0]));
}

/**
 * Convert a unicode character into its Utf8 representation.
 */
string Utf8::getUtf8String(int value) {
    string result;

    if (value < 0x0080) { // 1-byte char
        result = (char) value;
    }
    else if (value < 0x0800) { // 2-bytes char
        result =  (char) (0x000000C0 | ((value >> 6) & 0x0000001F));
        result += (char) (0x00000080 | (value & 0x0000003F));
    }
    else if (value < 0x10000) { // 3-bytes char
        result =  (char) (0x000000E0 | ((value >> 12) & 0x0000000F));
        result += (char) (0x00000080 | ((value >> 6)  & 0x0000003F));
        result += (char) (0x00000080 | (value & 0x0000003F));
    }
    else if (value < 0x200000) { // 4-bytes char
        result =  (char) (0x000000F0 | ((value >> 18) & 0x00000007));
        result += (char) (0x00000080 | ((value >> 12) & 0x0000003F));
        result += (char) (0x00000080 | ((value >> 6)  & 0x0000003F));
        result += (char) (0x00000080 | (value & 0x0000003F));
    }
    else if (value < 0x4000000) { // 5-bytes char
        result =  (char) (0x000000F8 | ((value >> 24) & 0x00000003));
        result += (char) (0x00000080 | ((value >> 18) & 0x0000003F));
        result += (char) (0x00000080 | ((value >> 12) & 0x0000003F));
        result += (char) (0x00000080 | ((value >> 6)  & 0x0000003F));
        result += (char) (0x00000080 | (value & 0x0000003F));
    }
    else { // 6-bytes char
        result =  (char) (0x000000FC | ((value >> 30) & 0x00000001));
        result += (char) (0x00000080 | ((value >> 24) & 0x0000003F));
        result += (char) (0x00000080 | ((value >> 18) & 0x0000003F));
        result += (char) (0x00000080 | ((value >> 12) & 0x0000003F));
        result += (char) (0x00000080 | ((value >> 6)  & 0x0000003F));
        result += (char) (0x00000080 | (value & 0x0000003F));
    }

    return result;        
}

/**
 * Convert the Unicode "value" into a printable Unicode character.
 */
string Utf8::getPrintableJavaUnicodeCharacter(int value) {
    string result;

    if (value < 0x0100) { // Extended Ascii characters:   00..FF ?
        if (value == '\b') {
            result = "\\b";
        }
        else if (value == '\t') {
            result = "\\t";
        }
        else if (value == '\n') {
            result = "\\n";
        }
        else if (value == '\f') {
            result = "\\f";
        }
        else if (value == '\r') {
            result = "\\r";
        }
        else if (value == '\"') {
            result = "\\\"";
        }
        else if (value == '\'') {
            result = "\\\'";
        }
        else if (value == '\\') {
            result = "\\\\";
        }
        else if (isprint(value)) {
            result = (char) value;
        }
        else {
            ostringstream octal; // stream used for the conversion
            octal.fill('0');
            octal.width(3);
            octal << oct << value; // compute the octal character representation of the value
            result = "\\";
            result += octal.str();
        }
    }
    else {
        ostringstream hexadecimal; // stream used for the conversion
        hexadecimal.fill('0');
        hexadecimal.width(4);
        hexadecimal << hex << value; // compute the Unicode character representation of the value
        result = "\\u";
        result += hexadecimal.str();
    }

    return result;
}

/**
 * Construct a printable unicode string from a given Utf8 string of characters.
 */
string Utf8::getPrintableJavaUnicodeString(const char *str) {
    string result = "";

    int size;
    for (const char *p = str; *p != 0; p += size) {
        size = getCharSize(*p);
        int value = getUnicodeValue(p, size);
        result += getPrintableJavaUnicodeCharacter(value);
    }

    return result;        
}


string Utf8::getUtf8() {
    return utf8;
}

string Utf8::getChar() {
    if (char_kind != CHAR8) {
// TODO: Remove this !
/*  
cout << "I am here !" << endl; cout.flush();      
*/
        throw wrong_char_kind_exception;
    }

    string result;

    const char *c_str = utf8.c_str();
    int num_bytes;
    for (int k = 0; k < utf8.size(); k += num_bytes) {
        const char *p = &(c_str[k]);
        num_bytes = getCharSize(*p);
        int value = getUnicodeValue(p, num_bytes);
        if (value > 0xFF) { // value greater than a byte (0xFF)?
            for (int i = 0; i < num_bytes; i++) { // Use the original utf8 substring.
                result += p[i];
            }
        }
        else result += (char) value;
    }

    return result;        
}

u16string Utf8::getChar16() {
    if (char_kind != CHAR16) {
        throw wrong_char_kind_exception;
    }

    u16string result;

    const char *c_str = utf8.c_str();
    int num_bytes;
    for (int k = 0; k < utf8.size(); k += num_bytes) {
        const char *p = &(c_str[k]);
        num_bytes = getCharSize(*p);
        int value = getUnicodeValue(p, num_bytes);
        if (value > 0xFFFF) { // value greater than 16-bit char (0xFFFF)?
            throw bad_unicode_exception;
        }
        else result += (char16_t) value;
    }

    return result;        
}

u32string Utf8::getChar32() {
    if (char_kind != CHAR32) {
        throw wrong_char_kind_exception;
    }

    u32string result;

// TODO: Remove this !
/*  
cout << "*** Outputting CHAR32: " << endl;
*/
    const char *c_str = utf8.c_str();
    int num_bytes;
    for (int k = 0; k < utf8.size(); k += num_bytes) {
        const char *p = &(c_str[k]);
        num_bytes = getCharSize(*p);
        int value = getUnicodeValue(p, num_bytes);
// TODO: Remove this !
/*  
cout << " 0x" << hex << value
     << endl;
cout.flush();
*/

        result += (char32_t) value;
    }

    return result;        
}


void Utf8::constructUtf8String(char *str, int length) {
// TODO: Remove this !
/*  
vector<int> codes;
cout << "Constructing string \"" << str << "\"" << endl;
*/
    size = 0;
    
    for (int i = 0; i < length; i++) {
        size++; // Each time we enter this loop, one character is processed.

        int char_value = ((int) str[i]) & 0x000000FF;
// TODO: Remove this !
/*  
cout << "*** At position " << i
     << "; str[" << i << "] = " << ((unsigned) str[i])
     << " with Char value " << char_value
     << endl;
*/
        if (char_value < 0x80) { // an ASCII character?
// TODO: Remove this !
/*  
cout << "    found an ASCII character" <<  endl;
*/
            if (str[i] == '\\' && i + 1 < length) {
// TODO: Remove this !
/*  
cout << "    found a backslash" <<  endl;
*/
                if (str[i + 1] == 'n') {
                    i++;
                    char_value = (int) '\n';
                }
                else if (str[i + 1] == 't') {
                    i++;
                    char_value = (int) '\t';
                }
                else if (str[i + 1] == 'b') {
                    i++;
                    char_value = (int) '\b';
                }
                else if (str[i + 1] == 'r') {
                    i++;
                    char_value = (int) '\r';
                }
                else if (str[i + 1] == 'f') {
                    i++;
                    char_value = (int) '\f';
                }
                else if (str[i + 1] == 'v') {
                    i++;
                    char_value = (int) '\v';
                }
                else if (str[i + 1] == 'a') {
                    i++;
                    char_value = (int) '\a';
                }
                else if (str[i + 1] == '?') {
                    i++;
                    char_value = (int) '\?';
                }
                else if (str[i + 1] == '\"') {
                    i++;
                    char_value = (int) '\"';
                }
                else if (str[i + 1] == '\'') {
                    i++;
                    char_value = (int) '\'';
                }
                else if (str[i + 1] == '\\') {
                    i++;
                    char_value = (int) '\\';
                }
                else if (str[i + 1] >= '0' && str[i + 1] < '8') { // Character specified as Octal sequence of 1-3 digits.
                    i++;
                    char_value = (str[i] - '0'); // the first digit
                    if (i + 1 < length  && // If we've not reached the end of the string
                        (str[i + 1] >= '0' && str[i + 1] < '8')) {
                        i++;
                        char_value = char_value * 7 + (str[i] - '0'); // the second digit
                        if (i + 1 < length  && // If we've not reached the end of the string
                            (str[i + 1] >= '0' && str[i + 1] < '8')) {
                            i++;
                            char_value = char_value * 7 + (str[i] - '0'); // the third digit
                        }
                    }
                }
                else if (str[i + 1] == 'x') { // Character specified as hexadecimal sequence
                    i++;
                    long long value = 0;
                    while(i + 1 < length) {
                        if (str[i + 1] >= '0' && str[i + 1] <= '9') {
                            i++;
                            value = value * 16 + (str[i] - '0');
                        }
                        else if (str[i + 1] >= 'A' && str[i + 1] <= 'F') {
                            i++;
                            value = value * 16 + (str[i] - 'A' + 10);
                        }
                        else if (str[i + 1] >= 'a' && str[i + 1] <= 'f') {
                            i++;
                            value = value * 16 + (str[i] - 'a' + 10);
                        }
                    }

                    if (value <= 0x7FFFFFFF) {
                        char_value = value;		      
                    }
		    else {
                        wrong_char_kind_exception.setMsg(char_kind, UNKNOWN);
                        throw wrong_char_kind_exception;
                    }
                }
                else if (str[i + 1] == 'u') { // Characters specified as 4-digits hexadecimal sequence
                    i++;
                    char_value = 0;
                    for (int k = 0; (k < 4) && (i + 1 < length); k++) {
                        if (str[i + 1] >= '0' && str[i + 1] <= '9') {
                            i++;
                            char_value = char_value * 16 + (str[i] - '0');
                        }
                        else if (str[i + 1] >= 'A' && str[i + 1] <= 'F') {
                            i++;
                            char_value = char_value * 16 + (str[i] - 'A' + 10);
                        }
                        else if (str[i + 1] >= 'a' && str[i + 1] <= 'f') {
                            i++;
                            char_value = char_value * 16 + (str[i] - 'a' + 10);
                        }
                    }
                }
                else if (str[i + 1] == 'U') { // Character specified as 8-digits hexadecimal sequence
                    i++;
                    char_value = 0;
                    for (int k = 0; (k < 8) && (i + 1 < length); k++) {
                        if (str[i + 1] >= '0' && str[i + 1] <= '9') {
                            i++;
                            char_value = char_value * 16 + (str[i] - '0');
                        }
                        else if (str[i + 1] >= 'A' && str[i + 1] <= 'F') {
                            i++;
                            char_value = char_value * 16 + (str[i] - 'A' + 10);
                        }
                        else if (str[i + 1] >= 'a' && str[i + 1] <= 'f') {
                            i++;
                            char_value = char_value * 16 + (str[i] - 'a' + 10);
                        }
                    }
                }
                else {
                    assert(false);
                }
// TODO: Remove this !
/*  
codes.push_back(char_value);
*/

                utf8 += getUtf8String(char_value);
            }
            else if (isprint(str[i])) { // a printable character?
// TODO: Remove this !
/*  
codes.push_back(str[i]);
*/
                utf8 += str[i];
            }
            else { // an unprintable ASCII character
// TODO: Remove this !
/*  
codes.push_back(char_value);
*/
                utf8 += getUtf8String(char_value);
            }
        }
        else { // an Extended ASCII character
// TODO: Remove this !
/*  
codes.push_back(char_value);
*/
            utf8 += getUtf8String(char_value);
        }
    }

// TODO: Remove this !
/*  
    cout << "String with " << codes.size() << " elements." << endl;
*/
}
