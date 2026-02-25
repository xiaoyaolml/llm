# 修改 Git Author 信息

## 🚀 修改未来提交的 Author（全局配置）

```bash
# 设置全局用户名和邮箱
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"

# 查看配置
git config --global user.name
git config --global user.email
```

## 📋 修改已提交记录的 Author

### 方案 1：修改最后一次提交

```bash
# 修改最近一次提交的 author
git commit --amend --author="New Name <new.email@example.com>" --no-edit

# 如果还没推送到远程，强制推送
git push -f origin main
```

### 方案 2：修改所有提交的 Author（批量）

```bash
# 使用 git filter-branch（旧方法）
git filter-branch --env-filter '
export GIT_AUTHOR_NAME="New Name"
export GIT_AUTHOR_EMAIL="new.email@example.com"
export GIT_COMMITTER_NAME="New Name"
export GIT_COMMITTER_EMAIL="new.email@example.com"
' -- HEAD

# 强制推送到远程
git push -f origin main
```

### 方案 3：使用 `git rebase -i`（推荐，更灵活）

```bash
# 1. 开始交互式变基（修改最近 3 次提交为例）
git rebase -i HEAD~3

# 2. 在编辑器中把要修改的提交前的 `pick` 改为 `edit`
# 保存退出

# 3. 修改 author
git commit --amend --author="New Name <new.email@example.com>" --no-edit

# 4. 继续变基
git rebase --continue

# 5. 重复步骤 3-4 直到完成

# 6. 强制推送
git push -f origin main
```

### 方案 4：使用 BFG Repo-Cleaner（最快，适合大仓库）

```bash
# 下载 BFG: https://rtyley.github.io/bfg-repo-cleaner/

# 修改所有提交的 author
bfg --replace-text emails.txt  # emails.txt 包含旧邮箱→新邮箱的映射

# 清理并强制推送
git reflog expire --expire=now --all
git gc --prune=now --aggressive
git push -f origin main
```

## 📝 项目级配置（不影响其他项目）

```bash
# 进入项目目录
cd /path/to/repo

# 设置项目级别的 author
git config user.name "Project Name"
git config user.email "project.email@example.com"

# 查看项目配置
git config user.name
git config user.email
```

## 🔍 查看当前 Author 配置

```bash
# 查看全局配置
git config --global --list

# 查看项目配置
git config --local --list

# 查看最近提交的 author
git log --format="%h %an <%ae>" -5
```

## ⚠️ 重要注意事项

| 场景         | 注意事项                            |
| ------------ | ----------------------------------- |
| 已推送到远程 | 修改后需要 `git push -f` 强制推送 |
| 多人协作     | 强制推送会覆盖他人工作，先沟通！    |
| 公开仓库     | 修改历史可能影响其他人，谨慎操作    |
| 私密信息泄露 | 如果邮箱已泄露，尽快修改并通知团队  |

## 💡 常见场景

### 场景 1：刚配置 Git，第一次提交发现 author 错了

```bash
# 修改配置
git config --global user.name "Correct Name"
git config --global user.email "correct@email.com"

# 修改最后一次提交
git commit --amend --reset-author --no-edit

# 推送（如果是第一次推送）
git push -u origin main
```

### 场景 2：公司项目要用公司邮箱

```bash
# 在项目目录设置（不影响个人项目）
cd ~/work/company-project
git config user.name "Your Name"
git config user.email "you@company.com"
```

### 场景 3：彻底替换某个邮箱的所有提交

```bash
# 创建替换文件 replace.txt
# 内容格式：旧邮箱=新邮箱
old@email.com=New Name <new@email.com>

# 使用 BFG
bfg --replace-text replace.txt
git reflog expire --expire=now --all && git gc --prune=now --aggressive
git push -f
```

## ⚡ 快速修复（最常见情况）

```bash
# 1. 修正全局配置
git config --global user.name "Your Name"
git config --global user.email "your@email.com"

# 2. 修改最后一次提交
git commit --amend --reset-author --no-edit

# 3. 强制推送
git push -f origin main
```

修改 author 后， **强制推送会影响远程历史** ，多人协作时务必先通知团队！
