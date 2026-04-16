# 金融性能优化报告模板

## 审查概要

**报告ID**: `FIN-OPT-{{timestamp}}`  
**审查时间**: `{{date_time}}`  
**审查者**: `Claude Code /finance-optim`  
**代码仓库**: `{{repository}}`  
**分支**: `{{branch}}`  

**性能指标**:  
- **延迟目标**: `{{latency_target}}μs` (当前: `{{current_latency}}μs`)  
- **吞吐量目标**: `{{throughput_target}} TPS` (当前: `{{current_throughput}} TPS`)  
- **内存使用**: `{{memory_usage}} MB` (目标: `{{memory_target}} MB`)  

---

## 问题详情

### 🔴 高危问题 (必须修复)

{{#high_issues}}
#### {{index}}. {{file}}:{{line}} - {{title}}

**问题描述**:  
{{description}}

**当前代码**:
```cpp
{{current_code}}
```

**优化建议**:
```cpp
{{optimized_code}}
```

**预期收益**:  
- 延迟降低: {{latency_reduction}}μs
- 吞吐量提升: {{throughput_improvement}}%
- 风险等级: ⚠️ 高 (需要充分测试)

**验证建议**:  
```bash
# 性能测试命令
./benchmark --function {{function_name}} --iterations 100000
```
{{/high_issues}}

### 🟡 中危问题 (建议修复)

{{#medium_issues}}
#### {{index}}. {{file}}:{{line}} - {{title}}

**问题描述**:  
{{description}}

**优化建议**:  
```cpp
{{optimized_code}}
```

**预期收益**:  
- 性能提升: {{performance_gain}}%
- 代码可维护性: {{maintainability_improvement}}

**实施难度**: {{difficulty_level}} (1-5)
{{/medium_issues}}

### 🟢 低危问题 (可选修复)

{{#low_issues}}
- **{{file}}:{{line}}** - {{description}} (收益: {{benefit}})
{{/low_issues}}

---

## 优化建议汇总

### 1. 架构优化建议
{{#architecture_suggestions}}
- **{{title}}**: {{description}} (预计收益: {{expected_gain}})
{{/architecture_suggestions}}

### 2. 算法优化建议
{{#algorithm_suggestions}}
- **{{algorithm}}**: 从 {{current_complexity}} → {{target_complexity}} ({{description}})
{{/algorithm_suggestions}}

### 3. 数据结构优化建议
{{#data_structure_suggestions}}
- **{{structure}}**: {{old_structure}} → {{new_structure}} ({{benefits}})
{{/data_structure_suggestions}}

---

## 预期收益总结

| 指标 | 当前值 | 优化后 | 提升幅度 | 优先级 |
|------|--------|--------|----------|--------|
| P99 延迟 | {{current_p99}}μs | {{target_p99}}μs | {{latency_improvement}}% | 🔴 |
| 吞吐量 | {{current_tps}} TPS | {{target_tps}} TPS | {{throughput_improvement}}% | 🔴 |
| 内存使用 | {{current_memory}} MB | {{target_memory}} MB | {{memory_improvement}}% | 🟡 |
| CPU 利用率 | {{current_cpu}}% | {{target_cpu}}% | {{cpu_improvement}}% | 🟡 |

**总体预期**: 优化后系统性能将提升 {{overall_improvement}}%，满足金融级低延迟要求。

---

## 风险评估

### 🔴 高风险改动
{{#high_risk_changes}}
- **{{change}}**: 可能导致 {{risk_description}}，需要 {{mitigation_strategy}}
{{/high_risk_changes}}

### 🟡 中风险改动
{{#medium_risk_changes}}
- **{{change}}**: 需要 {{testing_requirement}} 测试
{{/medium_risk_changes}}

### 🟢 低风险改动
{{#low_risk_changes}}
- **{{change}}**: 可安全实施
{{/low_risk_changes}}

---

## 实施计划

### 第一阶段 (1-2天) - 高危问题修复
{{#phase1_tasks}}
1. [ ] **{{task}}** (负责人: {{owner}}, 预计时间: {{time_estimate}})
{{/phase1_tasks}}

### 第二阶段 (3-5天) - 中危问题修复
{{#phase2_tasks}}
2. [ ] **{{task}}** (负责人: {{owner}}, 预计时间: {{time_estimate}})
{{/phase2_tasks}}

### 第三阶段 (1周+) - 架构优化
{{#phase3_tasks}}
3. [ ] **{{task}}** (负责人: {{owner}}, 预计时间: {{time_estimate}})
{{/phase3_tasks}}

---

## 验证计划

### 性能测试
```bash
# 延迟测试
./latency_test --target {{target_latency}}us --duration 60s

# 吞吐量测试  
./throughput_test --target {{target_tps}}tps --concurrency 100

# 压力测试
./stress_test --max-load 200% --duration 300s
```

### 正确性验证
```cpp
// 数值精度测试
TEST_F(PerformanceTest, PricingAccuracy) {
    double expected = 100.001;
    double actual = engine.price(option);
    ASSERT_NEAR(expected, actual, 1e-6); // 1e-6 精度要求
}

// 并发正确性测试
TEST_F(ConcurrencyTest, ThreadSafety) {
    std::vector<std::thread> threads;
    for (int i = 0; i < 100; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 1000; ++j) {
                engine.process_order(order);
            }
        });
    }
    for (auto& t : threads) t.join();
    ASSERT_EQ(engine.get_total_orders(), 100000);
}
```

---

## 附录

### 参考资料
- [Intel 低延迟编程指南](https://www.intel.com/content/www/us/en/developer/articles/technical/low-latency-programming.html)
- [C++ 性能优化最佳实践](https://github.com/lefticus/cppbestpractices)
- [金融系统架构设计](https://martinfowler.com/articles/financial-architecture.html)

### 工具推荐
- **性能分析**: Intel VTune, perf, gperftools
- **内存分析**: Valgrind, AddressSanitizer
- **并发测试**: ThreadSanitizer, Helgrind
- **数值验证**: QuantLib, Boost.Test

### 联系方式
- **性能团队**: perf-team@company.com
- **紧急支持**: +86-138-0013-8000
- **文档**: https://internal-docs.company.com/performance

---

**报告生成时间**: {{generation_time}}  
**工具版本**: Claude Code v2.1 /finance-optim v1.0  
**保密级别**: 🔒 内部机密 - 仅限授权人员访问