---
name: concurrency-review
description: 金融高频并发代码审查（无锁编程、线程亲和性、消息传递优化）
---
# 金融高频并发审查

## 触发条件

用户输入 `/concurrency-review` 时执行，或当代码包含以下关键词时自动触发：

- `thread`、`mutex`、`atomic`、`lock`、`concurrent`、`parallel`、`async`、`coroutine`、`lock-free`、`wait-free`

## 执行步骤

### 1. 识别并发模式

```
# 金融并发模式识别
1. 无锁数据结构 (Lock-Free DS) - 订单簿、价格档位
2. 生产者-消费者模式 (Producer-Consumer) - 市场数据分发
3. 工作窃取 (Work-Stealing) - 风险计算负载均衡
4. 内存屏障 (Memory Barrier) - 时序关键操作
5. 线程池模式 (Thread Pool) - 订单处理
6. 异步I/O (Async I/O) - 网络通信
```

### 2. 获取代码上下文

```bash
# 检测并发相关代码
git diff | grep -E "(thread|mutex|atomic|concurrent|lock_free)"  # 识别并发代码
grep -r "std::thread\|std::mutex\|std::atomic" . --include=*.cpp  # 识别并发原语
grep -r "std::shared_mutex\|std::unique_lock" . --include=*.cpp   # 识别高级锁
```

### 3. 并发性能审查维度

#### 3.1 无锁编程 (Lock-Free Programming)

- [ ] **CAS 操作**: 使用 compare-and-swap 而非互斥锁
- [ ] **ABA 问题**: 使用版本号或双宽度原子操作避免 ABA
- [ ] **内存顺序**: 正确使用 acquire/release 或 sequentially consistent
- [ ] **内存泄漏**: 无锁数据结构的节点回收 (Hazard Pointers, RCU)

#### 3.2 线程亲和性 (Thread Affinity)

- [ ] **CPU 绑定**: 关键线程绑定到特定 CPU 核心
- [ ] **NUMA 优化**: 内存分配在本地 NUMA 节点
- [ ] **中断处理**: 避免中断影响关键线程
- [ ] **缓存亲和**: 频繁交互的线程靠近绑定

#### 3.3 消息传递 (Message Passing)

- [ ] **无锁队列**: 使用无锁队列而非共享内存
- [ ] **零拷贝**: 避免不必要的数据拷贝
- [ ] **批处理**: 合并小消息减少开销
- [ ] **背压控制**: 防止生产者压垮消费者

#### 3.4 内存优化 (Memory Optimization)

- [ ] **False Sharing**: 添加 padding 避免不同线程访问同一缓存行
- [ ] **缓存行对齐**: 关键数据结构按 64 字节对齐
- [ ] **内存预取**: 使用 `__builtin_prefetch` 优化访问
- [ ] **内存屏障**: 正确使用内存栅栏保证顺序

#### 3.5 时序关键 (Timing Critical)

- [ ] **实时调度**: 关键线程使用 SCHED_FIFO 或 SCHED_RR
- [ ] **优先级反转**: 避免优先级继承或优先级天花板
- [ ] **抢占延迟**: 最小化内核抢占延迟
- [ ] **中断屏蔽**: 关键操作期间屏蔽中断

### 4. 严重级别定义

- **高危**：直接影响延迟或导致竞态条件
  - 互斥锁在高频路径
  - 未处理的竞态条件
  - ABA 问题
  - 无锁数据结构内存泄漏
- **中危**：影响性能但不导致错误
  - False sharing
  - 次优的内存顺序
  - 未优化的线程调度
- **低危**：代码风格或文档问题
  - 未注释的并发设计
  - 未使用的同步原语

### 5. 输出格式

```
## 金融高频并发审查报告

**审查时间**: {{date_time}}
**并发模式**: {{concurrency_pattern}} (无锁队列/生产者-消费者/工作窃取)
**性能目标**: {{performance_target}} (延迟 <10μs / 吞吐 >100K TPS)

### 问题清单

| # | 文件 | 行号 | 问题描述 | 并发模式 | 严重级别 | 修复建议 |
|---|------|------|----------|----------|----------|----------|
| 1 | exchange/order_book.cpp | 156 | 高频路径使用互斥锁 | Lock-Free | 高 | 改用无锁队列 |

### 详细分析

#### 1. 无锁编程问题
**问题**: ABA 问题未处理
```cpp
// 当前代码
std::atomic<Node*> head;
Node* expected = head.load();
while (!head.compare_exchange_weak(expected, new_node)) {
    // ABA 问题：expected 可能指向新分配的相同地址
}
```

**修复建议**:

```cpp
// 使用双宽度原子操作或版本号
struct TaggedPointer {
    Node* ptr;
    uint32_t tag;  // 版本号
};

std::atomic<TaggedPointer> head;
TaggedPointer expected = head.load();
while (!head.compare_exchange_weak(expected, {new_node, expected.tag + 1})) {
    // 版本号防止 ABA 问题
}
```

#### 2. 线程亲和性问题

**问题**: 关键线程未绑定 CPU

```cpp
// 当前代码
std::thread worker(process_orders);

// 修复建议
std::thread worker([]() {
    // 绑定到特定 CPU 核心
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);  // 绑定到 CPU 2
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
  
    process_orders();
});
```

### 并发性能评估

- **锁争用**: {{lock_contestion_score}}/100 ({{contension_status}})
- **False Sharing**: {{false_sharing_score}}/100 ({{sharing_status}})
- **内存顺序**: {{memory_order_score}}/100 ({{order_status}})
- **线程调度**: {{scheduling_score}}/100 ({{scheduling_status}})

### 性能预测

⚠️ **警告**: 以下问题可能导致:

- 订单处理延迟增加 100-500μs
- 吞吐量下降 30-50%
- 在高负载下出现性能抖动
- 优先级反转导致的死锁

**建议**: 高危问题必须在生产部署前修复

```

### 6. 金融并发特定检查清单

#### 无锁数据结构检查
- [ ] 订单簿使用无锁跳表或哈希表
- [ ] 价格档位使用无锁队列
- [ ] 市场数据快照使用 Copy-on-Write
- [ ] 内存回收使用 Hazard Pointers 或 RCU
- [ ] ABA 问题使用版本号或双宽度原子操作

#### 消息传递检查
- [ ] 使用无锁 MPSC 或 SPSC 队列
- [ ] 市场数据分发使用发布-订阅模式
- [ ] 订单路由使用扇出模式
- [ ] 背压控制避免缓冲区溢出
- [ ] 零拷贝避免不必要的内存拷贝

#### 时序关键检查
- [ ] 关键线程使用实时调度策略
- [ ] 优先级反转防护机制
- [ ] 中断处理快速返回
- [ ] 内存预取优化访问模式
- [ ] 缓存预热关键数据结构

## 集成建议

### CI/CD 集成
```bash
# .gitlab-ci.yml
concurrency_review:
  stage: test
  script:
    - claude /concurrency-review src/concurrent/ --output json > concurrency_report.json
    - python validate_concurrency.py concurrency_report.json
  artifacts:
    reports:
      concurrency: concurrency_report.json
```

### 本地开发集成

```bash
# pre-commit hook
#!/bin/bash
if git diff --cached | grep -q "thread\|mutex\|atomic\|concurrent"; then
    echo "🔍 并发审查中..."
    claude /concurrency-review --high-priority-only
    if [ $? -ne 0 ]; then
        echo "❌ 发现高危并发问题，提交被阻止"
        exit 1
    fi
fi
```

## 最佳实践

### 1. 无锁编程最佳实践

```cpp
// 无锁单向链表实现
template<typename T>
class LockFreeList {
private:
    struct Node {
        T data;
        std::atomic<Node*> next;
        uint32_t version;  // 防止 ABA 问题
      
        Node(const T& val) : data(val), next(nullptr), version(0) {}
    };
  
    std::atomic<Node*> head;
  
public:
    void push(const T& value) {
        Node* new_node = new Node(value);
        Node* current_head = head.load();
      
        do {
            new_node->next = current_head;
        } while (!head.compare_exchange_weak(current_head, new_node));
    }
  
    bool pop(T& result) {
        Node* current_head = head.load();
      
        while (current_head) {
            Node* next = current_head->next.load();
          
            if (head.compare_exchange_strong(current_head, next)) {
                result = current_head->data;
                delete current_head;
                return true;
            }
        }
        return false;
    }
};
```

### 2. 线程亲和性最佳实践

```cpp
// CPU 亲和性设置
class ThreadAffinity {
public:
    static void set_affinity(std::thread& t, int cpu_id) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(cpu_id, &cpuset);
      
        pthread_setaffinity_np(t.native_handle(), 
                               sizeof(cpuset), 
                               &cpuset);
    }
  
    static void pin_to_core(int cpu_id) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(cpu_id, &cpuset);
      
        sched_setaffinity(0, sizeof(cpuset), &cpuset);
    }
};
```

### 3. 避免的陷阱

- ❌ 在高频路径使用互斥锁
- ❌ 忽略 ABA 问题
- ❌ 未正确设置内存顺序
- ❌ 不处理无锁数据结构的内存回收
- ❌ 忽略 False sharing
- ❌ 关键线程未绑定 CPU

## 更新日志

- **v1.0** (2026-04-08): 初始版本
  - 无锁编程、线程亲和性、消息传递审查
  - 金融并发模式识别和性能评估
  - 并发审查报告模板
