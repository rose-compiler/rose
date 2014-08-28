#ifndef SAGE3_UTF8__H
#define SAGE3_UTF8__H

/*
 * Functions used to convert from Unicode to Utf8 and vice-versa
 *
 */

#include <exception>
#include "rosedll.h"
using namespace std;

class ROSE_DLL_API Utf8 {
private:
    /**
     * Compute the code value of a Unicode character encoded in UTF8 format
     * in the array bytes starting.  "size" indicates the number of ASCII
     * characters that was required to represent the Unicode char in question.
     */
    static int getUnicodeValue(const char *bytes, int size);


public:
    /**
     * Class thrown when a Unicode character is encountered.
     */
    class BadUnicodeException : public exception {
        virtual const char* what() const throw() {
            return "Invalid Unicode character encountered";
        }
    };

    //
    // Instance of BadUnicodeException use to communicate problems.
    //
    static BadUnicodeException bad_unicode_exception;

    /**
     * Class thrown when a bad Utf8 sequence is encountered.
     */
    class BadUtf8CodeException : public exception {
        virtual const char* what() const throw() {
            return "Invalid Utf8 sequence encountered";
        }
    };

    //
    // Instance of BadUtf8CodeException use to communicate problems.
    //
    static BadUtf8CodeException bad_utf8_code_exception;

    /**
     * Compute the number of bytes that was required to store a UTF8 character
     * sequence that starts with the character c.
     */
    static int getCharSize(int val);

    /**
     * Compute the code value of a Unicode character encoded in UTF8 format
     * in the array of characters "bytes".
     */
    static int getUnicodeValue(const char *bytes);

    /**
     * Convert a unicode character into its Utf8 representation.
     */
    static string getUtf8String(int value);

    /**
     * Convert the Unicode "value" into a printable Unicode character.
     *
     */
    static string getPrintableJavaUnicodeCharacter(int value);

    /**
     * Construct a printable unicode string for Java from a given Utf8 string of characters.
     */
    static string getPrintableJavaUnicodeString(const char *str);
};

#endif
