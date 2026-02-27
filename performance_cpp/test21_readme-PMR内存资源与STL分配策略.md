# test21：PMR 内存资源与 STL 分配策略

> 对应源码：`test21.cpp`
> 关联章节：补充 `test8` 第20章性能实践
> 标准：C++17

---

## 为什么补这个专题

`test8` 已覆盖容器/算法层面的性能实践，但对“分配器策略”几乎未展开。
在大量小对象构造场景下，分配器开销常是瓶颈。C++17 的 PMR 正是为此设计。

---

## 本专题内容

`test21.cpp` 对比三种方式：

1. 默认分配器：`std::vector<std::string>`
2. 线性 arena：`std::pmr::monotonic_buffer_resource`
3. 池化分配：`std::pmr::unsynchronized_pool_resource`

核心目标是展示：
- 如何给容器和字符串绑定同一个 memory resource；
- 如何在高频短生命周期对象场景减少分配开销；
- 为什么 PMR 常与批处理、请求级 arena 配合。

---

## 编译运行

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o test21 test21.cpp && ./test21
# 或
cl /std:c++17 /O2 /EHsc test21.cpp
```

---

## 实战建议

- `monotonic_buffer_resource`：适合“整批创建，整批销毁”的生命周期模型。
- `unsynchronized_pool_resource`：适合单线程下大量同类小对象分配。
- 跨线程共享资源时改用 `synchronized_pool_resource` 或线程局部资源。

> NOTE：PMR 不是万能加速器。若对象本身很大或分配次数不高，收益可能有限，需基准验证。
