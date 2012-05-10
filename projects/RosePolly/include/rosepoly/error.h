

#ifndef ERROR_H
#define ERROR_H

typedef enum {
	DATA,
	GRAPH,
	ACCESSPAT,
	EVAL
} frontStage;

void front_error( SgNode * aNode, frontStage stage, const string message, bool rec );

void report_error( const string message );

void report_error( const string message , SgNode * aNode );

void simple_error( string message );

#endif

