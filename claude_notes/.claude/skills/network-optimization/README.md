# 网络性能优化技能 (network-optimization)

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/network-optimization/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行网络性能优化审查
claude /network-optimization src/network/

# 指定优化维度
claude /network-optimization src/network/ --dimension latency-optimization

# 输出JSON格式报告
claude /network-optimization src/network/ --output json

# 详细模式
claude /network-optimization src/network/ --verbose
```

## ✨ 核心功能

- **低延迟传输**：优化TCP参数、实现内核旁路技术
- **带宽利用**：评估压缩策略、优化数据包大小
- **连接管理**：审查连接池配置、验证连接复用
- **协议优化**：检查协议头设计、减少序列化开销
- **网络诊断**：识别拥塞点、分析数据包丢失
- **硬件加速**：验证DPDK/RDMA使用、检查网卡配置

## 📚 使用示例

### 1. 延迟优化分析
```bash
# 分析高频交易系统网络延迟
claude /network-optimization src/hft/ --dimension latency-optimization --system-type hft
```

### 2. 连接管理优化
```bash
# 优化API网关连接池配置
claude /network-optimization src/gateway/ --dimension connection-management --target api-gateway
```

### 3. 协议开销评估
```bash
# 评估FIX协议消息大小优化空间
claude /network-optimization src/fix/ --dimension protocol-overhead --protocol fix
```

## 🔌 集成指南

### 监控系统集成
```yaml
- name: Network Optimization Review
  run: claude /network-optimization src/ --output json > network_report.json
```

### 实时性能监控
```bash
# 每5分钟检查网络性能
/loop 5m claude /network-optimization src/ --high-priority-only
```

## 📅 版本历史

- **v1.0.0 (2026-04-09)**: 初始版本，包含网络性能核心优化功能