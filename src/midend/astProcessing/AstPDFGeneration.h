// Author: Markus Schordan
// $Id: AstPDFGeneration.h,v 1.3 2007/02/19 16:08:38 vuduc2 Exp $

#ifndef ASTPDFGENERATION_H
#define ASTPDFGENERATION_H

class ROSE_DLL_API AstPDFGeneration {
public:
  AstPDFGeneration() {}
  void generate(std::string filename, SgNode* node); // Generate full AST
  void generate(SgProject* projectNode);
  void generateInputFiles(SgProject* projectNode); // Generate within files for each project file
  void generateWithinFile(const std::string& filename, SgFile* node); // ****
  void generateWithinFile(SgFile* node); // **** Generate pdf for only the nodes which represent code of the same file as the start node.
};

#endif
