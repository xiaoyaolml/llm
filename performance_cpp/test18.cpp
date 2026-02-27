// =============================================================================
// test18: 零开销抽象的边界与基准陷阱（C++17）
// =============================================================================
// 编译:
//   g++ -std=c++17 -O2 -Wall -Wextra -o test18 test18.cpp
//   cl /std:c++17 /O2 /W4 /EHsc test18.cpp
// =============================================================================

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

class Timer {
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start_;
    const char* label_;
public:
    explicit Timer(const char* label) : start_(Clock::now()), label_(label) {}
    ~Timer() {
        auto end = Clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
        std::cout << "  [" << label_ << "] " << us << " us\n";
    }
};

static volatile long long g_sink = 0;

template <typename T>
inline void sink(const T& x) {
    g_sink += static_cast<long long>(x);
}

namespace case1 {

inline int fp_impl(int x) { return x + 1; }

template <typename F>
long long run_template(F f, int n) {
    long long sum = 0;
    for (int i = 0; i < n; ++i) sum += f(i);
    return sum;
}

long long run_fn_ptr(int (*f)(int), int n) {
    long long sum = 0;
    for (int i = 0; i < n; ++i) sum += f(i);
    return sum;
}

long long run_std_function(std::function<int(int)> f, int n) {
    long long sum = 0;
    for (int i = 0; i < n; ++i) sum += f(i);
    return sum;
}

void demo() {
    constexpr int N = 50'000'000;

    {
        Timer t("template callable");
        auto sum = run_template([](int x) { return x + 1; }, N);
        sink(sum);
    }
    {
        Timer t("function pointer");
        auto sum = run_fn_ptr(&fp_impl, N);
        sink(sum);
    }
    {
        Timer t("std::function");
        auto sum = run_std_function([](int x) { return x + 1; }, N);
        sink(sum);
    }
}

} // namespace case1

namespace case2 {

struct IOp {
    virtual int op(int x) const = 0;
    virtual ~IOp() = default;
};

struct AddOne : IOp {
    int op(int x) const override { return x + 1; }
};

void demo() {
    constexpr int N = 10'000'000;

    std::vector<int> contiguous(N);
    std::iota(contiguous.begin(), contiguous.end(), 0);

    std::vector<std::unique_ptr<IOp>> heap_poly;
    heap_poly.reserve(N);
    for (int i = 0; i < N; ++i) heap_poly.push_back(std::make_unique<AddOne>());

    {
        Timer t("contiguous arithmetic");
        long long sum = 0;
        for (int x : contiguous) sum += (x + 1);
        sink(sum);
    }
    {
        Timer t("heap virtual dispatch");
        long long sum = 0;
        for (int i = 0; i < N; ++i) sum += heap_poly[i]->op(i);
        sink(sum);
    }
}

} // namespace case2

namespace case3 {

void demo() {
    std::string text(20000, 'x');
    constexpr int N = 1'000'000;

    {
        Timer t("std::string substr copy");
        size_t total = 0;
        for (int i = 0; i < N; ++i) {
            std::string sub = text.substr(100, 200);
            total += sub.size();
        }
        sink(total);
    }

    {
        Timer t("std::string_view substr view");
        size_t total = 0;
        std::string_view sv(text);
        for (int i = 0; i < N; ++i) {
            std::string_view sub = sv.substr(100, 200);
            total += sub.size();
        }
        sink(total);
    }
}

} // namespace case3

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cout << "===== test18: 零开销抽象的边界与基准陷阱 =====\n\n";

    std::cout << "[1] 调用抽象成本对比\n";
    case1::demo();

    std::cout << "\n[2] 分派+内存布局耦合成本\n";
    case2::demo();

    std::cout << "\n[3] 所有权与视图成本\n";
    case3::demo();

    std::cout << "\n(g_sink=" << g_sink << ")\n";
    return 0;
}
