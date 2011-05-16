
#ifndef _TDBU_TRAVERSAL_HPP_
#define _TDBU_TRAVERSAL_HPP_

#include "maths/PPLUtils.hpp"
#include "rose.h"

#include <set>
#include <vector>

enum constraint_type_e {
	equality,
	greater_equal,
	lesser_equal
};

struct Symbol;
class SymbolTable;
class PolyhedralElement;

class DataAccess {
	public:
		enum AccessType {
			read,
			write
		};

	public:
		PolyhedralElement * associated_element;
	
		AccessType access_type;
		Symbol * symbol;

		Polyhedron accessed_domain;
		
		unsigned int dimension;

	public:
		DataAccess(PolyhedralElement *, AccessType, Symbol *);
		
		void restrainAccessedDomain(LinearExpression &, unsigned int, constraint_type_e = equality);
		
		void addAccessDimension();
		unsigned int getAccessDimension() const;
};

class PolyhedralPredicate;
class PolyhedralControl;

class PolyhedralElement {
	protected:
		PolyhedralControl * parent; // Not use but we never know...
		std::set<PolyhedralPredicate *> predicate_set;
		std::map<Symbol *, VariableID> symbol_id_map;
	
	public:
		Polyhedron domain;
		
	public:
		PolyhedralElement(PolyhedralControl *);
		virtual ~PolyhedralElement();
		
		virtual void aggregate(PolyhedralElement *) = 0;
		
		virtual void print(std::ostream &, std::string = std::string()) = 0;
		
		VariableID getID(Symbol *);
};

class PolyhedralStatement : public PolyhedralElement {
	public:
		PolyhedralStatement(SgStatement *);
		virtual ~PolyhedralStatement();

		virtual void aggregate(PolyhedralElement *); // ERROR
		
		virtual void print(std::ostream &, std::string = std::string());
		
	public:
		SgStatement * statement;
};

class PolyhedralPredicate : public PolyhedralStatement {
	// TODO
};

class PolyhedralControl : public PolyhedralElement {
	public:
		PolyhedralControl(SgNode * = NULL, SymbolTable * = NULL);
		virtual ~PolyhedralControl();
	
		PolyhedralControl * genNext(SgNode *) const;

		virtual void aggregate(PolyhedralElement *);
		
		virtual void print(std::ostream &, std::string = std::string());
	
	public:
		const PolyhedralControl * parent;
	
		SgNode * associated_node;
		
		SymbolTable * symbol_table;

		// empty during top-down, filled by bottom-up needed for scattering generation
		std::vector<PolyhedralElement *> elements_set;
};

/**/

enum SymbolRelation {
// for a field
	arrow,
	dot,
// to be use in inter-procedural analysis for parameter and return value
	value, // read
	reference, // read/write
	dereference, // read/write
// for pointers dereference and addresse_of operator
	ptr_deref,
	addr_of,
// default
	nothing
};

struct Symbol {
	Symbol(unsigned int);

	SgSymbol * sage_symbol;
	
	Symbol * parent_symbol;
	SymbolRelation relation_with_parent;

	SgScopeStatement * scope; // NULL means the whole "program"
	
	bool generated; // <=> sage_symbol == NULL && parent_symbol == NULL
	
	bool iterator; // Only for iterator pattern in FOR loop (or generated).
	bool predicate; // is a data
	bool data; // saw as array or lvalue in an expression statement (or generated) or not declared as scalar
	
	bool function_parameter; // special case for function parameter
	
	unsigned int dimension; // for array only
	
	unsigned int id;
	
	// Needed to update accessed domain when a new array dimension is found
	std::vector<DataAccess *> associated_access;
	
	void print(std::ostream &, std::string = std::string());
};

class SymbolTable {
	public:
		SymbolTable * parent_table;

	protected:
		unsigned int nb_child;
		
		std::set<Symbol *> symbol_set;
		
		// Needed to update accessed domain when a new symbol is declared
		std::set<DataAccess *> associated_access;
		std::set<PolyhedralControl *> associated_control;
		
//		FunctionTable * function_table; // TODO

		Symbol * makeSymbol();

		Symbol * genSymbol(SgExpression *, bool = true);
		
		Symbol * symbolFor(SgSymbol *, bool, bool);
		
		bool haveSymbol(Symbol *) const;

	public:
//		SymbolTable(FunctionTable *); // TODO => SymbolTable(SymbolTable * != NULL); (assert)
		SymbolTable(SymbolTable *);
		~SymbolTable(); // assert if nb_child > 0 && parent->nb_child--
		
		Symbol * genSymbol(SgInitializedName *);
		
		Symbol * genSymbol(SgVarRefExp *, bool = true);
		
		Symbol * genSymbol(SgArrowExp *, bool = true);
		Symbol * genSymbol(SgDotExp *, bool = true);
		Symbol * genSymbol(SgPntrArrRefExp *, bool = true);
		Symbol * genSymbol(SgPointerDerefExp *, bool = true);
		Symbol * genSymbol(SgAddressOfOp *, bool = true);
		
		Symbol * genIteratorSymbol(SgScopeStatement *);
		
		bool isSymbol(SgExpression *, Symbol *);
		
		unsigned int nbrReferencedVariables() const;
		
		VariableID getID(Symbol *) const;
		
		void addControl(PolyhedralControl *);
		void addDataAccess(DataAccess *);
		
		void print(std::ostream &, std::string = std::string());

		// SymbolTable to be delete at the same time than this, it is symbol table inherited from statement not relevant for scattering (SgBasicBlock ...)
		std::set<SymbolTable *> embedded_tables;
};

/**/

class PolyhedralModelisation {
	protected:
		// FunctionTable *
	
	public:
		PolyhedralModelisation();
		
		void traverse(SgNode *);
		
	protected:	
		PolyhedralElement * evaluate(SgNode *, PolyhedralControl *, std::string = std::string());
};

#endif /* _TDBU_TRAVERSAL_HPP_ */
