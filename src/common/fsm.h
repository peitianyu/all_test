#ifndef __FSM_H__
#define __FSM_H__

#include<iostream>
#include<functional>
#include<set>
#include <vector>

enum year_state{
    SPRING = 1,
    SUMMER,
    AUTUMN,
    WINTER
};

enum year_event{
    EVENT1 = 1,
    EVENT2,
    EVENT3,
    EVENT4,
};

typedef struct FsmTable_s{
    int event;              //事件
    int CurState;           //当前状态
    void (*eventActFun)();  //函数指针
    int NextState;          //下一个状态
}FsmTable_t;

class Fsm{
public:
    Fsm(int cur_state) :m_cur_state(cur_state) {}

    Fsm(int cur_state, const std::vector<FsmTable_t>& pTable)
        :m_cur_state(cur_state), m_fsm_table(pTable) {}

    void AddTable(const FsmTable_t& Table){
        m_fsm_table.push_back(Table);
    }

    void EventHandle(int event){
        std::vector<FsmTable_t>::iterator iter = m_fsm_table.begin();
        for(; iter != m_fsm_table.end(); ++iter){
            if(iter->event == event && iter->CurState == m_cur_state){
                iter->eventActFun();
                StateTransfer(iter->NextState);
                break;
            }
        }
        if(iter == m_fsm_table.end()){
            std::cout << "can not handle this event" << std::endl;
        }
    }
private:
    void StateTransfer(int state){m_cur_state = state;}
private:
    int m_cur_state;
    std::vector<FsmTable_t> m_fsm_table;
};

#endif // __FSM_H__