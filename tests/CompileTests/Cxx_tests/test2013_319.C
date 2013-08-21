// I think this test code demonstrates an error in the symbol table lookup for template member functions.

template<class _T2>
struct pair
   {
     _T2 second;
   };

template<typename _Tp> struct _Rb_tree_iterator {};

template<typename _Val>
class _Rb_tree
   {
     public:
          typedef _Rb_tree_iterator<_Val>       iterator;
   };

template<class _Key>
class set
   {
     public:
          pair<typename _Rb_tree<_Key>::iterator> insert();
   };
   
class Descriptor {};

class FileDescriptor 
   {
     public:
          void DebugString() const;
   };

void FileDescriptor::DebugString() const 
   {
     set<const Descriptor*> groups;

#if 1
  // This appears to be important code in demonstrating the bug:
  // it is unparsed as: groups . insert((this) ->  extension (i) ->  message_type ());
  // groups.insert(extension(i)->message_type());
     groups.insert();
#endif
   }

class DescriptorBuilder 
   {
     set<const FileDescriptor*> dependencies_;

  // void RecordPublicDependencies(const FileDescriptor* file);
     void RecordPublicDependencies();

   };

// void DescriptorBuilder::RecordPublicDependencies(const FileDescriptor* file) 
void DescriptorBuilder::RecordPublicDependencies() 
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
  // if (dependencies_.insert(file).second) return;
  // if (dependencies_.insert().second) return;
     dependencies_.insert().second;
   }

