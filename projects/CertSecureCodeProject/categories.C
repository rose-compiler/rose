/**
 * \file categories.C
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

#include "rose.h"
#include "categories.h"

const SgBinaryOp *isAnyBinArithOp(const SgNode *node) {
        if (isSgAddOp(node)
          ||isSgDivideOp(node)
          ||isSgModOp(node)
          ||isSgSubtractOp(node)
          ||isSgMultiplyOp(node)
          ||isSgExponentiationOp(node)
          ||isSgIntegerDivideOp(node)) {
                return isSgBinaryOp(node);
        }
        return NULL;
}

const SgBinaryOp *isAnyBinArithAssignOp(const SgNode *node) {
        if (isSgPlusAssignOp(node)
          ||isSgDivAssignOp(node)
          ||isSgModAssignOp(node)
          ||isSgMinusAssignOp(node)
          ||isSgMultAssignOp(node)) {
                return isSgBinaryOp(node);
        }
        return NULL;
}

const SgBinaryOp *isAnyBinBitOp(const SgNode *node) {
        if (isSgBitAndOp(node)
          ||isSgBitOrOp(node)
          ||isSgBitXorOp(node)
          ||isSgLshiftOp(node)
          ||isSgRshiftOp(node)) {
                return isSgBinaryOp(node);
        }
        return NULL;
}

const SgBinaryOp *isAnyBinBitAssignOp(const SgNode *node) {
        if (isSgAndAssignOp(node)
          ||isSgIorAssignOp(node)
          ||isSgXorAssignOp(node)
          ||isSgLshiftAssignOp(node)
          ||isSgRshiftAssignOp(node)) {
                return isSgBinaryOp(node);
        }
        return NULL;
}

const SgBinaryOp *isAnyBinLogicOp(const SgNode *node) {
        if (isSgAndOp(node)
          ||isSgOrOp(node)) {
                return isSgBinaryOp(node);
        }
        return NULL;
}

const SgBinaryOp *isAnyAssignOp(const SgNode *node) {
        if (isSgAssignOp(node)
          ||isAnyBinArithAssignOp(node)
          ||isAnyBinBitAssignOp(node)
          ||isAnyBinLogicOp(node)) {
                return isSgBinaryOp(node);
        }
        return NULL;
}

const SgBinaryOp *isAnyEqualityOp(const SgNode *node) {
        if (isSgEqualityOp(node)
          ||isSgNotEqualOp(node)) {
                return isSgBinaryOp(node);
        }
        return NULL;
}

const SgBinaryOp *isAnyRelationalOp(const SgNode *node) {
        if (isSgGreaterOrEqualOp(node)
          ||isSgGreaterThanOp(node)
          ||isSgLessOrEqualOp(node)
          ||isSgLessThanOp(node)) {
                return isSgBinaryOp(node);
        }
        return NULL;
}

const SgBinaryOp *isAnyComparisonOp(const SgNode *node) {
        if (isAnyEqualityOp(node)
          ||isAnyRelationalOp(node)) {
                return isSgBinaryOp(node);
        }
        return NULL;
}

/** NOT ACCOUNTED FOR
          ||isSgConcatenationOp(node)
          ||isSgScopeOp(node)
          ||isSgArrowExp(node)
          ||isSgArrowStarOp(node)
          ||isSgCommaOpOp(node)
          ||isSgDotExp(node)
          ||isSgDotStarOp(node)
          ||isSgPntrArrRefExp(node)
*/

/**
 * Checks to see if the type is some kind of char or wchar_t
 *
 * \note we return false on [un]signed chars since they are numbers not
 * characters
 */
bool isAnyCharType(const SgType *type) {
        const SgType *t = type->stripTypedefsAndModifiers();
        return isSgTypeChar(t)
                || isSgTypeWchar(t)
                || isTypeWcharT(type);
}

bool isAnyPointerType(const SgType *type) {
        const SgType *t = type->stripTypedefsAndModifiers();
        return isSgPointerType(t)
                || isSgArrayType(t)
                || isTypeUintptrT(type);
}
