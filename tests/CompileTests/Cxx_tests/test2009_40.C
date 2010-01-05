/*
Run the file below through the identity Translator.  In the main() function, the Group::Iter constructor is unparsed
correctly when declared as a standalone statement,  but the declaration is erroneously unparsed when it is used in the
for-init context.

Note that the unparse that ROSE uses for the for-init declaration will be valid for some/most classes, but not for classes
that have explicitly disabled some fundamental operators (See IntStack private methods below).
*/

#include <vector>

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
                    Iter(const Iter &);
                    Iter & operator=(const Iter &);

                    std::vector<int>::iterator si;
                    std::vector<int>::iterator ei;
             };

          std::vector<int> data;

     private:
          IntStack();
          IntStack(const IntStack &);
          IntStack &operator=(const IntStack &);
   };

     IntStack::IntStack(int size) : data(size) {}
int& IntStack::push(int &x) { data.push_back(x) ; return x ; }
int  IntStack::pop() { int val = data.back() ; data.pop_back() ; return val ; }

     IntStack::Iter::Iter(IntStack *s) { si = s->data.begin() ; ei = s->data.end() ; }
     IntStack::Iter::~Iter() {} ;
bool IntStack::Iter::end() const { return si == ei ; }
void IntStack::Iter::next() { ++si ; }
int  IntStack::Iter::item() const { return *si ; }

int main(int argc, char *argv[])
   {
     IntStack x(20);
     IntStack::Iter y(&x);
     for (IntStack::Iter z(&x) ; !z.end() ; z.next());

     return 0;
   }
