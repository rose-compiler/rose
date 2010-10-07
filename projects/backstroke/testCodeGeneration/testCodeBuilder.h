#ifndef BACKSTROKE_TESTCODEBUILDER_H
#define	BACKSTROKE_TESTCODEBUILDER_H

#include <rose.h>
#include <boost/tuple/tuple.hpp>

class ExpressionBuilder
{
protected:
	std::vector<SgExpression*> results_;

public:
	virtual void build() = 0;

	std::vector<SgExpression*> getGeneratedExpressions() const
	{
		return results_;
	}

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
	virtual void build()
	{
		results_.push_back(SageBuilder::buildExprStatement(exp_));
	}
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


class ModelBuilder
{
	std::string name_;
	SgClassDeclaration* model_decl_;
	//std::vector<std::pair<std::string, SgType*> > members_;
	//std::vector<std::pair<std::string, SgExpression*> > members_;

	typedef boost::tuple<std::string, SgType*, SgExpression*> MemberType;
	std::vector<MemberType> members_;

public:
	ModelBuilder(const std::string& name)
	: name_(name), model_decl_(NULL) {}

	void addMember(const std::string& name, SgType* type)
	{ members_.push_back(MemberType(name, type, NULL)); }

	//! Given a name, get the corresponding arrow expression.
	SgExpression* getMemberExpression(const std::string& name) const;

	//! Given a type, get all corresponding members as expressions.
	std::vector<SgExpression*> getMemberExpression(SgType* type) const;

	//! Get the type of this model structure.
	SgType* getModelType() const
	{
		ROSE_ASSERT(model_decl_);
		return model_decl_->get_type();
	}


	//! This method builds declaration of the model structure, and all declarations of its members.
	//! At the same time, an arrow expression is build for each member so that we can use it to access
	//! this member in the event function.
	void build();
	
};



class EventFunctionBuilder
{
	std::string event_name_;
	std::vector<SgStatement*> stmts_;

	SgInitializedName* model_object_;
	ModelBuilder* model_builder_;

public:
	EventFunctionBuilder() {}

	//SgFunctionDeclaration* build();

	SgFunctionDeclaration* buildEventFunction(const std::string& event_name, const std::vector<SgStatement*>& stmts);
};



class TestCodeBuilder
{
	std::string file_name_;
	std::vector<SgFunctionDeclaration*> events_;

	SgClassDeclaration* model_;

public:
	TestCodeBuilder() {}

	void build()
	{

	}
};

class BasicExpressionTest : public TestCodeBuilder
{
public:
	void build()
	{
		ModelBuilder model_generator("Model");
	}
	
};


#endif	/* BACKSTROKE_TESTCODEBUILDER_H */

