PIrExpr PIrStatement_IrAssign_get_destAddr(PIrStatement NODE){ return (dynamic_cast<IrAssign *>(NODE))->destAddr.get();}
int is_op_PIrStatement_IrAssign(PIrStatement NODE){ return (dynamic_cast<IrAssign *>(NODE) != 0);}
PIrExpr PIrStatement_IrAssign_get_rightSide(PIrStatement NODE){ return (dynamic_cast<IrAssign *>(NODE))->rightSide.get();}
LIST_PIrMultiAssign_Assign PIrStatement_IrMultiAssign_get_assigns(PIrStatement NODE){ IrMultiAssign *node = dynamic_cast<IrMultiAssign *>(NODE); return new IrNodelistTmpl<IrMultiAssign::Assign>(node->assigns);}
int is_op_PIrStatement_IrMultiAssign(PIrStatement NODE){ return (dynamic_cast<IrMultiAssign *>(NODE) != 0);}
int LIST_PIrMultiAssign_Assign_empty(LIST_PIrMultiAssign_Assign L){ return (L->getFirst() == 0);}
PIrMultiAssign_Assign LIST_PIrMultiAssign_Assign_hd(LIST_PIrMultiAssign_Assign L){ return L->getFirst();}
LIST_PIrMultiAssign_Assign LIST_PIrMultiAssign_Assign_tl(LIST_PIrMultiAssign_Assign L){ L->fetchFirst(); return L;}
PIrExpr PIrStatement_IrMultiAssign_get_rightSide(PIrStatement NODE){ return (dynamic_cast<IrMultiAssign *>(NODE))->rightSide.get();}
PIrExpr PIrStatement_IrEvaluate_get_expression(PIrStatement NODE){ return (dynamic_cast<IrEvaluate *>(NODE))->expression.get();}
int is_op_PIrStatement_IrEvaluate(PIrStatement NODE){ return (dynamic_cast<IrEvaluate *>(NODE) != 0);}
PIrControlStatement PIrStatement_N_IrControlStatement_get_node(PIrStatement NODE){ return dynamic_cast<PIrControlStatement>(NODE);}
int is_op_PIrStatement_N_IrControlStatement(PIrStatement NODE){ return (dynamic_cast<PIrControlStatement>(NODE) != 0);}
PIrExpr PIrStatement_IrModuloLimit_get_localUse(PIrStatement NODE){ return (dynamic_cast<IrModuloLimit *>(NODE))->localUse.get();}
int is_op_PIrStatement_IrModuloLimit(PIrStatement NODE){ return (dynamic_cast<IrModuloLimit *>(NODE) != 0);}
Int64 PIrStatement_IrModuloLimit_get_modulo(PIrStatement NODE){ return (dynamic_cast<IrModuloLimit *>(NODE))->modulo.get();}
PIrExpr PIrMultiAssign_Assign_Assign_get_destAddr(PIrMultiAssign_Assign NODE){ return NODE->destAddr.get();}
int is_op_PIrMultiAssign_Assign_Assign(PIrMultiAssign_Assign NODE){ return 1;}
CString PIrMultiAssign_Assign_Assign_get_constraint(PIrMultiAssign_Assign NODE){ return NODE->constraint.get();}
Int32 PIrMultiAssign_Assign_Assign_get_reg(PIrMultiAssign_Assign NODE){ return NODE->reg.get();}
PIrExpr PIrControlStatement_IrJump_get_destBlock(PIrControlStatement NODE){ return (dynamic_cast<IrJump *>(NODE))->destBlock.get();}
int is_op_PIrControlStatement_IrJump(PIrControlStatement NODE){ return (dynamic_cast<IrJump *>(NODE) != 0);}
PIrExpr PIrControlStatement_IrReturnValue_get_returnExpr(PIrControlStatement NODE){ return (dynamic_cast<IrReturnValue *>(NODE))->returnExpr.get();}
int is_op_PIrControlStatement_IrReturnValue(PIrControlStatement NODE){ return (dynamic_cast<IrReturnValue *>(NODE) != 0);}
Int32 PIrControlStatement_IrReturnValue_get_returnReg(PIrControlStatement NODE){ return (dynamic_cast<IrReturnValue *>(NODE))->returnReg.get();}
PIrIf PIrControlStatement_N_IrIf_get_node(PIrControlStatement NODE){ return dynamic_cast<PIrIf>(NODE);}
int is_op_PIrControlStatement_N_IrIf(PIrControlStatement NODE){ return (dynamic_cast<PIrControlStatement>(NODE) != 0);}
PIrExpr PIrControlStatement_N_IrIf_get_condition(PIrControlStatement NODE){ return (dynamic_cast<PIrIf>(NODE))->condition.get();}
PIrExpr PIrControlStatement_N_IrIf_get_thenBlock(PIrControlStatement NODE){ return (dynamic_cast<PIrIf>(NODE))->thenBlock.get();}
PIrExpr PIrControlStatement_N_IrIf_get_elseBlock(PIrControlStatement NODE){ return (dynamic_cast<PIrIf>(NODE))->elseBlock.get();}
PIrExpr PIrControlStatement_IrLoopStart_get_iterationCount(PIrControlStatement NODE){ return (dynamic_cast<IrLoopStart *>(NODE))->iterationCount.get();}
int is_op_PIrControlStatement_IrLoopStart(PIrControlStatement NODE){ return (dynamic_cast<IrLoopStart *>(NODE) != 0);}
PIrExpr PIrControlStatement_IrLoopStart_get_headBlockAddr(PIrControlStatement NODE){ return (dynamic_cast<IrLoopStart *>(NODE))->headBlockAddr.get();}
PIrExpr PIrControlStatement_IrLoopEnd_get_headBlockAddr(PIrControlStatement NODE){ return (dynamic_cast<IrLoopEnd *>(NODE))->headBlockAddr.get();}
int is_op_PIrControlStatement_IrLoopEnd(PIrControlStatement NODE){ return (dynamic_cast<IrLoopEnd *>(NODE) != 0);}
PIrExpr PIrControlStatement_IrLoopEnd_get_exitBlockAddr(PIrControlStatement NODE){ return (dynamic_cast<IrLoopEnd *>(NODE))->exitBlockAddr.get();}
Int64 PIrExpr_IrConstant_get_constValue(PIrExpr NODE){ return (dynamic_cast<IrConstant *>(NODE))->constValue.get();}
int is_op_PIrExpr_IrConstant(PIrExpr NODE){ return (dynamic_cast<IrConstant *>(NODE) != 0);}
PIrUnary PIrExpr_N_IrUnary_get_node(PIrExpr NODE){ return dynamic_cast<PIrUnary>(NODE);}
int is_op_PIrExpr_N_IrUnary(PIrExpr NODE){ return (dynamic_cast<PIrControlStatement>(NODE) != 0);}
PIrExpr PIrExpr_N_IrUnary_get_child(PIrExpr NODE){ return (dynamic_cast<PIrIf>(NODE))->child.get();}
PIrBinary PIrExpr_N_IrBinary_get_node(PIrExpr NODE){ return dynamic_cast<PIrBinary>(NODE);}
int is_op_PIrExpr_N_IrBinary(PIrExpr NODE){ return (dynamic_cast<PIrControlStatement>(NODE) != 0);}
PIrExpr PIrExpr_N_IrBinary_get_left(PIrExpr NODE){ return (dynamic_cast<PIrIf>(NODE))->left.get();}
PIrExpr PIrExpr_N_IrBinary_get_right(PIrExpr NODE){ return (dynamic_cast<PIrIf>(NODE))->right.get();}
PIrExpr PIrExpr_IrCall_get_functionAddr(PIrExpr NODE){ return (dynamic_cast<IrCall *>(NODE))->functionAddr.get();}
int is_op_PIrExpr_IrCall(PIrExpr NODE){ return (dynamic_cast<IrCall *>(NODE) != 0);}
LIST_PIrCall_Argument PIrExpr_IrCall_get_arguments(PIrExpr NODE){ IrCall *node = dynamic_cast<IrCall *>(NODE); return new IrNodelistTmpl<IrCall::Argument>(node->arguments);}
CString PIrExpr_IrAsm_get_asmPattern(PIrExpr NODE){ return (dynamic_cast<IrAsm *>(NODE))->asmPattern.get();}
int is_op_PIrExpr_IrAsm(PIrExpr NODE){ return (dynamic_cast<IrAsm *>(NODE) != 0);}
CString PIrExpr_IrAsm_get_scratchPattern(PIrExpr NODE){ return (dynamic_cast<IrAsm *>(NODE))->scratchPattern.get();}
Int32 PIrExpr_IrAsm_get_exeModes(PIrExpr NODE){ return (dynamic_cast<IrAsm *>(NODE))->exeModes.get();}
LIST_PIrAsm_Info PIrExpr_IrAsm_get_infos(PIrExpr NODE){ IrAsm *node = dynamic_cast<IrAsm *>(NODE); return new IrNodelistTmpl<IrAsm::Info>(node->infos);}
LIST_PIrAsm_Arg PIrExpr_IrAsm_get_args(PIrExpr NODE){ IrAsm *node = dynamic_cast<IrAsm *>(NODE); return new IrNodelistTmpl<IrAsm::Arg>(node->args);}
PIrExpr PIrCall_Argument_Argument_get_argExpr(PIrCall_Argument NODE){ return NODE->argExpr.get();}
int is_op_PIrCall_Argument_Argument(PIrCall_Argument NODE){ return 1;}
Int32 PIrCall_Argument_Argument_get_reg(PIrCall_Argument NODE){ return NODE->reg.get();}
PIrExpr PIrAsm_Arg_Arg_get_expr(PIrAsm_Arg NODE){ return NODE->expr.get();}
int is_op_PIrAsm_Arg_Arg(PIrAsm_Arg NODE){ return 1;}
CString PIrAsm_Arg_Arg_get_constraint(PIrAsm_Arg NODE){ return NODE->constraint.get();}
Int32 PIrAsm_Arg_Arg_get_reg(PIrAsm_Arg NODE){ return NODE->reg.get();}
Int32 PIrAsm_Info_Info_get_argNr(PIrAsm_Info NODE){ return NODE->argNr.get();}
int is_op_PIrAsm_Info_Info(PIrAsm_Info NODE){ return 1;}
Int32 PIrAsm_Info_Info_get_group(PIrAsm_Info NODE){ return NODE->group.get();}
Int32 PIrAsm_Info_Info_get_accu(PIrAsm_Info NODE){ return NODE->accu.get();}
Int32 PIrAsm_Info_Info_get_addrModifier(PIrAsm_Info NODE){ return NODE->addrModifier.get();}
Int32 PIrAsm_Info_Info_get_cplNr(PIrAsm_Info NODE){ return NODE->cplNr.get();}
Int32 PIrAsm_Info_Info_get_psr(PIrAsm_Info NODE){ return NODE->psr.get();}
PIrExpr PIrBinary_IrModuloAdd_get_left(PIrBinary NODE){ return (dynamic_cast<IrModuloAdd *>(NODE))->left.get();}
int is_op_PIrBinary_IrModuloAdd(PIrBinary NODE){ return (dynamic_cast<IrModuloAdd *>(NODE) != 0);}
PIrExpr PIrBinary_IrModuloAdd_get_right(PIrBinary NODE){ return (dynamic_cast<IrModuloAdd *>(NODE))->right.get();}
PIrExpr PIrBinary_IrModuloAdd_get_lowerBound(PIrBinary NODE){ return (dynamic_cast<IrModuloAdd *>(NODE))->lowerBound.get();}
PIrExpr PIrBinary_IrModuloAdd_get_moduloSize(PIrBinary NODE){ return (dynamic_cast<IrModuloAdd *>(NODE))->moduloSize.get();}
PIrExpr PIrBinary_IrCircularAdd_get_left(PIrBinary NODE){ return (dynamic_cast<IrCircularAdd *>(NODE))->left.get();}
int is_op_PIrBinary_IrCircularAdd(PIrBinary NODE){ return (dynamic_cast<IrCircularAdd *>(NODE) != 0);}
PIrExpr PIrBinary_IrCircularAdd_get_right(PIrBinary NODE){ return (dynamic_cast<IrCircularAdd *>(NODE))->right.get();}
PIrExpr PIrBinary_IrCircularAdd_get_moduloSize(PIrBinary NODE){ return (dynamic_cast<IrCircularAdd *>(NODE))->moduloSize.get();}
PIrExpr PIrBinary_IrRevInc_get_left(PIrBinary NODE){ return (dynamic_cast<IrRevInc *>(NODE))->left.get();}
int is_op_PIrBinary_IrRevInc(PIrBinary NODE){ return (dynamic_cast<IrRevInc *>(NODE) != 0);}
PIrExpr PIrBinary_IrRevInc_get_right(PIrBinary NODE){ return (dynamic_cast<IrRevInc *>(NODE))->right.get();}
PIrExpr PIrBinary_IrSatAdd_get_left(PIrBinary NODE){ return (dynamic_cast<IrSatAdd *>(NODE))->left.get();}
int is_op_PIrBinary_IrSatAdd(PIrBinary NODE){ return (dynamic_cast<IrSatAdd *>(NODE) != 0);}
PIrExpr PIrBinary_IrSatAdd_get_right(PIrBinary NODE){ return (dynamic_cast<IrSatAdd *>(NODE))->right.get();}
PIrExpr PIrBinary_IrSatSub_get_left(PIrBinary NODE){ return (dynamic_cast<IrSatSub *>(NODE))->left.get();}
int is_op_PIrBinary_IrSatSub(PIrBinary NODE){ return (dynamic_cast<IrSatSub *>(NODE) != 0);}
PIrExpr PIrBinary_IrSatSub_get_right(PIrBinary NODE){ return (dynamic_cast<IrSatSub *>(NODE))->right.get();}
PIrExpr PIrBinary_IrSatMult_get_left(PIrBinary NODE){ return (dynamic_cast<IrSatMult *>(NODE))->left.get();}
int is_op_PIrBinary_IrSatMult(PIrBinary NODE){ return (dynamic_cast<IrSatMult *>(NODE) != 0);}
PIrExpr PIrBinary_IrSatMult_get_right(PIrBinary NODE){ return (dynamic_cast<IrSatMult *>(NODE))->right.get();}
PIrExpr PIrBinary_IrSatDiv_get_left(PIrBinary NODE){ return (dynamic_cast<IrSatDiv *>(NODE))->left.get();}
int is_op_PIrBinary_IrSatDiv(PIrBinary NODE){ return (dynamic_cast<IrSatDiv *>(NODE) != 0);}
PIrExpr PIrBinary_IrSatDiv_get_right(PIrBinary NODE){ return (dynamic_cast<IrSatDiv *>(NODE))->right.get();}
IrBoolField PIrBinary_IrSatDiv_get_roundToZero(PIrBinary NODE){ return (dynamic_cast<IrSatDiv *>(NODE))->roundToZero.get();}
PIrExpr PIrBinary_IrSatShiftLeft_get_left(PIrBinary NODE){ return (dynamic_cast<IrSatShiftLeft *>(NODE))->left.get();}
int is_op_PIrBinary_IrSatShiftLeft(PIrBinary NODE){ return (dynamic_cast<IrSatShiftLeft *>(NODE) != 0);}
PIrExpr PIrBinary_IrSatShiftLeft_get_right(PIrBinary NODE){ return (dynamic_cast<IrSatShiftLeft *>(NODE))->right.get();}
PIrExpr PIrBinary_IrRndShiftRight_get_left(PIrBinary NODE){ return (dynamic_cast<IrRndShiftRight *>(NODE))->left.get();}
int is_op_PIrBinary_IrRndShiftRight(PIrBinary NODE){ return (dynamic_cast<IrRndShiftRight *>(NODE) != 0);}
PIrExpr PIrBinary_IrRndShiftRight_get_right(PIrBinary NODE){ return (dynamic_cast<IrRndShiftRight *>(NODE))->right.get();}
PIrExpr PIrBinary_IrAdd_get_left(PIrBinary NODE){ return (dynamic_cast<IrAdd *>(NODE))->left.get();}
int is_op_PIrBinary_IrAdd(PIrBinary NODE){ return (dynamic_cast<IrAdd *>(NODE) != 0);}
PIrExpr PIrBinary_IrAdd_get_right(PIrBinary NODE){ return (dynamic_cast<IrAdd *>(NODE))->right.get();}
PIrExpr PIrBinary_IrDimAdd_get_left(PIrBinary NODE){ return (dynamic_cast<IrDimAdd *>(NODE))->left.get();}
int is_op_PIrBinary_IrDimAdd(PIrBinary NODE){ return (dynamic_cast<IrDimAdd *>(NODE) != 0);}
PIrExpr PIrBinary_IrDimAdd_get_right(PIrBinary NODE){ return (dynamic_cast<IrDimAdd *>(NODE))->right.get();}
Int32 PIrBinary_IrDimAdd_get_dimension(PIrBinary NODE){ return (dynamic_cast<IrDimAdd *>(NODE))->dimension.get();}
PIrExpr PIrBinary_IrSub_get_left(PIrBinary NODE){ return (dynamic_cast<IrSub *>(NODE))->left.get();}
int is_op_PIrBinary_IrSub(PIrBinary NODE){ return (dynamic_cast<IrSub *>(NODE) != 0);}
PIrExpr PIrBinary_IrSub_get_right(PIrBinary NODE){ return (dynamic_cast<IrSub *>(NODE))->right.get();}
PIrExpr PIrBinary_IrMult_get_left(PIrBinary NODE){ return (dynamic_cast<IrMult *>(NODE))->left.get();}
int is_op_PIrBinary_IrMult(PIrBinary NODE){ return (dynamic_cast<IrMult *>(NODE) != 0);}
PIrExpr PIrBinary_IrMult_get_right(PIrBinary NODE){ return (dynamic_cast<IrMult *>(NODE))->right.get();}
PIrExpr PIrBinary_IrDiv_get_left(PIrBinary NODE){ return (dynamic_cast<IrDiv *>(NODE))->left.get();}
int is_op_PIrBinary_IrDiv(PIrBinary NODE){ return (dynamic_cast<IrDiv *>(NODE) != 0);}
PIrExpr PIrBinary_IrDiv_get_right(PIrBinary NODE){ return (dynamic_cast<IrDiv *>(NODE))->right.get();}
IrBoolField PIrBinary_IrDiv_get_roundToZero(PIrBinary NODE){ return (dynamic_cast<IrDiv *>(NODE))->roundToZero.get();}
PIrExpr PIrBinary_IrMod_get_left(PIrBinary NODE){ return (dynamic_cast<IrMod *>(NODE))->left.get();}
int is_op_PIrBinary_IrMod(PIrBinary NODE){ return (dynamic_cast<IrMod *>(NODE) != 0);}
PIrExpr PIrBinary_IrMod_get_right(PIrBinary NODE){ return (dynamic_cast<IrMod *>(NODE))->right.get();}
IrBoolField PIrBinary_IrMod_get_roundToZero(PIrBinary NODE){ return (dynamic_cast<IrMod *>(NODE))->roundToZero.get();}
PIrExpr PIrBinary_IrShiftLeft_get_left(PIrBinary NODE){ return (dynamic_cast<IrShiftLeft *>(NODE))->left.get();}
int is_op_PIrBinary_IrShiftLeft(PIrBinary NODE){ return (dynamic_cast<IrShiftLeft *>(NODE) != 0);}
PIrExpr PIrBinary_IrShiftLeft_get_right(PIrBinary NODE){ return (dynamic_cast<IrShiftLeft *>(NODE))->right.get();}
PIrExpr PIrBinary_IrShiftRight_get_left(PIrBinary NODE){ return (dynamic_cast<IrShiftRight *>(NODE))->left.get();}
int is_op_PIrBinary_IrShiftRight(PIrBinary NODE){ return (dynamic_cast<IrShiftRight *>(NODE) != 0);}
PIrExpr PIrBinary_IrShiftRight_get_right(PIrBinary NODE){ return (dynamic_cast<IrShiftRight *>(NODE))->right.get();}
PIrExpr PIrBinary_IrMax_get_left(PIrBinary NODE){ return (dynamic_cast<IrMax *>(NODE))->left.get();}
int is_op_PIrBinary_IrMax(PIrBinary NODE){ return (dynamic_cast<IrMax *>(NODE) != 0);}
PIrExpr PIrBinary_IrMax_get_right(PIrBinary NODE){ return (dynamic_cast<IrMax *>(NODE))->right.get();}
PIrExpr PIrBinary_IrMin_get_left(PIrBinary NODE){ return (dynamic_cast<IrMin *>(NODE))->left.get();}
int is_op_PIrBinary_IrMin(PIrBinary NODE){ return (dynamic_cast<IrMin *>(NODE) != 0);}
PIrExpr PIrBinary_IrMin_get_right(PIrBinary NODE){ return (dynamic_cast<IrMin *>(NODE))->right.get();}
PIrExpr PIrBinary_IrOr_get_left(PIrBinary NODE){ return (dynamic_cast<IrOr *>(NODE))->left.get();}
int is_op_PIrBinary_IrOr(PIrBinary NODE){ return (dynamic_cast<IrOr *>(NODE) != 0);}
PIrExpr PIrBinary_IrOr_get_right(PIrBinary NODE){ return (dynamic_cast<IrOr *>(NODE))->right.get();}
PIrExpr PIrBinary_IrAnd_get_left(PIrBinary NODE){ return (dynamic_cast<IrAnd *>(NODE))->left.get();}
int is_op_PIrBinary_IrAnd(PIrBinary NODE){ return (dynamic_cast<IrAnd *>(NODE) != 0);}
PIrExpr PIrBinary_IrAnd_get_right(PIrBinary NODE){ return (dynamic_cast<IrAnd *>(NODE))->right.get();}
PIrExpr PIrBinary_IrXor_get_left(PIrBinary NODE){ return (dynamic_cast<IrXor *>(NODE))->left.get();}
int is_op_PIrBinary_IrXor(PIrBinary NODE){ return (dynamic_cast<IrXor *>(NODE) != 0);}
PIrExpr PIrBinary_IrXor_get_right(PIrBinary NODE){ return (dynamic_cast<IrXor *>(NODE))->right.get();}
PIrExpr PIrBinary_IrSetBits_get_left(PIrBinary NODE){ return (dynamic_cast<IrSetBits *>(NODE))->left.get();}
int is_op_PIrBinary_IrSetBits(PIrBinary NODE){ return (dynamic_cast<IrSetBits *>(NODE) != 0);}
PIrExpr PIrBinary_IrSetBits_get_right(PIrBinary NODE){ return (dynamic_cast<IrSetBits *>(NODE))->right.get();}
Int32 PIrBinary_IrSetBits_get_offset(PIrBinary NODE){ return (dynamic_cast<IrSetBits *>(NODE))->offset.get();}
Int32 PIrBinary_IrSetBits_get_width(PIrBinary NODE){ return (dynamic_cast<IrSetBits *>(NODE))->width.get();}
PIrExpr PIrBinary_IrComma_get_left(PIrBinary NODE){ return (dynamic_cast<IrComma *>(NODE))->left.get();}
int is_op_PIrBinary_IrComma(PIrBinary NODE){ return (dynamic_cast<IrComma *>(NODE) != 0);}
PIrExpr PIrBinary_IrComma_get_right(PIrBinary NODE){ return (dynamic_cast<IrComma *>(NODE))->right.get();}
PIrRelation PIrBinary_N_IrRelation_get_node(PIrBinary NODE){ return dynamic_cast<PIrRelation>(NODE);}
int is_op_PIrBinary_N_IrRelation(PIrBinary NODE){ return (dynamic_cast<PIrControlStatement>(NODE) != 0);}
PIrExpr PIrBinary_N_IrRelation_get_left(PIrBinary NODE){ return (dynamic_cast<PIrIf>(NODE))->left.get();}
PIrExpr PIrBinary_N_IrRelation_get_right(PIrBinary NODE){ return (dynamic_cast<PIrIf>(NODE))->right.get();}
PIrParallelBinary PIrBinary_N_IrParallelBinary_get_node(PIrBinary NODE){ return dynamic_cast<PIrParallelBinary>(NODE);}
int is_op_PIrBinary_N_IrParallelBinary(PIrBinary NODE){ return (dynamic_cast<PIrControlStatement>(NODE) != 0);}
PIrExpr PIrBinary_N_IrParallelBinary_get_left(PIrBinary NODE){ return (dynamic_cast<PIrIf>(NODE))->left.get();}
PIrExpr PIrBinary_N_IrParallelBinary_get_right(PIrBinary NODE){ return (dynamic_cast<PIrIf>(NODE))->right.get();}
Int32 PIrBinary_N_IrParallelBinary_get_sectionBits(PIrBinary NODE){ return (dynamic_cast<PIrIf>(NODE))->sectionBits.get();}
PIrExpr PIrUnary_IrBitReverse_get_child(PIrUnary NODE){ return (dynamic_cast<IrBitReverse *>(NODE))->child.get();}
int is_op_PIrUnary_IrBitReverse(PIrUnary NODE){ return (dynamic_cast<IrBitReverse *>(NODE) != 0);}
PIrExpr PIrUnary_IrSatNegate_get_child(PIrUnary NODE){ return (dynamic_cast<IrSatNegate *>(NODE))->child.get();}
int is_op_PIrUnary_IrSatNegate(PIrUnary NODE){ return (dynamic_cast<IrSatNegate *>(NODE) != 0);}
PIrExpr PIrUnary_IrSatAbs_get_child(PIrUnary NODE){ return (dynamic_cast<IrSatAbs *>(NODE))->child.get();}
int is_op_PIrUnary_IrSatAbs(PIrUnary NODE){ return (dynamic_cast<IrSatAbs *>(NODE) != 0);}
PIrExpr PIrUnary_IrSat_get_child(PIrUnary NODE){ return (dynamic_cast<IrSat *>(NODE))->child.get();}
int is_op_PIrUnary_IrSat(PIrUnary NODE){ return (dynamic_cast<IrSat *>(NODE) != 0);}
PIrExpr PIrUnary_IrSatRound_get_child(PIrUnary NODE){ return (dynamic_cast<IrSatRound *>(NODE))->child.get();}
int is_op_PIrUnary_IrSatRound(PIrUnary NODE){ return (dynamic_cast<IrSatRound *>(NODE) != 0);}
PIrExpr PIrUnary_IrNorm_get_child(PIrUnary NODE){ return (dynamic_cast<IrNorm *>(NODE))->child.get();}
int is_op_PIrUnary_IrNorm(PIrUnary NODE){ return (dynamic_cast<IrNorm *>(NODE) != 0);}
PIrExpr PIrUnary_IrNegate_get_child(PIrUnary NODE){ return (dynamic_cast<IrNegate *>(NODE))->child.get();}
int is_op_PIrUnary_IrNegate(PIrUnary NODE){ return (dynamic_cast<IrNegate *>(NODE) != 0);}
PIrExpr PIrUnary_IrInvert_get_child(PIrUnary NODE){ return (dynamic_cast<IrInvert *>(NODE))->child.get();}
int is_op_PIrUnary_IrInvert(PIrUnary NODE){ return (dynamic_cast<IrInvert *>(NODE) != 0);}
PIrExpr PIrUnary_IrNot_get_child(PIrUnary NODE){ return (dynamic_cast<IrNot *>(NODE))->child.get();}
int is_op_PIrUnary_IrNot(PIrUnary NODE){ return (dynamic_cast<IrNot *>(NODE) != 0);}
PIrExpr PIrUnary_IrRead_get_child(PIrUnary NODE){ return (dynamic_cast<IrRead *>(NODE))->child.get();}
int is_op_PIrUnary_IrRead(PIrUnary NODE){ return (dynamic_cast<IrRead *>(NODE) != 0);}
PIrExpr PIrUnary_IrConvert_get_child(PIrUnary NODE){ return (dynamic_cast<IrConvert *>(NODE))->child.get();}
int is_op_PIrUnary_IrConvert(PIrUnary NODE){ return (dynamic_cast<IrConvert *>(NODE) != 0);}
PIrExpr PIrUnary_IrBitLoyalConvert_get_child(PIrUnary NODE){ return (dynamic_cast<IrBitLoyalConvert *>(NODE))->child.get();}
int is_op_PIrUnary_IrBitLoyalConvert(PIrUnary NODE){ return (dynamic_cast<IrBitLoyalConvert *>(NODE) != 0);}
PIrExpr PIrUnary_IrRound_get_child(PIrUnary NODE){ return (dynamic_cast<IrRound *>(NODE))->child.get();}
int is_op_PIrUnary_IrRound(PIrUnary NODE){ return (dynamic_cast<IrRound *>(NODE) != 0);}
Int32 PIrUnary_IrRound_get_method(PIrUnary NODE){ return (dynamic_cast<IrRound *>(NODE))->method.get();}
PIrExpr PIrUnary_IrAbs_get_child(PIrUnary NODE){ return (dynamic_cast<IrAbs *>(NODE))->child.get();}
int is_op_PIrUnary_IrAbs(PIrUnary NODE){ return (dynamic_cast<IrAbs *>(NODE) != 0);}
PIrExpr PIrUnary_IrSquare_get_child(PIrUnary NODE){ return (dynamic_cast<IrSquare *>(NODE))->child.get();}
int is_op_PIrUnary_IrSquare(PIrUnary NODE){ return (dynamic_cast<IrSquare *>(NODE) != 0);}
PIrExpr PIrUnary_IrGetBits_get_child(PIrUnary NODE){ return (dynamic_cast<IrGetBits *>(NODE))->child.get();}
int is_op_PIrUnary_IrGetBits(PIrUnary NODE){ return (dynamic_cast<IrGetBits *>(NODE) != 0);}
IrBoolField PIrUnary_IrGetBits_get_sign(PIrUnary NODE){ return (dynamic_cast<IrGetBits *>(NODE))->sign.get();}
Int32 PIrUnary_IrGetBits_get_offset(PIrUnary NODE){ return (dynamic_cast<IrGetBits *>(NODE))->offset.get();}
Int32 PIrUnary_IrGetBits_get_width(PIrUnary NODE){ return (dynamic_cast<IrGetBits *>(NODE))->width.get();}
PIrExpr PIrUnary_IrMatchNtrm_get_child(PIrUnary NODE){ return (dynamic_cast<IrMatchNtrm *>(NODE))->child.get();}
int is_op_PIrUnary_IrMatchNtrm(PIrUnary NODE){ return (dynamic_cast<IrMatchNtrm *>(NODE) != 0);}
Int32 PIrUnary_IrMatchNtrm_get_ntrm(PIrUnary NODE){ return (dynamic_cast<IrMatchNtrm *>(NODE))->ntrm.get();}
PIrExpr PIrRelation_IrEqual_get_left(PIrRelation NODE){ return (dynamic_cast<IrEqual *>(NODE))->left.get();}
int is_op_PIrRelation_IrEqual(PIrRelation NODE){ return (dynamic_cast<IrEqual *>(NODE) != 0);}
PIrExpr PIrRelation_IrEqual_get_right(PIrRelation NODE){ return (dynamic_cast<IrEqual *>(NODE))->right.get();}
PIrExpr PIrRelation_IrUnequal_get_left(PIrRelation NODE){ return (dynamic_cast<IrUnequal *>(NODE))->left.get();}
int is_op_PIrRelation_IrUnequal(PIrRelation NODE){ return (dynamic_cast<IrUnequal *>(NODE) != 0);}
PIrExpr PIrRelation_IrUnequal_get_right(PIrRelation NODE){ return (dynamic_cast<IrUnequal *>(NODE))->right.get();}
PIrExpr PIrRelation_IrGreater_get_left(PIrRelation NODE){ return (dynamic_cast<IrGreater *>(NODE))->left.get();}
int is_op_PIrRelation_IrGreater(PIrRelation NODE){ return (dynamic_cast<IrGreater *>(NODE) != 0);}
PIrExpr PIrRelation_IrGreater_get_right(PIrRelation NODE){ return (dynamic_cast<IrGreater *>(NODE))->right.get();}
PIrExpr PIrRelation_IrGreaterEqual_get_left(PIrRelation NODE){ return (dynamic_cast<IrGreaterEqual *>(NODE))->left.get();}
int is_op_PIrRelation_IrGreaterEqual(PIrRelation NODE){ return (dynamic_cast<IrGreaterEqual *>(NODE) != 0);}
PIrExpr PIrRelation_IrGreaterEqual_get_right(PIrRelation NODE){ return (dynamic_cast<IrGreaterEqual *>(NODE))->right.get();}
PIrExpr PIrRelation_IrLess_get_left(PIrRelation NODE){ return (dynamic_cast<IrLess *>(NODE))->left.get();}
int is_op_PIrRelation_IrLess(PIrRelation NODE){ return (dynamic_cast<IrLess *>(NODE) != 0);}
PIrExpr PIrRelation_IrLess_get_right(PIrRelation NODE){ return (dynamic_cast<IrLess *>(NODE))->right.get();}
PIrExpr PIrRelation_IrLessEqual_get_left(PIrRelation NODE){ return (dynamic_cast<IrLessEqual *>(NODE))->left.get();}
int is_op_PIrRelation_IrLessEqual(PIrRelation NODE){ return (dynamic_cast<IrLessEqual *>(NODE) != 0);}
PIrExpr PIrRelation_IrLessEqual_get_right(PIrRelation NODE){ return (dynamic_cast<IrLessEqual *>(NODE))->right.get();}
PIrExpr PIrParallelBinary_IrParallelAdd_get_left(PIrParallelBinary NODE){ return (dynamic_cast<IrParallelAdd *>(NODE))->left.get();}
int is_op_PIrParallelBinary_IrParallelAdd(PIrParallelBinary NODE){ return (dynamic_cast<IrParallelAdd *>(NODE) != 0);}
PIrExpr PIrParallelBinary_IrParallelAdd_get_right(PIrParallelBinary NODE){ return (dynamic_cast<IrParallelAdd *>(NODE))->right.get();}
Int32 PIrParallelBinary_IrParallelAdd_get_sectionBits(PIrParallelBinary NODE){ return (dynamic_cast<IrParallelAdd *>(NODE))->sectionBits.get();}
PIrExpr PIrParallelBinary_IrParallelSub_get_left(PIrParallelBinary NODE){ return (dynamic_cast<IrParallelSub *>(NODE))->left.get();}
int is_op_PIrParallelBinary_IrParallelSub(PIrParallelBinary NODE){ return (dynamic_cast<IrParallelSub *>(NODE) != 0);}
PIrExpr PIrParallelBinary_IrParallelSub_get_right(PIrParallelBinary NODE){ return (dynamic_cast<IrParallelSub *>(NODE))->right.get();}
Int32 PIrParallelBinary_IrParallelSub_get_sectionBits(PIrParallelBinary NODE){ return (dynamic_cast<IrParallelSub *>(NODE))->sectionBits.get();}
