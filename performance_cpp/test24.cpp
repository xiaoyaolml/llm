// =============================================================================
// test24.cpp
// 无锁补充专题：CAS 热点争用下的退避策略（C++17）
// 编译：g++ -std=c++17 -O2 -pthread -o test24 test24.cpp
//       cl /std:c++17 /O2 /EHsc test24.cpp
// =============================================================================

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#endif

class Timer {
    using clk = std::chrono::high_resolution_clock;
    clk::time_point st_ = clk::now();
public:
    double ms() const {
        return std::chrono::duration<double, std::milli>(clk::now() - st_).count();
    }
};

static inline void cpu_relax() {
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
    _mm_pause();
#elif defined(__x86_64__) || defined(__i386__)
    _mm_pause();
#else
    std::this_thread::yield();
#endif
}

long long run_plain_cas_counter(int threads, int iters_per_thread) {
    std::atomic<long long> counter{0};
    std::vector<std::thread> workers;

    for (int t = 0; t < threads; ++t) {
        workers.emplace_back([&] {
            for (int i = 0; i < iters_per_thread; ++i) {
                long long cur = counter.load(std::memory_order_relaxed);
                while (!counter.compare_exchange_weak(
                    cur, cur + 1,
                    std::memory_order_relaxed,
                    std::memory_order_relaxed)) {
                }
            }
        });
    }

    for (auto& th : workers) th.join();
    return counter.load(std::memory_order_relaxed);
}

long long run_backoff_cas_counter(int threads, int iters_per_thread) {
    std::atomic<long long> counter{0};
    std::vector<std::thread> workers;

    for (int t = 0; t < threads; ++t) {
        workers.emplace_back([&] {
            for (int i = 0; i < iters_per_thread; ++i) {
                long long cur = counter.load(std::memory_order_relaxed);
                int backoff = 1;
                while (!counter.compare_exchange_weak(
                    cur, cur + 1,
                    std::memory_order_relaxed,
                    std::memory_order_relaxed)) {
                    for (int k = 0; k < backoff; ++k) cpu_relax();
                    backoff = std::min(backoff * 2, 256);
                }
            }
        });
    }

    for (auto& th : workers) th.join();
    return counter.load(std::memory_order_relaxed);
}

int main() {
    const int threads = std::max(2u, std::thread::hardware_concurrency());
    const int iters = 400000;

    std::cout << "CAS 热点争用对比 (threads=" << threads << ", each=" << iters << ")\n";

    Timer t1;
    auto plain = run_plain_cas_counter(threads, iters);
    double plain_ms = t1.ms();

    Timer t2;
    auto backoff = run_backoff_cas_counter(threads, iters);
    double backoff_ms = t2.ms();

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  plain CAS   : " << plain_ms << " ms, counter=" << plain << "\n";
    std::cout << "  backoff CAS : " << backoff_ms << " ms, counter=" << backoff << "\n";

    if (plain == backoff) {
        std::cout << "  correctness: ✓\n";
    } else {
        std::cout << "  correctness: ✗\n";
    }

    std::cout << "\nNOTE: 退避策略收益依赖核心数、缓存层次和调度器行为，请以实测为准。\n";
    return 0;
}
