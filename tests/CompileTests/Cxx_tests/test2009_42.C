/*
Run the file below through the identity Translator.  In the main() function, the Group::Iter constructor is unparsed
correctly when declared as a standalone statement,  but the declaration is erroneously unparsed when it is used in the
for-init context.

Note that the unparse that ROSE uses for the for-init declaration will be valid for some/most classes, but not for classes
that have explicitly disabled some fundamental operators (See IntStack private methods below).
*/

// #include <vector>

// Required to demontrate previous bug using an explicit cast (previous bug from test2004_130.C)
// #include <string>

class IntStack
   {
     public:
          IntStack(int size);
          int &push(int &x);
          int pop();

          class Iter
             {
               public:
                    Iter(IntStack *s);
                   ~Iter();
                    bool end() const;
                    void next();
                    int  item() const;

               private:
                 /* disable default and copy constructor, along with assignment */
                    Iter();

                 // Private copy constructor
                    Iter(const Iter &);

                 // Private equals operator
                    Iter & operator=(const Iter &);

                 // std::vector<int>::iterator si;
                 // std::vector<int>::iterator ei;
             };

       // std::vector<int> data;

     private:
          IntStack();
          IntStack(const IntStack &);
          IntStack &operator=(const IntStack &);
   };

void foo()
   {
  // Assignment using an explicit cast (previous bug from test2004_130.C)
  // std::string s = std::basic_string< char , std::char_traits< char >  , std::allocator< char >  > ("");

     IntStack x(20);
     IntStack::Iter y(&x);

  // This will use the non-copy constructor
     for (IntStack::Iter z(&x) ; !z.end() ; z.next());

// DQ (2/20/2010): This is a error for g++ 4.x compilers (at least g++ 4.2).
#if (__GNUC__ >= 3)
#else
  // In g++ 4.2.x this will force the use of the copy constructor which is private.
  // However in EDG this is allowed and will use the assignment initializer (a different constructor).
     for (IntStack::Iter z = (&x) ; !z.end() ; z.next());
#endif

  // return 0;
   }
