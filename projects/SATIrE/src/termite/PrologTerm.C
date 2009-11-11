#include "termite.h"

#include <cstring>

using namespace std;

#if HAVE_SWI_PROLOG


// true if the pattern can be unified with the term
bool PrologTerm::matches(std::string pattern) {
  fid_t fid = PL_open_foreign_frame();
  // FIXME: cache predicate
  term_t a0 = PL_new_term_refs(2);
  assert(PL_unify(a0, term));
  PL_put_variable(a0+1);
  PL_chars_to_term(pattern.c_str(), a0);
  PrologTerm t(a0);
  //cerr<<t.getRepresentation()<<endl;
  //cerr<<getRepresentation()<<endl;
  bool b = PL_call_predicate(NULL, PL_Q_NORMAL,
			     PL_predicate("=@=", 2, ""), a0);
  //cerr<<b<<endl;
  PL_discard_foreign_frame(fid);
  return b;
}

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
  // Sizes of "0" mean the largest possible limits.
  av[ac++] = strdup("-L0");  // Local stack
  av[ac++] = strdup("-G0");  // Global stack
  av[ac++] = strdup("-A0");  // Argument stack
  av[ac++] = strdup("-T0");  // Trail stack

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

  // Make sure SWI always finds... itself. Its home directory is determined
  // by the configure script and must be passed on the command line when
  // this file is compiled. It will then be hardcoded in the library, which
  // is not very nice, but somewhat reasonable.
  // (see http://www.swi-prolog.org/FAQ/FindResources.html for info)
  setenv("SWI_HOME_DIR", SWI_HOME_DIR, /* overwrite = */ 0);

  return PL_initialise(ac, av);
}

PrologCompTerm* isPrologCompTerm(PrologTerm* pt) { 
  term_t t = pt->getTerm();
  term_t h = PL_new_term_ref();
  if (PL_term_type(t) == PL_TERM && !PL_get_head(t, h))
    return (PrologCompTerm*)pt;
  else return NULL;
}
PrologList* isPrologList(PrologTerm* pt)  { 
  term_t t = pt->getTerm();
  term_t h = PL_new_term_ref();
  if ((PL_term_type(t) == PL_ATOM && PL_get_nil(t))
      || (PL_term_type(t) == PL_TERM && PL_get_head(t, h)))
    return (PrologList*)pt;
  else return NULL;
}
PrologAtom* isPrologAtom(PrologTerm* pt) {
  term_t t = pt->getTerm();
  if (PL_term_type(t) == PL_ATOM && !PL_get_nil(t))
  return (PrologAtom*)pt;
    else return NULL;
}
PrologInt* isPrologInt(PrologTerm* pt) {
  term_t t = pt->getTerm();
  if (PL_term_type(t) == PL_INTEGER)
    return (PrologInt*)pt;
  else return NULL;
}
PrologFloat* isPrologFloat(PrologTerm* pt) {
  term_t t = pt->getTerm();
  if (PL_term_type(t) == PL_FLOAT)
    return (PrologFloat*)pt;
  else return NULL;
}
PrologVariable* isPrologVariable(PrologTerm* pt) {
  term_t t = pt->getTerm();
  if (PL_term_type(t) == PL_VARIABLE)
    return (PrologVariable*)pt;
  else return NULL;
}

#else //////////////////////////////////////////////////////////////////

bool init_termite(int argc, char **argv, bool) {
  return true;
}

PrologCompTerm* isPrologCompTerm(PrologTerm* t) { 
  return dynamic_cast<PrologCompTerm*>(t);
}
PrologList* isPrologList(PrologTerm* t) { 
  return dynamic_cast<PrologList*>(t);
}
PrologAtom* isPrologAtom(PrologTerm* t) { 
  return dynamic_cast<PrologAtom*>(t);
}
PrologInt* isPrologInt(PrologTerm* t) { 
  return dynamic_cast<PrologInt*>(t);
}
PrologVariable* isPrologVariable(PrologTerm* t) { 
  return dynamic_cast<PrologVariable*>(t);
}

#endif
