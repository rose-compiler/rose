// Test code for string utility library
#include <rose.h>

#include "StringUtility.h"
#include "Combinatorics.h"
#include <EditDistance/Levenshtein.h>
#include <EditDistance/DamerauLevenshtein.h>

#include <iostream>
#include <Sawyer/Synchronization.h>

#define check(X) ASSERT_always_require(X)
#define check2(X, MESG) ASSERT_always_require2((X), (MESG))

using namespace Rose;
using namespace Rose::StringUtility;

static std::string
got(const std::string &s) {
    return "got \"" + s + "\"";
}

static void
test_isLineTerminated() {
    check(isLineTerminated("hello world\n"));
    check(isLineTerminated("hello world\r"));
    check(isLineTerminated("hello world\r\n"));
    check(isLineTerminated("hello world\n\r"));

    check(isLineTerminated("\n"));
    check(isLineTerminated("\r"));
    check(isLineTerminated("\r\n"));
    check(isLineTerminated("\n\r"));

    check(!isLineTerminated("\nhello world"));
    check(!isLineTerminated("\rhello world"));
    check(!isLineTerminated("\r\nhello world"));
    check(!isLineTerminated("\n\rhello world"));

    check(!isLineTerminated("hello\nworld"));
    check(!isLineTerminated("hello\rworld"));
    check(!isLineTerminated("hello\r\nworld"));
    check(!isLineTerminated("hello\n\rworld"));

    check(!isLineTerminated(""));
    check(!isLineTerminated(std::string("")+'\0'));
    check(!isLineTerminated(std::string("\n")+'\0'));
    check(!isLineTerminated(std::string("\r")+'\0'));
    check(!isLineTerminated(std::string("\r\n")+'\0'));
    check(!isLineTerminated(std::string("\n\r")+'\0'));

    check(isLineTerminated('\0'+std::string("\n")));
    check(isLineTerminated('\0'+std::string("\r")));
    check(isLineTerminated('\0'+std::string("\r\n")));
    check(isLineTerminated('\0'+std::string("\n\r")));
}

static void
test_prefixLines() {
    // Tests using an empty string as input. Empty lines are never modified.
    check(prefixLines("", "L", false, false)    == "");
    check(prefixLines("", "L", true, false)     == "");
    check(prefixLines("", "L", false, true)     == "");
    check(prefixLines("", "L", true, true)      == "");

    // Tests without internal line termination.
    check(prefixLines("aaa", "L", false, false) == "aaa");
    check(prefixLines("aaa", "L", true, false)  == "Laaa");
    check(prefixLines("aaa", "L", false, true)  == "aaa");
    check(prefixLines("aaa", "L", true, true)   == "Laaa");

    // Tests with trailing line termination.
    check(prefixLines("aaa\n", "L", false, false) == "aaa\n"); // LF (unix)
    check(prefixLines("aaa\n", "L", true, false)  == "Laaa\n");
    check(prefixLines("aaa\n", "L", false, true)  == "aaa\nL");
    check(prefixLines("aaa\n", "L", true, true)   == "Laaa\nL");

    check(prefixLines("aaa\r", "L", false, false) == "aaa\r"); // CR only
    check(prefixLines("aaa\r", "L", true, false)  == "Laaa\r");
    check(prefixLines("aaa\r", "L", false, true)  == "aaa\rL");
    check(prefixLines("aaa\r", "L", true, true)   == "Laaa\rL");

    check(prefixLines("aaa\r\n", "L", false, false) == "aaa\r\n"); // CR-LF (DOS/Windows)
    check(prefixLines("aaa\r\n", "L", true, false)  == "Laaa\r\n");
    check(prefixLines("aaa\r\n", "L", false, true)  == "aaa\r\nL");
    check(prefixLines("aaa\r\n", "L", true, true)   == "Laaa\r\nL");

    // Tests with internal line termination.
    check(prefixLines("aaa\nbbb", "L", false, false) == "aaa\nLbbb"); // LF (unix)
    check(prefixLines("aaa\nbbb", "L", true, false)  == "Laaa\nLbbb");
    check(prefixLines("aaa\nbbb", "L", false, true)  == "aaa\nLbbb");
    check(prefixLines("aaa\nbbb", "L", true, true)   == "Laaa\nLbbb");

    check(prefixLines("aaa\rbbb", "L", false, false) == "aaa\rLbbb"); // CR only
    check(prefixLines("aaa\rbbb", "L", true, false)  == "Laaa\rLbbb");
    check(prefixLines("aaa\rbbb", "L", false, true)  == "aaa\rLbbb");
    check(prefixLines("aaa\rbbb", "L", true, true)   == "Laaa\rLbbb");

    check(prefixLines("aaa\r\nbbb", "L", false, false) == "aaa\r\nLbbb"); // CR-LF (DOS/Windows)
    check(prefixLines("aaa\r\nbbb", "L", true, false)  == "Laaa\r\nLbbb");
    check(prefixLines("aaa\r\nbbb", "L", false, true)  == "aaa\r\nLbbb");
    check(prefixLines("aaa\r\nbbb", "L", true, true)   == "Laaa\r\nLbbb");

    // Tests for multiple lines
    check(prefixLines("aaa\nbbb\nccc\n", "L", false, false) == "aaa\nLbbb\nLccc\n"); // LF (unix)
    check(prefixLines("aaa\nbbb\nccc\n", "L", true, false)  == "Laaa\nLbbb\nLccc\n");
    check(prefixLines("aaa\nbbb\nccc\n", "L", false, true)  == "aaa\nLbbb\nLccc\nL");
    check(prefixLines("aaa\nbbb\nccc\n", "L", true, true)   == "Laaa\nLbbb\nLccc\nL");

    check(prefixLines("aaa\rbbb\rccc\r", "L", false, false) == "aaa\rLbbb\rLccc\r"); // CR only
    check(prefixLines("aaa\rbbb\rccc\r", "L", true, false)  == "Laaa\rLbbb\rLccc\r");
    check(prefixLines("aaa\rbbb\rccc\r", "L", false, true)  == "aaa\rLbbb\rLccc\rL");
    check(prefixLines("aaa\rbbb\rccc\r", "L", true, true)   == "Laaa\rLbbb\rLccc\rL");

    check(prefixLines("aaa\r\nbbb\r\nccc\r\n", "L", false, false) == "aaa\r\nLbbb\r\nLccc\r\n"); // CR-LF
    check(prefixLines("aaa\r\nbbb\r\nccc\r\n", "L", true, false)  == "Laaa\r\nLbbb\r\nLccc\r\n");
    check(prefixLines("aaa\r\nbbb\r\nccc\r\n", "L", false, true)  == "aaa\r\nLbbb\r\nLccc\r\nL");
    check(prefixLines("aaa\r\nbbb\r\nccc\r\n", "L", true, true)   == "Laaa\r\nLbbb\r\nLccc\r\nL");

    // Tests for default arguments
    check(prefixLines("aaa\n", "L")             == "Laaa\n");
    check(prefixLines("aaa\nbbb\nccc\n", "L")   == "Laaa\nLbbb\nLccc\n");


    // Test longer prefixes
    check(prefixLines("line1\nline2\nline3\n", "___", false, false) == "line1\n___line2\n___line3\n");
    check(prefixLines("line1\nline2\nline3\n", "___", false, true)  == "line1\n___line2\n___line3\n___");
    check(prefixLines("line1\nline2\nline3\n", "___", true, false)  == "___line1\n___line2\n___line3\n");
    check(prefixLines("line1\nline2\nline3\n", "___", true, true)   == "___line1\n___line2\n___line3\n___");
}

static void
test_makeOneLine()
{
    // These test that the input does not change if it is only one line to start with.
    check(makeOneLine("hello world")            == "hello world");
    check(makeOneLine("   hello world")         == "   hello world");
    check(makeOneLine("hello world   ")         == "hello world   ");
    check(makeOneLine("hello   world")          == "hello   world");

    // These test cases with internal line termination and no white space around the line termination
    check(makeOneLine("hello\nworld")           == "hello world");
    check(makeOneLine("hello\n\nworld")         == "hello world");
    check(makeOneLine("hello\rworld")           == "hello world");
    check(makeOneLine("hello\n\rworld")         == "hello world");

    // These test line termination at the beginning and end of the string.
    check(makeOneLine("\nhello world")          == "hello world");
    check(makeOneLine("  \n  hello world")      == "hello world");
    check(makeOneLine("\n\nhello world")        == "hello world");
    check(makeOneLine("hello world\n")          == "hello world");
    check(makeOneLine("hello world  \n  ")      == "hello world");
    check(makeOneLine("hello world\n\n")        == "hello world");

    // These test multi-line cases with surrounding white space
    check(makeOneLine("hello\n  world")         == "hello world");
    check(makeOneLine("hello  \nworld")         == "hello world");
    check(makeOneLine("hello  \n  world")       == "hello world");
    check(makeOneLine("hello  \n\n world")      == "hello world");

    // These test blank lines (lines with just white space)
    check(makeOneLine("hello\n   \nworld")      == "hello world");
    check(makeOneLine("hello  \n  \n  world")   == "hello world");

    // These test user-defined replacement strings
    check(makeOneLine("hello\nworld", "[]")     == "hello[]world");
    check(makeOneLine("\nhello\n  \n  world\n", "[]") == "hello[]world");
    check(makeOneLine("hello\n\n\n\n\nworld\n\n\n", "\n") == "hello\nworld");
}

static void
test_escapes() {
    const std::string embeddedNul("abc\0xyz", 7);

    // htmlEscape
    check(htmlEscape("") == "");
    check(htmlEscape(embeddedNul) == embeddedNul);
    check(htmlEscape("<>") == "&lt;&gt;");
    check(htmlEscape("-&-") == "-&amp;-");
    check(htmlEscape("&amp;") == "&amp;amp;");
    
    // cEscape
    check(cEscape("") == "");
    check(cEscape(embeddedNul) == "abc\\000xyz");
    check(cEscape("\a") == "\\a");
    check(cEscape("\b") == "\\b");
    check(cEscape("\t") == "\\t");
    check(cEscape("\n") == "\\n");
    check(cEscape("\v") == "\\v");
    check(cEscape("\f") == "\\f");
    check(cEscape("\r") == "\\r");
    check(cEscape("\\") == "\\\\");

    // cEscape quote escapes
    check(cEscape("\"") == "\\\"");
    check(cEscape("'") == "'");
    check(cEscape('"') == "\"");
    check(cEscape('\'') == "\\'");

    check(cEscape("\"", '\'') == "\"");
    check(cEscape("'", '\'') == "\\'");
    check(cEscape('"', '"') == "\\\"");
    check(cEscape('\'', '"') == "'");

    check(cEscape("\377") == "\\377");

    check(cEscape("-\t") == "-\\t");
    check(cEscape("\t-") == "\\t-");
    check(cEscape("\t\t") == "\\t\\t");
    check(cEscape("-\t-") == "-\\t-");

    // C++ unescape
    check(cUnescape("") == "");
    check(cUnescape("a") == "a");
    check(cUnescape("a\\0") == std::string("a") + '\0');
    check(cUnescape("a\\0b") == std::string("a") + '\0' + "b");
    check(cUnescape("a\\00b") == std::string("a") + '\0' + "b");
    check(cUnescape("a\\000b") == std::string("a") + '\0' + "b");
    check(cUnescape("a\\0000b") == std::string("a") + '\0' + "0b");
    check(cUnescape("a\\'b") == "a'b");
    check(cUnescape("a\\\"b") == "a\"b");
    check(cUnescape("a\\?b") == "a?b");
    check(cUnescape("a\\ab") == "a\ab");
    check(cUnescape("a\\bb") == "a\bb");
    check(cUnescape("a\\fb") == "a\fb");
    check(cUnescape("a\\nb") == "a\nb");
    check(cUnescape("a\\rb") == "a\rb");
    check(cUnescape("a\\tb") == "a\tb");
    check(cUnescape("a\\vb") == "a\vb");
    check(cUnescape("a\\x1b") == "a\033");
    check(cUnescape("a\\x01b") == "a\033");
    check(cUnescape("a\\x00000000001b") == "a\033");
    check(cUnescape("a\\x00000000001B") == "a\033");
    check(cUnescape("a\\x00000000000g") == std::string("a") + '\0' + "g");
    check(cUnescape("a\\u1234b") == "a\\u1234b");         // unicode not implemented
    check(cUnescape("a\\U12345678b") == "a\\U12345678b"); // unicode not implemented
}

static void
test_listToString() {

    // Integer lists
    std::list<int> il;
    const std::list<int> &cil = il;

    check(listToString(cil, false)      == "");
    check(listToString(cil, true)       == "");

    il.push_back(0);
    check(listToString(cil, false)      == "0 ");
    check(listToString(cil, true)       == "0 \n");

    il.push_back(1);
    check(listToString(cil, false)      == "0 1 ");
    check(listToString(cil, true)       == "0 \n1 \n");

    // String lists
    std::list<std::string> sl;
    const std::list<std::string> &csl = sl;

    check(listToString(csl, false)      == "");
    check(listToString(csl, true)       == "");

    sl.push_back("a");
    check(listToString(csl, false)      == "a ");
    check(listToString(csl, true)       == "a \n");

    sl.push_back("b");
    check(listToString(csl, false)      == "a b ");
    check(listToString(csl, true)       == "a \nb \n");

    // String vectors (but still named "list")
    std::vector<std::string> sv;
    const std::vector<std::string> &csv = sv;

    check(listToString(csv, false)      == "");
    check(listToString(csv, true)       == "");

    sv.push_back("a");
    check(listToString(csv, false)      == "a ");
    check(listToString(csv, true)       == "a \n");

    sv.push_back("b");
    check(listToString(csv, false)      == "a b ");
    check(listToString(csv, true)       == "a \nb \n");
}

static void
test_stringToList() {
    std::list<std::string> list;

    // Empty strings
    list = stringToList("");
    check(list.empty());

    // Near-empty input
    list = stringToList("\n");
    check(list.empty());

    list = stringToList("\n\n");
    check(list.empty());

    // Single substrings
    list = stringToList("a");
    check(list.size() == 1);
    check(list.front() == "a");

    list = stringToList("a\n");
    check(list.size() == 1);
    check(list.front() == "a");

    list = stringToList("\na");
    check(list.size() == 1);
    check(list.front() == "a");

    list = stringToList("\na\n");
    check(list.size() == 1);
    check(list.front() == "a");

    // Input resulting in two substrings
    list = stringToList("a\nb");
    check(list.size() == 2);
    check(list.front() == "a");
    check(list.back() == "b");

    list = stringToList("a\nb\n");
    check(list.size() == 2);
    check(list.front() == "a");
    check(list.back() == "b");

    list = stringToList("a\n\nb\n");
    check(list.size() == 2);
    check(list.front() == "a");
    check(list.back() == "b");

    list = stringToList("\n\na\n\nb\n\n");
    check(list.size() == 2);
    check(list.front() == "a");
    check(list.back() == "b");

    // Input with white space
    list = stringToList(" ");
    check(list.size() == 1);
    check(list.front() == " ");

    list = stringToList(" \n");
    check(list.size() == 1);
    check(list.front() == " ");

    list = stringToList("\n ");
    check(list.size() == 1);
    check(list.front() == " ");

    list = stringToList("\n \n");
    check(list.size() == 1);
    check(list.front() == " ");

    list = stringToList("\n\n\t\n\b\n\n");
    check(list.size() == 2);
    check(list.front() == "\t");
    check(list.back() == "\b");

    // Input with CR-LF termination
    list = stringToList("\r\n");
    check(list.size() == 1);
    check(list.front() == "\r");
}

static void
test_numberToString() {
    check(numberToString((long long)(-1))               == "-1");
    check(numberToString((unsigned long long)1)         == "1");
    check(numberToString((long)(-1))                    == "-1");
    check(numberToString((unsigned long)1)              == "1");
    check(numberToString((int)-1)                       == "-1");
    check(numberToString((unsigned int)1)               == "1");
    check(numberToString((void*)1)                      == "0x1");

    // double uses "%2.2f"
    check(numberToString(1.0)                           == "1.00");
    check(numberToString(1.0/32)                        == "0.03");
    check(numberToString(-1.5)                          == "-1.50");
}

static void
test_numberToHexString() {
    std::string s;

    // intToHex -- only prints as many digits as necessary
    check(intToHex((uint64_t)0)                         == "0x0");
    check(intToHex((uint64_t)0xf)                       == "0xf");
    check(intToHex((uint64_t)0xfff)                     == "0xfff");
    check(intToHex((uint64_t)0xfedcba9f)                == "0xfedcba9f");
    check(intToHex((uint64_t)0xffffffffffffull)         == "0xffffffffffff");

    // toHex2 -- also prints signed and/or unsigned decimal representations
    check(toHex2(0, 8, false, false, 0)                 == "0x00");
    check(toHex2(0, 8, false, false, 0)                 == "0x00");
    check(toHex2(0, 8, true,  false, 0)                 == "0x00<0>");
    check(toHex2(0, 8, true,  true,  0)                 == "0x00<0>");

    check(toHex2(0xff, 8, false, false, 0)              == "0xff");
    check(toHex2(0xff, 8, false, true,  0)              == "0xff<-1>");
    check(toHex2(0xff, 8, true,  false, 0)              == "0xff<255>");
    check(toHex2(0xff, 8, true,  true,  0)              == "0xff<255,-1>");

    check(toHex2(0x7ffe, 15, false, false)              == "0x7ffe");
    check(toHex2(0x7ffe, 15, false, true)               == "0x7ffe<-2>");
    check(toHex2(0x7ffe, 15, true,  false)              == "0x7ffe<32766>");
    check(toHex2(0x7ffe, 15, true,  true)               == "0x7ffe<32766,-2>");

    // shortcuts
    check(toHex2(0x7ffe, 15)                            == "0x7ffe<32766,-2>");
    check2((s=unsignedToHex((uint16_t)0xfffd))          == "0xfffd<65533>", got(s));
    check2((s=signedToHex((uint16_t)0xfffd))            == "0xfffd<-3>", got(s));

    // addrToString
    check(addrToString(0)                               == "0x00000000");
    check(addrToString(0x7fffffff)                      == "0x7fffffff");
    check(addrToString(0x80000000)                      == "0x80000000");
    check(addrToString(0xffffffff)                      == "0xffffffff");
    check(addrToString(0x100000000ull)                  == "0x0000000100000000");
    check(addrToString(0x7fffffffffffffffull)           == "0x7fffffffffffffff");
    check(addrToString(0x8000000000000000ull)           == "0x8000000000000000");
    check(addrToString(0xffffffffffffffffull)           == "0xffffffffffffffff");
}

static void
test_tokenize() {
    std::list<std::string> vs;

    vs = tokenize("a:bc::def:", ':');
    check(vs.size() == 4);

    std::list<std::string>::iterator vi = vs.begin();
    check(*vi++ == "a");
    check(*vi++ == "bc");
    check(*vi++ == "");
    check(*vi++ == "def");
    check(vi == vs.end());
}

static void
test_hexadecimalToInt() {
    check(hexadecimalToInt('0') == 0);
    check(hexadecimalToInt('1') == 1);
    check(hexadecimalToInt('2') == 2);
    check(hexadecimalToInt('3') == 3);
    check(hexadecimalToInt('4') == 4);
    check(hexadecimalToInt('5') == 5);
    check(hexadecimalToInt('6') == 6);
    check(hexadecimalToInt('7') == 7);
    check(hexadecimalToInt('8') == 8);
    check(hexadecimalToInt('9') == 9);
    check(hexadecimalToInt('a') == 10);
    check(hexadecimalToInt('b') == 11);
    check(hexadecimalToInt('c') == 12);
    check(hexadecimalToInt('d') == 13);
    check(hexadecimalToInt('e') == 14);
    check(hexadecimalToInt('f') == 15);
    check(hexadecimalToInt('A') == 10);
    check(hexadecimalToInt('B') == 11);
    check(hexadecimalToInt('C') == 12);
    check(hexadecimalToInt('D') == 13);
    check(hexadecimalToInt('E') == 14);
    check(hexadecimalToInt('F') == 15);

    check(hexadecimalToInt('g') == 0);
    check(hexadecimalToInt('G') == 0);
}

static void
test_removeRedundantSubstrings() {
    check(removeRedundantSubstrings("")                 == "");
    check(removeRedundantSubstrings("\n")               == "");
    check(removeRedundantSubstrings("\n\n")             == "");
    check(removeRedundantSubstrings("a")                == "a ");
    check(removeRedundantSubstrings("a\n")              == "a ");
    check(removeRedundantSubstrings("\na")              == "a ");
    check(removeRedundantSubstrings("\na\n")            == "a ");

    check(removeRedundantSubstrings("a\nb")             == "a b ");
    check(removeRedundantSubstrings("a\nb\n")           == "a b ");
    check(removeRedundantSubstrings("\na\n\nb\n")       == "a b ");

    check(removeRedundantSubstrings(" ")                == "  ");
    check(removeRedundantSubstrings(" \n")              == "  ");
    check(removeRedundantSubstrings("\n ")              == "  ");
    check(removeRedundantSubstrings("\n \n")            == "  ");

    check(removeRedundantSubstrings("a\nb\na")          == "a b ");
    check(removeRedundantSubstrings("b\na\nb")          == "a b ");
    check(removeRedundantSubstrings("a b b")            == "a b b ");
}

static void
test_caseConversion() {
    check(convertToLowerCase("")                        == "");
    check(convertToLowerCase("abCDef")                  == "abcdef");
}

static void
test_fixLineTermination() {
    // POSIX
    check(fixLineTermination("")                        == "");
    check(fixLineTermination("\n")                      == "\n");
    check(fixLineTermination("\n\n")                    == "\n\n");
    check(fixLineTermination("a\n")                     == "a\n");
    check(fixLineTermination("\na")                     == "\na");
    check(fixLineTermination("\na\n")                   == "\na\n");
    check(fixLineTermination("a\nb")                    == "a\nb");
    check(fixLineTermination("a\nb\n")                  == "a\nb\n");
    check(fixLineTermination("\na\nb")                  == "\na\nb");
    check(fixLineTermination("\na\nb\n")                == "\na\nb\n");

    // Microsoft and most other early OS neither Unix nor IBM
    check(fixLineTermination("")                        == "");
    check(fixLineTermination("\r\n")                    == "\n");
    check(fixLineTermination("\r\n\r\n")                == "\n\n");
    check(fixLineTermination("a\r\n")                   == "a\n");
    check(fixLineTermination("\r\na")                   == "\na");
    check(fixLineTermination("\r\na\r\n")               == "\na\n");
    check(fixLineTermination("a\r\nb")                  == "a\nb");
    check(fixLineTermination("a\r\nb\r\n")              == "a\nb\n");
    check(fixLineTermination("\r\na\r\nb")              == "\na\nb");
    check(fixLineTermination("\r\na\r\nb\r\n")          == "\na\nb\n");

    // Acorn BBC and RISC OS
    check(fixLineTermination("")                        == "");
    check(fixLineTermination("\n\r")                    == "\n");
    check(fixLineTermination("\n\r\n\r")                == "\n\n");
    check(fixLineTermination("a\n\r")                   == "a\n");
    check(fixLineTermination("\n\ra")                   == "\na");
    check(fixLineTermination("\n\ra\n\r")               == "\na\n");
    check(fixLineTermination("a\n\rb")                  == "a\nb");
    check(fixLineTermination("a\n\rb\n\r")              == "a\nb\n");
    check(fixLineTermination("\n\ra\n\rb")              == "\na\nb");
    check(fixLineTermination("\n\ra\n\rb\n\r")          == "\na\nb\n");

    // Mac OS up to version 9
    check(fixLineTermination("")                        == "");
    check(fixLineTermination("\r")                      == "\n");
    check(fixLineTermination("\r\r")                    == "\n\n");
    check(fixLineTermination("a\r")                     == "a\n");
    check(fixLineTermination("\ra")                     == "\na");
    check(fixLineTermination("\ra\r")                   == "\na\n");
    check(fixLineTermination("a\rb")                    == "a\nb");
    check(fixLineTermination("a\rb\r")                  == "a\nb\n");
    check(fixLineTermination("\ra\rb")                  == "\na\nb");
    check(fixLineTermination("\ra\rb\r")                == "\na\nb\n");
}






// WARNING: This "test" doesn't really test anything. It just calls the functions and spits out results without checking that
//          the results are valid.
static bool
stringTest(std::string s)
{
    std::string s_filename = StringUtility::stripPathFromFileName(s);
    std::string s_path     = StringUtility::getPathFromFileName(s);
    std::string s_nosuffix = StringUtility::stripFileSuffixFromFileName(s);
    printf("s = %s s_filename = %s \n",s.c_str(),s_filename.c_str());
    printf("s = %s s_path     = %s \n",s.c_str(),s_path.c_str());
    printf("s = %s s_nosuffix = %s \n",s.c_str(),s_nosuffix.c_str());
    printf("\n");
    return true;
}

// Here's another implementation of Damerau-Levenshtein edit distance which we can use to test the one in ROSE.
// https://github.com/ugexe/Text--Levenshtein--Damerau--XS/blob/master/damerau-int.c
namespace DamerauLevenshtein2 {
struct dictionary {
    unsigned int key;
    unsigned int value;
  struct dictionary* next;
};
typedef struct dictionary item;

static __inline item* push(unsigned int key,item* curr) {
    item* head;
    head = new item;
    head->key = key;
    head->value = 0;
    head->next = curr;
    return head;
}

static __inline item* find(item* head,unsigned int key) {
    item* iterator = head;
    while (iterator) {
        if (iterator->key == key) {
            return iterator;
        }
        iterator = iterator->next;
    }
    return NULL;
}

static __inline item* uniquePush(item* head,unsigned int key) {
    item* iterator = head;
    while (iterator) {
        if (iterator->key == key) {
            return head;
        }
        iterator = iterator->next;
    }
    return push(key,head);
}

static void dict_free(item* head) {
    item* iterator = head;
    while (iterator) {
        item* temp = iterator;
        iterator = iterator->next;
        delete temp;
    }
    head = NULL;
}

static int distance(const unsigned int *src, const unsigned int *tgt, unsigned int x, unsigned int y, unsigned int maxDistance)
{
    item *head = NULL;
    unsigned int swapCount, swapScore, targetCharCount, i, j;
    unsigned int *scores = new unsigned int[(x+2)*(y+2)];
    unsigned int score_ceil = x + y;
    unsigned int curr_score = 0;
    unsigned int diff = x > y ? x - y : y - x;

    if (maxDistance != 0 && diff > maxDistance) {
        delete[] scores;
        return -1;
    }
    
    /* intialize matrix start values */
    scores[0] = score_ceil;
    scores[1 * (y + 2) + 0] = score_ceil;
    scores[0 * (y + 2) + 1] = score_ceil;
    scores[1 * (y + 2) + 1] = 0;
    head = uniquePush(uniquePush(head,src[0]),tgt[0]);

    /* work loops */
    /* i = src index */
    /* j = tgt index */
    for (i=1;i<=x;i++) {
        if (i < x)
            head = uniquePush(head,src[i]);
        scores[(i+1) * (y + 2) + 1] = i;
        scores[(i+1) * (y + 2) + 0] = score_ceil;
        swapCount = 0;

        for (j=1;j<=y;j++) {
            if (i == 1) {
                if (j < y)
                    head = uniquePush(head,tgt[j]);
                scores[1 * (y + 2) + (j + 1)] = j;
                scores[0 * (y + 2) + (j + 1)] = score_ceil;
            }
            curr_score = 0;

            targetCharCount = find(head,tgt[j-1])->value;
            swapScore = scores[targetCharCount * (y + 2) + swapCount] + i - targetCharCount - 1 + j - swapCount;

            if (src[i-1] != tgt[j-1]) {
                scores[(i+1) * (y + 2) + (j + 1)] = std::min(swapScore,
                                                             std::min(scores[i * (y + 2) + j],
                                                                      std::min(scores[(i+1) * (y + 2) + j],
                                                                               scores[i * (y + 2) + (j + 1)])) + 1);
            } else {
                swapCount = j;
                scores[(i+1) * (y + 2) + (j + 1)] = std::min(scores[i * (y + 2) + j], swapScore);
            }

            curr_score = std::min(curr_score, scores[(i+1) * (y+2) + (j+1)]);
        }

        if (maxDistance != 0 && curr_score > maxDistance) {
            dict_free(head);
            delete[] scores;
            return -1;
        }

        find(head,src[i-1])->value = i;
    }

    unsigned int score = scores[(x+1) * (y + 2) + (y + 1)];
    dict_free(head);
    delete[] scores;
    return (maxDistance != 0 && maxDistance < score) ? -1 : score;
}
} // namespace

// Here's another implementation of Levenshtein edit distance so we can test the one in ROSE.
namespace Levenshtein2 {
template <class T>
unsigned int edit_distance(const T& s1, const T& s2)
{
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<unsigned int> > d(len1 + 1, std::vector<unsigned int>(len2 + 1));
 
    d[0][0] = 0;
    for (unsigned int i = 1; i <= len1; ++i)
        d[i][0] = i;
    for (unsigned int i = 1; i <= len2; ++i)
        d[0][i] = i;
 
    for (unsigned int i = 1; i <= len1; ++i) {
        for (unsigned int j = 1; j <= len2; ++j) {
            d[i][j] = std::min(std::min(d[i - 1][j] + 1,d[i][j - 1] + 1),
                               d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1));
        }
    }
    return d[len1][len2];
}
} // namespace

static bool
test_edit_distance()
{
    static const size_t sz_delta = 5;                   // max difference in lengths of vectors (v1 is longer than v2)
    static const size_t sz_max = 20;                    // maximum length of vectors
    static const unsigned int elmt_modulo = 8;          // modulo for choosing elements for the two vectors
    static const bool show_every_run = true;            // set if you want to see every test
    size_t nfailures = 0;

    // The first of two vectors
    for (size_t sz1=sz_delta; sz1<sz_max; ++sz1) {
        std::vector<unsigned int> v1(sz1, 0);
        for (size_t i=0; i<sz1; ++i)
            v1[i] = Sawyer::fastRandomIndex(elmt_modulo);

        // The second of two vectors.
        for (size_t sz2=sz1-sz_delta; sz2<sz1; ++sz2) {
            std::vector<unsigned int> v2(sz2, 0);
            for (size_t i=0; i<sz2; ++i)
                v2[i] = v1[i];
            Combinatorics::shuffle(v2);

            if (show_every_run) {
                std::cerr <<"using these vectors:\n"
                          <<"    v1[" <<sz1 <<"] = {";
                for (size_t i=0; i<sz1; ++i)
                    std::cerr <<" " <<v1[i];
                std::cerr <<"}\n"
                          <<"    v2[" <<sz2 <<"] = {";
                for (size_t i=0; i<sz2; ++i)
                    std::cerr <<" " <<v2[i];
                std::cerr <<"}\n";
            }
                

            for (size_t algo=0; algo<2; ++algo) {
                size_t d1, d2;
                const char *name;
                switch (algo) {
                    case 0:
                        name = "Levenshtein";
                        d1 = EditDistance::levenshteinDistance(v1, v2);
                        d2 = Levenshtein2::edit_distance(v1, v2);
                        break;
                    case 1:
                        name = "Damerau-Levenshtein";
                        d1 = EditDistance::damerauLevenshteinDistance(v1, v2);
                        if (v1.empty())
                            v1.push_back(911); // we need a pointer, and &v1[0] won't cut it if v1 is empty
                        if (v2.empty())
                            v2.push_back(911);
                        d2 = DamerauLevenshtein2::distance(&v1[0], &v2[0], sz1, sz2, 0);
                        v1.resize(sz1);
                        v2.resize(sz2);
                        break;
                }

                if (d1!=d2) {
                    std::cerr <<"failure for " <<name <<" edit distance:\n"
                              <<"    v1[" <<sz1 <<"] = {";
                    for (size_t i=0; i<sz1; ++i)
                        std::cerr <<" " <<v1[i];
                    std::cerr <<"}\n"
                              <<"    v2[" <<sz2 <<"] = {";
                    for (size_t i=0; i<sz2; ++i)
                        std::cerr <<" " <<v2[i];
                    std::cerr <<"}\n"
                              <<"    rose implementation: distance=" <<d1 <<"\n"
                              <<"    test implementation: disatnce=" <<d2 <<"\n";
                    ++nfailures;
                } else if (show_every_run) {
                    std::cerr <<"    " <<name <<"\n"
                              <<"        ROSE=" <<d1 <<", alternate=" <<d2 <<"\n";
                }
            }
        }
    }
    return 0==nfailures;
}


            


int
main()
{
    
    test_isLineTerminated();
    test_prefixLines();
    test_makeOneLine();
    test_escapes();
    test_listToString();
    test_stringToList();
    test_numberToString();
    test_numberToHexString();
    test_tokenize();
    test_hexadecimalToInt();
    test_removeRedundantSubstrings();
    test_caseConversion();
    test_fixLineTermination();

    size_t nfailures = 0;

    nfailures += stringTest("foo.h") ? 0 : 1;
    nfailures += stringTest("/foo.h") ? 0 : 1;
    nfailures += stringTest("//foo.h") ? 0 : 1;
    nfailures += stringTest("///foo.h") ? 0 : 1;
    nfailures += stringTest("////foo.h") ? 0 : 1;
    nfailures += stringTest("./foo.h") ? 0 : 1;
    nfailures += stringTest("../foo.h") ? 0 : 1;
    nfailures += stringTest("//foo.h") ? 0 : 1;
    nfailures += stringTest("path/foo.h") ? 0 : 1;
    nfailures += stringTest("/path/foo.h") ? 0 : 1;
    nfailures += stringTest("/pathA/pathB/foo.h") ? 0 : 1;
    nfailures += stringTest("foo") ? 0 : 1;
    nfailures += stringTest("/path/foo") ? 0 : 1;

    nfailures += test_edit_distance() ? 0 : 1;

    return 0==nfailures ? 0 : 1;
}
