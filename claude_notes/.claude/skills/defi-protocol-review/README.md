# DeFi协议审查技能 (defi-protocol-review)

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/defi-protocol-review/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行DeFi协议审查
claude /defi-protocol-review src/contracts/

# 指定审查维度
claude /defi-protocol-review src/contracts/ --dimension oracle-validation

# 输出JSON格式报告
claude /defi-protocol-review src/contracts/ --output json

# 详细模式
claude /defi-protocol-review src/contracts/ --verbose
```

## ✨ 核心功能

- **智能合约安全**：检测重入攻击、整数溢出等漏洞
- **流动性池**：验证AMM机制和价格曲线实现
- **价格预言机**：检查价格喂价安全性和防操纵机制
- **治理机制**：审查DAO投票和提案执行流程
- **代币经济**：评估代币分配和激励机制合理性
- **跨链协议**：验证桥接安全和资产锁定机制
- **权限管理**：检查敏感操作的访问控制

## 📚 使用示例

### 1. 智能合约安全审查
```bash
# 检测智能合约漏洞
claude /defi-protocol-review src/contracts/ --dimension security --severity critical
```

### 2. 流动性池机制验证
```bash
# 验证Uniswap V3核心合约
claude /defi-protocol-review src/uniswap/core/ --dimension liquidity-pool
```

### 3. 价格预言机检查
```bash
# 验证Chainlink价格喂价实现
claude /defi-protocol-review src/oracle/ --dimension oracle-validation --provider chainlink
```

## 🔌 集成指南

### Solidity项目集成
```yaml
- name: DeFi Protocol Review
  run: claude /defi-protocol-review src/contracts/ --output json > defi_report.json
```

### 预提交安全检查
```bash
#!/bin/sh
claude /defi-protocol-review --high-priority-only || exit 1
```

## 📅 版本历史

- **v1.0.0 (2026-04-09)**: 初始版本，包含DeFi核心协议审查功能