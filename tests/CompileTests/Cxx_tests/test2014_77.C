#if ( (__GNUC__ == 4) && (__GNUC_MINOR__ == 4) )

#include <memory>
// #include <exception>        	// std::exception
#include <typeinfo>         	// std::type_info in get_deleter
// #include <bits/stl_algobase.h>  // std::swap
#include <iosfwd>           	// std::basic_ostream
#include <ext/atomicity.h>
#include <ext/concurrence.h>
// #include <bits/functexcept.h>
#include <bits/stl_function.h> 	// std::less
// #include <debug/debug.h>
#include <tr1/type_traits>

#define _GLIBCXX_INCLUDE_AS_TR1
#define _GLIBCXX_BEGIN_NAMESPACE_TR1 namespace tr1 {
#define _GLIBCXX_END_NAMESPACE_TR1 }
#define _GLIBCXX_TR1 tr1::
#include <tr1_impl/boost_sp_counted_base.h>
#include <tr1/shared_ptr.h>
#undef _GLIBCXX_TR1
#undef _GLIBCXX_END_NAMESPACE_TR1
#undef _GLIBCXX_BEGIN_NAMESPACE_TR1
#undef _GLIBCXX_INCLUDE_AS_TR1

namespace scallop {
using std::tr1::shared_ptr;
} // namespace scallop

#include <vector>

// **************************************
namespace scallop {

class MessagePackage;
class MessageProcessor;

namespace common {

class MessageProcessorData {
private:
public:

    void addMessage(shared_ptr<MessagePackage const> const& message) 
       {
#if 1
         messages.push_back(message);
#endif
       }

private:
    std::vector<shared_ptr<MessagePackage const> > messages;
};


} // namespace common
} // namespace scallop

#endif
