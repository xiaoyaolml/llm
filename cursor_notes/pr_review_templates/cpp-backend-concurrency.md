# C++ 服务端 PR 审查模板：并发 / 生命周期

## 1. 适用场景

- 修改 `thread`、`mutex`、`atomic`、队列、线程池、回调注册表
- 修改连接关闭、取消、超时、shutdown、析构路径
- 修改共享状态

## 2. 建议先发给 Cursor 的 Prompt

```text
先不要修改代码。
请重点从并发与线程安全角度审查当前 PR：
1. 共享状态读写点
2. 锁顺序与死锁风险
3. 回调生命周期
4. 原子与内存序
5. shutdown / close / cancel 路径
6. 竞态、ABA、use-after-free 风险
7. 回归测试建议
```

## 3. Reviewer Checklist

### 线程安全

- [ ] 共享数据访问路径已识别
- [ ] 是否新增数据竞争风险
- [ ] 锁粒度是否合理
- [ ] 锁顺序是否一致

### 生命周期

- [ ] 对象销毁顺序明确
- [ ] 回调不会持有悬垂引用/指针
- [ ] 关闭连接 / 超时 / 取消后不会继续访问失效资源
- [ ] shutdown 路径能收敛后台任务

### 原子与无锁

- [ ] 原子变量用途明确
- [ ] 内存序说明充分
- [ ] 无锁结构的 ABA / 回收问题已考虑
- [ ] 不存在 double-close / re-entrancy 漏洞

## 4. 结论建议

### Blocking

- 存在 race / deadlock / use-after-free 风险
- shutdown 不可收敛
- 内存序明显不足或错误

### Non-blocking

- 锁争用可能偏高
- 注释和设计说明不足
- 压测 / stress test 建议补充
