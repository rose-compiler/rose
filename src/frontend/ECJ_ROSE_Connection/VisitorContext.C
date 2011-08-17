/*
 * VisitorContext.C
 *
 *  Created on: Jul 29, 2011
 *      Author: vcave
 */

#include "VisitorContext.h"

VisitorContext::VisitorContext(unsigned int nb_children__) : nb_children(nb_children__) {
}

VisitorContext::~VisitorContext() {
}

CallVisitorContext::CallVisitorContext(unsigned int nb_args) : VisitorContext(nb_args) {
}

CallVisitorContext::~CallVisitorContext() {
}

ForVisitorContext::ForVisitorContext(unsigned int nb_init__,
                                                                         bool has_cond__,
                                                                         unsigned int nb_incr__) :
                                                                         VisitorContext(0), nb_init(nb_init__),
                                                                         has_cond(has_cond__), nb_incr(nb_incr__)  {
}

ForVisitorContext::~ForVisitorContext() {
}

IfVisitorContext::IfVisitorContext(bool has_false_body__) : VisitorContext((has_false_body__) ? 2 : 1), has_false_body(has_false_body__) {
}

IfVisitorContext::~IfVisitorContext() {
}

BlockVisitorContext::BlockVisitorContext(unsigned int nb_stmt) : VisitorContext(nb_stmt) {
}

BlockVisitorContext::~BlockVisitorContext() {
}

MethodVisitorContext::MethodVisitorContext(std::string name__) : VisitorContext(1), name(name__) {
}

MethodVisitorContext::~MethodVisitorContext() {
}

IfVisitorContext * createIfVisitorContext(bool has_false_body) {
        return new IfVisitorContext(has_false_body);
}
MethodVisitorContext * createMethodVisitorContext(std::string name) {
        return new MethodVisitorContext(name);
}
BlockVisitorContext * createBlockVisitorContext(unsigned int nb_stmt) {
        return new BlockVisitorContext(nb_stmt);
}
CallVisitorContext * createCallVisitorContext(unsigned int nb_args) {
        return new CallVisitorContext(nb_args);
}
ForVisitorContext * createForVisitorContext(unsigned int nb_init, bool has_cond,
                unsigned int nb_incr) {
        return new ForVisitorContext(nb_init, has_cond, nb_incr);
}
