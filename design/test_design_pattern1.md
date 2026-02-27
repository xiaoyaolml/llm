# C++ 设计模式完全教程 —— 从零开始

## 目录
- [一、设计模式概述](#一设计模式概述)
- [二、设计原则（SOLID）](#二设计原则solid)
- [三、创建型模式（Creational Patterns）](#三创建型模式creational-patterns)
  - [1. 单例模式 Singleton](#1-单例模式-singleton)
  - [2. 工厂方法模式 Factory Method](#2-工厂方法模式-factory-method)
  - [3. 抽象工厂模式 Abstract Factory](#3-抽象工厂模式-abstract-factory)
  - [4. 建造者模式 Builder](#4-建造者模式-builder)
  - [5. 原型模式 Prototype](#5-原型模式-prototype)
- [四、结构型模式（Structural Patterns）](#四结构型模式structural-patterns)
  - [6. 适配器模式 Adapter](#6-适配器模式-adapter)
  - [7. 桥接模式 Bridge](#7-桥接模式-bridge)
  - [8. 组合模式 Composite](#8-组合模式-composite)
  - [9. 装饰器模式 Decorator](#9-装饰器模式-decorator)
  - [10. 外观模式 Facade](#10-外观模式-facade)
  - [11. 享元模式 Flyweight](#11-享元模式-flyweight)
  - [12. 代理模式 Proxy](#12-代理模式-proxy)
- [五、行为型模式（Behavioral Patterns）](#五行为型模式behavioral-patterns)
  - [13. 策略模式 Strategy](#13-策略模式-strategy)
  - [14. 观察者模式 Observer](#14-观察者模式-observer)
  - [15. 命令模式 Command](#15-命令模式-command)
  - [16. 模板方法模式 Template Method](#16-模板方法模式-template-method)
  - [17. 状态模式 State](#17-状态模式-state)
  - [18. 责任链模式 Chain of Responsibility](#18-责任链模式-chain-of-responsibility)
  - [19. 迭代器模式 Iterator](#19-迭代器模式-iterator)
  - [20. 中介者模式 Mediator](#20-中介者模式-mediator)
  - [21. 备忘录模式 Memento](#21-备忘录模式-memento)
  - [22. 访问者模式 Visitor](#22-访问者模式-visitor)
  - [23. 解释器模式 Interpreter](#23-解释器模式-interpreter)
- [六、现代C++设计模式进阶](#六现代c设计模式进阶)
- [七、设计模式总结与选型指南](#七设计模式总结与选型指南)

---

## 一、设计模式概述

### 1.1 什么是设计模式？
设计模式（Design Pattern）是软件开发中**反复出现的问题的通用解决方案**。它不是可以直接复制粘贴的代码，而是描述了在特定场景下如何优雅地解决问题的**模板和思想**。

1994年，Erich Gamma、Richard Helm、Ralph Johnson、John Vlissides（合称 **GoF，Gang of Four**）在《Design Patterns: Elements of Reusable Object-Oriented Software》中总结了 **23种经典设计模式**，分为三大类：

| 类别 | 模式数量 | 核心关注点 |
|------|---------|-----------|
| **创建型模式** (Creational) | 5 | **对象的创建机制** — 如何灵活创建对象 |
| **结构型模式** (Structural) | 7 | **类和对象的组合** — 如何组装更大的结构 |
| **行为型模式** (Behavioral) | 11 | **对象间的交互与职责分配** — 如何高效沟通 |

### 1.2 为什么要学习设计模式？
```
❌ 没有设计模式的世界：
- 代码耦合度高，牵一发而动全身
- 无法应对需求变化，修改一处需要改动几十处
- 代码复用性差，大量重复逻辑
- 团队协作困难，每个人写法不同

✅ 使用设计模式的世界：
- 代码松耦合，模块间职责清晰
- 对扩展开放，对修改关闭（OCP）
- 代码复用性强，通过组合实现灵活扩展
- 团队共享"设计词汇"，沟通效率大增
```

### 1.3 学习路线建议
```
第一阶段（必学，高频使用）：
  单例 → 工厂方法 → 策略 → 观察者 → 装饰器 → 代理

第二阶段（进阶，中频使用）：
  建造者 → 适配器 → 外观 → 模板方法 → 命令 → 状态

第三阶段（深入，低频但重要）：
  抽象工厂 → 桥接 → 组合 → 享元 → 责任链 → 访问者 → 迭代器 → 中介者 → 备忘录
```

---

## 二、设计原则（SOLID）

设计模式的基石是**面向对象设计原则**，其中最著名的是 SOLID 原则：

### 2.1 单一职责原则（SRP - Single Responsibility Principle）
> **一个类应该只有一个引起它变化的原因。**

```cpp
// ❌ 违反 SRP：一个类干了太多事
class Employee {
    void calculatePay();    // 薪资计算
    void saveToDatabase();  // 数据库操作
    void generateReport();  // 报表生成
};

// ✅ 遵循 SRP：每个类只负责一件事
class PayCalculator { void calculatePay(Employee&); };
class EmployeeRepository { void save(Employee&); };
class ReportGenerator { void generate(Employee&); };
```

### 2.2 开闭原则（OCP - Open/Closed Principle）
> **对扩展开放，对修改关闭。** 添加新功能时，不应修改已有代码。

```cpp
// ❌ 违反 OCP：每加一种形状都要改 drawShape
void drawShape(Shape& s) {
    if (s.type == CIRCLE) drawCircle(s);
    else if (s.type == RECTANGLE) drawRectangle(s);
    // 加新形状？必须改这里！
}

// ✅ 遵循 OCP：通过多态扩展
class Shape { virtual void draw() = 0; };
class Circle : public Shape { void draw() override { /*...*/ } };
class Rectangle : public Shape { void draw() override { /*...*/ } };
// 加新形状？只需新增一个子类！
```

### 2.3 里氏替换原则（LSP - Liskov Substitution Principle）
> **子类对象必须能替换其父类对象，且不改变程序的正确性。**

### 2.4 接口隔离原则（ISP - Interface Segregation Principle）
> **不要强迫客户端依赖它不需要的接口。** 接口要小而精。

### 2.5 依赖倒置原则（DIP - Dependency Inversion Principle）
> **高层模块不应该依赖低层模块，两者都应该依赖抽象。**

```cpp
// ❌ 高层直接依赖低层
class MySQLDatabase { void save(); };
class UserService {
    MySQLDatabase db;  // 直接依赖具体类！换数据库就要改
};

// ✅ 都依赖抽象
class IDatabase { virtual void save() = 0; };
class MySQLDatabase : public IDatabase { void save() override; };
class PostgresDB : public IDatabase { void save() override; };
class UserService {
    std::unique_ptr<IDatabase> db;  // 依赖抽象！
};
```

---

## 三、创建型模式（Creational Patterns）

> 核心思想：**将对象的创建与使用分离**，提高对象创建的灵活性。

### 1. 单例模式 Singleton

#### 意图
确保一个类**只有一个实例**，并提供全局访问点。

#### 适用场景
- 配置管理器（全局唯一配置）
- 日志系统（全局唯一日志器）
- 线程池、数据库连接池
- 设备驱动管理器

#### UML 结构
```
┌─────────────────────────┐
│      Singleton           │
├─────────────────────────┤
│ - instance: Singleton*   │
│ - Singleton()            │
├─────────────────────────┤
│ + getInstance(): S&      │
│ + doSomething()          │
└─────────────────────────┘
```

#### 关键要点
1. **私有构造函数** — 防止外部创建实例
2. **删除拷贝和赋值** — 防止复制
3. **静态方法获取实例** — 全局唯一访问点
4. C++11 的 **Meyers' Singleton**（局部静态变量）是最简洁且线程安全的写法

#### 代码详解
见 `test_design_pattern1.cpp` 中 `namespace Singleton` 部分。

---

### 2. 工厂方法模式 Factory Method

#### 意图
定义一个创建对象的接口，但让**子类决定实例化哪个类**。工厂方法使一个类的实例化延迟到其子类。

#### 适用场景
- 不知道确切需要创建哪个类的对象
- 希望将对象创建的职责委托给子类
- 日志框架（创建不同类型的Logger）
- UI框架（创建不同操作系统的控件）

#### UML 结构
```
┌──────────────┐         ┌──────────────┐
│   Product     │◁────────│ConcreteProduct│
│  (抽象产品)    │         │  (具体产品)    │
└──────────────┘         └──────────────┘
       △                        △
       │                        │ creates
┌──────────────┐         ┌──────────────┐
│   Creator     │◁────────│ConcreteCreator│
│  (抽象工厂)    │         │  (具体工厂)    │
│ +create()=0   │         │ +create()     │
└──────────────┘         └──────────────┘
```

#### 关键要点
1. **Product**：定义产品接口
2. **Creator**：声明工厂方法（`virtual create() = 0`）
3. **ConcreteCreator**：实现工厂方法，返回具体产品
4. 客户端代码只与 Creator 和 Product 打交道，不依赖具体类

---

### 3. 抽象工厂模式 Abstract Factory

#### 意图
提供一个创建**一系列相关或相互依赖对象**的接口，而无需指定它们的具体类。

#### 适用场景
- 跨平台 UI 工具包（Windows按钮 + Windows滚动条 vs Mac按钮 + Mac滚动条）
- 数据库访问层（MySQL连接+MySQL命令 vs PostgreSQL连接+PostgreSQL命令）
- 游戏引擎（不同图形API的渲染器+纹理+着色器）

#### 与工厂方法的区别
| 特征 | 工厂方法 | 抽象工厂 |
|------|---------|---------|
| 产品数量 | **一种**产品 | **一族**相关产品 |
| 实现方式 | 继承（子类决定） | 组合（工厂对象） |
| 扩展方向 | 增加新产品容易 | 增加新产品族容易 |

---

### 4. 建造者模式 Builder

#### 意图
将一个复杂对象的**构建与表示分离**，使得同样的构建过程可以创建不同的表示。

#### 适用场景
- 构建复杂对象（需要多步骤初始化）
- 配置类（大量可选参数）
- SQL查询构建器
- HTTP请求构建器

#### 关键要点
1. **Builder**：定义构建步骤的接口
2. **ConcreteBuilder**：实现具体的构建步骤
3. **Director**（可选）：定义构建的顺序
4. 支持**链式调用**（Fluent Interface）

---

### 5. 原型模式 Prototype

#### 意图
通过**复制现有对象**来创建新对象，而不是通过构造函数。

#### 适用场景
- 对象创建成本高（如需要数据库查询或复杂计算）
- 需要动态配置的对象（在运行时才知道具体类型）
- 游戏中克隆怪物/道具模板

#### 关键要点
1. 核心是 `clone()` 方法
2. 注意**深拷贝 vs 浅拷贝**的问题
3. C++ 中常通过拷贝构造函数 + `clone()` 虚函数实现

---

## 四、结构型模式（Structural Patterns）

> 核心思想：**如何组合类和对象形成更大的结构**，同时保持灵活和高效。

### 6. 适配器模式 Adapter

#### 意图
将一个类的接口**转换为客户端期望的另一种接口**，使原本不兼容的类可以协作。

#### 生活类比
电源适配器：中国的220V插头 → 适配器 → 美国的110V插座

#### 适用场景
- 集成第三方库（接口不匹配）
- 旧系统迁移（包装旧接口）
- 统一不同数据源的接口

#### 两种实现方式
- **类适配器**：通过多重继承实现
- **对象适配器**：通过组合实现（推荐）

---

### 7. 桥接模式 Bridge

#### 意图
将**抽象部分与实现部分分离**，使它们可以独立变化。

#### 生活类比
遥控器（抽象） ←→ 电视机（实现）  
不同品牌的遥控器可以控制不同品牌的电视

#### 适用场景
- 多维度变化（形状 × 颜色，平台 × 渲染方式）
- 避免类爆炸（如果用继承：Red-Circle, Blue-Circle, Red-Square...）
- 运行时切换实现

#### 关键要点
- 抽象层持有实现层的指针（"桥"）
- 两个维度可以独立扩展

---

### 8. 组合模式 Composite

#### 意图
将对象组合成**树形结构**以表示"部分-整体"层次。使得客户端对**单个对象和组合对象的使用一致**。

#### 适用场景
- 文件系统（文件/文件夹）
- UI控件（按钮/面板/窗口）
- 组织架构（员工/部门/公司）
- 表达式树

---

### 9. 装饰器模式 Decorator

#### 意图
**动态**地给对象添加额外职责。比继承更灵活。

#### 生活类比
咖啡 → 加牛奶 → 加糖 → 加奶泡  
每一步都在原有基础上"装饰"，而不是做一杯全新的咖啡

#### 适用场景
- I/O流（BufferedStream装饰FileStream）
- 日志增强
- 权限检查包装
- 数据压缩/加密

#### 关键要点
1. Decorator 和 Component 有**相同的接口**
2. Decorator **持有** Component 的引用
3. 可以**嵌套多层**装饰
4. 顺序可以随意组合

---

### 10. 外观模式 Facade

#### 意图
为子系统的一组接口提供一个**统一的高级接口**，使子系统更容易使用。

#### 生活类比
电脑开机按钮就是 Facade — 你按一下，内部 CPU、内存、硬盘、显卡全部启动。

#### 适用场景
- 简化复杂子系统的使用
- 分层架构中为每层提供入口
- 第三方库的简化封装

---

### 11. 享元模式 Flyweight

#### 意图
通过**共享**大量细粒度对象来高效支持大规模对象。

#### 适用场景
- 文本编辑器（字符对象共享字体/大小信息）
- 游戏中大量粒子/树木/子弹
- 缓存池

#### 关键要点
- **内在状态**（可共享）vs **外在状态**（不可共享，需外部传入）
- 通常配合工厂 + `unordered_map` 缓存

---

### 12. 代理模式 Proxy

#### 意图
为另一个对象提供一个**代理或占位符**，以控制对原对象的访问。

#### 类型
| 代理类型 | 用途 |
|---------|------|
| **远程代理** | 代表远程对象（RPC） |
| **虚拟代理** | 延迟创建开销大的对象 |
| **保护代理** | 控制访问权限 |
| **缓存代理** | 缓存请求结果 |
| **智能引用** | 引用计数（`shared_ptr`即是代理模式） |

---

## 五、行为型模式（Behavioral Patterns）

> 核心思想：**对象之间如何高效沟通与分配职责**。

### 13. 策略模式 Strategy

#### 意图
定义**一系列算法**，将每个算法封装起来，并使它们可以**互换**。让算法的变化独立于使用算法的客户端。

#### 生活类比
导航APP：从A到B，你可以选择"最快路线"、"最短路线"、"避开高速"等**不同策略**。

#### 适用场景
- 排序算法选择
- 支付方式（微信/支付宝/银行卡）
- 压缩算法
- 路径规划

#### 关键要点
1. **Context**：持有策略引用，委托策略执行
2. **Strategy**：算法的公共接口
3. **ConcreteStrategy**：各种具体算法
4. 可以运行时动态切换策略

---

### 14. 观察者模式 Observer

#### 意图
定义对象间的**一对多**依赖关系，当被观察对象状态变化时，**所有依赖者自动收到通知**。

#### 生活类比
微信公众号：你关注了一个公众号（订阅），公众号发新文章时（状态变化），所有粉丝都收到推送（通知）。

#### 适用场景
- 事件系统
- GUI事件处理
- 数据绑定（MVC/MVVM）
- 消息队列

---

### 15. 命令模式 Command

#### 意图
将**请求封装成对象**，从而可以用不同请求对客户进行参数化，支持**撤销/重做**等操作。

#### 适用场景
- 撤销/重做系统（Ctrl+Z / Ctrl+Y）
- 任务队列
- 宏命令（批量执行）
- 事务系统

---

### 16. 模板方法模式 Template Method

#### 意图
定义一个操作中算法的**骨架**，将某些步骤的实现**延迟到子类**。

#### 适用场景
- 框架设计（定义流程，用户实现细节）
- 数据处理管道（读取→解析→处理→输出）
- 游戏关卡（初始化→玩法→结算）

#### 关键要点
- 基类定义 `final` 模板方法（不可重写）
- 子类实现各个 `virtual` 步骤
- **好莱坞原则**："Don't call us, we'll call you."

---

### 17. 状态模式 State

#### 意图
允许一个对象在其**内部状态改变时改变其行为**，看起来像是改变了其类。

#### 适用场景
- TCP连接状态（Listen/Established/Closed）
- 游戏角色状态（站立/行走/跳跃/攻击）
- 订单状态机（待支付/已支付/已发货/已完成）
- 文档审批流程

---

### 18. 责任链模式 Chain of Responsibility

#### 意图
将请求沿着处理者链传递，每个处理者可以选择**处理或传递给下一个**。

#### 适用场景
- Web中间件管道
- 日志级别处理
- 审批流程（组长→经理→总监→VP）
- 异常处理链

---

### 19. 迭代器模式 Iterator

#### 意图
提供一种方法**顺序访问**聚合对象中各个元素，而不暴露其内部表示。

#### 说明
C++ STL已经完美实现了迭代器模式。`begin()`, `end()` 配合范围for循环就是迭代器模式的典范。

---

### 20. 中介者模式 Mediator

#### 意图
用一个中介对象来**封装一系列对象之间的交互**，使各对象不需要显式地相互引用。

#### 适用场景
- 聊天室（用户不直接通信，通过聊天室转发）
- 航空管制（飞机不直接通信，通过塔台协调）
- UI组件协调

---

### 21. 备忘录模式 Memento

#### 意图
在不破坏封装的前提下，**捕获和保存**对象的内部状态，以便以后**恢复**。

#### 适用场景
- 文本编辑器撤销
- 游戏存档/读档
- 数据库事务回滚

---

### 22. 访问者模式 Visitor

#### 意图
在不改变集合中元素的类的前提下，**定义新的操作**。将算法与其作用的对象分离。

#### 适用场景
- 编译器 AST 遍历（语法检查、代码生成、优化）
- 文档导出（同一文档 → PDF/HTML/Markdown）
- 复杂对象结构上的报表

#### 关键要点
- 使用**双重分派**（Double Dispatch）
- 增加新操作容易，增加新元素类型困难

---

### 23. 解释器模式 Interpreter

#### 意图
给定一个语言，定义它的文法的一种表示，并定义一个解释器来解释语言中的句子。

#### 适用场景
- SQL解析
- 正则表达式引擎
- 数学表达式求值
- 配置文件解析

---

## 六、现代C++设计模式进阶

### 6.1 使用 `std::variant` + `std::visit` 实现访问者（无虚函数）
```cpp
using Shape = std::variant<Circle, Rectangle, Triangle>;
auto area = std::visit([](auto& s) { return s.area(); }, shape);
```

### 6.2 使用 `std::function` 实现策略模式（无需继承体系）
```cpp
class Sorter {
    std::function<bool(int,int)> compare_;
public:
    void setStrategy(std::function<bool(int,int)> cmp) { compare_ = cmp; }
};
```

### 6.3 CRTP（奇异递归模板模式）实现静态多态
```cpp
template<typename Derived>
class Shape {
public:
    void draw() { static_cast<Derived*>(this)->drawImpl(); }
};
class Circle : public Shape<Circle> {
    void drawImpl() { /* ... */ }
};
```

### 6.4 使用智能指针管理生命周期
在所有设计模式中，裸指针应替换为 `std::unique_ptr`（独占所有权）或 `std::shared_ptr`（共享所有权）。

---

## 七、设计模式总结与选型指南

### 快速选型表

| 你的问题 | 推荐模式 |
|---------|---------|
| 全局只需要一个实例 | **Singleton** |
| 不确定要创建哪种对象 | **Factory Method / Abstract Factory** |
| 对象创建步骤很多 | **Builder** |
| 需要复制复杂对象 | **Prototype** |
| 接口不兼容 | **Adapter** |
| 多维度变化 | **Bridge** |
| 树形结构 | **Composite** |
| 动态增加功能 | **Decorator** |
| 简化子系统 | **Facade** |
| 大量相似对象 | **Flyweight** |
| 控制对象访问 | **Proxy** |
| 算法可替换 | **Strategy** |
| 事件通知 | **Observer** |
| 操作可撤销 | **Command / Memento** |
| 固定流程可变步骤 | **Template Method** |
| 状态驱动行为变化 | **State** |
| 请求经过多级处理 | **Chain of Responsibility** |
| 遍历集合 | **Iterator** |
| 减少对象间耦合 | **Mediator** |
| 在不改类的前提下加操作 | **Visitor** |

### 记忆口诀
```
创建型（5个）：单抽工建原（单例、抽象工厂、工厂方法、建造者、原型）
结构型（7个）：桥代理装适，组合享外观（桥接、代理、装饰器、适配器、组合、享元、外观）
行为型（11个）：观策模命状，中访迭备责解（观察者、策略、模板方法、命令、状态、中介者、访问者、迭代器、备忘录、责任链、解释器）
```

---

> **完整代码示例请看** `test_design_pattern1.cpp`
> 
> 每个模式都有独立的 namespace，可以在 main() 中选择性运行。
