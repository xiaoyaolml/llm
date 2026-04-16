---
name: finance-optim
description: 金融C++代码性能优化审查（低延迟、高吞吐、内存效率）
---

# 金融性能优化审查

## 触发条件

用户输入 `/finance-optim` 时执行，或当代码包含以下关键词时自动触发：
- `pricing`、`risk`、`trading`、`order`、`market`、`latency`、`throughput`

## 执行步骤

### 1. 识别优化目标

```
# 金融性能优先级
1. 延迟 (Latency) - 高频交易系统 (<100μs)
2. 吞吐量 (Throughput) - 风险计算系统 (>10K TPS)
3. 内存效率 - 大规模数据处理
4. 数值精度 - 财务计算准确性
```

### 2. 获取代码上下文

```bash
# 自动检测代码类型
git diff | grep -E "(pricing|risk|trading|order)"  # 识别金融代码
grep -r "std::chrono" . --include=*.cpp             # 识别时间关键代码
grep -r "new|malloc|allocator" . --include=*.cpp    # 识别内存分配
```

### 3. 金融性能审查维度

#### 3.1 延迟关键路径优化
- [ ] **动态内存分配**：高频路径是否避免 `new`/`malloc`
- [ ] **虚函数调用**：关键路径是否避免虚函数（使用 CRTP 或模板）
- [ ] **锁竞争**：是否使用无锁数据结构（CAS、原子操作）
- [ ] **缓存局部性**：数据结构是否对 CPU 缓存友好
- [ ] **分支预测**：高频路径是否分支预测友好

#### 3.2 金融特定优化
- [ ] **货币计算**：是否使用整数运算（避免 `double` 精度问题）
- [ ] **价格档位**：是否使用快速查找（哈希表 vs 二叉树）
- [ ] **随机数生成**：蒙特卡洛模拟是否使用快速 RNG（XorShift vs MT19937）
- [ ] **向量化**：风险计算是否可 SIMD 优化（AVX2/AVX512）

#### 3.3 内存效率
- [ ] **对象池**：高频对象是否使用内存池
- [ ] **零拷贝**：数据传输是否避免拷贝（引用传递、共享内存）
- [ ] **对齐分配**：关键数据结构是否内存对齐（`alignas(64)`）
- [ ] **缓存行填充**：是否避免 false sharing（添加 padding）

#### 3.4 编译器优化
- [ ] **内联提示**：关键小函数是否 `__attribute__((always_inline))`
- [ ] **循环展开**：关键循环是否手动展开或编译器提示
- [ ] **死代码消除**：调试代码是否用 `__builtin_expect` 隔离
- [ ] **链接时优化**：是否启用 LTO（`-flto`）

### 4. 严重级别定义

- **高危**：直接影响延迟/吞吐量的问题（每次执行 >1μs 开销）
  - 动态内存分配在高频路径
  - 虚函数调用在关键路径
  - 锁竞争在高频操作
- **中危**：可优化但不影响核心性能
  - 未对齐内存访问
  - 未优化的数据结构
  - 未充分利用 SIMD
- **低危**：风格或微优化建议
  - 未使用 `const` 修饰符
  - 未优化的循环变量

### 5. 输出格式

```
## 金融性能优化审查报告

**审查时间**: {{date_time}}
**代码类型**: {{code_type}} (交易引擎/风险计算/市场数据)
**性能目标**: {{target}} (<100μs 延迟 / >10K TPS 吞吐)

### 问题清单

| # | 文件 | 行号 | 问题描述 | 严重级别 | 优化建议 | 预期收益 |
|---|------|------|----------|----------|----------|----------|
| 1 | engine/order_matcher.cpp | 156 | 高频路径使用 std::map | 高 | 改用 std::unordered_map | 50μs → 15μs |

### 详细分析

#### 1. 延迟关键路径
**问题**: 高频路径包含动态内存分配
```cpp
// 当前代码
auto position = new Position();
position->update(order);

// 优化建议
static thread_local ObjectPool<Position> position_pool;
auto position = position_pool.allocate();
position->update(order);
```

#### 2. 金融计算精度
**问题**: 货币计算使用 double
```cpp
// 当前代码
double price = base_price * (1 + rate/100.0);

// 优化建议 (整数运算)
int64_t base_cents = base_price * 100;
int64_t price_cents = base_cents * (10000 + rate) / 10000;
```

### 优化建议优先级

1. **必须优化** (高危问题):
   - [ ] 替换动态内存分配为对象池
   - [ ] 消除关键路径虚函数调用

2. **建议优化** (中危问题):
   - [ ] 优化数据结构布局
   - [ ] 启用 SIMD 指令

3. **可选优化** (低危问题):
   - [ ] 添加编译器优化提示
   - [ ] 代码风格优化

### 预期性能提升
- **延迟**: 250μs → 85μs (66% 降低)
- **吞吐量**: 5K TPS → 15K TPS (200% 提升)
- **内存使用**: 1.2GB → 800MB (33% 降低)

### 风险提示
⚠️ **警告**: 以下优化可能引入风险：
- 无锁编程需要严格的测试
- SIMD 优化可能影响可移植性
- 低延迟优化可能增加代码复杂性

**建议**: 每次只优化一个维度，充分测试后再进行下一项
```

### 6. 金融特定检查清单

#### 交易引擎检查
- [ ] 订单匹配是否 O(1) 时间复杂度
- [ ] 价格档位更新是否无锁
- [ ] 市场数据处理是否零拷贝
- [ ] 网络 I/O 是否使用 DPDK 或 kernel bypass

#### 风险计算检查
- [ ] VaR 计算是否可并行化
- [ ] 蒙特卡洛路径是否批量生成
- [ ] 随机数生成器是否线程安全
- [ ] 数值积分是否自适应精度

#### 数据处理检查
- [ ] tick 数据是否压缩存储
- [ ] 历史数据是否内存映射 (mmap)
- [ ] 实时分析是否流式处理
- [ ] 缓存是否预热关键数据

## 集成建议

### CI/CD 集成
```bash
# .gitlab-ci.yml
performance_review:
  stage: test
  script:
    - claude /finance-optim src/trading/ --output json > perf_report.json
    - python validate_performance.py perf_report.json
  artifacts:
    reports:
      performance: perf_report.json
```

### 本地开发集成
```bash
# pre-commit hook
#!/bin/bash
if git diff --cached | grep -q "src/trading\|src/risk"; then
    echo "🔍 金融性能审查中..."
    claude /finance-optim --high-priority-only
    if [ $? -ne 0 ]; then
        echo "❌ 发现高危性能问题，提交被阻止"
        exit 1
    fi
fi
```

## 最佳实践

### 1. 测量驱动优化
```cpp
// 优化前先测量
auto start = std::chrono::high_resolution_clock::now();
// ... 待优化代码 ...
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
std::cout << "Execution time: " << duration.count() << "μs" << std::endl;
```

### 2. 金融代码优化技巧
- **整数运算**: 货币计算统一使用 cents (分)
- **无锁编程**: 使用 `std::atomic` + CAS 操作
- **内存池**: 预分配高频使用的对象
- **SIMD**: 使用 `#pragma omp simd` 或 Eigen 库

### 3. 避免的陷阱
- ❌ 过早优化未测量的代码
- ❌ 为了性能牺牲可读性（关键算法除外）
- ❌ 忽略数值稳定性（金融计算的核心）
- ❌ 未测试极端市场条件

## 更新日志

- **v1.0** (2026-04-08): 初始版本
  - 延迟、吞吐量、内存效率审查
  - 金融特定规则（货币计算、价格档位）
  - 性能优化报告模板