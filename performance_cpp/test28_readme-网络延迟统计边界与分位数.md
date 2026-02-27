# test28：网络延迟统计边界与分位数

## 目标
- 补充 `test15` 在“延迟测量方法学”上的工程边界。
- 演示三件事：预热（warmup）、基线扣除（clock/framework overhead）、分位数报告（P50/P99/P99.9）。

## 运行
```bash
g++ -std=c++17 -O2 -o test28 test28.cpp
./test28

# Windows (MSVC)
cl /std:c++17 /O2 /EHsc test28.cpp
```

## 关键结论
- 对短路径测量，计时器与框架开销可能与被测逻辑同量级，建议做基线扣除。
- 单看平均值会掩盖尾延迟，生产评估应同时报告 `P50/P99/P99.9`。
- 不同机器、内核、驱动与电源策略下，绝对数字不可直接横向比较。

## 与 test15 的关系
- `test15` 涵盖了 DPDK/Solarflare/RDMA/io_uring 等路径的延迟量级。
- `test28` 强调“如何测”与“如何报”同样重要，帮助避免把经验值当成普适结论。
