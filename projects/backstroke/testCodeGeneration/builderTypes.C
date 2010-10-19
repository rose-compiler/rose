#include "builderTypes.h"
#include <utilities/utilities.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <stdlib.h>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

#define foreach BOOST_FOREACH

SgFunctionDeclaration* EventFunctionBuilder::buildEventFunction(bool is_cxx_style)
{
	// Build the parameter list.
	SgFunctionParameterList* para_list = buildFunctionParameterList();
	foreach (SgInitializedName* para, parameters_)
		para_list->append_arg(para);

	SgFunctionDeclaration* func_decl = NULL;
	if (is_cxx_style)
		func_decl = buildDefiningMemberFunctionDeclaration(event_name_, return_type_, para_list, scope_);
	else
		func_decl = buildDefiningFunctionDeclaration(event_name_, return_type_, para_list, scope_);

	SgBasicBlock* event_body = func_decl->get_definition()->get_body();
	ROSE_ASSERT(event_body);
	replaceStatement(event_body, event_body_);

	return func_decl;
}

#if 0
SgFunctionDeclaration* EventFunctionBuilder::buildEventFunction(const string& event_name, const vector<SgStatement*>& stmts)
{
	SgType* state_type = state_decl_->get_type();
	//if (state_obj_ == NULL)
	//state_object_ = buildInitializedName(state_obj_name_, buildPointerType(state_type_));

	SgFunctionParameterList* para_list = buildFunctionParameterList(state_object_);
	SgFunctionDeclaration* func_decl = buildDefiningFunctionDeclaration(event_name, buildVoidType(), para_list);

	SgBasicBlock* event_body = func_decl->get_definition()->get_body();
	ROSE_ASSERT(event_body);

	foreach (SgStatement* stmt, stmts)
	{
		appendStatement(event_body, stmt);
	}
	//buildBasicBlock());
	//fixVariableReferences(decl);
	return func_decl;
}
#endif


SgExpression* StateClassBuilder::getMemberExpression(const string& name) const
{
	foreach (const MemberType& member, members_)
	{
		if (member.get<0>() == name)
		{
			ROSE_ASSERT(member.get<2>());
			return member.get<2>();
		}
	}
	return NULL;
}

vector<SgExpression*> StateClassBuilder::getMemberExpression(SgType* type) const
{
	vector<SgExpression*> exps;
	foreach (const MemberType& member, members_)
	{
		// It seems that every type has only one object in the memory pool in ROSE.
		if (member.get<1>() == type)
		{
			ROSE_ASSERT(member.get<2>());
			exps.push_back(member.get<2>());
		}
	}
	return exps;
}

void StateClassBuilder::build()
{
	state_decl_ = buildStructDeclaration(name_);
	SgClassDefinition* state_def = buildClassDefinition(state_decl_);
	//SgInitializedName* state_init_name = buildInitializedName(name_, buildPointerType(state_decl_->get_type()));

	pushScopeStack(isSgScopeStatement(state_def));

	// Build declarations for all members.
	foreach (MemberType& member, members_)
	{
		SgVariableDeclaration* var_decl = buildVariableDeclaration(member.get<0>(), member.get<1>());
		state_def->append_member(var_decl);
		// Build an expression for each member like m->i.
		member.get<2>() = buildVarRefExp(var_decl);
		//member.get<2>() = buildBinaryExpression<SgArrowExp>(
		//		buildVarRefExp(state_init_name), buildVarRefExp(var_decl));
	}

	popScopeStack();
}

void TestCodeBuilder::buildStateClass()
{
	if (state_builder_)
		delete state_builder_;
	ROSE_ASSERT(!state_name_.empty());
	state_builder_ = new StateClassBuilder(state_name_);

	// Add all members.
	for (size_t i = 0; i < state_members_.size(); ++i)
		state_builder_->addMember(state_members_[i].first, state_members_[i].second);

	state_builder_->build();

	// In C style test code, we need the following initialized name to build the paramenter.
	if (!is_cxx_style_)
	{
		// Build the initialized name of the state object parameter in event functions.
		if (state_init_name_)
			delete state_init_name_;
		state_init_name_ =
			buildInitializedName("state", buildPointerType(state_builder_->getStateClassType()));
	}
}

SgExpression* TestCodeBuilder::buildStateMemberExpression(const string& name)
{
	ROSE_ASSERT(state_builder_);
	if (SgExpression* member_exp = state_builder_->getMemberExpression(name))
	{
		if (is_cxx_style_)
			return member_exp;
		else
		{
			ROSE_ASSERT(state_init_name_);
			return buildBinaryExpression<SgArrowExp>(buildVarRefExp(state_init_name_), member_exp);
		}
	}
	return NULL;
}

void TestCodeBuilder::buildTestCode(const vector<SgBasicBlock*>& bodies)
{
	ROSE_ASSERT(state_builder_);

	// Push global scope here to make sure every event funciton is built with a valid scope.
	//SgGlobal* global_scope = source_file_->get_globalScope();

	// Set the file name of the output code the same as input.
	ROSE_ASSERT(project_->get_fileList().size() == 1);
	SgSourceFile* source_file = isSgSourceFile((*project_)[0]);
	ROSE_ASSERT(source_file);
	source_file->set_unparse_output_filename(source_file->getFileName());

	SgGlobal* global_scope = source_file->get_globalScope();
	ROSE_ASSERT(global_scope);

	int counter = 0;
	foreach (SgBasicBlock* body, bodies)
	{
		string event_name = "event" + lexical_cast<string>(counter++);
		EventFunctionBuilder event_builder(event_name, body);
		if (is_cxx_style_)
		{
			event_builder.setScope(state_builder_->getStateClassDeclaration()->get_definition());
		}
		else
		{
			// Note that in C++ style, we don't add the following parameter.
			ROSE_ASSERT(state_init_name_);
			event_builder.addParameter(state_init_name_);
			event_builder.setScope(global_scope);
		}

		SgFunctionDeclaration* event_decl = event_builder.buildEventFunction(is_cxx_style_);
		// Add the new defined event function in the event collection.
		events_.push_back(event_decl);
	}

	// Add state declaration.
	SgClassDeclaration* state_decl = state_builder_->getStateClassDeclaration();
	appendStatement(state_decl, global_scope);

	// Add events.
	foreach (SgFunctionDeclaration* event, events_)
	{
		if (is_cxx_style_)
		{
			//SgMemberFunctionDeclaration* decl = isSgMemberFunctionDeclaration(event);
			//ROSE_ASSERT(decl);
			//decl->set_associatedClassDeclaration(state_decl);
			appendStatement(event, state_decl->get_definition());
			//state_decl->get_definition()->append_member(event);
			//event->set_parent(state_decl);
		}
		else
			appendStatement(event, global_scope);
	}

	// Fix variable references here because of bottom up build.
	fixVariableReferences(global_scope);
}

SgFunctionDeclaration* TestCodeAssembler::buildInitializationFunction()
{
	ROSE_ASSERT(state_class_);
	
	// Build the parameter list.
	SgInitializedName* state_para =	buildInitializedName("state", buildPointerType(state_class_->get_type()));
	SgFunctionParameterList* para_list = buildFunctionParameterList(state_para);
	SgFunctionDeclaration* init_func = buildDefiningFunctionDeclaration(
			"initialize", buildVoidType(), para_list, getScope(state_class_));

	SgExpression* state_var = buildVarRefExp(state_para);

	//SgBasicBlock* body = buildBasicBlock();
	SgBasicBlock* body = init_func->get_definition()->get_body();
	
	foreach (SgDeclarationStatement* decl, state_class_->get_definition()->get_members())
	{
		if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl))
		{
			foreach (SgInitializedName* init_name, var_decl->get_variables())
			{
				SgVarRefExp* member_var = buildVarRefExp(init_name);
				SgExpression* var = buildBinaryExpression<SgArrowExp>(state_var, member_var);

				// Push the current scope to make sure that buildFunctionCallExp has a valid scope.
				pushScopeStack(body);
				appendStatement(initializeMember(var));
				popScopeStack();
			}
		}
	}

	//SgBasicBlock* prev_body = init_func->get_definition()->get_body();
	//ROSE_ASSERT(prev_body);
	//replaceStatement(prev_body, body);

	return init_func;
}

SgFunctionDeclaration* TestCodeAssembler::buildComparisonFunction()
{
	ROSE_ASSERT(state_class_);

	// Build the parameter list.
	SgInitializedName* state_para1 =	buildInitializedName("state1", buildPointerType(state_class_->get_type()));
	SgInitializedName* state_para2 =	buildInitializedName("state2", buildPointerType(state_class_->get_type()));
	SgFunctionParameterList* para_list = buildFunctionParameterList(state_para1, state_para2);
	SgFunctionDeclaration* comp_func = buildDefiningFunctionDeclaration(
			"compare", buildBoolType(), para_list, getScope(state_class_));

	SgExpression* state_var1 = buildVarRefExp(state_para1);
	SgExpression* state_var2 = buildVarRefExp(state_para2);

	SgBasicBlock* body = comp_func->get_definition()->get_body();

	foreach (SgDeclarationStatement* decl, state_class_->get_definition()->get_members())
	{
		if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl))
		{
			foreach (SgInitializedName* init_name, var_decl->get_variables())
			{
				SgVarRefExp* member_var = buildVarRefExp(init_name);
				SgExpression* var1 = buildBinaryExpression<SgArrowExp>(state_var1, member_var);
				SgExpression* var2 = buildBinaryExpression<SgArrowExp>(state_var2, member_var);

				// Push the current scope to make sure that buildFunctionCallExp has a valid scope.
				pushScopeStack(body);
				appendStatement(compareValue(var1, var2));
				popScopeStack();
			}
		}
	}

	// At the end of this function, return true.
	appendStatement(buildReturnStmt(buildBoolValExp(true)), body);
	return comp_func;
}

SgStatement* TestCodeAssembler::compareValue(SgExpression* var1, SgExpression* var2)
{
	ROSE_ASSERT(var1->get_type() == var2->get_type());
	
    SgType* type = var1->get_type()->stripTypedefsAndModifiers();
	
    if (isSgArrayType(type))
    {
		// This part should be modified.
		ROSE_ASSERT(false);

        SgExprListExp* memcmp_para = buildExprListExp(
                var1, var2,
                buildSizeOfOp(var1));
        SgStatement* cmp = buildFunctionCallStmt("memcmp", buildPointerType(buildVoidType()), memcmp_para);
        return buildIfStmt(cmp, buildReturnStmt(buildIntVal(0)), NULL);
    }
	// For a STL object, we use defined operator !=.
	else if (BackstrokeUtility::isSTLContainer(type))
	{
		// This part should be added.
		ROSE_ASSERT(false);
	}
	// For basic types, just use operator !=.
    else
    {
        SgNotEqualOp* compare_exp = buildBinaryExpression<SgNotEqualOp>(var1, var2);
        return buildIfStmt(compare_exp, buildReturnStmt(buildBoolValExp(false)), NULL);
    }
    return NULL;
}

SgStatement* TestCodeAssembler::initializeMember(SgExpression* exp)
{
	SgType* type = exp->get_type()->stripTypedefsAndModifiers();

	// An array uses a loop to initialized its members.
	if (isSgArrayType(type))
	{
		// Initialize int array member
		SgExprListExp* memset_para = buildExprListExp(
				copyExpression(exp), buildIntVal(0xFFFF), buildSizeOfOp(exp));
		return buildFunctionCallStmt("memset", buildPointerType(buildVoidType()), memset_para);
	}
	// For a STL object, currently we do not initialize it.
	else if (BackstrokeUtility::isSTLContainer(type))
	{
		return NULL;
	}
	// For a class object, we initialize all its members, if we can access them.
	else if (SgClassType* class_t = isSgClassType(type))
	{
		SgBasicBlock* block = buildBasicBlock();
		SgClassDeclaration* class_decl = isSgClassDeclaration(class_t->get_declaration()->get_definingDeclaration());
		
		foreach (SgDeclarationStatement* decl, class_decl->get_definition()->get_members())
		{
			if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl))
			{
				foreach (SgInitializedName* var, var_decl->get_variables())
				{
					SgVarRefExp* member_var = buildVarRefExp(var);
					SgExpression* var = buildBinaryExpression<SgDotExp>(exp, member_var);

					// Push the current scope to make sure that buildFunctionCallExp has a valid scope.
					pushScopeStack(block);
					appendStatement(initializeMember(var), block);
					popScopeStack();
				}
			}
		}
		return block;
	}
	// For a basic type, we initialize it with a random value now.
	else
	{
		SgExpression* rand_exp = buildFunctionCallExp("rand", buildIntType());
		SgAssignOp* init = buildBinaryExpression<SgAssignOp>(exp, rand_exp);
		return buildExprStatement(init);
	}
}

void TestCodeAssembler::assemble()
{
	// First, get the global scope.
	SgScopeStatement* global_scope = getScope(state_class_);
	ROSE_ASSERT(isSgGlobal(global_scope));

	// Build the initialization function.
	appendStatement(buildInitializationFunction(), global_scope);
	appendStatement(buildComparisonFunction(), global_scope);

	// Since we use rand() function which needs the following header.
	insertHeader("stdlib.h", PreprocessingInfo::after, true, global_scope);
}