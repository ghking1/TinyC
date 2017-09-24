#include <iostream>
#include <fstream>
#include <list>
#include <queue>
#include "mcode.h"
#include <cstdlib>
#include <cstring>

#define DEBUGER_buildTree

using namespace std;

vector<vector<declaration_node> > declaration_table;

Tree_node* buildTree(ifstream& tree_in)                 //rebuild tree from file
{
    string father;                                      //father nodeNO
    int nodeNO_count=0;
    Tree_node *root_node, *new_node, *cur;
    queue<Tree_node*> wait;

    root_node=new Tree_node();                         //build root node
    tree_in>>father;
    root_node->nodeNO=++nodeNO_count;
    tree_in>>root_node->type;
    tree_in>>root_node->value;

    bool father_valid=false;                        //if father variable is valid
    wait.push(root_node);
    while(!wait.empty())
    {
        cur=wait.front();
        wait.pop();

        while(tree_in)                              //build child node
        {
            if(father_valid==false)                 //if father variable is valid already, then need not read new one
            {
                tree_in>>father;
                if(!tree_in) { break;}
                father_valid=true;
            }

            if(atoi(father.c_str())==cur->nodeNO)   //if new_node's father nodeNO equal to cur->nodeNO, add it to cur->child
            {
        		new_node=new Tree_node();
        		new_node->nodeNO=++nodeNO_count;
        		tree_in>>new_node->type;
        		tree_in>>new_node->value;
                father_valid=false;
                if(!tree_in) { break;}

        		cur->childs.push_back(new_node);
        		wait.push(new_node);
            }
            else
            {
                break;
            }
        }
    }

#ifndef DEBUGER_buildTree
{
    Tree_node* cur;
    queue<Tree_node*> wait;
    wait.push(root_node);
    while(!wait.empty())
    {
        cur=wait.front();
        wait.pop();
        cout<<"*****************************"<<endl;
        cout<<cur->type<<":"<<cur->value<<endl;
        for(vector<Tree_node*>::iterator iter=cur->childs.begin(); iter!=cur->childs.end(); ++iter)
        {
            wait.push(*iter);
        }
    }
}
#endif // DEBUGER_buildTree

return root_node;
}


int main()
{
    ifstream tree_in("tree.txt");
    Tree_node* grammar_tree=buildTree(tree_in); //rebuild grammar tree
    tree_in.close();

    f_program((grammar_tree->childs)[0]);       //start grammar analysis from root


    //*****************release memory: tree
{
    queue<Tree_node*> wait, used;           //delete tree
    Tree_node *cur;
    wait.push(grammar_tree);
    used.push(grammar_tree);
    while(!wait.empty())
    {
        cur=wait.front();
        wait.pop();
        for(vector<Tree_node*>::iterator iter=cur->childs.begin(); iter!=cur->childs.end(); ++iter)
        {
            wait.push(*iter);
            used.push(*iter);
        }
    }
    while(!used.empty())
    {
        cur=used.front();
        used.pop();
        delete cur;
    }
}

    return 0;
}
