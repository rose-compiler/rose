
#ifndef __POLYGRAPH_HPP__
#  define __POLYGRAPH_HPP__

#include "rose-kernel/PolyhedralKernel.hpp"

#include <iostream>
#include <string>

struct isl_ctx;
struct isl_printer;
struct isl_set;
struct isl_map;
struct isl_multi_aff;

class ParseTree;

class PolyStatement;
class PolyDependency;
class FlowGraph;

class PolyGraph {
  public:
    typedef unsigned VarID;
    typedef unsigned StmtID;
    typedef unsigned DepID;

  private:
    std::string name;

    std::map<VarID, FlowGraph *> variables;
    std::map<StmtID, PolyStatement *> statements;
    std::map<DepID, PolyDependency *> dependencies;

    isl_ctx * context;
    isl_printer * printer;

    std::map<VarID, std::string> var_names;

  public:
    PolyGraph(const PolyhedricAnnotation::FunctionPolyhedralProgram & function_program, std::string name_);
    ~PolyGraph();

    void print(std::ostream & out) const;
    std::string toString() const;

    void toDot(std::ostream & out) const;
    void varToDot(std::ostream & out) const;

  friend class PolyStatement;
  friend class PolyDependency;

  friend class Data;
};

class PolyStatement {
  private:
    PolyGraph * polygraph;

    PolyGraph::StmtID id;

    isl_set * domain;
    isl_map * scattering;
    ParseTree * parse_tree;

  public:
    PolyStatement(
      PolyGraph * polygraph_,
      PolyGraph::StmtID id_,
      SgExprStatement * expr_stmt,
      const PolyhedricAnnotation::FunctionPolyhedralProgram & function_program
    );
    ~PolyStatement();

    void print(std::ostream & out) const;
    std::string toString() const;
    void toDot(std::ostream & out) const;

  friend class PolyGraph;
};

class PolyDependency {
  public:
    enum DepType {RaR, RaW, WaR, WaW, None};

  private:
    PolyGraph * polygraph;

    PolyGraph::DepID id;

    PolyGraph::StmtID source;
    PolyGraph::StmtID sink;

    PolyGraph::VarID variable;

    DepType type;

    isl_map * dependency;

  public:
    PolyDependency(
      PolyGraph * polygraph_,
      PolyGraph::DepID id_,
      PolyhedricDependency::FunctionDependency * dependency,
      const PolyhedricAnnotation::FunctionPolyhedralProgram & function_program
    );
    ~PolyDependency();


    void print(std::ostream & out) const;
    std::string toString() const;
    void toDot(std::ostream & out) const;

  friend class PolyGraph;
};

class FlowGraph {
  private:
    PolyGraph::VarID variable;

    unsigned dimension;

    std::map<std::pair<PolyGraph::StmtID, PolyGraph::StmtID>, unsigned> rar;
    std::map<std::pair<PolyGraph::StmtID, PolyGraph::StmtID>, unsigned> raw;
    std::map<std::pair<PolyGraph::StmtID, PolyGraph::StmtID>, unsigned> war;
    std::map<std::pair<PolyGraph::StmtID, PolyGraph::StmtID>, unsigned> waw;

  public:
    FlowGraph(PolyGraph::VarID var, unsigned dim);
    ~FlowGraph();

    void addRaR(PolyGraph::StmtID source, PolyGraph::StmtID sink);
    void addRaW(PolyGraph::StmtID source, PolyGraph::StmtID sink);
    void addWaR(PolyGraph::StmtID source, PolyGraph::StmtID sink);
    void addWaW(PolyGraph::StmtID source, PolyGraph::StmtID sink);

    void print(std::ostream & out) const;
    std::string toString() const;

    void toDot(std::ostream & out) const;

  friend class PolyGraph;
};

std::string islToMultiline(char * chr);

#endif /* __POLYGRAPH_HPP__ */

