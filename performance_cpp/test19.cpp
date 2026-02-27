// =============================================================================
// test19: C++20 并发补充专题 — 协作取消与阶段同步
// =============================================================================
// 目标：补充 test6 (C++17) 中未展开的现代并发能力：
//   1) std::jthread + std::stop_token（协作取消）
//   2) std::latch（一次性阶段同步）
//   3) std::barrier（循环阶段同步）
//
// 编译：
//   g++ -std=c++20 -O2 -pthread -o test19 test19.cpp
//   cl /std:c++20 /O2 /EHsc test19.cpp
// =============================================================================

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#if __has_include(<latch>)
#include <latch>
#define HAS_LATCH 1
#else
#define HAS_LATCH 0
#endif

#if __has_include(<barrier>)
#include <barrier>
#define HAS_BARRIER 1
#else
#define HAS_BARRIER 0
#endif

#if __has_include(<stop_token>)
#include <stop_token>
#define HAS_STOP_TOKEN 1
#else
#define HAS_STOP_TOKEN 0
#endif

using namespace std::chrono_literals;

namespace demo1 {

void run() {
#if HAS_STOP_TOKEN
    std::cout << "\n[1] jthread + stop_token (协作取消)\n";

    std::jthread worker([](std::stop_token st) {
        int tick = 0;
        while (!st.stop_requested()) {
            std::this_thread::sleep_for(20ms);
            if ((++tick % 5) == 0) {
                std::cout << "  worker tick=" << tick << "\n";
            }
        }
        std::cout << "  worker observed stop request, exiting.\n";
    });

    std::this_thread::sleep_for(180ms);
    worker.request_stop();
#else
    std::cout << "\n[1] stop_token unavailable on this toolchain.\n";
#endif
}

} // namespace demo1

namespace demo2 {

void run() {
#if HAS_LATCH
    std::cout << "\n[2] latch (一次性同步点)\n";

    constexpr int N = 4;
    std::latch done_latch(N);
    std::vector<std::jthread> workers;
    workers.reserve(N);

    for (int i = 0; i < N; ++i) {
        workers.emplace_back([i, &done_latch]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(30 + i * 15));
            std::cout << "  task " << i << " finished\n";
            done_latch.count_down();
        });
    }

    done_latch.wait();
    std::cout << "  all tasks arrived at latch.\n";
#else
    std::cout << "\n[2] latch unavailable on this toolchain.\n";
#endif
}

} // namespace demo2

namespace demo3 {

void run() {
#if HAS_BARRIER
    std::cout << "\n[3] barrier (多阶段同步)\n";

    constexpr int N = 3;
    std::barrier phase_barrier(N, []() noexcept {
        std::cout << "  -- phase completed --\n";
    });

    std::vector<std::jthread> workers;
    workers.reserve(N);

    for (int id = 0; id < N; ++id) {
        workers.emplace_back([id, &phase_barrier]() {
            for (int phase = 0; phase < 3; ++phase) {
                std::this_thread::sleep_for(std::chrono::milliseconds(15 + id * 8));
                std::cout << "  worker " << id << " reached phase " << phase << "\n";
                phase_barrier.arrive_and_wait();
            }
        });
    }
#else
    std::cout << "\n[3] barrier unavailable on this toolchain.\n";
#endif
}

} // namespace demo3

int main() {
    std::ios::sync_with_stdio(false);
    std::cout << "===== test19: C++20 并发补充专题 =====\n";

    demo1::run();
    demo2::run();
    demo3::run();

    std::cout << "\n===== done =====\n";
    return 0;
}
