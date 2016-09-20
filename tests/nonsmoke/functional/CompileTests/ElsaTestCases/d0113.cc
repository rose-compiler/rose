// CoderInfo-ootm.ii:17:3: error: no viable candidate for function call

// yes, gcc 2.96 will compile this.  I don't understand how the
// function tr::a() can be called when it has never been declared.  I
// don't understand how 'pos' can be used when it has never been
// declared

// sm: 10/02/04: The reason gcc accepts this is that it does not
// instantiate the templates.  The reason Elsa used to fail is that it
// would attempt operator overloading in uninstantiated template
// bodies, which fails for a variety of reasons.  I just fixed Elsa to
// not do that, so it accepts the (invalid; no specialization is
// possible; no diagnostic required) code for the same reason gcc
// does, namely that no instantiation occurs.

template<class C> struct S {};

template<bool t, int i> struct D {} ;

template<class C, class tr = S<C> >
struct B {
  struct Rep {               
    // sm: 2005-05-06: I added the following.  I do not know
    // why gcc and icc do not want it, since "(*p)[pos]" does
    // not seem to be dependent.
    int operator[](int);
  };
  template<class I> B &f (I j1);
};

template<class C, class tr>
  template<class I> B<C, tr> &B<C, tr>::f(I j1)
{
  // as Elsa now does non-dependent lookup, this needs to be
  // declared (in strict mode)
  int pos;

  Rep *p;
  (*p)[pos];
  tr::a((*p)[pos], *j1);
  // if I replace this with the below elsa seems to also like it but
  // what the heck is going on?
//    tr::a();
}
