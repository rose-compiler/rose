// 75 matches


// Don't match
#define SgLocatedNode foobar
#define CompassAnalyses::KeywordMacro foobar

#define public foobar
int main()
  {
    #define SgProject foobar
    #define SgNode foobar

/* Compile errors: (GCC 4.1.2)
    #define and foobar
    #define and_eq foobar
    #define bitand foobar
    #define bitor foobar
    #define compl foobar
    #define not foobar
    #define not_eq foobar
    #define or foobar
    #define or_eq foobar
    #define xor foobar
    #define xor_eq foobar
rose_keywordMacroTest1.C:12:13: error: "and" cannot be used as a macro name as it is an operator in C++
rose_keywordMacroTest1.C:13:13: error: "and_eq" cannot be used as a macro name as it is an operator in C++
rose_keywordMacroTest1.C:19:9: error: "bitand" cannot be used as a macro name as it is an operator in C++
rose_keywordMacroTest1.C:20:9: error: "bitor" cannot be used as a macro name as it is an operator in C++
rose_keywordMacroTest1.C:29:9: error: "compl" cannot be used as a macro name as it is an operator in C++
rose_keywordMacroTest1.C:57:9: error: "not" cannot be used as a macro name as it is an operator in C++
rose_keywordMacroTest1.C:58:9: error: "not_eq" cannot be used as a macro name as it is an operator in C++
rose_keywordMacroTest1.C:61:9: error: "or" cannot be used as a macro name as it is an operator in C++
rose_keywordMacroTest1.C:62:9: error: "or_eq" cannot be used as a macro name as it is an operator in C++
rose_keywordMacroTest1.C:94:9: error: "xor" cannot be used as a macro name as it is an operator in C++
rose_keywordMacroTest1.C:95:9: error: "xor_eq" cannot be used as a macro name as it is an operator in C++
*/
  }
#define alignas foobar
#define alignof foobar
#define asm foobar
#define auto foobar
#define bool foobar
#define break foobar
#define case foobar
#define catch foobar
#define char foobar
#define char16_t foobar
#define char32_t foobar
#define class foobar
#define const foobar
#define constexpr foobar
#define const_cast foobar
#define continue foobar
#define decltype foobar
#define default foobar
#define delete foobar
#define doudble foobar
#define dynamic_cast foobar
#define else foobar
#define enum foobar
#define explicit foobar
#define export foobar
#define extern foobar
#define false foobar
#define float foobar
#define for foobar
#define friend foobar
#define goto foobar
#define if foobar
#define inline foobar
#define int foobar
#define long foobar
#define mutable foobar
#define namespace foobar
#define new foobar
#define noexcept foobar
#define nullptr foobar
#define operator foobar
#define private foobar
#define protected foobar
#define public foobar
#define register foobar
#define reinterpret_cast foobar
#define return foobar
#define short foobar
#define signed foobar
#define sizedof foobar
#define static foobar
#define static_assert foobar
#define static_cast foobar
#define struct foobar
#define switch foobar
#define template foobar
#define this foobar
#define thread_local foobar
#define throw foobar
#define true foobar
#define try foobar
#define typedef foobar
#define typeid foobar
#define typenam foobar
#define union foobar
#define unsigned foobar
#define using foobar
#define virtual foobar
#define void foobar
#define volatile foobar
#define wchar_t foobar
#define while foobar
