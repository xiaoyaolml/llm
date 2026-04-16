---
name: defi-protocol-review
description: DeFi协议审查技能 - 审查区块链金融应用的智能合约安全性、流动性池机制、价格预言机验证和治理机制检查
---

# 🌐 DeFi协议审查技能 (defi-protocol-review)

## 🎯 功能概述

此技能专门用于区块链金融(DeFi)应用的协议审查，确保智能合约、流动性池、价格预言机和治理机制符合安全性、可靠性要求，特别关注金融行业对安全性和合规性的严格要求。

## 🔧 支持的DeFi协议类型

- 智能合约安全性 (Smart Contract Security)
- 流动性池机制 (Liquidity Pool Mechanisms)
- 价格预言机验证 (Oracle Validation)
- 治理机制检查 (Governance Mechanisms)
- 代币经济模型 (Token Economics)
- 跨链协议验证 (Cross-chain Protocol Validation)

## 📋 主要审查功能

### 1. 智能合约安全性
- 检查重入攻击漏洞
- 验证整数溢出/下溢
- 审查访问控制和权限管理
- 确保交易回滚保护

### 2. 流动性池机制
- 检查流动性提供者激励模型
- 验证交易费用结构
- 审查LP代币分配机制
- 确保池平衡和价格发现机制

### 3. 价格预言机验证
- 检查预言机源多样性
- 验证价格更新频率和机制
- 审查异常价格处理
- 确保抗操纵能力

### 4. 治理机制检查
- 检查投票机制合理性
- 验证治理代币分配
- 审查提案流程
- 确保治理安全性和去中心化

### 5. 代币经济模型
- 检查代币分配合理性
- 验证通胀/通缩机制
- 审查代币效用模型
- 确保经济模型可持续性

### 6. 跨链协议验证
- 检查跨链通信安全性
- 验证资产桥接机制
- 审查跨链消息传递
- 确保跨链操作原子性

## 🛠 使用方法

```bash
# 基本使用
claude /defi-protocol-review contracts/

# 指定审查范围
claude /defi-protocol-review contracts/ --module security

# 指定特定组件
claude /defi-protocol-review contracts/ --component erc20,uniswap,vault

# 输出JSON格式报告
claude /defi-protocol-review contracts/ --output json

# 指定特定审查模块
claude /defi-protocol-review contracts/ --module security,oracles

# 详细模式
claude /defi-protocol-review contracts/ --verbose
```

## 📊 输出格式

### Markdown 输出
```markdown
# DeFi协议审查报告

## 概述
- 协议名称: uniswap-v3
- 审查时间: 2026-04-09 10:30:00
- 区块链: Ethereum

## 审查结果

### 智能合约安全
- ✅ 无重入攻击漏洞
- ✅ 无整数溢出问题
- ⚠️ 访问控制存在改进空间
- ✅ 交易回滚保护完备

### 流动性池
- ✅ 流动性提供者激励合理
- ✅ 交易费用结构合理
- ⚠️ LP代币分配需优化
- ✅ 池平衡机制有效

### 价格预言机
- ✅ 多源预言机配置
- ✅ 价格更新频率合理
- ✅ 异常价格处理完善
- ✅ 抗操纵能力良好

### 治理机制
- ✅ 投票机制合理
- ⚠️ 治理代币分配不均
- ✅ 提案流程健全
- ✅ 治理安全机制完善

### 代币经济
- ⚠️ 代币分配需优化
- ✅ 通胀机制合理
- ✅ 代币效用明确
- ✅ 经济模型可持续

## 建议
1. 优化访问控制机制
2. 重新分配LP代币分配比例
3. 平衡治理代币分配
4. 优化代币分配模型
```

### JSON 输出
```json
{
  "protocol_name": "uniswap-v3",
  "timestamp": "2026-04-09T10:30:00Z",
  "blockchain": "Ethereum",
  "review_results": {
    "security": {
      "status": "warning",
      "checks": [
        {"name": "reentrancy", "status": "pass"},
        {"name": "integer_overflow", "status": "pass"},
        {"name": "access_control", "status": "warning"},
        {"name": "transaction_rollback", "status": "pass"}
      ]
    },
    "liquidity_pool": {
      "status": "warning",
      "checks": [
        {"name": "lp_incentives", "status": "pass"},
        {"name": "fee_structure", "status": "pass"},
        {"name": "lp_token_distribution", "status": "warning"},
        {"name": "pool_balance", "status": "pass"}
      ]
    },
    "oracles": {
      "status": "pass",
      "checks": [
        {"name": "oracle_sources", "status": "pass"},
        {"name": "update_frequency", "status": "pass"},
        {"name": "anomaly_handling", "status": "pass"},
        {"name": "manipulation_resistance", "status": "pass"}
      ]
    },
    "governance": {
      "status": "warning",
      "checks": [
        {"name": "voting_mechanism", "status": "pass"},
        {"name": "token_distribution", "status": "warning"},
        {"name": "proposal_process", "status": "pass"},
        {"name": "governance_security", "status": "pass"}
      ]
    },
    "token_economics": {
      "status": "warning",
      "checks": [
        {"name": "token_distribution", "status": "warning"},
        {"name": "inflation_model", "status": "pass"},
        {"name": "token_utility", "status": "pass"},
        {"name": "sustainability", "status": "pass"}
      ]
    }
  },
  "recommendations": [
    "优化访问控制机制",
    "重新分配LP代币分配比例",
    "平衡治理代币分配",
    "优化代币分配模型"
  ]
}
```

## 🔍 审查规则

### 智能合约安全规则
- ✅ 必须有适当的重入保护机制
- ✅ 必须处理整数溢出和下溢
- ✅ 必须实施访问控制和权限管理
- ✅ 必须包含交易回滚保护
- ✅ 必须验证外部调用安全性

### 流动性池机制规则
- ✅ 激励模型应防止套利攻击
- ✅ 交易费用结构应合理
- ✅ LP代币分配应公平
- ✅ 池平衡机制应有效
- ✅ 价格发现机制应准确

### 价格预言机规则
- ✅ 必须有多源预言机配置
- ✅ 价格更新频率应合理
- ✅ 必须有异常价格处理机制
- ✅ 必须有抗操纵能力
- ✅ 预言机验证应可靠

### 治理机制规则
- ✅ 投票机制应合理
- ✅ 治理代币分配应公平
- ✅ 提案流程应健全
- ✅ 治理安全机制应完善
- ✅ 治理机制应去中心化

### 代币经济模型规则
- ✅ 代币分配应公平合理
- ✅ 通胀/通缩机制应合理
- ✅ 代币效用应明确
- ✅ 经济模型应可持续
- ✅ 代币应有明确价值主张

## 🧪 示例用法

### 验证Uniswap协议
```bash
claude /defi-protocol-review uniswap-contracts/ --module security,oracles
```

### 审查流动性池机制
```bash
claude /defi-protocol-review liquidity-pools/ --module liquidity-pool
```

### 检查价格预言机
```bash
claude /defi-protocol-review oracles/ --module oracles
```

### 验证治理机制
```bash
claude /defi-protocol-review governance/ --module governance
```

## 📚 注意事项

1. **环境准备**：需要访问智能合约代码和链上数据
2. **权限要求**：需要读取链上数据的权限
3. **依赖项**：需要连接到区块链节点
4. **性能考虑**：大规模链上查询可能需要较长时间
5. **合规要求**：确保审查过程符合金融行业安全规范

## 🚀 适用场景

### 1. DeFi协议开发
- 评估智能合约安全性
- 验证流动性池设计
- 审查价格预言机实现
- 检查治理机制

### 2. DeFi协议升级
- 验证升级后的安全性
- 检查升级兼容性
- 评估对流动性的影响
- 验证治理变更

### 3. 项目审计
- 评估协议安全风险
- 检查经济模型合理性
- 审查预言机可靠性
- 验证跨链操作安全

### 4. 项目上线
- 全面安全性检查
- 评估系统稳定性
- 验证经济模型可持续性
- 确保合规性

## 📈 与金融开发技能组的协同

此技能与以下金融开发技能配合使用效果最佳：

- `/code-review` - 通用代码审查
- `/compliance-review` - 合规性审查
- `/market-data-validate` - 市场数据质量验证
- `/system-monitoring` - 系统监控审查

通过组合使用这些技能，可以建立完整的DeFi协议开发和运维体系。