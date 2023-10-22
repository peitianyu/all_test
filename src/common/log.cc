#include "log.h"

std::string get_file_name(const std::string &file_name) {
    size_t pos = file_name.find_last_of('/');
    if (pos == std::string::npos) return file_name; 
    return file_name.substr(pos + 1);
}

#include <chrono>
std::string get_curr_time()
{
    typedef std::chrono::system_clock Clock;
    typedef std::chrono::milliseconds milliseconds;

    auto get_time = []()->double{
        return std::chrono::duration_cast<milliseconds>(Clock::now().time_since_epoch()).count();
    };

    static double start = get_time();
    return "["+std::to_string(get_time()-start)+"] ";
}