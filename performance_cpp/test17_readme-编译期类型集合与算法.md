# C++17 编译期类型集合与算法 (test17)

> **对应源码**: `test17.cpp`
> **标准**: 严格 C++17
> **主题**: 继 `test3` 的 `TypeList` 之后，本章将构建一个功能更强大的编译期 `TypeSet`，并用它来实现更复杂的类型算法，例如判断一个参数包中的所有类型是否唯一。

---

## 编译方式

```bash
# GCC / Clang
g++ -std=c++17 -O2 -Wall -Wextra -o test17 test17.cpp && ./test17

# MSVC
cl /std:c++17 /O2 /W4 /EHsc test17.cpp
```

---

## 教程目标

在 `test3` 中，我们介绍了 `TypeList` 并实现了一些基本操作（如 `At`, `Contains`, `Reverse`）。但 `TypeList` 允许重复类型，这在需要唯一性的场景下有所不便。本章我们将：

1.  **构建 `TypeSet`**: 一个保证类型唯一的编译期集合。
2.  **实现集合算法**: 如并集 (`Union`)、交集 (`Intersection`) 和差集 (`Difference`)。
3.  **解决实际问题**: 用 `TypeSet` 优雅地实现 `all_unique_types`，完善 `test3` 中留下的一个简化实现。

---

## 第1章：从 TypeList 到 TypeSet

`TypeSet` 的核心思想是：在向集合添加类型时，首先检查该类型是否已存在。如果存在，则不添加。

### 1.1 基础：TypeList 和 Contains

我们复用 `test3` 中的 `TypeList` 和 `Contains` 元函数作为基础。

```cpp
// from test3
template <typename... Ts> struct TypeList {};

template <typename List, typename T> struct Contains;
template <typename T> struct Contains<TypeList<>, T> : std::false_type {};
template <typename T, typename... Tail>
struct Contains<TypeList<T, Tail...>, T> : std::true_type {};
template <typename Head, typename... Tail, typename T>
struct Contains<TypeList<Head, Tail...>, T> : Contains<TypeList<Tail...>, T> {};
```

### 1.2 构建 TypeSet

`TypeSet` 本质上是一个 `TypeList`，但我们通过一个 `Insert` 元函数来保证其唯一性。

```cpp
// 元函数：向 TypeList 中安全地插入一个类型（如果不存在）
template <typename List, typename T>
struct Insert;

template <typename... Ts, typename T>
struct Insert<TypeList<Ts...>, T> {
    using type = std::conditional_t<
        Contains<TypeList<Ts...>, T>::value, // 如果已包含 T
        TypeList<Ts...>,                     // 则返回原列表
        TypeList<Ts..., T>                   // 否则追加 T
    >;
};

// 从一个 TypeList 构建 TypeSet
template <typename List>
struct ToSet;

template <>
struct ToSet<TypeList<>> {
    using type = TypeList<>; // 空列表的集合是空列表
};

template <typename Head, typename... Tail>
struct ToSet<TypeList<Head, Tail...>> {
    // 递归地将 Tail 转换为集合，然后将 Head 插入
    using TailSet = typename ToSet<TypeList<Tail...>>::type;
    using type = typename Insert<TailSet, Head>::type;
};
```

### 1.3 测试 TypeSet 构建

```cpp
// 包含重复类型的 TypeList
using MyListWithDuplicates = TypeList<int, double, int, char, double>;

// 转换为 TypeSet
using MySet = typename ToSet<MyListWithDuplicates>::type;

// 验证结果
// 预期 MySet 等价于 TypeList<char, double, int> (顺序可能不同)
static_assert(Contains<MySet, int>::value, "Set should contain int");
static_assert(Contains<MySet, double>::value, "Set should contain double");
static_assert(Contains<MySet, char>::value, "Set should contain char");
static_assert(std::is_same_v<
    typename ToSet<TypeList<int, int, int>>::type,
    TypeList<int>
>, "Set of {int, int, int} is {int}");
```

> **NOTE (实现细节)**:
> 我们的 `ToSet` 实现是递归的，并且 `Insert` 是在末尾追加。这会导致最终 `TypeSet` 中类型的顺序与原始 `TypeList` 中的首次出现顺序相反。例如，`TypeList<int, double, int>` 会变成 `TypeList<double, int>`。这个顺序通常不影响集合的逻辑，但需要注意。

---

## 第2章：编译期集合算法

基于 `TypeSet`，我们可以实现标准的集合运算。

### 2.1 并集 (Union)

将两个集合的所有元素合并，并保持唯一性。

```cpp
template <typename Set1, typename Set2>
struct Union;

template <typename... T1s, typename... T2s>
struct Union<TypeList<T1s...>, TypeList<T2s...>> {
    // 直接将两个列表连接，然后转换为集合即可
    using CombinedList = TypeList<T1s..., T2s...>;
    using type = typename ToSet<CombinedList>::type;
};
```

### 2.2 交集 (Intersection)

找出同时存在于两个集合中的所有元素。

```cpp
template <typename Set1, typename Set2>
struct Intersection;

template <typename Set2>
struct Intersection<TypeList<>, Set2> {
    using type = TypeList<>; // 与空集的交集是空集
};

template <typename Head, typename... Tail, typename Set2>
struct Intersection<TypeList<Head, Tail...>, Set2> {
    using TailIntersection = typename Intersection<TypeList<Tail...>, Set2>::type;
    // 如果 Head 存在于 Set2 中，则将其加入结果
    using type = std::conditional_t<
        Contains<Set2, Head>::value,
        typename Insert<TailIntersection, Head>::type,
        TailIntersection
    >;
};
```

### 2.3 差集 (Difference)

从第一个集合中移除所有存在于第二个集合中的元素。

```cpp
template <typename Set1, typename Set2>
struct Difference;

template <typename Set2>
struct Difference<TypeList<>, Set2> {
    using type = TypeList<>;
};

template <typename Head, typename... Tail, typename Set2>
struct Difference<TypeList<Head, Tail...>, Set2> {
    using TailDifference = typename Difference<TypeList<Tail...>, Set2>::type;
    // 如果 Head 不存在于 Set2 中，则将其加入结果
    using type = std::conditional_t<
        !Contains<Set2, Head>::value,
        typename Insert<TailDifference, Head>::type,
        TailDifference
    >;
};
```

### 2.4 算法测试

```cpp
using SetA = typename ToSet<TypeList<int, double, char>>::type;
using SetB = typename ToSet<TypeList<float, double, bool, char>>::type;

// 并集: {int, double, char, float, bool}
using UnionSet = typename Union<SetA, SetB>::type;
static_assert(Contains<UnionSet, int>::value && Contains<UnionSet, bool>::value, "Union test");

// 交集: {double, char}
using IntersectionSet = typename Intersection<SetA, SetB>::type;
static_assert(Contains<IntersectionSet, double>::value && Contains<IntersectionSet, char>::value, "Intersection test");
static_assert(!Contains<IntersectionSet, int>::value, "Intersection test");

// 差集 A - B: {int}
using DifferenceSet = typename Difference<SetA, SetB>::type;
static_assert(Contains<DifferenceSet, int>::value, "Difference test");
static_assert(!Contains<DifferenceSet, double>::value, "Difference test");
```

---

## 第3章：应用：实现 all_unique_types

现在，我们可以用 `TypeSet` 来优雅地解决 `test3` 中提出的问题：如何判断一个参数包中的所有类型是否都是唯一的？

### 3.1 核心思想

如果一个类型列表中的所有类型都是唯一的，那么将这个列表转换为集合后，其大小应该保持不变。

### 3.2 实现

```cpp
// 复用 test3 的 Length 元函数
template <typename List> struct Length;
template <typename... Ts>
struct Length<TypeList<Ts...>> {
    static constexpr size_t value = sizeof...(Ts);
};

template <typename... Ts>
constexpr bool all_unique_types() {
    using OriginalList = TypeList<Ts...>;
    using Set = typename ToSet<OriginalList>::type;

    // 比较原始列表的长度和集合的长度
    return Length<OriginalList>::value == Length<Set>::value;
}
```

### 3.3 测试

```cpp
static_assert(all_unique_types<int, double, char>(), "All unique");
static_assert(!all_unique_types<int, double, int>(), "Not all unique (int duplicated)");
static_assert(all_unique_types<>(), "Empty pack is unique");
static_assert(all_unique_types<int>(), "Single element is unique");
```

这个实现比 `test3` 中留下的 `return true;` 存根要完善得多，并且逻辑清晰、易于理解。它完美展示了构建更高级编译期数据结构（如 `TypeSet`）所带来的威力。

---

## 总结

本章我们基于 `TypeList` 构建了 `TypeSet`，一个在编译期保证类型唯一的集合。通过 `Insert` 和 `ToSet` 元函数，我们可以将任意 `TypeList` 转换为 `TypeSet`。

在此基础上，我们实现了标准的集合算法：`Union`、`Intersection` 和 `Difference`。

最后，我们利用 `TypeSet` 的特性——长度不变性——优雅地解决了 `all_unique_types` 的判断问题，展示了这些编译期数据结构在解决实际问题中的应用价值。这套技术是 C++17 中进行高级模板元编程的基石。