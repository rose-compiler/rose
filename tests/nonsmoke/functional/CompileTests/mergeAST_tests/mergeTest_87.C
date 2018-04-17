// declaration and use of a function pointer builds a function declaration internally
// which should be marked as compiler generated.
typedef void (*function_pointer)(void);
function_pointer x;

