#ifndef __APIDepAttrib_H_LOADED__
#define __APIDepAttrib_H_LOADED__

#include <AstAttributeMechanism.h>

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

    virtual APIDepAttribute *copy() = 0;

    virtual void join(const APIDepAttribute *other) = 0;

    virtual void joinForm(const DependenceForm form) = 0;

    virtual bool matches(const APIDepAttribute *other) = 0;

    virtual bool contains(const APIDepAttribute *other) = 0;

    virtual DependenceForm getDepForm() = 0;

    virtual void setDepForm(DependenceForm depForm) = 0;
};

#endif
