# test26：跨平台对齐分配与释放配对

## 目标
- 给低延迟工程补齐一个常见“隐形坑”：对齐分配 API 的跨平台差异。
- 演示统一封装 `aligned_alloc_portable/aligned_free_portable`，避免释放接口错配。

## 运行
```bash
g++ -std=c++17 -O2 -o test26 test26.cpp
./test26

# Windows (MSVC)
cl /std:c++17 /O2 /EHsc test26.cpp
```

## 关键结论
- POSIX/C++17 `aligned_alloc` 需要 `size % alignment == 0`；否则行为未定义或返回失败。
- Windows 常用 `_aligned_malloc/_aligned_free`，不能与 `std::free` 混用。
- 建议在项目中统一封装对齐分配与释放，避免平台分支散落在业务代码。

## 与 test13 的关系
- `test13` 中对象池、Arena、无锁池都依赖对齐分配。
- 统一封装后，可显式保证：
  1. 对齐要求一致；
  2. 分配/释放接口匹配；
  3. 迁移平台时风险点集中。
