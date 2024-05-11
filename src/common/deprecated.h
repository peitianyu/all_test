#ifndef __DEPRECATED_H__
#define __DEPRECATED_H__

// 实现一个类似于C++17的[[deprecated]]的宏
// 参考：https://stackoverflow.com/questions/295120/c-mark-as-deprecated
#if defined(__GNUC__) || defined(__clang__)
#define DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#define DEPRECATED(msg) __declspec(deprecated(msg))
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(msg)
#endif


#endif // __DEPRECATED_H__