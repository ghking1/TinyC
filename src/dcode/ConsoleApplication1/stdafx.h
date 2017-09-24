// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

typedef enum{none_sentence,\
s_section, s_data, s_stack_change, s_stack_all, s_tab, s_jmp,\
s_call_ret, s_push_pop, s_load, s_store, s_calculate,
} sentence_type;            //sentence type of middle code

class life
{
public:
    int start;              //birthday
    int dead;               //dead day
    int allocated_reg;      //which register of destination machine allocated to it
};



// TODO: 在此处引用程序需要的其他头文件
