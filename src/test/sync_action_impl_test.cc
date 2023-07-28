#include "common/sync_action_impl.h"
#include "core/tt_test.h"

#include <iostream>

class SyncAction : public SyncActionImpl<SyncAction>
{
public:
    template<typename TYPE>
    void SetInput(const TYPE& data)
    {
        if(std::is_same<TYPE, uint>::value){
            std::lock_guard<std::mutex> lock(m_mutex);
            m_a = data;
        }
        else if(std::is_same<TYPE, float>::value){
            std::lock_guard<std::mutex> lock(m_mutex);
            m_b = data;
        }
        else{
            std::cout << "SyncAction::SetInput() error" << std::endl;
        }
    }

    void Run()
    {
        for(uint i = 0; i < 5; ++i)
        {
            uint a = 0;
            float b = 0.0f;
            m_mutex.try_lock();
            a = m_a;
            b = m_b;
            m_mutex.unlock();

            std::cout << "a = " << a << ", b = " << b << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
private:
    uint m_a;
    float m_b;
};

// JUST_RUN_TEST(sync_action_impl, test)
TEST(sync_action_impl, test)
{
    uint a = 1;
    float b = 2.0f;
    SyncAction::GetInstance().SetInput(a);
    SyncAction::GetInstance().SetInput(b);
}