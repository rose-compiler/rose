#ifndef H_VARIABLE_ID
#define H_VARIABLE_ID

#include <string>

// GB (2008-05-14): This class wraps the variable IDs computed by the AST
// numbering mechanism. Pointers to instances of this class can be used in
// PAG-generated analyzers like built-in types.
// The advantages over simply using numbers as before are stricter typing,
// which leads to more self-documenting code (and possibly the avoidance of
// a few mistakes), and better control of how variables are printed.
class VariableID
{
public:
 // identifier of the variable
    unsigned long id;
 // string visualization of the variable
    std::string print() const;

 // static stuff
 // PAG type id, a handle returned by GC_registertype and needed when
 // allocating instances on the PAG heap
    static int type_id;
 // Support for setting the format for visualizing variable IDs: number
 // only, name only, or a combined form that looks like "42(my_variable)"
 // (default).
 // We might add a format string based approach later, although that might
 // be overkill.
    enum PrintFormat { F_ID, F_Name, F_IDAndName };
    static void setPrintFormat(PrintFormat format);
    static PrintFormat printFormat;
};

typedef unsigned long _o_VariableID_acur;
typedef unsigned long *o_VariableID_acur;

#endif
