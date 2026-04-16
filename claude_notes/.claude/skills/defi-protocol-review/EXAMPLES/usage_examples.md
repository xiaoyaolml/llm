# DeFi协议审查使用示例

## 示例1: 基本DeFi协议审查

```bash
# 审查智能合约安全性
claude /defi-protocol-review contracts/ --module security

# 输出详细报告
claude /defi-protocol-review contracts/ --output markdown
```

## 示例2: 指定审查模块

```bash
# 只验证流动性池机制
claude /defi-protocol-review liquidity/ --module liquidity-pool

# 审查价格预言机
claude /defi-protocol-review oracles/ --module oracles

# 验证治理机制
claude /defi-protocol-review governance/ --module governance

# 验证所有模块
claude /defi-protocol-review all/ --validate-all
```

## 示例3: 指定审查范围

```bash
# 审查Uniswap协议
claude /defi-protocol-review uniswap/ --component uniswap,erc20

# 审查Aave协议
claude /defi-protocol-review aave/ --component aave,vault

# 审查特定代币经济
claude /defi-protocol-review tokens/ --component token-economics
```

## 示例4: JSON格式输出

```bash
# 输出JSON格式便于程序处理
claude /defi-protocol-review contracts/ --output json

# 将结果保存到文件
claude /defi-protocol-review contracts/ --output json > defi_protocol_report.json
```

## 示例5: 详细模式

```bash
# 显示详细验证过程
claude /defi-protocol-review contracts/ --verbose

# 显示所有警告和错误
claude /defi-protocol-review contracts/ --strict
```

## 示例6: CI/CD集成

```yaml
# GitHub Actions 示例
name: DeFi Protocol Review

on: [push, pull_request]

jobs:
  defi-review:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    
    - name: Setup DeFi Tools
      run: |
        # 配置DeFi开发环境
        echo "Setting up DeFi protocol review environment..."
    
    - name: Review Smart Contract Security
      run: |
        claude /defi-protocol-review contracts/ --module security --output json > security_report.json
    
    - name: Review Liquidity Pool
      run: |
        claude /defi-protocol-review liquidity/ --module liquidity-pool --output json > liquidity_report.json
    
    - name: Review Oracles
      run: |
        claude /defi-protocol-review oracles/ --module oracles --output json > oracles_report.json
    
    - name: Review Governance
      run: |
        claude /defi-protocol-review governance/ --module governance --output json > governance_report.json
    
    - name: Check Critical Issues
      run: |
        # 检查是否有严重问题
        if jq '.review_results | any(.status == "fail")' security_report.json; then
          echo "❌ 智能合约安全性存在严重问题"
          exit 1
        elif jq '.review_results | any(.status == "fail")' liquidity_report.json; then
          echo "❌ 流动性池机制存在严重问题"
          exit 1
        else
          echo "✅ DeFi协议审查通过"
        fi
```

## 示例7: 高级用法

```bash
# 审查多个DeFi协议
claude /defi-protocol-review uniswap/ aave/ compound/ --recursive

# 审查特定文件类型
claude /defi-protocol-review contracts/ --file-pattern "*.sol"

# 设置自定义阈值
claude /defi-protocol-review oracles/ --price-deviation-threshold 0.05 --update-frequency-threshold 60
```

## 示例8: 与现有技能组合使用

```bash
# 首先进行通用代码审查
/review src/

# 然后进行智能合约安全审查
/defi-protocol-review contracts/ --module security

# 最后验证合规性
/compliance-review contracts/
```

## 示例9: 批量验证

```bash
# 验证整个DeFi协议体系
claude /defi-protocol-review all/ --recursive

# 验证特定协议类型
claude /defi-protocol-review protocols/ --protocol uniswap,aave,compound

# 验证特定组件
claude /defi-protocol-review protocols/ --component erc20,uniswap,vault
```

## 示例10: 与金融开发技能结合

```bash
# 先审查智能合约安全性
/defi-protocol-review contracts/ --module security

# 再审查市场数据质量
/market-data-validate contracts/ --validate-data-quality

# 验证系统监控
/system-monitoring contracts/ --dimension all
```

## 示例11: 定期审查设置

```bash
# 创建每日协议安全检查
# 在 crontab 中添加
0 9 * * * cd /path/to/project && claude /defi-protocol-review contracts/ --module security --output json > daily_security_$(date +%Y%m%d).json

# 创建每周全面检查
0 9 * * 1 cd /path/to/project && claude /defi-protocol-review contracts/ --validate-all --output json > weekly_defi_$(date +%Y%m%d).json

# 创建每月审计检查
0 9 1 * * cd /path/to/project && claude /defi-protocol-review contracts/ --validate-all --output json > monthly_audit_$(date +%Y%m%d).json
```

## 示例12: 生产环境审查

```bash
# 审查生产环境DeFi协议
claude /defi-protocol-review prod/ --environment production --validate-completeness

# 重点关注安全性
claude /defi-protocol-review prod/ --module security --output json > security_report.json

# 重点关注预言机可靠性
claude /defi-protocol-review prod/ --module oracles --output json > oracles_report.json
```

## 示例13: 跨链协议审查

```bash
# 验证跨链通信安全性
/defi-protocol-review cross-chain/ --module cross-chain

# 检查资产桥接机制
/defi-protocol-review bridges/ --module asset-bridging

# 审查跨链消息传递
/defi-protocol-review messages/ --module cross-chain-messages

# 验证跨链操作原子性
/defi-protocol-review transactions/ --module atomicity
```

这些示例展示了如何在不同的场景下使用DeFi协议审查技能，从基础验证到复杂的CI/CD集成，以及与其他金融开发技能的协同使用。