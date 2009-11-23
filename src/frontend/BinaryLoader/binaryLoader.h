#ifndef ROSE_BINARYLOADER_H
#define ROSE_BINARYLOADER_H

#include <stdint.h>
#include <vector>
#include <map>
#include <set>


namespace BinaryLoader
{
  /** Loads, performs layout, relocates and disassembles the given file */
  bool load(SgBinaryComposite* binaryFile, bool performDynamic=false);

#if 0
  /** Performs loading only.  Will populate binaryFile->p_binaryFileList with SgAsmFile* 
      return false if any library cannot be loaded
  */

  bool loadAllLibraries(SgBinaryComposite* binaryFile);
  /** Performs layout on unmapped sections in all files in binaryFile->p_binaryFile
      If a valid layout cannot be performed, return false
  */
  bool layoutAllLibraries(SgBinaryComposite* binaryFile);
  /**

   */
  bool relocateAllLibraries(SgBinaryComposite* binaryFile);
#endif
}

#endif // BINARYLOADER_H
