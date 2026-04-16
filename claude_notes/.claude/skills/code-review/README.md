# code-review skill 完整文档

## 📁 目录结构

```
code-review/
├── SKILL.md                          # ✅ 技能入口（核心）
├── RULES.md                          # ✅ 通用审查规则库（所有语言）
├── README.md                         # 📖 本文件（使用和扩展指南）
├── CHECKLISTS/                       # ✅ 语言特定检查清单
│   └── language_checklists.md        # ✅ C++/Python/Go/JS/TS 规则
└── TEMPLATES/                        # ✅ 报告模板
    ├── markdown_template.md          # ✅ Markdown 格式
    └── json_template.json            # ✅ JSON 格式
```

---

## 🚀 快速开始

### 1. 重启 Claude Code

修改技能后需要**重启 Claude Code** 使新功能生效。

### 2. 基础使用

```bash
/review                    # 审查当前分支所有变更（未暂存 + 已暂存）
/review tools/excels.py    # 审查指定文件
/review -s                 # 只审查已暂存的变更
/review -u                 # 只审查未暂存的变更
```

### 3. 高级使用

```bash
/review --output markdown  # 导出 Markdown 报告到 review_report_YYYYMMDD_HHMMSS.md
/review --output json      # 导出 JSON 报告到 review_report_YYYYMMDD_HHMMSS.json
```

---

## 📊 审查维度

增强版 `/review` 包含以下 5 个维度的检查：

### 1. 正确性
- 边界条件（空输入、零值、极值）
- 错误处理（文件/网络/内存失败）
- 数据类型转换（溢出、隐式转换）
- 指针/引用有效性

### 2. 安全性（增强）
- 缓冲区溢出（`strcpy` → `strncpy`/`snprintf`）
- 注入漏洞（SQL、XSS、路径遍历）
- 内存安全（悬垂指针、野指针、越界）
- 敏感信息（硬编码密钥、.env 误提交）

### 3. 性能
- 时间复杂度（冗余循环、低效查找）
- 内存使用（传值→传引用、预分配）
- 编译器优化（分支预测、虚函数调用）

### 4. 可读性与可维护性
- 命名规范（避免 `a`、`tmp`）
- 函数职责单一（长度 <50 行）
- 注释与文档（复杂逻辑、接口说明）

### 5. 语言特定规则
- **C++**: 智能指针、三五法则、现代 C++ 特性
- **Python**: `with` 管理资源、类型注解、异常处理
- **Go**: 错误检查、goroutine 生命周期、`defer` 使用
- **JS/TS**: `const`/`let`、`async/await`、类型安全

---

## 🛠️ 扩展方式详解

### 方式 1：添加新语言规则

**场景**: 你想增加 Rust 语言的检查规则

**步骤:**

1. 在 `CHECKLISTS/` 下创建 `rust.md`：

```markdown
# Rust 特定检查清单

## 必查项
- [ ] 是否使用 `Result`/`Option` 处理错误（避免 `unwrap()`）
- [ ] 生命周期标注是否正确
- [ ] 是否避免不必要的 `clone()`

## 高风险模式
| 模式 | 风险 | 修复建议 |
|------|------|----------|
| `unsafe { ... }` | 内存安全 | 最小化作用域，添加注释 |
| `.unwrap()` | panic | 改为 `?` 或 `expect()` |
```

2. 在 `SKILL.md` 的「执行步骤」中添加语言识别逻辑，识别 `.rs` 文件时加载此清单

---

### 方式 2：添加项目特定规则

**场景**: 你的项目有特定的命名规范或架构要求

**步骤:**

1. 在 `RULES.md` 末尾添加「项目特定规则」章节：

```markdown
## 8. 项目特定规则

### 8.1 命名规范
- [ ] 所有工具函数以 `util_` 开头
- [ ] 配置变量全大写 + `_CONFIG` 后缀

### 8.2 文件组织
- [ ] 每个模块必须有 `__init__.py`
- [ ] 测试文件以 `test_` 开头
- [ ] 所有 API 路由必须在 `routes/` 目录下
```

2. 在审查时，对变更文件执行这些额外检查

---

### 方式 3：自定义报告模板

**场景**: 你需要导出到 Confluence、Jira 或其他系统

**步骤:**

1. 在 `TEMPLATES/` 下创建 `confluence_template.md`：

```markdown
h1. 代码审查报告

*分支*: {{branch}}
*日期*: {{date}}

h2. 问题列表

|| 文件 || 行号 || 问题 || 严重级别 ||
{{#issues}}
| {{file}} | {{line}} | {{description}} | {{severity}} |
{{/issues}}
```

2. 修改 `SKILL.md` 支持 `--output confluence` 选项：

```markdown
#### Confluence 导出

```bash
/review --output confluence
# 生成 confluence_report_YYYYMMDD_HHMMSS.md
```
```

---

### 方式 4：集成到工作流

**场景**: 每次提交前自动审查

**步骤:**

1. 创建 Git pre-commit hook：

```bash
# .git/hooks/pre-commit
#!/bin/bash

echo "🔍 正在审查代码变更..."
claude /review -s --output json > /tmp/review.json

# 检查是否有高危问题
if grep -q '"severity": "high"' /tmp/review.json; then
    echo "❌ 存在高危问题，提交被阻止"
    cat /tmp/review.json | jq '.issues[] | select(.severity=="high")'
    exit 1
fi

echo "✅ 审查通过，可以提交"
```

2. 赋予执行权限：

```bash
chmod +x .git/hooks/pre-commit
```

---

### 方式 5：添加自动修复建议

**场景**: 对低风险问题自动生成修复代码

**步骤:**

1. 在 `SKILL.md` 的输出部分添加「快速修复」章节：

```markdown
### 可自动修复的问题

{{#fixable_issues}}
{{index}}. **{{file}}:{{line}}** — {{description}}
```diff
{{diff}}
```
{{/fixable_issues}}

运行以下命令应用修复：
```bash
claude /fix review_report.json
```
```

2. 创建新的技能 `/fix`（可选）：

```yaml
---
name: /fix
description: 根据审查报告自动修复低风险问题
---

# 自动修复

读取 review_report.json，对每个「可自动修复」的问题：
1. 使用 Edit 工具应用修复
2. 输出已修复的问题列表
```

---

## 📖 实际案例

### 案例 1：为 C++20 项目增强审查

**需求**: 项目使用现代 C++20，需要检查 `std::span`、`std::format` 等新特性

**实现:**

1. 在 `CHECKLISTS/language_checklists.md` 中添加：

```markdown
## C++20 新特性检查

- [ ] 大数组参数是否改用 `std::span`（而非裸指针 + 长度）
- [ ] 字符串格式化是否用 `std::format`（而非 `sprintf`）
- [ ] 范围遍历是否用 `std::ranges::views`（避免中间容器）

**示例对比:**

差:
```cpp
void print(int* arr, size_t len) {
    for (size_t i = 0; i < len; ++i) cout << arr[i];
}
```

好:
```cpp
void print(std::span<int> arr) {
    for (int x : arr) cout << x;
}
```
```

2. 更新 `SKILL.md` 在识别 C++ 文件时加载此检查清单

---

### 案例 2：导出审查报告到 Slack

**需求**: 每次审查后自动通知团队

**实现:**

1. 创建脚本 `notify_slack.sh`：

```bash
#!/bin/bash
# 读取 JSON 报告并发送到 Slack
curl -X POST $SLACK_WEBHOOK_URL \
  -H 'Content-type: application/json' \
  -d "{
    \"text\": \"Code Review 完成\",
    \"attachments\": [{
      \"color\": \"warning\",
      \"fields\": [{
        \"title\": \"高危问题\",
        \"value\": \"$(jq '.review_report.summary.high_severity' review.json)\",
        \"short\": true
      }]
    }]
  }"
```

2. 在 `SKILL.md` 末尾添加：

```markdown
### 集成通知

审查完成后自动运行：
```bash
./scripts/notify_slack.sh
```
```

---

## 🧪 测试扩展

```bash
# 测试基础功能
/review tools/excels.py

# 测试语言规则（创建一个 .cpp 文件测试）
/review src/main.cpp

# 测试导出功能
/review --output json
cat review_report_*.json | jq .

# 测试已暂存变更
git add tools/excels.py
/review -s
```

---

## 🐛 故障排查

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| 新规则不生效 | 未重启 Claude Code | 重启 Claude Code |
| 报告模板未使用 | 路径错误 | 检查 `TEMPLATES/` 目录是否存在 |
| 语言规则未加载 | 文件扩展名不匹配 | 确认文件扩展名（`.cpp` vs `.cc`） |
| 技能无法触发 | `SKILL.md` frontmatter 错误 | 检查 YAML 格式（`---` 不能少） |
| 无输出 | 没有变更文件 | 运行 `git status` 确认 |

---

## 📝 维护建议

### 1. 定期更新规则库

- 每次发现新的 bug 模式，添加到 `RULES.md`
- 每次项目引入新技术栈，添加对应的检查清单
- 根据团队反馈调整严重级别判定标准

### 2. 版本管理

在 `SKILL.md` 底部维护更新日志：

```markdown
## 更新日志

- **v2.1** (2026-04-10): 
  - 添加 Rust 语言支持
  - 修复 JSON 模板字段名错误

- **v2.0** (2026-04-08): 
  - 多维度检查（正确性、安全性、性能、可读性）
  - 语言特定规则（C++、Python、Go、JS/TS）
  - 报告导出功能（Markdown、JSON）
```

### 3. 团队共享

将整个 `code-review/` 目录复制到团队成员的 `~/.claude/skills/` 目录：

```bash
# Linux/Mac
cp -r code-review ~/.claude/skills/

# Windows
xcopy code-review %USERPROFILE%\.claude\skills\code-review\ /E /I
```

---

## 🎯 下一步

1. ✅ **立即尝试**: 运行 `/review` 测试新功能
2. 🔧 **定制规则**: 根据项目需求修改 `RULES.md`
3. 📊 **集成工作流**: 设置 pre-commit hook 或 CI 检查
4. 🌐 **分享技能**: 将 `code-review/` 目录分享给团队成员
5. 📚 **持续学习**: 阅读 `RULES.md` 和 `CHECKLISTS/` 深入理解审查规则
