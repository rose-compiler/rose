#include "sage3basic.h"
#include "roseInternal.h"

#include "JSONGeneration.h"

#include <iostream>
#include <fstream>

using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

void JSONGeneration::json_setup(std::string filename) {
  this->filename = filename;
}

void JSONGeneration::json_finalize() {
  // write json to file
  std::ofstream ostr(this->filename.c_str());
  ostr << this->jsonDocument.dump(4);
}

void JSONGeneration::handle_node(SgNode *node, JSONInheritedAttribute inheritedValue) {
}

void JSONGeneration::generate(std::string filename, SgNode *node) {
  //mlog[INFO] << "generating json file: " << filename << std::endl;

  json_setup(filename);
  JSONInheritedAttribute jIA;
  traverse(node, jIA);
  json_finalize();

  //mlog[INFO] << "finished generating json file." << std::endl;
}

JSONInheritedAttribute
JSONGeneration::evaluateInheritedAttribute(SgNode* node, JSONInheritedAttribute inheritedValue) {
  // nothing to do, so just return a default JSON inherited attribute
  JSONInheritedAttribute jIA;

  handle_node(node, inheritedValue);

  return jIA;
}
