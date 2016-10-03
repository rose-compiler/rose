template<typename _Alloc> class allocator;

template<class _CharT> struct char_traits;

template<typename _CharT, typename _Traits = char_traits<_CharT>,typename _Alloc = allocator<_CharT> > class basic_string;

typedef basic_string<char>    string;

template<typename _Tp>
class new_allocator
   {
     public:
          new_allocator(const new_allocator&) throw() { }
   };

template<typename _Tp> class allocator : public new_allocator<_Tp> {};

template<typename _CharT, typename _Traits, typename _Alloc>
class basic_string
   {
     struct _Alloc_hider : _Alloc {};
     mutable _Alloc_hider	_M_dataplus;
  };

class GeneratedMessageReflection
   {
     public:
          string GetString() const;

          template <typename Type> inline const Type& GetField() const;
   };

string GeneratedMessageReflection::GetString() const
   {
  // BUG: This unparses to: return ( *(this) -> GetField< * > ());
     return *GetField<const string*>();
   }

