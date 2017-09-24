#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include "mcode.h"

#define DEBUGER_f_return_stmt
#define DEBUGER_f_fun_declaration
#define DEBUGER_f_call

/**************how much sentences there are, how much function here is****************/

using namespace std;

extern vector<vector<declaration_node> > declaration_table;     //symbol table
unsigned int registerNO, tabNO;                                 //used to generate different register and tab, backNO used to back write tab
ofstream out_mcode;                                             //out put mcode file
bool newScopeAdded=false;                                       //avoid re-added of new declaration scope in function declaration
vector<string> fun_args;                                        //used to save call arguments temply
unsigned int stack_deepth;                                      //used to indicate how much variable is in the stack
int which_fun;                                                  //used to indicated which function is defined

void f_program(Tree_node* node)
{
    declaration_table.push_back(vector<declaration_node>());    //add whole scope

    tabNO=0;   //initial it, so that it can start from 0, but they start from 1 in fact, because I like use ++registerNO


    out_mcode.open("mcode.txt");                //open mcode.txt
    if(!out_mcode)
    {
        cout<<"can't create mcode.txt!"<<endl;
        exit(1);
    }

    out_mcode<<s_section<<" segment code"<<endl;            //generate code_segment
    declaration_table.back().push_back(declaration_node(f_void, "write", 1));   //add write function
    declaration_table.back().push_back(declaration_node(f_int, "read", 0));     //add read function
    f_declaration_list((node->childs)[0]);
    out_mcode<<"\n"<<s_section<<" segment end code"<<endl;

    out_mcode<<"\n"<<s_section<<" segment data"<<endl;          //generate data_segment, all whole variable should put here
    for(vector<declaration_node>::iterator iter=declaration_table.begin()->begin(); iter!=declaration_table.begin()->end(); ++iter)
    {
        if(iter->type==v_int)       //is int variable
        {
            out_mcode<<s_data<<"\tDW "<<iter->name<<" [ 1 ]"<<endl;
        }
        else if(iter->type==a_int)  //is int array
        {
            out_mcode<<s_data<<"\tDW "<<iter->name<<" [ "<<iter->arry_length<<" ]"<<endl;
        }
    }
    out_mcode<<s_section<<" segment end data"<<endl;

    out_mcode<<"\n"<<s_section<<" segment stack\n"<<endl;           //generate stack_segment
    out_mcode<<s_section<<" segment end stack"<<endl;

    out_mcode.close();                          //close mcode.txt
    declaration_table.pop_back();               //delete whole scope
}

void f_declaration_list(Tree_node* node)    //just call other function, nothing!
{
    switch(node->childs.size())
    {
    case 1 :
        f_declaration((node->childs)[0]);
        break;

    case 2 :
        f_declaration_list((node->childs)[0]);
        f_declaration((node->childs)[1]);
        break;

    default:
        break;
    }
}

void f_declaration(Tree_node* node)    //just call other function, nothing!
{
    if((node->childs)[0]->type=="var-declaration")
    {
        f_var_declaration((node->childs)[0]);
    }
    else if((node->childs)[0]->type=="fun-declaration")
    {
        f_fun_declaration((node->childs)[0]);
    }
    else
    {

    }
}

void f_var_declaration(Tree_node* node)
{
    string type;
    declaration_node new_node;

    new_node.name=(node->childs)[1]->value;         //get variable name and judge repetition
    if(is_repetition(new_node))
    {
        cout<<"error! redefined of "<<(node->childs)[1]->value<<endl;
        exit(1);
    }

    switch(node->childs.size())                     //from here is get variable type
    {
    case 3 :            //is variable
        type+="v_";
        break;

    case 6 :            //is array
        type+="a_";
        new_node.arry_length=atoi(((node->childs)[3]->value).c_str());

    default:
        break;
    }

    type+=f_type_specifier((node->childs)[0]);
    new_node.type=get_node_type(type);

    declaration_table.back().push_back(new_node);   //new declaration push into table
    if(declaration_table.size() > 1)                //if is not whole scope, should change stack point, because it is saved in stack orderly
    {
        if(new_node.type==v_int)    //is variable
        {
            stack_deepth+=2;
            out_mcode<<s_stack_change<<"\tstack - 2"<<endl;
        }
        else if(new_node.type==a_int)   //is array
        {
            stack_deepth+=2*new_node.arry_length;
            out_mcode<<s_stack_change<<"\tstack - "<<itoa(2*new_node.arry_length)<<endl;
        }

    }
}

string f_type_specifier(Tree_node* node)    //just give back type string
{
    string type;
    for(vector<Tree_node*>::iterator iter=node->childs.begin(); iter!=node->childs.end(); ++iter)
    {
        type+=(*iter)->value;
        if(iter!=node->childs.end()-1)
        {
            type+="_";
        }
    }

    return type;
}

void f_fun_declaration(Tree_node* node)
{
    newScopeAdded=true;                         //means the compound-stmt need't add new scope
    string type;
    declaration_node new_node;
    stack_deepth=0;
    registerNO=0;
    new_node.name=(node->childs)[1]->value;     //get name and judge repetition
    if(is_repetition(new_node))
    {
        cout<<"error! redefined of "<<(node->childs)[1]->value<<endl;
        exit(1);
    }

    type+="f_";                                  //get type
    type+=f_type_specifier((node->childs)[0]);
    new_node.type=get_node_type(type);
    declaration_table.back().push_back(new_node);//push declaration node into table
    which_fun=declaration_table[0].size()-1;

#ifndef DEBUGER_f_fun_declaration
    cout<<"f_fun_declaration:"<<declaration_table[0][which_fun].name<<endl;
    cout<<"f_fun_declaration:"<<declaration_table[0][which_fun].params_length<<endl;
#endif // DEBUGER_f_fun_declaration

    declaration_table.push_back(vector<declaration_node>());//add new scope, it is function scope
    f_params((node->childs)[3]);                //get parameters

    out_mcode<<"\n"<<s_section<<" proc "<<new_node.name<<endl;  //generate new process
    unsigned int stack_tmp=2;                   //consider the IP pushed into stack when call function
    for(vector<declaration_node>::iterator iter=declaration_table.back().begin(); iter!=declaration_table.back().end(); ++iter)
    {                                                                                   //receive arguments
        if(iter->type=v_int)
        {
            out_mcode<<s_load<<"\treg"<<itoa(++registerNO);//no endl
            out_mcode<<" = stack [ + "<<itoa(stack_tmp)<<" ]"<<endl;
            out_mcode<<s_push_pop<<"\tpush reg"<<itoa(registerNO)<<endl;
            stack_tmp+=2*2;     //stack increase and next parameter more deeply
        }
    }

    f_compound_stmt((node->childs)[5]);

    out_mcode<<s_section<<" proc end "<<new_node.name<<endl;
    declaration_table.pop_back();       //delete function scope
}

void f_params(Tree_node* node)
{
    if(node->childs[0]->type=="param-list") //when params is void, do nothing
    {
        f_param_list((node->childs)[0]);
    }
}

void f_param_list(Tree_node* node)
{
    switch((node->childs).size())
    {
    case 1 :
        f_param((node->childs)[0]);
        break;

    case 3 :
        f_param_list((node->childs)[0]);
        f_param((node->childs)[2]);
        break;

    default:
        break;
    }

}

void f_param(Tree_node* node)
{
    string type;
    declaration_node new_node;

    new_node.name=(node->childs)[1]->value;     //get name and judge repetition
    if(is_repetition(new_node))
    {
        cout<<"error! redefined of "<<(node->childs)[1]->value<<endl;
        exit(1);
    }

    type+="v_";
    type+=f_type_specifier((node->childs)[0]);
    new_node.type=get_node_type(type);

    if(new_node.type==v_int)                    //here function only receive int type
    {
        declaration_table.back().push_back(new_node);
        ++(declaration_table[0][which_fun].params_length);
        stack_deepth+=2;
    }
    else
    {
        cout<<"function can only receive the type of int"<<endl;
        exit(1);
    }
}

void f_compound_stmt(Tree_node* node)
{
    if(newScopeAdded==false)            //true means it is the compound_stmt of function, no need add new scope anymore
    {
        declaration_table.push_back(vector<declaration_node>());
        stack_deepth=0;
    }

    f_local_declarations((node->childs)[1]);
    f_statement_list((node->childs)[2]);

    if(newScopeAdded==false)            //true means it is the compound_stmt of function, no need delete scope anymore
    {
        if(declaration_table.size() > 1)
        {
            int stack_size=0;
            for(vector<declaration_node>::iterator iter=declaration_table.back().begin(); iter!=declaration_table.back().end(); ++iter)
            {
                if(iter->type==v_int)
                {
                    stack_size+=2;
                }
                else if(iter->type==a_int)
                {
                    stack_size+=2*iter->arry_length;
                }
            }
            out_mcode<<s_stack_change<<"\tstack + "<<itoa(stack_deepth)<<endl;
        }

        declaration_table.pop_back();
    }
    else
    {
        newScopeAdded==false;
    }
}

void f_local_declarations(Tree_node* node)
{
    switch((node->childs).size())
    {
    case 1 :
        break;

    case 2 :
        f_local_declarations((node->childs)[0]);
        f_var_declaration((node->childs)[1]);
        break;

    default:
        break;
    }
}

void f_statement_list(Tree_node* node)
{
    switch((node->childs).size())
    {
    case 1 :
        break;

    case 2 :
        f_statement_list((node->childs)[0]);
        f_statement((node->childs)[1]);
        break;

    default:
        break;
    }
}

void f_statement(Tree_node* node)    //just call other function, nothing!
{
    if((node->childs)[0]->type=="expression-stmt")
    {
        f_expression_stmt((node->childs)[0]);
    }
    else if((node->childs)[0]->type=="compound-stmt")
    {
        f_compound_stmt((node->childs)[0]);
    }
    else if((node->childs)[0]->type=="selection-stmt")
    {
        f_selection_stmt((node->childs)[0]);
    }
    else if((node->childs)[0]->type=="iteration-stmt")
    {
        f_iteration_stmt((node->childs)[0]);
    }
    else if((node->childs)[0]->type=="return-stmt")
    {
        f_return_stmt((node->childs)[0]);
    }
    else
    {

    }
}

void f_expression_stmt(Tree_node* node)    //just call other function, nothing!
{
    switch((node->childs).size())
    {
    case 1 :
        break;

    case 2 :
        f_expression((node->childs)[0]);
        break;

    default :
        break;
    }
}

void f_selection_stmt(Tree_node* node)
{
    node->value=itoa(++tabNO);

    f_expression((node->childs)[2]);    //generate label and jump sentence
    out_mcode<<s_jmp<<"\tif_false "<<(node->childs)[2]->value<<" goto "<<"B"<<node->value<<endl;
    f_compound_stmt((node->childs)[4]);
    out_mcode<<s_jmp<<"\tgoto B"<<node->value<<"_"<<endl;
    out_mcode<<s_tab<<" B"<<node->value<<":"<<endl;
    f_compound_stmt((node->childs)[6]);
    out_mcode<<s_tab<<" B"<<node->value<<"_:"<<endl;
}

void f_iteration_stmt(Tree_node* node)
{
    node->value=itoa(++tabNO);
                                        //generate label and jump sentence
    out_mcode<<s_tab<<" B"<<node->value<<":"<<endl;
    f_expression((node->childs)[2]);
    out_mcode<<s_jmp<<"\tif_false "<<(node->childs)[2]->value<<" goto "<<"B"<<node->value<<"_"<<endl;
    f_compound_stmt((node->childs)[4]);
    out_mcode<<s_jmp<<"\tgoto B"<<node->value<<endl;
    out_mcode<<s_tab<<" B"<<node->value<<"_:"<<endl;
}

void f_return_stmt(Tree_node* node)
{
    int stack_size=0;
    if((declaration_table[0][which_fun].type==f_void && (node->childs).size()==3) || (declaration_table[0][which_fun].type==f_int && (node->childs).size()==2))
    {
        cout<<"error! when return function "<<declaration_table[0][which_fun].name<<endl;
        exit(1);
    }

    //calculate stack_size
    for(vector<vector<declaration_node> >::reverse_iterator iter=declaration_table.rbegin(); iter!=declaration_table.rend()-1; ++iter)
    {
        for(vector<declaration_node>::iterator iter1=iter->begin(); iter1!=iter->end(); ++iter1)
        {
            if(iter1->type==v_int)
            {
                stack_size+=2;
            }
            else if(iter1->type==a_int)
            {
                stack_size+=2*iter1->arry_length;
            }
        }
    }
#ifndef DEBUGER_f_return_stmt
    cout<<"f_return_stmt:"<<declaration_table[0][which_fun].params_length<<endl;
#endif // DEBUGER_f_return_stmt
    switch((node->childs).size())
    {
    case 2 :
        break;
    case 3 :
        f_expression((node->childs)[1]);    //save return value to stack
        out_mcode<<s_store<<"\tstack [ + "<<itoa(stack_size+(declaration_table[0][which_fun].params_length)*2+18)<<" ] = "<<(node->childs)[1]->value<<endl;  //set return value in return space
        break;
    default:
        break;
    }

    out_mcode<<s_stack_change<<"\tstack + "<<itoa(stack_size)<<endl;    //clear scope value
    out_mcode<<s_call_ret<<"\tret"<<endl;
}

void f_expression(Tree_node* node)
{
    switch((node->childs).size())
    {
    case 1 :
        f_simple_expression((node->childs)[0]);
        node->value=(node->childs)[0]->value;
        break;

    case 3 :
        f_var((node->childs)[0]);
        f_expression((node->childs)[2]);
        node->value=(node->childs)[2]->value;
        out_mcode<<s_store<<"\t"<<(node->childs)[0]->value<<" "<<"="<<" "<<(node->childs)[2]->value<<endl;
        break;

    default:
        break;
    }
}

void f_var(Tree_node* node)
{
    unsigned int stack_tmp=0;
    vector<vector<declaration_node> >::reverse_iterator  iter;
    vector<declaration_node>::reverse_iterator iter1;
    for(iter=declaration_table.rbegin(); iter!=declaration_table.rend(); ++iter)    //go every scope
    {
        for(iter1=iter->rbegin(); iter1!=iter->rend(); ++iter1)                     //go every declaration node
        {
            if(iter1->type==a_int)  //array
            {
                stack_tmp+=2*iter1->arry_length;
            }

            if(iter1->name==(node->childs)[0]->value)   //accord, then stop
            {
                goto next;
            }

            if(iter1->type==v_int)  //int
            {
                stack_tmp+=2;
            }
        }
    }

next:
    if(iter==declaration_table.rend())               //judge if has been declared
    {
        cout<<"\nerror! variable not defined in "<<(node->childs)[0]->value<<endl;
        exit(1);
    }

    switch((node->childs).size())
    {
    case 1 :        //variable
        if(iter==declaration_table.rend()-1)    //is whole scope variable
        {
            node->value=(node->childs)[0]->value;
        }
        else                                    //is local scope variable
        {
            node->value="stack [ + "+string(itoa(stack_tmp))+" ]";
        }

        break;

    case 4 :        //array
        f_expression((node->childs)[2]);
        if(iter==declaration_table.rend()-1)//is whole scope array
        {
            node->value=(node->childs)[0]->value + " [ "+(node->childs)[2]->value +" ] ";
        }
        else                                //is local scope array
        {
            out_mcode<<s_load<<"\treg"<<itoa(++registerNO)<<" = "<<itoa(stack_tmp)<<endl;
            out_mcode<<s_calculate<<"\treg"<<itoa(registerNO)<<" = reg"<<itoa(registerNO)<<" - "<<(node->childs)[2]->value<<endl;
            out_mcode<<s_calculate<<"\treg"<<itoa(registerNO)<<" = reg"<<itoa(registerNO)<<" - "<<(node->childs)[2]->value<<endl;
            node->value="stack [ + reg"+string(itoa(registerNO))+" ]";
        }
        break;

    default:
        break;
    }
}

void f_simple_expression(Tree_node* node)
{
    switch((node->childs).size())
    {
    case 1 :
        f_additive_expression((node->childs)[0]);    //give back result directly
        node->value=(node->childs)[0]->value;
        break;

    case 3 :
        f_additive_expression((node->childs)[0]);
        f_additive_expression((node->childs)[2]);
        f_relop((node->childs)[1]);
        node->value="reg"+string(itoa(++registerNO));   //put result in an register
        out_mcode<<s_calculate<<"\treg"<<registerNO<<" "<<"="<<" "<<(node->childs)[0]->value<<" "<<(node->childs)[1]->value<<" "<<(node->childs)[2]->value<<endl;
        break;

    default:
        break;
    }
}

void f_relop(Tree_node* node)   //give back option directly
{
    node->value=(node->childs)[0]->value;
}

void f_additive_expression(Tree_node* node)
{
    switch((node->childs).size())
    {
    case 1 :
        f_term((node->childs)[0]);    //give back result directly
        node->value=(node->childs)[0]->value;
        break;

    case 3 :
        f_additive_expression((node->childs)[0]);
        f_term((node->childs)[2]);
        f_addop((node->childs)[1]);
        node->value="reg"+string(itoa(++registerNO));   //put result in an register
        out_mcode<<s_calculate<<"\treg"<<registerNO<<" "<<"="<<" "<<(node->childs)[0]->value<<" "<<(node->childs)[1]->value<<" "<<(node->childs)[2]->value<<endl;
        break;

    default:
        break;
    }
}

void f_addop(Tree_node* node)   //give back option directly
{
    node->value=(node->childs)[0]->value;
}

void f_term(Tree_node* node)
{
    switch((node->childs).size())
    {
    case 1 :
        f_factor((node->childs)[0]);    //give back result directly
        node->value=(node->childs)[0]->value;
        break;

    case 3 :
        f_term((node->childs)[0]);
        f_factor((node->childs)[2]);
        f_mulop((node->childs)[1]);
        node->value="reg"+string(itoa(++registerNO));   //put result in an register
        out_mcode<<s_calculate<<"\treg"<<registerNO<<" "<<"="<<" "<<(node->childs)[0]->value<<" "<<(node->childs)[1]->value<<" "<<(node->childs)[2]->value<<endl;
        break;

    default:
        break;
    }
}

void f_mulop(Tree_node* node)   //give back option directly
{
    node->value=(node->childs)[0]->value;
}

void f_factor(Tree_node* node)
{
    switch((node->childs).size())
    {
    case 1 :
        if((node->childs)[0]->type=="var")  //should load it to an register
        {
            f_var((node->childs)[0]);
            out_mcode<<s_load<<"\treg"<<itoa(++registerNO)<<" = "<<(node->childs)[0]->value<<endl;
            node->value="reg"+string(itoa(registerNO));
        }
        else if((node->childs)[0]->type=="call")    //put result to an register
        {
            f_call((node->childs)[0]);
            node->value=(node->childs)[0]->value;
        }
        else if((node->childs)[0]->type=="NUM")     //give back it directly
        {
            node->value=(node->childs)[0]->value;
        }
        else
        {

        }
        break;

    case 3 :
        f_expression((node->childs)[0]);
        node->value=(node->childs)[0]->value;
        break;

    default :
        break;
    }
}

void f_call(Tree_node* node)
{
    declaration_node *dnode=get_declaration_node((node->childs)[0]->value);         //judge if function has been defined, need't care used variable as function, because it don't allow variable use the same name as function
    if(dnode==NULL)
    {
        cout<<"\nerror! function not defined in "<<(node->childs)[0]->value<<endl;
        out_mcode.close();
        exit(1);
    }

#ifndef DEBUGER_f_call
    cout<<"f_call:"<<dnode->name<<endl;
    cout<<"f_call:"<<dnode->params_length<<endl;
#endif // DEBUGER_f_call

    fun_args.clear();
    f_args((node->childs)[2]);

    if(dnode->params_length!=fun_args.size())   //judge if params_length is  valuable, need't judge type, because it just allow int
    {
        cout<<"\nerror! function call parameter does not match in "<<(node->childs)[0]->value<<endl;
        out_mcode.close();
        exit(1);
    }

    if(dnode->type==f_int)                                  //set stack space to receive return value
    {
        out_mcode<<s_stack_change<<"\tstack - 2"<<endl;
    }

    out_mcode<<s_stack_all<<"\tpush_all"<<endl;                          //save all register

    for(vector<string>::reverse_iterator iter=fun_args.rbegin(); iter!=fun_args.rend(); ++iter)
    {                                                       //push arguments reversely
        out_mcode<<s_push_pop<<"\tpush "<<*iter<<endl;
    }

    out_mcode<<s_call_ret<<"\tcall "<<(node->childs)[0]->value<<endl;   //call function

    out_mcode<<s_stack_change<<"\tstack + "<<itoa(2*fun_args.size())<<endl; //recycle space used to transmit arguments

    out_mcode<<s_stack_all<<"\tpop_all"<<endl;                           //get all register

    if(dnode->type==f_int)                                  //pop to get return value
    {
        out_mcode<<s_push_pop<<"\tpop reg"<<itoa(++registerNO)<<endl;
        node->value="reg"+string(itoa(registerNO));
    }
}

void f_args(Tree_node* node)                        //args's value is the number of parameters
{
    if((node->childs)[0]->type=="arg-list")
    {
        f_arg_list((node->childs)[0]);
    }
    else if((node->childs)[0]->type=="epsilon")
    {

    }
    else
    {

    }
}

void f_arg_list(Tree_node* node)                    //arg-list's value is the number of parameters
{
    switch((node->childs).size())
    {
    case 1 :
        f_expression((node->childs)[0]);
        fun_args.push_back((node->childs)[0]->value);
        break;

    case 3 :
        f_arg_list((node->childs)[0]);
        f_expression((node->childs)[2]);
        break;

    default:
        break;
    }
}

























