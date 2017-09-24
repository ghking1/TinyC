#ifndef NEED_INCLUDED
#define NEED_INCLUDED

#include <vector>
#include <map>
#include <string>

class Sentence
{
public:
    std::string VN;                      //non-terminal symbol
    std::vector<std::string> V;               //symbol set
};

class LR0_Sentence
{
public:
    std::string VN;                      //non-terminal symbol
    std::vector<std::string> V;
};

class LR1_Sentence
{
public:
    std::string VN;                      //non-terminal symbol
    std::vector<std::string> V;               //symbol set
    std::string preceding;               //preceding symbol
};

class LR1_node
{
public:
    int stateNO;                             //state number
    bool traverse;                           //traversed flag
    std::vector<LR1_Sentence> lr1_sentences;          //save DFA LR1_node's sentences;
    std::map<std::string, LR1_node*> stateChangeTable; //input string, goto LR1_node*'s state
};

class Cell
{
public:
    char type;  //'n'-->no type, 's'-->shift, 'r'-->reduce
    int id;     //goto which node or use which sentence

public:
    Cell(): type('n'), id(-1)
    {

    }
};

class Token
{
public:
    std::string type;       //identifier or number or others, also 'error' means not valuable
    std::string value;      //real value
};

class Tree_node
{
public:
    int nodeNO;            //it will be distributed by width
    int fatherNO;
    std::string type;      //identifier or number or others, also 'error' means not valuable
    std::string value;     //real value
    std::vector<Tree_node*> childs;
    bool traverse;

public:
    Tree_node(): nodeNO(0), type(""), value(""), traverse(false)
    {

    }

    Tree_node(std::string _type, std::string _value): type(_type), value(_value), traverse(false)
    {

    }
};

class Stack_node
{
public:
     Tree_node* tree_node_p;
     int state;              //state id
     std::string symbol;     //symbol

public:
    Stack_node(): tree_node_p(NULL), state(-1), symbol("")
    {

    }
};


#endif // NEED_INCLUDED































