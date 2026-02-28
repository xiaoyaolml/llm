# 📚 命令历史记录

---

## 2026-02-27 07:44:50

**用户提问：** 这个命令清单的内容里附带上当前的时间和我的提问内容，然后写到/home/leo/workspace/openclaw/cmd_current.md ,并追加记录到/home/leo/workspace/openclaw/cmd_history.md，可以随着 github 提交的时候一起提交

### 使用的命令清单

| 命令 | 说明 |
|------|------|
| `date "+%Y-%m-%d %H:%M:%S"` | 获取当前时间 |
| `write > cmd_current.md` | 创建当前会话命令记录文件 |
| `write > cmd_history.md` | 创建历史命令记录文件 |
| `git add cmd_current.md cmd_history.md` | 添加到 git 暂存区 |
| `git commit -m "..."` | 提交更改 |
| `git push origin main` | 推送到 GitHub |

---

---

## 2026-02-28 11:05

**用户提问：** 文件 embedded_c_exam_prep.md 不要放在 leetcode 目录下，可以在/home/leo/workspace/openclaw/目录下新建一个文件夹存放 embedded_c_exam_prep.md，完成这个操作，然后提交到 github，记录命令。

### 使用的命令清单

| 命令 | 说明 |
|------|------|
| mkdir -p embedded_c | 创建新的 embedded_c 目录 |
| mv leetcode/embedded_c_exam_prep.md embedded_c/ | 将文件从 leetcode 目录移动到 embedded_c 目录 |
| git status | 查看 git 状态 |
| git add -A | 添加所有更改到暂存区 |
| git commit -m "refactor: 将嵌入式复习资料移至独立目录 embedded_c" | 提交更改 |
| git push origin main | 推送到 GitHub |

---
