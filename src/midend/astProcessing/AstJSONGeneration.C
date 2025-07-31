#include "sage3basic.h"
#include "roseInternal.h"
#include "AstNodeVisitMapping.h"
#include <assert.h>
#include <iostream>

#include "JSONGeneration.h"
#include "AstJSONGeneration.h"

using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

class AstJSONGeneration_private : public JSONGeneration {
public:
  virtual void generate(std::string filename, SgNode* node);
  virtual void generate(SgProject* projectNode);
  void generateInputFiles(SgProject* projectNode);
  void generateWithinFile(const std::string& filename, SgFile* node); // ****
  void generateWithinFile(SgFile* node); // ****
protected:
  void handle_node(SgNode* node, JSONInheritedAttribute inheritedValue);
};

void AstJSONGeneration::generate(SgProject* projectNode) {
  AstJSONGeneration_private p;
  p.generate(projectNode);
}

void AstJSONGeneration::generateInputFiles(SgProject* projectNode) {
  AstJSONGeneration_private p;
  p.generateInputFiles(projectNode);
}

void AstJSONGeneration::generateWithinFile(const std::string& filename, SgFile* node) {
  AstJSONGeneration_private p;
  p.generateWithinFile(filename, node);
}

void AstJSONGeneration::generate(std::string filename, SgNode* node) {
  AstJSONGeneration_private p;
  p.generate(filename, node);
}

void
AstJSONGeneration_private::generateWithinFile(SgFile* node) {
  std::string jsonfilename = std::string("./") + std::string(Rose::utility_stripPathFromFileName(node->getFileName()));
  generateWithinFile (jsonfilename, node);
}

void
AstJSONGeneration_private::generateWithinFile(const std::string& jsonfilename, SgFile* node) {
  json_setup(jsonfilename + ".json");
  JSONInheritedAttribute jIA;
  traverseWithinFile(node, jIA);
  json_finalize();
}

void
AstJSONGeneration_private::generate(SgProject* projectNode) {
  const SgFilePtrList& fList = projectNode->get_fileList();
  for (SgFilePtrList::const_iterator fl_iter = fList.begin();
       fl_iter != fList.end(); fl_iter++) {
    ASSERT_require(*fl_iter != 0);
    SgFile* fp = *fl_iter;
    std::string filename = fp->getFileName();
    filename = Rose::StringUtility::stripPathFromFileName(filename);
    generate(filename,  *fl_iter);
  }
}

void
AstJSONGeneration_private::generateInputFiles(SgProject* projectNode) {
  const SgFilePtrList& fList = projectNode->get_fileList();
  for (SgFilePtrList::const_iterator fl_iter = fList.begin();
       fl_iter != fList.end(); fl_iter++) {
    ASSERT_require(*fl_iter != 0);
    generateWithinFile(*fl_iter);
  }
}

void AstJSONGeneration_private::generate(std::string filename, SgNode *node) {
  // NOTE: this is redundant with the stuff in JSONGeneration base class impl
  mlog[INFO] << "generating json file: " << filename << std::endl;

  json_setup(filename);
  JSONInheritedAttribute jIA;
  traverse(node, jIA);
  json_finalize();

  mlog[INFO] << "finished generating json file." << std::endl;
}

// modeled on AstPDFGeneration_private::edit_page
void AstJSONGeneration_private::handle_node(SgNode* node, JSONInheritedAttribute /*inheritedValue*/)
{
  nlohmann::json jdata;

  // obtain string with hex representation of node pointer value
  std::ostringstream _ss;
  _ss << std::hex << node;
  std::string addrString = _ss.str();
  jdata["address"] = addrString;

  // clear and reuse string stream for parent address
  _ss.str(std::string());
  _ss << std::hex << node->get_parent();
  jdata["parent"] = _ss.str();

  jdata["sageClassName"] = node->sage_class_name();

  if (SgLocatedNode *locNode = isSgLocatedNode(node)) {
    Sg_File_Info *fi = locNode->get_file_info();
    jdata["filename"] = fi->get_filename();
    jdata["line"] = fi->get_line();
    jdata["column"] = fi->get_col();
    jdata["isTransformation"] = fi->isTransformation();
    jdata["isOutputInCodeGeneration"] = fi->isOutputInCodeGeneration();
  }

  if (SgDeclarationStatement *dclStmt = isSgDeclarationStatement(node)) {
    jdata["declarationMangledName"] = dclStmt->get_mangled_name().getString();
    // NOTE: may not be necessary if p_name field has same info
    // NOTE: exclude using directive statements to prevent issue with test cases
    //       failing due to get_symbol_from_symbol_table()
    bool has_search_implementation = !(
                                        isSgUsingDirectiveStatement(node)   ||
                                        isSgUsingDeclarationStatement(node) ||
                                        isSgStaticAssertionDeclaration(node)
                                     );
    if (dclStmt->hasAssociatedSymbol() && has_search_implementation) {
      if (SgSymbol *symbol = dclStmt->search_for_symbol_from_symbol_table()) {
        jdata["symbolName"] = symbol->get_name().getString();
      }
    }
  }

  if (SgExpression *expr = isSgExpression(node)) {
    jdata["expressionType"] = expr->get_type()->unparseToString();
  }

  RTIReturnType rti = node->roseRTI();
  nlohmann::json jRTI = nlohmann::json::array();
  for (auto i : rti) {
    if (strlen(i.type) >= 7 && strncmp(i.type, "static ", 7) == 0) {
      continue; // skip static members
    }

    nlohmann::json jRTIEntry;
    jRTIEntry["type"] = i.type;
    jRTIEntry["name"] = i.name;
    jRTIEntry["value"] = i.value;
    jRTI.push_back(jRTIEntry);
  }
  jdata["rti"] = jRTI;

  if (node->get_attributeMechanism() != nullptr) {
    AstAttributeMechanism::AttributeIdentifiers aidents =
      node->get_attributeMechanism()->getAttributeIdentifiers();

    nlohmann::json jAttribs;
    for (auto i : aidents) {
      jAttribs[i] = node->getAttribute(i)->toString();
    }
    jdata["attributes"] = jAttribs;
  }

  // somewhat redundant to have the address in the jdata value and
  // used as the key in the outer dictionary, but this will allow us
  // to later change the way the outer dictionary is keyed (e.g., move
  // to an array of elements instead of key/value pairs.)
  jsonDocument[addrString] = jdata;
}
