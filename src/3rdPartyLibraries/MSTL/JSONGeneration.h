//
// JSON generation to replace PDF generation.  Modeled on PDF generator but
// removes layout details and instead captures the necessary structure of
// the AST for an external tool to traverse and handle layout.
//
// mjs // sept. 2022
//
#ifndef JSONGENERATION_H
#define JSONGENERATION_H

#include <iostream>
#include "nlohmann/json.hpp"
#include "AstProcessing.h"

class JSONInheritedAttribute {
public:
  explicit JSONInheritedAttribute() {}
  ~JSONInheritedAttribute() {}
};

class JSONGeneration : public SgTopDownProcessing<JSONInheritedAttribute> {
public:
  JSONGeneration() { }
  virtual void generate(std::string filename, SgNode *node);

protected:
  virtual JSONInheritedAttribute evaluateInheritedAttribute(SgNode *node, JSONInheritedAttribute inheritedValue);

  nlohmann::json jsonDocument;

  virtual void handle_node(SgNode *node, JSONInheritedAttribute inheritedValue);
  void json_setup(std::string filename);
  void json_finalize();

private:
  std::string filename;
};

#endif
