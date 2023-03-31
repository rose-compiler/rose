/** Base class for references to a machine register. */
class SgAsmStackExpression: public SgAsmExpression {
    /** Property: Position of element referenced on the stack.
     *
     *  This is a value that references the values on the stack (zero is top of stack, positive numbers are the depth into the
     *  stack). */
    [[using Rosebud: rosetta, ctor_arg]]
    int stack_position = 0;
};
