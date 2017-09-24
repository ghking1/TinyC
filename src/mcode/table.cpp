#include <iostream>
#include <vector>
#include <string>
#include "mcode.h"

using namespace std;

extern vector<vector<declaration_node> > declaration_table; //just as what the name indicated
extern int stack_deepth;        //used to indicate where the variable is in the stack

bool is_repetition(declaration_node& node)  //judge if this declaration node is repetition
{
    declaration_node * dnode=get_declaration_node(node.name);  //get the declaration node the same name as current name in declaration table
    if(dnode!=NULL && (dnode->type==f_int || dnode->type==f_void))  //all name should't  conflict with function name
    {
        return true;
    }
    vector<declaration_node>::reverse_iterator iter;    //if not conflict with function name, then judge if it is conflict with other variable name in current scope
    for(iter=declaration_table.rbegin()->rbegin(); iter!=declaration_table.rbegin()->rend(); ++iter)
    {
        if(iter->name==node.name)
        {
            return true;
        }
    }
    return false;
}

node_type get_node_type(string type)    //transmit string to node_type, so easy!
{
    if(type=="v_int")               {return v_int;}
    else if(type=="v_void")         {return none;}
    else if(type=="f_int")          {return f_int;}
    else if(type=="f_void")         {return f_void;}
    else if(type=="a_int")          {return a_int;}
    else if(type=="a_void")         {return none;}
    else                            {return none; }
}

declaration_node* get_declaration_node(string ID)   //find declaration node in declaration table reversely
{
    vector<vector<declaration_node> >::reverse_iterator  iter;
    vector<declaration_node>::reverse_iterator iter1;
    for(iter=declaration_table.rbegin(); iter!=declaration_table.rend(); ++iter)    //go every scope
    {
        for(iter1=iter->rbegin(); iter1!=iter->rend(); ++iter1)                     //go every declaration node
        {
            if(iter1->name==ID)
            {
                goto next;
            }
        }
    }

next:
    if(iter==declaration_table.rend())          //has no declaration node with name of ID
    {
        return NULL;
    }
    else                                        //find! return it's point
    {
        if(iter==declaration_table.rend()-1)
        {
            stack_deepth=0;
        }
        return &(*iter1);
    }
}

char* itoa(unsigned int number)     //transmit int to string, so easy!
{
    static char alpha[10];  //save string after transmitted

    if(number==0)           //if number is 0, return directly
    {
        alpha[0]='0';
        alpha[1]='\0';
        return alpha;
    }

    int length=0;           //how long is the string after transmitted
    char tmp[10];
    for(int i=0; number!=0; ++i)    //get every bit of number
    {
        ++length;
        tmp[i]=number%10+48;
        number/=10;
    }

    for(int i=0; i<length; ++i)     //save every bit of number
    {
        alpha[i]=tmp[length-i-1];
    }
    alpha[length]='\0';

    return alpha;
}
































