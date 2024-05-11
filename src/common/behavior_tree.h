#ifndef __BEHAVIOR_TREE_H__
#define __BEHAVIOR_TREE_H__

#include <vector>
#include <unordered_map>
#include <iostream>

enum BehaviorTreeStatus
{
    BT_IDLE,
    BT_RUNNING,
    BT_SUCCESS,
    BT_FAILURE
};

enum NodeType
{
    NODE_FALLBACK,
    NODE_SEQUENCE,
    NODE_SWITCH,
    NODE_REPEAT,
    NODE_ACTION
};

class BehaviorTree
{
public:
    BehaviorTree(const std::string &name, NodeType type) 
        : m_name(name), m_status(BT_IDLE), m_ctrl_type(type) {}
    
    virtual ~BehaviorTree() { for (auto child : m_children) delete child; }

    virtual void run() { set_status(BT_SUCCESS); };

    void add_child(BehaviorTree *child) { m_children.push_back(child); }

    void update() 
    {
        switch(m_ctrl_type)
        {
            case NODE_FALLBACK:     bt_fallback();  break;
            case NODE_SEQUENCE:     bt_sequence();  break;  
            case NODE_SWITCH:       bt_switch();    break;
            case NODE_REPEAT:       bt_repeat();    break;
            case NODE_ACTION:       bt_action();    break;
        }
    }
protected:
    void bt_fallback()
    {
        for (auto child : m_children)
        {
            child->update();
            if (child->get_status() == BT_SUCCESS)
            {
                m_status = BT_SUCCESS;
                return;
            }
        }
        m_status = BT_FAILURE;
    }

    void bt_sequence()
    {
        for (auto child : m_children)
        {
            child->update();
            if (child->get_status() == BT_FAILURE)
            {
                m_status = BT_FAILURE;
                return;
            }
        }
        m_status = BT_SUCCESS;
    }

    void bt_switch()
    {
        if(!check_condition_status("switch_index")) { 
            m_status = BT_FAILURE;
            return;
        }

        int index = std::stoi(m_children[0]->get_data(m_children[0]->get_name()).second);
        m_children[index+1]->update();
        m_status = BT_SUCCESS;
    }

    void bt_repeat()
    {
        if(!check_condition_status("need_repeat")) {
            m_status = BT_FAILURE;
            return;
        }

        while(std::stoi(m_children[0]->get_data(m_children[0]->get_name()).second)) {
            m_children[1]->update();

            m_children[0]->update();
            if(!std::stoi(m_children[0]->get_data(m_children[0]->get_name()).second)) break;
        }
        m_status = BT_SUCCESS;
    }

    void bt_action() { run(); }
protected:
    bool check_condition_status(const std::string& key) 
    {
        if(m_children.size() < 2) return false;

        m_children[0]->update();
        if(m_children[0]->get_status() != BT_SUCCESS) return false;

        if(m_children[0]->get_data(m_children[0]->get_name()).first != key) {
            std::cout << "ERROR: THIS CONDITION IS NOT MATCHED!" << std::endl;
            return false;
        }

        return true;
    }

    std::string get_name() { return m_name; }

    BehaviorTreeStatus get_status() { return m_status; }

    std::pair<std::string, std::string> get_data(const std::string& name) 
    { 
        if(m_data.find(name) != m_data.end())   return m_data[name]; 
        else                                    return std::make_pair("", ""); 
    }

    void set_status(const BehaviorTreeStatus& status) { m_status = status; }
protected:
    std::string m_name;
    BehaviorTreeStatus m_status;
    NodeType m_ctrl_type;
    std::vector<BehaviorTree *> m_children;
    static std::unordered_map<std::string, std::pair<std::string, std::string>> m_data;
};

std::unordered_map<std::string, std::pair<std::string, std::string>> BehaviorTree::m_data = {};

#endif // __BEHAVIOR_TREE_H__