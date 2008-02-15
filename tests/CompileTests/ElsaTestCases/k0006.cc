// error: E_alignofType is not constEval'able

// originally found in package gettext

// ERR-MATCH: E_alignofType is not constEval

enum E {
    a = __alignof__(int)
};
