#ifndef SIGNATURE_READER_H
#define SIGNATURE_READER_H

#include <string>
#include <map>
#include <set>
#include <ostream>
#include <vector>

#include "ssexpr.h"
#include "APISpec.h"
/* ArgMapsTo stores a mapping from the callers arguments to the callees
 * arguments. This enables us to do dependency analysis for arguments.
 */
class ArgMapsTo {
  public:

  typedef unsigned int ArgId;
  typedef std::string FunId;

  ArgMapsTo(const ArgId & callerArg, const FunId & calledFun, const ArgId & calleeArg)
  : callerArg(callerArg), calledFun(calledFun), calleeArg(calleeArg) {}

  ArgId callerArg;
  FunId calledFun;
  ArgId calleeArg;

  bool operator==(const ArgMapsTo & other) const;
};

typedef std::vector<ArgMapsTo> ArgMapping;

/* Signature stores the call list and argument mapping for each function */
class Signature {
  public:

  Signature() : calls(), argMapping(), argCount(0) {}
  std::set<std::string> calls;
  ArgMapping argMapping;
  int argCount;
};
typedef std::map<std::string,Signature> Signatures;

// Builds a map of signatures from the input
// The input should be roughly of this form,
// with a "function-signature" section for each
// function:
//  (function-signature
//    (function-name "main")
//    (arg-count "2")
//    (calls "MPI_Init"
//           "MPI_Comm_rank"
//           "MPI_Comm_split"
//           "master_io"
//           "slave_io"
//           "MPI_Finalize"))
//
Signatures readSignatures(const std::string & fname, bool debug=false);

Signature readCalls(const SExpr * const sx, bool debug=false);

ArgMapping readArgMapping(const SExpr * const sx, bool debug=false);

std::string readFunctionName(const SExpr * const sx, bool debug=false);

// See the .cc file for details on the structure of the SExpr
SExpr * toSExpr(const Signature & sig);
SExpr * toSExpr(const Signatures & sigs);
SExpr * toSExpr(const ArgMapping & argMap);

// Utility functions for Signatures:

std::ostream& operator<<(std::ostream & os, const Signatures & sigs);
std::ostream& operator<<(std::ostream & os, const Signature & sig);
std::ostream& operator<<(std::ostream & os, const ArgMapping & argMap);

// Computes the transitive call information at each graph node
// This makes dependency propagation easier. We can easily see
// if a node will eventually call something we care about.
//
// Note: Currently this ignores data flow and only considers
// static control flow information (flow in-sensitive)
Signatures transitiveClosure( const Signatures & sigs );
Signatures transitiveCalls( const Signatures & sigs );
Signatures transitiveParameters( const Signatures & sigs );

bool
edgeExists( const Signatures & sigs
          , const ArgMapsTo::FunId & callerFun
          , const ArgMapsTo::ArgId & callerArg
          , const ArgMapsTo::FunId & calleeFun
          , const ArgMapsTo::ArgId & calleeArg );

// Constructs a new APISpec from the signatures passed in
// and appends that to the provided APISpecs.
void
buildAPISpecFromSignature( APISpecs & specs
                         , const Signatures & sigs );

// Only inserts argMap if it's not already there (to avoid duplicates)
void insertArgMapping(ArgMapping & argMapping, const ArgMapsTo & argMap);
#endif
