#ifndef __TT_TICTOC_H__
#define __TT_TICTOC_H__

#include <chrono>

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

#endif // __TT_TICTOC_H__