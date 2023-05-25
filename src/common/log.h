#ifndef __COMMON_LOG_H__
#define __COMMON_LOG_H__

#include <iostream>

template <typename... Args>
std::ostream & print(Args&&... args) {
    auto a = {(std::cout << std::forward<Args>(args), 0)...};
    (void)a;
    return std::cout;
}

#define COLOR_RED "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_GREEN "\033[32m"
#define COLOR_BLUE "\033[34m"
#define COLOR_WHITE "\033[37m"
#define COLOR_GRAY "\033[90m"

#define LOG_INFO(...) print(COLOR_GREEN, "[ INFO] ", __VA_ARGS__) << std::endl
#define LOG_WARN(...) print(COLOR_YELLOW, "[ WARN]", " [", __FILE__, ":", __LINE__, " (", __FUNCTION__, ")] ", COLOR_YELLOW, __VA_ARGS__) << "\033[0m" << std::endl
#define LOG_DEBUG(...) print(COLOR_BLUE, "[DEBUG]", " [", __FILE__, ":", __LINE__, " (", __FUNCTION__, ")] ", COLOR_BLUE, __VA_ARGS__) << "\033[0m" << std::endl
#define LOG_ERROR(...) print(COLOR_RED, "[ERROR]", " [", __FILE__, ":", __LINE__, " (", __FUNCTION__, ")] ", COLOR_RED, __VA_ARGS__) << "\033[0m" << std::endl


#endif // __COMMON_LOG_H__