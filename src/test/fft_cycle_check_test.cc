#include "core/tt_test.h"

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
using namespace std;

// 定义一个复数类型
typedef complex<double> Complex;

// 使用Cooley-Tukey算法实现FFT
void fft(vector<Complex>& data) {
  int n = data.size();
  // 如果数据长度不是2的幂次，直接返回
  if (n <= 1) return;
  // 如果数据长度不是偶数，直接返回
  if (n % 2 != 0) return;
  // 把数据分成偶数项和奇数项
  vector<Complex> even(n / 2);
  vector<Complex> odd(n / 2);
  for (int i = 0; i < n / 2; i++) {
    even[i] = data[2 * i];
    odd[i] = data[2 * i + 1];
  }
  // 对偶数项和奇数项分别进行FFT
  fft(even);
  fft(odd);
  // 合并结果
  for (int k = 0; k < n / 2; k++) {
    // 计算旋转因子
    Complex t = polar(1.0, -2 * M_PI * k / n) * odd[k];
    // 使用蝶形运算合并
    data[k] = even[k] + t;
    data[k + n / 2] = even[k] - t;
  }
}

// 判断两个数组是否是环形的
bool isCycle(vector<int>& arr1, vector<int>& arr2) {
  // 如果长度不等，直接返回false
  if (arr1.size() != arr2.size()) return false;
  // 如果长度为0，直接返回true
  if (arr1.empty()) return true;
  // 把第一个数组复制一遍，拼接到末尾
  vector<Complex> temp(arr1.begin(), arr1.end());
  temp.insert(temp.end(), arr1.begin(), arr1.end());
  // 对拼接后的数组进行FFT
  fft(temp);
  // 对第二个数组进行FFT
  vector<Complex> freq(arr2.begin(), arr2.end());
  fft(freq);
  // 比较两个频域数组是否相同
  for (uint i = 0; i < temp.size(); i++) {
    std::cout << temp[i] << " " << freq[i] << std::endl;
    // 如果有任何一个元素不同，就返回false
    if (temp[i] != freq[i]) return false;
  }
  // 如果都相同，就返回true
  return true;
}

JUST_RUN_TEST(fft_cycle_check, test)
TEST(fft_cycle_check, test)
{
    std::vector<int> arr1 = {1, 2, 3, 4, 5};
    std::vector<int> arr2 = {3, 4, 5, 1, 2};
    std::cout << isCycle(arr1, arr2) << std::endl;
}