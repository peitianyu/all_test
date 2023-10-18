#ifndef __TT_TICTOC_H__
#define __TT_TICTOC_H__

#include <chrono>
#include <iostream>

class TicToc
{
public:
    TicToc() : start_(std::chrono::high_resolution_clock::now()) {}
    void Tic() { start_ = std::chrono::high_resolution_clock::now(); }
    double Toc() const
    {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::duration<double>>(end - start_).count();
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

class TicTocAuto
{
public:
    TicTocAuto(const std::string& msg = "") : msg_(msg) { tictoc_.Tic(); }
    ~TicTocAuto() { 
        std::cout << msg_ << " cost: " << tictoc_.Toc() * 1000 << " ms" << std::endl;
        #ifdef OPEN_TO_FILE
        LOG_TO_FILE("../out/"+msg_+"_cost.txt", tictoc_.Toc() * 1000);
        #endif
    }
private:
    std::string msg_;
    TicToc tictoc_;
};

#endif // __TT_TICTOC_H__