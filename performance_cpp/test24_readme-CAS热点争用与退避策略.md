# test24：CAS 热点争用与退避策略（C++17）

## 定位

本专题补充 `test11` 在高争用 CAS 循环下的工程策略：
- 朴素 CAS 循环（一直重试）
- 指数退避 CAS 循环（`pause/yield`）

目标是说明：无锁并不等于无代价，热点争用下需要退避以减少总线争夺与失败重试风暴。

## 编译运行

```bash
# Linux / macOS
g++ -std=c++17 -O2 -pthread -o test24 test24.cpp && ./test24

# Windows (MSVC)
cl /std:c++17 /O2 /EHsc test24.cpp && test24.exe
```

## 输出解读

程序会输出两组时间：
- `plain CAS`
- `backoff CAS`

两者计数值应一致；时间差体现争用下退避策略的收益。

## 工程结论

- 退避通常在高竞争场景更有收益。
- 低竞争场景退避不一定更快。
- 退避上限与步进系数需结合业务压测调参。

## 与 `test11` 的关系

- `test11`：讲解 memory order 与 CAS 模式正确性。
- `test24`：补充“在正确前提下，如何降低 CAS 热点性能退化”。
