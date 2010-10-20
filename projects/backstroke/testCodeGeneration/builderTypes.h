#ifndef BUILDERTYPES_H
#define	BUILDERTYPES_H

#include <backstroke.h>
#include <rose.h>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>

class ExpressionBuilder
{
protected:
	std::vector<SgExpression*> results_;

public:
	virtual ~ExpressionBuilder() {}

	virtual void build() = 0;

	std::vector<SgExpression*> getGeneratedExpressions() const
	{
		return results_;
	}

};
typedef boost::shared_ptr<ExpressionBuilder> ExpressionBuilderPtr;



class StatementBuilder
{
protected:
	std::vector<SgStatement*> results_;

public:
	virtual void build() = 0;

	std::vector<SgStatement*> getGeneratedStatements() const
	{
		return results_;
	}
};
typedef boost::shared_ptr<StatementBuilder> StatementBuilderPtr;




//! This class is used to build the declaration and definition of the state class.
class StateClassBuilder
{
protected:
	//! The name of the state class.
	std::string name_;

	//std::string obj_name_;

	//! The declaration of the state class.
	SgClassDeclaration* state_decl_;
	//std::vector<std::pair<std::string, SgType*> > members_;
	//std::vector<std::pair<std::string, SgExpression*> > members_;

	typedef boost::tuple<std::string, SgType*, SgExpression*> MemberType;

	//! All data members of the state class.
	std::vector<MemberType> members_;

public:
	StateClassBuilder(const std::string& name)
	:	name_(name),
		//obj_name_(name + "_obj"),
		state_decl_(NULL)
	{}

	//! Add a data member to the state class.
	void addMember(const std::string& name, SgType* type)
	{ members_.push_back(MemberType(name, type, NULL)); }

	//! Given a name, get the corresponding arrow expression.
	SgExpression* getMemberExpression(const std::string& name) const;

	//! Given a type, get all corresponding members as expressions.
	std::vector<SgExpression*> getMemberExpression(SgType* type) const;

	//! Get the type of this state structure. Note that the class must be built.
	SgType* getStateClassType() const
	{
		ROSE_ASSERT(state_decl_);
		return state_decl_->get_type();
	}

	//! Get the declaration of the state class.
	SgClassDeclaration* getStateClassDeclaration() const
	{ return state_decl_; }

	//std::string getStateObjectName() const
	//{ return obj_name_; }


	//! This method builds declaration of the state class, and all declarations of its members.
	//! At the same time, an arrow expression is build for each member so that we can use it to access
	//! this member in the event function.
	void build();

	//void buildInitializationFunction()

};

class SimpleStateClass
{
	StateClassBuilder* state_builder_;
public:
	SimpleStateClass()
	{
	}

	SgClassDeclaration* build()
	{
		return NULL;
	}

};


//! This class is used to build event functions.
//! This event function builder can build both C and C++ style events (ROSS vs SPEEDES).
class EventFunctionBuilder
{
	//! The function name of the event.
	std::string event_name_;

	////! The state object parameter's name.
	//std::string state_name_;

	//! The function body of the event.
	SgBasicBlock* event_body_;

	////! All statements inside of the function body.
	//std::vector<SgStatement*> stmts_;

	//SgInitializedName* state_object_;

	////! A state class builder to build the state class.
	//StateClassBuilder* state_builder_;

	//! All parameters.
	std::vector<SgInitializedName*> parameters_;

	//! The scope the built funtion belongs to.
	SgScopeStatement* scope_;

	//! Return type.
	SgType* return_type_;

public:
	EventFunctionBuilder(const std::string& name, SgBasicBlock* body = NULL)
	:	event_name_(name),
		//state_name_("m"),
		event_body_(body),
		scope_(NULL),
		return_type_(SageBuilder::buildVoidType())
	{}

	//! Given a member in state class, return its real accessing variable in function body.
	SgExpression* getStateVariable(SgExpression*) const;

	//! Set the event body.
	void setEventBody(SgBasicBlock* body)
	{ event_body_ = body; }

	//! Add a parameter to the event function.
	void addParameter(SgType* type, const std::string& name)
	{ parameters_.push_back(SageBuilder::buildInitializedName(name, type)); }

	//! Add a parameter to the event function.
	void addParameter(SgInitializedName* para)
	{ parameters_.push_back(para); }

	//! Set the return type of the event function.
	void setReturnType(SgType* type)
	{ return_type_ = type; }

	//! Get the return type of the event function.
	SgType* getReturnType() const
	{ return return_type_; }

	//! Set the scope to put event functions.
	void setScope(SgScopeStatement* scope)
	{ scope_ = scope; }

	//! Build the event function declaration.
	SgFunctionDeclaration* buildEventFunction(bool is_cxx_style);

	//SgFunctionDeclaration* buildEventFunction(const std::string& event_name, const std::vector<SgStatement*>& stmts);
};



//! This class is used to build a source file as test code including declarations of state class and events.
//! Since it's an abstract class, the concrete test code is built using a concrete class inheriting this class.
class TestCodeBuilder
{
protected:
	//! Build test code as C++ style. That is, event functions are members of state class.
	bool is_cxx_style_;

	//! The file name of test code.
	std::string file_name_;

	////! A SgSourceFile object which creates the final output.
	//SgSourceFile* source_file_;

	//! The ROSE project.
	SgProject* project_;

	//! All declarations of events.
	std::vector<SgFunctionDeclaration*> events_;

	//! A state class builder to build the state class.
	StateClassBuilder* state_builder_;

	//! The initialized name of the state object parameter in event functions.
	SgInitializedName* state_init_name_;

	//! The name of the state class.
	std::string state_name_;

	//! All members in the state class.
	std::vector<std::pair<std::string, SgType*> > state_members_;

protected:
	//! Set the state class's name.
	void setStateClassName(const std::string& name)
	{ state_name_ = name; }

	//! Add a data member to the state class.
	void addStateMember(const std::string& name, SgType* type)
	{ state_members_.push_back(std::make_pair(name, type)); }

	//! Remove all added state members.
	void clearStateMembers()
	{ state_members_.clear(); }

	//! Build the state class.
	void buildStateClass();

	//! Get all names of state members.
	std::vector<SgExpression*> getAllStateMemberVars() const;

	//! Given a name, search the state class for its data members. If found, return a VarRefExp or arrow
	//! expression based on the flag is_cxx_style_.
	SgExpression* buildStateMemberExpression(const std::string& name) const;

	//! Build the final test source file.
	void buildTestCode(const std::vector<SgBasicBlock*>& bodies);

public:
	TestCodeBuilder(SgProject* project, bool is_cxx_style)
	:	is_cxx_style_(is_cxx_style),
		project_(project),
		state_builder_(NULL),
		state_init_name_(NULL)
	{}

	virtual ~TestCodeBuilder()
	{
		delete state_builder_;
		delete state_init_name_;
	}

	//! Once all is set, call this funtion to create the test source file.
	//! Note this is a pure virtual function which needs to be overridden.
	virtual void build() = 0;

	std::vector<SgFunctionDeclaration*> getAllEvents() const
	{ return events_; }

	//! Get the declaration of the state class.
	SgClassDeclaration* getStateClassDeclaration()
	{ return state_builder_->getStateClassDeclaration(); }
};



class TestCodeAssembler
{
	bool is_cxx_style_;

	SgClassDeclaration* state_class_;

	std::map<SgFunctionDeclaration*, FuncDeclPairs> events_;

	std::string init_func_name_;

	std::string comp_func_name_;

public:
	TestCodeAssembler(SgClassDeclaration* state_decl,
			const std::map<SgFunctionDeclaration*, FuncDeclPairs>& events)
	: state_class_(isSgClassDeclaration(state_decl->get_definingDeclaration())),
	events_(events),
	init_func_name_("initialize"),
	comp_func_name_("compare")
	{
		ROSE_ASSERT(state_class_);
	}

	//void setStateClass(SgClassDeclaration* state_decl)
	//{ state_class_ = state_decl; }

	SgFunctionDeclaration* buildInitializationFunction();

	SgFunctionDeclaration* buildComparisonFunction();

	SgFunctionDeclaration* buildMainFunction();

	SgStatement* initializeMember(SgExpression* exp);

	SgStatement* compareValue(SgExpression* var1, SgExpression* var2);

	void assemble();
};

#endif	/* BUILDERTYPES_H */

