
#include "ROSETTA_macros.h"
#include "grammar.h"
#include "AstNodeClass.h"
#include "grammarString.h"
#include <sstream>
#include <algorithm>

void Grammar::generateRoseTraits() {
  std::string ofilename(target_directory + "/Rose/Traits/generated.h");
  std::ofstream ofile( ofilename, std::ios::out) ;

  ofile << "#ifndef Rose_Traits_generated_h" << std::endl;
  ofile << "#define Rose_Traits_generated_h" << std::endl;

  ofile << "namespace Rose {" << std::endl;
  ofile << "namespace Traits {" << std::endl;
  ofile << "namespace generated {" << std::endl;

  std::vector<std::string> all_node_names;
  for (auto p: astVariantToTerminalMap) {
    auto variant = p.first;
    auto term = p.second;
    if (term) {
      all_node_names.push_back("Sg" + term->baseName);
    
      assert(term->memberDataPrototypeList[0][1].size() == 0);
      assert(term->memberDataPrototypeList[1][0].size() == 0);
      assert(term->memberDataPrototypeList[1][1].size() == 0);

      std::vector<GrammarString *> fields;
      for (auto gsp: term->memberDataPrototypeList[0][0]) {
        if (gsp->typeNameString.find("static ") != 0) {
          fields.push_back(gsp);
        }
      }

      ofile << "// Class: " << term->baseName << std::endl;

      for (auto i = 0; i < fields.size(); i++) {
        auto gsp = fields[i];
        auto type_str = gsp->typeNameString;

        if (type_str == "hash_iterator") type_str = "Sg" + term->baseName + "::hash_iterator";
        else if (type_str == "$CLASSNAME*") type_str = "Sg" + term->baseName + "*";

        ofile << "template <> struct describe_field_t<Sg" << term->baseName << "," << type_str << ",&Sg" << term->baseName << "::p_" << gsp->variableNameString << "> {" << std::endl;
        ofile << "  using parent = Sg" << term->baseName << ";" << std::endl;
        ofile << "  using field_type = " << type_str << ";" << std::endl;
        ofile << "  static constexpr size_t position{" << i << "};" << std::endl;

        ofile << "  static constexpr char const * const name{\"" << gsp->variableNameString << "\"};" << std::endl;
        ofile << "  static constexpr char const * const typestr{\"" << type_str << "\"};" << std::endl;
        ofile << "  static constexpr bool traverse{" << (gsp->toBeTraversed == DEF_TRAVERSAL ? "true" : "false") << "};" << std::endl;

        ofile << "  static constexpr auto mbr_ptr{&Sg" << term->baseName << "::p_" << gsp->variableNameString << "};" << std::endl;

        ofile << "  template <template <typename NodeT, typename FieldT, FieldT fld_ptr> class Desc>" << std::endl;
        ofile << "  using bind = Desc<Sg" << term->baseName << ", " << type_str << " Sg" << term->baseName << "::*, &Sg" << term->baseName << "::p_" << gsp->variableNameString << ">;" << std::endl;
        ofile << "};" << std::endl;
      }

      ofile << "template <> struct describe_node_t<Sg" << term->baseName << "> {" << std::endl;
      ofile << "  using node = Sg" << term->baseName << ";" << std::endl;
      if (term->baseClass) ofile << "  using base = Sg" << term->baseClass->baseName << ";" << std::endl;
      else                 ofile << "  using base = void;" << std::endl;

      ofile << "  static constexpr char const * const name{\"" << term->baseName << "\"};" << std::endl;
      ofile << "  static constexpr unsigned long variant{" << variant << "};" << std::endl;
      ofile << "  static constexpr bool concrete{" << ( term->canHaveInstances ? "true" : "false") << "};" << std::endl;

      ofile << "  using subclasses_t = mp::List<";
      for (auto i = 0; i < term->subclasses.size(); ++i) {
        ofile << "Sg" << term->subclasses[i]->baseName;
        if (i < term->subclasses.size() - 1) {
          ofile << ", ";
        }
      }
      ofile << ">;" << std::endl;

      ofile << "  using fields_t = mp::List<";
      for (auto i = 0; i < fields.size(); i++) {
        auto gsp = fields[i];
        auto type_str = gsp->typeNameString;

        if (type_str == "hash_iterator") type_str = "Sg" + term->baseName + "::hash_iterator";
        else if (type_str == "$CLASSNAME*") type_str = "Sg" + term->baseName + "*";

        ofile << "describe_field_t<Sg" << term->baseName << "," << type_str << ",&Sg" << term->baseName << "::p_" << gsp->variableNameString << ">";
        if (i < fields.size() - 1) {
          ofile << ", ";
        }
      }
      ofile << ">;" << std::endl;

      ofile << "};" << std::endl;

      ofile << "template <> struct node_from_variant_t<" << variant << "> { using type = Sg" + term->baseName + "; };\n" << std::endl;
    }
  }
#if 0
  ofile << "using node_list = mp::List<";
  for (auto i = 0; i < all_node_names.size(); i++) {
    ofile << all_node_names[i];
    if (i < all_node_names.size() - 1) {
      ofile << ", ";
    }
  }
  ofile << ">;" << std::endl;
#endif

  ofile << "} } }" << std::endl;
  ofile << "#endif /* Rose_Traits_generated_h */" << std::endl;
}

