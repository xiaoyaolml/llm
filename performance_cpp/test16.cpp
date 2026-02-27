// =============================================================================
// C++ 高性能编程: SIMD (Single Instruction, Multiple Data) 深度解析
// =============================================================================
// SIMD 是现代 CPU 实现数据并行化的核心技术，通过单条指令处理多个数据，
// 大幅提升计算密集型任务的性能。
//
// 本教程覆盖：
//   1. SIMD 基础概念与适用场景
//   2. 手动 SIMD 编程 (Compiler Intrinsics)
//      - Intel SSE/AVX/AVX2/AVX-512 指令集
//      - ARM NEON 指令集
//   3. 自动矢量化 (Auto-Vectorization)
//      - 编译器如何自动生成 SIMD 代码
//      - 编写对自动矢量化友好的代码
//   4. SIMD 封装库与抽象
//      - 使用 std::experimental::simd (C++20/23 提案)
//      - 第三方库如 a-xy-z/highway
//   5. 实战案例：
//      - SIMD 优化数组求和
//      - SIMD 优化图像处理 (灰度转换)
//      - SIMD 优化 SAXPY (Single-precision A*X Plus Y)
// =============================================================================

#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
#include <cstdint>

// 引入 SIMD 指令集头文件
#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h> // Intel Intrinsics (SSE, AVX, AVX2, AVX-512)
#elif defined(__aarch64__)
#include <arm_neon.h> // ARM NEON Intrinsics
#endif

// =============================================================================
// 第1章：SIMD 基础概念
// =============================================================================
// SIMD (Single Instruction, Multiple Data) 是一种并行计算模型。
// CPU 内部有特殊的宽寄存器（如 128-bit, 256-bit, 512-bit），
// 一条 SIMD 指令可以同时对寄存器内的多个数据元素执行相同的操作。
//
// 例如，一个 128-bit 的 SSE 寄存器可以容纳：
//   - 4 个 32-bit 浮点数 (float)
//   - 2 个 64-bit 浮点数 (double)
//   - 16 个 8-bit 整数 (char/uint8_t)
//   - 8 个 16-bit 整数 (short/uint16_t)
//   - 4 个 32-bit 整数 (int/uint32_t)
//
// 适用场景：
//   - 线性代数 (矩阵/向量运算)
//   - 图像/视频处理 (像素操作)
//   - 物理模拟、科学计算
//   - 数据处理 (求和、过滤、转换)
//   - 任何数据并行、循环独立的计算密集型任务
// =============================================================================

// --- 1.1 标量版本 (Scalar) vs SIMD 版本 ---

// 标量版本：一次处理一个元素
void scalar_add(float* a, float* b, float* result, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        result[i] = a[i] + b[i];
    }
}

// SIMD 版本 (以 AVX 为例)：一次处理 8 个 float
void simd_add_avx(float* a, float* b, float* result, size_t n) {
#if defined(__AVX__)
    size_t i = 0;
    // AVX 寄存器 __m256 可以装 8 个 float
    for (; i + 7 < n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vresult = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&result[i], vresult);
    }
    // 处理剩余的不足 8 个的元素
    for (; i < n; ++i) {
        result[i] = a[i] + b[i];
    }
#else
    // 如果不支持 AVX，则回退到标量版本
    scalar_add(a, b, result, n);
#endif
}


// =============================================================================
// 第2章：手动 SIMD 编程 (Compiler Intrinsics)
// =============================================================================
// Intrinsics 是编译器提供的特殊函数，它们直接映射到 CPU 的 SIMD 指令。
// 命名约定 (Intel): _mm<width>_<op>_<type>
//   - <width>: 空 (128-bit SSE), 256 (AVX), 512 (AVX-512)
//   - <op>: 操作名 (e.g., add, mul, load, store)
//   - <type>: 数据类型 (ps: float, pd: double, epi32: 32-bit int)

// --- 2.1 实战：使用 AVX2 优化 SAXPY ---
// SAXPY: Y = a*X + Y
void saxpy_scalar(float a, const std::vector<float>& x, std::vector<float>& y) {
    for (size_t i = 0; i < x.size(); ++i) {
        y[i] = a * x[i] + y[i];
    }
}

void saxpy_avx2(float a, const std::vector<float>& x, std::vector<float>& y) {
#if defined(__AVX2__)
    size_t n = x.size();
    __m256 va = _mm256_set1_ps(a); // 将标量 a 广播到向量的所有通道

    size_t i = 0;
    for (; i + 7 < n; i += 8) {
        __m256 vx = _mm256_loadu_ps(&x[i]);
        __m256 vy = _mm256_loadu_ps(&y[i]);
        
        // FMA (Fused Multiply-Add) 指令: a*x+y
        // 相比分开的 mul 和 add，FMA 精度更高、延迟更低
        __m256 vresult = _mm256_fmadd_ps(va, vx, vy);
        
        _mm256_storeu_ps(&y[i], vresult);
    }

    // 处理剩余部分
    for (; i < n; ++i) {
        y[i] = a * x[i] + y[i];
    }
#else
    saxpy_scalar(a, x, y);
#endif
}

// --- 2.2 实战：使用 NEON 优化图像灰度转换 ---
// Gray = 0.299*R + 0.587*G + 0.114*B
void grayscale_scalar(const uint8_t* rgb, uint8_t* gray, size_t num_pixels) {
    for (size_t i = 0; i < num_pixels; ++i) {
        float r = rgb[i * 3 + 0];
        float g = rgb[i * 3 + 1];
        float b = rgb[i * 3 + 2];
        gray[i] = static_cast<uint8_t>(0.299f * r + 0.587f * g + 0.114f * b);
    }
}

void grayscale_neon(const uint8_t* rgb, uint8_t* gray, size_t num_pixels) {
#if defined(__aarch64__)
    const float32x4_t vr_coeff = vdupq_n_f32(0.299f);
    const float32x4_t vg_coeff = vdupq_n_f32(0.587f);
    const float32x4_t vb_coeff = vdupq_n_f32(0.114f);

    size_t i = 0;
    // NEON 128-bit 寄存器一次处理 8 个 uint8_t
    // 这里我们一次加载 8*3=24 个 uint8_t，处理 8 个像素
    for (; i + 7 < num_pixels; i += 8) {
        // 加载 24 个 u8 (RGBRGB...)
        uint8x8x3_t src = vld3_u8(&rgb[i * 3]);
        
        // 转换为 u16，再转为 float32
        uint16x8_t r_u16 = vmovl_u8(src.val[0]);
        uint16x8_t g_u16 = vmovl_u8(src.val[1]);
        uint16x8_t b_u16 = vmovl_u8(src.val[2]);

        // 分成两组 float32x4_t 进行计算
        float32x4_t r_low = vcvtq_f32_u32(vmovl_u16(vget_low_u16(r_u16)));
        float32x4_t g_low = vcvtq_f32_u32(vmovl_u16(vget_low_u16(g_u16)));
        float32x4_t b_low = vcvtq_f32_u32(vmovl_u16(vget_low_u16(b_u16)));

        float32x4_t gray_low = vmlaq_f32(vmlaq_f32(vmulq_f32(b_low, vb_coeff), g_low, vg_coeff), r_low, vr_coeff);
        
        // ... 同样处理 high part ...

        // 转换回 u32，再窄化为 u16, u8
        uint32x4_t gray_u32 = vcvtq_u32_f32(gray_low);
        uint16x4_t gray_u16 = vqmovn_u32(gray_u32);
        uint8x8_t result = vqmovn_u16(vcombine_u16(gray_u16, gray_u16 /* high part */));

        vst1_u8(&gray[i], result);
    }

    // 处理尾部
    grayscale_scalar(rgb + i * 3, gray + i, num_pixels - i);
#else
    grayscale_scalar(rgb, gray, num_pixels);
#endif
}


// =============================================================================
// 第3章：自动矢量化 (Auto-Vectorization)
// =============================================================================
// 现代编译器 (GCC, Clang, MSVC) 能够自动分析代码，并将简单的循环
// 转换为 SIMD 指令。
//
// 如何帮助编译器进行自动矢量化：
//   1. 使用简单的循环结构：for 循环，计数器从 0 开始，步长为 1。
//   2. 循环体内部无复杂的分支、函数调用或依赖关系。
//   3. 访问连续的内存 (数组/vector)，避免指针跳跃。
//   4. 使用 `#pragma omp simd` (OpenMP 4.0+) 或 `#pragma clang loop vectorize(enable)`
//      等指令强制编译器矢量化。
//   5. 确保没有数据依赖：循环的当前迭代不能依赖于之前的迭代结果。
//      (e.g., `a[i] = a[i-1] + 1;` 存在依赖，无法矢量化)
//   6. 对齐内存：将数据对齐到 32 或 64 字节边界可以提高加载/存储性能。
//      `alignas(32) float my_array[N];`

// --- 3.1 自动矢量化友好的代码 ---
void auto_vectorizable_mul(float* a, float* b, float* result, size_t n) {
    // 编译器可以轻松地将这个循环转换为 SIMD
    for (size_t i = 0; i < n; ++i) {
        result[i] = a[i] * b[i];
    }
}

// --- 3.2 使用 pragma 提示编译器 ---
void pragma_guided_sum(float* data, size_t n) {
    float sum = 0.0f;
    // 提示编译器这个循环可以被安全地矢量化
    #pragma omp simd reduction(+:sum)
    for (size_t i = 0; i < n; ++i) {
        sum += data[i];
    }
    // ...
}


// =============================================================================
// 第4章：SIMD 封装库与抽象
// =============================================================================
// 手写 Intrinsics 代码冗长、易错且不可移植。
// 使用封装库可以实现代码复用和跨平台。

// --- 4.1 std::experimental::simd (C++20/23 提案 P0214) ---
// 该提案旨在将 SIMD 类型和操作标准化到 C++ 中。
// 虽然还未正式进入标准，但 GCC 和 Clang 已有实现。
// 需要 `-fveclib=libmvec` (GCC) 或特定头文件。
/*
#include <experimental/simd>
namespace stdex = std::experimental;

void std_simd_add(float* a, float* b, float* result, size_t n) {
    using float_simd = stdex::native_simd<float>;
    size_t i = 0;
    for (; i + float_simd::size() <= n; i += float_simd::size()) {
        float_simd va(a + i, stdex::element_aligned);
        float_simd vb(b + i, stdex::element_aligned);
        float_simd vresult = va + vb;
        vresult.copy_to(result + i, stdex::element_aligned);
    }
    // ... tail loop
}
*/

// --- 4.2 第三方库：Google Highway ---
// Highway 是一个现代、可移植、高性能的 SIMD 库。
// 它提供了类型安全的抽象，并能自动选择当前平台最佳的指令集。
// https://github.com/google/highway

/*
#include "hwy/highway.h"
namespace hn = hwy::HWY_NAMESPACE;

void highway_saxpy(float a, const std::vector<float>& x, std::vector<float>& y) {
    const hn::ScalableTag<float> d;
    const auto va = hn::Set(d, a);
    
    for (size_t i = 0; i < x.size(); i += hn::Lanes(d)) {
        const auto vx = hn::Load(d, &x[i]);
        const auto vy = hn::Load(d, &y[i]);
        const auto vresult = hn::MulAdd(va, vx, vy);
        hn::Store(vresult, d, &y[i]);
    }
}
*/


// =============================================================================
// main: 性能对比演示
// =============================================================================
void run_benchmark() {
    const size_t N = 1024 * 1024 * 16;
    std::vector<float> x(N);
    std::vector<float> y(N);
    std::vector<float> z(N);
    float a = 2.5f;

    // 初始化数据
    std::iota(x.begin(), x.end(), 0.0f);
    std::iota(y.begin(), y.end(), 1.0f);

    // --- 标量版本 ---
    auto start = std::chrono::high_resolution_clock::now();
    saxpy_scalar(a, x, y);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> scalar_ms = end - start;
    std::cout << "[1] Scalar SAXPY: " << scalar_ms.count() << " ms\n";

    // 重置 y
    std::iota(y.begin(), y.end(), 1.0f);

    // --- SIMD 版本 ---
    start = std::chrono::high_resolution_clock::now();
    saxpy_avx2(a, x, y);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> simd_ms = end - start;
    std::cout << "[2] AVX2 SAXPY:   " << simd_ms.count() << " ms\n";

    if (simd_ms.count() > 0) {
        std::cout << "    -> Speedup: " << scalar_ms.count() / simd_ms.count() << "x\n";
    }
}

int main() {
    std::cout << "===== C++ 高性能编程: SIMD 教程 =====\n\n";
    
    run_benchmark();

    std::cout << "\n===== SIMD 演示结束 =====\n";
    return 0;
}
