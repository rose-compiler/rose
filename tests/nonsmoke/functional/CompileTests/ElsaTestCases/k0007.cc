// error: E_floatLit is not constEval'able

// originally found in package coreutils

enum E {
    a = (0.5 == 0)
};
