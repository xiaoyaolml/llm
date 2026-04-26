# Claude Skills 标准目录结构

建议每个 Skill 统一采用以下结构：

```text
skill-name/
  README.md
  SKILL.md
  CHECKLISTS/
    README.md
    *.md
  EXAMPLES/
    README.md
    *.md
  TEMPLATES/
    README.md
    *.md / *.json
```

## 各目录职责

- `README.md`：说明 Skill 的定位、适用场景、主要内容、与其他 Skills 的边界。
- `SKILL.md`：保留面向 Claude/Agent 的核心定义与执行说明。
- `CHECKLISTS/`：放审查清单、核对项、规则摘要。
- `EXAMPLES/`：放高质量使用示例、输入输出样例。
- `TEMPLATES/`：放审查报告模板、导出格式模板、提示词模板。

## 维护原则

- 缺少专项内容时，也保留目录并使用 `README.md` 说明“待补充内容”。
- 不再把大量 Claude CLI 安装、重启、hook 说明复制到每个 Skill 中。
- 重复主题优先收敛到共享规则或共享 Playbook，不重复扩写。