// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
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



// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
