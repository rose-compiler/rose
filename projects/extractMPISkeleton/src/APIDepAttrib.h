#ifndef __APIDepAttrib_H_LOADED__
#define __APIDepAttrib_H_LOADED__

#include <AstAttributeMechanism.h>

/*
 * this class represents an API dependency attribute.  The goal
 * of this class is to allow attributes to be associated with
 * nodes in the AST, and allow attributes to be meaningfully
 * merged and interrogated.  Internally this shouldn't be much
 * more complex than operations on a short bit sequence, but we'd
 * like to hide the representation and operations on it behind
 * a proper object.
 */
class APIDepAttribute : public AstAttribute {
    public:

    enum DependenceForm {
        /* This node is of type depType itself. */
        SELF    = 0x00,
        /* Something of type depType is control-dependent on this node. */
        CONTROL = 0x01,
        /* Something of type depType is data-dependent on this node. */
        DATA    = 0x02,
    };

    virtual APIDepAttribute *copy() const = 0;

    // join with another instance of APIDepAttribute
    virtual void join(const APIDepAttribute *other) = 0;

    // join with an enumeration value
    virtual void joinForm(const DependenceForm form) = 0;

    // query if two APIDepAttributes match
    virtual bool matches(const APIDepAttribute *other) const = 0;

    // does this instance contain another one?
    virtual bool contains(const APIDepAttribute *other) const = 0;

    // extract the enumeration value
    virtual DependenceForm getDepForm() const = 0;

    // set the enumeration value
    virtual void setDepForm(DependenceForm depForm) = 0;
};

#endif
