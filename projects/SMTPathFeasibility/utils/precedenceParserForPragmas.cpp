// this is needed for pragma parsing because the equations given by the user
// are not handled by C++

/*
Precendence Rules
1) 	left-to-right
	:: scope resolution

2) 	left-to-right
	++ -- suffix/postfix increment decrement
	type() type{} function-style typecast
	() function call
	[] array subscripting
	. element selection by reference
	-> element selection through pointer

3)	right-to-left
	++ -- prefix increment decrement
	+ - unary plus and minus	
	! ~ logical NOT and bitwise NOT
	(type) C-style type cast
	* indirection (dereference)
	& address-of
	sizeof size of
	new, new[] Dynamic Memory Allocation
	delete, delete[] Dynamic Memory Deallocation

4)	Left to right
	.* ->*, pointer to member

5)	Left to right
	* / %, times divide mod

6)	Left to right
	+ -, addition subtraction
7) 	Left to right
	<< >>, bitwise left and right shift
8)	Left to right
	<, <=, >, >=	for relational operators < and <= , for relational operators > and >=
9)	Left-to-right
	==, != for relational eq and neq
10)	left to right
	& bitwise AND
11)	left to right
	^ bitwise XOR
12) 	left-to-right
	| bitwise OR
13)	left-to-right
	&& logical AND
14) 	left-to-right 
	|| logical OR
15)	right-to-left
	?: Ternary Conditional
	throw, throw operator
	=, direct assignment
	+=, -=, assignment by sum/difference
	*=,/=,%= assignment by product, quotient, modulus
	<<==, >>== assignment by bitwise left shift and right shift
	&= ^= |=, assignment by bitwise AND,XOR,OR
16) left-to-right
	, comma
*/
// Dijkstra's Shunting-yard algorithm
// READ token

namespace PragmaShuntingAlgorithm {

// parser based heavily on https://en.wikipedia.org/wiki/Recursive_descent_parser#C_implementation as of July 8,2015.
// all errors should be assumed to be mine and do not reflect the wikipedia code


class tokenizer {

void nextsym(void);
void error(const char msg[]);


/*
Grammar for pragmas

[S]
S -> Stat ',' S | Stat
Stat -> Exp Rel Exp
Exp -> Exp BinaryOp Exp | UnaryOp Exp | Exp PostfixUnary | NUMBER | ident
BinaryOp -> DIVIDE | MULTIPLY | ADD | SUBTRACT | MODULUS | LOGICALOR | LOGICALAND | BITWISEAND | BITWISEOR | BITWISEXOR | BITWISELEFTSHIFT | BITWISERIGHTSHIFT
UnaryOp -> UNARYMINUS | UNARYPLUS | PLUSPLUSPREFIX | MINUSMINUSPREFIX | LOGICALNOT | BITWISENOT | ADDRESSOF | DEREFERENCE
PostfixUnary -> PLUSPLUSSUFFIX | MINUSMINUSSUFFIX


*/
int accept(Symbol s) {
	if (sym == s) {
		nextsym();
		return 1;
	}
	return 0;
}

int expect(Symbol s) {
	if (accept(s)) {
		return 1;
	}
	else {
		error("expect: unexpected symbol");
		return 0;
	}
}

typedef enum {
	LEFTTORIGHT,
	RIGHTTOLEFT
} Associativity;

int getTokenPrecedence(token t) {
	if (t==SCOPERES) {
		return 1;
	}
	else if (t==PLUSPLUSSUFFIX ||t==MINUSMINUSSUFFIX || t==FUNCTIONSTYLETYPECAST || t==FUNCTIONCALL || t==ARRAYSUBSCRIPTING || t==ELEMENTSELETIONBYREF || t==ELEMENTSELECTIONBYPTR) {
		return 2;
	}
	else if (t==PLUSPLUSPREFIX||t==MINUSMINUSPREFIX||t==UNARYPLUS||t==UNARYMINUS||t==LOGICALNOT||t==BITWISENOT||t==CSTYLETYPECAST||t==DEREFERENCE||t==SIZEOF||t==ADDRESSOF||t==NEW||t==NEWARR||t==DELETE||t==DELETEARR) {
		return 3;
	}
	else if(t==POINTERTOMEMBER) {
		return 4;
	}
	else if (t==MULTIPLICATION||t==DIVISION||t==MODULUS) {
		return 5;
	}
	else if (t==ADDITION||t==SUBTRACTION) {
		return 6;	
	}
	else if (t==BITWISELEFTSHIFT||t==BITWISERIGHTSHIFT) {
		return 7;
	}
	else if (t==GTHAN||t==GTHANEQ||t==LTHAN||t==LTHANEQ) {
		return 8;
	}
	else if (t==EQUALTO||t==NOTEQUALTO) {
		return 9;
	}
	else if(t==BITWISEAND) {
		return 10;
	}
	else if(t==BITWISEXOR) {
		return 11;
	}
	else if (t==BITWISEOR) {
		return 12;
	}
	else if (t==LOGICALAND) {
		return 13;
	}
	else if (t==LOGICALOR) {
		return 14;
	}
	else if (t==TERNARYCONDITIONAL||t==THROWOP||t==ASSIGN||t==ASSIGNPLUS||t==ASSIGNMINUS||t==ASSIGNMULTIPLY||t==ASSIGNDIVIDE||t==ASSIGNMODULUS||t==ASSIGNBITWISELEFTSHIFT||t==ASSIGNBITWISERIGHTSHIFT||t==ASSIGNBITWISEAND||t==ASSIGNBITWISEXOR||t==ASSIGNBITWISEOR) {
		return 15;
	}
	else if (t==COMMA) {
		return 16;
	}
	else {
		ROSE_ASSERT(false);
	}
	ROSE_ASSERT(false);
}

Associativity getAssociativity(int tokenPrecendence) {
	switch tokenPrecedence {
		case 1:
			return LEFTTORIGHT;
		case 2:
			return LEFTOTRIGHT;
		case 3:
			return RIGHTTOLEFT;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14: return LEFTTORIGHT;
		case 15: return RIGHTTOLEFT;
		case 16: return LEFTTORIGHT;
		default: ROSE_ASSERT(false);	
	}
	ROSE_ASSERT(false);
}

typedef enum {
	NUMBER,
	SCOPERES,
	PLUSPLUSSUFFIX,
	MINUSMINUSSUFFIX,
	FUNCTIONSTYLETYPECAST, //type() type{}
	FUNCTIONCALL,
	ARRAYSUBSCRIPTING, // []
	ELEMENTSELECTIONBYREF, //.
	ELEMENTSELECTIONBYPTR, // ->
	PLUSPLUSPREFIX,
	MINUSMINUSPREFIX,
	UNARYPLUS,
	UNARYMINUS,
	LOGICALNOT,
	BITNOT, // ~
	CSTYLETYPECAST, // (type)
	DEREFERENCE,
	ADDRESSOF,
	SIZEOF,
	NEW,
	NEWARR, // new[]
	DELETE,
	DELETEARR, //delete[]
	POINTERTOMEMBER, // .*, ->*
	MULTIPLICATION,
	DIVISION,
	MODULUS, //%
	SUBTRACTION,
	BITWISELEFTSHIFT, // <<
	BITWISERIGHTSHIFT, // >>
	GTHAN, // >
	GTHANEQ, // >=
	LTHAN, // <
	LTHANEQ, // <=
	EQUALTO,
	NOTEQUALTO,
	BITWISEAND, //&
	BITWISEXOR, // ^
	BITWISEOR, // |
	LOGICALAND, // &&
	LOGICALOR, // ||
	TERNARYCOND, // ?:
	THROWOP, // throw
	ASSIGN, // =
	ASSIGNPLUS, // +=
	ASSIGNMINUS, // -=
	ASSIGNMULTIPLY, // *=
	ASSIGNDIVIDE, // /=
	ASSIGNMODULUS, // %=
	ASSIGNBITWISELEFTSHIFT, //<<=
	ASSIGNBITWISERIGHTSHIFT, //>>=
	ASSIGNBITWISEAND, //&=
	ASSIGNBITWISEXOR, //^=
	ASSIGNBITWISEOR, // |=
	COMMA,
	LEFTPAREN,
	RIGHTPAREN
} token;
	

		

token ReadToken();

void shuntingYardAlg(std::vector<token> tokens) {
	std::vector<token> reversed_tokens = tokens.reverse();
	while (reversed_tokens.size() != 0) {
	if (isNumber(t)) {
		outputStack.push(t);
	}
	else if (isFunctionCall(t)) {
		stack.push(t);
	}
	else if (isFunctionArgumentSeparator(t)) {
		while (!(t == RIGHTPAREN)) {
			token p = stack.pop();
			outputStack.push(p);
		}
	}
	else if (isOperator(t)) {
		while (isOperator(stack.top())) {
			if ((getAssociativity(getTokenPrecedence(t)) == LEFTTORIGHT && getTokenPrecedence(t) <= getTokenPrecedence(stack.top())) || (getAssociativity(getTokenPrecedence(t)) == RIGHTTOLEFT && getTokenPrecedence(t) < getTokenPrecedence(stack.top()))){
				token p = stack.pop();
				outputStack.push(p);
			}
			stack.push(t);
		}
	}
	else if (t==RIGHTPAREN) {
		stack.push(t);
	}
	else if (t==LEFTPAREN) {
		while (stack.top() != RIGHTPAREN) {
			token p = stack.pop();
			outputStack.push(p);
		}
		stack.pop();
		if (isFunction(stack.top())) {
			outputStack.push(stack.top());
			stack.pop();
			if stack.empty() {
				ROSE_ASSERT(false);
			}
		}
	}
	reversed_tokens.pop();
}
while (!stack.empty()) {
	
	token p = stack.pop();
	if (isOperator(p)) {
		outputStack.push(p);
	}
}

			
				  	

 



