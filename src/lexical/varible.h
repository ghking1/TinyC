#ifndef VARIBLE_H
#define VARIBLE_H
#include <map>
#include <iostream>
#include <stack>
#include <set>
#include <fstream>

#define DEBUG_1

/*
    in regular express: '@' represent all letter, '$' represent all digital, '\' represent transmit meaning,
                        '#' represent the end of express, '|' '.' '^' '[' ']' are operations
    in NFA:             '\' represent epsilon
*/

class NFA_node;
class DFA_node;

typedef enum{none, start, middle, finish} attr;              //stateAttr
typedef NFA_node *NFA;
typedef DFA_node *DFA;
typedef std::multimap<char, NFA_node*> NFAstateChangeTable;
typedef std::map<char, DFA_node*> DFAstateChangeTable;

const int reservedKeyNumber=9;
char* const reservedKey[reservedKeyNumber]={"if", "else", "while", "break", "return", "void", "int", "real", "point"};

const char operatorRelationTable[6][6]=     //(left to right) and (top to bottom) is '|' '.' '*' '[' ']' '#'
{
    {'>', '<', '<', '<', '>', '>'},
    {'>', '>', '<', '<', '>', '>'},
    {'>', '>', '>', '<', '>', '>'},
    {'<', '<', '<', '<', '=', '!'},
    {'>', '>', '>', '!', '>', '>'},
    {'<', '<', '<', '<', '!', '='}
};

class NFA_node
{
public:
    NFA_node();
    NFA_node(attr);

public:
    int stateNO;
    attr stateAttr;                          //none, start, middle, finish
    bool traverse;
    NFAstateChangeTable stateChangeTable;    //next state points
};

class DFA_node
{
public:
    DFA_node();
    DFA_node(attr);

public:
    int stateNO;
    attr stateAttr;                         //none, start, middle, finish
    bool traverse;
    std::set<NFA_node*> NFA_State_Ps;       //this DFA include which NFA state
    DFAstateChangeTable stateChangeTable;   //next state points
};

template<typename T, typename D>        //output DNF or NFA
void show(T A)
{
    std::stack<T> will_visit;
    std::stack<T> changed_node;
    T tmp;
    will_visit.push(A);
    A->traverse=true;
    changed_node.push(A);
    while(!will_visit.empty())
    {
        tmp=will_visit.top();
        will_visit.pop();
        std::cout<<tmp->stateNO<<","<<tmp->stateAttr;

        for(D iter= tmp->stateChangeTable.begin(); iter!=tmp->stateChangeTable.end(); ++iter)
        {
            std::cout<<'\t'<<iter->first<<" --> "<<iter->second->stateNO<<"\n";
            if((iter->second)->traverse == false)
            {
                will_visit.push(iter->second);
                iter->second->traverse = true;
                changed_node.push(iter->second);
            }
        }
        std::cout<<std::endl;
    }

    while(!changed_node.empty())
    {
        (changed_node.top())->traverse=false;
        changed_node.pop();
    }
}

template<typename T, typename D>             //delete DFA or NFA
void deleteChart(T A)
{
    std::stack<T> will_visit;
    std::stack<T> changed_node;
    T tmp;
    will_visit.push(A);
    A->traverse=true;
    changed_node.push(A);
    while(!will_visit.empty())
    {
        tmp=will_visit.top();
        will_visit.pop();

        for(D iter= tmp->stateChangeTable.begin(); iter!=tmp->stateChangeTable.end(); ++iter)
        {
            if((iter->second)->traverse == false)
            {
                will_visit.push(iter->second);
                iter->second->traverse = true;
                changed_node.push(iter->second);
            }
        }
    }

    while(!changed_node.empty())
    {
        delete changed_node.top();
        changed_node.pop();
    }
}

template<typename T, typename D>        //distribute state number for DFA or NFA
void setStateNO(T A)
{
    std::stack<T> will_visit;
    std::stack<T> changed_node;
    T tmp;
    will_visit.push(A);
    A->traverse=true;
    changed_node.push(A);
    int i=0;

    while(!will_visit.empty())
    {
        tmp=will_visit.top();
        will_visit.pop();
        tmp->stateNO = ++i;

        for(D iter= tmp->stateChangeTable.begin(); iter!=tmp->stateChangeTable.end(); ++iter)
        {
            if((iter->second)->traverse == false)
            {
                will_visit.push(iter->second);
                iter->second->traverse = true;
                changed_node.push(iter->second);
            }
        }
    }

    while(!changed_node.empty())
    {
        (changed_node.top())->traverse=false;
        changed_node.pop();
    }
}

#endif // VARIBLE_H
