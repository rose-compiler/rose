// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: analysis_info.h,v 1.1 2007-09-20 09:25:32 adrian Exp $

#ifndef H_ANALYSIS_INFO
#define H_ANALYSIS_INFO

// #include <AttributeMechanism.h>

#include "cfg_support.h"

/* The StatementAttribute represents a block of the CFG. The
 * intention is to associate SgStatement nodes from the AST with the
 * blocks they are mapped to in the CFG; this allows access to
 * analysis information.
 * 
 * The enum pos specifies whether the analysis information from the
 * incoming (pre) or outgoing (post) edge of the specified block is
 * relevant. This is needed because for most statements, the "post"
 * information is on the outgoing edge of the last associated block
 * of the CFG, but not always.
 * 
 * The exception are if, switch and loop statements, which do not
 * have a unique last block. For these we must use the incoming
 * information of the first block *after* the control statement,
 * where the paths of execution run together. This block always
 * exists, possibly it is the function's exit block. */
enum pos { POS_PRE, POS_POST };

class StatementAttribute : public AstAttribute
{
public:
    StatementAttribute(BasicBlock *bb_, enum pos pos_) : bb(bb_), pos(pos_) {}
    BasicBlock *get_bb() const { return bb; }
    enum pos get_pos() const { return pos; }

private:
    BasicBlock *bb;
    enum pos pos;
};

#endif
