
#include "polygraph.hpp"

#include "parsetree.hpp"

#include "isl/map.h"
#include "isl/set.h"
#include "isl/aff.h"

#ifndef DEBUG
#  define DEBUG 1
#endif

#ifndef DEBUG_POLYGRAPH_CTOR
#  define DEBUG_POLYGRAPH_CTOR DEBUG
#endif

std::string islToMultiline(char * chr) {
  std::string res(chr+2);
  size_t pos = res.find(" : ", 0);
  if (pos != std::string::npos)
    res = res.replace(pos, 3, "\\l      ");
  pos = res.find(" and ", 0);
  while (pos != std::string::npos) {
    res = res.replace(pos, 5, "\\l      ");
    pos = res.find(" and ", 0);
  }
  pos = res.find(" }", 0);
  if (pos != std::string::npos)
    res = res.replace(pos, 2, "\\l");

  return res;
}

PolyGraph::PolyGraph(const PolyhedricAnnotation::FunctionPolyhedralProgram & function_program, std::string name_) :
  name(name_),
  variables(),
  statements(),
  dependencies(),
  context(NULL),
  printer(NULL),
  var_names()
{
#if DEBUG_POLYGRAPH_CTOR
  std::cerr << "Start of PolyGraph CTOR" << std::endl;
#endif

  context = isl_ctx_alloc();
  printer = isl_printer_to_str(context);

#if DEBUG_POLYGRAPH_CTOR
  std::cerr << "  Variables" << std::endl;
#endif

  for (int i = 1; i <= function_program.getNumberOfVariables(); i++) {
    RoseVariable var = function_program.getVariableByID(i);
    SgInitializedName * init_name = var.getInitializedName();
    ROSE_ASSERT(init_name);
    SgType * type = init_name->get_typeptr();
    ROSE_ASSERT(type != NULL);
    unsigned dim = 0;
    while (isSgArrayType(type)) {
      dim++;
      type = isSgArrayType(type)->get_base_type();
    }
    variables.insert(std::pair<VarID, FlowGraph *>(i, new FlowGraph(i, dim)));
    var_names.insert(std::pair<VarID, std::string>(i, var.getString()));
  }

#if DEBUG_POLYGRAPH_CTOR
  std::cerr << "  Statements" << std::endl;
#endif

  const std::vector<SgExprStatement *> & stmts = function_program.getExpressions();
  std::vector<SgExprStatement *>::const_iterator it_stmt;
  for (it_stmt = stmts.begin(); it_stmt != stmts.end(); it_stmt++) {
    StmtID id = function_program.getStatementID(*it_stmt);
    statements.insert(std::pair<StmtID, PolyStatement *>(id, new PolyStatement(this, id, *it_stmt, function_program)));
  }

#if DEBUG_POLYGRAPH_CTOR
  std::cerr << "  Dependencies" << std::endl;
#endif

  try {
    std::vector<PolyhedricDependency::FunctionDependency *> * deps;
    std::vector<PolyhedricDependency::FunctionDependency *>::iterator it_dep;
    DepID id = 0;

#if DEBUG_POLYGRAPH_CTOR
  std::cerr << "    RaW" << std::endl;
#endif

    deps = PolyhedricDependency::ComputeRaW<SgFunctionDeclaration, SgExprStatement, RoseVariable>(function_program, true);
    for (it_dep = deps->begin(); it_dep != deps->end(); it_dep++) {
      PolyDependency * dep = new PolyDependency(this, id, *it_dep, function_program);
      delete *it_dep;

      dependencies.insert(std::pair<DepID, PolyDependency *>(id++, dep));
      std::map<VarID, FlowGraph *>::iterator it_var = variables.find(dep->variable);
      ROSE_ASSERT(it_var != variables.end());
      it_var->second->addRaW(dep->sink, dep->source);
    }
    delete deps;

#if DEBUG_POLYGRAPH_CTOR
  std::cerr << "    WaR" << std::endl;
#endif

    deps = PolyhedricDependency::ComputeWaR<SgFunctionDeclaration, SgExprStatement, RoseVariable>(function_program, false);
    for (it_dep = deps->begin(); it_dep != deps->end(); it_dep++) {
      std::map<VarID, FlowGraph *>::iterator it_var = variables.find((*it_dep)->getVariable());
      ROSE_ASSERT(it_var != variables.end());
      it_var->second->addWaR((*it_dep)->getTo(), (*it_dep)->getFrom());
      delete *it_dep;
    }
    delete deps;

#if DEBUG_POLYGRAPH_CTOR
  std::cerr << "    WaW" << std::endl;
#endif

    deps = PolyhedricDependency::ComputeWaW<SgFunctionDeclaration, SgExprStatement, RoseVariable>(function_program, true);
    for (it_dep = deps->begin(); it_dep != deps->end(); it_dep++) {
      std::map<VarID, FlowGraph *>::iterator it_var = variables.find((*it_dep)->getVariable());
      ROSE_ASSERT(it_var != variables.end());
      it_var->second->addWaW((*it_dep)->getTo(), (*it_dep)->getFrom());
      delete *it_dep;
    }
    delete deps;

#if DEBUG_POLYGRAPH_CTOR
  std::cerr << "    RaR" << std::endl;
#endif

    deps = PolyhedricDependency::ComputeRaR<SgFunctionDeclaration, SgExprStatement, RoseVariable>(function_program, false);
    for (it_dep = deps->begin(); it_dep != deps->end(); it_dep++) {
      std::map<VarID, FlowGraph *>::iterator it_var = variables.find((*it_dep)->getVariable());
      ROSE_ASSERT(it_var != variables.end());
      it_var->second->addRaR((*it_dep)->getTo(), (*it_dep)->getFrom());
      delete *it_dep;
    }
    delete deps;
  }
  catch (Exception::ExceptionBase & e) {
    e.print(std::cerr);
  }
#if DEBUG_POLYGRAPH_CTOR
  std::cerr << "End of PolyGraph CTOR" << std::endl;
#endif
}

PolyGraph::~PolyGraph() {}

void PolyGraph::print(std::ostream & out) const {
  out << "# PolyGraph" << std::endl;
  out << "# " << std::endl;

  out << "# Variables flow-graph:" << std::endl;
  std::map<VarID, FlowGraph *>::const_iterator it_var;
  for (it_var = variables.begin(); it_var !=  variables.end(); it_var++) {
    it_var->second->print(out);
    out << std::endl;
  }

  out << "# Statements:" << std::endl;
  std::map<StmtID, PolyStatement *>::const_iterator it_stmt;
  for (it_stmt = statements.begin(); it_stmt != statements.end(); it_stmt++) {
    it_stmt->second->print(out);
    out << std::endl;
  }

  out << "# Dependencies:" << std::endl;
  std::map<DepID, PolyDependency *>::const_iterator it_dep;
  for (it_dep = dependencies.begin(); it_dep != dependencies.end(); it_dep++) {
    it_dep->second->print(out);
    out << std::endl;
  }
}

std::string PolyGraph::toString() const {
  std::ostringstream res;
  print(res);
  return res.str();
}

void PolyGraph::toDot(std::ostream & out) const {
  out << "digraph {" << std::endl;
  varToDot(out);
  out << "subgraph cluster_statements {" << std::endl;
  std::map<StmtID, PolyStatement *>::const_iterator it_stmt;
  for (it_stmt = statements.begin(); it_stmt != statements.end(); it_stmt++) {
    it_stmt->second->toDot(out);
  }
  std::map<DepID, PolyDependency *>::const_iterator it_dep;
  for (it_dep = dependencies.begin(); it_dep != dependencies.end(); it_dep++) {
    it_dep->second->toDot(out);
  }
  out << "}" << std::endl;
  out << "}" << std::endl;
}

void PolyGraph::varToDot(std::ostream & out) const {
  out << "subgraph cluster_variables {" << std::endl;
  out << "label=\"Dependency graph foreach variable\"" << std::endl;
  std::map<VarID, FlowGraph *>::const_iterator it_var;
  for (it_var = variables.begin(); it_var !=  variables.end(); it_var++) {
    out << "subgraph cluster_variables_" << it_var->first << " {" << std::endl;
    out << "  label=\"" << var_names.find(it_var->first)->second;
    for (int i = 0; i < it_var->second->dimension; i++) out << "[]";
    out << "\\nID=" << it_var->first << "\"" << std::endl;
    it_var->second->toDot(out);
    out << "}" << std::endl;
  }
  out << "}" << std::endl;
}

PolyStatement::PolyStatement(
  PolyGraph * polygraph_,
  PolyGraph::StmtID id_,
  SgExprStatement * expr_stmt,
  const PolyhedricAnnotation::FunctionPolyhedralProgram & function_program
) :
  polygraph(polygraph_),
  id(id_),
  domain(NULL),
  scattering(NULL),
  parse_tree(NULL)
{
  PolyhedricAnnotation::FunctionDomain & domain_ =
      PolyhedricAnnotation::getDomain<SgFunctionDeclaration, SgExprStatement, RoseVariable>(expr_stmt);
  size_t nbr_it = domain_.getNumberOfIterators();
  std::ostringstream space_str;
  space_str << "[";
  for (int i = 0; i < nbr_it; i++) {
    space_str << function_program.getIteratorById(expr_stmt, i).getString();
    if (i != nbr_it - 1) space_str << ",";
  }
  space_str << "]";
  {
    const Polyhedron & poly_domain = domain_.getDomainPolyhedron();
    const ConstraintSystem & cs = poly_domain.constraints();
    std::ostringstream domain_str;
    domain_str << "{ " << space_str.str() << " : ";
    ConstraintSystem::const_iterator it = cs.begin();
    while (it != cs.end()) {
      for (int i = 0; i < nbr_it; i++) {
        domain_str << "(" << it->coefficient(VariableID(i)).get_si() << "*" << function_program.getIteratorById(expr_stmt, i).getString() << ") + ";
      }
      domain_str << it->inhomogeneous_term().get_si();
      if (it->is_equality())
        domain_str << " = 0";
      else
        domain_str << " >= 0";
      it++;
      if (it != cs.end())
        domain_str << " and ";
    }
    domain_str << " }";
    domain = isl_set_read_from_str(polygraph->context, domain_str.str().c_str());
  }
  {
    PolyhedricAnnotation::FunctionScattering & scattering_ =
        PolyhedricAnnotation::getScattering<SgFunctionDeclaration, SgExprStatement, RoseVariable>(expr_stmt);
    const std::vector<LinearExpression_ppl> & poly_scattering = scattering_.getScattering();
    isl_aff_list * list = isl_aff_list_alloc(polygraph->context, poly_scattering.size());
    std::ostringstream map;
    map << "{ " << space_str.str() << " -> [";
    std::vector<LinearExpression_ppl>::const_iterator it = poly_scattering.begin();
    while (it != poly_scattering.end()) {
      for (int i = 0; i < nbr_it; i++) {
        map << "(" << it->coefficient(VariableID(i)).get_si() << "*" << function_program.getIteratorById(expr_stmt, i).getString() << ") + ";
      }
      map << it->inhomogeneous_term().get_si();
      it++;
      if (it != poly_scattering.end()) map << ",";
    }
    map << " ] }";
    scattering = isl_map_read_from_str(polygraph->context, map.str().c_str());
  }
  parse_tree = ParseTree::extractParseTree(expr_stmt->get_expression(), function_program, polygraph, space_str.str(), id);
}

PolyStatement::~PolyStatement() {
  if (domain != NULL)
    isl_set_free(domain);
  if (scattering != NULL)
    isl_map_free(scattering);
  if (parse_tree != NULL)
    delete parse_tree;
}

void PolyStatement::print(std::ostream & out) const {
  out << id << " # id" << std::endl;
  if (domain != NULL) {
    polygraph->printer = isl_printer_print_set(polygraph->printer, domain);
    out << isl_printer_get_str(polygraph->printer) << " # domain" << std::endl;
    polygraph->printer = isl_printer_flush(polygraph->printer);
  }
  if (scattering != NULL) {
    polygraph->printer = isl_printer_print_map(polygraph->printer, scattering);
    out << isl_printer_get_str(polygraph->printer) << " # scattering" << std::endl;
    polygraph->printer = isl_printer_flush(polygraph->printer);
  }
  if (parse_tree != NULL) {
    parse_tree->print(out);
    out << " # parse tree" << std::endl;
  }
}

std::string PolyStatement::toString() const {
  std::ostringstream res;
  print(res);
  return res.str();
}

void PolyStatement::toDot(std::ostream & out) const {
  out << "subgraph cluster_stmt_" << id << " {" << std::endl;
  out << "label=\"Stmt " << id << "\"" << std::endl;
  out << "domain_stmt_" << id << "     [label=\"";
  if (domain != NULL) {
    polygraph->printer = isl_printer_print_set(polygraph->printer, domain);
    out << islToMultiline(isl_printer_get_str(polygraph->printer));
    polygraph->printer = isl_printer_flush(polygraph->printer);
  }
  out << "\", shape=box, labeljust=l]" << std::endl;
  out << "scattering_stmt_" << id << " [label=\"";
  if (scattering != NULL) {
    polygraph->printer = isl_printer_print_map(polygraph->printer, scattering);
    out << islToMultiline(isl_printer_get_str(polygraph->printer));
    polygraph->printer = isl_printer_flush(polygraph->printer);
  }
  out << "\", shape=box, labeljust=l]" << std::endl;
  out << "subgraph cluster_parse_tree_" << id << " {" << std::endl;
  out << "label=\" \"" << std::endl;
  parse_tree->toDot(out);
  out << "}" << std::endl;
  out << "}" << std::endl;
}

PolyDependency::PolyDependency(
  PolyGraph * polygraph_,
  PolyGraph::DepID id_,
  PolyhedricDependency::FunctionDependency * dependency_,
  const PolyhedricAnnotation::FunctionPolyhedralProgram & function_program
) :
  polygraph(polygraph_),
  id(id_),
  source(dependency_->getFrom()),
  sink(dependency_->getTo()),
  variable(dependency_->getVariable()),
  type(PolyDependency::RaW), // FIXME not a problem now as we only build PolyDependency for RaW deps.
  dependency(NULL)
{
  SgExprStatement * source_stmt = function_program.getStatementById(source);
  SgExprStatement * sink_stmt = function_program.getStatementById(sink);

  size_t dim_source = PolyhedricAnnotation::getDomain<SgFunctionDeclaration, SgExprStatement, RoseVariable>(source_stmt).getNumberOfIterators();
  size_t dim_sink = PolyhedricAnnotation::getDomain<SgFunctionDeclaration, SgExprStatement, RoseVariable>(sink_stmt).getNumberOfIterators();

  const ConstraintSystem & cs = dependency_->getPolyhedron().constraints();

  std::ostringstream map; 

  map << "{ [";
  for (int i = 0; i < dim_source; i++) {
    map << function_program.getIteratorById(source_stmt, i) << "_1";
    if (i != dim_source - 1) map << ",";
  }
  map << "] -> [";
  for (int i = 0; i < dim_sink; i++) {
    map << function_program.getIteratorById(sink_stmt, i) << "_2";
    if (i != dim_sink - 1) map << ",";
  }
  map << "] : ";
  ConstraintSystem::const_iterator it = cs.begin();
  while (it != cs.end()) {
    for (int i = 0; i < dim_sink; i++)
      map << it->coefficient(VariableID(i)).get_si() << "*" << function_program.getIteratorById(sink_stmt, i) << "_2 + ";
    for (int i = 0; i < dim_source; i++)
      map << it->coefficient(VariableID(dim_sink + i)).get_si() << "*" << function_program.getIteratorById(source_stmt, i) << "_1 + ";
    map << it->inhomogeneous_term().get_si();
    if (it->is_equality())
      map << " = 0";
    else
      map << " >= 0";

    it++;
    if (it != cs.end()) map << " and ";
  }
  map << " }";
  dependency = isl_map_read_from_str(polygraph->context, map.str().c_str());
}

PolyDependency::~PolyDependency() {
  if (dependency != NULL)
    isl_map_free(dependency);
}

void PolyDependency::print(std::ostream & out) const {
  out << id << " # id" << std::endl;
  out << source << " # source" << std::endl;
  out << sink << " # sink" << std::endl;
  out << variable << " # variable" << std::endl;
  switch (type) {
    case PolyDependency::RaR:
      out << "RaR" << std::endl; break;
    case PolyDependency::RaW:
      out << "RaW" << std::endl; break;
    case PolyDependency::WaR:
      out << "WaR" << std::endl; break;
    case PolyDependency::WaW:
      out << "WaW" << std::endl; break;
    case PolyDependency::None:
      out << "None" << std::endl; break;
  }
  if (dependency != NULL) {
    polygraph->printer = isl_printer_print_map(polygraph->printer, dependency);
    out << isl_printer_get_str(polygraph->printer) << " # dependency" << std::endl;
    polygraph->printer = isl_printer_flush(polygraph->printer);
  }
}

std::string PolyDependency::toString() const {
  std::ostringstream res;
  print(res);
  return res.str();
}

void PolyDependency::toDot(std::ostream & out) const {
  out << "dependency_" << id << " [label=\"";
  if (dependency != NULL) {
    polygraph->printer = isl_printer_print_map(polygraph->printer, dependency);
    out << islToMultiline(isl_printer_get_str(polygraph->printer));
    polygraph->printer = isl_printer_flush(polygraph->printer);
  }
  out << "\", labeljust=l]" << std::endl;
  out << "domain_stmt_" << source << " -> dependency_" << id << " [lhead=cluster_stmt_" << source << "]" << std::endl;
  out << "dependency_" << id << " -> domain_stmt_" << sink << " [ltail=cluster_stmt_" << sink << "]" << std::endl;
}

FlowGraph::FlowGraph(PolyGraph::VarID var, unsigned dim) :
  variable(var),
  dimension(dim),
  rar(),
  raw(),
  war(),
  waw()
{}

FlowGraph::~FlowGraph() {}

void insertDep(std::map<std::pair<PolyGraph::StmtID, PolyGraph::StmtID>, unsigned> & map, PolyGraph::StmtID source, PolyGraph::StmtID sink) {
  std::pair<PolyGraph::StmtID, PolyGraph::StmtID> edge = std::pair<PolyGraph::StmtID, PolyGraph::StmtID>(source,sink);
  std::map<std::pair<PolyGraph::StmtID, PolyGraph::StmtID>, unsigned>::iterator it = map.find(edge);
  if (it == map.end())
    map.insert(std::pair<std::pair<PolyGraph::StmtID, PolyGraph::StmtID>, unsigned>(edge, 1));
  else
    it->second++;
}

void FlowGraph::addRaR(PolyGraph::StmtID source, PolyGraph::StmtID sink) {
  insertDep(rar, source, sink);
}

void FlowGraph::addRaW(PolyGraph::StmtID source, PolyGraph::StmtID sink) {
  insertDep(raw, source, sink);
}

void FlowGraph::addWaR(PolyGraph::StmtID source, PolyGraph::StmtID sink) {
  insertDep(war, source, sink);
}

void FlowGraph::addWaW(PolyGraph::StmtID source, PolyGraph::StmtID sink) {
  insertDep(waw, source, sink);
}

void FlowGraph::print(std::ostream & out) const {
  out << variable << " # variable" << std::endl;
  out << dimension << " # dimension" << std::endl;
  std::map<std::pair<PolyGraph::StmtID, PolyGraph::StmtID>, unsigned>::const_iterator it;
  for (it = rar.begin(); it != rar.end(); it++)
    out << "( " << it->first.first << " -> " << it->first.second << ", " << it->second << " ) ";
  out << " # RaR" << std::endl;
  for (it = raw.begin(); it != raw.end(); it++)
    out << "( " << it->first.first << " -> " << it->first.second << ", " << it->second << " ) ";
  out << " # RaW" << std::endl;
  for (it = war.begin(); it != war.end(); it++)
    out << "( " << it->first.first << " -> " << it->first.second << ", " << it->second << " ) ";
  out << " # WaR" << std::endl;
  for (it = waw.begin(); it != waw.end(); it++)
    out << "( " << it->first.first << " -> " << it->first.second << ", " << it->second << " ) ";
  out << " # WaW" << std::endl;
}

std::string FlowGraph::toString() const {
  std::ostringstream res;
  print(res);
  return res.str();
}

void FlowGraph::toDot(std::ostream & out) const {
  std::set<std::string>::iterator itn;
  std::map<std::pair<PolyGraph::StmtID, PolyGraph::StmtID>, unsigned>::const_iterator it;
  {
    out << "subgraph cluster_" << this << "_rar {" << std::endl;
    out << "label=\"RaR\"" << std::endl;
    std::set<std::string> nodes;
    for (it = rar.begin(); it != rar.end(); it++) {
      std::ostringstream n_in;   n_in << "n_" << this << "_rar_" << variable << "_" << it->first.first;
      std::ostringstream n_out; n_out << "n_" << this << "_rar_" << variable << "_" << it->first.second;
      out << n_in.str() << " -> " << n_out.str() << " [label=\"" << it->second << "\"]" << std::endl;
      nodes.insert(n_in.str()); nodes.insert(n_out.str());
    }
    for (itn = nodes.begin(); itn != nodes.end(); itn++) out << *itn << "[label=\" \"]" << std::endl;
    out << "}" << std::endl;
  }
  {
    out << "subgraph cluster_" << this << "_raw {" << std::endl;
    out << "label=\"RaW\"" << std::endl;
    std::set<std::string> nodes;
    for (it = raw.begin(); it != raw.end(); it++) {
      std::ostringstream n_in;   n_in << "n_" << this << "_raw_" << variable << "_" << it->first.first;
      std::ostringstream n_out; n_out << "n_" << this << "_raw_" << variable << "_" << it->first.second;
      out << n_in.str() << " -> " << n_out.str() << " [label=\"" << it->second << "\"]" << std::endl;
      nodes.insert(n_in.str()); nodes.insert(n_out.str());
    }
    for (itn = nodes.begin(); itn != nodes.end(); itn++) out << *itn << "[label=\" \"]" << std::endl;
    out << "}" << std::endl;
  }
  {
    out << "subgraph cluster_" << this << "_war {" << std::endl;
    out << "label=\"WaR\"" << std::endl;
    std::set<std::string> nodes;
    for (it = war.begin(); it != war.end(); it++) {
      std::ostringstream n_in;   n_in << "n_" << this << "_war_" << variable << "_" << it->first.first;
      std::ostringstream n_out; n_out << "n_" << this << "_war_" << variable << "_" << it->first.second;
      out << n_in.str() << " -> " << n_out.str() << " [label=\"" << it->second << "\"]" << std::endl;
      nodes.insert(n_in.str()); nodes.insert(n_out.str());
    }
    for (itn = nodes.begin(); itn != nodes.end(); itn++) out << *itn << "[label=\" \"]" << std::endl;
    out << "}" << std::endl;
  }
  {
    out << "subgraph cluster_" << this << "_waw {" << std::endl;
    out << "label=\"WaW\"" << std::endl;
    std::set<std::string> nodes;
    for (it = waw.begin(); it != waw.end(); it++) {
      std::ostringstream n_in;   n_in << "n_" << this << "_waw_" << variable << "_" << it->first.first;
      std::ostringstream n_out; n_out << "n_" << this << "_waw_" << variable << "_" << it->first.second;
      out << n_in.str() << " -> " << n_out.str() << " [label=\"" << it->second << "\"]" << std::endl;
      nodes.insert(n_in.str()); nodes.insert(n_out.str());
    }
    for (itn = nodes.begin(); itn != nodes.end(); itn++) out << *itn << "[label=\" \"]" << std::endl;
    out << "}" << std::endl;
  }
}

