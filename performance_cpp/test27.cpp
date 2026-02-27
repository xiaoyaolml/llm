// =============================================================================
// test27.cpp
// 主题：rdtsc 测量边界与序列化开销
// 编译：g++ -std=c++17 -O2 -o test27 test27.cpp
//       cl /std:c++17 /O2 /EHsc /arch:AVX2 test27.cpp
// =============================================================================

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

#if defined(_MSC_VER)
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#pragma intrinsic(__rdtscp)
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#define HAS_X86_TSC 1
#else
#define HAS_X86_TSC 0
#endif

#if HAS_X86_TSC
#if defined(_MSC_VER)
static inline uint64_t rdtsc_plain() {
    return __rdtsc();
}

static inline uint64_t rdtsc_serialized() {
    unsigned int aux = 0;
    int info[4];
    __cpuid(info, 0);
    const uint64_t t = __rdtscp(&aux);
    _mm_lfence();
    return t;
}
#else
#include <x86intrin.h>

static inline uint64_t rdtsc_plain() {
    return __rdtsc();
}

static inline uint64_t rdtsc_serialized() {
    unsigned int aux = 0;
    asm volatile("cpuid" : : "a"(0) : "rbx", "rcx", "rdx", "memory");
    const uint64_t t = __rdtscp(&aux);
    _mm_lfence();
    return t;
}
#endif
#endif

template <typename Func>
double bench_ns(Func&& fn, int iters) {
    const auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iters; ++i) {
        fn();
    }
    const auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::nano>(end - start).count() / iters;
}

int main() {
    constexpr int iters = 200000;
    std::cout << "test27: rdtsc 测量边界与序列化开销\n";

#if !HAS_X86_TSC
    std::cout << "当前平台非 x86，跳过 rdtsc 示例。\n";
    return 0;
#else
    volatile uint64_t sink = 0;

    const double plain_ns = bench_ns([&]() {
        sink ^= rdtsc_plain();
    }, iters);

    const double serialized_ns = bench_ns([&]() {
        sink ^= rdtsc_serialized();
    }, iters);

    std::vector<int> data(1 << 20, 1);
    volatile int sum = 0;

    auto cycle_loop_plain = [&]() {
        const uint64_t t0 = rdtsc_plain();
        for (int i = 0; i < 1024; ++i) {
            sum += data[i];
        }
        const uint64_t t1 = rdtsc_plain();
        return t1 - t0;
    };

    auto cycle_loop_serialized = [&]() {
        const uint64_t t0 = rdtsc_serialized();
        for (int i = 0; i < 1024; ++i) {
            sum += data[i];
        }
        const uint64_t t1 = rdtsc_serialized();
        return t1 - t0;
    };

    uint64_t accum_plain = 0;
    uint64_t accum_ser = 0;
    for (int i = 0; i < 2000; ++i) {
        accum_plain += cycle_loop_plain();
        accum_ser += cycle_loop_serialized();
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "rdtsc() 平均调用开销:           " << plain_ns << " ns\n";
    std::cout << "cpuid+rdtscp+lfence 平均开销:  " << serialized_ns << " ns\n";
    std::cout << "\n";
    std::cout << "短循环测量(未序列化) 平均:     " << (accum_plain / 2000.0) << " cycles\n";
    std::cout << "短循环测量(序列化)   平均:     " << (accum_ser / 2000.0) << " cycles\n";
    std::cout << "\n";
    std::cout << "NOTE: 未序列化 rdtsc 结果更易受乱序执行影响；\n";
    std::cout << "      序列化版本更稳健，但引入额外测量开销。\n";

    (void)sum;
    (void)sink;
    return 0;
#endif
}
