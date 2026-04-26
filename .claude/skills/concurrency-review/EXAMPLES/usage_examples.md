# 并发审查示例

## 示例输入

```text
请参考 concurrency-review 的审查清单，审查以下代码的并发安全性：
（粘贴连接池 / 回调注册 / 队列实现代码）
```

## 期望输出

### 问题 1（阻塞）

- **位置**：`ConnectionPool::release()` 第 87 行
- **类型**：数据竞争
- **风险**：`idle_connections_` 在无锁状态下被 IO 线程和 worker 线程同时修改
- **修复**：用 `std::lock_guard` 保护，或改为 lockfree queue
- **验证**：TSan + 并发压测

### 问题 2（非阻塞）

- **位置**：`TimerManager::cancel()` 第 142 行
- **类型**：生命周期
- **风险**：取消后仍可能触发一次过期回调
- **建议**：回调内检查 `cancelled_` 标志或使用 `weak_ptr`
