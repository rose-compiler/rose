#ifndef H_VARIABLE_ID
#define H_VARIABLE_ID

#include <string>

class VariableID
{
public:
 // identifier of the variable
    unsigned long id;
 // string visualization of the variable
    std::string print() const;

 // static stuff
 // PAG type id, a handle returned by GC_registertype
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

#endif
