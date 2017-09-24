#ifndef MCODE_H_INCLUDED
#define MCODE_H_INCLUDED

#include <string>
#include <vector>

typedef enum{none_sentence,\    //out put sentence of middle code type
s_section, s_data, s_stack_change, s_stack_all, s_tab, s_jmp,\
s_call_ret, s_push_pop, s_load, s_store, s_calculate,
} sentence_type;

typedef enum{none,\             //type of variable, array and function
v_int, v_real, v_void, v_int_point, v_real_point, v_void_point,\
f_int, f_real, f_void, f_int_point, f_real_point, f_void_point,\
a_int, a_real, a_void, a_int_point, a_real_point, a_void_point\
} node_type;

class Tree_node
{
public:
    int nodeNO;
    std::string type;      //identifier or number or others, also 'error' means not valuable
    std::string value;     //real value
    std::vector<Tree_node*> childs;

public:
    Tree_node()
    {

    }
};

class declaration_node
{
public:
    node_type type;     //type of variable, array and function
    std::string name;
    int arry_length;    //the length of array
    int params_length;  //parameters of function

public:
    declaration_node() : type(none), arry_length(0), params_length(0)
    {

    }
    declaration_node(node_type _type, std::string _name, int _length)
        : type(_type), name(_name), params_length(_length)
    {

    }

};

void f_program(Tree_node* node);
void f_declaration_list(Tree_node* node);
void f_declaration(Tree_node* node);
void f_var_declaration(Tree_node* node);
std::string f_type_specifier(Tree_node* node);
void f_fun_declaration(Tree_node* node);
void f_params(Tree_node* node);
void f_param_list(Tree_node* node);
void f_param(Tree_node* node);
void f_compound_stmt(Tree_node* node);
void f_local_declarations(Tree_node* node);
void f_statement_list(Tree_node* node);
void f_statement(Tree_node* node);
void f_expression_stmt(Tree_node* node);
void f_selection_stmt(Tree_node* node);
void f_iteration_stmt(Tree_node* node);
void f_return_stmt(Tree_node* node);
void f_expression(Tree_node* node);
void f_var(Tree_node* node);
void f_simple_expression(Tree_node* node);
void f_relop(Tree_node* node);
void f_additive_expression(Tree_node* node);
void f_addop(Tree_node* node);
void f_term(Tree_node* node);
void f_mulop(Tree_node* node);
void f_factor(Tree_node* node);
void f_call(Tree_node* node);
void f_args(Tree_node* node);
void f_arg_list(Tree_node* node);

bool is_repetition(declaration_node& node);
node_type get_node_type(std::string type);
declaration_node* get_declaration_node(std::string ID);
char* itoa(unsigned int number);

#endif // MCODE_H_INCLUDED
