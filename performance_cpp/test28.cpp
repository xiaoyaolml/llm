// =============================================================================
// test28.cpp
// 主题：网络延迟统计边界（预热、基线扣除、分位数）
// 编译：g++ -std=c++17 -O2 -o test28 test28.cpp
//       cl /std:c++17 /O2 /EHsc test28.cpp
// =============================================================================

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

using clk = std::chrono::high_resolution_clock;

static inline uint64_t now_ns() {
    return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(
               clk::now().time_since_epoch())
        .count();
}

static double percentile(std::vector<double> data, double p) {
    if (data.empty()) return 0.0;
    const double pos = (p / 100.0) * (data.size() - 1);
    const size_t lo = (size_t)std::floor(pos);
    const size_t hi = (size_t)std::ceil(pos);
    std::nth_element(data.begin(), data.begin() + lo, data.end());
    const double vlo = data[lo];
    if (hi == lo) return vlo;
    std::nth_element(data.begin(), data.begin() + hi, data.end());
    const double vhi = data[hi];
    return vlo + (pos - lo) * (vhi - vlo);
}

int main() {
    std::cout << "test28: 网络延迟统计边界（预热/基线扣除/分位数）\n";

    constexpr int warmup = 2000;
    constexpr int samples = 20000;

    std::mt19937 rng(42);
    std::normal_distribution<double> base_dist(350.0, 60.0); // ns
    std::bernoulli_distribution tail_hit(0.01);

    std::vector<double> baseline;
    baseline.reserve(samples);

    for (int i = 0; i < warmup + samples; ++i) {
        const uint64_t t0 = now_ns();
        const uint64_t t1 = now_ns();
        const double dt = (double)(t1 - t0);
        if (i >= warmup) baseline.push_back(dt);
    }

    std::vector<double> measured;
    measured.reserve(samples);

    for (int i = 0; i < warmup + samples; ++i) {
        double v = std::max(30.0, base_dist(rng));
        if (tail_hit(rng)) v += 1500.0; // 模拟偶发尾延迟
        if (i >= warmup) measured.push_back(v);
    }

    const double baseline_p50 = percentile(baseline, 50.0);
    const double baseline_p99 = percentile(baseline, 99.0);

    std::vector<double> corrected = measured;
    for (double& x : corrected) {
        x = std::max(0.0, x - baseline_p50);
    }

    auto avg = [](const std::vector<double>& v) {
        return std::accumulate(v.begin(), v.end(), 0.0) / (double)v.size();
    };

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "baseline  P50/P99: " << baseline_p50 << " / " << baseline_p99 << " ns\n";
    std::cout << "measured  mean/P50/P99/P99.9: " << avg(measured) << " / "
              << percentile(measured, 50.0) << " / " << percentile(measured, 99.0) << " / "
              << percentile(measured, 99.9) << " ns\n";
    std::cout << "corrected mean/P50/P99/P99.9: " << avg(corrected) << " / "
              << percentile(corrected, 50.0) << " / " << percentile(corrected, 99.0) << " / "
              << percentile(corrected, 99.9) << " ns\n";

    std::cout << "\nNOTE:\n";
    std::cout << "1) 先预热再采样，避免冷启动污染结果。\n";
    std::cout << "2) 对极短路径，建议做基线扣除（时钟与框架开销）。\n";
    std::cout << "3) 延迟报告优先看分位数（P50/P99/P99.9）而不是仅平均值。\n";
    return 0;
}
