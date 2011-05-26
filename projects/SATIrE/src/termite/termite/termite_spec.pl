% This is termite_spec.pl, a tree grammar (abstract syntax specification) to
% be used with term_lint. It defines the structure of Termite terms.
% Author: Gergo Barany <gergo@complang.tuwien.ac.at>


% TODO with the grammar itself:
% - see if there is anything missing (valid terms are not covered)
% - see if there is anything wrong
% - find uses of the nonterminal 'todo', which is a placeholder meaning "did
%   not bother to define this yet", and replace them with stricter rules
% - find other places where things could be stricter (for instance, the rhs
%   of a dot or arrow expression is always a var_ref_exp, and we could
%   express this in our grammar)
% - maybe put the rules in some nice order

% TODO with the grammar and term_lint.pl:
% - add support for as-patterns to term_lint.pl (see comment there) and use
%   them to check awesome properties like "the operands and result of a
%   binary_op usually have sort of the same type"

% TODO with the Termite term format in general:
% - rename basic_block to compound_statement already
% - introduce consistent naming of statements/expressions; e.g., use _expr
%   and _stmt everywhere instead of the current mix
% - find other things that should be fixed (the always-one-element list in
%   variable declarations, for instance)


% This is the start symbol (in case you didn't guess).
start_symbol(termite).


% Grammar rules.
termite ::=
    project.

project ::=
    project([source_file], default_annotation, analysis_info, file_info).

source_file ::=
    source_file(global, default_annotation, analysis_info, file_info).

initialized_name ::=
    initialized_name(initializer?, initialized_name_annotation,
                     analysis_info, file_info).

% --- statements ---
statement ::=
    break_stmt
  | case_option_stmt
  | continue_stmt
  | declaration_statement
  | default_option_stmt
  | expr_statement
  | for_init_statement
  | goto_statement
  | label_statement
  | null_statement
  | return_stmt
  | scope_statement.

break_stmt ::=
    break_stmt(default_annotation, analysis_info, file_info).

case_option_stmt ::=
    case_option_stmt(expression, statement, expression? /* key_range_end */,
                     default_annotation, analysis_info, file_info).

continue_stmt ::=
    continue_stmt(default_annotation, analysis_info, file_info).

declaration_statement ::=
    class_declaration
  | enum_declaration
  | function_declaration
  | function_parameter_list
  | pragma_declaration
  | typedef_declaration
  | variable_declaration
  | variable_definition.

class_declaration ::=
    class_declaration(class_definition?, class_declaration_annotation,
                      analysis_info, file_info).

enum_declaration ::=
    enum_declaration([initialized_name], enum_declaration_annotation,
                     analysis_info, file_info).

function_declaration ::=
    function_declaration(function_parameter_list, function_definition?,
                         function_declaration_annotation,
                         analysis_info, file_info).

function_parameter_list ::=
    function_parameter_list([initialized_name],
                            default_annotation, analysis_info, file_info).

pragma_declaration ::=
    pragma_declaration(todo).

typedef_declaration ::=
    typedef_declaration(declaration_statement? /* base type definition */,
                        typedef_annotation, analysis_info, file_info).

variable_declaration ::=
    variable_declaration([initialized_name], variable_declaration_specific,
                         analysis_info, file_info).

variable_definition ::=
    variable_definition(todo).


default_option_stmt ::=
    default_option_stmt(statement,
                        default_annotation, analysis_info, file_info).

expr_statement ::=
    expr_statement(expression, default_annotation, analysis_info, file_info).

for_init_statement ::=
    for_init_statement([statement],
                       default_annotation, analysis_info, file_info).

goto_statement ::=
    goto_statement(label_annotation, analysis_info, file_info).

label_statement ::=
    label_statement(label_annotation, analysis_info, file_info).

null_statement ::=
    null_statement(default_annotation, analysis_info, file_info). /* really? */

return_stmt ::=
    return_stmt(expression, default_annotation, analysis_info, file_info).

scope_statement ::=
    basic_block
  | class_definition
  | do_while_stmt
  | for_statement
  | function_definition
  | global
  | if_stmt
  | switch_statement
  | while_stmt.

basic_block ::=
    basic_block([statement], default_annotation, analysis_info, file_info).

class_definition ::=
    class_definition([variable_declaration], class_definition_annotation,
                     analysis_info, file_info).

do_while_stmt ::=
    do_while_stmt(statement /* body */, statement /* condition */,
                  default_annotation, analysis_info, file_info).

for_statement ::=
    for_statement(for_init_statement, statement /* test */,
                  expression /* increment */, statement /* body */,
                  default_annotation, analysis_info, file_info).

function_definition ::=
    function_definition(basic_block,
                        default_annotation, analysis_info, file_info).

global ::=
    global([declaration_statement],
           default_annotation, analysis_info, file_info).

if_stmt ::=
    if_stmt(statement /* condition */, statement /* true */,
            statement? /* else */,
            default_annotation, analysis_info, file_info).

switch_statement ::=
    switch_statement(statement /* key */, statement /* body */,
                     default_annotation, analysis_info, file_info).

while_stmt ::=
    while_stmt(statement /* condition */, statement /* body */,
               default_annotation, analysis_info, file_info).

% --- expressions ---
expression ::=
    binary_op
  | conditional_exp
  | expr_list_exp
  | function_call_exp
  | function_ref_exp
  | initializer
  | null_expression
  | size_of_op
  | unary_op
  | value_exp
  | var_arg_copy_op
  | var_arg_end_op
  | var_arg_op
  | var_arg_start_one_operand_op
  | var_arg_start_op
  | var_ref_exp.

binary_op ::=
    functors [add_op, and_assign_op, and_op, arrow_exp, assign_op,
        bit_and_op, bit_or_op, bit_xor_op, comma_op_exp, div_assign_op,
        divide_op, dot_exp, equality_op, greater_or_equal_op,
        greater_than_op, ior_assign_op, less_or_equal_op, less_than_op,
        lshift_assign_op, lshift_op, minus_assign_op, mod_assign_op, mod_op,
        mult_assign_op, multiply_op, not_equal_op, or_op, plus_assign_op,
        pntr_arr_ref_exp, rshift_assign_op, rshift_op, subtract_op,
        xor_assign_op]
    with (expression /* lhs */, expression /* rhs */,
          binary_op_annotation, analysis_info, file_info).

conditional_exp ::=
    conditional_exp(expression /* condition */,
                    expression /* true */, expression /* false */,
                    conditional_exp_annotation, analysis_info, file_info).

expr_list_exp ::=
    expr_list_exp([expression], default_annotation, analysis_info, file_info).

function_call_exp ::=
    function_call_exp(expression /* function */, expr_list_exp /* args */,
                      function_call_exp_annotation, analysis_info, file_info).

function_ref_exp ::=
    function_ref_exp(function_ref_exp_annotation, analysis_info, file_info).

initializer ::=
    aggregate_initializer
  | assign_initializer.

aggregate_initializer ::=
    aggregate_initializer(expr_list_exp,
                          default_annotation, analysis_info, file_info).

assign_initializer ::=
    assign_initializer(expression, assign_initializer_annotation,
                       analysis_info, file_info).

null_expression ::=
    null_expression(default_annotation, analysis_info, file_info).

size_of_op ::=
    size_of_op(expression?, size_of_op_annotation, analysis_info, file_info).

unary_op ::=
    cast_exp(expression, expression? /* original expression tree */,
             unary_op_annotation, analysis_info, file_info)
  | functors [address_of_op, bit_complement_op, minus_minus_op,
        minus_op, not_op, plus_plus_op, pointer_deref_exp, unary_add_op]
    with (expression, unary_op_annotation, analysis_info, file_info).

value_exp ::=
    functors [long_long_int_val, unsigned_long_long_int_val, long_int_val,
        unsigned_long_val, int_val, unsigned_int_val, short_val,
        unsigned_short_val, char_val, unsigned_char_val, float_val,
        double_val, long_double_val, string_val]
    with (expression? /* original expression tree */,
          value_annotation, analysis_info, file_info).

var_arg_copy_op ::=
    var_arg_copy_op(todo).

var_arg_end_op ::=
    var_arg_end_op(todo).

var_arg_op ::=
    var_arg_op(todo).

var_arg_start_one_operand_op ::=
    var_arg_start_one_operand_op(todo).

var_arg_start_op ::=
    var_arg_start_op(todo).

var_ref_exp ::=
    var_ref_exp(var_ref_exp_annotation, analysis_info, file_info).

% --- annotations ---
default_annotation ::=
    default_annotation({null})
  | default_annotation({null}, preprocessing_info).

initialized_name_annotation ::=
    initialized_name_annotation(type, name, todo /* storage modifier */,
                                scope_name?, preprocessing_info).

function_declaration_annotation ::=
    function_declaration_annotation(type, name, declaration_modifier,
                                    preprocessing_info).

class_declaration_annotation ::=
    class_declaration_annotation(name, todo /* class kind */, type,
                                 preprocessing_info).

enum_declaration_annotation ::=
    enum_declaration_annotation(name, todo, todo, preprocessing_info).

class_definition_annotation ::=
    class_definition_annotation(file_info, preprocessing_info).

variable_declaration_specific ::=
    variable_declaration_specific(todo /* declaration modifier */,
                                  declaration_statement? /* base type decl */,
                                  preprocessing_info).

label_annotation ::=
    label_annotation(name, preprocessing_info).

size_of_op_annotation ::=
    size_of_op_annotation(type? /* operand */, type /* sizeof expression */,
                          preprocessing_info).

value_annotation ::=
    value_annotation(number_or_string, preprocessing_info).

binary_op_annotation ::=
    binary_op_annotation(type, preprocessing_info).

unary_op_annotation ::=
    unary_op_annotation(fixity, type, todo /* cast type */,
                        todo /* throw kind */, preprocessing_info).

var_ref_exp_annotation ::=
    var_ref_exp_annotation(type, name, todo /* storage modifier */,
                           scope_name?, preprocessing_info).

typedef_annotation ::=
    typedef_annotation(name, type, preprocessing_info).

function_ref_exp_annotation ::=
    function_ref_exp_annotation(name, type, preprocessing_info).

function_call_exp_annotation ::=
    function_call_exp_annotation(type, preprocessing_info).

assign_initializer_annotation ::=
    assign_initializer_annotation(type, preprocessing_info).

conditional_exp_annotation ::=
    conditional_exp_annotation(type, preprocessing_info).

% --- other stuff ---
analysis_info ::=
    analysis_info([_]).

file_info ::=
    file_info({_}, {_}, {_}).

preprocessing_info ::=
    preprocessing_info([_]).

type ::=
    basic_type
  | array_type(type, expression?)
  | function_type(type /* return */, todo /* ellipses */, [type] /* args */)
  | modifier_type(type, type_modifier)
  | named_type
  | type_default
  | pointer_type(type).

basic_type ::=
    atoms [type_bool, type_char, type_double, type_ellipse, type_float,
        type_int, type_long, type_long_double, type_long_long, type_short,
        type_signed_char, type_string, type_unsigned_char,
        type_unsigned_int, type_unsigned_long, type_unsigned_long_long,
        type_unsigned_short, type_void].

type_default ::= atoms [type_default].

named_type ::=
    class_type(name, todo, todo)
  | enum_type(todo)
  | typedef_type(name, type).

type_modifier ::=
    type_modifier([todo], todo, todo, todo).

name ::=
    {Name} where atom(Name).

scope_name ::=  % name of a scope
  {::}
  | class_scope(name, class_kind, preprocessing_info)
  | name.

class_kind ::=
    {class}
  | {struct}
  | {union}.

number_or_string ::=
    {It} where ( numberatom(It) ; number(It) ; string(It) ; atom(It) ).

fixity ::=  % fixity of unary operators
    {prefix}
  | {postfix}.

declaration_modifier ::=
    declaration_modifier(todo, todo, todo, todo).

todo ::=
    {_}.


% For an optional argument  A? , this predicate is tried first.
missing(null).


% Test whether the atom A can be interpreted as a number.
numberatom(A) :-
    atom(A),
    catch(atom_number(A, _N), _, fail).
