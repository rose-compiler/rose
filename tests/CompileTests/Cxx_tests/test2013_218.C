
#include <string>
using namespace std;


#if 0
// class string {};
template<typename _Alloc> class allocator;
template<class _CharT> struct char_traits;
template<typename _CharT, typename _Traits = char_traits<_CharT>, typename _Alloc = allocator<_CharT> > class basic_string {};
template<> struct char_traits<char>;
typedef basic_string<char> string;
#endif




class Message {};
class FieldDescriptor;

class GeneratedMessageReflection
   {
     public:
          string GetString(const Message& message,const FieldDescriptor* field) const;

          template <typename Type> inline const Type& GetField(const Message& message,const FieldDescriptor* field) const;
   };



string GeneratedMessageReflection::GetString(const Message& message, const FieldDescriptor* field) const
   {
     switch (42)
        {
          default:
               return *GetField<const string*>(message, field);
        }
   }

