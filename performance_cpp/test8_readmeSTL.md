# 现代C++ STL详细指导教程与代码示例教学


test8.cpp 已创建完毕。20 章内容涵盖：

| 篇                   | 章 | 主题                                                                                         |
| -------------------- | -- | -------------------------------------------------------------------------------------------- |
| **一、容器**   | 1  | 序列容器 —`vector` / `array` / `deque` / `list` / `forward_list`                  |
|                      | 2  | 关联容器 —`set` / `map` / `multiset` / `multimap`                                   |
|                      | 3  | 无序容器 —`unordered_set` / `unordered_map` + 自定义哈希 + 性能对比                     |
|                      | 4  | 容器适配器 —`stack` / `queue` / `priority_queue`（含最小堆、Dijkstra）                |
|                      | 5  | 特殊容器 —`string` / `string_view` / `bitset`                                         |
|                      | 6  | C++17 更新 —`try_emplace` / `insert_or_assign` / 节点操作 / `merge`                   |
| **二、迭代器** | 7  | 迭代器分类与 `iterator_traits` / `advance` / `distance` / `next` / `prev`          |
|                      | 8  | 迭代器适配器 — 反向 / 插入 / 流 / 移动迭代器                                                |
| **三、算法**   | 9  | 非修改 —`find` / `count` / `all_of` / `search` / `equal` / `mismatch`           |
|                      | 10 | 修改 —`copy` / `transform` / `replace` / `remove` / `fill` / `shuffle`          |
|                      | 11 | 排序搜索 —`sort` / `stable_sort` / `partial_sort` / `nth_element` / 二分查找        |
|                      | 12 | 集合 —`set_union` / `set_intersection` / `set_difference`                             |
|                      | 13 | 数值 —`accumulate` / `inner_product` / `partial_sum` / `iota` / `gcd` / `clamp` |
|                      | 14 | 最值排列 —`minmax` / `next_permutation` / 堆操作                                        |
| **四、工具**   | 15 | 函数对象 — 标准函子 /`std::function` / Lambda 深入（泛型、`mutable`、移动捕获、IIFE）   |
|                      | 16 | `optional` / `variant` / `any`                                                         |
|                      | 17 | `tuple` 与结构化绑定                                                                       |
|                      | 18 | 智能指针 —`unique_ptr` / `shared_ptr` / `weak_ptr` + 循环引用                         |
| **五、实战**   | 19 | 组合拳 — 词频统计、TopK、分组统计、矩阵转置                                                 |
|                      | 20 | 性能陷阱 —`reserve`、`emplace_back`、容器选择基准测试 + 最佳实践表                      |

编译命令：

```
g++ -std=c++17 -O2 -o test8 test8.cpp
```
