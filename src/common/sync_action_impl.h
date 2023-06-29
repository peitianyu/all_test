#ifndef __SYNC_ACTION_IMPL_H__
#define __SYNC_ACTION_IMPL_H__

#include <thread>  
#include <mutex> 

template<typename DERIVED>
class SyncActionImpl
{
public:
    static SyncActionImpl &GetInstance()
    {
        static SyncActionImpl instance;
        return instance;
    }

    template<typename TYPE>
    void SetInput(const TYPE& data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        GetDerived().SetInput(data);
    }

    void GetOutput()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        GetDerived().GetOutput();
    }
private:
    SyncActionImpl(){
        m_thread = std::thread([this](){
            this->Run();
        });
    }
    ~SyncActionImpl(){
        m_thread.join();
    }

    SyncActionImpl(const SyncActionImpl &) = delete;
    SyncActionImpl &operator=(const SyncActionImpl &) = delete;
private:
    void Run()
    {
        GetDerived().Run();
    }

    DERIVED& GetDerived() {
        return static_cast<DERIVED&>(*this);
    }
protected:
    std::thread m_thread;
    std::mutex m_mutex;
};

#endif // __SYNC_ACTION_IMPL_H__