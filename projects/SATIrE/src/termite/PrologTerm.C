#include "termite.h"

using namespace std;

#if HAVE_SWI_PROLOG

// Create a new PrologTerm from a real Prolog Atom
PrologTerm *PrologTerm::wrap_PL_Term(term_t t)
{
  //cerr<< "WRAPPING " << display(t) << endl;

  PrologTerm *pt = 0;
  switch( PL_term_type(t) ) {
  case PL_VARIABLE: assert(false && "Encountered Variable!");

  case PL_ATOM:     
    if (PL_get_nil(t)) pt = new PrologList(t);
    else               pt = new PrologAtom(t); 
    break;

  case PL_INTEGER:  
    // Careful about overloading
    pt = new PrologInt(); 
    ((PrologInt*)pt)->createFromTerm(t);
    break;

  case PL_FLOAT:    assert(false);
  case PL_STRING:   assert(false);
  case PL_TERM: {
    term_t h = PL_new_term_ref();
    if (PL_get_head(t, h)) pt = new PrologList(t);
    else                   pt = new PrologCompTerm(t);
    break;
  }
  default:          assert(false);
  }
  return pt;
}

// Create a new PrologTerm from a real Prolog Atom
// it will automatically be freed at the end of this object's lifetime
PrologTerm* PrologTerm::newPrologTerm(term_t t)
{
  PrologTerm *pt = wrap_PL_Term(t);
  garbage_bin.push_front(pt);
  return pt;
}

// Create a real Prolog term from a PrologTerm
// it will be garbage-collected by SWI-Prolog
term_t PrologTerm::newTerm_t(PrologTerm* pt) 
{
  //if (PrologAtom *a = dynamic_cast<PrologAtom *>pt) {
  assert(0 && "do we need this function at all???");
}

void abort_termite() {
  PL_cleanup(0);
}

bool init_termite(int argc, char **argv, bool interactive)
{ 
  char *av[10];
  int ac = 0;

  av[ac++] = argv[0];
  av[ac++] = strdup("-q");
  av[ac++] = strdup("-O");
  av[ac++] = strdup("-L65536k");  // Local stack
  av[ac++] = strdup("-G8192M");  // Global stack
  av[ac++] = strdup("-A1024M");  // Argument stack
  av[ac++] = strdup("-T8192k");  // Trail stack

  if (interactive == false) {
    // At runtime, it is advised to pass the flag -nosignals, which
    // inhibits all default signal handling. This has a few consequences
    // though:
    //
    // It is no longer possible to break into the tracer using an
    // interrupt signal (Control-C).
    //
    // SIGPIPE is normally set to be ignored. Prolog uses return-codes to
    // diagnose broken pipes. Depending on the situation one should take
    // appropriate action if Prolog streams are connected to pipes.
    //
    // Fatal errors normally cause Prolog to call PL_cleanup() and
    // exit(). It is advised to call PL_cleanup() as part of the
    // exit-procedure of your application.
    av[ac++] = strdup("-nosignals");
    atexit(abort_termite);
  }

  av[ac]   = NULL;

  return PL_initialise(ac, av);
}

#else

bool init_termite(int argc, char **argv, bool) {
  return true;
}

#endif
