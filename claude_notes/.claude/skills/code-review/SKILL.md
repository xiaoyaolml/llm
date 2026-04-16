---
name: code-review
description: 对当前分支的代码变更进行结构化审查（增强版：支持多维度、多语言、导出报告）
---

# 代码审查（增强版）

## 触发方式

### 基础触发
- `/review` — 审查当前分支所有变更（未暂存 + 已暂存）
- `/review <file>` — 审查指定文件
- `/review -s` — 只审查已暂存的变更
- `/review -u` — 只审查未暂存的变更

### 高级触发
- `/review --output markdown` — 输出 Markdown 报告到文件
- `/review --output json` — 输出 JSON 报告到文件
- `/review --lang cpp` — 启用 C++ 专用检查规则
- `/review --lang python` — 启用 Python 专用检查规则

## 执行步骤

### 1. 获取变更范围

```bash
# 自动检测变更范围
git status              # 确认分支和状态
git diff                # 未暂存变更
git diff --staged       # 已暂存变更
git log --oneline -5    # 最近提交上下文
```

**特殊场景：**
- 如果提供了文件路径，只审查该文件
- 如果提供了 `-s`/`-u`，只审查对应范围
- 如果在 PR/合并请求上下文，审查整个 PR 差异

### 2. 识别语言和项目类型

1. 检查变更文件的扩展名
2. 加载对应的语言检查清单（`CHECKLISTS/`）
3. 检查项目配置文件（`CMakeLists.txt`、`package.json` 等）

### 3. 多维度审查

#### 正确性
- [x] 边界条件（空输入、零值、极值）
- [x] 错误处理（文件/网络/内存失败）
- [x] 数据类型转换（溢出、隐式转换）
- [x] 指针/引用有效性检查

**参考**: [RULES.md](RULES.md#1-正确性检查)

#### 安全性（增强）
- [x] 缓冲区溢出（`strcpy` → `strncpy`/`snprintf`）
- [x] 注入漏洞（SQL、XSS、路径遍历）
- [x] 内存安全（悬垂指针、野指针、越界）
- [x] 敏感信息（硬编码密钥、.env 误提交）

**参考**: [RULES.md](RULES.md#2-安全性检查)

#### 性能
- [x] 时间复杂度（冗余循环、低效查找）
- [x] 内存使用（传值→传引用、预分配 `reserve`）
- [x] 编译器优化（分支预测、虚函数调用）

**参考**: [RULES.md](RULES.md#3-性能检查)

#### 可读性与可维护性
- [x] 命名规范（避免 `a`、`tmp`，使用有意义名称）
- [x] 函数职责单一（长度 <50 行）
- [x] 注释与文档（复杂逻辑、接口说明）

**参考**: [RULES.md](RULES.md#4-可读性与可维护性)

#### 语言特定规则

**C++ 特有：**
- [x] 智能指针（`unique_ptr`/`shared_ptr` 优于裸指针）
- [x] 三五法则（拷贝/移动构造函数一致性）
- [x] 析构函数虚函数（基类必须 `virtual`）
- [x] 现代 C++ 特性（`auto`、range-for、`constexpr`）

**参考**: [CHECKLISTS/language_checklists.md](CHECKLISTS/language_checklists.md)

### 4. 输出格式

#### 默认（终端输出）

```
## 审查报告

**分支**: main
**审查时间**: 2026-04-08 15:30:00

---

### 问题清单

| # | 文件 | 行号 | 问题描述 | 严重级别 | 建议 |
|---|------|------|----------|----------|------|
| 1 | tools/excels.py | 104 | Excel 进程未释放 | 高 | 取消注释 wb.close() |

---

### 总结
- 批准 / 有条件批准 / 需修改
- 核心问题：[描述]
```

#### Markdown 导出

```bash
/review --output markdown
# 生成 review_report_YYYYMMDD_HHMMSS.md
```

**模板**: [TEMPLATES/markdown_template.md](TEMPLATES/markdown_template.md)

#### JSON 导出

```bash
/review --output json
# 生成 review_report_YYYYMMDD_HHMMSS.json
```

**模板**: [TEMPLATES/json_template.json](TEMPLATES/json_template.json)

### 5. 严重级别定义

- **高**: 运行时崩溃、安全漏洞、逻辑错误（必须修复）
- **中**: 内存泄漏、未定义行为、边界未处理（建议修复）
- **低**: 风格问题、可读性、性能优化（可选修复）

## 高级功能

### 1. 自定义检查清单

在 `CHECKLISTS/` 目录下添加 `<language>.md`：

```
CHECKLISTS/
├── cpp.md          # C++ 专用检查
├── python.md       # Python 专用检查
├── go.md           # Go 专用检查
└── custom.md       # 项目自定义检查
```

### 2. 集成到工作流

**Git hooks（示例）：**

```bash
# .git/hooks/pre-commit
#!/bin/bash
claude /review -s --output json > review.json
if grep -q '"severity": "high"' review.json; then
    echo "❌ 存在高危问题，禁止提交"
    exit 1
fi
```

**CI/CD（GitHub Actions）：**

```yaml
- name: Code Review
  run: |
    claude /review --output json > review.json
    # 解析 JSON 并设置失败条件
```

### 3. 快速修复模式

对「低」级别问题，可以自动生成修复建议：

```markdown
### 可自动修复的问题

1. **tools/excels.py:104** — 取消注释 `wb.close()`
   ```diff
   - # wb.close()
   + wb.close()
   ```

2. **tools/excels.py:133** — 使用精确索引图片
   ```diff
   - pic = sheet.pictures[-1]
   + pic_count_before = len(sheet.pictures)
   + sheet.api.Paste()
   + pic = sheet.pictures[pic_count_before]
   ```

运行以下命令应用修复：
```bash
claude /fix review_report.json
```
```

## 最佳实践

### 审查前
- ✅ 确保代码能编译/运行
- ✅ 暂存有意义的变更（`git add`）
- ✅ 撰写清晰的提交信息

### 审查时
- ✅ 先看整体架构，再看细节
- ✅ 优先处理「高」级别问题
- ✅ 对模糊问题给出具体示例

### 审查后
- ✅ 修复问题并重新审查
- ✅ 导出报告供团队评审
- ✅ 更新项目检查清单（如有新发现）

## 参考资料

- [RULES.md](RULES.md) — 通用审查规则库
- [CHECKLISTS/](CHECKLISTS/) — 语言特定检查清单
- [TEMPLATES/](TEMPLATES/) — 报告模板

## 故障排查

| 问题 | 解决方案 |
|------|----------|
| 技能不能触发 | 检查 `.claude/skills/code-review/SKILL.md` 是否存在 |
| 无输出 | 确认有变更文件（`git status`） |
| 输出格式错误 | 检查 `--output` 参数（`markdown` 或 `json`） |
| 语言规则未生效 | 检查文件扩展名是否匹配（`.cpp`、`.py` 等） |

## 更新日志

- **v2.0** (2026-04-08): 增强版发布
  - 多维度检查（正确性、安全性、性能、可读性）
  - 语言特定规则（C++、Python、Go、JS/TS）
  - 报告导出（Markdown、JSON）
  - 自定义检查清单支持
