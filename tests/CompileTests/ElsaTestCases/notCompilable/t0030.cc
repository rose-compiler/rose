// cc.in30
// misc

int f(int x = 3);

// parameter is unnamed
int f2(int = 5);

class Foo {
public:
  // this doesn't work right now..
  //int g(int x = y);    // refers to static member Foo::y
  //static int y;

  int h(int j=9);
  
  // ctor with "inline"
  inline Foo(int *x);
};

// anonymous class
class {
  int x;
} xx;

// anonymous enum
enum { whazza, whozza, howza } futza;

// array size that's not a simple literal integer
typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int))) ];
  } __sigset_t;

// enum where one value is used to initialize another
enum Blah { A, B, C=B, D };
  


// struct with same name as function
struct stat {
  int x;
};

extern int stat (const  char *   __file,
		 struct stat *   __buf)  ;

  
// struct with same name as variable
struct timezone
  {
    int tz_minuteswest;
    int tz_dsttime;
  };

extern long int timezone;


// enumerators with values calculated from prior enumerators
enum { skipws= 01 ,
       left= 02 , right= 04 , internal= 010 ,
       dec= 020 , oct= 040 , hex= 0100 ,
       showbase= 0200 , showpoint= 0400 ,
       uppercase= 01000 , showpos= 02000 ,
       scientific= 04000 , fixed= 010000 ,
       unitbuf= 020000 , stdio= 040000
       };
enum {
    basefield=dec+oct+hex,
    floatfield = scientific+fixed,
    adjustfield = left+right+internal
};



class ios {
    // overloading on constness of 'this'
    void*& pword(int);
    void* pword(int) const;
    long& iword(int);
    long iword(int) const;

    // defining a typedef of an enum
    enum seek_dir { beg, cur, end};
    typedef enum seek_dir seekdir;

};

// and then typedef'ing that outside the class scope
typedef ios::seek_dir _seek_dir;


class streammarker {
    streammarker(int *sb);
    ~streammarker();

    // doesn't think 'streammarker' is a type name here?
    // I see, it was getting confused by the constructor name
    // hiding the type name itself.  now that's fixed.
    int delta(streammarker&);
    int delta();
};


// apparently the keyword clash is ok
class streambuf;
struct streambuf { int foo; };


// call to delete[] using unusual syntax
void call_delete()
{
  int *x;
  operator delete[] (x);
}


class ostream {
    // problems with "this"?
    ostream& operator<<(unsigned char c) { return (*this) << (char)c; }
};


