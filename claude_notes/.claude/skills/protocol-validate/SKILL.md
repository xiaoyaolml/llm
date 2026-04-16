---
name: protocol-validate
description: 金融协议实现验证（FIX、FAST、OUCH、ITCH等协议正确性）
---

# 金融协议实现验证

## 触发条件

用户输入 `/protocol-validate` 时执行，或当代码包含以下关键词时自动触发：
- `fix`、`fast`、`ouch`、`itch`、`protocol`、`message`、`codec`、`parser`、`serializer`

## 执行步骤

### 1. 识别协议类型

```
# 金融协议识别
1. FIX Protocol - 金融市场交易通信标准
2. FAST Protocol - FIX Adapted for Streaming
3. OUCH Protocol - NASDAQ OMX Direct Market Access
4. ITCH Protocol - NASDAQ TotalView-ITCH
5. BOE Protocol - CME Globex Binary Order Entry
6. 自定义协议 - 交易所特定二进制协议
```

### 2. 获取代码上下文

```bash
# 检测协议相关代码
git diff | grep -E "(fix|fast|ouch|itch|protocol|message)"  # 识别协议代码
grep -r "class.*Protocol\|struct.*Message" . --include=*.cpp  # 识别协议类
grep -r "encode\|decode\|serialize\|deserialize" . --include=*.cpp  # 识别编解码
```

### 3. 协议正确性审查维度

#### 3.1 消息格式验证 (Message Format Validation)
- [ ] **字段顺序**: 是否按照协议规范的字段顺序排列
- [ ] **必填字段**: 是否包含所有必需字段（Required Fields）
- [ ] **字段类型**: 字段类型是否符合协议规范（Int/String/Float/Data）
- [ ] **字段长度**: 可变长度字段是否符合长度限制
- [ ] **保留字段**: 保留字段是否正确填充（通常为 0 或空格）

#### 3.2 校验和验证 (Checksum Validation)
- [ ] **校验和计算**: 是否正确计算消息体的校验和
- [ ] **校验和位置**: 校验和字段是否在正确位置（通常在消息末尾）
- [ ] **校验和算法**: 是否使用协议规定的校验和算法（如 XOR、CRC）
- [ ] **校验和验证**: 接收端是否正确验证校验和
- [ ] **错误处理**: 校验和错误时是否有适当处理（丢弃、重传）

#### 3.3 会话管理 (Session Management)
- [ ] **登录流程**: 是否正确实现登录/登出流程
- [ ] **心跳机制**: 是否正确发送和响应心跳消息
- [ ] **序列号管理**: 是否正确管理和同步序列号
- [ ] **重传机制**: 是否支持消息重传和Gap Fill
- [ ] **会话状态**: 是否正确维护会话状态（Connected/Disconnected）

#### 3.4 错误处理 (Error Handling)
- [ ] **拒绝消息**: 是否正确处理和发送拒绝消息（Business Message Reject）
- [ ] **序列号问题**: 序列号不连续时的处理机制
- [ ] **消息类型未知**: 收到未知消息类型的处理
- [ ] **字段值错误**: 字段值不符合规范时的处理
- [ ] **超时处理**: 各种超时情况的处理（连接、响应、心跳）

#### 3.5 性能优化 (Performance Optimization)
- [ ] **零拷贝**: 是否避免不必要的消息拷贝
- [ ] **预分配**: 是否预分配消息缓冲区
- [ ] **批处理**: 是否支持消息批处理
- [ ] **内存池**: 是否使用内存池管理消息对象
- [ ] **编解码效率**: 编解码是否高效（避免字符串操作）

### 4. 严重级别定义

- **高危**：可能导致通信失败或数据错误
  - 校验和计算错误
  - 必填字段缺失
  - 序列号管理错误
  - 消息格式严重不符
- **中危**：可能影响性能或兼容性
  - 可选字段处理不当
  - 错误处理不完整
  - 会话管理缺陷
  - 性能优化不足
- **低危**：代码风格或文档问题
  - 注释不完整
  - 命名不规范
  - 未使用的代码

### 5. 输出格式

```
## 金融协议实现验证报告

**验证时间**: {{date_time}}
**协议类型**: {{protocol_type}} (FIX/FAST/OUCH/ITCH/BOE)
**协议版本**: {{protocol_version}} (如 FIX.4.4, FAST 1.2)
**验证范围**: {{scope}} (编码器/解码器/完整实现)

### 问题清单

| # | 文件 | 行号 | 问题描述 | 协议要求 | 严重级别 | 修复建议 |
|---|------|------|----------|----------|----------|----------|
| 1 | fix/encoder.cpp | 142 | 校验和计算错误 | FIX Spec 1.5 | 高 | 重新实现校验和算法 |

### 详细分析

#### 1. 消息格式问题
**问题**: 必填字段缺失
```cpp
// 当前代码
struct NewOrderSingle {
    std::string clordid;     // 11: ClOrdID (Required)
    std::string symbol;      // 55: Symbol (Required)
    // 缺失 HandlInst(21) 字段
    char side;               // 54: Side (Required)
};

// 修复建议
struct NewOrderSingle {
    std::string clordid;     // 11: ClOrdID (Required)
    std::string symbol;      // 55: Symbol (Required)
    char handlinst = '1';    // 21: HandlInst (Required) - Added
    char side;               // 54: Side (Required)
};
```

#### 2. 校验和问题
**问题**: 校验和计算不包括分隔符
```cpp
// 当前代码
uint8_t calculate_checksum(const std::string& msg) {
    uint8_t sum = 0;
    for (char c : msg) {
        sum += static_cast<uint8_t>(c);
    }
    return sum % 256;
}

// 修复建议 (FIX 协议要求包括 SOH 分隔符)
uint8_t calculate_checksum(const std::string& msg_body) {
    uint8_t sum = 0;
    for (char c : msg_body) {
        sum += static_cast<uint8_t>(c);
    }
    // FIX 要求校验和包括消息体和分隔符
    return sum % 256;
}
```

### 协议合规性评估

- **消息格式**: {{format_score}}/100 ({{format_status}})
- **校验和**: {{checksum_score}}/100 ({{checksum_status}})
- **会话管理**: {{session_score}}/100 ({{session_status}})
- **错误处理**: {{error_handling_score}}/100 ({{error_handling_status}})
- **性能优化**: {{performance_score}}/100 ({{performance_status}})

### 互操作性风险

⚠️ **警告**: 以下问题可能导致:
- 与交易所网关通信失败
- 消息被对方系统拒绝
- 序列号不同步导致会话中断
- 监管审计不通过

**建议**: 高危问题必须在生产部署前修复
```

### 6. 金融协议特定检查清单

#### FIX 协议检查
- [ ] 是否正确实现所有 Admin 消息（Logon, Logout, Heartbeat, Test Request, Resend Request, Reject, Sequence Reset）
- [ ] 是否正确处理所有 Application 消息（NewOrderSingle, ExecutionReport, OrderCancelRequest 等）
- [ ] 是否支持所有必需的会话参数（SenderCompID, TargetCompID, MsgSeqNum, SendingTime）
- [ ] 是否正确实现 BeginString 和 BodyLength 字段
- [ ] 是否正确处理时间和日期格式（UTC 时间）

#### FAST 协议检查
- [ ] 是否正确实现 PMap（Presence Map）
- [ ] 是否正确处理可选字段的编码
- [ ] 是否正确实现模板定义和应用
- [ ] 是否正确处理增量编码
- [ ] 是否正确实现操作码（Copy, Delta, Default, Constant, Increment）

#### 二进制协议检查
- [ ] 是否正确处理字节序（Big Endian vs Little Endian）
- [ ] 是否正确对齐字段边界
- [ ] 是否正确处理变长字段的长度前缀
- [ ] 是否正确实现打包和解包逻辑
- [ ] 是否处理边界条件和异常情况

## 集成建议

### CI/CD 集成
```bash
# .gitlab-ci.yml
protocol_validate:
  stage: test
  script:
    - claude /protocol-validate src/protocol/ --output json > protocol_report.json
    - python validate_protocol.py protocol_report.json
  artifacts:
    reports:
      protocol: protocol_report.json
```

### 本地开发集成
```bash
# pre-commit hook
#!/bin/bash
if git diff --cached | grep -q "protocol\|fix\|fast\|ouch\|itch"; then
    echo "🔍 协议实现验证中..."
    claude /protocol-validate --high-priority-only
    if [ $? -ne 0 ]; then
        echo "❌ 发现高危协议问题，提交被阻止"
        exit 1
    fi
fi
```

## 最佳实践

### 1. 协议消息结构最佳实践
```cpp
// FIX 消息基类
class FixMessage {
protected:
    std::map<int, std::string> fields_;
    uint8_t checksum_;
    
public:
    // 设置必填字段
    void set_field(int tag, const std::string& value) {
        if (value.empty()) {
            throw std::invalid_argument("Field value cannot be empty");
        }
        fields_[tag] = value;
    }
    
    // 获取字段值
    std::string get_field(int tag) const {
        auto it = fields_.find(tag);
        return (it != fields_.end()) ? it->second : "";
    }
    
    // 计算校验和
    uint8_t calculate_checksum() const {
        std::string body = build_message_body();
        uint8_t sum = 0;
        for (char c : body) {
            sum += static_cast<uint8_t>(c);
        }
        return sum % 256;
    }
    
    // 序列化消息
    std::string serialize() const {
        validate_required_fields();
        std::string body = build_message_body();
        uint8_t checksum = calculate_checksum();
        
        std::ostringstream oss;
        oss << "8=FIX.4.4\x01"  // BeginString
            << "9=" << body.length() << "\x01"  // BodyLength
            << body
            << "10=" << std::setfill('0') << std::setw(3) 
            << static_cast<int>(checksum) << "\x01";  // CheckSum
        
        return oss.str();
    }
};
```

### 2. 会话管理最佳实践
```cpp
// FIX 会话管理器
class FixSession {
private:
    uint32_t next_outgoing_seq_num_ = 1;
    uint32_t next_expected_seq_num_ = 1;
    std::string sender_comp_id_;
    std::string target_comp_id_;
    std::chrono::steady_clock::time_point last_heartbeat_;
    std::chrono::seconds heartbeat_interval_;
    
public:
    // 发送消息
    void send_message(FixMessage& msg) {
        msg.set_field(34, std::to_string(next_outgoing_seq_num_));  // MsgSeqNum
        msg.set_field(52, get_current_utc_time());  // SendingTime
        
        std::string serialized = msg.serialize();
        send_to_network(serialized);
        
        next_outgoing_seq_num_++;
        last_heartbeat_ = std::chrono::steady_clock::now();
    }
    
    // 接收消息
    bool receive_message(const std::string& raw_msg) {
        FixMessage msg = parse_message(raw_msg);
        uint32_t seq_num = std::stoul(msg.get_field(34));
        
        // 检查序列号
        if (seq_num != next_expected_seq_num_) {
            if (seq_num < next_expected_seq_num_) {
                // 重复消息，忽略
                return false;
            } else {
                // 序列号间隙，请求重传
                request_resend(next_expected_seq_num_, seq_num - 1);
                return false;
            }
        }
        
        // 处理消息
        process_message(msg);
        next_expected_seq_num_++;
        return true;
    }
};
```

### 3. 避免的陷阱
- ❌ 忽略必填字段检查
- ❌ 错误的校验和计算
- ❌ 不正确的序列号管理
- ❌ 未处理的消息类型
- ❌ 不规范的时间格式
- ❌ 未验证的字段值范围
- ❌ 不恰当的错误处理

## 更新日志

- **v1.0** (2026-04-09): 初始版本
  - FIX、FAST、OUCH、ITCH 协议验证
  - 消息格式、校验和、会话管理审查
  - 协议实现验证报告模板