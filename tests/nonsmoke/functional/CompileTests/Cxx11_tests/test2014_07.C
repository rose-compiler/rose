typedef unsigned long size_t;

class MemoryPage
   {
     size_t size;
     char*  buf;

     public:
          explicit MemoryPage(int sz=512) : size(sz), buf(new char [size]) {}
         ~MemoryPage() { delete[] buf;}
       // typical C++03 copy ctor and assignment operator
          MemoryPage(const MemoryPage&);
          MemoryPage& operator=(const MemoryPage&);

          MemoryPage(MemoryPage&& other);

       // C& C::operator=(C&& other);//C++11 move assignment operator
          MemoryPage& MemoryPage::operator=(MemoryPage&& other);
   };



// Here’s a definition of MemoryPage‘s move assignment operator:

//C++11
MemoryPage::MemoryPage& MemoryPage::operator=(MemoryPage&& other)
   {
     if (this!=&other)
        {
       // release the current object’s resources
          delete[] buf;
          size = 0;
       // pilfer other’s resource
          size = other.size;
          buf = other.buf;
       // reset other
          other.size = 0;
       // other.buf = nullptr;
        }
     return *this;
   }
