#ifndef SAGE3_UTF8__H
#define SAGE3_UTF8__H

/*
 * Functions used to convert from Unicode to Utf8 and vice-versa
 *
 */

#include <string.h>
#include <exception>
#include <iostream>
#include <sstream>
//#include "rosedll.h"

using namespace std;

class /* ROSE_DLL_API */ Utf8 {
public:
    enum Char_Kind {
        CHAR8,
        CHAR16,
        CHAR32,
        UNKNOWN
    };
    
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
     * Class thrown when there is an attempt to get a string of the wrong kind.
     */
    class WrongCharKindException : public exception {
        virtual const char* what() const throw() {
            return msg.c_str();
        }

    public:
	string msg;

        WrongCharKindException() {
            msg = "Mismatched character types";
        }

        void setMsg(Char_Kind char_kind, Char_Kind bad_kind) {
            ostringstream excep;
            excep << "A character of type "
                  << (bad_kind == CHAR8 ? "CHAR8"
                                        : bad_kind == CHAR16 ? "CHAR16"
                                                             : bad_kind == CHAR32 ? "CHAR32"
                                                                                  : "UNKNOWN")
                  << " was found in a context where a character compatible with type "
                  << (char_kind == CHAR8 ? "CHAR8"
                                         : char_kind == CHAR16 ? "CHAR16"
                                                               : char_kind == CHAR32 ? "CHAR32"
                                                                                     : "UNKNOWN")
                  << " was expected.";
            msg = excep.str();
        }
    };

    //
    // Instance of WrongCharKindException use to communicate attempt to retrieve a string with the wrong kind.
    //
    static WrongCharKindException wrong_char_kind_exception;

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

    string getUtf8();
    string getChar();
    u16string getChar16();
    u32string getChar32();

    Utf8(string utf8_, Char_Kind char_kind_) : utf8(utf8_),
                                               char_kind(char_kind_),
                                               size(0)
    {
    }
    /**
     * Construct a Utf8 string for str with len characters.
     */
    Utf8(char *str_, int length_, Char_Kind char_kind_) : char_kind(char_kind_),
                                                          size(0)
    {
        constructUtf8String(str_, length_);
    }

    /**
     * Construct a Utf8 string for str.
     */
    Utf8(char *str_, Char_Kind char_kind_) : Utf8(str_, strlen(str_), char_kind_){
    }
    
private:
    /**
     * Compute the code value of a Unicode character encoded in UTF8 format
     * in the array bytes starting.  "size" indicates the number of ASCII
     * characters that was required to represent the Unicode char in question.
     */
    static int getUnicodeValue(const char *bytes, int size);

    /**
     *
     */
    void constructUtf8String(char *str, int length);
    
    Char_Kind char_kind;
    size_t size;
    string utf8;
};

#endif
