# GitHub Git 典型工作流程

### 0：生成并配置 SSH 密钥（首次使用）

```bash
# 1. 生成新密钥（推荐 ed25519）
ssh-keygen -t ed25519 -C "xiaoyaolml@163.com"
# 一路回车即可

# 2. 查看公钥
cat ~/.ssh/id_ed25519.pub

# 3. 复制输出内容，去 GitHub 添加：
#    Settings → SSH and GPG keys → New SSH key
#    粘贴公钥，取个名字，保存

# 4. 测试连接
ssh -T git@github.com
# 成功会看到：Hi username! You've successfully authenticated...
```

## 1️⃣ 创建仓库

**在 GitHub 上:**

* 点击右上角 `+` → `New repository`
* 填写 repo 名称，选择 public/private
* 可选：添加 README、.gitignore、license
* 点击 `Create repository`

## 2️⃣ 首次克隆（本地没有代码）

```bash
# 克隆空仓库
git clone git@github.com:username/repo.git
cd repo

# 或者 HTTPS
git clone https://github.com/username/repo.git
```

## 3️⃣ 情况1：已有本地项目，关联远程

```bash
# 初始化本地 git
git init -b main

# 添加远程仓库
git remote add origin git@github.com:xiaoyaolml/llm.git

# 首次配置用户名和邮箱(全局用git config --global)
git config user.name "Leo"
git config user.email "xiaoyaolml@163.com"

# 首次推送
git add .
git commit -m "Initial commit"
# git branch -M main # 不需要重命名了
git push -u origin main
```

## 3️⃣情况2：本地已有项目，远程也有内容

上面的最后一步  `git push -u origin main` 会报错

```bash
hint: Updates were rejected because the remote contains work that you do not
hint: have locally. This is usually caused by another repository pushing to
hint: the same ref. If you want to integrate the remote changes, use
hint: 'git pull' before pushing again.
hint: See the 'Note about fast-forwards' in 'git push --help' for details.
```

问题：远程和本地有分歧
远程仓库有你本地没有的提交，Git 阻止推送以避免覆盖他人工作。

🚀 解决方案
方案 1：先拉取合并（推荐，安全）

```bash
# 1. 拉取远程变更并合并
git pull origin main
# 2. 如果有冲突，手动解决后：
git add .
git commit -m "Initial commit in this PC"
# 3. 推送
git push -u origin main
```

方案 2：变基（保持提交历史线性）

```bash
# 1. 拉取并变基
git pull --rebase origin main
# 2. 如果有冲突，手动解决后：
git add .
git rebase --continue
# 没冲突的情况
git add .
git commit -m "Initial commit in this PC"
# 3. 推送
git push -u origin main
```

方案 3：强制推送（⚠️ 危险，慎用！）
仅当你确定要覆盖远程内容时使用：

```bash
# 强制推送，会丢失远程的提交！
git push -f origin main
```

## 4️⃣ 日常开发流程

```bash
# 开始新功能前，确保本地是最新的
git pull origin main

# 创建新分支（推荐！）
git checkout -b feature/new-feature

# 开发... 然后
git add .
git commit -m "feat: add new feature"

# 推送到远程分支
git push origin feature/new-feature

# 去 GitHub 创建 Pull Request
```

## 5️⃣ 同步他人更新

```bash
# 拉取最新代码并合并
git pull origin main

# 或者只获取不合并
git fetch origin
```

## 6️⃣ 常用命令速查

| 命令                       | 作用           |
| -------------------------- | -------------- |
| `git status`             | 查看状态       |
| `git log --oneline`      | 简洁历史       |
| `git diff`               | 查看改动       |
| `git branch -a`          | 所有分支       |
| `git checkout -b <name>` | 创建并切换分支 |
| `git merge <branch>`     | 合并分支       |
| `git stash`              | 暂存改动       |

## 📝 Commit 消息规范（推荐）

```
feat: 新功能
fix: 修复 bug
docs: 文档更新
style: 代码格式（不影响功能）
refactor: 重构
test: 测试相关
chore: 构建/工具链
```

## ⚠️ 常见坑

* **推送被拒绝** → 先 `git pull` 合并远程变更
* **推错分支** → `git reset --hard HEAD~1` 撤销最后一次 commit
* **冲突** → 手动解决后 `git add` + `git commit`

需要我详细解释某个环节吗？
