#include <iostream>
#include <sstream>
#include <string.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */
#include "Utf8.h"

// Define static member variables
Utf8::BadUnicodeException Utf8::bad_unicode_exception;
Utf8::BadUtf8CodeException Utf8::bad_utf8_code_exception;

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

    if (code < 0x0000 || code > 0xFFFF) // restrict this to UTF-16 for now!
        throw bad_unicode_exception;

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
    if (value < 0x0000 || value > 0xFFFF) // restrict this to UTF-16 for now!
        throw bad_unicode_exception;

    string result;

    if (value == 0) {
         result = (char) 0xC0;
         result += (char) 0x80;
    }
    else if (value < 0x0080)
         result = (char) value;
    else if (value < 0x0800) {
        result = (char) (0x000000C0 | ((value >> 6) & 0x0000001F));
        result += (char) (0x00000080 | (value & 0x0000003F));
    }
    else {
        result = (char) (0x000000E0 | ((value >> 12) & 0x0000000F));
        result += (char) (0x00000080 | ((value >> 6) & 0x0000003F));
        result += (char) (0x00000080 | (value & 0x0000003F));
    }

    return result;
}

/**
 * Convert the Unicode "value" into a printable Unicode character.
 */
string Utf8::getPrintableJavaUnicodeCharacter(int value) {
    if (value < 0x0000 || value > 0xFFFF) // restrict this to UTF-16 for now!
        throw bad_unicode_exception;

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

// TODO: Remove this !
/*
cout << "Input string: \"" << str << "\" with length " << strlen(str) << "; ";
cout.flush();
*/
    int size;
    for (const char *p = str; *p != 0; p += size) {
        size = getCharSize(*p);
        int value = getUnicodeValue(p, size);
// TODO: Remove this !
/*
cout << "   found character of size " << size << " with value " << hex << value << endl;
cout.flush();
*/
        result += getPrintableJavaUnicodeCharacter(value);
    }
// TODO: Remove this !
/*
cout << "Output string: \"" << result << "\" with length " << result.size() << "."  << endl;
cout.flush();
*/

    return result;
}
