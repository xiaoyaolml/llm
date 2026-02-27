// =============================================================================
// test22.cpp
// C++17 高并发补充专题：Hazard Pointer 安全回收（教学版）
// 编译：g++ -std=c++17 -O2 -pthread -o test22 test22.cpp
//       cl /std:c++17 /O2 /EHsc test22.cpp
// =============================================================================

#include <atomic>
#include <array>
#include <cassert>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

class Timer {
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start_;
    const char* label_;
public:
    explicit Timer(const char* label) : start_(Clock::now()), label_(label) {}
    ~Timer() {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start_).count();
        std::cout << "  [" << label_ << "] " << us << " us\n";
    }
};

namespace hp {

constexpr int MAX_HAZARD_SLOTS = 128;

struct HazardSlot {
    std::atomic<std::thread::id> owner;
    std::atomic<void*> ptr;

    HazardSlot() : owner(std::thread::id{}), ptr(nullptr) {}
};

class HazardDomain {
    std::array<HazardSlot, MAX_HAZARD_SLOTS> slots_{};

public:
    HazardSlot* acquire_slot() {
        std::thread::id expected{};
        const auto self = std::this_thread::get_id();

        for (auto& slot : slots_) {
            if (slot.owner.compare_exchange_strong(expected, self,
                                                   std::memory_order_acq_rel,
                                                   std::memory_order_relaxed)) {
                slot.ptr.store(nullptr, std::memory_order_release);
                return &slot;
            }
            expected = std::thread::id{};
        }
        return nullptr;
    }

    void release_slot(HazardSlot* slot) {
        if (!slot) return;
        slot->ptr.store(nullptr, std::memory_order_release);
        slot->owner.store(std::thread::id{}, std::memory_order_release);
    }

    bool is_hazard(void* p) const {
        for (const auto& slot : slots_) {
            if (slot.ptr.load(std::memory_order_acquire) == p) {
                return true;
            }
        }
        return false;
    }
};

HazardDomain& global_domain() {
    static HazardDomain domain;
    return domain;
}

class HazardGuard {
    HazardSlot* slot_{nullptr};

public:
    HazardGuard() {
        slot_ = global_domain().acquire_slot();
        if (!slot_) throw std::runtime_error("No hazard slot available");
    }

    ~HazardGuard() {
        global_domain().release_slot(slot_);
    }

    void protect(void* p) {
        slot_->ptr.store(p, std::memory_order_release);
    }

    void clear() {
        slot_->ptr.store(nullptr, std::memory_order_release);
    }
};

template <typename T>
class LockFreeStackHP {
    struct Node {
        T data;
        Node* next;
        explicit Node(T v) : data(std::move(v)), next(nullptr) {}
    };

    std::atomic<Node*> head_{nullptr};

    struct RetiredNode {
        Node* node;
        RetiredNode* next;
    };

    std::atomic<RetiredNode*> retired_{nullptr};
    static constexpr int RECLAIM_THRESHOLD = 64;
    std::atomic<int> retired_count_{0};

    void retire(Node* node) {
        auto* r = new RetiredNode{node, nullptr};
        r->next = retired_.load(std::memory_order_relaxed);
        while (!retired_.compare_exchange_weak(r->next, r,
                                               std::memory_order_release,
                                               std::memory_order_relaxed)) {}

        int n = retired_count_.fetch_add(1, std::memory_order_relaxed) + 1;
        if (n >= RECLAIM_THRESHOLD) {
            scan();
        }
    }

    void scan() {
        RetiredNode* list = retired_.exchange(nullptr, std::memory_order_acq_rel);
        if (!list) {
            retired_count_.store(0, std::memory_order_relaxed);
            return;
        }

        RetiredNode* keep = nullptr;
        int keep_count = 0;

        while (list) {
            RetiredNode* cur = list;
            list = list->next;

            if (global_domain().is_hazard(cur->node)) {
                cur->next = keep;
                keep = cur;
                ++keep_count;
            } else {
                delete cur->node;
                delete cur;
            }
        }

        if (keep) {
            RetiredNode* old = retired_.load(std::memory_order_relaxed);
            RetiredNode* tail = keep;
            while (tail->next) tail = tail->next;
            do {
                tail->next = old;
            } while (!retired_.compare_exchange_weak(old, keep,
                                                     std::memory_order_release,
                                                     std::memory_order_relaxed));
        }

        retired_count_.store(keep_count, std::memory_order_relaxed);
    }

public:
    ~LockFreeStackHP() {
        while (pop()) {}
        scan();

        RetiredNode* remain = retired_.exchange(nullptr, std::memory_order_acq_rel);
        while (remain) {
            RetiredNode* next = remain->next;
            delete remain->node;
            delete remain;
            remain = next;
        }
    }

    void push(T value) {
        Node* node = new Node(std::move(value));
        node->next = head_.load(std::memory_order_relaxed);
        while (!head_.compare_exchange_weak(node->next, node,
                                            std::memory_order_release,
                                            std::memory_order_relaxed)) {}
    }

    std::optional<T> pop() {
        HazardGuard guard;

        for (;;) {
            Node* old = head_.load(std::memory_order_acquire);
            if (!old) return std::nullopt;

            guard.protect(old);

            if (head_.load(std::memory_order_acquire) != old) {
                continue;
            }

            Node* next = old->next;
            if (head_.compare_exchange_strong(old, next,
                                              std::memory_order_acq_rel,
                                              std::memory_order_relaxed)) {
                T value = std::move(old->data);
                guard.clear();
                retire(old);
                return value;
            }
        }
    }
};

} // namespace hp

int main() {
    using hp::LockFreeStackHP;

    LockFreeStackHP<int> stack;
    constexpr int THREADS = 4;
    constexpr int PER_THREAD = 30000;

    {
        Timer t("hazard pointer stack push");
        std::vector<std::thread> threads;
        for (int tid = 0; tid < THREADS; ++tid) {
            threads.emplace_back([&, tid]() {
                for (int i = 0; i < PER_THREAD; ++i) {
                    stack.push(tid * PER_THREAD + i);
                }
            });
        }
        for (auto& th : threads) th.join();
    }

    std::atomic<int> popped{0};
    {
        Timer t("hazard pointer stack pop");
        std::vector<std::thread> threads;
        for (int i = 0; i < THREADS; ++i) {
            threads.emplace_back([&]() {
                while (stack.pop().has_value()) {
                    popped.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        for (auto& th : threads) th.join();
    }

    std::cout << "  popped=" << popped.load() << " expected=" << (THREADS * PER_THREAD) << "\n";
    std::cout << "\nDone.\n";
    return 0;
}
