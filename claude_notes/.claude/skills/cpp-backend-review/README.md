# C++后端专项审查技能 (cpp-backend-review)

## 🚀 快速开始

### 安装
```bash
# 技能已安装在 ~/.claude/skills/cpp-backend-review/
# 重启 Claude Code 使技能生效
claude restart
```

### 首次使用
```bash
# 执行C++后端专项审查
claude /cpp-backend-review src/backend/

# 指定审查维度
claude /cpp-backend-review src/backend/ --dimension memory-management

# 输出JSON格式报告
claude /cpp-backend-review src/backend/ --output json

# 详细模式
claude /cpp-backend-review src/backend/ --verbose
```

## ✨ 核心功能

- **内存管理**：检测内存泄漏、检查智能指针使用
- **线程安全**：验证无锁编程、检查线程同步机制
- **性能优化**：分析函数调用开销、优化内存布局
- **异常处理**：审查异常安全保证、检查RAII实践
- **模板编程**：验证SFINAE应用、检查编译期优化
- **C++20特性**：评估协程、概念、模块等新特性使用

## 📚 使用示例

### 1. 内存管理审查
```bash
# 深度检查内存使用模式
claude /cpp-backend-review src/memory/ --dimension memory-management --verbose
```

### 2. 线程安全验证
```bash
# 评估高频交易系统并发安全性
claude /cpp-backend-review src/concurrency/ --dimension thread-safety --system-type hft
```

### 3. C++20特性审查
```bash
# 检查协程实现的正确性
claude /cpp-backend-review src/coroutines/ --dimension cpp20-features --feature coroutines
```

## 🔌 集成指南

### CI/CD 集成
```yaml
- name: C++ Backend Review
  run: claude /cpp-backend-review src/ --output json > cpp_backend_report.json
```

### 预提交钩子
```bash
#!/bin/sh
claude /cpp-backend-review --high-priority-only || exit 1
```

## 📅 版本历史

- **v1.0.0 (2026-04-09)**: 初始版本，包含C++后端核心审查功能