#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <map>
#include <stack>
#include <cctype>
#include <set>
#include "varible.h"

using namespace std;

NFA_node::NFA_node():stateNO(0), stateAttr(none), traverse(false)
{
}

NFA_node::NFA_node(attr a):stateNO(0), stateAttr(a), traverse(false)
{
}

NFA_node* getFinish(NFA A)          //find the only finish state in NFA
{
    stack<NFA_node*> will_visit;
    stack<NFA_node*> changed_node;
    NFA_node *tmp, *finish_node;
    will_visit.push(A);
    A->traverse=true;
    changed_node.push(A);
    while(!will_visit.empty())
    {
        tmp=will_visit.top(); will_visit.pop();
        if(tmp->stateAttr == finish)
        {
            finish_node=tmp;
            break;
        }

        for(NFAstateChangeTable::iterator iter= tmp->stateChangeTable.begin(); iter!=tmp->stateChangeTable.end(); ++iter)
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
    return finish_node;
}

NFA closure_N(NFA A)        //compute '^'
{
    NFA_node *finish_node=getFinish(A);
    NFA_node *a=new NFA_node(start);
    NFA_node *b=new NFA_node(finish);
    a->stateChangeTable.insert(make_pair('\\', A));
    a->stateChangeTable.insert(make_pair('\\', b));
    finish_node->stateChangeTable.insert(make_pair('\\', A));
    finish_node->stateChangeTable.insert(make_pair('\\', b));
    A->stateAttr=middle;
    finish_node->stateAttr=middle;
    return a;
}

NFA or_N(NFA A, NFA B)        //compute '|'
{
    NFA_node *A_finish_node=getFinish(A);
    NFA_node *B_finish_node=getFinish(B);
    NFA_node *a=new NFA_node(start);
    NFA_node *b=new NFA_node(finish);
    a->stateChangeTable.insert(make_pair('\\', A));
    a->stateChangeTable.insert(make_pair('\\', B));
    A_finish_node->stateChangeTable.insert(make_pair('\\', b));
    B_finish_node->stateChangeTable.insert(make_pair('\\', b));
    A->stateAttr=middle;
    A_finish_node->stateAttr=middle;
    B->stateAttr=middle;
    B_finish_node->stateAttr=middle;
    return a;
}

NFA and_N(NFA A, NFA B)        //compute '.'
{
    NFA_node *A_finish_node=getFinish(A);
    A_finish_node->stateChangeTable.insert(make_pair('\\', B));
    A_finish_node->stateAttr=middle;
    B->stateAttr=middle;
    return A;
}

NFA compute_NFA(char operator_, NFA A, NFA B)       //according to the operator chose the function compute A and B
{
    switch (operator_)
    {
    case '^':
        return closure_N(A);
        break;
    case '.':
        return and_N(A, B);
        break;
    case '|':
        return or_N(A, B);
        break;
    default:
        cerr<<operator_<<"is: invalid operator!\n";
        exit(1);
        break;
    }
}

NFA charToNFA(char operand)                         // transmit char to NFA, in order it can be push into NFA stack
{
    NFA_node *a=new NFA_node(start);
    NFA_node *b=new NFA_node(finish);
    a->stateChangeTable.insert(make_pair(operand, b));
    return a;
}

char compareOperators(char A, char B)       //compareOperators according to operatorRelationTable in varible.h
{
    char charTemp, intTemp, Aint, Bint;
    for(int i=0; i<2; ++i)
    {
        i==0 ? charTemp=A : charTemp=B;     //i==0 get rows of A, i==1 get columns of B
        switch (charTemp)
        {
        case '|':
            intTemp=0;
            break;
        case '.':
            intTemp=1;
            break;
        case '^':
            intTemp=2;
            break;
        case '[':
            intTemp=3;
            break;
        case ']':
            intTemp=4;
            break;
        case '#':
            intTemp=5;
            break;
        default:
            break;
        }
        i==0 ? Aint=intTemp : Bint=intTemp;
    }
    return operatorRelationTable[Aint][Bint];   // return the relationship of tow operators
}

NFA createNFA(ifstream& char_in, set<char>& acceptableChar)
{
    stack<NFA> operands;
    stack<char> operators;
    operators.push('#');

    char c, operatorTemp;             // the former used to save input char, the latter used to save the char popped from stack
    NFA operandATemp, operandBTemp;
    char_in>>c;
    while(c!='#' || operators.top()!='#')
    {
        if(c==' ' || c=='\n' ||c=='\r' || c=='\t')                        //ignore blank
        {
            char_in>>c;
            continue;
        }
        else if(c=='\\')                                                   // '\' is transmit meaning symbol
        {
            char_in>>c;     //get next char after '\', and use it as operand anyhow
            operands.push(charToNFA(c));
            acceptableChar.insert(c);
            char_in>>c;
        }
        else if(c!='|' && c!='.' && c!='^' && c!='[' && c!=']' && c!='#') //input char is not operator of regular express
        {
            operands.push(charToNFA(c));    //push into operands
            acceptableChar.insert(c);       //insert into acceptableChar
            char_in>>c;
        }
        else                                                        //is operator of regular express, then chose suitable operation
        {
            switch(compareOperators(operators.top(), c))            //reverse Poland algorithm
            {
            case '<':
                operators.push(c);
                char_in>>c;
                break;
            case '=':
                operators.pop();
                char_in>>c;
                break;
            case '>':
                operatorTemp=operators.top(); operators.pop();
                operandATemp=operands.top(); operands.pop();
                if(operatorTemp=='^')   //'^' need single operand
                {
                    operands.push(compute_NFA(operatorTemp, operandATemp, operandATemp));
                }
                else                    // '.' and '|' need double operands
                {
                    operandBTemp=operands.top(); operands.pop();
                    operands.push(compute_NFA(operatorTemp, operandBTemp, operandATemp));
                }
                break;
            default:
                break;
            }//switch
        }//if-else
    }//while

    setStateNO<NFA, NFAstateChangeTable::iterator>(operands.top());
    return operands.top();
}

NFA_node* getFinish(NFA A);

DFA_node::DFA_node():stateNO(0), stateAttr(none), traverse(false)
{
}

DFA_node::DFA_node(attr a):stateNO(0), stateAttr(a), traverse(false)
{
}

set<NFA_node*> getEpsilonClosure(NFA_node* A)       //just like go DFA, but only walk epsilon path
{
    stack<NFA_node*> will_visit;
    stack<NFA_node*> changed_node;
    NFA_node *tmp;
    set<NFA_node*> epsilonClosure;
    will_visit.push(A);
    A->traverse=true;
    changed_node.push(A);
    while(!will_visit.empty())
    {
        tmp=will_visit.top();
        will_visit.pop();
        epsilonClosure.insert(tmp);

        for(NFAstateChangeTable::iterator iter= tmp->stateChangeTable.begin(); iter!=tmp->stateChangeTable.end(); ++iter)
        {
            if((iter->first)=='\\' && (iter->second)->traverse == false)
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

    return epsilonClosure;
}

set<DFA_node*> nextLevel(DFA_node* N, set<char>& acceptableChar, set<DFA_node*>& DFA_nodes, int finishNO)
{
    set<NFA_node*> tmp;
    set<DFA_node*> new_DFA_nodes;
    for(set<char>::iterator iter1=acceptableChar.begin(); iter1!=acceptableChar.end(); ++iter1)              //go every acceptableChar
    {
        DFA_node *newDFA_node=new DFA_node;     //create new DFA_node
        for(set<NFA_node*>::iterator iter2= N->NFA_State_Ps.begin(); iter2!=N->NFA_State_Ps.end(); ++iter2)  //go every NFA state included in current DFA_node
        {
            for(NFAstateChangeTable::iterator iter3= (*iter2)->stateChangeTable.lower_bound(*iter1); iter3 != (*iter2)->stateChangeTable.upper_bound(*iter1); ++iter3)
                                                                                                            //go every change to next state path in NFA_node
            {
                tmp=getEpsilonClosure(iter3->second);
                newDFA_node->NFA_State_Ps.insert(tmp.begin(), tmp.end());
            }
        }

        if(!newDFA_node->NFA_State_Ps.empty())  //not null indicated: at current DFA_node, use current input can goto new DFA_node
        {
            set<DFA_node*>::iterator iter4;
            set<NFA_node*>::iterator iter5;
            for(iter4=DFA_nodes.begin(); iter4!=DFA_nodes.end(); ++iter4)                               //find in all DFA_node has been created currently, whether the new node is repetition
            {
                for(iter5=(*iter4)->NFA_State_Ps.begin(); iter5!=(*iter4)->NFA_State_Ps.end(); ++iter5) //how to judge two node is the same? just need judge whether NFA_State_Ps is the same
                {
                    if((*iter4)->NFA_State_Ps.size() != newDFA_node->NFA_State_Ps.size()){break;}
                    if(newDFA_node->NFA_State_Ps.find(*iter5)==newDFA_node->NFA_State_Ps.end()) {break;}
                }
                if(iter5==(*iter4)->NFA_State_Ps.end()) {break;}
            }

            if(iter4!=DFA_nodes.end())        //indicated new DFA_node is repetition
            {
                delete newDFA_node;
                newDFA_node=*iter4;
            }
            else                             //indicated new DFA_node is not repetition
            {
                set<NFA_node*>::iterator iter6; //set new DFA_node's stateAttr, how to judge it?  think yourself!
                for(iter6=newDFA_node->NFA_State_Ps.begin(); iter6 != newDFA_node->NFA_State_Ps.end(); ++iter6)
                {
                    if((*iter6)->stateNO == finishNO) break;
                }
                if(iter6!=newDFA_node->NFA_State_Ps.end())
                {
                    newDFA_node->stateAttr=finish;
                }
                else
                {
                    newDFA_node->stateAttr=middle;
                }

                DFA_nodes.insert(newDFA_node);               //add new DFA_node to all DFA_node created already
                new_DFA_nodes.insert(newDFA_node);           //add new DFA_node to new_DFA_nodes
            }
            N->stateChangeTable.insert(make_pair(*iter1, newDFA_node));//insert one change path point to new_node to cur DFA_node
        }
        else                                //null indicated: at current DFA_node, use current input can't goto new DFA_node
        {
            delete newDFA_node;
        }
    }
    return new_DFA_nodes;       //return all new_node created this time, also is all node can be reached directly from cur DFA_node
}

DFA createDFA(NFA N, set<char>& acceptableChar)
{
    DFA newDFA=new DFA_node(start);             //generate start node of DFA
    newDFA->NFA_State_Ps=getEpsilonClosure(N);

    set<DFA_node*> DFA_nodes, new_DFA_nodes;    //all DFA_node created already and new created DFA_node
    DFA_nodes.insert(newDFA);

    DFA_node *tmp;
    int finishNO=getFinish(N)->stateNO;         //get the only finish state number in NFA
    stack<DFA_node*> will_handle;
    will_handle.push(newDFA);
    while(!will_handle.empty())
    {
        tmp=will_handle.top();
        will_handle.pop();
        new_DFA_nodes=nextLevel(tmp, acceptableChar, DFA_nodes, finishNO);
        for(set<DFA_node*>::iterator iter=new_DFA_nodes.begin(); iter!=new_DFA_nodes.end(); ++iter)//push all new DFA_node into stack
        {
            will_handle.push(*iter);
        }
    }
    setStateNO<DFA, DFAstateChangeTable::iterator>(newDFA); //distribute state number for DFA
    return newDFA;
}

/*void DFA_to_Matrix(map<int, int*>& M, DFA A, set<char>& acceptableChar)
{
    stack<DFA_node*> will_visit;
    stack<DFA_node*> changed_node;
    DFA_node* tmp;
    will_visit.push(A);
    A->traverse=true;
    changed_node.push(A);
    int j=1;
    while(!will_visit.empty())
    {
        tmp=will_visit.top();
        will_visit.pop();
        int *nextStates=new int[acceptableChar.size()];
        set<char>::iterator iter;
        int i;
        for(iter=acceptableChar.begin(), i=0; iter!=acceptableChar.end(); ++iter, ++i)
        {
            if(tmp->stateChangeTable.find(*iter)==tmp->stateChangeTable.end())
            {
                nextStates[i]=0;
            }
            else
            {
                nextStates[i]=tmp->stateChangeTable.find(*iter)->second->stateNO;
            }
        }
        M.insert(make_pair(j++, nextStates));

        for(DFAstateChangeTable::iterator iter= tmp->stateChangeTable.begin(); iter!=tmp->stateChangeTable.end(); ++iter)
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

DFA minimalDFA(DFA D, set<char>& acceptableChar)
{
    map<int, int*> Matrix;
    DFA_to_Matrix(Matrix, D, acceptableChar);

#ifndef DEBUG_1
    cout<<endl;
    for(map<int, int*>::iterator iter=Matrix.begin(); iter!=Matrix.end(); ++iter)
    {
        cout<<iter->first<<"\t";
        for(int i=0; i<acceptableChar.size(); ++i)
        {
            cout<<iter->second[i]<<" ";
        }
        cout<<endl;
    }
#endif

    vector<vector<int> > tmp1;

    bool isMinimal=false;
    while(!isMinimal)
    {
        for(set<char>::iterator iter=acceptableChar.begin(); iter!=acceptableChar.end(); ++iter)
        {

        }
    }
    return NULL;
}

*/

bool isReservedKey(const string& newToken)      //judge if is reserved key according to reservedKey in varible.h
{
    for(int i=0; i<reservedKeyNumber; ++i)
    {
        if(!newToken.compare(reservedKey[i]))
        {
            return true;
        }
    }
    return false;
}

void outPutToken(const string& newToken, ofstream& token_out)
{
    if(newToken.size() != 0)
    {
        if(isdigit(newToken[0]))              //is number
        {
            token_out<<"NUM\t"<<newToken<<endl;
        }
        else if(isReservedKey(newToken) || !isalpha(newToken[0]))      //is reserved key
        {
            token_out<<newToken<<"\t"<<newToken<<endl;
        }
        else                                  //is special key
        {
            token_out<<"ID\t"<<newToken<<endl;
        }
    }
}

DFA_node* currentStateCanAccept(DFA_node* currentState, char c)     //just use input char go in DFA, so easy!
{
    if(currentState->stateChangeTable.find(c) != currentState->stateChangeTable.end())                  //can be find directly
    {
        return currentState->stateChangeTable.find(c)->second;
    }
    if(isdigit(c) && currentState->stateChangeTable.find('$') != currentState->stateChangeTable.end())  //don't forget '$' represent all number
    {
        return currentState->stateChangeTable.find('$')->second;
    }
    if(isalpha(c) && currentState->stateChangeTable.find('@') != currentState->stateChangeTable.end())  //don't forget '@' represent all letter
    {
        return currentState->stateChangeTable.find('@')->second;
    }
    return NULL;
}


int main(int argc, char* args[])
{
    if(argc != 2 )
    {
        exit(1);
    }

    ifstream char_in("RE.txt");
    if(!char_in) {cerr<<"cann't open RE.txt!\n";}
    set<char> acceptableChar;                                 //valid char
    NFA myNFA=createNFA(char_in, acceptableChar);             //create NFA
    char_in.close();

    cout<<"acceptableChar: ";                                                    //output acceptableChar
    for(set<char>::iterator iter=acceptableChar.begin(); iter!=acceptableChar.end(); ++iter)
    {
        cout<<*iter<<" ";
    }
    cout<<"\n\nNFA chart\nstateNO:stateAttr\tinputChar-->nextState"<<endl;      //output every NFA node
    show<NFA, NFAstateChangeTable::iterator>(myNFA);

    DFA myDFA=createDFA(myNFA, acceptableChar);             //create DFA
    cout<<"\nDFA chart\nstateNO:stateAttr\tinputChar-->nextState"<<endl;        //output every DFA node
    show<DFA, DFAstateChangeTable::iterator>(myDFA);

    //DFA myMinimalDFA=minimalDFA(myDFA, acceptableChar);   //minimalDFA
    //cout<<"\nminimal DFA chart\nstateNO:stateAttr\tinputChar-->nextState"<<endl;

    ifstream program_in(args[1]);                                         //open program file and create token file
    if(!program_in) {cerr<<"can't open file!\n";}
    ofstream token_out("token.txt");
    if(!token_out) {cerr<<"can't create token.txt!\n";}

    char c;                 //used to save single char from program file
    string newToken;
    program_in.get(c);      //get single char from program file
    while(program_in)       //reach end of file, then stop
    {
        if(c==' ' || c=='\n' ||c=='\r' || c=='\t')  //ignore blank char
        {
            program_in.get(c);
            continue;
        }

        newToken="";
        DFA_node *currentState=myDFA, *tmp; //start of DFA state
        while(program_in)    //reach end of file, then stop
        {
            if((tmp=currentStateCanAccept(currentState, c)) != NULL)  //input the char and go it in DFA from current state
            {
                currentState=tmp;           //set new state after input new char
                newToken.append(1, c);
                program_in.get(c);
            }
            else
            {
                if(currentState->stateAttr != finish)   //error
                {
                    newToken.append(1, c);
                    cout<<newToken<<": is invalid!"<<endl;
                    goto close;
                }
                else                                    //is finish state, accept it
                {
                    outPutToken(newToken, token_out);
                    break;
                }
            }

            if(!program_in)                             //attention! when reach the end of file, the lase token has not be handled, handle it here
            {
                if(currentState->stateAttr != finish)   //error, ignore it
                {
                    cout<<newToken<<": is invalid!"<<endl;
                    goto close;
                }
                else                                    //is finish state, accept it
                {
                    outPutToken(newToken, token_out);
                    break;
                }
            }
        }
    }
    token_out<<"\n$\t$";

close:
    program_in.close();
    token_out.close();
    deleteChart<NFA, NFAstateChangeTable::iterator>(myNFA);
    deleteChart<DFA, DFAstateChangeTable::iterator>(myDFA);
    return 0;
}

















