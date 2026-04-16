# 金融开发技能组合完整指南 (最新版)

> 维护说明：这组 Claude Skills 已完成一轮结构评审。
> 建议优先参考 `REVIEW_AND_OPTIMIZATION.md` 中的收敛分组、去重建议和 Cursor 迁移方案，
> 不建议继续按 1:1 方式扩展高度重叠的同类 Skills。

## 📁 已创建的技能 (20个)

```
.claude/skills/
├── code-review/                    # 通用代码审查（增强版）
├── finance-optim/                  # 金融性能优化
├── risk-check/                     # 风险计算验证
├── compliance-review/              # 合规性审查
├── concurrency-review/             # 高频并发审查
├── protocol-validate/              # 金融协议验证
├── quant-backtest/                 # 量化策略回测验证
├── market-data-validate/           # 市场数据质量验证
├── ml-model-validate/              # 机器学习模型验证
├── cloud-native-review/            # 云原生应用审查
├── trading-simulation/             # 交易模拟器审查
├── system-monitoring/              # 系统监控审查
├── defi-protocol-review/           # DeFi协议审查
├── cpp-backend-review/             # C++后端专项审查
├── network-optimization/           # 网络性能优化
├── database-interaction-optimization/ # 数据库交互优化
├── performance-bottleneck-identification/ # 性能瓶颈识别
├── security-review/                # 安全审查
├── low-latency-systems-design/     # 低延迟系统设计
├── api-design-review/              # API设计审查
└── memory-management-review/       # C++内存管理审查
```

---

## 🎯 技能组合策略

### 🔴 核心技能包（必装）

适用于所有金融开发项目

| 技能                      | 触发命令                  | 主要功能                           |
| ------------------------- | ------------------------- | ---------------------------------- |
| `/code-review`          | `/review`               | 通用代码审查（安全、性能、可读性） |
| `/finance-optim`        | `/finance-optim`        | 金融特定性能优化                   |
| `/risk-check`           | `/risk-check`           | 风险计算数值验证                   |
| `/compliance-review`    | `/compliance-review`    | 合规性审查                         |
| `/concurrency-review`   | `/concurrency-review`   | 高频并发审查                       |
| `/protocol-validate`    | `/protocol-validate`    | 金融协议验证                       |
| `/quant-backtest`       | `/quant-backtest`       | 量化策略回测验证                   |
| `/market-data-validate` | `/market-data-validate` | 市场数据质量验证                   |
| `/ml-model-validate`    | `/ml-model-validate`    | 机器学习模型验证                   |
| `/cloud-native-review`  | `/cloud-native-review`  | 云原生应用审查                     |
| `/trading-simulation`   | `/trading-simulation`   | 交易模拟器审查                     |
| `/system-monitoring`    | `/system-monitoring`    | 系统监控审查                       |
| `/defi-protocol-review` | `/defi-protocol-review` | DeFi协议审查                       |
| `/cpp-backend-review`   | `/cpp-backend-review`   | C++后端专项审查                    |
| `/network-optimization`  | `/network-optimization`  | 网络性能优化                       |
| `/database-interaction-optimization` | `/database-interaction-optimization` | 数据库交互优化               |
| `/performance-bottleneck-identification` | `/performance-bottleneck-identification` | 性能瓶颈识别               |
| `/security-review` | `/security-review` | 安全审查               |
| `/low-latency-systems-design` | `/low-latency-systems-design` | 低延迟系统设计               |
| `/api-design-review` | `/api-design-review` | API设计审查               |
| `/memory-management-review` | `/memory-management-review` | C++内存管理审查          |

### 🧭 当前建议的收敛分组

为了降低内容重复和维护成本，建议按以下 6 组理解这批 Skills：

1. **通用与 C++ 后端**：`code-review`、`cpp-backend-review`
2. **性能 / 并发 / 低延迟**：`finance-optim`、`concurrency-review`、`memory-management-review`、`network-optimization`、`database-interaction-optimization`、`performance-bottleneck-identification`、`low-latency-systems-design`
3. **安全 / 合规**：`security-review`、`compliance-review`
4. **风险 / 数据 / 协议 / API**：`risk-check`、`quant-backtest`、`market-data-validate`、`protocol-validate`、`api-design-review`
5. **云原生 / 监控 / 演练**：`cloud-native-review`、`system-monitoring`、`trading-simulation`
6. **按需专项**：`ml-model-validate`、`defi-protocol-review`

> 这 6 组也更适合迁移到 Cursor：稳定规则放 Rules，流程化内容放 Playbooks，强领域专项放 Prompt 模板。

### 🟡 场景化使用建议

#### 场景 1: 高频交易系统开发

```bash
# 1. 性能优化审查（优先级最高）
/finance-optim src/engine/

# 2. 并发安全审查
/concurrency-review src/engine/

# 3. 协议实现验证
/protocol-validate src/fix/

# 4. 风险计算验证
/risk-check src/risk/

# 5. 合规性审查
/compliance-review src/audit/

# 6. 市场数据验证
/market-data-validate src/data/

# 7. 通用代码审查
/review src/
```

**审查顺序**: 性能 → 并发 → 协议 → 风险 → 数据 → 合规 → 通用

#### 场景 2: 风险管理系统开发

```bash
# 1. 风险计算验证（核心）
/risk-check src/models/ --output json

# 2. 合规性审查
/compliance-review src/reporting/

# 3. 性能优化
/finance-optim src/calculations/

# 4. 并发优化
/concurrency-review src/calculations/

# 5. 数据质量验证
/market-data-validate src/data/

# 6. 通用审查
/review src/
```

#### 场景 3: 监管报告系统开发

```bash
# 1. 合规性审查（核心）
/compliance-review src/reporting/ --framework mifid

# 2. 数据完整性检查
/compliance-review src/data/

# 3. 数据质量验证
/market-data-validate src/data/

# 4. 通用代码审查
/review src/
```

#### 场景 4: 交易所网关开发

```bash
# 1. 协议实现验证（核心）
/protocol-validate src/fix/ --protocol fix --version FIX.4.4

# 2. 并发性能审查
/concurrency-review src/gateway/

# 3. 性能优化
/finance-optim src/gateway/

# 4. 市场数据验证
/market-data-validate src/data/

# 5. 合规性审查
/compliance-review src/audit/

# 6. 通用审查
/review src/
```

#### 场景 5: 量化策略开发

```bash
# 1. 策略回测验证（核心）
/quant-backtest src/strategies/

# 2. 市场数据验证（核心）
/market-data-validate src/data/

# 3. 性能优化
/finance-optim src/strategies/

# 4. 风险计算验证
/risk-check src/risk_models/

# 5. 合规性审查
/compliance-review src/audit/

# 6. 通用审查
/review src/
```

---

## 🔧 CI/CD 集成示例

### GitHub Actions 完整流程

```yaml
name: Financial System Review

on: [push, pull_request]

jobs:
  financial-review:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
  
    - name: Run Code Review
      run: claude /review src/ --output json > review.json
  
    - name: Run Finance Optim
      run: claude /finance-optim src/engine/ --output json > optim.json
  
    - name: Run Risk Check
      run: claude /risk-check src/risk/ --output json > risk.json
  
    - name: Run Compliance Review
      run: claude /compliance-review src/audit/ --output json > compliance.json
  
    - name: Run Concurrency Review
      run: claude /concurrency-review src/concurrent/ --output json > concurrency.json
  
    - name: Run Protocol Validate
      run: claude /protocol-validate src/protocol/ --output json > protocol.json
  
    - name: Run Quant Backtest
      run: claude /quant-backtest src/strategies/ --output json > backtest.json
  
    - name: Run Market Data Validate
      run: claude /market-data-validate src/data/ --output json > data.json
  
    - name: Check High Priority Issues
      run: |
        for file in review.json optim.json risk.json compliance.json concurrency.json protocol.json backtest.json data.json; do
          if [ -f "$file" ]; then
            if jq '.issues.high_severity | length' "$file" | grep -q '[1-9]'; then
              echo "❌ 高危问题需要修复: $file"
              jq '.issues.high_severity' "$file"
              exit 1
            fi
          fi
        done
  
    - name: Upload Reports
      uses: actions/upload-artifact@v3
      with:
        name: review-reports
        path: |
          *.json
          *.md
```

---

## 📊 技能对比矩阵（最新版）

| 维度                 | code-review | finance-optim | risk-check  | compliance-review | concurrency-review | protocol-validate | quant-backtest | market-data-validate | ml-model-validate | cloud-native-review | trading-simulation | system-monitoring | defi-protocol-review | network-optimization | database-interaction-optimization | performance-bottleneck-identification |
| -------------------- | ----------- | ------------- | ----------- | ----------------- | ------------------ | ----------------- | -------------- | -------------------- | ----------------- | ------------------- | ------------------ | ----------------- | -------------------- |
| **延迟优化**   | ✅ 基础     | ✅✅✅ 深度   | ❌          | ❌                | ✅✅ 中等          | ❌                | ❌             | ❌                   | ❌                | ✅✅✅ 深度         | ❌                 | ❌               | ❌                 | ✅✅✅ 深度         | ✅✅✅ 深度         | ✅✅✅ 深度         |
| **吞吐量优化** | ✅ 基础     | ✅✅✅ 深度   | ❌          | ❌                | ✅✅✅ 深度        | ❌                | ❌             | ❌                   | ❌                | ✅✅✅ 深度         | ❌                 | ❌               | ❌                 | ✅✅✅ 深度         | ✅✅✅ 深度         | ✅✅✅ 深度         |
| **数值稳定性** | ✅ 基础     | ❌            | ✅✅✅ 深度 | ❌                | ❌                 | ❌                | ✅ 基础        | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ❌                 | ❌                  | ❌                 | ❌                 |
| **精度控制**   | ✅ 基础     | ❌            | ✅✅✅ 深度 | ❌                | ❌                 | ❌                | ✅ 基础        | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ❌                 | ❌                  | ❌                 | ❌                 |
| **审计追踪**   | ❌          | ❌            | ❌          | ✅✅✅ 深度       | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ✅✅✅ 深度      | ✅✅✅ 深度        |
| **访问控制**   | ✅ 基础     | ❌            | ❌          | ✅✅✅ 深度       | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ✅ 基础          | ✅✅✅ 深度        |
| **数据保护**   | ✅ 基础     | ❌            | ❌          | ✅✅✅ 深度       | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ✅ 基础          | ✅✅✅ 深度        |
| **安全漏洞**   | ✅✅✅ 深度 | ✅ 基础       | ✅ 基础     | ✅✅ 中等         | ✅ 中等            | ✅ 中等           | ✅ 中等        | ✅ 中等              | ✅ 中等           | ✅✅✅ 深度         | ✅✅✅ 深度        | ✅✅✅ 深度      | ✅✅✅ 深度        |
| **无锁编程**   | ❌          | ❌            | ❌          | ❌                | ✅✅✅ 深度        | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ❌                 |
| **线程亲和性** | ❌          | ❌            | ❌          | ❌                | ✅✅✅ 深度        | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ❌                 |
| **消息传递**   | ❌          | ❌            | ❌          | ❌                | ✅✅✅ 深度        | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ❌                 |
| **协议验证**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ✅✅✅ 深度       | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ✅✅✅ 深度        |
| **校验和**     | ❌          | ❌            | ❌          | ❌                | ❌                 | ✅✅✅ 深度       | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ✅✅✅ 深度        |
| **会话管理**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ✅✅✅ 深度       | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ✅✅✅ 深度        |
| **策略逻辑**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ✅✅✅ 深度    | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ❌                 |
| **数据质量**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ✅✅✅ 深度    | ✅✅✅ 深度          | ❌                | ❌                  | ❌                 | ❌               | ❌                 |
| **过拟合检测** | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ✅✅✅ 深度    | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ❌                 |
| **数据完整性** | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ✅✅✅ 深度          | ❌                | ❌                  | ❌                 | ❌               | ❌                 |
| **数据准确性** | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ✅✅✅ 深度          | ❌                | ❌                  | ❌                 | ❌               | ❌                 |
| **数据一致性** | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ✅✅✅ 深度          | ❌                | ❌                  | ❌                 | ❌               | ❌                 |
| **数据时效性** | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ✅✅✅ 深度          | ❌                | ❌                  | ❌                 | ❌               | ❌                 |
| **模型验证**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ✅✅✅ 深度       | ❌                  | ❌                 | ❌               | ❌                 |
| **特征工程**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ✅✅✅ 深度       | ❌                  | ❌                 | ❌               | ❌                 |
| **模型漂移**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ✅✅✅ 深度       | ❌                  | ❌                 | ❌               | ❌                 |
| **公平性检查** | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ✅✅✅ 深度       | ❌                  | ❌                 | ❌               | ❌                 |
| **容器化**     | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ✅✅✅ 深度         | ❌                 | ❌               | ❌                 |
| **微服务**     | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ✅✅✅ 深度         | ❌                 | ❌               | ❌                 |
| **自动扩缩容** | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ✅✅✅ 深度         | ❌                 | ❌               | ❌                 |
| **服务网格**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ✅✅✅ 深度         | ❌                 | ❌               | ❌                 |
| **CI/CD**      | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ✅✅✅ 深度         | ❌                 | ❌               | ❌                 |
| **压力测试**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ✅✅✅ 深度        | ❌               | ❌                 |
| **性能基准**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ✅✅✅ 深度        | ❌               | ❌                 |
| **故障恢复**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ✅✅✅ 深度        | ❌               | ❌                 |
| **容错能力**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ✅✅✅ 深度        | ❌               | ❌                 |
| **交易流程**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ✅✅✅ 深度        | ❌               | ❌                 |
| **监控指标**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ✅✅✅ 深度      | ❌                 |
| **告警规则**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ✅✅✅ 深度      | ❌                 |
| **日志收集**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ✅✅✅ 深度      | ❌                 |
| **性能阈值**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ✅✅✅ 深度      | ❌                 |
| **仪表板**     | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ✅✅✅ 深度      | ❌                 |
| **智能合约安全** | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ✅✅✅ 深度        |
| **流动性池**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ✅✅✅ 深度        |
| **预言机**     | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ✅✅✅ 深度        |
| **治理机制**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ✅✅✅ 深度        |
| **代币经济**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ✅✅✅ 深度        |
| **跨链协议**   | ❌          | ❌            | ❌          | ❌                | ❌                 | ❌                | ❌             | ❌                   | ❌                | ❌                  | ❌                 | ❌               | ✅✅✅ 深度        |

---

## 🚀 快速开始指南

### 1. 安装所有技能

```bash
# 技能已安装在 ~/.claude/skills/
# 重启 Claude Code 使技能生效
claude restart
```

### 2. 验证技能列表

```bash
# 检查可用技能
claude /help
```

应该看到：

- `/review` - 通用代码审查
- `/finance-optim` - 金融性能优化
- `/risk-check` - 风险计算验证
- `/compliance-review` - 合规性审查
- `/concurrency-review` - 高频并发审查
- `/protocol-validate` - 金融协议验证
- `/quant-backtest` - 量化策略回测验证
- `/market-data-validate` - 市场数据质量验证
- `/network-optimization` - 网络性能优化
- `/database-interaction-optimization` - 数据库交互优化
- `/performance-bottleneck-identification` - 性能瓶颈识别
- `/security-review` - 安全审查
- `/low-latency-systems-design` - 低延迟系统设计

### 3. 运行首次审查

```bash
# 测试通用审查
/review tools/excels.py

# 测试性能优化
/finance-optim tools/excels.py

# 测试风险计算
/risk-check tools/excels.py

# 测试合规性
/compliance-review tools/excels.py

# 测试并发审查
/concurrency-review tools/excels.py

# 测试协议验证
/protocol-validate tools/excels.py

# 测试量化回测
/quant-backtest tools/excels.py

# 测试市场数据验证
/market-data-validate tools/excels.py

# 测试网络性能优化
/network-optimization tools/excels.py

# 测试数据库交互优化
/database-interaction-optimization tools/excels.py

# 测试性能瓶颈识别
/performance-bottleneck-identification tools/excels.py

# 测试安全审查
/security-review tools/excels.py

# 测试低延迟系统设计
/low-latency-systems-design tools/excels.py
```

---

## 📋 团队协作建议

### 技能共享

```bash
# 创建团队技能仓库
git init finance-skills
cp -r ~/.claude/skills/* finance-skills/
git add . && git commit -m "Add financial development skills"
git remote add origin https://your-git-server/finance-skills.git
git push -u origin master

# 团队成员安装
git clone https://your-git-server/finance-skills.git ~/.claude/skills/
```

### 规则定制流程

1. **发现问题**: 团队发现新的代码模式或合规要求
2. **更新规则**: 编辑对应技能的 `RULES.md` 或 `CHECKLISTS/`
3. **测试验证**: 使用测试代码验证新规则
4. **团队评审**: 团队评审规则变更
5. **发布更新**: 更新团队技能仓库

---

## 🎓 学习路径建议

### 初级开发者

1. 从 `/code-review` 开始，理解通用代码质量标准
2. 学习 `/finance-optim` 基础性能优化
3. 了解金融代码的特殊要求

### 中级开发者

1. 深入 `/finance-optim` 性能优化技巧
2. 掌握 `/risk-check` 数值计算最佳实践
3. 学习 `/concurrency-review` 并发编程
4. 了解 `/protocol-validate` 协议实现
5. 学习 `/quant-backtest` 量化策略验证
6. 学习 `/market-data-validate` 市场数据验证
7. 学习 `/ml-model-validate` 机器学习模型验证
8. 学习 `/cloud-native-review` 云原生应用审查
9. 学习 `/trading-simulation` 交易模拟器审查
10. 学习 `/system-monitoring` 系统监控审查
11. 学习 `/defi-protocol-review` DeFi协议审查
12. 学习 `/cpp-backend-review` C++后端专项审查

### 高级开发者

1. 精通所有技能的高级用法
2. 能够定制和扩展现有技能
3. 建立团队技能库和规则体系
4. 设计新的金融领域特定技能
5. 指导团队成员使用技能体系

---

## 🔮 下一步技能建议

### 已创建的技能

✅ `/concurrency-review` - 高频并发审查
✅ `/protocol-validate` - 金融协议验证（FIX/FAST/OUCH/ITCH）
✅ `/quant-backtest` - 量化策略回测验证
✅ `/market-data-validate` - 市场数据质量检查

### 可选扩展技能

#### 1. 人工智能领域

- `/ml-model-validate` - 机器学习模型验证
  - 模型输入输出验证
  - 特征工程检查
  - 模型漂移检测
  - 公平性和偏见检查

#### 2. 区块链金融领域

- `/defi-protocol-review` - DeFi协议审查
  - 智能合约安全性
  - 流动性池机制
  - 价格预言机验证
  - 治理机制检查

#### 3. 云原生领域

- `/cloud-native-review` - 云原生应用审查
  - 容器化部署检查
  - 微服务架构验证
  - 服务网格配置
  - 自动扩缩容机制

#### 4. 系统运维领域

- `/trading-simulation` - 交易模拟器审查

  - 压力测试场景验证
  - 性能基准测试
  - 故障恢复测试
  - 容错能力评估
- `/system-monitoring` - 系统监控审查

  - 监控指标完整性
  - 告警规则合理性
  - 日志收集完整性
  - 性能阈值设置

---

## 💡 最佳实践

### 1. 定期审查

```bash
# 每日自动审查（crontab）
0 9 * * * cd /path/to/project && claude /review --high-priority-only

# 每周完整审查
0 9 * * 1 cd /path/to/project && ./run_complete_review.sh
```

### 2. 提交前审查

```bash
# Git pre-commit hook
#!/bin/bash
claude /review -s --high-priority-only || exit 1
```

### 3. 代码审查工作流

```bash
# 1. 本地开发
/finance-optim src/new_feature.cpp
/concurrency-review src/new_feature.cpp

# 2. 提交前
git add .
/review -s

# 3. PR 审查
/risk-check src/changed_files/
/compliance-review src/changed_files/
/protocol-validate src/changed_files/
/quant-backtest src/changed_files/
/market-data-validate src/changed_files/
```

### 4. 生产环境部署前

```bash
# 部署前完整审查
./run_pre_deploy_check.sh

# 脚本内容：
#!/bin/bash
echo "🚀 执行部署前完整审查..."

# 性能审查
claude /finance-optim src/ --output json > perf_report.json

# 并发审查
claude /concurrency-review src/ --output json > conc_report.json

# 协议验证
claude /protocol-validate src/ --output json > proto_report.json

# 风险验证
claude /risk-check src/ --output json > risk_report.json

# 合规审查
claude /compliance-review src/ --output json > comp_report.json

# 量化回测
claude /quant-backtest src/ --output json > backtest_report.json

# 数据验证
claude /market-data-validate src/ --output json > data_report.json

# 通用审查
claude /review src/ --output json > code_report.json

echo "✅ 部署前审查完成"
```

---

## 📞 支持与维护

### 技能维护

- **定期更新**: 每月检查一次规则更新
- **团队反馈**: 收集团队使用反馈
- **合规更新**: 跟踪法规变化并更新规则
- **技术演进**: 跟踪新技术发展更新技能

### 问题排查

- **技能不工作**: 重启 Claude Code
- **规则不匹配**: 检查关键词配置
- **报告格式错误**: 检查模板文件权限
- **性能问题**: 检查技能实现和规则复杂度

---

## 🎯 技能发展路线图

### 短期目标（1-3个月）

1. ✅ 完善现有8个核心技能
2. 🔄 建立技能更新和维护机制
3. 👥 在团队内推广使用
4. 📊 收集使用反馈并优化

### 中期目标（3-6个月）

1. 🚀 开发 `/ml-model-validate` 机器学习模型验证技能
2. 🌐 开发 `/defi-protocol-review` DeFi协议审查技能
3. ☁️ 开发 `/cloud-native-review` 云原生审查技能
4. 📈 建立技能使用度量和效果评估

### 长期目标（6-12个月）

1. 🤖 开发 `/trading-simulation` 交易模拟器审查技能
2. 🌐 建立企业级技能库和知识管理体系
3. 🎓 培训更多开发者使用技能体系
4. 🚀 建立技能生态和社区贡献机制

---

**技能版本**: v8.0 (2026-04-09)
**技能总数**: 18个核心技能 + 未来扩展
**适用行业**: 互联网金融、量化交易、风险管理、合规审计、DeFi、区块链金融、云原生金融、人工智能金融、数据库性能优化、网络性能优化、系统性能瓶颈识别、金融安全、高频交易、低延迟系统
**目标用户**: 金融开发人员、量化研究员、系统架构师、AI工程师、DevOps工程师、数据库管理员、性能工程师、安全工程师、高频交易开发团队

**最新更新**:
- v8.0: 新增低延迟系统设计技能 (low-latency-systems-design)
- v7.0: 新增安全审查技能 (security-review)
- v6.0: 新增性能瓶颈识别技能 (performance-bottleneck-identification)
- v5.0: 新增数据库交互优化技能 (database-interaction-optimization)
- v5.0: 新增网络性能优化技能 (network-optimization)
- v5.0: 更新技能对比矩阵，增加新技能维度

需要创建更多技能或定制现有技能，请继续提问！
