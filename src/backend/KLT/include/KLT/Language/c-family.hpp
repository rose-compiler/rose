
#ifndef __KLT_LANGUAGE_CFAMILY_HPP__
#define __KLT_LANGUAGE_CFAMILY_HPP__

namespace KLT {

namespace Language {

class Cfamily { virtual void polymorphe() const = 0; };

    class C        : public Cfamily { virtual void polymorphe() const; };
//    class C89    : public C       { virtual void polymorphe() const; };
//    class C99    : public C       { virtual void polymorphe() const; };
//    class C11    : public C       { virtual void polymorphe() const; };
      class OpenCL : public C       { virtual void polymorphe() const; };
    class Cxx      : public Cfamily { virtual void polymorphe() const; };
//    class Cxx04  : public Cxx     { virtual void polymorphe() const; };
//    class Cxx11  : public Cxx     { virtual void polymorphe() const; };
      class CUDA   : public Cxx     { virtual void polymorphe() const; };

}

}

#endif /* __KLT_LANGUAGE_CFAMILY_HPP__ */

