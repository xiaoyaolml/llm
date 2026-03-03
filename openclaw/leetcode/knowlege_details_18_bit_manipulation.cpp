/*
 * 专题十八：位运算 Bit Manipulation — 可运行示例代码
 * 作者：大胖超 😜
 *
 * 编译：g++ -std=c++17 -O2 knowlege_details_18_bit_manipulation.cpp -o bit_manip
 * 运行：./bit_manip
 *
 * Demo 列表：
 *   Demo1  — 基础位操作工具函数演示
 *   Demo2  — LC 136  只出现一次的数字
 *   Demo3  — LC 137  只出现一次的数字 II（逐位统计 mod 3）
 *   Demo4  — LC 260  只出现一次的数字 III（异或分组）
 *   Demo5  — LC 191  位1的个数（Brian Kernighan）
 *   Demo6  — LC 338  比特位计数（DP）
 *   Demo7  — LC 461 + LC 477  汉明距离 + 总汉明距离
 *   Demo8  — LC 371  两整数之和（位加法）
 *   Demo9  — LC 29   两数相除（位移法）
 *   Demo10 — LC 201  数字范围按位与（公共前缀）
 *   Demo11 — LC 89   格雷编码
 *   Demo12 — LC 190  颠倒二进制位
 *   Demo13 — LC 268  丢失的数字
 *   Demo14 — LC 78   子集（位枚举）
 *   Demo15 — 快速幂 + Gosper's Hack 演示
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <climits>
using namespace std;

/* ============================================================
 * Demo1: 基础位操作工具函数演示
 * ============================================================ */
namespace Demo1 {

// 获取第 k 位（0-indexed，从右数）
int getBit(int n, int k)   { return (n >> k) & 1; }
// 将第 k 位设为 1
int setBit(int n, int k)   { return n | (1 << k); }
// 将第 k 位设为 0
int clearBit(int n, int k) { return n & ~(1 << k); }
// 翻转第 k 位
int flipBit(int n, int k)  { return n ^ (1 << k); }
// 最低位的 1
int lowbit(int n)           { return n & (-n); }
// 消除最低位的 1
int clearLowest(int n)      { return n & (n - 1); }
// 判断 2 的幂
bool isPow2(int n)          { return n > 0 && (n & (n - 1)) == 0; }
// popcount
int popcount(int n) {
    int cnt = 0;
    while (n) { n &= n - 1; cnt++; }
    return cnt;
}

void printBinary(int n) {
    for (int i = 7; i >= 0; i--)
        cout << ((n >> i) & 1);
}

void run() {
    cout << "===== Demo1: 基础位操作工具函数 =====\n";
    int n = 0b01011010;  // 90
    cout << "n = 90 (二进制: ";
    printBinary(n);
    cout << ")\n";

    cout << "getBit(90, 3) = " << getBit(n, 3) << "  (第3位)\n";
    cout << "setBit(90, 0) = " << setBit(n, 0) << " = 91 (设第0位为1)\n";
    cout << "clearBit(90, 1) = " << clearBit(n, 1) << " = 88 (清第1位)\n";
    cout << "flipBit(90, 4) = " << flipBit(n, 4) << " = 74 (翻转第4位)\n";
    cout << "lowbit(90) = " << lowbit(n) << "  (最低位的1 = 2)\n";
    cout << "clearLowest(90) = " << clearLowest(n) << " = 88\n";
    cout << "popcount(90) = " << popcount(n) << "  (1的个数 = 4)\n";
    cout << "isPow2(64) = " << isPow2(64) << ",  isPow2(90) = " << isPow2(90) << "\n";
    cout << endl;
}
} // namespace Demo1

/* ============================================================
 * Demo2: LC 136 — 只出现一次的数字
 * 异或: a^a=0, a^0=a
 * ============================================================ */
namespace Demo2 {

int singleNumber(vector<int>& nums) {
    int result = 0;
    for (int num : nums)
        result ^= num;
    return result;
}

void run() {
    cout << "===== Demo2: LC 136 只出现一次的数字 =====\n";
    {
        vector<int> nums = {4, 1, 2, 1, 2};
        cout << "[4,1,2,1,2] → " << singleNumber(nums) << "  (期望 4)\n";
    }
    {
        vector<int> nums = {2, 2, 1};
        cout << "[2,2,1] → " << singleNumber(nums) << "  (期望 1)\n";
    }
    cout << endl;
}
} // namespace Demo2

/* ============================================================
 * Demo3: LC 137 — 只出现一次的数字 II
 * 每个元素出现3次，只有一个出现1次 → 逐位 mod 3
 * ============================================================ */
namespace Demo3 {

// 方法一: 逐位统计 mod 3 （最直观）
int singleNumber_bitcount(vector<int>& nums) {
    int result = 0;
    for (int bit = 0; bit < 32; bit++) {
        int sum = 0;
        for (int num : nums)
            sum += (num >> bit) & 1;
        if (sum % 3)
            result |= (1 << bit);
    }
    return result;
}

// 方法二: 有限状态机 (ones, twos)
// 状态转移: 00 → 01 → 10 → 00 (循环计数 mod 3)
int singleNumber_fsm(vector<int>& nums) {
    int ones = 0, twos = 0;
    for (int num : nums) {
        ones = (ones ^ num) & ~twos;
        twos = (twos ^ num) & ~ones;
    }
    return ones;
}

void run() {
    cout << "===== Demo3: LC 137 只出现一次的数字 II =====\n";
    {
        vector<int> nums = {2, 2, 3, 2};
        cout << "[2,2,3,2]\n";
        cout << "  逐位法: " << singleNumber_bitcount(nums) << "  (期望 3)\n";
        cout << "  状态机: " << singleNumber_fsm(nums) << "  (期望 3)\n";
    }
    {
        vector<int> nums = {0, 1, 0, 1, 0, 1, 99};
        cout << "[0,1,0,1,0,1,99]\n";
        cout << "  逐位法: " << singleNumber_bitcount(nums) << "  (期望 99)\n";
        cout << "  状态机: " << singleNumber_fsm(nums) << "  (期望 99)\n";
    }
    cout << endl;
}
} // namespace Demo3

/* ============================================================
 * Demo4: LC 260 — 只出现一次的数字 III
 * 两个出现1次的数 → 全部异或 + 按最低位分组
 * ============================================================ */
namespace Demo4 {

vector<int> singleNumber(vector<int>& nums) {
    // 1. 全部异或 → xorAll = a ^ b
    int xorAll = 0;
    for (int num : nums) xorAll ^= num;

    // 2. 取最低位的 1 (a 和 b 在这一位不同)
    //    注意: xorAll 可能为 INT_MIN, 用 unsigned 防溢出
    int diff = xorAll & (-(unsigned)xorAll);

    // 3. 分组异或
    int a = 0, b = 0;
    for (int num : nums) {
        if (num & diff) a ^= num;
        else            b ^= num;
    }
    return {a, b};
}

void run() {
    cout << "===== Demo4: LC 260 只出现一次的数字 III =====\n";
    {
        vector<int> nums = {1, 2, 1, 3, 2, 5};
        auto res = singleNumber(nums);
        sort(res.begin(), res.end());
        cout << "[1,2,1,3,2,5] → [" << res[0] << "," << res[1]
             << "]  (期望 [3,5])\n";
    }
    {
        vector<int> nums = {-1, 0};
        auto res = singleNumber(nums);
        sort(res.begin(), res.end());
        cout << "[-1,0] → [" << res[0] << "," << res[1]
             << "]  (期望 [-1,0])\n";
    }
    cout << endl;
}
} // namespace Demo4

/* ============================================================
 * Demo5: LC 191 — 位1的个数 (Hamming Weight)
 * Brian Kernighan: n &= n-1 循环
 * ============================================================ */
namespace Demo5 {

int hammingWeight(uint32_t n) {
    int count = 0;
    while (n) {
        n &= (n - 1);   // 消除最低位的 1
        count++;
    }
    return count;
}

void run() {
    cout << "===== Demo5: LC 191 位1的个数 =====\n";
    cout << "11 (1011) → " << hammingWeight(11) << " 个1  (期望 3)\n";
    cout << "128 (10000000) → " << hammingWeight(128) << " 个1  (期望 1)\n";
    cout << "255 (11111111) → " << hammingWeight(255) << " 个1  (期望 8)\n";
    cout << endl;
}
} // namespace Demo5

/* ============================================================
 * Demo6: LC 338 — 比特位计数 0~n 每个数的 1 的个数
 * DP: dp[i] = dp[i & (i-1)] + 1
 * ============================================================ */
namespace Demo6 {

vector<int> countBits(int n) {
    vector<int> dp(n + 1, 0);
    for (int i = 1; i <= n; i++)
        dp[i] = dp[i & (i - 1)] + 1;
    return dp;
}

void run() {
    cout << "===== Demo6: LC 338 比特位计数 =====\n";
    int n = 10;
    auto res = countBits(n);
    cout << "0~" << n << " 各数的1个数: [";
    for (int i = 0; i <= n; i++) {
        if (i) cout << ",";
        cout << res[i];
    }
    cout << "]\n(期望 [0,1,1,2,1,2,2,3,1,2,2])\n\n";
}
} // namespace Demo6

/* ============================================================
 * Demo7: LC 461 汉明距离 + LC 477 总汉明距离
 * ============================================================ */
namespace Demo7 {

// LC 461: 两个数的汉明距离 = 异或后 popcount
int hammingDistance(int x, int y) {
    int xorVal = x ^ y;
    int count = 0;
    while (xorVal) {
        xorVal &= xorVal - 1;
        count++;
    }
    return count;
}

// LC 477: 数组中所有数对的总汉明距离
// 逐位统计: 第b位有c个1 → 贡献 c * (n - c)
int totalHammingDistance(vector<int>& nums) {
    int total = 0, n = nums.size();
    for (int bit = 0; bit < 32; bit++) {
        int countOnes = 0;
        for (int num : nums)
            countOnes += (num >> bit) & 1;
        total += countOnes * (n - countOnes);
    }
    return total;
}

void run() {
    cout << "===== Demo7: LC 461 & 477 汉明距离 =====\n";
    cout << "LC 461: hammingDistance(1, 4) = " << hammingDistance(1, 4)
         << "  (期望 2, 因为 001 ^ 100 = 101)\n";

    vector<int> nums = {4, 14, 2};
    cout << "LC 477: totalHammingDistance([4,14,2]) = " << totalHammingDistance(nums)
         << "  (期望 6)\n";
    cout << "  分析: d(4,14)=2, d(4,2)=2, d(14,2)=2 → 总6\n\n";
}
} // namespace Demo7

/* ============================================================
 * Demo8: LC 371 — 两整数之和（不用 +/- 运算符）
 * 异或 = 无进位加法, (a&b)<<1 = 进位
 * ============================================================ */
namespace Demo8 {

int getSum(int a, int b) {
    while (b != 0) {
        unsigned carry = (unsigned)(a & b) << 1;  // 注意用unsigned防未定义行为
        a = a ^ b;         // 无进位加
        b = (int)carry;    // 进位继续加
    }
    return a;
}

void run() {
    cout << "===== Demo8: LC 371 两整数之和（位加法）=====\n";
    cout << "getSum(1, 2) = " << getSum(1, 2) << "  (期望 3)\n";
    cout << "getSum(-2, 3) = " << getSum(-2, 3) << "  (期望 1)\n";
    cout << "getSum(15, 27) = " << getSum(15, 27) << "  (期望 42)\n";
    cout << "getSum(-1, 1) = " << getSum(-1, 1) << "  (期望 0)\n\n";
}
} // namespace Demo8

/* ============================================================
 * Demo9: LC 29 — 两数相除（不用乘法、除法、mod）
 * 核心：用减法+移位模拟除法
 * ============================================================ */
namespace Demo9 {

int divide(int dividend, int divisor) {
    // 特殊情况：溢出
    if (dividend == INT_MIN && divisor == -1)
        return INT_MAX;

    // 转为 long long 取绝对值运算
    long long a = llabs((long long)dividend);
    long long b = llabs((long long)divisor);
    bool negative = (dividend < 0) ^ (divisor < 0);

    long long result = 0;
    while (a >= b) {
        long long temp = b, multiple = 1;
        // 倍增：找最大的 b * 2^k ≤ a
        while (a >= (temp << 1)) {
            temp <<= 1;
            multiple <<= 1;
        }
        a -= temp;
        result += multiple;
    }

    return negative ? -(int)result : (int)result;
}

void run() {
    cout << "===== Demo9: LC 29 两数相除 =====\n";
    cout << "divide(10, 3) = " << divide(10, 3) << "  (期望 3)\n";
    cout << "divide(7, -2) = " << divide(7, -2) << "  (期望 -3)\n";
    cout << "divide(-2147483648, -1) = " << divide(INT_MIN, -1)
         << "  (期望 2147483647, 溢出处理)\n";
    cout << "divide(100, 7) = " << divide(100, 7) << "  (期望 14)\n\n";
}
} // namespace Demo9

/* ============================================================
 * Demo10: LC 201 — 数字范围按位与
 * 结果 = left 和 right 的二进制公共前缀
 * ============================================================ */
namespace Demo10 {

int rangeBitwiseAnd(int left, int right) {
    int shift = 0;
    while (left < right) {
        left >>= 1;
        right >>= 1;
        shift++;
    }
    return left << shift;
}

void run() {
    cout << "===== Demo10: LC 201 数字范围按位与 =====\n";
    cout << "rangeBitwiseAnd(5, 7) = " << rangeBitwiseAnd(5, 7)
         << "  (期望 4, 公共前缀 01xx → 0100)\n";
    cout << "rangeBitwiseAnd(0, 0) = " << rangeBitwiseAnd(0, 0)
         << "  (期望 0)\n";
    cout << "rangeBitwiseAnd(1, 2147483647) = " << rangeBitwiseAnd(1, 2147483647)
         << "  (期望 0)\n";
    cout << "rangeBitwiseAnd(12, 15) = " << rangeBitwiseAnd(12, 15)
         << "  (期望 12, 1100 & 1101 & 1110 & 1111 = 1100)\n\n";
}
} // namespace Demo10

/* ============================================================
 * Demo11: LC 89 — 格雷编码
 * 第 i 个 Gray 码 = i ^ (i >> 1)
 * ============================================================ */
namespace Demo11 {

vector<int> grayCode(int n) {
    int total = 1 << n;
    vector<int> result(total);
    for (int i = 0; i < total; i++)
        result[i] = i ^ (i >> 1);
    return result;
}

void run() {
    cout << "===== Demo11: LC 89 格雷编码 =====\n";
    for (int n = 2; n <= 3; n++) {
        auto res = grayCode(n);
        cout << "n=" << n << ": [";
        for (int i = 0; i < (int)res.size(); i++) {
            if (i) cout << ",";
            cout << res[i];
        }
        cout << "]\n";
    }
    cout << "(n=2 期望 [0,1,3,2], n=3 期望 [0,1,3,2,6,7,5,4])\n\n";
}
} // namespace Demo11

/* ============================================================
 * Demo12: LC 190 — 颠倒二进制位
 * 逐位取出,反向放置
 * ============================================================ */
namespace Demo12 {

uint32_t reverseBits(uint32_t n) {
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        result = (result << 1) | (n & 1);
        n >>= 1;
    }
    return result;
}

void run() {
    cout << "===== Demo12: LC 190 颠倒二进制位 =====\n";
    // 43261596 = 00000010100101000001111010011100
    // reverse  = 00111001011110000010100101000000 = 964176192
    uint32_t n1 = 43261596;
    cout << "reverseBits(" << n1 << ") = " << reverseBits(n1)
         << "  (期望 964176192)\n";

    // 4294967293 = 11111111111111111111111111111101
    // reverse    = 10111111111111111111111111111111 = 3221225471
    uint32_t n2 = 4294967293u;
    cout << "reverseBits(" << n2 << ") = " << reverseBits(n2)
         << "  (期望 3221225471)\n\n";
}
} // namespace Demo12

/* ============================================================
 * Demo13: LC 268 — 丢失的数字
 * 方法一: 与下标异或  方法二: 高斯求和
 * ============================================================ */
namespace Demo13 {

// 方法一: 异或法 (与下标异或)
int missingNumber_xor(vector<int>& nums) {
    int n = nums.size();
    int xorAll = n;  // 下标 0~n-1，再加上 n
    for (int i = 0; i < n; i++)
        xorAll ^= i ^ nums[i];
    return xorAll;
}

// 方法二: 高斯求和
int missingNumber_sum(vector<int>& nums) {
    int n = nums.size();
    int expected = n * (n + 1) / 2;
    int actual = 0;
    for (int num : nums) actual += num;
    return expected - actual;
}

void run() {
    cout << "===== Demo13: LC 268 丢失的数字 =====\n";
    {
        vector<int> nums = {3, 0, 1};
        cout << "[3,0,1] 异或法→" << missingNumber_xor(nums)
             << "  求和法→" << missingNumber_sum(nums) << "  (期望 2)\n";
    }
    {
        vector<int> nums = {9,6,4,2,3,5,7,0,1};
        cout << "[9,6,4,2,3,5,7,0,1] 异或法→" << missingNumber_xor(nums)
             << "  求和法→" << missingNumber_sum(nums) << "  (期望 8)\n";
    }
    cout << endl;
}
} // namespace Demo13

/* ============================================================
 * Demo14: LC 78 — 子集（位枚举法）
 * 枚举 0 ~ 2^n - 1, 每个整数的二进制位表示选/不选
 * ============================================================ */
namespace Demo14 {

vector<vector<int>> subsets(vector<int>& nums) {
    int n = nums.size();
    int total = 1 << n;
    vector<vector<int>> result;

    for (int mask = 0; mask < total; mask++) {
        vector<int> subset;
        for (int i = 0; i < n; i++) {
            if (mask & (1 << i))
                subset.push_back(nums[i]);
        }
        result.push_back(subset);
    }
    return result;
}

void run() {
    cout << "===== Demo14: LC 78 子集（位枚举）=====\n";
    vector<int> nums = {1, 2, 3};
    auto res = subsets(nums);
    cout << "nums=[1,2,3] 的所有子集 (" << res.size() << " 个):\n";
    for (auto& sub : res) {
        cout << "  [";
        for (int i = 0; i < (int)sub.size(); i++) {
            if (i) cout << ",";
            cout << sub[i];
        }
        cout << "]\n";
    }
    cout << endl;
}
} // namespace Demo14

/* ============================================================
 * Demo15: 快速幂 + Gosper's Hack 演示
 * ============================================================ */
namespace Demo15 {

// 快速幂: base^exp % mod
long long quickPow(long long base, long long exp, long long mod) {
    long long result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1)
            result = result * base % mod;
        base = base * base % mod;
        exp >>= 1;
    }
    return result;
}

// Gosper's Hack: 枚举 n 个元素中选 k 个的所有组合 (用 bitmask)
void gospersHack(int n, int k) {
    cout << "从 " << n << " 个元素中选 " << k << " 个的所有 bitmask:\n";
    int mask = (1 << k) - 1;  // 最小的 k 位 mask
    int count = 0;
    while (mask < (1 << n)) {
        // 打印当前 mask 对应的选择
        cout << "  mask=" << mask << " (选: {";
        bool first = true;
        for (int i = 0; i < n; i++) {
            if (mask & (1 << i)) {
                if (!first) cout << ",";
                cout << i;
                first = false;
            }
        }
        cout << "})\n";
        count++;

        // Gosper's Hack 核心公式
        int c = mask & (-mask);      // 最低位的 1
        int r = mask + c;            // 进位
        mask = (((r ^ mask) >> 2) / c) | r;
    }
    cout << "  共 " << count << " 种组合\n";
}

void run() {
    cout << "===== Demo15: 快速幂 + Gosper's Hack =====\n";

    // 快速幂
    cout << "--- 快速幂 ---\n";
    cout << "2^10 % 1000 = " << quickPow(2, 10, 1000) << "  (期望 24, 因为1024%1000)\n";
    cout << "3^13 % 1000000007 = " << quickPow(3, 13, 1000000007)
         << "  (期望 1594323)\n";
    cout << "7^256 % 13 = " << quickPow(7, 256, 13) << "  (期望 9)\n\n";

    // Gosper's Hack
    cout << "--- Gosper's Hack ---\n";
    gospersHack(5, 3);  // C(5,3) = 10 种
    cout << endl;
}
} // namespace Demo15

/* ============================================================
 * main — 运行所有 Demo
 * ============================================================ */
int main() {
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║  专题十八：位运算 Bit Manipulation — 示例代码            ║\n";
    cout << "║  作者：大胖超 😜                                        ║\n";
    cout << "╚══════════════════════════════════════════════════════════╝\n\n";

    Demo1::run();
    Demo2::run();
    Demo3::run();
    Demo4::run();
    Demo5::run();
    Demo6::run();
    Demo7::run();
    Demo8::run();
    Demo9::run();
    Demo10::run();
    Demo11::run();
    Demo12::run();
    Demo13::run();
    Demo14::run();
    Demo15::run();

    cout << "===== 全部 Demo 运行完毕！=====\n";
    return 0;
}
