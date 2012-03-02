#include "ShapeAnalyzerOptions.h"

extern ShapeAnalyzerOptions *opt;

// remove temporary variables introduced in the normalisation process
PAG_BOOL get_universal_attribute__option_aliases_commontail() {
  return opt->variantAliasesCommonTail();
}

// remove temporary variables introduced in the normalisation process
PAG_BOOL get_universal_attribute__option_keep_tempvars() {
  return opt->variantKeepTempvars();
}

// remove temporary variables introduced in the normalisation process
PAG_BOOL get_universal_attribute__option_keep_helpvars() {
  return opt->variantKeepHelpvars();
}

// if visualisation etc. do not require graph in srw format, omit conversion
PAG_BOOL get_universal_attribute__option_omit_conversion_to_srw() {
    return !opt->convertToSRW();
}

// if visualisation etc. do not require graph in nnh format, omit conversion
PAG_BOOL get_universal_attribute__option_omit_conversion_to_nnh() {
    return !opt->convertToNNH();
}

// if garbage loops should be removed
PAG_BOOL get_universal_attribute__option_perform_shape_gc() {
    return opt->performShapeGC();
}

// if debug output: assign, tassign, ...
PAG_BOOL get_universal_attribute__option_debugmessage_astmatch()        { return opt->debugmessageAstmatch(); }
PAG_BOOL get_universal_attribute__option_debugmessage_assign()          { return opt->debugmessageAssign(); }
PAG_BOOL get_universal_attribute__option_debugmessage_tassign()         { return opt->debugmessageTassign(); }
PAG_BOOL get_universal_attribute__option_debugmessage_lowlevel()        { return opt->debugmessageLowlevel(); }
PAG_BOOL get_universal_attribute__option_debugmessage_materialisation() { return opt->debugmessageMaterialisation(); }

