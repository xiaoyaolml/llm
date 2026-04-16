# 云原生应用审查使用示例

## 示例1: 基本容器化审查

```bash
# 审查Dockerfile和容器配置
claude /cloud-native-review containers/ --validate-container

# 输出详细报告
claude /cloud-native-review containers/ --output markdown
```

## 示例2: 指定审查模块

```bash
# 只验证微服务架构
claude /cloud-native-review k8s/ --module microservices

# 验证自动扩缩容配置
claude /cloud-native-review k8s/ --module autoscaling

# 验证服务网格配置
claude /cloud-native-review istio/ --module service-mesh

# 验证所有模块
claude /cloud-native-review k8s/ --validate-all
```

## 示例3: 指定部署环境

```bash
# 审查生产环境配置
claude /cloud-native-review prod/ --environment production

# 审查测试环境配置
claude /cloud-native-review test/ --environment testing

# 指定Kubernetes集群
claude /cloud-native-review k8s/ --cluster my-cluster
```

## 示例4: JSON格式输出

```bash
# 输出JSON格式便于程序处理
claude /cloud-native-review k8s/ --output json

# 将结果保存到文件
claude /cloud-native-review k8s/ --output json > cloud_native_report.json
```

## 示例5: 详细模式

```bash
# 显示详细验证过程
claude /cloud-native-review k8s/ --verbose

# 显示所有警告和错误
claude /cloud-native-review k8s/ --strict
```

## 示例6: CI/CD集成

```yaml
# GitHub Actions 示例
name: Cloud Native Review

on: [push, pull_request]

jobs:
  cloud-native-review:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    
    - name: Setup kubectl
      uses: azure/setup-kubectl@v3
    
    - name: Configure kubeconfig
      run: |
        echo "${{ secrets.KUBECONFIG }}" | base64 -d > ~/.kube/config
    
    - name: Review Cloud Native Configuration
      run: |
        claude /cloud-native-review k8s/ --output json > review.json
    
    - name: Check for critical issues
      run: |
        # 检查是否有严重问题
        if jq '.review_results | any(.status == "fail")' review.json; then
          echo "❌ 云原生配置存在严重问题"
          exit 1
        else
          echo "✅ 云原生配置通过审查"
        fi
```

## 示例7: 高级用法

```bash
# 审查多个目录
claude /cloud-native-review containers/ k8s/ istio/ --recursive

# 审查特定文件类型
claude /cloud-native-review k8s/ --file-pattern "*.yaml"

# 设置自定义阈值
claude /cloud-native-review k8s/ --cpu-threshold 70 --memory-threshold 80
```

## 示例8: 与现有技能组合使用

```bash
# 首先进行通用代码审查
/review src/

# 然后进行金融性能优化检查
/finance-optim src/

# 最后进行云原生审查
/cloud-native-review k8s/ --validate-container --validate-microservices

# 验证合规性
/compliance-review k8s/
```

## 示例9: 批量验证

```bash
# 验证整个项目目录
claude /cloud-native-review . --recursive

# 验证特定环境
claude /cloud-native-review environments/ --environment dev,staging,prod

# 验证特定组件
claude /cloud-native-review k8s/ --component containers,microservices,autoscaling
```

## 示例10: 与金融开发技能结合

```bash
# 先审查金融应用的云原生配置
/cloud-native-review trading-app/ --validate-container --validate-microservices

# 再审查性能优化
/finance-optim trading-app/ --validate-scaling

# 最后审查风险计算
/risk-check trading-app/src/risk/

# 验证合规性
/compliance-review trading-app/src/compliance/
```

## 示例11: 定期审查设置

```bash
# 创建每日自动审查任务
# 在 crontab 中添加
0 9 * * * cd /path/to/project && claude /cloud-native-review k8s/ --output json > daily_review_$(date +%Y%m%d).json

# 创建每周完整审查
0 9 * * 1 cd /path/to/project && claude /cloud-native-review k8s/ --validate-all --output json > weekly_review_$(date +%Y%m%d).json
```

## 示例12: 预生产环境审查

```bash
# 审查预生产环境配置
claude /cloud-native-review preprod/ --environment preproduction --validate-security --validate-scalability

# 重点关注安全配置
claude /cloud-native-review preprod/ --module security --output json > security_review.json

# 重点关注性能配置
claude /cloud-native-review preprod/ --module scalability --output json > scalability_review.json
```

这些示例展示了如何在不同的场景下使用云原生应用审查技能，从基础验证到复杂的CI/CD集成，以及与其他金融开发技能的协同使用。