# C++ 特定检查清单

## 必查项

- [ ] 使用 `std::unique_ptr` 或 `std::shared_ptr` 管理动态内存（避免裸 `new`/`delete`）
- [ ] `delete[]` 与 `new[]` 配对使用
- [ ] 类析构函数是否 `virtual`（如果类设计为基类）
- [ ] 拷贝构造函数/拷贝赋值运算符是否实现或 `= delete`
- [ ] `const` 成员函数是否正确标记（不修改成员状态）
- [ ] 循环中使用引用避免拷贝（`for (const auto& item : vec)`）
- [ ] STL 容器访问使用 `at()` 时是否捕获 `std::out_of_range` 异常
- [ ] 枚举类型是否使用 `enum class`（强类型枚举）
- [ ] 头文件是否使用 `#pragma once` 或传统包含守卫
- [ ] 宏定义是否全大写且避免副作用（如 `#define SQUARE(x) ((x)*(x))`）

## 高风险模式

| 模式                                  | 风险         | 修复建议                                      |
| ------------------------------------- | ------------ | --------------------------------------------- |
| `int* p = new int[10]; delete p;`   | 内存泄漏     | 改用 `delete[] p;` 或 `std::vector<int>`  |
| `strcpy(buf, input);`               | 缓冲区溢出   | 改用 `strncpy` + 手动终止或 `std::string` |
| `if (ptr)`                          | 未初始化指针 | 改为 `if (ptr != nullptr)`                  |
| `class A { public: ~A() { ... } };` | 非虚析构     | 如果 `A` 被继承，析构函数必须 `virtual`   |
| `return local_string.c_str();`      | 悬垂指针     | 返回 `std::string` 或确保生命周期           |

## 现代 C++ 替代

| 旧写法                                      | 现代写法                          | 优点                      |
| ------------------------------------------- | --------------------------------- | ------------------------- |
| `for (size_t i = 0; i < vec.size(); ++i)` | `for (const auto& x : vec)`     | 简洁，避免索引错误        |
| `if (ptr != nullptr)`                     | `if (ptr)`                      | 更简洁（C++11 起）        |
| `int* arr = new int[n];`                  | `std::vector<int> arr(n);`      | 自动管理内存              |
| `std::bind(&func, obj, _1)`               | `[&](auto x) { func(obj, x); }` | 可读性更好                |
| `std::auto_ptr`                           | `std::unique_ptr`               | C++17 已移除 `auto_ptr` |

## 编译器警告检查

- [ ] 启用 `-Wall -Wextra` 后无警告
- [ ] 无 `unused variable`（未使用变量）
- [ ] 无 `comparison between signed and unsigned`（符号不匹配）
- [ ] 无 `control reaches end of non-void function`（非 `void` 函数未返回）

## 代码示例对比

**差：**

```cpp
class Stack {
public:
    int* data;
    int size;
    Stack(int s) {
        data = new int[s];
        size = s;
    }
    ~Stack() {
        delete data;  // 错误：应为 delete[]
    }
};
```

**好：**

```cpp
class Stack {
public:
    std::vector<int> data;
    Stack(int s) : data(s) {}
    // 无需析构函数，vector 自动管理
};
```

---

# Python 特定检查清单

## 必查项

- [ ] 是否使用 `with` 管理文件/资源（避免手动 `close()`）
- [ ] 字符串拼接是否用 `join()` 而非多次 `+=`
- [ ] 是否使用类型注解（`def foo(x: int) -> str:`）
- [ ] 异常处理是否具体（避免裸 `except:`）
- [ ] 列表推导式是否用于简单转换（而非嵌套循环）
- [ ] 魔法方法（如 `__str__`、`__eq__`）是否正确实现

## 高风险模式

| 模式                       | 风险         | 修复建议                                            |
| -------------------------- | ------------ | --------------------------------------------------- |
| `except:`                | 掩盖所有异常 | 改为 `except ValueError:` 等具体异常              |
| `if x == None:`          | 不规范比较   | 改为 `if x is None:`                              |
| `def foo(x=[])`          | 可变默认参数 | 改为 `def foo(x=None)` + `if x is None: x = []` |
| `exec(user_input)`       | 代码注入     | 完全避免，或使用 `ast.literal_eval`               |
| `password = "hardcoded"` | 信息泄露     | 使用环境变量或配置文件                              |

## 现代 Python 替代

| 旧写法                       | 现代写法            | 优点                 |
| ---------------------------- | ------------------- | -------------------- |
| `for i in range(len(lst))` | `for item in lst` | 简洁                 |
| `if x is not None:`        | 同左                | 清晰                 |
| `dict.get(key, default)`   | `dict[key]` + try | 避免 KeyError        |
| `from typing import List`  | `list[int]`       | Python 3.9+ 原生支持 |
| `str.format()`             | `f"{var}"`        | 可读性更好           |

---

# Go 特定检查清单

## 必查项

- [ ] 错误是否全部检查（`if err != nil`）
- [ ] goroutine 是否有合适的生命周期管理
- [ ] `defer` 是否正确释放资源（如文件、锁）
- [ ] 切片扩容是否预分配 `cap` 避免多次分配
- [ ] 接口是否小而专（Go 哲学）
- [ ] 是否避免全局变量

## 高风险模式

| 模式                                                 | 风险             | 修复建议                                                              |
| ---------------------------------------------------- | ---------------- | --------------------------------------------------------------------- |
| `go func() { ... }()`                              | goroutine 泄漏   | 使用 `context` 控制生命周期                                         |
| `for _, v := range items { go func() { use(v) } }` | 闭包捕获循环变量 | 改为 `for i := range items { v := items[i]; go func() { use(v) } }` |
| `map[string]int` 无锁并发                          | 数据竞争         | 使用 `sync.Map` 或加锁                                              |
| `select {}` 无 default                             | 阻塞             | 添加超时或 `default`                                                |

---

# JavaScript/TypeScript 特定检查清单

## 必查项

- [ ] 是否使用 `const`/`let` 而非 `var`
- [ ] 异步操作是否正确使用 `async/await` 或 `.then()`
- [ ] 是否避免 `==`（改用 `===`）
- [ ] 对象属性访问是否检查 `undefined`/`null`
- [ ] TypeScript 是否有完整的类型注解
- [ ] 是否避免 `any`（除非必要）

## 高风险模式

| 模式                      | 风险       | 修复建议                             |
| ------------------------- | ---------- | ------------------------------------ |
| `if (x == null)`        | 类型不安全 | 改为 `if (x === null                 |
| `userInput.split(',')`  | 未验证输入 | 先校验格式                           |
| `innerHTML = userInput` | XSS 攻击   | 改用 `textContent` 或转义          |
| `setTimeout(fn, 0)`     | 隐式队列   | 明确用途或改用 `Promise.resolve()` |
