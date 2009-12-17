#ifndef ROSE_BINARYLOADERGENERIC_H
#define ROSE_BINARYLOADERGENERIC_H

#include <stdint.h>
#include <vector>
#include <map>
#include <set>

class BinaryLoaderGeneric
{
 public:
  BinaryLoaderGeneric();
  virtual ~BinaryLoaderGeneric(){}

  void set_perform_dynamic_linking(bool);
  bool get_perform_dynamic_linking() const;

  void set_perform_layout(bool);
  bool get_perform_layout() const;

  void set_perform_relocations(bool);
  bool get_perform_relocations() const;

  void set_perform_disassembly(bool);
  bool get_perform_disassembly() const;

  void set_verbose(int verbose);
  int get_verbose() const;


  /** Loads, performs layout, relocates and disassembles the given file */
  virtual bool load(SgBinaryComposite* binaryFile,SgAsmGenericFile* executable);
  
  /** Performs loading only.  Will populate binaryFile->p_binaryFileList with SgAsmFile* 
      return false if any library cannot be loaded
  */
  virtual bool loadAllLibraries(SgBinaryComposite* binaryFile);
  
  /** Performs layout on unmapped sections in all files in binaryFile->p_binaryFile
      If a valid layout cannot be performed, return false
  */
  virtual bool layoutAllLibraries(SgBinaryComposite* binaryFile);
  
  /**
   */
  virtual bool relocateAllLibraries(SgBinaryComposite* binaryFile);


  virtual bool disassembleAllLibraries(SgBinaryComposite* binaryFile);

 protected:

  /** Performs loading only.  Will populate binaryFile->p_binaryFileList with SgAsmFile* 
      return false if any library cannot be loaded
  */
  virtual bool loadInterpLibraries(SgBinaryComposite* binaryFile,SgAsmInterpretation* interp);


  virtual bool layoutInterpLibraries(SgBinaryComposite* binaryFile,SgAsmInterpretation* interp);

  /** Used to select which sections should be layed out by the Loader (i.e. which sections should
      actually occupy virtual memory)
   */
  virtual void addSectionsForLayout(SgAsmGenericHeader* header, 
				    SgAsmGenericSectionPtrList &allSections);

  virtual SgAsmGenericFile* createAsmAST( SgBinaryComposite* binaryFile,std::string filePath);



  virtual void handleSectionMapped(SgAsmGenericSection* section){} /* =0 TODO */; 
  /// helper function to get raw dll list from a file
  virtual Rose_STL_Container<std::string> getDLLs(SgAsmGenericHeader* header,
						  const Rose_STL_Container<std::string> &dllFilesAlreadyLoaded);

  int get_verbose(SgBinaryComposite* binary) const;

 private: 
  int p_verbose;
  bool p_perform_dynamic_linking;
  bool p_perform_layout;
  bool p_perform_relocations;
  bool p_perform_disassembly;
};


#endif // BINARYLOADERGENERIC_H
