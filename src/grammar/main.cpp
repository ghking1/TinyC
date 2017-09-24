#include <iostream>
#include <fstream>
#include "need.h"
#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <utility>
#include <stack>
#include <queue>

#define DEBUGER_GETGRAMMER
#define DEBUGER_GENERATEFIRST
#define DEBUGER_GENERATEFOLLOW
#define DEBUGER_GENERATEDFA
#define DEBUGER_generateTree
#define DEBUGER_GENERATECHART

using namespace std;

vector<Sentence> grammar;                                 //save all grammar sentence
vector<LR0_Sentence> LR0_grammar;                         //save all LR(0) sentence
set<string> nonTerminal_Symbol;
set<string> terminal_Symbol;
set<string> all_Symbol;
map<string, set<string> > first_set, follow_set;
int nodeNumber;


void getGrammar(ifstream& in)
{
    Sentence newSentence;
    newSentence.VN="S'";                        //put S' --> S into grammar
    newSentence.V.push_back("S");
    grammar.push_back(newSentence);
    nonTerminal_Symbol.insert(newSentence.VN);  //add S' into nonTerminal_Symbol

    LR0_Sentence newLR0_Sentence;
    newLR0_Sentence.VN="S'";                    //put S' --> .S and S' -->S. into LR0_Grammar
    newLR0_Sentence.V.push_back(".");
    newLR0_Sentence.V.push_back("S");
    LR0_grammar.push_back(newLR0_Sentence);
    newLR0_Sentence.V.clear();
    newLR0_Sentence.V.push_back("S");
    newLR0_Sentence.V.push_back(".");
    LR0_grammar.push_back(newLR0_Sentence);


    string cur_v;
    while(in)           //
    {
        cur_v="";
        in>>newSentence.VN;
        nonTerminal_Symbol.insert(newSentence.VN);              //add into nonTerminal_Symbol
        while(in && cur_v != "#")                               //'#' means new line or new sentence
        {
            newSentence.V.clear();                              //read and put grammar into grammar
            while(in>>cur_v && (cur_v!="|") && (cur_v!="#"))    //here '|' means an selection, '#' means in sentence tail
            {
                newSentence.V.push_back(cur_v);
                terminal_Symbol.insert(cur_v);                  //add into terminal_Symbol
            }
            grammar.push_back(newSentence);

            newLR0_Sentence.VN=newSentence.VN;                  //generate LR0_Grammar from grammar sentence and put it into LR0_Grammar
            for(int i=0; i<=newSentence.V.size(); ++i)          //every grammar sentence can generate newSentence.V.size() LR0 sentence
            {
                newLR0_Sentence.V.clear();
                int j=0;
                for(vector<string>::iterator iter=newSentence.V.begin(); iter!= newSentence.V.end(); ++iter, ++j)
                {
                    if(j==i)                                    //j and i control add point where
                    {
                        newLR0_Sentence.V.push_back(".");
                    }
                    newLR0_Sentence.V.push_back(*iter);
                }
                if(i==newSentence.V.size())                     //the last LR0_Sentence should be treat carefully
                {
                    newLR0_Sentence.V.push_back(".");
                }
                LR0_grammar.push_back(newLR0_Sentence);
            }
        }
    }

    for(set<string>::iterator iter=nonTerminal_Symbol.begin(); iter!=nonTerminal_Symbol.end(); ++iter)  //erase all nonTerminal symbol from terminal_Symbol
    {
        terminal_Symbol.erase(*iter);
    }

    for(set<string>::iterator iter=nonTerminal_Symbol.begin(); iter!=nonTerminal_Symbol.end(); ++iter)  //add nonTerminal symbol to all terminal symbol
    {
        all_Symbol.insert(*iter);
    }
    for(set<string>::iterator iter=terminal_Symbol.begin(); iter!=terminal_Symbol.end(); ++iter)        //add terminal symbol to all terminal symbol
    {
        all_Symbol.insert(*iter);
    }
    all_Symbol.insert("$");                                                                             //add '$' to all symbol


#ifndef DEBUGER_GETGRAMMER
    cout<<"\n\ngrammar:"<<endl;
    for(vector<Sentence>::iterator iter=grammar.begin(); iter!=grammar.end(); ++iter)
    {
        cout<< iter->VN << "\t";
        for(vector<string>::iterator iter2=iter->V.begin(); iter2!=iter->V.end(); ++iter2)
        {
            cout<< *iter2 <<" ";
        }
        cout<<endl;
    }
    cout<<"\n\nLR0_Grammar:"<<endl;
    for(vector<LR0_Sentence>::iterator iter=LR0_grammar.begin(); iter!=LR0_grammar.end(); ++iter)
    {
        cout<< iter->VN << "\t";
        for(vector<string>::iterator iter2=iter->V.begin(); iter2!=iter->V.end(); ++iter2)
        {
            cout<< *iter2 <<" ";
        }
        cout<<endl;
    }
    cout<<"\n\nnonTerminal symbol:"<<endl;
    for(set<string>::iterator iter=nonTerminal_Symbol.begin(); iter!=nonTerminal_Symbol.end(); ++iter)
    {
        cout<<*iter<<" ";
    }
    cout<<"\n\nTerminal symbol:"<<endl;
    for(set<string>::iterator iter=terminal_Symbol.begin(); iter!=terminal_Symbol.end(); ++iter)
    {
        cout<<*iter<<" ";
    }
    cout<<"\n\nall symbol:"<<endl;
    for(set<string>::iterator iter=all_Symbol.begin(); iter!=all_Symbol.end(); ++iter)
    {
        cout<<*iter<<" ";
    }
#endif // DEBUGER_GETGRAMMER

}

void generateFirst()
{
    for(set<string>::iterator iter=nonTerminal_Symbol.begin(); iter!=nonTerminal_Symbol.end(); ++iter) //let all nonterminal symbol has first
    {
        set<string> a;
        first_set.insert(make_pair(*iter, a));
    }

    int i, j;
    bool stop=false;                //control the big circle whether should be stopped
    while(stop==false)
    {
        stop=true;
        for(i=0; i<grammar.size(); ++i) //go every grammar sentence
        {
            for(j=0; j<grammar[i].V.size(); ++j)    //go every symbol in the grammar sentence
            {
                if(nonTerminal_Symbol.find(grammar[i].V[j])==nonTerminal_Symbol.end())              //is terminal symbol add it into set and break
                {
                    if(first_set[grammar[i].VN].find(grammar[i].V[j]) == first_set[grammar[i].VN].end()) //if new symbol is not in, then add it and set stop=false
                    {
                        first_set[grammar[i].VN].insert(grammar[i].V[j]);
                        stop=false;
                    }
                    break;
                }
                else if(first_set[grammar[i].V[j]].find("epsilon")==first_set[grammar[i].V[j]].end())     //epsilon not in it's first_set, add all it's first into set and break
                {
                    for(set<string>::iterator iter=first_set[grammar[i].V[j]].begin(); iter!=first_set[grammar[i].V[j]].end(); ++iter)
                    {
                        if(first_set[grammar[i].VN].find(*iter) == first_set[grammar[i].VN].end())  //if new symbol is not in, then add it and set stop=false
                        {
                            first_set[grammar[i].VN].insert(*iter);
                            stop=false;
                        }
                    }
                    break;
                }
                else                                                                                //it's first_set include epsilon, add all it's first into set
                {
                    for(set<string>::iterator iter=first_set[grammar[i].V[j]].begin(); iter!=first_set[grammar[i].V[j]].end(); ++iter)
                    {
                        if(j==grammar[i].V.size()-1)    //the last symbol of sentence, epsilon can be added
                        {
                            if(first_set[grammar[i].VN].find(*iter) == first_set[grammar[i].VN].end())
                            {
                                first_set[grammar[i].VN].insert(*iter);
                                stop=false;
                            }
                        }
                        else                            //not the last, epsilon can't add into
                        {
                            if((*iter != "epsilon") && (first_set[grammar[i].VN].find(*iter) == first_set[grammar[i].VN].end()))  //if new symbol is not in, then add it and set stop=false
                            {
                                first_set[grammar[i].VN].insert(*iter);
                                stop=false;
                            }
                        }
                    }
                }
            }
        }
    }

#ifndef DEBUGER_GENERATEFIRST
    cout<<"\n\nFirst Set:"<<endl;
    for(map<string, set<string> >::iterator iter=first_set.begin(); iter!=first_set.end(); ++iter)
    {
        cout<< iter->first<<"\t";
        for(set<string>::iterator iter1=iter->second.begin(); iter1!=iter->second.end(); ++iter1)
        {
            cout<< *iter1<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
#endif // DEBUGER_GENERATEFIRST
}

void generateFollow()
{
    for(set<string>::iterator iter=nonTerminal_Symbol.begin(); iter!=nonTerminal_Symbol.end(); ++iter) //let all nonterminal symbol has follow
    {
        set<string> a;
        follow_set.insert(make_pair(*iter, a));
    }
    follow_set["S'"].insert("$");                //add '$' into start symbol's follow

    int i, j, k;
    bool stop=false;
    while(stop==false)
    {
        stop=true;
        for(i=0; i<grammar.size(); ++i)                 //go every grammar sentence
        {
            for(int j=0; j<grammar[i].V.size(); ++j)    //go every symbol in the grammar sentence
            {
                if(nonTerminal_Symbol.find(grammar[i].V[j])!=nonTerminal_Symbol.end())  //only nonTerminal symbol has follow set
                {
                	for(k=j+1; k<grammar[i].V.size()+1; ++k)//go every symbol after current symbol in the grammar sentence
                	{
                	    if(k==(grammar[i].V.size()))        //if from current up the end of sentence means follow_set of VN should be add into it
                	    {
                	        for(set<string>::iterator iter=follow_set[grammar[i].VN].begin(); iter!=follow_set[grammar[i].VN].end(); ++iter)
                	        {
                	            if(follow_set[grammar[i].V[j]].find(*iter)==follow_set[grammar[i].V[j]].end())
                	            {
                	                follow_set[grammar[i].V[j]].insert(*iter);
                	                stop=false;
                	            }
                	        }
                	    }
                	    else                                //or we must add first_set into it except epsilon
                	    {
                	        if(nonTerminal_Symbol.find(grammar[i].V[k])==nonTerminal_Symbol.end())  //is terminal, and it's first set is itself
                            {
                                if(follow_set[grammar[i].V[j]].find(grammar[i].V[k])==follow_set[grammar[i].V[j]].end())
                                {
                                    follow_set[grammar[i].V[j]].insert(grammar[i].V[k]);
                                    stop=false;
                                }
                                break;
                            }
                            else                                                                   //is nonTerminal, and it's first set in first_set
                            {
                                for(set<string>::iterator iter=first_set[grammar[i].V[k]].begin(); iter!=first_set[grammar[i].V[k]].end(); ++iter)
                                {
                                    if((*iter != "epsilon") && (follow_set[grammar[i].V[j]].find(*iter)==follow_set[grammar[i].V[j]].end()))  //if new symbol is not in, then add it and set stop=false
                                    {
                                        follow_set[grammar[i].V[j]].insert(*iter);
                                        stop=false;
                                    }
                                }
                                if(first_set[grammar[i].V[k]].find("epsilon")==first_set[grammar[i].V[k]].end())  //if epsilon is not in, we should break it
                                {
                                    break;
                                }
                            }
                	    }
                	}
                }
            }
        }
    }

#ifndef DEBUGER_GENERATEFOLLOW
    cout<<"\n\nFollow Set:"<<endl;
    for(map<string, set<string> >::iterator iter=follow_set.begin(); iter!=follow_set.end(); ++iter)
    {
        cout<< iter->first<<"\t";
        for(set<string>::iterator iter1=iter->second.begin(); iter1!=iter->second.end(); ++iter1)
        {
            cout<< *iter1<<" ";
        }
        cout<<endl;
    }
     cout<<endl;
#endif // DEBUGER_GENERATEFOLLOW
}

bool notHaveSentense(LR1_node* cur_node, LR1_Sentence& sentence)
{
    for(vector<LR1_Sentence>::iterator iter=cur_node->lr1_sentences.begin(); iter!=cur_node->lr1_sentences.end(); ++iter) //go every sentence
    {
        if(sentence.VN!=iter->VN) { continue;}                  //is VN equal?
        if(sentence.preceding!=iter->preceding) { continue;}    //is preceding equal?
        vector<string>::iterator iter2, iter3;                                //is right part equal?
        for(iter2=iter->V.begin(), iter3=sentence.V.begin(); iter2!=iter->V.end() && iter3!=sentence.V.end(); ++iter2, ++iter3)
        {
            if(*iter2 != *iter3) { break;}
        }
        if(iter2!=iter->V.end() || iter3!=sentence.V.end())
        {
            continue;
        }
        return false;
    }
    return true;
}

void generateClosure(LR1_node* cur_node)
{
    LR1_Sentence newLR1_Sentence;
    bool stop=false;/******should be changed***********/
    while(stop==false)
    {
       stop=true;
       int i=0;
       for( ; i<cur_node->lr1_sentences.size(); ++i) //go every sentence
       {
           for(int j=0; j != cur_node->lr1_sentences[i].V.size(); ++j)      //find '.' and generate closure
           {
               if(cur_node->lr1_sentences[i].V[j] == ".")
               {
                  ++j;
                  if(j==cur_node->lr1_sentences[i].V.size()) {break;}                                                //'.' at the end
                  if(nonTerminal_Symbol.find(cur_node->lr1_sentences[i].V[j])==nonTerminal_Symbol.end()) { break;}          //it's terminal has no closure
                  else                                                                             //it's nonTerminal, may has closure
                  {
                      vector<string> gamma_preceding;                                              //"gamma"+"preceding"
                      set<string> firsts;                                                          //first set of "gamma"+"preceding"

                      for(int k=j+1; k!=cur_node->lr1_sentences[i].V.size(); ++k)   //get "gamma"+"preceding"
                      {
                          gamma_preceding.push_back(cur_node->lr1_sentences[i].V[k]);
                      }
                      gamma_preceding.push_back(cur_node->lr1_sentences[i].preceding);

                      for(vector<string>::iterator iter2=gamma_preceding.begin(); iter2!=gamma_preceding.end(); ++iter2)//generate first set of "gamma"+"preceding"
                      {
                          if(nonTerminal_Symbol.find(*iter2)==nonTerminal_Symbol.end())
                          {
                              firsts.insert(*iter2);
                              break;
                          }
                          else
                          {
                              if(first_set[*iter2].find("epsilon")==first_set[*iter2].end())
                              {
                                  for(set<string>::iterator iter3=first_set[*iter2].begin(); iter3!=first_set[*iter2].end(); ++iter3)
                                  {
                                      firsts.insert(*iter3);
                                  }
                                  break;
                              }
                              else
                              {
                                  for(set<string>::iterator iter3=first_set[*iter2].begin(); iter3!=first_set[*iter2].end(); ++iter3)
                                  {
                                      if(*iter3 != "epsilon")
                                      {
                                          firsts.insert(*iter3);
                                      }
                                  }
                              }
                          }
                      }

                      for(vector<Sentence>::iterator iter2=grammar.begin(); iter2!=grammar.end(); ++iter2) //go every grammar sentence
                      {
                          if(iter2->VN == cur_node->lr1_sentences[i].V[j])                                                          //find nonTerminal equals B
                          {
                              for(set<string>::iterator iter3=firsts.begin(); iter3!=firsts.end(); ++iter3)//go every first symbol and use it as preceding
                              {
                                  newLR1_Sentence.VN=cur_node->lr1_sentences[i].V[j];
                                  newLR1_Sentence.V.clear();
                                  if((iter2->V)[0]=="epsilon")                  //if it is epsilon sentence, '.' put in the end
                                  {
                                        newLR1_Sentence.V.push_back("epsilon");
                                        newLR1_Sentence.V.push_back(".");
                                  }
                                  else                                          //else it is not epsilon sentence, '.' put in the front
                                  {
                                        newLR1_Sentence.V.push_back(".");
                                        for(vector<string>::iterator iter4=iter2->V.begin(); iter4!=iter2->V.end(); ++iter4)
                                        {
                                            newLR1_Sentence.V.push_back(*iter4);
                                        }
                                  }
                                  newLR1_Sentence.preceding=*iter3;
                                  if(notHaveSentense(cur_node, newLR1_Sentence))    //is repetition?
                                  {
                                     cur_node->lr1_sentences.push_back(newLR1_Sentence);
                                     stop=false;
                                  }
                              }
                          }
                      }
                      break;   //because we have find '.', so this sentence have been handled
                   }
               }
           }
       }
    }
}

LR1_node* notHaveNode(vector<LR1_node*>& all_LR1_nodes, LR1_node* node)   //because every node it's core object is not the same so here we just need core sentence
{
    for(vector<LR1_node*>::iterator iter=all_LR1_nodes.begin(); iter!=all_LR1_nodes.end(); ++iter)  //go all node
    {
        vector<LR1_Sentence>::iterator iter1;
        int core_sentence_counter=0;
        for(iter1=(*iter)->lr1_sentences.begin(); iter1!=(*iter)->lr1_sentences.end(); ++iter1)  //go all sentence in iter
        {
            if(iter1->V[0]==".") {continue;}                            //is not core sentence
            ++core_sentence_counter;
            vector<LR1_Sentence>::iterator iter2;
            for(iter2=node->lr1_sentences.begin(); iter2!=node->lr1_sentences.end(); ++iter2)    //go all sentence in node
            {
                if(iter2->VN != iter1->VN) {continue;}                   //is VN equal?
                if(iter2->preceding != iter1->preceding) {continue;}     //is preceding equal?
                vector<string>::iterator iter3, iter4;                   //is right part equal?
                for(iter3=iter1->V.begin(), iter4=iter2->V.begin(); iter3!=iter1->V.end() && iter4!=iter2->V.end(); ++iter3, ++iter4)
                {
                    if(*iter3 != *iter4) { break;}
                }
                if(iter3!=iter1->V.end() || iter4!=iter2->V.end())    //sentence not equal
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            if(iter2!=node->lr1_sentences.end())            //indicated find iter1 in node
            {
                continue;
            }
            else
            {
                break;
            }
        }
        if(iter1==(*iter)->lr1_sentences.end() && core_sentence_counter==node->lr1_sentences.size()) //indicated node equal
        {
            return *iter;
        }
    }
    return NULL;
}

LR1_node* generateDFA()
{
    int stateNO=0, i, j;                                //stateNO used to generating stateNO for every new node
    LR1_Sentence newLR1_Sentence;
    vector<LR1_node*> all_LR1_nodes;
    stack<LR1_node*> wait_LR1_nodes;

    LR1_node *start=new LR1_node();
    start->stateNO=stateNO++;
    start->traverse=false;
    newLR1_Sentence.VN="S'";
    newLR1_Sentence.V.push_back(".");
    newLR1_Sentence.V.push_back("S");
    newLR1_Sentence.preceding="$";
    start->lr1_sentences.push_back(newLR1_Sentence);
    all_LR1_nodes.push_back(start);

    wait_LR1_nodes.push(start);
    LR1_node *cur_node, *tmp_node=NULL, *newLR1_node=NULL;
    while(!wait_LR1_nodes.empty())
    {
        cur_node=wait_LR1_nodes.top();
        wait_LR1_nodes.pop();
        generateClosure(cur_node);

        map<string, LR1_node*> next_level_nodes;
        for(vector<LR1_Sentence>::iterator iter=cur_node->lr1_sentences.begin(); iter!=cur_node->lr1_sentences.end(); ++iter)   //go every sentence
        {
            for(vector<string>::iterator iter1=iter->V.begin(); iter1!=iter->V.end(); ++iter1)  //find '.' the symbol after it should be used as input
            {
                if(*iter1==".")
                {
                    ++iter1;
                    if(iter1==iter->V.end()) {break;}                       //'.' at the end
                    newLR1_Sentence.VN=iter->VN;
                    newLR1_Sentence.V.clear();
                    for(vector<string>::iterator iter2=iter->V.begin(); iter2!=iter->V.end(); ++iter2)  //generate sentence after input the symbol after '.'
                    {
                        if(*iter2 != ".")
                        {
                            newLR1_Sentence.V.push_back(*iter2);
                        }
                        if(iter2==iter1)
                        {
                            newLR1_Sentence.V.push_back(".");
                        }
                    }
                    newLR1_Sentence.preceding=iter->preceding;

                    if(next_level_nodes.find(*iter1)==next_level_nodes.end())   //this input has not appeared before, create new node
                    {
                        newLR1_node=new LR1_node;
                        newLR1_node->lr1_sentences.push_back(newLR1_Sentence);
                        next_level_nodes[*iter1]=newLR1_node;                  //add stateChange point to new node into cur node
                    }
                    else                                                       //this input has appeared before, add sentence to the node generated before
                    {
                        next_level_nodes.find(*iter1)->second->lr1_sentences.push_back(newLR1_Sentence);
                    }
                }
            }
        }

        for(map<string, LR1_node*>::iterator iter=next_level_nodes.begin(); iter!=next_level_nodes.end(); ++iter)
        {
            tmp_node=notHaveNode(all_LR1_nodes, iter->second);            //get node where core sentence has appeared
            if(tmp_node==NULL)                                      //if this core sentence not have appeared then generate new node
            {
                iter->second->stateNO=stateNO++;
                iter->second->traverse=false;
                all_LR1_nodes.push_back(iter->second);                  //add to all
                wait_LR1_nodes.push(iter->second);                      //add to wait
                cur_node->stateChangeTable[iter->first]=iter->second;
            }
            else                                                   //this core sentence has appeared
            {
                cur_node->stateChangeTable[iter->first]=tmp_node;
            }
        }
    }

#ifndef DEBUGER_GENERATEDFA
{
    LR1_node* cur;
    stack<LR1_node*> wait;
    stack<LR1_node*> used;
    start->traverse=true;
    wait.push(start);
    used.push(start);

    cout<<"\n\nLR1_DFA:\n"<<endl;
    while(!wait.empty())
    {
        cur=wait.top();
        wait.pop();
        cout<<"***************"<<cur->stateNO<<"********************"<<endl;
        for(vector<LR1_Sentence>::iterator iter=cur->lr1_sentences.begin(); iter!=cur->lr1_sentences.end(); ++iter)
        {
            cout<<iter->VN<<"\t-->\t";
            for(vector<string>::iterator iter1=iter->V.begin(); iter1!=iter->V.end(); ++iter1)
            {
                cout<<*iter1<<" ";
            }
            cout<<iter->preceding<<endl;
        }
        cout<<"\nstate change table:"<<endl;
        for(map<string, LR1_node*>::iterator iter=cur->stateChangeTable.begin(); iter!=cur->stateChangeTable.end(); ++iter)
        {
            cout<<iter->first<<"-->"<<iter->second->stateNO<<endl;
            if(iter->second->traverse == false)
            {
                wait.push(iter->second);
                used.push(iter->second);
                iter->second->traverse=true;
            }
        }
    }

    while(!used.empty())
    {
        cur=used.top();
        used.pop();
        cur->traverse=false;
    }
}
#endif // DEBUGER_GENERATEDFA

    nodeNumber=stateNO;     //set how much node in total
    return start;           //give back start node
}

int getColumnNumber(string s)   //return a symbol's position of column in the chart also equals the position in the all_symbol set
{
    int counter=0;
    for(set<string>::iterator iter=all_Symbol.begin(); iter!=all_Symbol.end(); ++iter)
    {
        if(s==*iter)
        {
            return counter;
        }
        else
        {
            ++counter;
        }
    }
    return -1;
}

int getReduceSentenceNumber(LR1_Sentence& sentence) //return a reduce LR1 sentence's opposite grammar sentence's position in the grammar
{
    for(vector<string>::iterator iter=sentence.V.begin(); iter!=sentence.V.end(); ++iter)
    {
        if(*iter == ".")
        {
            if(++iter == sentence.V.end())  //if '.' at the end means this is reduce sentence else is not
            {
                break;
            }
            else
            {
                return -1;                  //means is not reduce sentence
            }
        }
    }

    int i, j, k;
    for(i=0; i<grammar.size(); ++i)                                              //go every grammar sentence
    {
        if(sentence.VN != grammar[i].VN) { continue;}                                 //opposite sentence's VN must match
        for(j=0, k=0; j<sentence.V.size()-1 && k<grammar[i].V.size(); ++j, ++k)  //go every symbol except '.'
        {
            if(sentence.V[j] != grammar[i].V[k]) { break;}
        }
        if(j==sentence.V.size()-1 && k==grammar[i].V.size())                     //if all is matched then return this grammar sentence's position
        {
            return i;
        }
        else
        {
            continue;
        }
    }
    return -1;
}

Cell** generateChart(LR1_node* start)
{
    Cell **newChart=new Cell*[nodeNumber];          //allocate chart space
    for(int i=0; i<nodeNumber; ++i)
    {
        newChart[i]=new Cell[all_Symbol.size()];
    }

    LR1_node* cur;
    int columnNumber, sentenceNumber;
    stack<LR1_node*> wait;
    stack<LR1_node*> used;
    start->traverse=true;
    wait.push(start);
    used.push(start);
    while(!wait.empty())
    {
        cur=wait.top();
        wait.pop();

        //***************go every sentence and set reduce cell *******************
        for(vector<LR1_Sentence>::iterator iter=cur->lr1_sentences.begin(); iter!=cur->lr1_sentences.end(); ++iter)
        {
            sentenceNumber=getReduceSentenceNumber(*iter);                //get reduce sentence number
            if(sentenceNumber!=-1)                                        //is reduce sentence
            {
                columnNumber=getColumnNumber(iter->preceding);            //get column number
                if(newChart[cur->stateNO][columnNumber].type=='n')       //no ambiguous
                {
                    newChart[cur->stateNO][columnNumber].type='r';
                    newChart[cur->stateNO][columnNumber].id=sentenceNumber;
                }
                else
                {
                    cout<<"\n\n!!!!!!!error encountered, may be not LR1 grammar!\n\n"<<endl;
                    exit(1);
                }
            }
        }

        //***************go change table and set shift cell & input no traversed node into stack*******************
        for(map<string, LR1_node*>::iterator iter=cur->stateChangeTable.begin(); iter!=cur->stateChangeTable.end(); ++iter)
        {
            columnNumber=getColumnNumber(iter->first);
            if(newChart[cur->stateNO][columnNumber].type=='n')           //no ambiguous
            {
                newChart[cur->stateNO][columnNumber].type='s';
                newChart[cur->stateNO][columnNumber].id=iter->second->stateNO;
            }
            else
            {
                cout<<"\n\n!!!!!!!error encountered, may be not LR1 grammar!\n\n"<<endl;
                exit(1);
            }

            if(iter->second->traverse == false)  // no traversed push stack
            {
                wait.push(iter->second);
                used.push(iter->second);
                iter->second->traverse=true;
            }
        }
    }

    while(!used.empty())
    {
        cur=used.top();
        used.pop();
        cur->traverse=false;
    }

#ifndef DEBUGER_GENERATECHART
{
    cout<<"\nchart:\n\n\t";
    for(set<string>::iterator iter=all_Symbol.begin(); iter!=all_Symbol.end(); ++iter)
    {
        cout<<*iter<<"\t";
    }
    cout<<endl;
    for(int i=0; i<nodeNumber; ++i)
    {
        cout<<i<<"\t";
        for(int j=0; j<all_Symbol.size(); ++j)
        {
            if(newChart[i][j].type!='n')
            {
                cout<<newChart[i][j].type<<newChart[i][j].id;
            }
            cout<<"\t";
        }
        cout<<endl;
    }
    cout<<endl;
}
#endif // DEBUGER_GENERATECHART

return newChart;
}

Token getToken(ifstream& in)
{
    Token newToken;
    in>>newToken.type;
    in>>newToken.value;
    if(!in)
    {
        newToken.type="error";
        newToken.value="";

    }
    return newToken;
}

Tree_node* generateTree(ifstream& in, Cell** chart)
{
    Stack_node newStack_node, newStack_node_tmp, curStack_node;
    Token newToken, newToken_tmp;
    Tree_node *newTree_node=NULL;
    Cell cell;
    stack<Stack_node> ana_stack;

    //******stack initialization*****************
    newStack_node.symbol="$";
    newStack_node.state=0;
    ana_stack.push(newStack_node);

    //*********analysis and generate tree*********
    /*
        here the two flag is very important:
        1. if isReducing=false, isReduced=false then read a new token from file
        2. if occur an reducing we save newToken and newStack_node to their temp object, and generate new of them with reducing VN, and set isReducing=true, isReduced=false
        3. now we don't read new token, just use the objects generated in the second step and set isReducing=false, isReduced=true
        4. now we don't read new token too, just use the objects saved in the second step and set isReducing=false, isReduced=false, and now all things go to the begin just like done first step
    */
    bool isReducing=false, isReduced=false;
    while(true)
    {
        if(isReducing==false)
        {
            if(isReduced==false)
            {
                newToken=getToken(in);
                if(newToken.type=="error") { break;}
                newTree_node=new Tree_node(newToken.type, newToken.value);
                newStack_node.tree_node_p=newTree_node;
                newStack_node.symbol=newToken.type;
            }
            else
            {
                newToken=newToken_tmp;
                newStack_node=newStack_node_tmp;
                isReduced=false;
            }
        }

        cell=chart[ana_stack.top().state][getColumnNumber(newToken.type)];

        if(cell.type=='s')
        {
            newStack_node.state=cell.id;
            ana_stack.push(newStack_node);
            if(isReducing==true)
            {
                isReducing=false;
                isReduced=true;
            }
        }
        else if(cell.type=='r')
        {
            newStack_node_tmp=newStack_node;                        //save input in temp
            newToken_tmp=newToken;

            newToken.type=grammar[cell.id].VN;                      //create new of VN
            newToken.value="NULL";
            newTree_node=new Tree_node(newToken.type, newToken.value);
            newStack_node.tree_node_p=newTree_node;
            newStack_node.symbol=newToken.type;
            if(grammar[cell.id].V[0]=="epsilon")    //when use epsilon sentence, need not pop stack
            {
                newTree_node=new Tree_node("epsilon", "epsilon");
                newStack_node.tree_node_p->childs.push_back(newTree_node);
            }
            else                                    //else should pop all right sentence
            {
                for(int i=0; i<grammar[cell.id].V.size(); ++i)
                {
                    curStack_node=ana_stack.top();
                    ana_stack.pop();
                    newStack_node.tree_node_p->childs.push_back(curStack_node.tree_node_p);
                }
            }

            if(newToken.type=="S'" && ana_stack.top().state==0)         //accept state
            {
#ifndef DEBUGER_generateTree
                {
                    Tree_node* cur;
                    queue<Tree_node*> wait;
                    wait.push(newTree_node);
                    while(!wait.empty())
                    {
                        cur=wait.front();
                        wait.pop();
                        cout<<"*****************************"<<endl;
                        cout<<cur->type<<":"<<cur->value<<endl;
                        for(vector<Tree_node*>::reverse_iterator iter=cur->childs.rbegin(); iter!=cur->childs.rend(); ++iter)
                        {
                            wait.push(*iter);
                        }
                    }
                }
#endif // DEBUGER_generateTree
                return newTree_node;
            }
            isReducing=true;
        }
        else
        {
            cout<<"\n\n!!!!!!!!!error in:"<<newToken.type<<":"<<newToken.value<<endl;
            exit(1);        //******************here may cause memory release
        }
    }
}


int main()
{
    ifstream in_grammar("grammar.txt");
    if(!in_grammar)
    {
        printf("can not open grammar.txt\n");
        exit(1);
    }
    getGrammar(in_grammar);

    in_grammar.close();
    generateFirst();
    generateFollow();
    LR1_node *newDFA=generateDFA();
    Cell **newChart=generateChart(newDFA);

    ifstream  in_token("token.txt");
    if(!in_token)
    {
        printf("can not open token.txt\n");
        exit(1);
    }
    Tree_node *newTree=generateTree(in_token, newChart);
    in_token.close();

{
    ofstream out_tree("tree.txt");          //open tree.txt
    if(!out_tree)
    {
        printf("can not open tree.txt\n");
        exit(0);
    }

    Tree_node* cur;                         //output tree info to tree.txt
    queue<Tree_node*> wait;
    wait.push(newTree);
    newTree->fatherNO=0;                    //root node has no father, so it is 0
    int nodeNO_counter=0;
    while(!wait.empty())
    {
        cur=wait.front();
        wait.pop();
        cur->nodeNO=++nodeNO_counter;
        out_tree<<cur->fatherNO<<"\t"<<cur->type<<"\t"<<cur->value<<endl;
        for(vector<Tree_node*>::reverse_iterator iter=cur->childs.rbegin(); iter!=cur->childs.rend(); ++iter)
        {
            (*iter)->fatherNO=cur->nodeNO;
            wait.push(*iter);
        }
    }
}

    //*****************release memory: tree, chart, DFA

{
    queue<Tree_node*> wait, used;           //delete tree
    Tree_node *cur;
    wait.push(newTree);
    used.push(newTree);
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

    for(int i=0; i<nodeNumber; ++i)         //delete chart
    {
        delete newChart[i];
    }

    stack<LR1_node*> wait1, used1;          //delete DFA
    LR1_node *cur1;
    newDFA->traverse=true;
    wait1.push(newDFA);
    used1.push(newDFA);
    while(!wait.empty())
    {
        cur1=wait1.top();
        wait1.pop();

        for(map<string, LR1_node*>::iterator iter=cur1->stateChangeTable.begin(); iter!=cur1->stateChangeTable.end(); ++iter)
        {
            wait1.push(iter->second);
            used1.push(iter->second);
            iter->second->traverse=true;
        }

    }
    while(!used.empty())
    {
        cur1=used1.top();
        used1.pop();
        delete cur1;
    }
}

    return 0;
}
