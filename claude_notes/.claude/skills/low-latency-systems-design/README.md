# 低延迟系统设计审查技能 (low-latency-systems-design)

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/low-latency-systems-design/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行低延迟系统设计审查
claude /low-latency-systems-design src/engine/

# 指定审查维度
claude /low-latency-systems-design src/engine/ --dimension latency-budget

# 指定延迟目标
claude /low-latency-systems-design src/engine/ --latency-target 10us

# 输出JSON格式报告
claude /low-latency-systems-design src/engine/ --output json

# 详细模式
claude /low-latency-systems-design src/engine/ --verbose
```

## ✨ 核心功能

- **延迟预算分析**：验证各组件延迟贡献与预算分配
- **关键路径优化**：消除交易路径上的阻塞点和冗余操作
- **内存架构设计**：优化内存布局与缓存友好性
- **网络栈优化**：实施内核旁路技术与零拷贝网络
- **硬件加速策略**：利用FPGA/GPU/SIMD指令优化
- **操作系统调优**：配置CPU隔离与中断亲和性
- **数据结构优化**：对齐缓存行与预取策略
- **并发模型设计**：实现无锁数据结构与消息传递

## 📚 使用示例

### 1. 延迟预算验证
```bash
# 检查10微秒延迟目标达成情况
claude /low-latency-systems-design src/engine/ --latency-target 10us --verbose
```

### 2. 网络栈优化
```bash
# 验证DPDK集成效果
claude /low-latency-systems-design src/network/ --dimension network-stack --target dpdk
```

### 3. 内存布局优化
```bash
# 检查缓存行对齐与内存池设计
claude /low-latency-systems-design src/memory/ --dimension memory-architecture --target cache-friendly
```

## 🔌 集成指南

### 量化交易系统集成
```yaml
- name: Low-Latency System Review
  run: claude /low-latency-systems-design src/ --output json > latency_report.json
```

### 实时性能监控
```bash
# 每小时检查延迟指标
/loop 60m claude /low-latency-systems-design src/engine/ --latency-target 10us
```

## 📅 版本历史

- **v1.0.0 (2026-04-09)**: 初始版本，包含低延迟系统核心审查功能