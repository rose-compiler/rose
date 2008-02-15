// Demonstrates bug specific to two member functions with the same name (overloaded)
template<typename T> struct template_class {};

template<> struct template_class<char>
    {
      static void assign(int x) {}
      static void assign(char c) {}
    };

