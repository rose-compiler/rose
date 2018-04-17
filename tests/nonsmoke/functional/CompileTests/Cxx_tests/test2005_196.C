/*
compiling the code:

gives the following error message from ROSE:

rose_test.C: In member function `void B::prod(int)':
rose_test.C:11: error: no matching function for call to `B::prod(B&)'
rose_test.C:9: note: candidates are: void B::prod(int)

and the code outputted from ROSE is:
class B ;
inline extern void prod(class B b);

class B
{


  public: inline void prod(int x)
{
    class B z;
    prod(z);
  }

}

;

inline void prod(class B b)
{
}


int main()
{
  return 0;
}

as you can see the '::' in '::prod(z) in B::prod(..) is gone.

Andreas
*/


class B;

// inline void prod(B b );
void prod(B b );

class B {

  public:

  void prod(int x){
         B z;
        ::prod(z);
  };
};

inline void prod(B b ){};

int main(){return 0;}
