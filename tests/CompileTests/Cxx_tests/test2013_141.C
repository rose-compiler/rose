class MemoryManager;
class XSerializable;

class XProtoType
   {
     public:
//        unsigned char* fClassName;

       // This function pointer can point to a sttic member function OR and non-member function.
          XSerializable* (*fCreateObject)(MemoryManager*);
   };
// class XSerializable {};

class KVStringPair
   {
     public:
         KVStringPair(MemoryManager* const manager);

     public: 
         static XProtoType classKVStringPair;
      // static XSerializable* createObject(MemoryManager* manager);
         static XSerializable* createObject(MemoryManager* manager);
   };

// XProtoType KVStringPair::classKVStringPair = {(unsigned char*) "KVStringPair", KVStringPair::createObject };
XProtoType KVStringPair::classKVStringPair = { KVStringPair::createObject };
