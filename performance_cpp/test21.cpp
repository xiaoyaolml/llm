// =============================================================================
// test21: STL PMR (Polymorphic Memory Resource) 实战补充
// =============================================================================
// 目标：补充 test8 在 STL 内存分配策略上的空白，演示 C++17 PMR 的实用模式。
//
// 编译：
//   g++ -std=c++17 -O2 -Wall -Wextra -o test21 test21.cpp
//   cl /std:c++17 /O2 /EHsc test21.cpp
// =============================================================================

#include <chrono>
#include <iostream>
#include <memory_resource>
#include <string>
#include <vector>

class Timer {
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start_;
    const char* label_;
public:
    explicit Timer(const char* label) : start_(Clock::now()), label_(label) {}
    ~Timer() {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start_).count();
        std::cout << "  [" << label_ << "] " << us << " us\n";
    }
};

void baseline_default_allocator() {
    constexpr int N = 200000;
    Timer t("std::vector<std::string> default allocator");

    std::vector<std::string> v;
    v.reserve(N);
    for (int i = 0; i < N; ++i) {
        v.emplace_back("item_" + std::to_string(i));
    }

    std::cout << "  size=" << v.size() << "\n";
}

void pmr_monotonic_buffer() {
    constexpr int N = 200000;

    // 预分配一块连续内存作为 arena
    std::vector<std::byte> arena(8 * 1024 * 1024);
    std::pmr::monotonic_buffer_resource pool(arena.data(), arena.size());

    Timer t("pmr::vector<pmr::string> monotonic_buffer_resource");

    std::pmr::vector<std::pmr::string> v{&pool};
    v.reserve(N);
    for (int i = 0; i < N; ++i) {
        v.emplace_back("item_", &pool);
        v.back() += std::to_string(i);
    }

    std::cout << "  size=" << v.size() << "\n";
}

void pmr_unsynchronized_pool() {
    constexpr int N = 200000;
    std::pmr::unsynchronized_pool_resource pool;

    Timer t("pmr::vector<pmr::string> unsynchronized_pool_resource");

    std::pmr::vector<std::pmr::string> v{&pool};
    v.reserve(N);
    for (int i = 0; i < N; ++i) {
        v.emplace_back("item_", &pool);
        v.back() += std::to_string(i);
    }

    std::cout << "  size=" << v.size() << "\n";
}

int main() {
    std::ios::sync_with_stdio(false);

    std::cout << "===== test21: STL PMR 补充 =====\n";
    baseline_default_allocator();
    pmr_monotonic_buffer();
    pmr_unsynchronized_pool();

    std::cout << "\nNOTE: 结果依赖编译器/标准库实现/字符串长度与硬件。\n";
    return 0;
}
