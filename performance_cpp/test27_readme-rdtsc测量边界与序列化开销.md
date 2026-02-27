# test27：rdtsc 测量边界与序列化开销

## 目标
- 补充 `test14` 中微基准测量方法的工程边界。
- 对比 `rdtsc` 直接读时钟与 `cpuid + rdtscp + lfence` 序列化方案的开销与稳定性。

## 运行
```bash
g++ -std=c++17 -O2 -o test27 test27.cpp
./test27

# Windows (MSVC)
cl /std:c++17 /O2 /EHsc /arch:AVX2 test27.cpp
```

## 关键结论
- `rdtsc` 本身开销低，但未序列化时容易被乱序执行影响，短代码段测量抖动更大。
- `cpuid + rdtscp + lfence` 测量通常更稳健，但会引入额外固定开销。
- 对于极短路径，测量框架开销可能与被测代码同量级，必须做空基线扣除并多次采样。

## 与 test14 的关系
- `test14` 中多处使用 cycle/延迟量级做教学说明。
- `test27` 进一步强调：
  1. 测量方法本身会改变结果；
  2. 固定“绝对数字”不具备跨平台普适性；
  3. 应结合 perf/PMU 与统计方法交叉验证。
