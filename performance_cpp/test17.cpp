// =============================================================================
// C++17 编译期类型集合与算法
// =============================================================================
// 标准：严格 C++17
// 编译：g++ -std=c++17 -O2 -o test17 test17.cpp
//
// 主题：
//   本文件实现了 C++17 标准下的编译期 `TypeSet` 数据结构，
//   并提供了相关的集合算法（并集、交集、差集）。
//   最后，利用 `TypeSet` 解决了 `all_unique_types` 的问题。
// =============================================================================

#include <iostream>
#include <type_traits>

// =============================================================================
// 基础：TypeList 和元函数 (from test3)
// =============================================================================

template <typename... Ts>
struct TypeList {};

template <typename List>
struct Length;

template <typename... Ts>
struct Length<TypeList<Ts...>> {
    static constexpr size_t value = sizeof...(Ts);
};

template <typename List, typename T>
struct Contains;

template <typename T>
struct Contains<TypeList<>, T> : std::false_type {};

template <typename T, typename... Tail>
struct Contains<TypeList<T, Tail...>, T> : std::true_type {};

template <typename Head, typename... Tail, typename T>
struct Contains<TypeList<Head, Tail...>, T> : Contains<TypeList<Tail...>, T> {};


// =============================================================================
// 第1章：从 TypeList 到 TypeSet
// =============================================================================

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

// --- 测试 TypeSet 构建 ---
void test_to_set() {
    // 包含重复类型的 TypeList
    using MyListWithDuplicates = TypeList<int, double, int, char, double>;

    // 转换为 TypeSet
    using MySet = typename ToSet<MyListWithDuplicates>::type;

    // 验证结果
    // 预期 MySet 等价于 TypeList<char, double, int> (顺序可能不同)
    static_assert(Contains<MySet, int>::value, "Set should contain int");
    static_assert(Contains<MySet, double>::value, "Set should contain double");
    static_assert(Contains<MySet, char>::value, "Set should contain char");
    static_assert(Length<MySet>::value == 3, "Set should have 3 unique elements");

    static_assert(std::is_same_v<
        typename ToSet<TypeList<int, int, int>>::type,
        TypeList<int>
    >, "Set of {int, int, int} is {int}");

    std::cout << "[1] TypeSet construction tests passed.\n";
}


// =============================================================================
// 第2章：编译期集合算法
// =============================================================================

// --- 2.1 并集 (Union) ---
template <typename Set1, typename Set2>
struct Union;

template <typename... T1s, typename... T2s>
struct Union<TypeList<T1s...>, TypeList<T2s...>> {
    // 直接将两个列表连接，然后转换为集合即可
    using CombinedList = TypeList<T1s..., T2s...>;
    using type = typename ToSet<CombinedList>::type;
};

// --- 2.2 交集 (Intersection) ---
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

// --- 2.3 差集 (Difference) ---
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

// --- 算法测试 ---
void test_set_algorithms() {
    using SetA = typename ToSet<TypeList<int, double, char>>::type;
    using SetB = typename ToSet<TypeList<float, double, bool, char>>::type;

    // 并集: 预期包含 {int, double, char, float, bool}
    using UnionSet = typename Union<SetA, SetB>::type;
    static_assert(Length<UnionSet>::value == 5, "Union should have 5 elements");
    static_assert(Contains<UnionSet, int>::value && Contains<UnionSet, bool>::value, "Union test");

    // 交集: 预期包含 {double, char}
    using IntersectionSet = typename Intersection<SetA, SetB>::type;
    static_assert(Length<IntersectionSet>::value == 2, "Intersection should have 2 elements");
    static_assert(Contains<IntersectionSet, double>::value && Contains<IntersectionSet, char>::value, "Intersection test");
    static_assert(!Contains<IntersectionSet, int>::value, "Intersection test");

    // 差集 A - B: 预期包含 {int}
    using DifferenceSet = typename Difference<SetA, SetB>::type;
    static_assert(Length<DifferenceSet>::value == 1, "Difference should have 1 element");
    static_assert(Contains<DifferenceSet, int>::value, "Difference test");
    static_assert(!Contains<DifferenceSet, double>::value, "Difference test");

    std::cout << "[2] Set algorithm tests passed.\n";
}


// =============================================================================
// 第3章：应用：实现 all_unique_types
// =============================================================================

template <typename... Ts>
constexpr bool all_unique_types() {
    using OriginalList = TypeList<Ts...>;
    using Set = typename ToSet<OriginalList>::type;

    // 如果一个类型列表中的所有类型都是唯一的，
    // 那么将这个列表转换为集合后，其大小应该保持不变。
    return Length<OriginalList>::value == Length<Set>::value;
}

// --- 测试 all_unique_types ---
void test_all_unique() {
    static_assert(all_unique_types<int, double, char>(), "All unique");
    static_assert(!all_unique_types<int, double, int>(), "Not all unique (int duplicated)");
    static_assert(all_unique_types<>(), "Empty pack is unique");
    static_assert(all_unique_types<int>(), "Single element is unique");
    static_assert(!all_unique_types<char, short, int, long, int>(), "Not all unique (int duplicated at end)");

    std::cout << "[3] 'all_unique_types' tests passed.\n";
}


// =============================================================================
// main: 运行演示
// =============================================================================
int main() {
    std::cout << "===== C++17 编译期类型集合与算法 演示 =====\n\n";

    test_to_set();
    test_set_algorithms();
    test_all_unique();

    std::cout << "\n===== 所有 static_assert 通过 = 编译期正确性已验证 =====\n";
    return 0;
}
