// g0020.cc
// gcc-2 accepts this code, despite the mismatch between argument
// and parameter type for the second argument to 'signal'; Elsa
// and gcc-3 do not accept it, but I'm creating this testcase just
// in case I want Elsa to accept it at some point

typedef void (*SignalProc)(... );

extern "C" {

typedef void (*__sighandler_t) (int);
extern __sighandler_t xxxsignal (int __sig, __sighandler_t __handler) throw () ;

}

void foo()
{
  SignalProc s;
  xxxsignal(1, s);
}
