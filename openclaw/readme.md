# 📋 当前会话命令记录

**时间：** 2026-02-28 11:05  
**用户提问：** 文件 embedded_c_exam_prep.md 不要放在 leetcode 目录下，可以在/home/leo/workspace/openclaw/目录下新建一个文件夹存放 embedded_c_exam_prep.md，完成这个操作，然后提交到 github，记录命令。

---

## 🛠️ 使用的命令清单

| 命令 | 说明 |
|------|------|
| mkdir -p embedded_c | 创建新的 embedded_c 目录 |
| mv leetcode/embedded_c_exam_prep.md embedded_c/ | 将文件从 leetcode 目录移动到 embedded_c 目录 |
| git status | 查看 git 状态 |
| git add -A | 添加所有更改到暂存区 |
| git commit -m "refactor: 将嵌入式复习资料移至独立目录 embedded_c" | 提交更改 |
| git push origin main | 推送到 GitHub |

---

## 📁 项目结构

```
/home/leo/workspace/openclaw/
├── readme.md              # 项目说明 + 命令记录
├── cmd_history.md         # 命令历史记录
├── embedded_c/            # 嵌入式 C 笔试资料（新建）
│   └── embedded_c_exam_prep.md
└── leetcode/              # LeetCode 刷题资料
    ├── leetcode_guide.md
    ├── leetcode_note_template.md
    └── leetcode_progress_tracker.md
```

---

*此文件会在下次会话时更新*
