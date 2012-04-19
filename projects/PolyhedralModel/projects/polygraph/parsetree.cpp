
#include "parsetree.hpp"

#include <sstream>

#include <isl/map.h>

#include "polygraph.hpp"

#include "rose/Parser.hpp"

ParseTree::ParseTree(PolyGraph * polygraph_) : polygraph(polygraph_) {}

ParseTree::~ParseTree() {}

std::string ParseTree::toString() const {
  std::ostringstream res;
  print(res);
  return res.str();
}

ParseTree * ParseTree::extractParseTree(
  SgExpression * exp,
  const PolyhedricAnnotation::FunctionPolyhedralProgram & function_program,
  PolyGraph * polygraph,
  std::string isl_domain,
  unsigned stmt_id
) {
  SgBinaryOp * bin_op = isSgBinaryOp(exp);
  SgUnaryOp * una_op = isSgUnaryOp(exp);
  SgConditionalExp * cond_exp = isSgConditionalExp(exp);
  SgFunctionCallExp * func_call = isSgFunctionCallExp(exp);

  ParseTree * c1 = NULL;
  ParseTree * c2 = NULL;
  ParseTree * c3 = NULL;

  unsigned variant = exp->variantT();

  if (isSgPntrArrRefExp(exp)) bin_op = NULL;

  if (bin_op != NULL) {
    c1 = extractParseTree(bin_op->get_lhs_operand_i(), function_program, polygraph, isl_domain, stmt_id);
    c2 = extractParseTree(bin_op->get_rhs_operand_i(), function_program, polygraph, isl_domain, stmt_id);
  }
  else if (una_op != NULL) {
    c1 = extractParseTree(una_op->get_operand_i(), function_program, polygraph, isl_domain, stmt_id);
  }
  else if (cond_exp != NULL) {
    c1 = extractParseTree(cond_exp->get_conditional_exp(), function_program, polygraph, isl_domain, stmt_id);
    c2 = extractParseTree(cond_exp->get_true_exp(), function_program, polygraph, isl_domain, stmt_id);
    c3 = extractParseTree(cond_exp->get_false_exp(), function_program, polygraph, isl_domain, stmt_id);
  }
  else if (func_call != NULL) {
    std::vector<SgExpression *> args = func_call->get_args()->get_expressions();
    ROSE_ASSERT(args.size() == 1);
    c1 = extractParseTree(args[0], function_program, polygraph, isl_domain, stmt_id);
  }
  else {
    return new Data(polygraph, exp, function_program, isl_domain);
  }

  return new Operator(polygraph, variant, c1, c2, c3);
}

Operator::Operator(PolyGraph * polygraph_, unsigned id_, ParseTree * c1, ParseTree * c2, ParseTree * c3) :
  ParseTree(polygraph_),
  id(id_),
  child1(c1),
  child2(c2),
  child3(c3)
{}

Operator::~Operator() {
  if (child1 != NULL) delete child1;
  if (child2 != NULL) delete child2;
  if (child3 != NULL) delete child3;
}

void Operator::print(std::ostream & out) const {
  out << "( " << id;
  if (child1 != NULL) {
    out << " ";
    child1->print(out);
  }
  if (child2 != NULL) {
    out << " ";
    child2->print(out);
  }
  if (child3 != NULL) {
    out << " ";
    child3->print(out);
  }
  out << " )";
}

void Operator::toDot(std::ostream & out) const {
  std::ostringstream node;
  node << "ptn_" << this;
  out << node.str() << " [label=\"" << id << "\"]" << std::endl;
  if (child1 != NULL) {
    std::ostringstream child;
    child << "ptn_" << child1;
    out << node.str() << " -> " << child.str() << std::endl;
    child1->toDot(out);
  }
  if (child2 != NULL) {
    std::ostringstream child;
    child << "ptn_" << child2;
    out << node.str() << " -> " << child.str() << std::endl;
    child2->toDot(out);
  }
  if (child3 != NULL) {
    std::ostringstream child;
    child << "ptn_" << child3;
    out << node.str() << " -> " << child.str() << std::endl;
    child3->toDot(out);
  }
}

std::string sageExpToIslAff(SgExpression * exp) {
  std::vector<std::pair<RoseVariable, int> > lin_exp;
  PolyhedricAnnotation::translateLinearExpression(exp, lin_exp);
  std::ostringstream aff;
  std::vector<std::pair<RoseVariable, int> >::iterator it = lin_exp.begin();
  while (it != lin_exp.end()) {
    if (isConstant(it->first))
      aff << it->second;
    else
      aff << it->second << "*" << it->first.getString();
    it++;
    if (it != lin_exp.end())
      aff << " + ";
  }
  return aff.str();
}

Data::Data(
  PolyGraph * polygraph_, SgExpression * exp,
  const PolyhedricAnnotation::FunctionPolyhedralProgram & function_program,
  std::string isl_domain
) :
  ParseTree(polygraph_),
  id(0),
  access(NULL)
{
  if (!isSgValueExp(exp)) {
    try {
      SgExpression * last = exp;
      SgPntrArrRefExp * arr_ref = isSgPntrArrRefExp(last);
      std::vector<std::string> subscripts;
      while (arr_ref != NULL) {
        subscripts.insert(subscripts.begin(), sageExpToIslAff(arr_ref->get_rhs_operand_i()));
        last = arr_ref->get_lhs_operand_i();
        arr_ref = isSgPntrArrRefExp(last);
      }
      if (subscripts.size() > 0) {
        std::ostringstream map;
        map << "{ " << isl_domain << " -> [";
        for (int i = 0; i < subscripts.size(); i++) {
          map << subscripts[i];
          if (i != subscripts.size() - 1) map << ", "; 
        }
        map << "] }";
        access = isl_map_read_from_str(polygraph->context, map.str().c_str());
      }
      RoseVariable * var = PolyhedricAnnotation::translateVarRef(last);
      id = function_program.getVariableID(*var);
      std::cerr << var << " -> " << var->getString() << " -> " << id << std::endl;
      delete var;
    }
    catch (Exception::ExceptionBase & e) {
      e.print(std::cerr);
      exit(-1);
    }
  }
}

Data::~Data() {
  if (access != NULL) {
    isl_map_free(access);
  }
}

unsigned Data::getID() const {
  return id;
}

void Data::print(std::ostream & out) const {
  out << "[ " << id;
  if (access != NULL) {
    polygraph->printer = isl_printer_print_map(polygraph->printer, access);
    out << " " << isl_printer_get_str(polygraph->printer);
    polygraph->printer = isl_printer_flush(polygraph->printer);
  }
  out << " ]";
}

void Data::toDot(std::ostream & out) const {
  std::ostringstream node;
  node << "ptn_" << this;
  if (access != NULL) {
    polygraph->printer = isl_printer_print_map(polygraph->printer, access);
    out << node.str() << " [label=\"" << id << "\\n" << islToMultiline(isl_printer_get_str(polygraph->printer)) << "\"]" << std::endl;
    polygraph->printer = isl_printer_flush(polygraph->printer); 
  }
  else
    out << node.str() << " [label=\"" << id << "\"]" << std::endl;
}

