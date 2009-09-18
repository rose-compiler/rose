#include <map>
#include <iostream>
#include <sstream>
#include <maplec.h>
#include <maple++.h>

using namespace std;

namespace MaplePP {

Maple *Maple::inst = NULL;

Maple::Maple(const vector<string> &args) : first(NULL)
   {
     char err[2048];  /* error string buffer */
     MCallBackVectorDesc cb = {  _textCallBack,
          _errorCallBack,
          0,   /* statusCallBack not used */
          0,   /* readLineCallBack not used */
          0,   /* redirectCallBack not used */
          0,   /* streamCallBack not used */
          0,
          0    /* callBackCallBack not used */
     };
     char **maple_argv = new char*[args.size()+2];
     maple_argv[0] = "/usr/lib/interpreter"; /* /usr/lib here is the path to libgmp.so which Maple loads dynamically */
     for (size_t i = 0; i < args.size(); ++i)
        {
          maple_argv[i+1] = const_cast<char *>(args[i].c_str());
        }
     maple_argv[args.size()+1] = 0;
     if( (kv=StartMaple(args.size()+1,maple_argv,&cb,static_cast<void *>(this),NULL,err)) == NULL ) {
          throw MapleError(err);
     }
     delete[] maple_argv;
     inst = this;

     ptrDummy = ToMaplePointer(kv, static_cast<void *>(this), 0);
     MaplePointerSetMarkFunction(kv, ptrDummy, _markAllRef);
     MapleGcProtect(kv, ptrDummy);
   }

void M_DECL Maple::_textCallBack( void *data, int tag, char *output )
   {
     Maple *self = static_cast<Maple *>(data);
     self->textCallBack(tag, output);
   }

void M_DECL Maple::_errorCallBack( void *data, M_INT offset, char *msg )
   {
     Maple *self = static_cast<Maple *>(data);
     self->errorCallBack(offset, msg);
   }

void M_DECL Maple::_markAllRef( ALGEB a )
   {
     Maple *self = Maple::inst;
     self->markAllRef();
   }

void Maple::markAllRef()
   {
     for (Algeb *alg = first; alg; alg = alg->next)
          if (alg->algeb)
               MapleGcMark(kv, alg->algeb);
   }

void Maple::textCallBack( int tag, const string &output )
   {
     cout << output << endl;
   }

/* If you override this function you need to make sure to call the base
   implementation for mplTry to work correctly. */
void Maple::errorCallBack( M_INT offset, char *msg )
   {
     lastErrorFlag = true;
     lastErrorOffset = offset;
     lastErrorMsg = msg;
   }

/* The purpose of this routine is to trap any errors returned by Maple.
   Maple's error handling is done using a wrapper routine MapleTrapError
   which I can only guess operates using setjmp/longjmp.  For this reason
   we need to be very careful about what the passed in function does
   (i.e. no allocation of C++ objects on the stack). */
void *Maple::mplTry(void *(M_DECL *proc) ( void *data ), void *data)
   {
     M_BOOL errorflag = FALSE;
     lastErrorFlag = false;
     void *result = MapleTrapError(kv, proc, data, &errorflag);
     if (errorflag || lastErrorFlag)
        {
          throw MapleError(lastErrorMsg);
        }
     return result;
   }

struct evalStatementData
   {
     MKernelVector kv;
     char *stmt;
   };

static void *M_DECL _evalStatement(void *p)
   {
     evalStatementData *esd = static_cast<evalStatementData *>(p);
     return EvalMapleStatement(esd->kv, esd->stmt);
   }

Algeb Maple::evalStatement(const string &statement)
   {
     evalStatementData esd = { kv, const_cast<char *>(statement.c_str()) };
     ALGEB rv = static_cast<ALGEB>(mplTry(_evalStatement, static_cast<void *>(&esd)));
     return Algeb(rv);
   }

struct newExpSeqData
   {
     MKernelVector kv;
     int nargs;
   };

static void *M_DECL _newExpSeq(void *p)
   {
     newExpSeqData *nesd = static_cast<newExpSeqData *>(p);
     return NewMapleExpressionSequence(nesd->kv, nesd->nargs);
   }

Algeb Maple::newExpSeq(int nargs)
   {
     newExpSeqData nesd = { kv, nargs };
     ALGEB rv = static_cast<ALGEB>(mplTry(_newExpSeq, static_cast<void *>(&nesd)));
     return Algeb(rv);
   }

Algeb Maple::toExpSeq(const vector<Algeb> &exps)
   {
     Algeb a = newExpSeq(exps.size());
     for (size_t i = 0; i < exps.size(); ++i)
        {
          a.expSeqAssign(i+1, exps[i]);
        }
     return a;
   }

Maple::~Maple()
   {
     MapleGcAllow(kv, ptrDummy);
     inst = NULL;
     StopMaple(kv);
   }

/* TODO: the tedious task of error-proofing these functions */
Algeb Maple::fromFloat(double d)
   {
     return Algeb(ToMapleFloat(kv, d));
   }

Algeb Maple::fromInteger64(INTEGER64 ll)
   {
     return Algeb(ToMapleInteger64(kv, ll));
   }

Algeb Maple::mkName(const string &name, bool isGlobal)
   {
     return Algeb(ToMapleName(kv, const_cast<char *>(name.c_str()), isGlobal));
   }

/* There is no direct API for performing an arbitrary binary operation,
   Therefore we need to use this workaround. */
Algeb Maple::binOp(const Algeb &lhs, const std::string &op, const Algeb &rhs)
   {
  // store the operands in the global symbol table
     Algeb __a = mkName("__a", true), __b = mkName("__b", true);
     __a.assign(lhs);
     __b.assign(rhs);
  // construct a statement to evaluate, and do so
     stringstream stmt;
     stmt << "__a " << op << " __b:";
     return evalStatement(stmt.str());
   }

Algeb Maple::toFunction(const Algeb &fname, const Algeb &args)
   {
     return Algeb(ToMapleFunc(kv, fname.algeb, args.algeb));
   }

Algeb Maple::toFunction(const Algeb &fname, const vector<Algeb> &args)
   {
     return toFunction(fname, toExpSeq(args));
   }

Algeb Maple::toFunction(const std::string &fname, const Algeb &args)
   {
     return toFunction(mkName(fname, true), args);
   }

Algeb Maple::toFunction(const std::string &fname, const vector<Algeb> &args)
   {
     return toFunction(fname, toExpSeq(args));
   }

Algeb::Algeb(ALGEB algeb) :
        algeb(algeb),
        prev(NULL),
        next(Maple::inst->first)
     {
       Maple::inst->first = this;
       if (next) next->prev = this;
     }

Algeb::Algeb() : algeb(NULL),
        prev(NULL),
        next(Maple::inst->first)
     {
       Maple::inst->first = this;
       if (next) next->prev = this;
     }

Algeb::Algeb(const Algeb &other) : algeb(other.algeb),
        prev(NULL),
        next(Maple::inst->first)
     {
       Maple::inst->first = this;
       if (next) next->prev = this;
     }

Algeb::~Algeb()
   {
     if (prev) prev->next = next;
          else Maple::inst->first = next;
     if (next) next->prev = prev;
   }

Algeb &Algeb::operator=(const Algeb &other)
   {
     algeb = other.algeb;
     return *this;
   }

struct sprintf1Data
   {
     MKernelVector kv;
     char *pattern;
     ALGEB algeb;
   };

static void *M_DECL _sprintf1(void *p)
   {
     sprintf1Data *sd = static_cast<sprintf1Data *>(p);
     return MapleALGEB_SPrintf1(sd->kv, sd->pattern, sd->algeb);
   }

string Algeb::show() const
   {
     // Maple performs lazy evaluation of expressions and it segfaults if you
     // attempt to sprintf an expression whose evaluation results in an error.
     // We work around this by wrapping the expression in an uneval, to prevent
     // the expression from being evaluated.
     Algeb uneval = toUneval();
     sprintf1Data sd = { Maple::inst->kv, "%a", uneval.algeb };
     ALGEB strALGEB = static_cast<ALGEB>(Maple::inst->mplTry(_sprintf1, static_cast<void *>(&sd)));
     Algeb strAlgeb(strALGEB);
     return strAlgeb.toString();
   }

struct toStringData
   {
     MKernelVector kv;
     ALGEB algeb;
   };

static void *M_DECL _toString(void *p)
   {
     toStringData *tsd = static_cast<toStringData *>(p);
     return MapleToString(tsd->kv, tsd->algeb);
   }

string Algeb::toString() const
   {
     toStringData tsd = { Maple::inst->kv, algeb };
     char *s = static_cast<char *>(Maple::inst->mplTry(_toString, static_cast<void *>(&tsd)));
     return string(s);
   }

struct expSeqSelectData
   {
     MKernelVector kv;
     ALGEB algeb;
     M_INT i;
   };

static void *M_DECL _expSeqSelect(void *p)
   {
     expSeqSelectData *esd = static_cast<expSeqSelectData *>(p);
     return MapleExpseqSelect(esd->kv, esd->algeb, esd->i);
   }

Algeb Algeb::expSeqSelect(M_INT i) const
   {
     expSeqSelectData esd = { Maple::inst->kv, algeb, i };
     ALGEB rv = static_cast<ALGEB>(Maple::inst->mplTry(_expSeqSelect, static_cast<void *>(&esd)));
     return Algeb(rv);
   }

struct expSeqAssignData
   {
     MKernelVector kv;
     ALGEB algeb;
     M_INT i;
     ALGEB elem;
   };

static void *M_DECL _expSeqAssign(void *p)
   {
     expSeqAssignData *esd = static_cast<expSeqAssignData *>(p);
     MapleExpseqAssign(esd->kv, esd->algeb, esd->i, esd->elem);
     return NULL;
   }

void Algeb::expSeqAssign(M_INT i, const Algeb &exp)
   {
     expSeqAssignData esd = { Maple::inst->kv, algeb, i, exp.algeb };
     Maple::inst->mplTry(_expSeqAssign, static_cast<void *>(&esd));
   }

struct assignData
   {
     MKernelVector kv;
     ALGEB lhs, rhs;
   };

static void *M_DECL _assign(void *p)
   {
     assignData *ad = static_cast<assignData *>(p);
     MapleAssign(ad->kv, ad->lhs, ad->rhs);
     return NULL;
   }

void Algeb::assign(const Algeb &rhs) const
   {
     assignData ad = { Maple::inst->kv, algeb, rhs.algeb };
     Maple::inst->mplTry(_assign, static_cast<void *>(&ad));
   }

Algeb::operator bool() const
   {
     return (algeb != NULL);
   }

struct evalData
   {
     MKernelVector kv;
     ALGEB algeb;
   };

static void *M_DECL _eval(void *p)
   {
     evalData *ed = static_cast<evalData *>(p);
     return MapleEval(ed->kv, ed->algeb);
   }

Algeb Algeb::eval() const
   {
     evalData ed = { Maple::inst->kv, algeb };
     ALGEB rv = static_cast<ALGEB>(Maple::inst->mplTry(_eval, static_cast<void *>(&ed)));
     return Algeb(rv);
   }

struct toUnevalData
   {
     MKernelVector kv;
     ALGEB algeb;
   };

static void *M_DECL _toUneval(void *p)
   {
     toUnevalData *ed = static_cast<toUnevalData *>(p);
     return ToMapleUneval(ed->kv, ed->algeb);
   }

Algeb Algeb::toUneval() const
   {
     toUnevalData ed = { Maple::inst->kv, algeb };
     ALGEB rv = static_cast<ALGEB>(Maple::inst->mplTry(_toUneval, static_cast<void *>(&ed)));
     return Algeb(rv);
   }

};

