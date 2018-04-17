template<typename _StateT>
class fpos
   {
     public:
          long operator-(const fpos& __other) const { return __other._M_off; }
    };

class ios_base
   {
     protected:
          int _M_word_size;
   };

template<typename _CharT, typename _Traits>
class basic_ios : public ios_base
   {
     public:
          basic_ios& copyfmt(const basic_ios& __rhs);
   };

template<typename _CharT, typename _Traits>
basic_ios<_CharT, _Traits>&
basic_ios<_CharT, _Traits>::copyfmt(const basic_ios& __rhs)
   {
     int* __words = new int[__rhs._M_word_size];

     return *this;
   }

