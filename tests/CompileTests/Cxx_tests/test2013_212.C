// I think this test code demonstrates an error in the symbol table lookup for template member functions.

#include "set"

#if 0
template <typename T>
class set
   {
     T t;

     public:
          class iterator
             {
               public:
                    T second;
             };

          iterator insert(T);
   };
#endif
   
namespace google {
namespace protobuf {

class Descriptor {};

class FieldDescriptor 
   {
     public:
          const Descriptor* message_type() const;
   };

class DescriptorPool 
   {
     class Tables;
   };

class FileDescriptor
 
   {
     public:
          int DebugString() const;
          int extension_count() const;
          const FieldDescriptor* extension(int index) const;
   };

class DescriptorPool::Tables {};

class FileDescriptorTables {};


int FileDescriptor::DebugString() const 
   {
     int contents = 0;

     std::set<const Descriptor*> groups;

     for (int i = 0; i < extension_count(); i++) 
        {
          if (true) 
             {
#if 1
            // This appears to be important code in demonstrating the bug:
            // it is unparsed as: groups . insert((this) ->  extension (i) ->  message_type ());
               groups.insert(extension(i)->message_type());
#endif
             }
        }

     return contents;
   }


class DescriptorBuilder 
   {
     std::set<const FileDescriptor*> dependencies_;

     void RecordPublicDependencies(const FileDescriptor* file);

//   class OptionInterpreter {};

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

  // It might be nice to now put out the base class qualifier for the member variable: "second"
  // however, it is also a problem (I think) that we might be getting it wrong internally within ROSE.

  // g++ generates an error for this code:
  //    rose_test2013_apps_12.cc:86: error: 'std::pair<std::_Rb_tree_const_iterator<const google::protobuf::Descriptor*>, bool>' is not a base of 'std::pair<std::_Rb_tree_const_iterator<const google::protobuf::FileDescriptor*>, bool>'
  //                                         std::pair<std::_Rb_tree_const_iterator<const google::protobuf::FileDescriptor*>, bool>
     if (dependencies_.insert(file).second) return;
   }

}  // namespace protobuf
}  // namespace google
