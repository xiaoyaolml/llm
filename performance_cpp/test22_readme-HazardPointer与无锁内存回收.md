# test22：Hazard Pointer 与无锁内存回收（C++17）

## 定位

本专题补足 `test9` 在无锁数据结构“安全内存回收”上的工程细节。`test9` 的无锁栈已提示 ABA 与回收问题，但教学实现主要使用延迟回收；本文件给出一个可运行的 Hazard Pointer 教学版实现。

## 为什么需要 Hazard Pointer

在无锁结构中，线程 A 读取到节点指针后，线程 B 可能已把该节点弹出并释放。若 A 继续解引用，就会触发悬垂指针（Use-After-Free）。

Hazard Pointer 的核心思想：
- 线程在访问节点前，先把节点地址发布到全局 hazard 槽。
- 回收线程删除节点前，扫描 hazard 槽。
- 仍被任何线程“保护”的节点暂不释放，放入退休列表等待下轮回收。

## 本实现包含

- `HazardDomain`：全局 hazard 槽管理。
- `HazardGuard`：线程本地 RAII 保护器。
- `LockFreeStackHP<T>`：Treiber 栈 + 退休链表 + 扫描回收。
- `main()`：4 线程并发 push / pop 的简要演示。

## 运行

```bash
# Linux / macOS
g++ -std=c++17 -O2 -pthread -o test22 test22.cpp && ./test22

# Windows (MSVC)
cl /std:c++17 /O2 /EHsc test22.cpp && test22.exe
```

## 结果解读

预期输出中 `popped` 应接近 `THREADS * PER_THREAD`，并且程序稳定结束，不出现崩溃或未定义行为。

## 工程注意事项

- 本实现是教学版：hazard 槽容量固定，回收阈值固定，未做 NUMA/批量优化。
- 生产实现通常还会考虑：
  - 动态槽管理；
  - 更细粒度的退休批处理；
  - 与对象池或分代回收结合；
  - 与 Epoch-Based Reclamation (EBR/QSBR) 的权衡。

## 与 `test9` / `test11` 的关系

- `test9`：给出高并发架构全景。
- `test11`：深入内存序与无锁语义。
- `test22`：把“无锁 + 安全回收”这一步补齐为可执行示例。
