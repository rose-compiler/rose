#if 0
Dan,

In ROSE+EDG 3.3, expression_type of SgAggregateInitializer is SgArrayType for SginitializedName x shown below.

//----input ---------cat array.c
void foo()
{
  int x[2] = { 1, 2 };
}
---------------------------------------------
In ROSE+ EDG 4.4, such type info is missing () according to its dot graph

projects/interpreter fails since it cannot grab the correct type in the following code:

ValueP StackFrame::evalInitializedName(SgInitializedName *var, blockScopeVars_t &blockScope, varBindings_t &varBindings, bool isStatic)
   {
...
     SgInitializer *init = var->get_initializer();
     SgType *varType = var->get_type();
     if (isSgArrayType(varType) && init) // in this case, var may have an incomplete array type
                                         // which we need to replace with the complete type
                                         // from the initializer
        {
          varType = init->get_type();
        }
     ValueP binding = newValue(varType, PStack);
...
   }

#endif

void foo()
{
  int x[2] = { 1, 2 };
}

