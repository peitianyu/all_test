#include "core/tt_test.h"
#include "common/log.h"
#include <iostream>
#include "core/tt_tictoc.h"

// 参考: https://zhuanlan.zhihu.com/p/115507378

using namespace std;

long combine_trival(const vector<int>& v);
void combine_trival_bad(const vector<int>& v, long& result);
long combine_outofloop(const vector<int>& v);
long combine_direct_access(const vector<int>& v);
long combine_unroll_2(const vector<int>& v);
long combine_unroll_3(const vector<int>& v);
long combine_unroll_4(const vector<int>& v);
long combine_auto_unroll(const vector<int>& v);
long combine_unroll_2_parallel_2(const vector<int>& v);
long combine_unroll_3_parallel_3(const vector<int>& v);
long combine_unroll_2_reassociate(const vector<int>& v);
long combine_final(const vector<int>& v);

// JUST_RUN_TEST(code_optimize, test)
TEST(code_optimize, test)
{
    LOG_TEST("------------------------------code_optimize--------------------------");

    vector<int> v;
    for (int i = 0; i < 1000000; i++) {
        v.push_back(i);
    }

    LOG_TEST("------------------------------combine_trival--------------------------");
    long result_trival = 0;
    {
        TicTocAuto tic_toc_auto("combine_trival");
        result_trival = combine_trival(v);
    }

    LOG_TEST("------------------------------combine_trival_bad--------------------------");
    {
        TicTocAuto tic_toc_auto("combine_trival_bad");
        long result_trival_bad = 0;
        combine_trival_bad(v, result_trival_bad);
        ASSERT_EQ(result_trival, result_trival_bad);
    }

    LOG_TEST("------------------------------combine_outofloop--------------------------");
    {
        TicTocAuto tic_toc_auto("combine_outofloop");
        long result_outofloop = combine_outofloop(v);
        ASSERT_EQ(result_trival, result_outofloop);
    }

    LOG_TEST("------------------------------combine_direct_access--------------------------");
    {
        TicTocAuto tic_toc_auto("combine_direct_access");
        long result_direct_access = combine_direct_access(v);
        ASSERT_EQ(result_trival, result_direct_access);
    }

    LOG_TEST("------------------------------combine_unroll_2--------------------------");
    {
        TicTocAuto tic_toc_auto("combine_unroll_2");
        long result_unroll_2 = combine_unroll_2(v);
        ASSERT_EQ(result_trival, result_unroll_2);
    }

    LOG_TEST("------------------------------combine_unroll_3--------------------------");
    {
        TicTocAuto tic_toc_auto("combine_unroll_3");
        long result_unroll_3 = combine_unroll_3(v);
        ASSERT_EQ(result_trival, result_unroll_3);
    }

    LOG_TEST("------------------------------combine_unroll_4--------------------------");
    {
        TicTocAuto tic_toc_auto("combine_unroll_4");
        long result_unroll_4 = combine_unroll_4(v);
        ASSERT_EQ(result_trival, result_unroll_4);
    }

    LOG_TEST("------------------------------combine_auto_unroll--------------------------");
    {
        TicTocAuto tic_toc_auto("combine_auto_unroll");
        long result_auto_unroll = combine_auto_unroll(v);
        ASSERT_EQ(result_trival, result_auto_unroll);
    }

    LOG_TEST("------------------------------combine_unroll_2_parallel_2--------------------------");
    {
        TicTocAuto tic_toc_auto("combine_unroll_2_parallel_2");
        long result_unroll_2_parallel_2 = combine_unroll_2_parallel_2(v);
        ASSERT_EQ(result_trival, result_unroll_2_parallel_2);
    }

    LOG_TEST("------------------------------combine_unroll_3_parallel_3--------------------------");
    {
        TicTocAuto tic_toc_auto("combine_unroll_3_parallel_3");
        long result_unroll_3_parallel_3 = combine_unroll_3_parallel_3(v);
        ASSERT_EQ(result_trival, result_unroll_3_parallel_3);
    }

    LOG_TEST("------------------------------combine_unroll_2_reassociate--------------------------");
    {
        TicTocAuto tic_toc_auto("combine_unroll_2_reassociate");
        long result_unroll_2_reassociate = combine_unroll_2_reassociate(v);
        ASSERT_EQ(result_trival, result_unroll_2_reassociate);
    }

    LOG_TEST("------------------------------combine_final--------------------------");
    {
        TicTocAuto tic_toc_auto("combine_final");
        long result_final = combine_final(v);
        ASSERT_EQ(result_trival, result_final);
    }
}

/* Trival implementation */
long combine_trival(const vector<int>& v) {
    long sum = 0;
    for (size_t i = 0; i < v.size(); i++) {
        sum += v[i];
    }
    return sum;
}

/* A bad trival implementation */
void combine_trival_bad(const vector<int>& v, long& result) {
    for (size_t i = 0; i < v.size(); i++) {
        result += v[i];
    }
}

/* Move call to v.size() out of loop */
long combine_outofloop(const vector<int>& v) {
    long sum = 0;
    size_t size = v.size();
    for (size_t i = 0; i < size; i++) {
        sum += v[i];
    }
    return sum;
}

/* Direct access to vector data */
long combine_direct_access(const vector<int>& v) {
    long sum = 0;
    const int* end = v.data() + v.size();
    for (const int* i = v.data(); i < end; i++) {
        sum += *i;
    }
    return sum;
}

/* Unroll loop by 2 */
long combine_unroll_2(const vector<int>& v) {
    long sum = 0;
    const int* i;
    const int* end = v.data() + v.size();
    for (i = v.data(); i < end; i += 2) {
        sum = sum + *i + *(i+1);
    }
    for (; i < end; i++) {
        sum += *i;
    }
    return sum;
}

/* Unroll loop by 3 */
long combine_unroll_3(const vector<int>& v) {
    long sum = 0;
    const int* i;
    const int* end = v.data() + v.size();
    for (i = v.data(); i < end; i += 3) {
        sum = sum + *i + *(i + 1) + *(i + 2);
    }
    for (; i < end; i++) {
        sum += *i;
    }
    return sum;
}

/* Unroll loop by 4 */
long combine_unroll_4(const vector<int>& v) {
    long sum = 0;
    const int* i;
    const int* end = v.data() + v.size();
    for (i = v.data(); i < end; i += 4) {
        sum = sum + *i + *(i + 1) + *(i + 2) + *(i + 3);
    }
    for (; i < end; i++) {
        sum += *i;
    }
    return sum;
}

/* Unroll by compiler */
__attribute__((optimize("-funroll-loops")))
long combine_auto_unroll(const vector<int>& v) {
    long sum = 0;
    const int* const end = v.data() + v.size();
    for (const int* i = v.data(); i < end; i++) {
        sum += *i;
    }
    return sum;
}

/* Unroll loop by 2, 2-way parallelism */
long combine_unroll_2_parallel_2(const vector<int>& v) {
    long sum1 = 0;
    long sum2 = 0;
    const int* i;
    const int* end = v.data() + v.size();
    for (i = v.data(); i < end; i += 2) {
        sum1 = sum1 + *i;
        sum2 = sum2 + *(i + 1);
    }
    for (; i < end; i++) {
        sum1 += *i;
    }
    return sum1 + sum2;
}

/* Unroll loop by 3, 3-way parallelism */
long combine_unroll_3_parallel_3(const vector<int>& v) {
    long sum1 = 0;
    long sum2 = 0;
    long sum3 = 0;
    const int* i;
    const int* end = v.data() + v.size();
    for (i = v.data(); i < end; i += 3) {
        sum1 = sum1 + *i;
        sum2 = sum2 + *(i + 1);
        sum3 = sum3 + *(i + 2);
    }
    for (; i < end; i++) {
        sum1 += *i;
    }
    return sum1 + sum2 + sum3;
}

/* Change associativity of combining operation */
long combine_unroll_2_reassociate(const vector<int>& v) {
    long sum = 0;
    const int* i;
    const int* end = v.data() + v.size();
    for (i = v.data(); i < end; i += 2) {
        sum += *i + *(i+1);
    }
    for (; i < end; i++) {
        sum += *i;
    }
    return sum;
}

/* Final version */
long combine_final(const vector<int>& v) {
    long sum = 0;
    const int* i;
    const int* end = v.data() + v.size();
    for (i = v.data(); i < end; i += 4) {
        sum += *i + *(i + 1) + *(i + 2) + *(i + 3);
    }
    for (; i < end; i++) {
        sum += *i;
    }
    return sum;
}