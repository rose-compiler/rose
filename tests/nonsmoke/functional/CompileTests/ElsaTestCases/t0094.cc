// t0094.cc
// nasty excerpt from gcc-2.95.3's iomanip.h

// ouch, not sure what to do.  the line marked "/* !!! */" 
// has some angle brackets which the Standard grammar does
// not allow.  I'm not entirely sure what the goal of the
// author was.  for now I'm just going to comment-out the
// place that mozilla #includes iomanip.h, since I don't
// appear to actually need it to parse their file.

// 2005-02-20: Now elsa accepts this!  (After adding a couple of
// needed forward declarations.)  Maybe I was wrong about the grammar?
// icc and gcc-3 both like it too.

// 2005-02-27: I just figured this out.  The problematic syntax is
// *not* allowed by the grannar of ISO/IEC 14882:1998(E), but *is*
// allowed by the grammar of ISO/IEC 14882:2003(E).  My original
// conclusion that the syntax was invalid was itself based on a
// faulty grammar.  So now things are correct, I believe.


class ios;
class istream;
class ostream;


template<class TP> class smanip;

template<class TP> class sapp {
    ios& (*_f)(ios&, TP);
public:
    sapp(ios& (*f)(ios&, TP)) : _f(f) {}

    smanip<TP> operator()(TP a)
      { return smanip<TP>(_f, a); }
};

template<class TP>
inline istream& operator>>(istream& i, const smanip<TP>& m);
template<class TP>
inline ostream& operator<<(ostream& o, const smanip<TP>& m);

template <class TP> class smanip {
    ios& (*_f)(ios&, TP);
    TP _a;
public:
    smanip(ios& (*f)(ios&, TP), TP a) : _f(f), _a(a) {}

    friend
      istream& operator>> <>(istream& i, const smanip<TP>& m);  /* !!! */
    friend
      ostream& operator<< <>(ostream& o, const smanip<TP>& m);
};






template<class TP>
inline istream& operator>>(istream& i, const smanip<TP>& m)
{ (*m._f)(i, m._a); return i; }

template<class TP>
inline ostream& operator<<(ostream& o, const smanip<TP>& m)
{ (*m._f)(o, m._a); return o;}




template<class TP> class imanip;

template<class TP> class iapp {
    istream& (*_f)(istream&, TP);
public:
    iapp(istream& (*f)(istream&,TP)) : _f(f) {}

    imanip<TP> operator()(TP a)
       { return imanip<TP>(_f, a); }
};

template <class TP>
inline istream& operator>>(istream&, const imanip<TP>&);

template <class TP> class imanip {
    istream& (*_f)(istream&, TP);
    TP _a;
public:
    imanip(istream& (*f)(istream&, TP), TP a) : _f(f), _a(a) {}
     
    friend
      istream& operator>> <>(istream& i, const imanip<TP>& m);
};

template <class TP>
inline istream& operator>>(istream& i, const imanip<TP>& m)
{ return (*m._f)( i, m._a); }

 
 
 
 
template<class TP> class omanip; 

template<class TP> class oapp {
    ostream& (*_f)(ostream&, TP);
public: 
    oapp(ostream& (*f)(ostream&,TP)) : _f(f) {}
     
    omanip<TP> operator()(TP a)
      { return omanip<TP>(_f, a); }
};

template <class TP>
inline ostream& operator<<(ostream&, const omanip<TP>&);

template <class TP> class omanip {
    ostream& (*_f)(ostream&, TP);
    TP _a;
public:
    omanip(ostream& (*f)(ostream&, TP), TP a) : _f(f), _a(a) {}
     
    friend
      ostream& operator<< <>(ostream& o, const omanip<TP>& m);
};

template <class TP>
inline ostream& operator<<(ostream& o, const omanip<TP>& m)
{ return (*m._f)(o, m._a); }
