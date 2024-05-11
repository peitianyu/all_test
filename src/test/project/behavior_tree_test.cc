#include "core/tt_test.h"
#include "common/log.h"
#include "common/behavior_tree.h"

class PrintValue : public BehaviorTree {
public:
    PrintValue(const std::string &name, const std::string &key)
        : BehaviorTree(name, NODE_ACTION), m_key(key) {}

    virtual void run() override {
        LOG("PrintValue: ", m_key);
        set_status(BT_SUCCESS);
    }
private:
    std::string m_key;
};

class SwitchIndex : public BehaviorTree {
public:
    SwitchIndex(const std::string &name)
        : BehaviorTree(name, NODE_ACTION) {}

    virtual void run() override {
        m_data[m_name] = std::make_pair<std::string, std::string>("switch_index", "1");
        set_status(BT_SUCCESS);
    }
};

class RepeatNode : public BehaviorTree
{
public:
    RepeatNode(const std::string &name, int cnt) 
        : BehaviorTree(name, NODE_ACTION), m_cnt(cnt) {}
    
    virtual void run() override {
        m_data[m_name] = std::make_pair<std::string, std::string>("need_repeat", "1");
        if(m_cnt--) m_data[m_name].second = "1";
        else        m_data[m_name].second = "0"; 

        set_status(BT_SUCCESS);  
    }
protected:
    int m_cnt;
};

// JUST_RUN_TEST(behavior_tree, test)
TEST(behavior_tree, test)
{
    LOG_TEST("----------------------------behavior_tree--------------------------");

    LOG_TEST("-------------------------sequence test--------------------------");
    BehaviorTree* bt_sequence = new BehaviorTree("", NODE_SEQUENCE);
    bt_sequence->add_child(new PrintValue("print_value_1", "key1"));
    bt_sequence->add_child(new PrintValue("print_value_2", "key2"));
    bt_sequence->update();

    LOG_TEST("----------------------------fallback test----------------------------");

    BehaviorTree* bt_fallback = new BehaviorTree("", NODE_FALLBACK);
    bt_fallback->add_child(new PrintValue("print_value_3", "key3"));
    bt_fallback->add_child(new PrintValue("print_value_4", "key4"));
    bt_fallback->update();

    LOG_TEST("----------------------------switch test----------------------------");

    BehaviorTree* bt_switch = new BehaviorTree("", NODE_SWITCH);
    bt_switch->add_child(new SwitchIndex("switch_choice"));
    bt_switch->add_child(new PrintValue("print_value_5", "key5"));
    bt_switch->add_child(new PrintValue("print_value_6", "key6"));
    bt_switch->update();

    LOG_TEST("----------------------------repeat test----------------------------");

    BehaviorTree* bt_repeat = new BehaviorTree("", NODE_REPEAT);
    bt_repeat->add_child(new RepeatNode("print_choice", 3));
    bt_repeat->add_child(new PrintValue("print_value_7", "key7"));
    bt_repeat->update();

    LOG_TEST("----------------------------sub tree test----------------------------");

    BehaviorTree* root = new BehaviorTree("root", NODE_SEQUENCE);
    root->add_child(bt_sequence);
    root->add_child(bt_fallback);
    root->add_child(bt_switch);
    BehaviorTree* bt_repeat1 = new BehaviorTree("", NODE_REPEAT);
    bt_repeat1->add_child(new RepeatNode("print_choice", 3));
    bt_repeat1->add_child(new PrintValue("print_value_7", "key7"));
    root->add_child(bt_repeat1);
    root->update();
}