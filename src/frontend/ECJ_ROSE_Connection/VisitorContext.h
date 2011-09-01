/*
 * VisitorContext.h
 *
 *  Created on: Jul 29, 2011
 *      Author: vcave
 */

#ifndef VISITORCONTEXT_H_
#define VISITORCONTEXT_H_

#include <string>
#include <assert.h>

class VisitorContext {
private:
        unsigned int nb_children;

public:
        VisitorContext(unsigned int nb_children);
        virtual ~VisitorContext();

        virtual unsigned int getNbChildren() {
                return nb_children;
        }
};

class IfVisitorContext : public VisitorContext {
private:
        bool has_false_body;

public:
        IfVisitorContext(bool has_false_body);
        virtual ~IfVisitorContext();

        bool hasFalseBody() {
                return has_false_body;
        }
};

class BlockVisitorContext : public VisitorContext {
public:
        BlockVisitorContext(unsigned int nb_stmt);
        virtual ~BlockVisitorContext();
};

class CallVisitorContext : public VisitorContext {
private:
                bool has_receiver;

public:
        CallVisitorContext(unsigned int nb_args);
        virtual ~CallVisitorContext();

        unsigned int getNbArguments() {
                return getNbChildren();
        }
};

class ForVisitorContext : public VisitorContext {
private:
                bool has_cond;
                unsigned int nb_init;
                unsigned int nb_incr;
public:
        ForVisitorContext(unsigned int nb_init, bool has_cond, unsigned int nb_incr);
        virtual ~ForVisitorContext();

        unsigned int getNbInit() {
                return getNbChildren();
        }

        bool hasCond() {
                return has_cond;
        }

        unsigned int getNbIncr() {
                return getNbChildren();
        }

        virtual unsigned int getNbChildren() {
                // Not yet implemented
                assert(false);
                return 0;
        }
};

class MethodVisitorContext : public VisitorContext {
private:
        std::string name;

public:
        MethodVisitorContext(std::string name);
        virtual ~MethodVisitorContext();

        std::string getName() {
                return name;
        }
};

BlockVisitorContext * createBlockVisitorContext(unsigned int nb_stmt);
CallVisitorContext * createCallVisitorContext(unsigned int nb_stmt);
IfVisitorContext * createIfVisitorContext(bool has_false_body);
MethodVisitorContext * createMethodVisitorContext(std::string name);
ForVisitorContext * createForVisitorContext(unsigned int nb_init, bool has_cond,
                unsigned int nb_incr);

#endif /* VISITORCONTEXT_H_ */
