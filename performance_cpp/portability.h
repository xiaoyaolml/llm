#ifndef PORTABILITY_H
#define PORTABILITY_H

#include <cstddef>

// =============================================================================
// 编译器与平台检测
// =============================================================================

#if defined(_MSC_VER)
    #define COMPILER_MSVC
#elif defined(__GNUC__)
    #define COMPILER_GCC
#elif defined(__clang__)
    #define COMPILER_CLANG
#else
    #error "Unsupported compiler"
#endif

// =============================================================================
// 强制内联
// =============================================================================

#ifdef COMPILER_MSVC
    #define FORCE_INLINE __forceinline
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif

// =============================================================================
// 阻止编译器优化
// =============================================================================

#ifdef COMPILER_MSVC
    #include <intrin.h>
    #pragma intrinsic(_ReadWriteBarrier)
    template <typename T>
    void do_not_optimize(T&& val) {
        // volatile read to force value to be loaded
        (void)*(volatile T*)&val; 
        _ReadWriteBarrier();
    }
    #define clobber_memory() _ReadWriteBarrier()
#else // GCC/Clang
    template <typename T>
    void do_not_optimize(T&& val) {
        asm volatile("" : : "r,m"(val) : "memory");
    }
    inline void clobber_memory() {
        asm volatile("" : : : "memory");
    }
#endif

// =============================================================================
// restrict 关键字
// =============================================================================

#ifdef COMPILER_MSVC
    #define RESTRICT __declspec(restrict)
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
    #define RESTRICT __restrict__
#else
    #define RESTRICT
#endif

// =============================================================================
// 跨平台对齐内存分配
// =============================================================================

#include <cstdlib>
#ifdef COMPILER_MSVC
    #include <malloc.h> // For _aligned_malloc and _aligned_free
#endif

inline void* portable_aligned_alloc(size_t alignment, size_t size) {
#ifdef COMPILER_MSVC
    return _aligned_malloc(size, alignment);
#else
    // posix_memalign is another option but aligned_alloc is in C++17 standard library
    // and often available in <cstdlib> with modern compilers.
    return aligned_alloc(alignment, size);
#endif
}

inline void portable_aligned_free(void* ptr) {
#ifdef COMPILER_MSVC
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

#endif // PORTABILITY_H