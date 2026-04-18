# 金融开发技能组合完整指南 (最新版)

> 维护说明：这组 Claude Skills 已完成一轮结构评审。
> 建议优先参考 `REVIEW_AND_OPTIMIZATION.md` 中的收敛分组、去重建议和 Cursor 迁移方案，
> 不建议继续按 1:1 方式扩展高度重叠的同类 Skills。

> 目录结构统一标准见 `STANDARD_STRUCTURE.md`。
> 分层索引见 `LAYERED_INDEX.md`，快速导航见 `COMPACT_GUIDE.md`。
> 支持文档风格约定见 `SUPPORT_DOC_STYLE.md`。

## 🪞 从 Skill 侧看审查阶段

| 审查阶段 | 优先 Skill | 典型问题 |
| --- | --- | --- |
| 0. 快速分流 | `code-review` | 还不确定问题属于代码质量、性能、并发、协议还是安全 |
| 1. 通用实现审查 | `code-review` + `cpp-backend-review` | 普通 PR、边界条件、错误处理、可维护性、C++ 服务端实现质量 |
| 2. 并发 / 生命周期 / 内存 | `concurrency-review` + `memory-management-review` | 线程安全、锁顺序、shutdown、回调生命周期、泄漏、悬垂引用 |
| 3. 性能 / 延迟 / 吞吐 | `performance-bottleneck-identification` + `finance-optim` + `network-optimization` + `database-interaction-optimization` + `low-latency-systems-design` | 热点路径、尾延迟、CPU 异常、分配开销、网络瓶颈、数据库交互低效 |
| 4. 协议 / 风险 / 数据正确性 | `protocol-validate` + `risk-check` + `market-data-validate` + `quant-backtest` + `api-design-review` | 字段兼容性、风控结果异常、行情质量、回测偏差、接口语义问题 |
| 5. 安全 / 合规 | `security-review` + `compliance-review` | 输入校验、权限边界、敏感信息、审计要求、监管约束 |
| 6. 云上交付 / 观测 / 演练 | `cloud-native-review` + `system-monitoring` + `trading-simulation` | 部署配置、容量、告警、日志、指标、恢复演练、系统稳定性 |
| 7. 按需专项 | `ml-model-validate` + `defi-protocol-review` | 强领域语义验证，不属于常规 C++ 服务端主线审查 |

### 最短使用顺序

1. 先用 `code-review` 做总入口。
2. 如果是 C++ 服务端 PR，追加 `cpp-backend-review`。
3. 看到锁、原子、回调、析构、资源释放，切到 `concurrency-review` / `memory-management-review`。
4. 看到尾延迟、吞吐、网络、数据库热点，切到性能组 Skill。
5. 看到协议字段、行情、风控、回测偏差，切到风险 / 数据 / 协议组 Skill。
6. 看到权限、审计、脱敏、监管要求，切到 `security-review` / `compliance-review`。
7. 只有碰到专项场景时，再进入 `ml-model-validate` 或 `defi-protocol-review`。

### 与 Cursor 侧索引的对应关系

- Skill 侧阶段索引：当前文档
- Playbook 侧阶段索引：[cursor_notes/skill-playbook-quick-reference.md](cursor_notes/skill-playbook-quick-reference.md)
- 现象分流图：[cursor_notes/problem-symptom-navigation.md](cursor_notes/problem-symptom-navigation.md)
- 极简速记版：[cursor_notes/review-cheat-sheet.md](cursor_notes/review-cheat-sheet.md)

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

### 📚 当前维护建议

- `README.md` 保持“定位 + 边界 + 场景”风格
- `SKILL.md` 保持“目标 + 输入 + 输出 + 协作边界”风格
- `CHECKLISTS/` 保持“使用时机 + 关键检查项”风格
- `EXAMPLES/` 保持“示例输入 + 期望输出”风格
- `TEMPLATES/` 保持“最小可复用模板”风格

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


---

## 使用方式

这批 Skills 不是通过 CLI 命令调用的独立工具。它们是一套**领域知识库和审查清单库**。

### 在 Cursor 中使用

这些 Skills 的核心价值已迁移到 Cursor 侧：

| 内容类型 | 去哪里找 |
| --- | --- |
| 长期规则约束 | `.cursor/rules/` |
| 审查流程 | `cursor_notes/playbooks/` |
| C++ PR 审查模板 | `cursor_notes/pr_review_templates/` |
| 专项 Prompt | `cursor_notes/prompts/` |
| 快速导航 | `cursor_notes/review-cheat-sheet.md` |
| 现象分流图 | `cursor_notes/problem-symptom-navigation.md` |

### 在 Claude 中使用

直接引用 Skill 目录下的 `SKILL.md`、`CHECKLISTS/`、`TEMPLATES/` 作为审查上下文。

例如：

```text
请参考 .claude/skills/concurrency-review/SKILL.md 中的审查重点，
对当前改动做并发安全审查。
```

---

## 学习路径建议

### 入门

1. 从 `code-review` 开始，理解通用代码质量标准
2. 用 `cpp-backend-review` 理解 C++ 服务端专项关注点
3. 用 `concurrency-review` 理解并发审查重点

### 进阶

1. 深入 `performance-bottleneck-identification` + `finance-optim` 做性能分析
2. 掌握 `risk-check` + `market-data-validate` 做金融数据验证
3. 学习 `protocol-validate` 做协议兼容性审查
4. 了解 `security-review` + `compliance-review` 做安全合规审查

### 高级

1. 组合多个 Skill 做跨维度审查
2. 定制和扩展现有 Skill 的清单与模板
3. 建立团队级审查规范，融入 CI/CD

---

## 维护建议

- 核心 Skill 优先保持高质量
- 不再在各 Skill 中重复 CLI 安装说明
- 重复主题优先收敛到共享 Playbook
- 每次线上事故补一条清单项
- 每次性能优化补一条模板

---

**状态**：21 个 Skills，分为 核心(7) / 专项(12) / 按需(2)  
**Cursor 映射**：见 `cursor_notes/claude-skills-to-cursor-mapping.md`  
**分层索引**：见 `LAYERED_INDEX.md`  
**精简导航**：见 `COMPACT_GUIDE.md`
