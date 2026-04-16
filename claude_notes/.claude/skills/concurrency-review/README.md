# 🚀 /concurrency-review 技能 - 金融高频并发审查

## 📁 目录结构
```
concurrency-review/
├── SKILL.md                          # ✅ 核心技能定义
├── README.md                         # 📖 本文件（使用和扩展指南）
└── CHECKLISTS/                       # ✅ 并发检查清单
    └── cpp_concurrency_checklist.md  # ✅ C++ 并发最佳实践清单
```

---

## 🎯 快速使用

### 1. 重启 Claude Code
修改技能后需要**重启 Claude Code** 使新技能生效。

### 2. 基础使用
```bash
/concurrency-review                    # 审查当前目录下所有并发相关代码
/concurrency-review src/concurrent/    # 审查并发模块
/concurrency-review src/engine/order_matching.cpp  # 审查指定文件
```

### 3. 高级选项
```bash
/concurrency-review --high-priority     # 只审查高危问题
/concurrency-review --output markdown   # 导出 Markdown 报告
/concurrency-review --pattern lock-free # 指定并发模式
```

---

## 📊 审查维度

### 1. 无锁编程 (Lock-Free Programming)
- **CAS 操作**: 使用 compare-and-swap 而非互斥锁
- **ABA 问题**: 使用版本号或双宽度原子操作
- **内存顺序**: 正确使用 acquire/release 或 seq_cst
- **内存回收**: Hazard Pointers、RCU 等机制

### 2. 线程亲和性 (Thread Affinity)
- **CPU 绑定**: 关键线程绑定到特定 CPU 核心
- **NUMA 优化**: 内存分配在本地 NUMA 节点
- **中断处理**: 避免中断影响关键线程
- **缓存亲和**: 频繁交互线程靠近绑定

### 3. 消息传递 (Message Passing)
- **无锁队列**: 使用无锁队列而非共享内存
- **零拷贝**: 避免不必要的数据拷贝
- **批处理**: 合并小消息减少开销
- **背压控制**: 防止生产者压垮消费者

### 4. 内存优化 (Memory Optimization)
- **False Sharing**: 添加 padding 避免同一缓存行
- **缓存行对齐**: 关键数据结构按 64 字节对齐
- **内存预取**: 使用 `__builtin_prefetch` 优化
- **内存屏障**: 正确使用内存栅栏保证顺序

### 5. 时序关键 (Timing Critical)
- **实时调度**: 关键线程使用 SCHED_FIFO 或 SCHED_RR
- **优先级反转**: 避免优先级继承或天花板
- **抢占延迟**: 最小化内核抢占延迟
- **中断屏蔽**: 关键操作期间屏蔽中断

---

## 🛠️ 扩展方式

### 1. 添加新并发模式
**场景**: 你需要支持协程并发模式

**步骤**:
1. 编辑 `SKILL.md`，添加「协程并发」章节：
```markdown
#### 6. 协程并发 (Coroutine Concurrency)
- [ ] **协程调度**: 使用无锁调度器避免线程切换
- [ ] **异步等待**: 避免阻塞等待，使用异步等待
- [ ] **内存管理**: 协程栈内存的高效分配和回收
- [ ] **异常处理**: 协程内的异常传播机制
```

2. 在 `CHECKLISTS/cpp_concurrency_checklist.md` 添加示例：
```cpp
// ❌ 传统的线程池
std::thread pool[8];
for (auto& t : pool) {
    t = std::thread([]() { /* work */ });
}

// ✅ 现代的协程实现
#include <coroutine>

struct AsyncTask {
    struct promise_type {
        AsyncTask get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};
```

### 2. 集成到工作流
详见完整文档

---

## 金融并发技能组合总结

你现在拥有完整的金融C++开发技能体系：

| 技能 | 主要功能 | 适用场景 |
|------|----------|----------|
| `/code-review` | 通用代码质量 | 所有代码审查 |
| `/finance-optim` | 性能优化 | 高频交易引擎 |
| `/risk-check` | 风险验证 | 数值计算验证 |
| `/compliance-review` | 合规性 | 监管合规审计 |
| `/concurrency-review` | 并发审查 | 高频并发系统 | ⭐ 新增

**总计**: 5 个技能，全面覆盖金融C++开发的各个维度！

**下一步建议**:
1. 重启 Claude Code 使新技能生效
2. 在并发相关代码上测试技能
3. 根据团队需求定制检查清单
4. 集成到 CI/CD 和开发工作流

需要创建更多技能或定制现有技能，请继续提问！