// Liao 11/9/2010
// Test a tricky case
//Ideally, #endif in this code should be attached before the function declaration 'buldprocessorMap'
// But It cannot be achieved since the beginning location information for the function is the same
// as 'long int*', which is before '#endif' 
// 
// During the inheritance traversal, ROSE cannot attach '#endif' to some node with a smaller line number 
// than that of '#endif',
// So it will attach '#endif' to the next locatedNode .
// It used to attach it to 'SgBasicBlock' (The function definition body).
// But now it is 'int x' since SgInitializedName is now moved to SgLoatedNodeSupport.

#if defined(PPP)
int *
#else
long int *
#endif
buildProcessorMap ( int x, float y )
{
     return 0;
}

