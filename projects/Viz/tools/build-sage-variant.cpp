
#include "jsonxx.h"
#include <fstream>
#include <cassert>

struct sage_tree_t {
  std::string name;

  sage_tree_t * parent;
  std::vector<sage_tree_t *> children;

  sage_tree_t(const std::string & name_, sage_tree_t * parent_ = NULL) :
    name(name_),
    parent(parent_),
    children()
  {}

  ~sage_tree_t() {
    std::vector<sage_tree_t *>::iterator it_child;
    for (it_child = children.begin(); it_child != children.end(); it_child++)
      delete *it_child;
  }

  sage_tree_t * newChild(const std::string & name) {
    assert(this != NULL);
    sage_tree_t * res = new sage_tree_t(name, this);
    children.push_back(res);
    return res;
  }

  void toGraphViz(std::ostream & out) const {
    out << name << "[shape=box];" << std::endl;
    std::vector<sage_tree_t *>::const_iterator it_child;
    for (it_child = children.begin(); it_child != children.end(); it_child++) {
      (*it_child)->toGraphViz(out);
      out << name << " -> " << (*it_child)->name << ";" << std::endl;
    }
  }

  void toNameVariantMap(std::ostream & out, std::string indent) const {
    if (parent != NULL)
      out << indent << "name_variant_map.insert(std::pair<std::string, VariantT>(\"" << name << "\", V_" << name << "));" << std::endl;

    std::vector<sage_tree_t *>::const_iterator it_child;
    for (it_child = children.begin(); it_child != children.end(); it_child++)
      (*it_child)->toNameVariantMap(out, indent + "  ");
  }

  void toVariantNameMap(std::ostream & out, std::string indent) const {
    if (parent != NULL)
      out << indent << "variant_name_map.insert(std::pair<VariantT, std::string>(V_" << name << ", \"" << name << "\"));" << std::endl;

    std::vector<sage_tree_t *>::const_iterator it_child;
    for (it_child = children.begin(); it_child != children.end(); it_child++)
      (*it_child)->toVariantNameMap(out, indent + "  ");
  }

  void toParentVariantMap(std::ostream & out, std::string indent) const {
    if (parent != NULL)
      out << indent << "parent_variant_map.insert(std::pair<VariantT, VariantT>(V_" << name << ", V_" << parent->name << "));" << std::endl;

    std::vector<sage_tree_t *>::const_iterator it_child;
    for (it_child = children.begin(); it_child != children.end(); it_child++)
      (*it_child)->toParentVariantMap(out, indent + "  ");
  }

  static void parse(sage_tree_t * base, jsonxx::Object & node) {
    const std::map<std::string, jsonxx::Value *> & node_map = node.kv_map();
    std::map<std::string, jsonxx::Value *>::const_iterator it_node;
    for (it_node = node_map.begin(); it_node != node_map.end(); it_node++)
      parse(base->newChild(it_node->first), it_node->second->get<jsonxx::Object>());
  }
};

int main(int argc, const char **argv) {
  assert(argc == 3);

  std::ifstream sage_file;
  sage_file.open(argv[1]);
  assert(sage_file.is_open());

  jsonxx::Object sage;
  sage.parse(sage_file);

  sage_tree_t * sage_tree = new sage_tree_t("SgNode");
  sage_tree_t::parse(sage_tree, sage.get<jsonxx::Object>("SgNode"));

  std::string extension(argv[2]);
  extension = extension.substr(extension.length()-3);

  if (extension == "dot") {
    std::ofstream sage_dot;
    sage_dot.open(argv[2]);
    assert(sage_dot.is_open());

    sage_dot << "digraph sage {" << std::endl;
    sage_dot << "rankdir=LR;" << std::endl;
    sage_tree->toGraphViz(sage_dot);
    sage_dot << "}" << std::endl;
  }
  else if (extension == "cpp") {
    std::ofstream sage_cpp;
    sage_cpp.open(argv[2]);
    assert(sage_cpp.is_open());

    sage_cpp << "#include \"sage3basic.h\"" << std::endl << std::endl;
    sage_cpp << "void build_sage_name_variant_map(std::map<std::string, VariantT> & name_variant_map) {" << std::endl;
    sage_tree->toNameVariantMap(sage_cpp, "");
    sage_cpp << "}" << std::endl << std::endl;
    sage_cpp << "void build_sage_variant_name_map(std::map<VariantT, std::string> & variant_name_map) {" << std::endl;
    sage_tree->toVariantNameMap(sage_cpp, "");
    sage_cpp << "}" << std::endl << std::endl;
    sage_cpp << "void build_sage_parent_variant_map(std::map<VariantT, VariantT> & parent_variant_map) {" << std::endl;
    sage_tree->toParentVariantMap(sage_cpp, "");
    sage_cpp << "}" << std::endl << std::endl;
  }
  else assert(false);

  return 0;
}

