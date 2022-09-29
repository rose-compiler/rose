#ifndef ASTJSONGENERATION_H
#define ASTJSONGENERATION_H

class ROSE_DLL_API AstJSONGeneration {
 public:
  AstJSONGeneration() {}
  void generate(std::string filename, SgNode* node);
  void generate(SgProject* projectNode);
  void generateInputFiles(SgProject* projectNode); // Generate within files for each project file
  void generateWithinFile(const std::string& filename, SgFile* node);
};

#endif // ASTJSONGENERATION_H
