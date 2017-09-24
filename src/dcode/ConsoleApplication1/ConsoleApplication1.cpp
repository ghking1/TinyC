// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <cstring>

using namespace std;

vector<string> words;       //used to save all words from line
ifstream in_mcode;
ofstream out_dcode;
map<int, life> regs_life;   //save all register's life in middle code
int LNO=0;                  //used to generate label number

string transmit(int i)
{
    switch(i)
    {
    case 0:
        return "AX";
        break;

    case 1:
        return "BX";
        break;

    case 2:
        return "CX";
        break;

    case 3:
        return "DX";
        break;

    default:
        break;
    }
}

void calculate_life()       //calculate all register's life in middle code
{
    string line, word;          //line used to save line from file, word used to save word from line
    int regNO;
    life new_life;
    int history_time=0;
    regs_life.clear();

    ifstream::pos_type temp=in_mcode.tellg();
    while(getline(in_mcode, line))
    {
        words.clear();
        ++history_time;
        istringstream stream(line);
        stream>>word;   //ignore the type number at begin
        while(stream>>word)                     //put line's word into words
        {
            words.push_back(word);
        }
        if(words[0]=="proc" && words[1]=="end") //reached proc end
        {
            break;
        }
        for(int i=0; i<words.size(); ++i)       //get words's reg* and set their life
        {
            if(words[i].substr(0,3)=="reg")
            {
                regNO=atoi(words[i].substr(3, words[i].length()-3).c_str());
                if(regs_life.find(regNO)==regs_life.end())
                {
                    new_life.start=history_time;
                    new_life.dead=history_time;
                    regs_life[regNO]=new_life;
                }
                else
                {
                    regs_life[regNO].dead=history_time;
                }
            }
        }
    }
    in_mcode.seekg(temp);
}

void allocate_reg()      //allocate register in destination machine to middle code's register
{
    int regs[4], history_time=0, current_time=0;
    for(int i=0; i<4; ++i)
    {
        regs[i]=0;
    }

    for(int regNO=1; regNO<=regs_life.size(); ++regNO)
    {
		current_time=regs_life[regNO].start;
        int i=0;
        for( ; i<4; ++i)
        {
            if(regs[i]<current_time) {break;}
        }

        if(i!=4)
        {
            regs_life[regNO].allocated_reg=i;
			regs[i]=regs_life[regNO].dead;
        }
        else
        {
            cout<<"register conflict!"<<endl;
            exit(1);
        }
    }
}

void f_section()    //generate section code
{
    if(words[0]=="segment")                                             //output segment
    {
        if(words[1]=="end")                                             //segment end
        {
            out_dcode<<words[2]<<" ends"<<endl;
        }
        else                                                            //segment head
        {
            out_dcode<<"\n;********************"<<endl;
            out_dcode<<words[1]<<" segment"<<endl;
            if(words[1]=="stack")                                       //stack segment need extra output
            {
                out_dcode<<"\tDB 0FFH DUP('0')"<<endl;
                out_dcode<<"\ttos label word"<<endl;
            }
            else if(words[1]=="code")                                   //code segment need add IO code here
            {
                ifstream in_io("IO.ASM");
                if(!in_io)
                {
                    cout<<"open file error!"<<endl;
                    exit(1);
                }

                char a;
                while(in_io.get(a))
                {
                    out_dcode.put(a);
                }
                in_io.close();
            }
        }
    }
    else if(words[0]=="proc")                                           //output proc
    {
        if(words[1]=="end")                                             //proc end
        {
            if(words[2]=="main")                                        //main proc need add code to return DDS
            {
                out_dcode<<"\tMOV AX, 4C00H"<<endl;
                out_dcode<<"\tINT 20H"<<endl;
            }
            out_dcode<<"\tRET"<<endl;
            out_dcode<<words[2]<<" endp\n"<<endl;
        }
        else                                                            //proc head
        {
            out_dcode<<"\n;--------------------"<<endl;
            if(words[1]=="main")                                        //main proc need extra output
            {
                out_dcode<<words[1]<<" proc far"<<endl;
                out_dcode<<"\tassume cs:code, ds:data, ss:stack"<<endl;
                out_dcode<<"start:"<<endl;
                out_dcode<<"\tMOV AX, data"<<endl;
                out_dcode<<"\tMOV DS, AX"<<endl;
                out_dcode<<"\tMOV AX, stack"<<endl;
                out_dcode<<"\tMOV SS, AX"<<endl;
                out_dcode<<"\tMOV SP, OFFSET tos\n"<<endl;
            }
            else
            {
                out_dcode<<words[1]<<" proc near"<<endl;
            }
            calculate_life();
            allocate_reg();
        }
    }
    else
    {

    }
}

void f_data()           //generate data code
{
    switch(words.size())
    {
    case 4 :
        out_dcode<<"\t"<<words[0]<<"\t"<<words[2]<<" DUP('0')"<<endl;
        break;

    case 5 :
        out_dcode<<"\t"<<words[1]<<"\t"<<words[0]<<"\t"<<words[3]<<" DUP('0')"<<endl;
        break;

    default:
        break;
    }
}

void f_stack_change()       //generate stack change code
{
    if(words[1]=="+")
    {
        out_dcode<<"\t"<<"ADD SP, "<<words[2]<<endl;
    }
    else if(words[1]=="-")
    {
        out_dcode<<"\t"<<"SUB SP, "<<words[2]<<endl;
    }
    else
    {

    }
}

void f_stack_all()          //generate PUSHA and POPA code
{
    if(words[0]=="push_all")
    {
        out_dcode<<"\tPUSHA"<<endl;
    }
    else if(words[0]=="pop_all")
    {
        out_dcode<<"\tPOPA"<<endl;
    }
    else
    {

    }
}

void f_tab()            //generate label
{
    out_dcode<<words[0]<<endl;
}

void f_jmp()            //generate jump code
{
    switch(words.size())
    {
    case 2:
        out_dcode<<"\tJMP "<<words[1]<<endl;
        break;

    case 4:
        out_dcode<<"\tCMP "<<words[1]<<", 0"<<endl;
        out_dcode<<"\tJZ "<<words[3]<<endl;
        break;

    default:
        break;
    }
}

void f_call_ret()   //generate call code
{
    switch(words.size())
    {
    case 1:
        //here don't need ret, it output in f_section
        break;

    case 2:
        out_dcode<<"\tCALL "<<words[1]<<endl;
        break;

    default:
        break;
    }
}

void f_push_pop()       //generate push and pop code
{
    if(words[0]=="push")
    {
        out_dcode<<"\tPUSH "<<words[1]<<endl;
    }
    else if(words[0]=="pop")
    {
        out_dcode<<"\tPOP "<<words[1]<<endl;
    }
    else
    {

    }
}

void f_load()       //generate MOV code, transmit data from memory to register
{
    if(words.size()==3) //is variable from data section
    {
        out_dcode<<"\tMOV "<<words[0]<<", WORD PTR "<<words[2]<<endl;
    }
    else if(words.size()==6)    //is array from data section
    {
        out_dcode<<"\tMOV DI, "<<words[4]<<endl;
		out_dcode<<"\tADD DI, DI"<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", WORD PTR "<<words[2]<<"[DI]"<<endl;
    }
    else if(words.size()==7)    //is variable or array from stack section
    {
        out_dcode<<"\tMOV DI, SP"<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", WORD PTR SS:[DI"<<words[4]<<words[5]<<"]"<<endl;
    }
    else
    {

    }
}

void f_store()      //similar to load
{
    if(words.size()==3)
    {
        out_dcode<<"\tMOV WORD PTR "<<words[0]<<", "<<words[2]<<endl;
    }
    else if(words.size()==6)
    {
        out_dcode<<"\tMOV DI, "<<words[2]<<endl;
		out_dcode<<"\tADD DI, DI"<<endl;
        out_dcode<<"\tMOV WORD PTR "<<words[0]<<"[DI], "<<words[5]<<endl;
    }
    else if(words.size()==7)
    {
        out_dcode<<"\tMOV DI, SP"<<endl;
        out_dcode<<"\tMOV WORD PTR SS:[DI"<<words[2]<<words[3]<<"], "<<words[6]<<endl;
    }
    else
    {

    }
}

void f_calculate()  //generate mathematical code
{
    if(words[3]=="+")
    {
        out_dcode<<"\tMOV "<<words[0]<<", "<<words[2]<<endl;
        out_dcode<<"\tADD "<<words[0]<<", "<<words[4]<<endl;
    }
    else if(words[3]=="-")
    {
        out_dcode<<"\tMOV "<<words[0]<<", "<<words[2]<<endl;
        out_dcode<<"\tSUB "<<words[0]<<", "<<words[4]<<endl;
    }
    else if(words[3]=="*")
    {
        out_dcode<<"\tMOV AX, "<<words[2]<<endl;
        out_dcode<<"\tMOV DX, "<<words[4]<<endl;
        out_dcode<<"\tMUL DL"<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", AX"<<endl;
    }
    else if(words[3]=="/")
    {
        out_dcode<<"\tMOV AX, "<<words[2]<<endl;
        out_dcode<<"\tMOV DX, "<<words[4]<<endl;
        out_dcode<<"\tDIV DL"<<endl;
        out_dcode<<"\tMOV AH, 0"<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", AX"<<endl;
    }
    else if(words[3]=="%")
    {
        out_dcode<<"\tMOV AX, "<<words[2]<<endl;
        out_dcode<<"\tMOV DX, "<<words[4]<<endl;
        out_dcode<<"\tDIV DL"<<endl;
        out_dcode<<"\tMOV AL, AH"<<endl;
        out_dcode<<"\tMOV AH, 0"<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", AX"<<endl;
    }
    else if(words[3]=="<")
    {
        ++LNO;
        out_dcode<<"\tMOV "<<words[0]<<", "<<words[2]<<endl;
        out_dcode<<"\tCMP "<<words[0]<<", "<<words[4]<<endl;
        out_dcode<<"\tJB L"<<LNO<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", 0"<<endl;
        out_dcode<<"\tJMP L"<<LNO<<"_"<<endl;
        out_dcode<<"L"<<LNO<<":"<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", 1"<<endl;
        out_dcode<<"L"<<LNO<<"_:"<<endl;
    }
    else if(words[3]=="<=")
    {
        ++LNO;
        out_dcode<<"\tMOV "<<words[0]<<", "<<words[4]<<endl;
        out_dcode<<"\tCMP "<<words[0]<<", "<<words[2]<<endl;
        out_dcode<<"\tJB L"<<LNO<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", 1"<<endl;
        out_dcode<<"\tJMP L"<<LNO<<"_"<<endl;
        out_dcode<<"L"<<LNO<<":"<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", 0"<<endl;
        out_dcode<<"L"<<LNO<<"_:"<<endl;
    }
    else if(words[3]=="==")
    {
        ++LNO;
        out_dcode<<"\tMOV "<<words[0]<<", "<<words[2]<<endl;
        out_dcode<<"\tCMP "<<words[0]<<", "<<words[4]<<endl;
        out_dcode<<"\tJZ L"<<LNO<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", 0"<<endl;
        out_dcode<<"\tJMP L"<<LNO<<"_"<<endl;
        out_dcode<<"L"<<LNO<<":"<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", 1"<<endl;
        out_dcode<<"L"<<LNO<<"_:"<<endl;
    }
    else if(words[3]==">=")
    {
        ++LNO;
        out_dcode<<"\tMOV "<<words[0]<<", "<<words[2]<<endl;
        out_dcode<<"\tCMP "<<words[0]<<", "<<words[4]<<endl;
        out_dcode<<"\tJB L"<<LNO<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", 1"<<endl;
        out_dcode<<"\tJMP L"<<LNO<<"_"<<endl;
        out_dcode<<"L"<<LNO<<":"<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", 0"<<endl;
        out_dcode<<"L"<<LNO<<"_:"<<endl;
    }
    else if(words[3]==">")
    {
        ++LNO;
        out_dcode<<"\tMOV "<<words[0]<<", "<<words[4]<<endl;
        out_dcode<<"\tCMP "<<words[0]<<", "<<words[2]<<endl;
        out_dcode<<"\tJB L"<<LNO<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", 0"<<endl;
        out_dcode<<"\tJMP L"<<LNO<<"_"<<endl;
        out_dcode<<"L"<<LNO<<":"<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", 1"<<endl;
        out_dcode<<"L"<<LNO<<"_:"<<endl;
    }
    else if(words[3]=="!=")
    {
        ++LNO;
        out_dcode<<"\tMOV "<<words[0]<<", "<<words[2]<<endl;
        out_dcode<<"\tCMP "<<words[0]<<", "<<words[4]<<endl;
        out_dcode<<"\tJNZ L"<<LNO<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", 0"<<endl;
        out_dcode<<"\tJMP L"<<LNO<<"_"<<endl;
        out_dcode<<"L"<<LNO<<":"<<endl;
        out_dcode<<"\tMOV "<<words[0]<<", 1"<<endl;
        out_dcode<<"L"<<LNO<<"_:"<<endl;
    }
    else
    {

    }
}

int main()
{
    string line, word;          //line used to save line from file, word used to save word from line
    int sentence_type,regNO;
    in_mcode.open("mcode.txt");
    out_dcode.open("dcode.txt");
    if(!in_mcode || !out_dcode)
    {
        cout<<"open file error!"<<endl;
        out_dcode.close();
        in_mcode.close();
        exit(1);
    }

    while(getline(in_mcode, line))
    {
        words.clear();
        sentence_type=none_sentence; //clear sentence type
        istringstream stream(line);  //get new line from file
        stream>>sentence_type;       //get sentence type
        while(stream>>word)          //get words from line
        {
            words.push_back(word);
        }

        for(int i=0; i<words.size(); ++i)       //transmit reg*
        {
            if(words[i].substr(0,3)=="reg")
            {
                regNO=atoi(words[i].substr(3, words[i].length()-3).c_str());
                words[i]=transmit(regs_life[regNO].allocated_reg);
            }
        }

        switch(sentence_type)       //call function according it's sentence type
        {
        case s_section:
            f_section();
            break;

        case s_data:
            f_data();
            break;

        case s_stack_change:
            f_stack_change();
            break;

        case s_stack_all:
            f_stack_all();
            break;

        case s_tab:
            f_tab();
            break;

        case s_jmp:
            f_jmp();
            break;

        case s_call_ret:
            f_call_ret();
            break;

        case s_push_pop:
            f_push_pop();
            break;

        case s_load:
            f_load();
            break;

        case s_store:
            f_store();
            break;

        case s_calculate:
            f_calculate();
            break;

        default:
            break;
        }
    }

    out_dcode.close();
    in_mcode.close();
    return 0;
}
