#define PIG_RULE_get_0(NODE,TYPE,CONSTR,PIG_VAR_1819307350,FIELD,PIG_VAR_1819307351,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    return dynamic_cast<FTYPE>(NODE);\
}
#define PIG_RULE_get_1(NODE,TYPE,CONSTR,PIG_VAR_1819307354,FIELD,PIG_VAR_1819307355,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    return (dynamic_cast<PIrIf>(NODE))->FIELD.get();\
}
#define PIG_RULE_get_2(NODE,TYPE,CONSTR,PIG_VAR_1819307358,FIELD,PIG_VAR_1819307359,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    return (dynamic_cast<PIrIf>(NODE))->FIELD.get();\
}
#define PIG_RULE_get_3(NODE,TYPE,CONSTR,PIG_VAR_1819307362,FIELD,PIG_VAR_1819307363,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    return (dynamic_cast<PIrIf>(NODE))->FIELD.get();\
}
#define PIG_RULE_get_4(NODE,TYPE,CONSTR,PIG_VAR_1819307366,FIELD,PIG_VAR_1819307367,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    return (dynamic_cast<PIrIf>(NODE))->FIELD.get();\
}
#define PIG_RULE_get_5(NODE,TYPE,CONSTR,PIG_VAR_1819307370,FIELD,PIG_VAR_1819307371,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    return (dynamic_cast<PIrIf>(NODE))->FIELD.get();\
}
#define PIG_RULE_get_6(NODE,TYPE,CONSTR,PIG_VAR_1819307374,FIELD,PIG_VAR_1819307375,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    return NODE->FIELD.get();\
}
#define PIG_RULE_get_7(NODE,TYPE,CONSTR,PIG_VAR_1819307378,FIELD,PIG_VAR_1819307379,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    return NODE->FIELD.get();\
}
#define PIG_RULE_get_8(NODE,TYPE,CONSTR,PIG_VAR_1819307382,FIELD,PIG_VAR_1819307383,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    return NODE->FIELD.get();\
}
#define PIG_RULE_get_9(NODE,TYPE,CONSTR,PIG_VAR_1819307386,FIELD,PIG_VAR_1819307387,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    return NODE->FIELD.get();\
}
#define PIG_RULE_get_10(NODE,TYPE,CONSTR,PIG_VAR_1819307390,FIELD,PIG_VAR_1819307391,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    CONSTR *node = dynamic_cast<CONSTR *>(NODE);\
    return new IrNodelistTmpl<IrMultiAssign::Assign>(node->FIELD);\
}
#define PIG_RULE_get_11(NODE,TYPE,CONSTR,PIG_VAR_1819307394,FIELD,PIG_VAR_1819307395,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    CONSTR *node = dynamic_cast<CONSTR *>(NODE);\
    return new IrNodelistTmpl<IrCall::Argument>(node->FIELD);\
}
#define PIG_RULE_get_12(NODE,TYPE,CONSTR,PIG_VAR_1819307398,FIELD,PIG_VAR_1819307399,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    CONSTR *node = dynamic_cast<CONSTR *>(NODE);\
    return new IrNodelistTmpl<IrAsm::Info>(node->FIELD);\
}
#define PIG_RULE_get_13(NODE,TYPE,CONSTR,PIG_VAR_1819307402,FIELD,PIG_VAR_1819307403,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    CONSTR *node = dynamic_cast<CONSTR *>(NODE);\
    return new IrNodelistTmpl<IrAsm::Arg>(node->FIELD);\
}
#define PIG_RULE_get_14(NODE,TYPE,CONSTR,PIG_VAR_1819307406,FIELD,PIG_VAR_1819307407,FTYPE)\
FTYPE TYPE##_##CONSTR##_get_##FIELD(TYPE NODE)\
{\
    return (dynamic_cast<CONSTR *>(NODE))->FIELD.get();\
}
#define PIG_RULE_is_0(NODE,TYPE,CONSTR,PIG_VAR_4,PIG_VAR_5,PIG_VAR_6,PIG_VAR_7)\
int is_op_##TYPE##_##CONSTR(TYPE NODE)\
{\
    return (dynamic_cast<PIrControlStatement>(NODE) != 0);\
}
#define PIG_RULE_is_1(NODE,TYPE,CONSTR,PIG_VAR_12,PIG_VAR_13,PIG_VAR_14,PIG_VAR_15)\
int is_op_##TYPE##_##CONSTR(TYPE NODE)\
{\
    return (dynamic_cast<PIrControlStatement>(NODE) != 0);\
}
#define PIG_RULE_is_2(NODE,TYPE,CONSTR,PIG_VAR_20,PIG_VAR_21,PIG_VAR_22,PIG_VAR_23)\
int is_op_##TYPE##_##CONSTR(TYPE NODE)\
{\
    return (dynamic_cast<PIrControlStatement>(NODE) != 0);\
}
#define PIG_RULE_is_3(NODE,TYPE,CONSTR,PIG_VAR_28,PIG_VAR_29,PIG_VAR_30,PIG_VAR_31)\
int is_op_##TYPE##_##CONSTR(TYPE NODE)\
{\
    return (dynamic_cast<PIrControlStatement>(NODE) != 0);\
}
#define PIG_RULE_is_4(NODE,TYPE,CONSTR,PIG_VAR_36,PIG_VAR_37,PIG_VAR_38,PIG_VAR_39)\
int is_op_##TYPE##_##CONSTR(TYPE NODE)\
{\
    return (dynamic_cast<PIrControlStatement>(NODE) != 0);\
}
#define PIG_RULE_is_5(NODE,TYPE,CONSTR,PIG_VAR_44,PIG_VAR_45,PIG_VAR_46,PIG_VAR_47)\
int is_op_##TYPE##_##CONSTR(TYPE NODE)\
{\
    return (dynamic_cast<PIrControlStatement>(NODE) != 0);\
}
#define PIG_RULE_is_6(NODE,TYPE,CONSTR,PIG_VAR_52,PIG_VAR_53,PIG_VAR_54,PIG_VAR_55)\
int is_op_##TYPE##_##CONSTR(TYPE NODE)\
{\
    return 1;\
}
#define PIG_RULE_is_7(NODE,TYPE,CONSTR,PIG_VAR_60,PIG_VAR_61,PIG_VAR_62,PIG_VAR_63)\
int is_op_##TYPE##_##CONSTR(TYPE NODE)\
{\
    return 1;\
}
#define PIG_RULE_is_8(NODE,TYPE,CONSTR,PIG_VAR_68,PIG_VAR_69,PIG_VAR_70,PIG_VAR_71)\
int is_op_##TYPE##_##CONSTR(TYPE NODE)\
{\
    return 1;\
}
#define PIG_RULE_is_9(NODE,TYPE,CONSTR,PIG_VAR_76,PIG_VAR_77,PIG_VAR_78,PIG_VAR_79)\
int is_op_##TYPE##_##CONSTR(TYPE NODE)\
{\
    return 1;\
}
#define PIG_RULE_is_10(NODE,TYPE,CONSTR,PIG_VAR_84,PIG_VAR_85,PIG_VAR_86,PIG_VAR_87)\
int is_op_##TYPE##_##CONSTR(TYPE NODE)\
{\
    return (dynamic_cast<CONSTR *>(NODE) != 0);\
}
#define PIG_RULE_empty_0(L,PIG_VAR_5,PIG_VAR_6,PIG_VAR_7,PIG_VAR_8,PIG_VAR_9,FTYPE)\
int LIST_##FTYPE##_empty(LIST_##FTYPE L)\
{\
    return (L->getFirst() == 0);\
}
#define PIG_RULE_hd_0(L,PIG_VAR_5,PIG_VAR_6,PIG_VAR_7,PIG_VAR_8,PIG_VAR_9,FTYPE)\
FTYPE LIST_##FTYPE##_hd(LIST_##FTYPE L)\
{\
    return L->getFirst();\
}
#define PIG_RULE_tl_0(L,PIG_VAR_5,PIG_VAR_6,PIG_VAR_7,PIG_VAR_8,PIG_VAR_9,FTYPE)\
LIST_##FTYPE LIST_##FTYPE##_tl(LIST_##FTYPE L)\
{\
    L->fetchFirst();\
    return L;\
}

PIG_RULE_get_14(NODE, PIrStatement, IrAssign, 0, destAddr, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrStatement, IrAssign, 0, destAddr, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrStatement, IrAssign, 0, rightSide, 1, PIrExpr)
PIG_RULE_get_10(NODE, PIrStatement, IrMultiAssign, 1, assigns, 0, LIST_PIrMultiAssign_Assign)
PIG_RULE_is_10(NODE, PIrStatement, IrMultiAssign, 1, assigns, 0, LIST_PIrMultiAssign_Assign)
PIG_RULE_empty_0(L, PIrStatement, IrMultiAssign, 1, assigns, 0, PIrMultiAssign_Assign)
PIG_RULE_hd_0(L, PIrStatement, IrMultiAssign, 1, assigns, 0, PIrMultiAssign_Assign)
PIG_RULE_tl_0(L, PIrStatement, IrMultiAssign, 1, assigns, 0, PIrMultiAssign_Assign)
PIG_RULE_get_14(NODE, PIrStatement, IrMultiAssign, 1, rightSide, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrStatement, IrEvaluate, 2, expression, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrStatement, IrEvaluate, 2, expression, 0, PIrExpr)
PIG_RULE_get_0(NODE, PIrStatement, N_IrControlStatement, 3, node, 0, PIrControlStatement)
PIG_RULE_is_0(NODE, PIrStatement, N_IrControlStatement, 3, node, 0, PIrControlStatement)
PIG_RULE_get_14(NODE, PIrStatement, IrModuloLimit, 4, localUse, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrStatement, IrModuloLimit, 4, localUse, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrStatement, IrModuloLimit, 4, modulo, 1, Int64)
PIG_RULE_get_6(NODE, PIrMultiAssign_Assign, Assign, 0, destAddr, 0, PIrExpr)
PIG_RULE_is_6(NODE, PIrMultiAssign_Assign, Assign, 0, destAddr, 0, PIrExpr)
PIG_RULE_get_6(NODE, PIrMultiAssign_Assign, Assign, 0, constraint, 1, CString)
PIG_RULE_get_6(NODE, PIrMultiAssign_Assign, Assign, 0, reg, 2, Int32)
PIG_RULE_get_14(NODE, PIrControlStatement, IrJump, 0, destBlock, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrControlStatement, IrJump, 0, destBlock, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrControlStatement, IrReturnValue, 2, returnExpr, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrControlStatement, IrReturnValue, 2, returnExpr, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrControlStatement, IrReturnValue, 2, returnReg, 1, Int32)
PIG_RULE_get_0(NODE, PIrControlStatement, N_IrIf, 3, node, 0, PIrIf)
PIG_RULE_is_1(NODE, PIrControlStatement, N_IrIf, 3, node, 0, PIrIf)
PIG_RULE_get_1(NODE, PIrControlStatement, N_IrIf, 3, condition, 1, PIrExpr)
PIG_RULE_get_1(NODE, PIrControlStatement, N_IrIf, 3, thenBlock, 2, PIrExpr)
PIG_RULE_get_1(NODE, PIrControlStatement, N_IrIf, 3, elseBlock, 3, PIrExpr)
PIG_RULE_get_14(NODE, PIrControlStatement, IrLoopStart, 4, iterationCount, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrControlStatement, IrLoopStart, 4, iterationCount, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrControlStatement, IrLoopStart, 4, headBlockAddr, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrControlStatement, IrLoopEnd, 5, headBlockAddr, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrControlStatement, IrLoopEnd, 5, headBlockAddr, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrControlStatement, IrLoopEnd, 5, exitBlockAddr, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrExpr, IrConstant, 0, constValue, 0, Int64)
PIG_RULE_is_10(NODE, PIrExpr, IrConstant, 0, constValue, 0, Int64)
PIG_RULE_get_0(NODE, PIrExpr, N_IrUnary, 1, node, 0, PIrUnary)
PIG_RULE_is_2(NODE, PIrExpr, N_IrUnary, 1, node, 0, PIrUnary)
PIG_RULE_get_2(NODE, PIrExpr, N_IrUnary, 1, child, 1, PIrExpr)
PIG_RULE_get_0(NODE, PIrExpr, N_IrBinary, 2, node, 0, PIrBinary)
PIG_RULE_is_3(NODE, PIrExpr, N_IrBinary, 2, node, 0, PIrBinary)
PIG_RULE_get_3(NODE, PIrExpr, N_IrBinary, 2, left, 1, PIrExpr)
PIG_RULE_get_3(NODE, PIrExpr, N_IrBinary, 2, right, 2, PIrExpr)
PIG_RULE_get_14(NODE, PIrExpr, IrCall, 3, functionAddr, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrExpr, IrCall, 3, functionAddr, 0, PIrExpr)
PIG_RULE_get_11(NODE, PIrExpr, IrCall, 3, arguments, 1, LIST_PIrCall_Argument)
PIG_RULE_get_14(NODE, PIrExpr, IrAsm, 4, asmPattern, 0, CString)
PIG_RULE_is_10(NODE, PIrExpr, IrAsm, 4, asmPattern, 0, CString)
PIG_RULE_get_14(NODE, PIrExpr, IrAsm, 4, scratchPattern, 1, CString)
PIG_RULE_get_14(NODE, PIrExpr, IrAsm, 4, exeModes, 2, Int32)
PIG_RULE_get_12(NODE, PIrExpr, IrAsm, 4, infos, 3, LIST_PIrAsm_Info)
PIG_RULE_get_13(NODE, PIrExpr, IrAsm, 4, args, 4, LIST_PIrAsm_Arg)
PIG_RULE_get_7(NODE, PIrCall_Argument, Argument, 0, argExpr, 0, PIrExpr)
PIG_RULE_is_7(NODE, PIrCall_Argument, Argument, 0, argExpr, 0, PIrExpr)
PIG_RULE_get_7(NODE, PIrCall_Argument, Argument, 0, reg, 1, Int32)
PIG_RULE_get_8(NODE, PIrAsm_Arg, Arg, 0, expr, 0, PIrExpr)
PIG_RULE_is_8(NODE, PIrAsm_Arg, Arg, 0, expr, 0, PIrExpr)
PIG_RULE_get_8(NODE, PIrAsm_Arg, Arg, 0, constraint, 1, CString)
PIG_RULE_get_8(NODE, PIrAsm_Arg, Arg, 0, reg, 2, Int32)
PIG_RULE_get_9(NODE, PIrAsm_Info, Info, 0, argNr, 0, Int32)
PIG_RULE_is_9(NODE, PIrAsm_Info, Info, 0, argNr, 0, Int32)
PIG_RULE_get_9(NODE, PIrAsm_Info, Info, 0, group, 1, Int32)
PIG_RULE_get_9(NODE, PIrAsm_Info, Info, 0, accu, 2, Int32)
PIG_RULE_get_9(NODE, PIrAsm_Info, Info, 0, addrModifier, 3, Int32)
PIG_RULE_get_9(NODE, PIrAsm_Info, Info, 0, cplNr, 4, Int32)
PIG_RULE_get_9(NODE, PIrAsm_Info, Info, 0, psr, 5, Int32)
PIG_RULE_get_14(NODE, PIrBinary, IrModuloAdd, 0, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrModuloAdd, 0, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrModuloAdd, 0, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrModuloAdd, 0, lowerBound, 2, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrModuloAdd, 0, moduloSize, 3, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrCircularAdd, 1, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrCircularAdd, 1, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrCircularAdd, 1, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrCircularAdd, 1, moduloSize, 2, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrRevInc, 2, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrRevInc, 2, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrRevInc, 2, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSatAdd, 3, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrSatAdd, 3, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSatAdd, 3, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSatSub, 4, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrSatSub, 4, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSatSub, 4, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSatMult, 5, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrSatMult, 5, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSatMult, 5, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSatDiv, 6, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrSatDiv, 6, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSatDiv, 6, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSatDiv, 6, roundToZero, 2, IrBoolField)
PIG_RULE_get_14(NODE, PIrBinary, IrSatShiftLeft, 7, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrSatShiftLeft, 7, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSatShiftLeft, 7, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrRndShiftRight, 8, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrRndShiftRight, 8, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrRndShiftRight, 8, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrAdd, 9, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrAdd, 9, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrAdd, 9, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrDimAdd, 10, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrDimAdd, 10, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrDimAdd, 10, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrDimAdd, 10, dimension, 2, Int32)
PIG_RULE_get_14(NODE, PIrBinary, IrSub, 11, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrSub, 11, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSub, 11, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrMult, 12, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrMult, 12, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrMult, 12, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrDiv, 13, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrDiv, 13, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrDiv, 13, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrDiv, 13, roundToZero, 2, IrBoolField)
PIG_RULE_get_14(NODE, PIrBinary, IrMod, 14, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrMod, 14, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrMod, 14, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrMod, 14, roundToZero, 2, IrBoolField)
PIG_RULE_get_14(NODE, PIrBinary, IrShiftLeft, 15, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrShiftLeft, 15, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrShiftLeft, 15, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrShiftRight, 16, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrShiftRight, 16, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrShiftRight, 16, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrMax, 17, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrMax, 17, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrMax, 17, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrMin, 18, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrMin, 18, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrMin, 18, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrOr, 19, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrOr, 19, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrOr, 19, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrAnd, 20, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrAnd, 20, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrAnd, 20, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrXor, 21, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrXor, 21, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrXor, 21, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSetBits, 22, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrSetBits, 22, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSetBits, 22, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrSetBits, 22, offset, 2, Int32)
PIG_RULE_get_14(NODE, PIrBinary, IrSetBits, 22, width, 3, Int32)
PIG_RULE_get_14(NODE, PIrBinary, IrComma, 23, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrBinary, IrComma, 23, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrBinary, IrComma, 23, right, 1, PIrExpr)
PIG_RULE_get_0(NODE, PIrBinary, N_IrRelation, 24, node, 0, PIrRelation)
PIG_RULE_is_4(NODE, PIrBinary, N_IrRelation, 24, node, 0, PIrRelation)
PIG_RULE_get_4(NODE, PIrBinary, N_IrRelation, 24, left, 1, PIrExpr)
PIG_RULE_get_4(NODE, PIrBinary, N_IrRelation, 24, right, 2, PIrExpr)
PIG_RULE_get_0(NODE, PIrBinary, N_IrParallelBinary, 25, node, 0, PIrParallelBinary)
PIG_RULE_is_5(NODE, PIrBinary, N_IrParallelBinary, 25, node, 0, PIrParallelBinary)
PIG_RULE_get_5(NODE, PIrBinary, N_IrParallelBinary, 25, left, 1, PIrExpr)
PIG_RULE_get_5(NODE, PIrBinary, N_IrParallelBinary, 25, right, 2, PIrExpr)
PIG_RULE_get_5(NODE, PIrBinary, N_IrParallelBinary, 25, sectionBits, 3, Int32)
PIG_RULE_get_14(NODE, PIrUnary, IrBitReverse, 0, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrBitReverse, 0, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrSatNegate, 1, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrSatNegate, 1, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrSatAbs, 2, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrSatAbs, 2, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrSat, 3, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrSat, 3, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrSatRound, 4, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrSatRound, 4, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrNorm, 5, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrNorm, 5, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrNegate, 6, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrNegate, 6, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrInvert, 7, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrInvert, 7, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrNot, 8, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrNot, 8, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrRead, 9, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrRead, 9, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrConvert, 10, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrConvert, 10, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrBitLoyalConvert, 11, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrBitLoyalConvert, 11, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrRound, 12, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrRound, 12, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrRound, 12, method, 1, Int32)
PIG_RULE_get_14(NODE, PIrUnary, IrAbs, 13, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrAbs, 13, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrSquare, 14, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrSquare, 14, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrGetBits, 15, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrGetBits, 15, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrGetBits, 15, sign, 1, IrBoolField)
PIG_RULE_get_14(NODE, PIrUnary, IrGetBits, 15, offset, 2, Int32)
PIG_RULE_get_14(NODE, PIrUnary, IrGetBits, 15, width, 3, Int32)
PIG_RULE_get_14(NODE, PIrUnary, IrMatchNtrm, 16, child, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrUnary, IrMatchNtrm, 16, child, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrUnary, IrMatchNtrm, 16, ntrm, 1, Int32)
PIG_RULE_get_14(NODE, PIrRelation, IrEqual, 0, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrRelation, IrEqual, 0, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrRelation, IrEqual, 0, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrRelation, IrUnequal, 1, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrRelation, IrUnequal, 1, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrRelation, IrUnequal, 1, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrRelation, IrGreater, 2, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrRelation, IrGreater, 2, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrRelation, IrGreater, 2, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrRelation, IrGreaterEqual, 3, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrRelation, IrGreaterEqual, 3, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrRelation, IrGreaterEqual, 3, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrRelation, IrLess, 4, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrRelation, IrLess, 4, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrRelation, IrLess, 4, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrRelation, IrLessEqual, 5, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrRelation, IrLessEqual, 5, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrRelation, IrLessEqual, 5, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrParallelBinary, IrParallelAdd, 0, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrParallelBinary, IrParallelAdd, 0, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrParallelBinary, IrParallelAdd, 0, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrParallelBinary, IrParallelAdd, 0, sectionBits, 2, Int32)
PIG_RULE_get_14(NODE, PIrParallelBinary, IrParallelSub, 1, left, 0, PIrExpr)
PIG_RULE_is_10(NODE, PIrParallelBinary, IrParallelSub, 1, left, 0, PIrExpr)
PIG_RULE_get_14(NODE, PIrParallelBinary, IrParallelSub, 1, right, 1, PIrExpr)
PIG_RULE_get_14(NODE, PIrParallelBinary, IrParallelSub, 1, sectionBits, 2, Int32)
