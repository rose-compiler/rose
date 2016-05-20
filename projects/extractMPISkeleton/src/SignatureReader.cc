#include "SignatureReader.h"

#include <boost/foreach.hpp>

#include <vector>
#include <iostream>
#include <sstream>

#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#error The transitive closure algorithm uses partial specialization.
#endif

#include <boost/graph/transitive_closure.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/labeled_graph.hpp>
#define foreach BOOST_FOREACH

#include "rose.h"

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

bool ArgMapsTo::operator==(const ArgMapsTo & other) const
{
  return other.calledFun == this->calledFun &&
         other.callerArg == this->callerArg &&
         other.calleeArg == this->calleeArg;
}

Signatures
readSignatures(const std::string & fname, bool debug)
{
    if( debug ) std::cout << "Reading from: " << fname << std::endl;
    SExpr * const sx = SExpr::parse_file(fname);
    Signatures ret;
    ROSE_ASSERT(sx != NULL);

    if( debug ) std::cout << "Read: " << *sx << std::endl;
    for(SExpr *cur = sx; cur != NULL; cur = cur->getNext()){
      if( debug ) std::cout << "Starting loop" << std::endl;
      if( debug ) std::cout << "cur = " << *cur << std::endl;

      // Find the "function-signature" portion of the list
      SExpr * function_signature = cur->getList();
      // if the function_signature is empty, skip this one
      if( function_signature == NULL ) continue;
      if( debug ) std::cout << "loop body" << std::endl;

      // If this is non-NULL then we should be at the function-signature
      function_signature = function_signature->getNext();
      ROSE_ASSERT(function_signature != NULL);
      if( debug ) std::cout << "function_signature = " << *function_signature << std::endl;

      // Read out the function-name portion of the function-signature
      const std::string function_name = readFunctionName(function_signature, debug);
      if( debug ) std::cout << "function_name = '" << function_name << "'" << std::endl;

      // Move the cursor to the "calls" sublist
      SExpr * calls = function_signature->getNext();
      if( debug ) std::cout << "calls = '" << *calls << "'" << std::endl;
      ROSE_ASSERT( calls != NULL );

      Signature sig = readCalls(calls,debug);
      // Skip over both (arg-count ...) and (calls ...)
      SExpr * args = calls->getNext()->getNext();
      if( debug ) std::cout << "args = '" << *args << "'" << std::endl;
      ROSE_ASSERT( args != NULL );
      sig.argMapping = readArgMapping(args, debug);
      if( debug) std::cout << "mapping = '" << *toSExpr(sig.argMapping) << "'" << std::endl;
      // Finally, update the collection of signatures with what we've read so
      // far
      ret[function_name] = sig;

    }
    if ( debug ) std::cout << __PRETTY_FUNCTION__ << ": " << std::endl
                           << ret << std::endl;
    return ret;
}

// Parses this portion of the input:
// (arg-mapping ("0" ("foo" "1"))
//              ("0" ("baz" "3"))
//              ("3" ("bar" "0")))
ArgMapping readArgMapping(const SExpr * const sx, bool debug /* =false */)
{
  ROSE_ASSERT( sx != NULL );
  ArgMapping argMapping;
  SExpr * tmp = sx->getList();
  ROSE_ASSERT( tmp != NULL );
  if(debug) std::cout << "tmp->getValue() = '" << tmp->getValue() << "'" << std::endl;
  ROSE_ASSERT( tmp->getValue() == "arg-mapping" );
  for(tmp = tmp->getNext(); tmp != NULL; tmp = tmp->getNext()){
    ROSE_ASSERT( tmp != NULL );
    SExpr * mapping = tmp->getList();
    ROSE_ASSERT( mapping != NULL );
    const std::string callerArgId = mapping->getValue();
    mapping = mapping->getNext();
    ROSE_ASSERT( mapping != NULL );
    mapping = mapping->getList();
    ROSE_ASSERT( mapping != NULL );
    const std::string calleeFunId = mapping->getValue();
    mapping = mapping->getNext();
    const std::string calleeArgId = mapping->getValue();
    insertArgMapping(argMapping, ArgMapsTo(atoi(callerArgId.c_str()), calleeFunId, atoi(calleeArgId.c_str())));
  }
  return argMapping;
}

// We'd like to avoid inserting duplicate arg mappings, so we check before
// we push_back.
void insertArgMapping(ArgMapping & argMapping, const ArgMapsTo & argMap)
{
  for(size_t i = 0; i < argMapping.size(); ++i){
    const ArgMapsTo & cur = argMapping[i];
    if( cur == argMap ) return;
  }
  argMapping.push_back(argMap);
}

// Parses this portion of the input:
//    (function-name "main")
//
std::string
readFunctionName(const SExpr * const sx, bool debug)
{
      SExpr * tmp = sx->getList();
      ROSE_ASSERT( tmp != NULL );
      ROSE_ASSERT(tmp->getValue() == "function-name");
      tmp = tmp->getNext();
      ROSE_ASSERT(tmp != NULL);
      const std::string function_name = tmp->getValue();
      if ( debug ) std::cout << __PRETTY_FUNCTION__ << ": read " << function_name << std::endl;
      ROSE_ASSERT(function_name != "");
      return function_name;
}

// Parses this portion of the input:
//    (arg-count "3")
//    (calls "MPI_Init"
//           "MPI_Comm_rank"
//           "MPI_Comm_split"
//           "master_io"
//           "slave_io"
//           "MPI_Finalize")
Signature
readCalls(const SExpr * const sx, bool debug)
{
   Signature ret;
   if ( debug ) std::cout << __PRETTY_FUNCTION__ << ": " << *sx << std::endl;
   
   SExpr * tmp = sx->getList();
   ROSE_ASSERT(tmp != NULL);
   ROSE_ASSERT(tmp->getValue() == "arg-count");
   // Read the argument count
   tmp = tmp->getNext();
   ROSE_ASSERT( tmp != NULL );
   ROSE_ASSERT( tmp->getValue() != "" );
   const int arg_count = atoi(tmp->getValue().c_str());
   ret.argCount = arg_count;

   tmp = sx->getNext();
   ROSE_ASSERT(tmp != NULL);
   tmp = tmp->getList();
   ROSE_ASSERT(tmp != NULL);
   ROSE_ASSERT(tmp->getValue() == "calls");
   for(tmp = tmp->getNext(); tmp != NULL; tmp = tmp->getNext()){
     ret.calls.insert(tmp->getValue());
   }
   return ret;
}

// Generates an SExpr of this form:
// (arg-mapping ("0" ("foo" "1"))
//              ("0" ("baz" "3"))
//              ("3" ("bar" "0")))
SExpr * toSExpr(const ArgMapping & argMapping)
{
  SExpr * const argMaps = new SExpr();
  argMaps->setList(new SExpr("arg-mapping"));
  foreach(const ArgMapsTo & argMapsTo, argMapping)
  {
    const ArgMapsTo::ArgId & callerArg(argMapsTo.callerArg);
    const ArgMapsTo::FunId & calledFun(argMapsTo.calledFun);
    const ArgMapsTo::ArgId & calleeArg(argMapsTo.calleeArg);
    SExpr * const sx = new SExpr();
    std::stringstream ss;

    // Convert the caller arg to a string
    ss << callerArg;
    sx->setList(new SExpr(ss.str()));

    // Reset the string strem
    ss.str(""); ss.clear();

    // Construct the callee info
    SExpr * const callee = new SExpr();
    callee->setList(new SExpr(calledFun));
    ss << calleeArg;
    callee->getList()->append(new SExpr(ss.str()));

    // Now append all the callee info to the caller info
    // and also to the overall list we're building.
    sx->getList()->append(callee);
    argMaps->getList()->append(sx);
  }
  return argMaps;
}

// Generates an SExpr of this form:
// (arg-count "n")
// (calls "call1" "call2" ... "callN")
// (arg-mapping ("0" ("call1" "0")))
SExpr * toSExpr(const Signature & sig)
{
   SExpr * const sx = new SExpr();
   sx->setList(new SExpr("arg-count"));

   std::stringstream ss;
   ss << sig.argCount;

   sx->getList()->append(new SExpr(ss.str()));
   SExpr * const calls = new SExpr(new SExpr("calls"));
   sx->append(calls);
   foreach(const std::string & callee, sig.calls){
      calls->getList()->append(new SExpr(callee));
   }
   SExpr * const argMaps = toSExpr(sig.argMapping);
   sx->append(argMaps);
   return sx;
}

// Generates an SExpr of this form:
// (function-signature
//  (function-name "foo")
//  (arg-count "n")
//  (calls "bar1" ... "barN")
//  (arg-mapping ("1" ("bar1" "0"))))
SExpr * toSExpr(const Signatures & sigs)
{
   SExpr * const ret = new SExpr();
   foreach(const Signatures::value_type & v, sigs){
      SExpr * const fsig = new SExpr();
      fsig->setValue("function-signature");
      SExpr * const fname = new SExpr();
      fname->setList(new SExpr("function-name"));
      fname->getList()->append(new SExpr(v.first));
      fsig->append(fname);
      SExpr * const calls = toSExpr(v.second);
      fsig->append(calls);
      ret->getList() == NULL ? ret->setList(fsig) : ret->append(new SExpr(fsig));
   }
   return ret;
}

std::ostream& operator<<(std::ostream & os, const Signatures & sigs)
{
   foreach(const Signatures::value_type & pair, sigs){
      os << pair.first << " calls: " << pair.second << std::endl;
   }
   return os;
}

std::ostream& operator<<(std::ostream & os, const Signature & sig)
{
  os << "arg-count: " << sig.argCount << std::endl;
  foreach(const std::string & callee, sig.calls){
    os << callee << " ";
  }
  os << std::endl;
  os << sig.argMapping << " ";
  return os;
}

std::ostream& operator<<(std::ostream & os, const ArgMapping & argMap)
{
  foreach(const ArgMapsTo & argMapsTo, argMap){
    os << "  " << argMapsTo.callerArg << " -> " << argMapsTo.calledFun << "_" << argMapsTo.calleeArg << std::endl;
  }
  return os;
}

Signatures transitiveClosure( const Signatures & sigs ){
  // These two calls are expensive but necessary.
  // The first one computes the transitive closure of function calls.
  // The second one computes the transitive closure of parameter passing between
  // functions.
  //
  // At first pass it seems that the second graph could be used to copmute the first.
  // Unfortunately, this isn't true because a function f might call an api function
  // but not pass its parameters to the function.
  Signatures transitiveCallSigs(transitiveCalls(sigs));
  const Signatures transitiveParameterSigs(transitiveParameters(sigs));

  // Now we join the information from both of the above signatures into one mega-signature
  foreach(const Signatures::value_type & pair, transitiveParameterSigs){
    transitiveCallSigs[pair.first].argMapping = pair.second.argMapping;
  }

  return transitiveCallSigs;
}

// See the header file for details on what this does
Signatures transitiveCalls( const Signatures & sigs )
{
   // We use boost graph library (BGL) so that we don't have to do a bunch of
   // heavy lifting. Unfortunately, BGL makes this maximally annoying.
   using namespace boost;
   typedef adjacency_list< vecS, vecS, directedS, std::string > graph_t;
   typedef labeled_graph< graph_t, std::string > labeled_graph_t;

   // Use a set to compute the set of distinct function names
   // as the union of all the callers and callees
   std::set<std::string> function_names;
   foreach(const Signatures::value_type & pair, sigs){
      function_names.insert(pair.first);
      foreach(const std::string & callee, pair.second.calls){
         function_names.insert(callee);
      }
   }
   // We use g to compute the transitive closure.
   // Once we have it, we will extract the graph back to a
   // list of verticies (Signatures).
   labeled_graph_t g;

   // Add the vertices and build up a bijection between 
   // function names and their vertex id.
   foreach(const std::string & name, function_names){
      add_vertex(name, g);
      g[name] = name;
   }
   // Add all the edges
   foreach(const Signatures::value_type & pair, sigs){
      const std::string & caller = pair.first;
      foreach(const std::string & callee, pair.second.calls){
         add_edge_by_label(caller, callee, g);
      }
   }

   graph_t gt;
   // this is really wasteful, but we need a mapping between the verticies of g and gt
   std::vector<graph_t::vertex_descriptor> vertmap(num_vertices(g));

   // Finally we can compute the transitive closure of g,
   // BGL is weird about the way it takes the third parameter.
   // It takes the vertmap by value so we need to pass a pointer
   // or reference to something that supports operator[] and operator=
   // If we pass std::vector we are forced to pass it by value
   // so instead we pass the address of the underlying array (data()).
   // It's ugly but we don't have a lot of options.
   #ifdef _MSC_VER
      transitive_closure(g.graph(), gt, vertmap, get(vertex_index,g));
   #else
      transitive_closure(g.graph(), gt, vertmap.data(), get(vertex_index,g));
   #endif


#ifndef _MSC_VER
   // Emperically vertmap is always the identity map, what gives?
   // We'll be accessing both of these a lot, so lift them out of the loop
   {  // Create a new scope so that g_graph and vertmap_p are local to the
      // loop
      const graph_t & g_graph(g.graph());
      const graph_t::vertex_descriptor * const vertmap_p(vertmap.data());
      for(size_t i = 0; i < num_vertices(g); ++i){
         // vertmap maps from g's indices to gt's indices
         // so we're mapping, gt[j] = g[i], where j = vertmap[i]
         gt[vertmap_p[i]] = g_graph[i];
      }
   }
#endif

   Signatures sig_tc;
   // go over all the adjacency information in gt and put in the labels from g
   typedef graph_traits<graph_t>::vertex_iterator vertex_iterator;
   typedef graph_traits<graph_t>::adjacency_iterator adjacency_iterator;
   std::pair<vertex_iterator, vertex_iterator> vp;
   for (vp = vertices(gt); vp.first != vp.second; ++vp.first) {
      vertex_iterator caller_itr = vp.first;
      std::pair<adjacency_iterator, adjacency_iterator> pair = 
         adjacent_vertices(*caller_itr, gt);
      for(; pair.first != pair.second; ++pair.first){
         const std::string & caller = gt[*caller_itr];
         const std::string & callee = gt[*pair.first];
         sig_tc[caller].calls.insert(callee);
         // Note: This find should always succeed, so I haven't bothered with a check.
         const Signatures::const_iterator callerAt = sigs.find(caller);
         sig_tc[caller].argMapping = callerAt->second.argMapping;
         sig_tc[caller].argCount   = callerAt->second.argCount;
      }
   }

   return sig_tc;
}

// TODO: refactor this and transitiveCalls
Signatures transitiveParameters( const Signatures & sigs )
{
  // First we need to convert the signatures to a graph that represents
  // the way function parameters are passed. Edges look like this:
  // f_i -> g_j
  // This would say that f passes it's ith parameter as g's jth parameter.
   using namespace boost;
   typedef std::pair<ArgMapsTo::FunId, ArgMapsTo::ArgId> ArgPair;
   typedef adjacency_list< vecS, vecS, directedS
                         , ArgPair                                // Vertex data
                         > graph_t;
   typedef labeled_graph< graph_t
                        , ArgPair   // Repeated vertex data for easy lookup of edges
                        > labeled_graph_t;

   // Use a set to compute the set of distinct function names
   // as the union of all the callers and callees
   std::set< ArgPair > nodes;
   foreach(const Signatures::value_type & pair, sigs){
      ArgMapsTo::FunId callerId = pair.first;
      foreach(const ArgMapsTo & argMap, pair.second.argMapping){
         nodes.insert(std::make_pair(callerId,argMap.callerArg));
         nodes.insert(std::make_pair(argMap.calledFun,argMap.calleeArg));
      }
   }
   // We use g to propagate the roles.
   // Once we have them, we will extract the graph back to a
   // list of verticies (Signatures).
   labeled_graph_t g;

   // Add the vertices and build up a bijection between 
   // function names and their vertex id.
   foreach(const ArgPair & name, nodes){
      add_vertex(name, g);
      g[name] = name;
   }
   // Add all the edges
   foreach(const Signatures::value_type & pair, sigs){
     ArgMapsTo::FunId callerId = pair.first;
      foreach(const ArgMapsTo & argMap, pair.second.argMapping){
         add_edge_by_label(std::make_pair(callerId, argMap.callerArg)
                          ,std::make_pair(argMap.calledFun, argMap.calleeArg)
                          ,g);
      }
   }
   // copied from transitiveCalls
   graph_t gt;
   // this is really wasteful, but we need a mapping between the verticies of g and gt
   std::vector<graph_t::vertex_descriptor> vertmap(num_vertices(g));

   // Finally we can compute the transitive closure of g,
   // BGL is weird about the way it takes the third parameter.
   
   // It takes the vertmap by value so we need to pass a pointer
   // or reference to something that supports operator[] and operator=
   // If we pass std::vector we are forced to pass it by value
   // so instead we pass the address of the underlying array (data()).
   // It's ugly but we don't have a lot of options.
   #ifdef _MSC_VER
      transitive_closure(g.graph(), gt, vertmap, get(vertex_index,g));
   #else
      transitive_closure(g.graph(), gt, vertmap.data(), get(vertex_index,g));
   #endif

#ifndef _MSC_VER
   // Emperically vertmap is always the identity map, what gives?
   // We'll be accessing both of these a lot, so lift them out of the loop
   {  // Create a new scope so that g_graph and vertmap_p are local to the
      // loop
      const graph_t & g_graph(g.graph());
      const graph_t::vertex_descriptor * const vertmap_p(vertmap.data());
      for(size_t i = 0; i < num_vertices(g); ++i){
         // vertmap maps from g's indices to gt's indices
         // so we're mapping, gt[j] = g[i], where j = vertmap[i]
         gt[vertmap_p[i]] = g_graph[i];
      }
   }
#endif

   Signatures sig_tc;
   // go over all the adjacency information in gt and put in the labels from g
   typedef graph_traits<graph_t>::vertex_iterator vertex_iterator;
   typedef graph_traits<graph_t>::adjacency_iterator adjacency_iterator;
   std::pair<vertex_iterator, vertex_iterator> vp;
   for (vp = vertices(gt); vp.first != vp.second; ++vp.first) {
      vertex_iterator caller_itr = vp.first;
      std::pair<adjacency_iterator, adjacency_iterator> pair = adjacent_vertices(*caller_itr, gt);
      for(; pair.first != pair.second; ++pair.first){
         const ArgPair & caller = gt[*caller_itr];
         const ArgPair & callee = gt[*pair.first];
         const ArgMapsTo & argMap = ArgMapsTo(caller.second, callee.first, callee.second);
         sig_tc[caller.first].calls.insert(callee.first);
         sig_tc[caller.first].argMapping.push_back(argMap);
      }
   }

   return sig_tc;
  
}

// Checks for an edge between two parameters of functions.
// This is implemented as a O(ln n) lookup in the set of
// verticies and a linear scan among the parameters of the
// lookup functions.
bool
edgeExists( const Signatures & sigs
          , const ArgMapsTo::FunId & callerFun
          , const ArgMapsTo::ArgId & callerArg
          , const ArgMapsTo::FunId & calleeFun
          , const ArgMapsTo::ArgId & calleeArg )
{
  Signatures::const_iterator sig = sigs.find(callerFun);
  if( sig == sigs.end() ) return false;

  foreach( const ArgMapsTo & argMap, sig->second.argMapping ){
    if( argMap.calledFun == calleeFun &&
        argMap.callerArg == calleeArg ){
      return true;
    }
  }
  // there are no edges to see here, move along...
  return false;
}

void
buildAPISpecFromSignature( APISpecs & specs
                         , const Signatures & sigs )
{
  // TODO: This typeCode hack is not so great.
  // We need to find the right starting point for typeCode.
  // What would be even better is to be able to share typeCode
  // across instances of APISpec.
  uint32_t typeCode = 1;
  foreach( const APISpec * const spec, specs ){
    typeCode = std::max(typeCode, spec->nextTypeCode());
  }
  // construct an APISpec with the largest typeCode found
  APISpec * const newSpec = new APISpec(typeCode);
  // We need some junk type for things we don't care about. Let's call it
  // other. Ideally, this would be the same as 'other' defined in other
  // APISpecs. eg., shared across all APISpecs.
  GenericDepAttribute * const other = newSpec->registerType("other");

  // Now we have the glorious task of going over the function signatures
  // and copying over the GenericDepAttributes whenever there is an edge
  // that from a user-defined function to an API function.
  foreach(const Signatures::value_type & sig, sigs){
    const ArgMapsTo::FunId      & funId      = sig.first;
    const std::set<std::string> & calls      = sig.second.calls;
    const ArgMapping            & argMapping = sig.second.argMapping;
    const int                     argCount   = sig.second.argCount;

    // This loop considers each edge from funId to other functions
    // in the call graph
    foreach(const std::string & call, calls){
      // Filter for things that endup calling an API function.
      // Basically, if this is not an API function, go to the next edge.
      APISpec * const spec = lookupFunction(&specs, call);
      if( spec == NULL ) continue;

      // Start building the new APISpec entry.
      // At this point we don't know if the arguments of the caller
      // should be marked with special attributes but what we do know
      // is that the caller calls an API function so we can at least
      // add this function into the API spec that we're building.
      ArgTreatments * const argTreats = newSpec->addFunction(funId
                          , argCount
                          , other);

      // Now we need to look at the attributes of each argument of the
      // callee (the destination).
      for(size_t i = 0; i < argMapping.size(); ++i){
        const ArgMapsTo & argMap = argMapping[i];
        // If the callee doesn't have API info then we just move on...
        APISpec * const spec = lookupFunction(&specs, argMap.calledFun);
        if( spec == NULL ) continue;

        // At this point we have a user-defined function calling an API function
        // and passing info.
        const ArgTreatments * const calleeArgTreats = spec->getFunction(argMap.calledFun);

        // This check is only here because we're paranoid.
        if( argMap.calleeArg < calleeArgTreats->size() &&
            argMap.callerArg < argTreats->size()       ){
          // Fetch the attributes for ts[argMap.calleeArg] and copy them to
          // be the attributes of (funId, argMap.callerArg])
          GenericDepAttribute * const depAttr = (*argTreats)[argMap.callerArg]->copy();
          depAttr->join((*calleeArgTreats)[argMap.calleeArg]);
          (*argTreats)[argMap.callerArg] = depAttr;
        }
      }
    }
  }
  specs.push_back(newSpec);
}
