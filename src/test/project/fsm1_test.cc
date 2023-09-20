#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

// 状态枚举
enum class State {
    STATE_A,
    STATE_B,
    STATE_C,
    STATE_D,
};

// 转换结构体
struct Transition {
    State sourceState;
    State targetState;
    string event;
};

class StateMachine {
private:
    unordered_map<State, vector<Transition>> transitions;
    State currentState;

public:
    void addTransition(State sourceState, State targetState, string event) {
        transitions[sourceState].push_back({sourceState, targetState, event});
    }

    void setCurrentState(State state) {
        currentState = state;
    }

    void processEvent(string event) {
        cout << "Processing event: " << event << endl;
        const auto& possibleTransitions = transitions[currentState];
        for (const auto& transition : possibleTransitions) {
            if (transition.event == event) {
                cout << "Transition from " << static_cast<int>(transition.sourceState)
                     << " to " << static_cast<int>(transition.targetState) << endl;
                currentState = transition.targetState;
                return;
            }
        }
        cout << "No valid transition for event: " << event << endl;
    }
};

#include"core/tt_test.h"
// JUST_RUN_TEST(fsm1, test)
TEST(fsm1, test)
{
    StateMachine stateMachine;

    // 添加状态转换
    stateMachine.addTransition(State::STATE_A, State::STATE_B, "event1");
    stateMachine.addTransition(State::STATE_B, State::STATE_C, "event2");
    stateMachine.addTransition(State::STATE_C, State::STATE_D, "event3");

    // 设置初始状态
    stateMachine.setCurrentState(State::STATE_A);

    // 处理事件
    stateMachine.processEvent("event1");  // 触发有效的转换
    stateMachine.processEvent("event2");  // 触发有效的转换
    stateMachine.processEvent("event3");  // 触发有效的转换
    stateMachine.processEvent("event4");  // 触发无效的转换
}