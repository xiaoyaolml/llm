// =============================================================================
// test26.cpp
// 主题：跨平台对齐内存分配封装（aligned alloc portability）
// 编译：g++ -std=c++17 -O2 -o test26 test26.cpp
//       cl /std:c++17 /O2 /EHsc test26.cpp
// =============================================================================

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <malloc.h>
#endif

static void* aligned_alloc_portable(size_t alignment, size_t size) {
#ifdef _WIN32
    return _aligned_malloc(size, alignment);
#else
    const size_t adjusted = (size + alignment - 1) & ~(alignment - 1);
    return std::aligned_alloc(alignment, adjusted);
#endif
}

static void aligned_free_portable(void* ptr) {
#ifdef _WIN32
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

class Timer {
    using clk = std::chrono::high_resolution_clock;
    clk::time_point start_ = clk::now();

public:
    double elapsed_us() const {
        return std::chrono::duration<double, std::micro>(clk::now() - start_).count();
    }
};

struct alignas(64) TickData {
    uint64_t ts;
    double bid;
    double ask;
    uint32_t qty;
    uint32_t flags;
};

int main() {
    constexpr size_t alignment = 64;
    constexpr size_t count = 1 << 18;
    const size_t bytes = count * sizeof(TickData);

    void* raw = aligned_alloc_portable(alignment, bytes);
    if (!raw) {
        std::cerr << "allocation failed\n";
        return 1;
    }

    const auto addr = reinterpret_cast<uintptr_t>(raw);
    std::cout << "address: 0x" << std::hex << addr << std::dec << "\n";
    std::cout << "aligned(" << alignment << "): " << ((addr % alignment == 0) ? "yes" : "no")
              << "\n";

    auto* data = static_cast<TickData*>(raw);
    for (size_t i = 0; i < count; ++i) {
        data[i] = TickData{static_cast<uint64_t>(i), 100.0 + i * 0.01, 100.01 + i * 0.01, 100, 0};
    }

    {
        Timer t;
        volatile double sum = 0.0;
        for (size_t i = 0; i < count; ++i) {
            sum += data[i].bid + data[i].ask;
        }
        std::cout << "scan " << count << " items: " << std::fixed << std::setprecision(1)
                  << t.elapsed_us() << " us\n";
    }

    aligned_free_portable(raw);
    return 0;
}
