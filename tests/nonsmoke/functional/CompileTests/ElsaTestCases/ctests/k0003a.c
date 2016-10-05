// old-style function decl with parentheses around func name, returning a
// struct-typedef

// "WARNING: there is no action to merge nonterm KandRFunctionDefinition"

// originally found in package gcl

// ERR-MATCH: merge nonterm KandRFunctionDefinition

typedef struct {} S;

S (foo)(x)
    int x;
{
}


// similar, but this time using implicit-int
/*implint*/ functionName(paramName)
  int paramName;
{}
