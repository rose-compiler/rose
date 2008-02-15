#include "rose.h"

#include "CreateSlice.h"


bool CreateSlice::evaluateInheritedAttribute(SgNode * node, bool partOfSlice) {
  if (_toSave.count(node)) {
    partOfSlice = true;
  }

  if (partOfSlice)
    _toSave.insert(node);

  return partOfSlice;
}

bool CreateSlice::evaluateSynthesizedAttribute(SgNode * node, bool inherited, SubTreeSynthesizedAttributes atts) {
  bool partOfSlice = false;
  if (_toSave.count(node) != 0) {
    partOfSlice = true;
  } else {
    for (SubTreeSynthesizedAttributes::iterator i = atts.begin(); i != atts.end(); i++) {
      if (*i == true) {
	partOfSlice = true;
	break;
      }
    }
  }

  if (isSgLocatedNode(node) && !partOfSlice) {
    Sg_File_Info * file_info = node->get_file_info();
    
    file_info->unsetCompilerGeneratedNodeToBeUnparsed();
    file_info->setCompilerGenerated();
  }

  return partOfSlice;
}
