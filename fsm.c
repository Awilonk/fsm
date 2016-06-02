#include "fsm.h"


#define TIME_OUT   1
void *msg[10];

typedef struct { //ϵͳ��Ϣ��������Ϣ��ͨ������ṹ�崫��
    INT8U type;
    INT8U value;
} SYSMSG;

enum STATE_ID
{state_1, state_2, state_3, state_4, state_5};

typedef void (*FSM_FUNC)(SYSMSG *);

typedef struct {//״̬��״̬�ṹ
    enum STATE_ID id;
    FSM_FUNC enter_func;
    FSM_FUNC exit_func;
    FSM_FUNC default_func;
} FSM_STATE;

typedef struct tagFSM_FUNC_TRAN {  
    INT8U MsgType;      //����״̬ת������Ҫ��msg
    enum STATE_ID NextStateId;
    FSM_FUNC tran_func;
    struct tagFSM_FUNC_TRAN *next;
} FSM_STATE_TRAN;   //״̬ת������

FSM_STATE_TRAN *fsm_tran[10] = {{TIME_OUT��}};  //״̬ת����ÿ��״̬�µ�ת�����������
OS_EVENT * msgQueue;  //ϵͳ��Ϣ

void Fsm_init(void)
{
    msgQueue = OSQCreate(msg, 10);

}

void fsm_do_event(FSM_STATE *State, SYSMSG sysmsg)  //�¼���������
{
    u8 err;

    FSM_STATE_TRAN *Temp = fsm_tran[State->id]->next;

    if(State->default_func) { //ִ�е�ǰ״̬Ĭ�Ϸ���
        State->default_func(&sysmsg);
    }
    OSQPend(msgQueue, 500, &err);

    if(err != OS_ERR_NONE ) {
        sysmsg.type = TIME_OUT;
    }
    while(Temp != NULL) {
        if(Temp->MsgType == sysmsg.type) {
            if(State->exit_func) {
                State->exit_func(&sysmsg);
            }
            if(Temp->tran_func) {
                Temp->tran_func(&sysmsg);
            }
            State = Temp->NextStateId;
            if(State->enter_func) {
                State->enter_func(&sysmsg);
            }
            break;
        }

    }
}
