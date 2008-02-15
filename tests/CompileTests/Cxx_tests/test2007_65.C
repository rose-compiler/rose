
#if 0

the attached code gives the following error when compiled with ROSE:
lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-14a-2007/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:20916:
SgExpression* sage_gen_expr(an_expr_node*, a_boolean,
DataRequiredForComputationOfSourcePostionInformation*): Assertion
operand_1->kind == (an_expr_node_kind)enk_variable_address failed.
Aborted (core dumped) 
#endif

typedef struct {
   __builtin_va_list ap;
} ScanfState;

void GetInt(ScanfState *state)
{
    *__builtin_va_arg(state->ap,int *);
}

