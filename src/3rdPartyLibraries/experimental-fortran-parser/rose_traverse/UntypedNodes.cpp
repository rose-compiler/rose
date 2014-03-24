#include "UntypedNodes.h"
#include <assert.h>

SgUntypedType* isSgUntypedType(SgNode* node)
{
   return dynamic_cast<SgUntypedType*>(node);   
}

SgUntypedUnaryOperator* isSgUntypedUnaryOperator(SgNode* node)
{
   return dynamic_cast<SgUntypedUnaryOperator*>(node);   
}

SgUntypedProgramHeaderDeclaration* isSgUntypedProgramHeaderDeclaration(SgNode* node)
{
   return dynamic_cast<SgUntypedProgramHeaderDeclaration*>(node);   
}

SgUntypedDeclarationStatement* isSgUntypedDeclarationStatement(SgNode* node)
{
   return dynamic_cast<SgUntypedDeclarationStatement*>(node);   
}

SgUntypedStatement* isSgUntypedStatement(SgNode* node)
{
   return dynamic_cast<SgUntypedStatement*>(node);   
}


Sg_File_Info* SgLocatedNode::get_startOfConstruct() const {return p_startOfConstruct;}
//void SgLocatedNode::set_startOfConstruct(Sg_File_Info* startOfConstruct);

SgLocatedNode::~SgLocatedNode() {}
SgLocatedNode::SgLocatedNode(Sg_File_Info* start) : p_startOfConstruct(start) {}


SgLocatedNodeSupport::SgLocatedNodeSupport(Sg_File_Info* start) : SgLocatedNode(start) {}


SgUntypedNode::~SgUntypedNode() {}
SgUntypedNode::SgUntypedNode(Sg_File_Info* start) : SgLocatedNodeSupport(start) {}

VariantT SgUntypedNode::variantT() const {return V_SgUntypedNode;}


std::string SgUntypedStatement::get_label_string() const             {return p_label_string;}
void SgUntypedStatement::set_label_string(std::string label_string)  {p_label_string = label_string;}

SgToken::ROSE_Fortran_Keywords SgUntypedStatement::get_statement_enum() const         {return p_statement_enum;}
void SgUntypedStatement::set_statement_enum(SgToken::ROSE_Fortran_Keywords stmt_enum) {p_statement_enum = stmt_enum;}

SgUntypedStatement::~SgUntypedStatement() {}
SgUntypedStatement::SgUntypedStatement(Sg_File_Info* start) : SgUntypedNode(start) {}


//========================================================================================
// SgUntypedExpression
//----------------------------------------------------------------------------------------
SgUntypedExpression::SgUntypedExpression(Sg_File_Info* start, SgToken::ROSE_Fortran_Keywords stmt_enum)
   : SgUntypedNode(start), p_statement_enum(stmt_enum)
   {
   }
SgUntypedExpression::~SgUntypedExpression() {}

VariantT SgUntypedExpression::variantT() const {return V_SgUntypedExpression;}

SgToken::ROSE_Fortran_Keywords SgUntypedExpression::get_statement_enum() const         {return p_statement_enum;}
void SgUntypedExpression::set_statement_enum(SgToken::ROSE_Fortran_Keywords stmt_enum) {p_statement_enum = stmt_enum;}


//========================================================================================
// SgUntypedReferenceExpression
//----------------------------------------------------------------------------------------
SgUntypedReferenceExpression::SgUntypedReferenceExpression(Sg_File_Info* start,
                                                           SgToken::ROSE_Fortran_Keywords stmt_enum, std::string name)
   : SgUntypedExpression(start, stmt_enum), p_name(name)
   {
   }
SgUntypedReferenceExpression::~SgUntypedReferenceExpression() {}

VariantT SgUntypedReferenceExpression::variantT() const {return V_SgUntypedReferenceExpression;}

std::string SgUntypedReferenceExpression::get_name() const             {return p_name;}
void        SgUntypedReferenceExpression::set_name(std::string name)   {p_name = name;}


//========================================================================================
// SgUntypedValueExpression
//----------------------------------------------------------------------------------------
SgUntypedValueExpression::SgUntypedValueExpression(Sg_File_Info* start, SgToken::ROSE_Fortran_Keywords stmt_enum,
                                                   std::string value_string, SgUntypedType* type)
   : SgUntypedExpression(start, stmt_enum), p_value_string(value_string), p_type(type)
   {
   }
SgUntypedValueExpression::~SgUntypedValueExpression() {}

VariantT SgUntypedValueExpression::variantT() const {return V_SgUntypedValueExpression;}

std::string SgUntypedValueExpression::get_value_string() const            {return p_value_string;}
void SgUntypedValueExpression::set_value_string(std::string value_string) {p_value_string = value_string;}

SgUntypedType* SgUntypedValueExpression::get_type() const    {return p_type;}
void SgUntypedValueExpression::set_type(SgUntypedType* type) {p_type = type;}


//========================================================================================
// SgUntypedType
//----------------------------------------------------------------------------------------
SgUntypedType::SgUntypedType(Sg_File_Info* start, std::string type_name)
   : SgUntypedNode(start), p_type_name(type_name)
   {
   }
SgUntypedType::~SgUntypedType() {}

std::string SgUntypedType::get_type_name() const {return p_type_name;}
//void SgUntypedType::set_type_name(std::string type_name);

SgUntypedExpression* SgUntypedType::get_type_kind() const  {return p_type_kind;}
void SgUntypedType::set_type_kind(SgUntypedExpression* type_kind)
   {
      p_has_kind = true;
      p_type_kind = type_kind;
   }

bool SgUntypedType::get_has_kind() const            {return p_has_kind;}
void SgUntypedType::set_has_kind(bool has_kind)     {p_has_kind = has_kind;}

//bool SgUntypedType::get_is_literal() const;
void SgUntypedType::set_is_literal(bool is_literal) {p_is_literal = is_literal;}

//bool SgUntypedType::get_is_class() const;
//void SgUntypedType::set_is_class(bool is_class);

//bool SgUntypedType::get_is_intrinsic() const;
//void SgUntypedType::set_is_intrinsic(bool is_intrinsic);

//bool SgUntypedType::get_is_constant() const;
void SgUntypedType::set_is_constant(bool is_constant) {p_is_constant = is_constant;}

//bool SgUntypedType::get_is_user_defined() const;
//void SgUntypedType::set_is_user_defined(bool is_user_defined);

//SgUntypedExpression* SgUntypedType::get_char_length_expression() const;
//void SgUntypedType::set_char_length_expression(SgUntypedExpression* char_length_expression);

//std::string get_char_length_string() const;
//void set_char_length_string(std::string char_length_string);

//bool SgUntypedType::get_char_length_is_string() const;
//void SgUntypedType::set_char_length_is_string(bool char_length_is_string);


//========================================================================================
// SgUntypedAssignmentStatement
//----------------------------------------------------------------------------------------
SgUntypedAssignmentStatement::SgUntypedAssignmentStatement(Sg_File_Info* start, SgUntypedExpression* lhs, SgUntypedExpression* rhs)
   : SgUntypedStatement(start), p_lhs_operand(lhs), p_rhs_operand(rhs)
   {
   }
SgUntypedAssignmentStatement::~SgUntypedAssignmentStatement() {}

VariantT SgUntypedAssignmentStatement::variantT() const {return V_SgUntypedAssignmentStatement;}

SgUntypedExpression* SgUntypedAssignmentStatement::get_lhs_operand() const           {return p_lhs_operand;}
void SgUntypedAssignmentStatement::set_lhs_operand(SgUntypedExpression* lhs_operand) {p_lhs_operand = lhs_operand;}

SgUntypedExpression* SgUntypedAssignmentStatement::get_rhs_operand() const           {return p_rhs_operand;}
void SgUntypedAssignmentStatement::set_rhs_operand(SgUntypedExpression* rhs_operand) {p_rhs_operand = rhs_operand;}


//========================================================================================
// SgUntypedBinaryOperator
//----------------------------------------------------------------------------------------
SgUntypedBinaryOperator::SgUntypedBinaryOperator(Sg_File_Info* start, SgToken::ROSE_Fortran_Keywords stmt_enum,
                                                 SgToken::ROSE_Fortran_Operators operator_enum, std::string name,
                                                 SgUntypedExpression* lhs, SgUntypedExpression* rhs)
   : SgUntypedExpression(start, stmt_enum), p_operator_enum(operator_enum),
     p_operator_name(name), p_lhs_operand(lhs), p_rhs_operand(rhs)
   {
   }
SgUntypedBinaryOperator::~SgUntypedBinaryOperator() {}

VariantT SgUntypedBinaryOperator::variantT() const {return V_SgUntypedBinaryOperator;}

SgToken::ROSE_Fortran_Operators SgUntypedBinaryOperator::get_operator_enum() const             {return p_operator_enum;}
void SgUntypedBinaryOperator::set_operator_enum(SgToken::ROSE_Fortran_Operators operator_enum) {p_operator_enum = operator_enum;}

std::string SgUntypedBinaryOperator::get_operator_name() const                   {return p_operator_name;}
void SgUntypedBinaryOperator::set_operator_name(std::string operator_name)       {p_operator_name = operator_name;}

SgUntypedExpression* SgUntypedBinaryOperator::get_lhs_operand() const            {return p_lhs_operand;}
void SgUntypedBinaryOperator::set_lhs_operand(SgUntypedExpression* lhs_operand)  {p_lhs_operand = lhs_operand;}

SgUntypedExpression* SgUntypedBinaryOperator::get_rhs_operand() const            {return p_rhs_operand;}
void SgUntypedBinaryOperator::set_rhs_operand(SgUntypedExpression* rhs_operand)  {p_rhs_operand = rhs_operand;}


std::string SgUntypedNamedStatement::get_statement_name() const            {return p_statement_name;}
void        SgUntypedNamedStatement::set_statement_name(std::string name)  {p_statement_name = name;}

SgUntypedNamedStatement::~SgUntypedNamedStatement() {}
SgUntypedNamedStatement::SgUntypedNamedStatement(Sg_File_Info* start, std::string statement_name)
   : SgUntypedStatement(start), p_statement_name(statement_name)
   {
   }

VariantT SgUntypedNamedStatement::variantT() const {return V_SgUntypedNamedStatement;}


const SgUntypedDeclarationStatementPtrList&  SgUntypedDeclarationList::get_decl_list() const {return p_decl_list;}
SgUntypedDeclarationStatementPtrList& SgUntypedDeclarationList::get_decl_list()              {return p_decl_list;}

SgUntypedDeclarationList::~SgUntypedDeclarationList() {}
SgUntypedDeclarationList::SgUntypedDeclarationList(Sg_File_Info* start) : SgUntypedNode(start) {}


SgUntypedDeclarationStatement::~SgUntypedDeclarationStatement() {}
SgUntypedDeclarationStatement::SgUntypedDeclarationStatement(Sg_File_Info* start)
   : SgUntypedStatement(start)
   {
   }


const SgUntypedStatementPtrList&  SgUntypedStatementList::get_stmt_list() const {return p_stmt_list;}
SgUntypedStatementPtrList& SgUntypedStatementList::get_stmt_list()              {return p_stmt_list;}

SgUntypedStatementList::~SgUntypedStatementList() {}
SgUntypedStatementList::SgUntypedStatementList(Sg_File_Info* start)
   : SgUntypedNode(start)
   {
   }


std::string SgUntypedFunctionDeclaration::get_name() const     {return p_name;}
void SgUntypedFunctionDeclaration::set_name(std::string name)  {p_name = name;}

// SgUntypedInitializedNameList* SgUntypedFunctionDeclaration::get_parameters() const;
// void SgUntypedFunctionDeclaration::set_parameters(SgUntypedInitializedNameList* parameters);

// SgUntypedType* SgUntypedFunctionDeclaration::get_type() const;
// void SgUntypedFunctionDeclaration::set_type(SgUntypedType* type);

SgUntypedFunctionScope* SgUntypedFunctionDeclaration::get_scope() const     {return p_scope;}
void SgUntypedFunctionDeclaration::set_scope(SgUntypedFunctionScope* scope) {p_scope = scope;}

SgUntypedNamedStatement* SgUntypedFunctionDeclaration::get_end_statement() const        {return p_end_statement;}
void SgUntypedFunctionDeclaration::set_end_statement(SgUntypedNamedStatement* end_stmt) {p_end_statement = end_stmt;}

SgUntypedFunctionDeclaration::~SgUntypedFunctionDeclaration() {}
SgUntypedFunctionDeclaration::SgUntypedFunctionDeclaration(Sg_File_Info* start, std::string name) 
   :  SgUntypedDeclarationStatement(start), p_name(name)
   {
   }

SgUntypedFunctionDeclaration* isSgUntypedFunctionDeclaration(SgNode* node)
{
   return dynamic_cast<SgUntypedFunctionDeclaration*>(node);   
}

SgUntypedProgramHeaderDeclaration::~SgUntypedProgramHeaderDeclaration() {}
SgUntypedProgramHeaderDeclaration:: SgUntypedProgramHeaderDeclaration(Sg_File_Info* start, std::string name)
   :  SgUntypedFunctionDeclaration(start, name)
   {
   }

VariantT SgUntypedProgramHeaderDeclaration::variantT() const {return V_SgUntypedProgramHeaderDeclaration;}


SgUntypedImplicitDeclaration::~SgUntypedImplicitDeclaration() {}
SgUntypedImplicitDeclaration::SgUntypedImplicitDeclaration(Sg_File_Info* start)
   :  SgUntypedDeclarationStatement(start)
   {
   }

VariantT SgUntypedImplicitDeclaration::variantT() const {return V_SgUntypedImplicitDeclaration;}


SgUntypedType* SgUntypedInitializedName::get_type() const     {return p_type;}
void SgUntypedInitializedName::set_type(SgUntypedType* type)  {p_type = type;}

std::string SgUntypedInitializedName::get_name() const        {return p_name;}
void SgUntypedInitializedName::set_name(std::string name)     {p_name = name;}

SgUntypedInitializedName::~SgUntypedInitializedName() {}
SgUntypedInitializedName::SgUntypedInitializedName(Sg_File_Info* start, SgUntypedType* type, std::string name)
   :  SgUntypedNode(start), p_type(type), p_name(name)
   {
   }


//========================================================================================
// SgUntypedVariableDeclaration
//----------------------------------------------------------------------------------------
SgUntypedVariableDeclaration::SgUntypedVariableDeclaration(Sg_File_Info* start, SgUntypedType* type) 
   : SgUntypedDeclarationStatement(start), p_type(type)
   {
      p_parameters = new SgUntypedInitializedNameList(start);
   }
SgUntypedVariableDeclaration::~SgUntypedVariableDeclaration() {}

VariantT SgUntypedVariableDeclaration::variantT() const {return V_SgUntypedVariableDeclaration;}

SgUntypedType* SgUntypedVariableDeclaration::get_type() const     {return p_type;}
void SgUntypedVariableDeclaration::set_type(SgUntypedType* type)  {p_type = type;}

SgUntypedInitializedNameList* SgUntypedVariableDeclaration::get_parameters() const           {return p_parameters;}
void SgUntypedVariableDeclaration::set_parameters(SgUntypedInitializedNameList* parameters)  {p_parameters = parameters;}


const SgUntypedInitializedNamePtrList&  SgUntypedInitializedNameList::get_name_list() const {return p_name_list;}
SgUntypedInitializedNamePtrList& SgUntypedInitializedNameList::get_name_list()              {return p_name_list;}


SgUntypedInitializedNameList::~SgUntypedInitializedNameList() {}
SgUntypedInitializedNameList::SgUntypedInitializedNameList(Sg_File_Info* start)
   : SgUntypedNode(start)
   {
   }


const SgUntypedFunctionDeclarationPtrList& SgUntypedFunctionDeclarationList::get_func_list() const {return p_func_list;}
SgUntypedFunctionDeclarationPtrList& SgUntypedFunctionDeclarationList::get_func_list()             {return p_func_list;}

SgUntypedFunctionDeclarationList::~SgUntypedFunctionDeclarationList() {}
SgUntypedFunctionDeclarationList::SgUntypedFunctionDeclarationList(Sg_File_Info* start)
   : SgUntypedNode(start)
   {
   }


// SgToken::ROSE_Fortran_Operators SgUntypedUnaryOperator::get_operator_enum() const;
// void SgUntypedUnaryOperator::set_operator_enum(SgToken::ROSE_Fortran_Operators operator_enum);

// std::string SgUntypedUnaryOperator::get_operator_name() const;
// void SgUntypedUnaryOperator::set_operator_name(std::string operator_name);

// SgUntypedExpression* SgUntypedUnaryOperator::get_operand() const;
// void SgUntypedUnaryOperator::set_operand(SgUntypedExpression* operand);

SgUntypedUnaryOperator::~SgUntypedUnaryOperator() {}
SgUntypedUnaryOperator::SgUntypedUnaryOperator(Sg_File_Info* start, SgToken::ROSE_Fortran_Keywords stmt_enum,
                       SgToken::ROSE_Fortran_Operators op_enum, std::string op_name, SgUntypedExpression* op)
   : SgUntypedExpression(start, stmt_enum), p_operator_enum(op_enum), p_operator_name(op_name), p_operand(op)
   {
   }


//========================================================================================
// SgUntypedScope
//----------------------------------------------------------------------------------------
SgUntypedScope::SgUntypedScope(Sg_File_Info* start)
   : SgUntypedStatement(start)
   {
      p_declaration_list = new SgUntypedDeclarationList(NULL);
      p_statement_list   = new SgUntypedStatementList(NULL);
      p_function_list    = new SgUntypedFunctionDeclarationList(NULL);
   }
SgUntypedScope::~SgUntypedScope()
   {
      //delete p_declaration_list;
      //delete p_statement_list;
      //delete p_function_list;
   }

SgUntypedDeclarationList* SgUntypedScope::get_declaration_list() const
   {
      return p_declaration_list;
   }
void SgUntypedScope::set_declaration_list(SgUntypedDeclarationList* decl_list)
   {
      assert(decl_list);
      //delete p_declaration_list;
      p_declaration_list = decl_list;
   }

SgUntypedStatementList* SgUntypedScope::get_statement_list() const
   {
      return p_statement_list;
   }
void SgUntypedScope::set_statement_list(SgUntypedStatementList* stmt_list)
   {
      assert(stmt_list);
      //delete p_statement_list;
      p_statement_list = stmt_list;
   }

SgUntypedFunctionDeclarationList* SgUntypedScope::get_function_list() const
   {
      return p_function_list;
   }
void SgUntypedScope::set_function_list(SgUntypedFunctionDeclarationList* func_list)
   {
      assert(func_list);
      //delete p_function_list;
      p_function_list = func_list;
   }

//========================================================================================
// SgUntypedFunctionScope
//----------------------------------------------------------------------------------------
SgUntypedFunctionScope::~SgUntypedFunctionScope() {}
SgUntypedFunctionScope::SgUntypedFunctionScope(Sg_File_Info* start) : SgUntypedScope(start) {}

//========================================================================================
// SgUntypedGlobalScope
//----------------------------------------------------------------------------------------
SgUntypedGlobalScope::SgUntypedGlobalScope(Sg_File_Info* start) : SgUntypedScope(start) {}
SgUntypedGlobalScope::~SgUntypedGlobalScope() {}

//========================================================================================
// SgUntypedFile
//----------------------------------------------------------------------------------------
SgUntypedFile::SgUntypedFile(Sg_File_Info* start) : SgUntypedNode(start) {}
SgUntypedFile::~SgUntypedFile() {}

VariantT SgUntypedFile::variantT() const {return V_SgUntypedFile;}

SgUntypedGlobalScope* SgUntypedFile::get_scope() const         {return p_scope;}
void SgUntypedFile::set_scope(SgUntypedGlobalScope* scope)     {p_scope = scope;}

SgUntypedFile* isSgUntypedFile(SgNode* node)
{
   return dynamic_cast<SgUntypedFile*>(node);   
}
