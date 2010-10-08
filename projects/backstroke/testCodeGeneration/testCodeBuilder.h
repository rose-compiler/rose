#ifndef BACKSTROKE_TESTCODEBUILDER_H
#define	BACKSTROKE_TESTCODEBUILDER_H

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


class ExpressionBuilderPool : public ExpressionBuilder
{
	std::vector<ExpressionBuilderPtr> exp_builders_;
	
public:
	void addExpressionBuilder(ExpressionBuilder* builder)
	{ addExpressionBuilder(ExpressionBuilderPtr(builder)); }

	void addExpressionBuilder(ExpressionBuilderPtr builder)
	{ exp_builders_.push_back(builder); }

	virtual void build();
};

class UnaryExpressionBuilder : public ExpressionBuilder
{
protected:
	SgExpression* operand_;

public:

	UnaryExpressionBuilder(SgExpression* operand)
	: operand_(operand)
	{
	}

	virtual void build();
};

class BinaryExpressionBuilder : public ExpressionBuilder
{
protected:
	SgExpression* lhs_operand_;
	SgExpression* rhs_operand_;

public:

	BinaryExpressionBuilder(SgExpression* lhs, SgExpression* rhs)
	: lhs_operand_(lhs), rhs_operand_(rhs)
	{
	}

	virtual void build();
};

class ConditionalExpressionBuilder
{
};

class FunctionCallExpressionBuilder
{
};

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

class ExpressionStatementBuilder : public StatementBuilder
{
protected:
	SgExpression* exp_;

public:
	virtual void build();
};

class IfStatementBuilder
{
};

class ForStatementBuilder
{
};

class WhileStatementBuilder
{
};

class DoWhileStatementBuilder
{
};

class SwitchStatementBuilder
{
};


class StateClassBuilder
{
protected:
	std::string name_;
	std::string obj_name_;
	SgClassDeclaration* state_decl_;
	//std::vector<std::pair<std::string, SgType*> > members_;
	//std::vector<std::pair<std::string, SgExpression*> > members_;

	typedef boost::tuple<std::string, SgType*, SgExpression*> MemberType;
	std::vector<MemberType> members_;

public:
	StateClassBuilder(const std::string& name)
	:name_(name), 
	obj_name_(name + "_obj"),
	state_decl_(NULL)
	{}

	void addMember(const std::string& name, SgType* type)
	{ members_.push_back(MemberType(name, type, NULL)); }

	//! Given a name, get the corresponding arrow expression.
	SgExpression* getMemberExpression(const std::string& name) const;

	//! Given a type, get all corresponding members as expressions.
	std::vector<SgExpression*> getMemberExpression(SgType* type) const;

	//! Get the type of this state structure.
	SgType* getStateClassType() const
	{
		ROSE_ASSERT(state_decl_);
		return state_decl_->get_type();
	}

	SgClassDeclaration* getStateClassDeclaration() const
	{ return state_decl_; }

	std::string getStateObjectName() const
	{ return obj_name_; }


	//! This method builds declaration of the state class, and all declarations of its members.
	//! At the same time, an arrow expression is build for each member so that we can use it to access
	//! this member in the event function.
	void build();
	
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

	//! Return type.
	SgType* return_type_;

public:
	EventFunctionBuilder(const std::string& name, SgBasicBlock* body = NULL)
	: event_name_(name),
	//state_name_("m"),
	event_body_(body),
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

	//! Build the event function declaration.
	SgFunctionDeclaration* buildEventFunction();

	//SgFunctionDeclaration* buildEventFunction(const std::string& event_name, const std::vector<SgStatement*>& stmts);
};



//! This class is used to build a source file as test code including declarations of state class and events.
//! Since it's an abstract class, the concrete test code is built using a concrete class inheriting this class.
class TestCodeBuilder
{
protected:
	//! The file name of test code.
	std::string file_name_;

	//! A SgSourceFile object which creates the final output.
	SgSourceFile* source_file_;

	//! All declarations of events.
	std::vector<SgFunctionDeclaration*> events_;

	//SgClassDeclaration* state_;
	
	//! A state class builder to build the state class.
	StateClassBuilder* state_builder_;

	//! The initialized name of the state object parameter in event functions.
	SgInitializedName* state_init_name_;

	virtual void build_() = 0;

	void setStateClassName(const std::string& name)
	{
		if (state_builder_)
			delete state_builder_;
		state_builder_ = new StateClassBuilder(name);
	}

	void addStateMember(const std::string& name, SgType* type)
	{
		ROSE_ASSERT(state_builder_);
		state_builder_->addMember(name, type);
	}

	void buildStateClass();

	SgExpression* buildStateMemberExpression(const std::string& name);

	void buildTestCode(const std::vector<SgBasicBlock*> bodies);

public:
	TestCodeBuilder(const std::string& filename)
	: file_name_(filename),
	source_file_(NULL),
	state_builder_(NULL),
	state_init_name_(NULL)
	{}

	~TestCodeBuilder()
	{
		delete state_builder_;
		delete state_init_name_;
	}

	void build();
};

class BasicExpressionTest : public TestCodeBuilder
{
protected:
	virtual void build_();
public:
	BasicExpressionTest(const std::string& filename)
	: TestCodeBuilder(filename) {}
};


#endif	/* BACKSTROKE_TESTCODEBUILDER_H */

