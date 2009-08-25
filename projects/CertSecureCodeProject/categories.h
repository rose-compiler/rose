/**
 * \file categories.h
 *
 * Copyright (c) 2007 Carnegie Mellon University.
 * All rights reserved.

 * Permission to use this software and its documentation for any purpose is
 * hereby granted, provided that the above copyright notice appear and that
 * both that copyright notice and this permission notice appear in supporting
 * documentation, and that the name of CMU not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WSTRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL CMU BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, RISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#ifndef CATEGORIES_H
#define CATEGORIES_H

// #include "rose.h"
#include "utilities.h"

const SgBinaryOp *isAnyBinArithOp(const SgNode *node);
const SgBinaryOp *isAnyBinArithAssignOp(const SgNode *node);
const SgBinaryOp *isAnyBinBitOp(const SgNode *node);
const SgBinaryOp *isAnyBinBitAssignOp(const SgNode *node);
const SgBinaryOp *isAnyBinLogicOp(const SgNode *node);
const SgBinaryOp *isAnyAssignOp(const SgNode *node);
const SgBinaryOp *isAnyEqualityOp(const SgNode *node);
const SgBinaryOp *isAnyRelationalOp(const SgNode *node);
const SgBinaryOp *isAnyComparisonOp(const SgNode *node);

bool isAnyCharType(const SgType *type);
bool isAnyPointerType(const SgType *type);

#endif
