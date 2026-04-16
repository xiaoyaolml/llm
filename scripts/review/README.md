# Review Scripts

本目录提供一组面向当前仓库的审查辅助脚本。

## 脚本

- `export_pr_snapshot.py`：导出当前分支 / staged / unstaged 的审查上下文
- `generate_review_prompt.py`：生成不同主题的 Cursor 审查提示词
- `repo_delivery_audit.py`：检查仓库是否具备基础交付条件

## 典型用法

```bash
py -3 scripts/review/export_pr_snapshot.py --scope all
py -3 scripts/review/generate_review_prompt.py --mode cpp-pr
py -3 scripts/review/repo_delivery_audit.py
```

输出文件默认写入 `review_reports/`。
