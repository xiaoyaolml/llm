# Windows 下配置 SSH IdentityFile

## 路径写法（关键！）

在 `~/.ssh/config` 文件中，Windows 路径有  **3 种写法** ：

### ✅ 推荐：使用正斜杠 `/`

```ssh
Host github.com
    HostName github.com
    User git
    IdentityFile C:/Users/miali/.ssh/id_rsa_github
    IdentitiesOnly yes
```

### ✅ 也可：使用双反斜杠 `\\`

```ssh
Host github.com
    HostName github.com
    User git
    IdentityFile C:\\Users\\miali\\.ssh\\id_rsa_github
    IdentitiesOnly yes
```

### ✅ Git Bash 环境：使用 `~` 简写

```ssh
Host github.com
    HostName github.com
    User git
    IdentityFile ~/.ssh/id_rsa_github
    IdentitiesOnly yes
```



### 测试连接

在 Git Bash 中：

```bash
ssh -T git@github.com
```

成功会看到：

```
Hi xiaoyaolml! You've successfully authenticated, but GitHub does not provide shell access.
```


我用的是 Git Bash 环境，配置config文件如下：

```
# GitHub 专用密钥
Host github.com
    HostName github.com
    User git
    IdentityFile ~/.ssh/id_rsa_github
    IdentitiesOnly yes
```

另外，这个rsa生成的时候用了passphrase，123456
