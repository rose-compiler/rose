class MemoryPage
   {
     public:
          MemoryPage& operator=(const MemoryPage&);
          MemoryPage(MemoryPage& other);

       // Move constructor.
          MemoryPage(MemoryPage&& other);

       // C& C::operator=(C&& other);//C++11 move assignment operator
       // MemoryPage& MemoryPage::operator=(MemoryPage&& other);
          MemoryPage& operator=(MemoryPage&& other);
   };
