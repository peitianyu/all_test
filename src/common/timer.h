#ifndef __TIMER_H__
#define __TIMER_H__

#include <chrono>
#include <iostream>

class Timer {
public:
    typedef std::chrono ::milliseconds Milliseconds;
    typedef std::chrono::high_resolution_clock Clock;

    Timer(const uint& period) : period_(period), start_(Clock::now()) {}

    void set_period(const uint& period) { period_ = period; }
    uint get_period() { return period_; }

    void reset() { start_ = Clock::now();}
    bool is_alarm(){ return (Clock::now() - start_).count() >= period_ * 1e6; }
private:
    uint period_; // ms
    std::chrono::time_point<Clock> start_;
};


#endif // __TIMER_H__