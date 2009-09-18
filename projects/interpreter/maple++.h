#ifndef _MAPLEPP_H
#define _MAPLEPP_H

#include <map>
#include <vector>
#include <string>

#include <maplec.h>

namespace MaplePP
   {

     struct MapleError
        {
          std::string err;
          MapleError(const std::string &err) : err(err) {}
        };

     class Algeb;

     class Maple
        {
          friend class Algeb;

          static Maple *inst;

          MKernelVector kv;

          bool lastErrorFlag;
          M_INT lastErrorOffset;
          std::string lastErrorMsg;

          /* This points to the first Algeb object we allocate.  Algeb objects
             maintain a doubly linked list of themselves which is used to hook
             into Maple's mark-sweep garbage collector. */
          Algeb *first;

          /* This ALGEB has a registered mark callback which iterates
             through all known Algebs (via the linked list). This enables us
             to mark all referenced Maple objects when the garbage collector is
             invoked. */
          ALGEB ptrDummy;

          static void M_DECL _textCallBack( void *data, int tag, char *output );
          static void M_DECL _errorCallBack( void *data, M_INT offset, char *msg );
          static void M_DECL _markAllRef( ALGEB a );

          void *mplTry(void *(M_DECL *proc) ( void *data ), void *data);

          protected:
          virtual void textCallBack( int tag, const std::string &output );
          virtual void errorCallBack( M_INT offset, char *msg );
          virtual void markAllRef();

          public:
          Maple(const std::vector<std::string> &args);
          Algeb evalStatement(const std::string &statement);
          Algeb newExpSeq(int nargs);
          Algeb toExpSeq(const std::vector<Algeb> &exps);
          Algeb fromFloat(double d);
          Algeb fromInteger64(INTEGER64 ll);
          Algeb mkName(const std::string &name, bool isGlobal);
          Algeb binOp(const Algeb &lhs, const std::string &op, const Algeb &rhs);
          Algeb toFunction(const Algeb &fname, const Algeb &args);
          Algeb toFunction(const Algeb &fname, const std::vector<Algeb> &args);
          Algeb toFunction(const std::string &fname, const Algeb &args);
          Algeb toFunction(const std::string &fname, const std::vector<Algeb> &args);
          virtual ~Maple();

        };

     class Algeb
        {
          friend class Maple;

          ALGEB algeb;
          Algeb *prev, *next;

          Algeb(ALGEB algeb);

          public:
          Algeb();
          Algeb(const Algeb &other);
          ~Algeb();
          Algeb &operator=(const Algeb &other);

          operator bool() const;

          std::string show() const;
          std::string toString() const;

          Algeb expSeqSelect(M_INT i) const;
          void expSeqAssign(M_INT i, const Algeb &exp);
          void assign(const Algeb &rhs) const;
          Algeb eval() const;
          Algeb toUneval() const;
        };

   };

#endif
