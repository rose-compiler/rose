// I think this test code demonstrates an error in the symbol table lookup for template member functions.

# define _GLIBCXX_BEGIN_NAMESPACE(X) namespace X { 
# define _GLIBCXX_END_NAMESPACE } 

#  define _GLIBCXX_BEGIN_NESTED_NAMESPACE(X, Y) _GLIBCXX_BEGIN_NAMESPACE(X)
#  define _GLIBCXX_END_NESTED_NAMESPACE _GLIBCXX_END_NAMESPACE


_GLIBCXX_BEGIN_NAMESPACE(std)

template<class _T1, class _T2>
struct pair
   {
  // typedef _T1 first_type;    ///<  @c first_type is the first bound type
     typedef _T2 second_type;   ///<  @c second_type is the second bound type

  // _T1 first;                 ///< @c first is a copy of the first object
     _T2 second;                ///< @c second is a copy of the second object
   };

_GLIBCXX_END_NAMESPACE

_GLIBCXX_BEGIN_NAMESPACE(__gnu_cxx)

template<typename _Tp>
class new_allocator
   {
     public:
          template<typename _Tp1>
          struct rebind { typedef new_allocator<_Tp1> other; };
   };

_GLIBCXX_END_NAMESPACE

#define __glibcxx_base_allocator  __gnu_cxx::new_allocator

_GLIBCXX_BEGIN_NAMESPACE(std)

template<typename _Tp> class allocator;

template<typename _Tp> class allocator : public __glibcxx_base_allocator<_Tp> {};

_GLIBCXX_END_NAMESPACE


_GLIBCXX_BEGIN_NAMESPACE(std)

template <class _Tp> struct less
   {
     bool operator()(const _Tp& __x, const _Tp& __y) const;
   };

template <class _Tp> struct _Identity
   {
     _Tp& operator()(_Tp& __x) const;
     const _Tp& operator()(const _Tp& __x) const;      
   };

template<typename _Tp>
struct _Rb_tree_iterator
   {
     typedef _Tp  value_type;
     typedef _Tp& reference;
   };

template<typename _Tp>
struct _Rb_tree_const_iterator
   {
     typedef _Tp        value_type;
   };

template<typename _Key, typename _Val, typename _KeyOfValue, typename _Compare, typename _Alloc = allocator<_Val> >
class _Rb_tree
   {
     public:
           typedef _Val value_type;

     public:
          typedef _Rb_tree_iterator<value_type>       iterator;
          typedef _Rb_tree_const_iterator<value_type> const_iterator;
   };

_GLIBCXX_END_NAMESPACE



_GLIBCXX_BEGIN_NESTED_NAMESPACE(std, _GLIBCXX_STD)

template<class _Key, class _Compare = std::less<_Key>, class _Alloc = std::allocator<_Key> >
class set
   {
     public:
          typedef _Key     key_type;
          typedef _Key     value_type;
          typedef _Compare key_compare;

     private:
          typedef typename _Alloc::template rebind<_Key>::other _Key_alloc_type;
          typedef _Rb_tree<key_type, value_type, _Identity<value_type>,key_compare, _Key_alloc_type> _Rep_type;
          typedef typename _Rep_type::const_iterator            iterator;

     public:
          std::pair<iterator,bool> insert(const value_type& __x);
   };

_GLIBCXX_END_NESTED_NAMESPACE

   
class Descriptor {};

class FieldDescriptor 
   {
     public:
          const Descriptor* message_type() const;
   };

class FileDescriptor 
   {
     public:
          int DebugString() const;
          const FieldDescriptor* extension(int index) const;
   };

class FileDescriptorTables {};

int FileDescriptor::DebugString() const 
   {
     std::set<const Descriptor*> groups;

     int i;
#if 1
  // This appears to be important code in demonstrating the bug:
  // it is unparsed as: groups . insert((this) ->  extension (i) ->  message_type ());
     groups.insert(extension(i)->message_type());
#endif

     return 0;
   }


class DescriptorBuilder 
   {
     std::set<const FileDescriptor*> dependencies_;

     void RecordPublicDependencies(const FileDescriptor* file);

   };

void DescriptorBuilder::RecordPublicDependencies(const FileDescriptor* file) 
   {
  // When it works it looks like: 
  //    if (file == 0L || !(this) -> dependencies_ . insert(file) . std::pair< std::set< const Descriptor *,class std::less< const Descriptor * > ,class std::allocator< const Descriptor * > > ::iterator ,bool > ::second) {
  // And when it fails it looks like:
  //    if (file == 0L || !(this) -> dependencies_ . insert(file) . std::pair< std::set< const Descriptor *,class std::less< const Descriptor * > ,class std::allocator< const Descriptor * > > ::iterator ,bool > ::second) {

  // if (file == NULL || !dependencies_.insert(file).second) return;
  // if (dependencies_.insert(file).second) return;

  // DQ (6/16/2013): This might be a subtle error in the symbol table handling: that we map:
  //    std::pair<std::_Rb_tree_const_iterator<const google::protobuf::Descriptor*>, bool>
  // to 
  //    std::pair<std::_Rb_tree_const_iterator<const google::protobuf::FileDescriptor*>, bool>
  // by mistake, and thus call the wrong function.

  // It might be nice to not put out the base class qualifier for the member variable: "second"
  // however, it is also a problem (I think) that we might be getting it wrong internally within ROSE.

  // g++ generates an error for this code:
  //    rose_test2013_apps_12.cc:86: error: 'std::pair<std::_Rb_tree_const_iterator<const google::protobuf::Descriptor*>, bool>' is not a base of 'std::pair<std::_Rb_tree_const_iterator<const google::protobuf::FileDescriptor*>, bool>'
  //                                         std::pair<std::_Rb_tree_const_iterator<const google::protobuf::FileDescriptor*>, bool>
     if (dependencies_.insert(file).second) return;
   }

