#include <map>
#include <memory>
#include <functional>
#include <typeindex>
#include <iostream>

namespace {

// The base type that is stored in the collection.
struct Func_t
   {
     virtual ~Func_t() = default;
   };

// The map that stores the callbacks.
using callbacks_t = std::map<std::type_index, std::unique_ptr<Func_t>>;
callbacks_t callbacks;

// The derived type that represents a callback.
template<typename ...A>
struct Cb_t : public Func_t 
   {
     using cb = std::function<void(A...)>;
     cb callback;
     Cb_t(cb p_callback) : callback(p_callback) {}
   };

// Wrapper function to call the callback stored at the given index with the
// passed argument.
template<typename ...A>
void call(std::type_index index, A&& ... args)
   {
     using func_t = Cb_t<A...>;
     using cb_t = std::function<void(A...)>;
     const Func_t& base = *callbacks[index];
     const cb_t& fun = static_cast<const func_t&>(base).callback;
     fun(std::forward<A>(args)...);
   }

} // end anonymous namespace

void foo1()
   {
     std::cout << "foo1 is called.\n";
   }

void foo2(int i)
   {
     std::cout << "foo2 is called with: " << i << "\n";
   }

void foo3(std::string s, int i)
   {
     std::cout << "foo3 is called with: " << s << " and: " << i << "\n";
   }

int main()
   {
  // Define our functions.
     using func1 = Cb_t<>;
     std::unique_ptr<func1> f1(new func1(&foo1));
     using func2 = Cb_t<int>;
     std::unique_ptr<func2> f2(new func2(&foo2));
     using func3 = Cb_t<std::string, int>;
     std::unique_ptr<func3> f3(new func3(&foo3));

  // Add the to the map.
     std::type_index index1(typeid(f1));
     std::type_index index2(typeid(f2));
     std::type_index index3(typeid(f3));
     callbacks.insert(callbacks_t::value_type(index1, std::move(f1)));
     callbacks.insert(callbacks_t::value_type(index2, std::move(f2)));
     callbacks.insert(callbacks_t::value_type(index3, std::move(f3)));

  // Call the callbacks.
     call(index1);
     call(index2, 5);
     call(index3, std::string("an answer of"), 42);

     return 0;
   }

