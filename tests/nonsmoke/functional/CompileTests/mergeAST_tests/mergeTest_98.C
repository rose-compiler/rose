// This demonstrates where a SgClassSymbol is not in parent's child list
namespace std
   {
     template<class _CharT> struct char_traits;
     template<> struct char_traits<char>;
   }

namespace std
   {
     template<class _CharT> struct char_traits { };
     template<> struct char_traits<char> {};
   }

