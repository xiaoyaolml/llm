// =============================================================================
// test25.cpp
// 主题：constexpr 容器安全查找与哈希分发边界
// 编译：g++ -std=c++17 -O2 -o test25 test25.cpp
// =============================================================================

#include <array>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string_view>

namespace demo_map {

template <typename K, typename V, size_t N>
class ConstexprMap {
    std::array<std::pair<K, V>, N> data_;

public:
    constexpr explicit ConstexprMap(std::array<std::pair<K, V>, N> data) : data_(data) {
        for (size_t i = 1; i < N; ++i) {
            auto key = data_[i];
            size_t j = i;
            while (j > 0 && data_[j - 1].first > key.first) {
                data_[j] = data_[j - 1];
                --j;
            }
            data_[j] = key;
        }
    }

    constexpr const V* find(const K& key) const {
        size_t lo = 0, hi = N;
        while (lo < hi) {
            size_t mid = lo + (hi - lo) / 2;
            if (data_[mid].first < key) lo = mid + 1;
            else hi = mid;
        }
        if (lo < N && data_[lo].first == key) return &data_[lo].second;
        return nullptr;
    }

    constexpr std::optional<V> get_copy(const K& key) const {
        if (const V* p = find(key)) return *p;
        return std::nullopt;
    }
};

} // namespace demo_map

namespace demo_hash_dispatch {

constexpr uint32_t fnv1a(std::string_view sv) {
    uint32_t h = 2166136261u;
    for (char c : sv) {
        h ^= static_cast<uint32_t>(c);
        h *= 16777619u;
    }
    return h;
}

std::string_view dispatch(std::string_view cmd) {
    switch (fnv1a(cmd)) {
        case fnv1a("start"):
            return cmd == "start" ? "启动" : "哈希冲突(start)";
        case fnv1a("stop"):
            return cmd == "stop" ? "停止" : "哈希冲突(stop)";
        case fnv1a("status"):
            return cmd == "status" ? "查询" : "哈希冲突(status)";
        default:
            return "未知";
    }
}

} // namespace demo_hash_dispatch

int main() {
    using demo_map::ConstexprMap;

    constexpr ConstexprMap status_map(std::array{
        std::pair{200, "OK"},
        std::pair{404, "Not Found"},
        std::pair{500, "Internal Error"}
    });

    constexpr auto ok = status_map.get_copy(200);
    static_assert(ok.has_value() && std::string_view(*ok) == "OK");

    std::cout << "[ConstexprMap]\n";
    if (auto v = status_map.get_copy(404)) {
        std::cout << "  404 -> " << *v << "\n";
    }
    if (auto v = status_map.get_copy(302)) {
        std::cout << "  302 -> " << *v << "\n";
    } else {
        std::cout << "  302 -> <missing> (安全返回)\n";
    }

    std::cout << "\n[Hash Dispatch]\n";
    for (auto cmd : {"start", "stop", "status", "stat"}) {
        std::cout << "  " << cmd << " -> " << demo_hash_dispatch::dispatch(cmd) << "\n";
    }

    return 0;
}
