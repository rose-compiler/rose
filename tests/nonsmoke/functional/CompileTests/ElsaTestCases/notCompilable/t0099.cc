// t0099.cc
// I claim something is ambiguous...
// excerpt from gcc-2.95.3's ststream.h

class streambuf;
typedef int __ssize_t;

struct _ios_fields
{  
    streambuf *_strbuf;
};

class ios : public _ios_fields {
};

class ostream : virtual public ios
{
};

class strstreambuf {
  int pcount();
};

class strstreambase : virtual public ios {
};

class ostrstream : public strstreambase, public ostream {
    __ssize_t   pcount() { return ((strstreambuf*)_strbuf)->pcount(); }
};
