#ifndef H_EXPRESSION_ID
#define H_EXPRESSION_ID

#include <string>

// GB (2008-05-14): This class wraps the expression IDs computed by the AST
// numbering mechanism. Pointers to instances of this class can be used in
// PAG-generated analyzers like built-in types.
// That's neat because we don't have to mess around with numbers anymore,
// and when visualizing analysis results, expression IDs can be told to
// print as the expression they represent rather than an opaque number.
class ExpressionID
{
public:
 // identifier of the expression
    unsigned long id;
 // string visualization of the expression
    std::string print() const;

 // static stuff
 // PAG type id, a handle returned by GC_registertype and needed when
 // allocating instances on the PAG heap
    static int type_id;
 // Support for setting the format for visualizing expression IDs: number,
 // expression string, or both (default).
    enum PrintFormat { F_ID, F_Expression, F_IDAndExpression };
    static void setPrintFormat(PrintFormat format);
    static PrintFormat printFormat;
};

typedef unsigned long _o_ExpressionID_acur;
typedef unsigned long *o_ExpressionID_acur;

#endif
