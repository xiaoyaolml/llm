/**
 * ============================================================================
 *  C++ 设计模式完全教程 — 23种GoF设计模式代码示例
 * ============================================================================
 *  
 *  配合 test_design_pattern1.md 阅读
 *  编译: g++ -std=c++17 -o test_design_pattern1 test_design_pattern1.cpp
 *  运行: ./test_design_pattern1
 *
 *  每个设计模式都封装在独立的 namespace 中，可在 main() 中选择性运行。
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <sstream>
#include <stack>
#include <mutex>
#include <cassert>
#include <variant>
#include <numeric>

using namespace std;

// ============================================================================
// ======================== 一、创建型模式 (Creational) ========================
// ============================================================================

// ============================================================================
// 1. 单例模式 (Singleton)
// ============================================================================
// 意图：保证一个类仅有一个实例，并提供一个全局访问点。
// 场景：配置管理器、日志系统、线程池、数据库连接池等。
// 
// 关键点：
//   - 私有构造函数：禁止外部直接创建对象
//   - 删除拷贝/赋值：禁止复制
//   - 静态方法 getInstance()：全局唯一访问入口
//   - C++11 Meyers' Singleton（局部静态变量）：线程安全且最简洁
// ============================================================================
namespace SingletonPattern {

// -------------------- 方式一：Meyers' Singleton（推荐） --------------------
// C++11标准保证局部静态变量的初始化是线程安全的
class Logger {
public:
    // 全局唯一访问点
    static Logger& getInstance() {
        static Logger instance;  // C++11保证线程安全
        return instance;
    }

    void log(const string& message) {
        cout << "[LOG] " << message << endl;
    }

    // 模拟配置
    void setLevel(const string& level) { level_ = level; }
    string getLevel() const { return level_; }

    // 禁止拷贝和赋值
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    // 私有构造函数 — 禁止外部创建
    Logger() : level_("INFO") {
        cout << "  [Logger构造] 单例被创建" << endl;
    }
    ~Logger() {
        cout << "  [Logger析构] 单例被销毁" << endl;
    }

    string level_;
};

// -------------------- 方式二：线程安全的双重检查锁（了解即可） --------------------
// 在C++11之前常用的方式，现在Meyers' Singleton更好
class ConfigManager {
public:
    static ConfigManager* getInstance() {
        if (instance_ == nullptr) {
            lock_guard<mutex> lock(mutex_);
            if (instance_ == nullptr) {
                instance_ = new ConfigManager();
            }
        }
        return instance_;
    }

    void set(const string& key, const string& value) { config_[key] = value; }
    string get(const string& key) const {
        auto it = config_.find(key);
        return it != config_.end() ? it->second : "";
    }

private:
    ConfigManager() = default;
    static ConfigManager* instance_;
    static mutex mutex_;
    map<string, string> config_;
};
ConfigManager* ConfigManager::instance_ = nullptr;
mutex ConfigManager::mutex_;

void demo() {
    cout << "===== 单例模式 (Singleton) Demo =====" << endl;

    // Meyers' Singleton
    Logger& logger1 = Logger::getInstance();
    Logger& logger2 = Logger::getInstance();
    
    cout << "  logger1 和 logger2 是否是同一个对象: "
         << (&logger1 == &logger2 ? "是 ✓" : "否 ✗") << endl;
    
    logger1.setLevel("DEBUG");
    cout << "  通过 logger1 设置级别为 DEBUG" << endl;
    cout << "  通过 logger2 读取级别: " << logger2.getLevel() << endl;
    
    logger1.log("这是一条日志消息");

    cout << endl;
}

} // namespace SingletonPattern


// ============================================================================
// 2. 工厂方法模式 (Factory Method)
// ============================================================================
// 意图：定义创建对象的接口，让子类决定实例化哪个类。
// 场景：日志框架、跨平台UI控件、文档类型创建等。
//
// 核心结构：
//   Product (抽象产品)  →  ConcreteProduct (具体产品)
//   Creator (抽象工厂)  →  ConcreteCreator (具体工厂)
//
// 优势：
//   - 遵循开闭原则：新增产品只需新增工厂子类
//   - 客户端代码与具体产品类解耦
// ============================================================================
namespace FactoryMethodPattern {

// ---- 抽象产品：运输工具 ----
class Transport {
public:
    virtual ~Transport() = default;
    virtual void deliver() const = 0;
    virtual string getName() const = 0;
};

// ---- 具体产品 ----
class Truck : public Transport {
public:
    void deliver() const override {
        cout << "  🚛 卡车：通过陆路运输货物" << endl;
    }
    string getName() const override { return "卡车"; }
};

class Ship : public Transport {
public:
    void deliver() const override {
        cout << "  🚢 轮船：通过海路运输货物" << endl;
    }
    string getName() const override { return "轮船"; }
};

class Airplane : public Transport {
public:
    void deliver() const override {
        cout << "  ✈️  飞机：通过空运运输货物" << endl;
    }
    string getName() const override { return "飞机"; }
};

// ---- 抽象工厂（Creator）----
class Logistics {
public:
    virtual ~Logistics() = default;

    // 工厂方法 — 子类决定创建哪个具体产品
    virtual unique_ptr<Transport> createTransport() const = 0;

    // 业务逻辑 — 使用工厂方法创建的产品
    void planDelivery() const {
        auto transport = createTransport();
        cout << "  [物流规划] 使用 " << transport->getName() << " 进行配送:" << endl;
        transport->deliver();
    }
};

// ---- 具体工厂 ----
class RoadLogistics : public Logistics {
public:
    unique_ptr<Transport> createTransport() const override {
        return make_unique<Truck>();
    }
};

class SeaLogistics : public Logistics {
public:
    unique_ptr<Transport> createTransport() const override {
        return make_unique<Ship>();
    }
};

class AirLogistics : public Logistics {
public:
    unique_ptr<Transport> createTransport() const override {
        return make_unique<Airplane>();
    }
};

void demo() {
    cout << "===== 工厂方法模式 (Factory Method) Demo =====" << endl;

    // 客户端代码只与 Logistics 接口打交道，不知道具体产品类
    vector<unique_ptr<Logistics>> companies;
    companies.push_back(make_unique<RoadLogistics>());
    companies.push_back(make_unique<SeaLogistics>());
    companies.push_back(make_unique<AirLogistics>());

    for (const auto& company : companies) {
        company->planDelivery();
    }

    cout << endl;
}

} // namespace FactoryMethodPattern


// ============================================================================
// 3. 抽象工厂模式 (Abstract Factory)
// ============================================================================
// 意图：提供一个创建「一系列相关对象」的接口，无需指定具体类。
// 场景：跨平台UI(Windows/Mac控件)、数据库访问层、游戏不同主题等。
//
// 与工厂方法的区别：
//   工厂方法：一个工厂创建一种产品
//   抽象工厂：一个工厂创建一族相关产品（按钮+文本框+复选框...）
// ============================================================================
namespace AbstractFactoryPattern {

// ---- 抽象产品族 ----
class Button {
public:
    virtual ~Button() = default;
    virtual void render() const = 0;
};

class TextBox {
public:
    virtual ~TextBox() = default;
    virtual void render() const = 0;
};

class CheckBox {
public:
    virtual ~CheckBox() = default;
    virtual void render() const = 0;
};

// ---- Windows 风格产品 ----
class WindowsButton : public Button {
public:
    void render() const override { cout << "  [Windows风格按钮]" << endl; }
};
class WindowsTextBox : public TextBox {
public:
    void render() const override { cout << "  [Windows风格文本框]" << endl; }
};
class WindowsCheckBox : public CheckBox {
public:
    void render() const override { cout << "  [Windows风格复选框]" << endl; }
};

// ---- Mac 风格产品 ----
class MacButton : public Button {
public:
    void render() const override { cout << "  [Mac风格按钮]" << endl; }
};
class MacTextBox : public TextBox {
public:
    void render() const override { cout << "  [Mac风格文本框]" << endl; }
};
class MacCheckBox : public CheckBox {
public:
    void render() const override { cout << "  [Mac风格复选框]" << endl; }
};

// ---- 抽象工厂 ----
class UIFactory {
public:
    virtual ~UIFactory() = default;
    virtual unique_ptr<Button> createButton() const = 0;
    virtual unique_ptr<TextBox> createTextBox() const = 0;
    virtual unique_ptr<CheckBox> createCheckBox() const = 0;
};

// ---- 具体工厂 ----
class WindowsUIFactory : public UIFactory {
public:
    unique_ptr<Button> createButton() const override { return make_unique<WindowsButton>(); }
    unique_ptr<TextBox> createTextBox() const override { return make_unique<WindowsTextBox>(); }
    unique_ptr<CheckBox> createCheckBox() const override { return make_unique<WindowsCheckBox>(); }
};

class MacUIFactory : public UIFactory {
public:
    unique_ptr<Button> createButton() const override { return make_unique<MacButton>(); }
    unique_ptr<TextBox> createTextBox() const override { return make_unique<MacTextBox>(); }
    unique_ptr<CheckBox> createCheckBox() const override { return make_unique<MacCheckBox>(); }
};

// 客户端代码 — 完全不知道具体产品类
void renderUI(const UIFactory& factory) {
    auto button = factory.createButton();
    auto textBox = factory.createTextBox();
    auto checkBox = factory.createCheckBox();
    
    button->render();
    textBox->render();
    checkBox->render();
}

void demo() {
    cout << "===== 抽象工厂模式 (Abstract Factory) Demo =====" << endl;

    cout << "  --- Windows 平台 ---" << endl;
    WindowsUIFactory winFactory;
    renderUI(winFactory);

    cout << "  --- Mac 平台 ---" << endl;
    MacUIFactory macFactory;
    renderUI(macFactory);

    cout << endl;
}

} // namespace AbstractFactoryPattern


// ============================================================================
// 4. 建造者模式 (Builder)
// ============================================================================
// 意图：将复杂对象的构建与表示分离，相同的构建过程可以创建不同的表示。
// 场景：构建复杂配置对象、SQL查询、HTTP请求、游戏角色创建等。
//
// 关键：
//   - 链式调用（Fluent Interface）让API更优雅
//   - Director（可选）定义构建顺序
//   - 适合参数很多且可选的场景
// ============================================================================
namespace BuilderPattern {

// ---- 产品：一台电脑 ----
class Computer {
public:
    void showSpecs() const {
        cout << "  电脑配置:" << endl;
        cout << "    CPU: " << cpu_ << endl;
        cout << "    RAM: " << ram_ << endl;
        cout << "    Storage: " << storage_ << endl;
        cout << "    GPU: " << gpu_ << endl;
        if (hasWifi_) cout << "    WiFi: 有" << endl;
        if (hasBluetooth_) cout << "    蓝牙: 有" << endl;
    }

private:
    string cpu_;
    string ram_;
    string storage_;
    string gpu_;
    bool hasWifi_ = false;
    bool hasBluetooth_ = false;

    friend class ComputerBuilder;  // 让Builder能访问私有成员
};

// ---- 建造者 ----
class ComputerBuilder {
public:
    ComputerBuilder() : computer_(make_unique<Computer>()) {}

    // 链式调用：每个方法返回 *this 的引用
    ComputerBuilder& setCPU(const string& cpu) {
        computer_->cpu_ = cpu;
        return *this;
    }
    ComputerBuilder& setRAM(const string& ram) {
        computer_->ram_ = ram;
        return *this;
    }
    ComputerBuilder& setStorage(const string& storage) {
        computer_->storage_ = storage;
        return *this;
    }
    ComputerBuilder& setGPU(const string& gpu) {
        computer_->gpu_ = gpu;
        return *this;
    }
    ComputerBuilder& enableWifi() {
        computer_->hasWifi_ = true;
        return *this;
    }
    ComputerBuilder& enableBluetooth() {
        computer_->hasBluetooth_ = true;
        return *this;
    }

    // 构建最终产品
    unique_ptr<Computer> build() {
        return move(computer_);
    }

private:
    unique_ptr<Computer> computer_;
};

// ---- Director（可选）：定义预设配置 ----
class ComputerDirector {
public:
    static unique_ptr<Computer> buildGamingPC() {
        return ComputerBuilder()
            .setCPU("Intel i9-13900K")
            .setRAM("64GB DDR5")
            .setStorage("2TB NVMe SSD")
            .setGPU("RTX 4090")
            .enableWifi()
            .enableBluetooth()
            .build();
    }

    static unique_ptr<Computer> buildOfficePC() {
        return ComputerBuilder()
            .setCPU("Intel i5-13400")
            .setRAM("16GB DDR4")
            .setStorage("512GB SSD")
            .setGPU("集成显卡")
            .enableWifi()
            .build();
    }
};

void demo() {
    cout << "===== 建造者模式 (Builder) Demo =====" << endl;

    // 方式1：使用Director的预设配置
    cout << "  --- 游戏电脑（Director预设）---" << endl;
    auto gamingPC = ComputerDirector::buildGamingPC();
    gamingPC->showSpecs();

    cout << "  --- 办公电脑（Director预设）---" << endl;
    auto officePC = ComputerDirector::buildOfficePC();
    officePC->showSpecs();

    // 方式2：手动链式构建
    cout << "  --- 自定义电脑（手动构建）---" << endl;
    auto customPC = ComputerBuilder()
        .setCPU("AMD Ryzen 7 7800X3D")
        .setRAM("32GB DDR5")
        .setStorage("1TB NVMe SSD")
        .setGPU("RTX 4070")
        .enableWifi()
        .enableBluetooth()
        .build();
    customPC->showSpecs();

    cout << endl;
}

} // namespace BuilderPattern


// ============================================================================
// 5. 原型模式 (Prototype)
// ============================================================================
// 意图：通过克隆（复制）已有对象来创建新对象，而不是通过构造函数。
// 场景：对象创建成本高（需要数据库查询）、运行时才知道具体类型、
//       游戏中克隆怪物/道具模板。
//
// 关键：clone() 虚函数 + 拷贝构造函数
// 注意：深拷贝 vs 浅拷贝！
// ============================================================================
namespace PrototypePattern {

// ---- 抽象原型 ----
class Monster {
public:
    virtual ~Monster() = default;
    virtual unique_ptr<Monster> clone() const = 0;  // 克隆方法
    virtual void showInfo() const = 0;

    void setPosition(int x, int y) { x_ = x; y_ = y; }

protected:
    string name_;
    int health_;
    int attack_;
    int x_ = 0, y_ = 0;
};

// ---- 具体原型 ----
class Goblin : public Monster {
public:
    Goblin(const string& name, int health, int attack) {
        name_ = name;
        health_ = health;
        attack_ = attack;
    }

    // 克隆自己
    unique_ptr<Monster> clone() const override {
        return make_unique<Goblin>(*this);  // 调用拷贝构造函数
    }

    void showInfo() const override {
        cout << "  哥布林[" << name_ << "] HP:" << health_ 
             << " ATK:" << attack_ << " 位置:(" << x_ << "," << y_ << ")" << endl;
    }
};

class Dragon : public Monster {
public:
    Dragon(const string& name, int health, int attack, const string& element) 
        : element_(element) {
        name_ = name;
        health_ = health;
        attack_ = attack;
    }

    unique_ptr<Monster> clone() const override {
        return make_unique<Dragon>(*this);
    }

    void showInfo() const override {
        cout << "  巨龙[" << name_ << "] HP:" << health_ 
             << " ATK:" << attack_ << " 属性:" << element_
             << " 位置:(" << x_ << "," << y_ << ")" << endl;
    }

private:
    string element_;
};

// ---- 怪物注册表（原型管理器）----
class MonsterRegistry {
public:
    void registerPrototype(const string& key, unique_ptr<Monster> prototype) {
        prototypes_[key] = move(prototype);
    }

    unique_ptr<Monster> create(const string& key) const {
        auto it = prototypes_.find(key);
        if (it != prototypes_.end()) {
            return it->second->clone();  // 克隆原型
        }
        return nullptr;
    }

private:
    map<string, unique_ptr<Monster>> prototypes_;
};

void demo() {
    cout << "===== 原型模式 (Prototype) Demo =====" << endl;

    // 创建原型注册表
    MonsterRegistry registry;
    registry.registerPrototype("goblin_warrior", 
        make_unique<Goblin>("哥布林战士", 100, 15));
    registry.registerPrototype("fire_dragon", 
        make_unique<Dragon>("炎龙", 5000, 200, "火"));

    // 通过克隆创建实例（而非直接构造）
    auto goblin1 = registry.create("goblin_warrior");
    goblin1->setPosition(10, 20);
    
    auto goblin2 = registry.create("goblin_warrior");
    goblin2->setPosition(30, 40);
    
    auto dragon1 = registry.create("fire_dragon");
    dragon1->setPosition(100, 200);

    goblin1->showInfo();
    goblin2->showInfo();
    dragon1->showInfo();

    cout << "  (三个对象都是从原型克隆而来，互相独立)" << endl;
    cout << endl;
}

} // namespace PrototypePattern


// ============================================================================
// ======================== 二、结构型模式 (Structural) ========================
// ============================================================================

// ============================================================================
// 6. 适配器模式 (Adapter)
// ============================================================================
// 意图：将一个类的接口转换成客户端期望的另一个接口。
// 类比：电源适配器（220V → 110V）
// 场景：集成第三方库、旧系统迁移、统一不同数据源接口。
//
// 两种方式：
//   - 类适配器（多重继承，耦合度高）
//   - 对象适配器（组合，推荐✓）
// ============================================================================
namespace AdapterPattern {

// ---- 目标接口：客户端期望的接口 ----
class MediaPlayer {
public:
    virtual ~MediaPlayer() = default;
    virtual void play(const string& filename) = 0;
};

// ---- 已有的"不兼容"类（第三方库） ----
class VLCPlayer {
public:
    void playVLC(const string& filename) {
        cout << "  [VLC引擎] 播放VLC格式: " << filename << endl;
    }
};

class FFmpegPlayer {
public:
    void playFFmpeg(const string& filename) {
        cout << "  [FFmpeg引擎] 播放FFmpeg格式: " << filename << endl;
    }
};

// ---- 适配器：将不兼容接口转换为目标接口 ----
class VLCAdapter : public MediaPlayer {
public:
    VLCAdapter() : vlcPlayer_(make_unique<VLCPlayer>()) {}

    void play(const string& filename) override {
        // 把 play() 调用转发给 VLC 的 playVLC()
        vlcPlayer_->playVLC(filename);
    }

private:
    unique_ptr<VLCPlayer> vlcPlayer_;  // 组合而非继承
};

class FFmpegAdapter : public MediaPlayer {
public:
    FFmpegAdapter() : ffmpegPlayer_(make_unique<FFmpegPlayer>()) {}

    void play(const string& filename) override {
        ffmpegPlayer_->playFFmpeg(filename);
    }

private:
    unique_ptr<FFmpegPlayer> ffmpegPlayer_;
};

// ---- 高级播放器：根据格式选择适配器 ----
class AudioPlayer : public MediaPlayer {
public:
    void play(const string& filename) override {
        string ext = filename.substr(filename.find_last_of('.') + 1);
        
        if (ext == "mp3") {
            cout << "  [内置引擎] 播放MP3格式: " << filename << endl;
        } else if (ext == "vlc") {
            VLCAdapter adapter;
            adapter.play(filename);
        } else if (ext == "mkv" || ext == "mp4") {
            FFmpegAdapter adapter;
            adapter.play(filename);
        } else {
            cout << "  不支持的格式: " << ext << endl;
        }
    }
};

void demo() {
    cout << "===== 适配器模式 (Adapter) Demo =====" << endl;

    AudioPlayer player;
    player.play("song.mp3");
    player.play("movie.vlc");
    player.play("video.mkv");
    player.play("clip.mp4");
    player.play("unknown.avi");

    cout << endl;
}

} // namespace AdapterPattern


// ============================================================================
// 7. 桥接模式 (Bridge)
// ============================================================================
// 意图：将抽象部分与实现部分分离，使它们可以独立变化。
// 类比：遥控器（抽象） ←桥→ 电视机（实现）
// 场景：多维度变化（形状×颜色，设备×渲染方式），避免类爆炸。
//
// 关键：抽象层持有实现层的指针（这就是"桥"）
// ============================================================================
namespace BridgePattern {

// ---- 实现层接口 ----
class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void renderCircle(float x, float y, float radius) const = 0;
    virtual void renderRectangle(float x, float y, float w, float h) const = 0;
};

// ---- 具体实现 ----
class OpenGLRenderer : public Renderer {
public:
    void renderCircle(float x, float y, float radius) const override {
        cout << "  [OpenGL] 绘制圆形 at (" << x << "," << y << ") r=" << radius << endl;
    }
    void renderRectangle(float x, float y, float w, float h) const override {
        cout << "  [OpenGL] 绘制矩形 at (" << x << "," << y << ") " << w << "x" << h << endl;
    }
};

class VulkanRenderer : public Renderer {
public:
    void renderCircle(float x, float y, float radius) const override {
        cout << "  [Vulkan] 绘制圆形 at (" << x << "," << y << ") r=" << radius << endl;
    }
    void renderRectangle(float x, float y, float w, float h) const override {
        cout << "  [Vulkan] 绘制矩形 at (" << x << "," << y << ") " << w << "x" << h << endl;
    }
};

// ---- 抽象层 ----
class Shape {
public:
    Shape(shared_ptr<Renderer> renderer) : renderer_(renderer) {}
    virtual ~Shape() = default;
    virtual void draw() const = 0;

protected:
    shared_ptr<Renderer> renderer_;  // "桥" — 指向实现层
};

// ---- 扩展抽象 ----
class Circle : public Shape {
public:
    Circle(float x, float y, float radius, shared_ptr<Renderer> renderer)
        : Shape(renderer), x_(x), y_(y), radius_(radius) {}

    void draw() const override {
        renderer_->renderCircle(x_, y_, radius_);
    }

private:
    float x_, y_, radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(float x, float y, float w, float h, shared_ptr<Renderer> renderer)
        : Shape(renderer), x_(x), y_(y), w_(w), h_(h) {}

    void draw() const override {
        renderer_->renderRectangle(x_, y_, w_, h_);
    }

private:
    float x_, y_, w_, h_;
};

void demo() {
    cout << "===== 桥接模式 (Bridge) Demo =====" << endl;

    auto opengl = make_shared<OpenGLRenderer>();
    auto vulkan = make_shared<VulkanRenderer>();

    // 相同的形状，不同的渲染器
    Circle circle1(10, 20, 5, opengl);
    Circle circle2(10, 20, 5, vulkan);

    Rectangle rect1(0, 0, 100, 50, opengl);
    Rectangle rect2(0, 0, 100, 50, vulkan);

    circle1.draw();
    circle2.draw();
    rect1.draw();
    rect2.draw();

    // 形状和渲染器可以独立扩展，避免类爆炸！
    // 如果用继承：OpenGLCircle, VulkanCircle, OpenGLRect, VulkanRect...
    // 用桥接：2个Shape + 2个Renderer = 4种组合

    cout << endl;
}

} // namespace BridgePattern


// ============================================================================
// 8. 组合模式 (Composite)
// ============================================================================
// 意图：将对象组合成树形结构表示"部分-整体"层次。
//       客户端对单个对象和组合对象的使用一致。
// 场景：文件系统、UI控件树、组织架构、菜单系统。
// ============================================================================
namespace CompositePattern {

// ---- Component：统一接口 ----
class FileSystemItem {
public:
    FileSystemItem(const string& name) : name_(name) {}
    virtual ~FileSystemItem() = default;

    virtual void display(int indent = 0) const = 0;
    virtual int getSize() const = 0;

    string getName() const { return name_; }

protected:
    string name_;
};

// ---- Leaf：文件 ----
class File : public FileSystemItem {
public:
    File(const string& name, int size) : FileSystemItem(name), size_(size) {}

    void display(int indent = 0) const override {
        cout << string(indent, ' ') << "📄 " << name_ << " (" << size_ << "KB)" << endl;
    }

    int getSize() const override { return size_; }

private:
    int size_;
};

// ---- Composite：文件夹（可以包含文件和子文件夹） ----
class Folder : public FileSystemItem {
public:
    Folder(const string& name) : FileSystemItem(name) {}

    void add(shared_ptr<FileSystemItem> item) {
        children_.push_back(item);
    }

    void display(int indent = 0) const override {
        cout << string(indent, ' ') << "📁 " << name_ 
             << " (总计:" << getSize() << "KB)" << endl;
        for (const auto& child : children_) {
            child->display(indent + 4);
        }
    }

    int getSize() const override {
        int total = 0;
        for (const auto& child : children_) {
            total += child->getSize();
        }
        return total;
    }

private:
    vector<shared_ptr<FileSystemItem>> children_;
};

void demo() {
    cout << "===== 组合模式 (Composite) Demo =====" << endl;

    // 构建文件系统
    auto root = make_shared<Folder>("root");
    auto src = make_shared<Folder>("src");
    auto docs = make_shared<Folder>("docs");

    src->add(make_shared<File>("main.cpp", 15));
    src->add(make_shared<File>("utils.cpp", 8));
    src->add(make_shared<File>("utils.h", 3));

    docs->add(make_shared<File>("README.md", 5));
    docs->add(make_shared<File>("API.md", 12));

    root->add(src);
    root->add(docs);
    root->add(make_shared<File>("Makefile", 2));

    // 统一调用 display() 和 getSize()
    // 客户端无需区分是文件还是文件夹
    root->display();

    cout << endl;
}

} // namespace CompositePattern


// ============================================================================
// 9. 装饰器模式 (Decorator)
// ============================================================================
// 意图：动态地给对象添加额外职责，比继承更灵活。
// 类比：咖啡 → +牛奶 → +糖 → +奶泡（层层装饰）
// 场景：I/O流增强、日志增强、权限检查、数据压缩/加密。
//
// 关键：
//   - Decorator 和 Component 有相同接口
//   - Decorator 持有 Component 的引用（包装）
//   - 可以嵌套多层，顺序自由
// ============================================================================
namespace DecoratorPattern {

// ---- Component：数据流接口 ----
class DataSource {
public:
    virtual ~DataSource() = default;
    virtual string read() const = 0;
    virtual void write(const string& data) = 0;
};

// ---- ConcreteComponent：基础 ----
class FileDataSource : public DataSource {
public:
    FileDataSource(const string& filename) : filename_(filename) {}

    string read() const override {
        return data_;  // 模拟从文件读取
    }

    void write(const string& data) override {
        data_ = data;
        cout << "  [文件] 写入到 " << filename_ << ": " << data << endl;
    }

private:
    string filename_;
    mutable string data_;
};

// ---- Decorator 基类 ----
class DataSourceDecorator : public DataSource {
public:
    DataSourceDecorator(unique_ptr<DataSource> source)
        : wrappee_(move(source)) {}

    string read() const override { return wrappee_->read(); }
    void write(const string& data) override { wrappee_->write(data); }

protected:
    unique_ptr<DataSource> wrappee_;
};

// ---- ConcreteDecorator：加密装饰器 ----
class EncryptionDecorator : public DataSourceDecorator {
public:
    using DataSourceDecorator::DataSourceDecorator;

    string read() const override {
        string data = wrappee_->read();
        return decrypt(data);
    }

    void write(const string& data) override {
        cout << "  [加密层] 加密数据..." << endl;
        wrappee_->write(encrypt(data));
    }

private:
    string encrypt(const string& data) const {
        string result = data;
        for (char& c : result) c = c + 3;  // 简单凯撒加密
        return result;
    }
    string decrypt(const string& data) const {
        string result = data;
        for (char& c : result) c = c - 3;
        return result;
    }
};

// ---- ConcreteDecorator：压缩装饰器 ----
class CompressionDecorator : public DataSourceDecorator {
public:
    using DataSourceDecorator::DataSourceDecorator;

    string read() const override {
        string data = wrappee_->read();
        return decompress(data);
    }

    void write(const string& data) override {
        cout << "  [压缩层] 压缩数据..." << endl;
        wrappee_->write(compress(data));
    }

private:
    string compress(const string& data) const {
        return "[compressed:" + data + "]";  // 模拟压缩
    }
    string decompress(const string& data) const {
        if (data.size() > 14) {
            return data.substr(12, data.size() - 13);  // 移除标记
        }
        return data;
    }
};

void demo() {
    cout << "===== 装饰器模式 (Decorator) Demo =====" << endl;

    // 层层包装：文件 → 加密 → 压缩
    auto source = make_unique<FileDataSource>("data.txt");
    auto encrypted = make_unique<EncryptionDecorator>(move(source));
    auto compressed = make_unique<CompressionDecorator>(move(encrypted));

    // 写入时：压缩 → 加密 → 文件
    compressed->write("Hello, Design Patterns!");

    // 读取时：文件 → 解密 → 解压
    cout << "  [读取] 解码后数据: " << compressed->read() << endl;

    cout << endl;
}

} // namespace DecoratorPattern


// ============================================================================
// 10. 外观模式 (Facade)
// ============================================================================
// 意图：为复杂子系统提供一个简单的统一接口。
// 类比：电脑开机按钮 — 一键启动CPU、内存、硬盘、显卡。
// 场景：简化复杂API、分层架构入口、第三方库封装。
// ============================================================================
namespace FacadePattern {

// ---- 复杂子系统 ----
class CPU {
public:
    void freeze() { cout << "  [CPU] 冻结处理器" << endl; }
    void jump(long position) { cout << "  [CPU] 跳转到地址 0x" << hex << position << dec << endl; }
    void execute() { cout << "  [CPU] 执行指令" << endl; }
};

class Memory {
public:
    void load(long position, const string& data) {
        cout << "  [内存] 加载数据到地址 0x" << hex << position << dec 
             << ": " << data << endl;
    }
};

class HardDrive {
public:
    string read(long lba, int size) {
        cout << "  [硬盘] 读取扇区 " << lba << ", 大小 " << size << " bytes" << endl;
        return "boot_sector_data";
    }
};

class GPU {
public:
    void initialize() { cout << "  [GPU] 初始化图形处理器" << endl; }
    void render() { cout << "  [GPU] 渲染启动画面" << endl; }
};

// ---- Facade：一键启动 ----
class ComputerFacade {
public:
    ComputerFacade() 
        : cpu_(make_unique<CPU>()), memory_(make_unique<Memory>()),
          hdd_(make_unique<HardDrive>()), gpu_(make_unique<GPU>()) {}

    void start() {
        cout << "  === 电脑启动中... ===" << endl;
        cpu_->freeze();
        string bootData = hdd_->read(0, 512);
        memory_->load(0x00, bootData);
        gpu_->initialize();
        cpu_->jump(0x00);
        cpu_->execute();
        gpu_->render();
        cout << "  === 电脑启动完成! ===" << endl;
    }

    void shutdown() {
        cout << "  === 电脑关机中... ===" << endl;
        cout << "  [保存状态] → [停止进程] → [断电]" << endl;
        cout << "  === 电脑已关机 ===" << endl;
    }

private:
    unique_ptr<CPU> cpu_;
    unique_ptr<Memory> memory_;
    unique_ptr<HardDrive> hdd_;
    unique_ptr<GPU> gpu_;
};

void demo() {
    cout << "===== 外观模式 (Facade) Demo =====" << endl;

    ComputerFacade computer;
    // 用户只需一行代码，不需要知道内部复杂过程
    computer.start();
    computer.shutdown();

    cout << endl;
}

} // namespace FacadePattern


// ============================================================================
// 11. 享元模式 (Flyweight)
// ============================================================================
// 意图：通过共享来高效支持大量细粒度对象。
// 场景：文本编辑器字符对象、游戏中大量树木/粒子、图标缓存。
//
// 关键概念：
//   - 内在状态（Intrinsic）：可共享，存在享元内部（如：树的类型、纹理）
//   - 外在状态（Extrinsic）：不可共享，由外部传入（如：树的位置、大小）
// ============================================================================
namespace FlyweightPattern {

// ---- 享元：树的类型（可共享的内在状态） ----
class TreeType {
public:
    TreeType(const string& name, const string& color, const string& texture)
        : name_(name), color_(color), texture_(texture) {
        cout << "  [创建TreeType] " << name_ << " (这个操作开销很大)" << endl;
    }

    void draw(int x, int y) const {
        cout << "  绘制 " << name_ << "(" << color_ << ") at (" << x << "," << y << ")" << endl;
    }

private:
    string name_;     // 内在状态
    string color_;    // 内在状态
    string texture_;  // 内在状态（假设是大纹理数据）
};

// ---- 享元工厂 ----
class TreeFactory {
public:
    static shared_ptr<TreeType> getTreeType(const string& name, 
                                             const string& color,
                                             const string& texture) {
        string key = name + "_" + color + "_" + texture;
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            cache_[key] = make_shared<TreeType>(name, color, texture);
        }
        return cache_[key];
    }

    static size_t getCacheSize() { return cache_.size(); }

private:
    static unordered_map<string, shared_ptr<TreeType>> cache_;
};
unordered_map<string, shared_ptr<TreeType>> TreeFactory::cache_;

// ---- 具体的树（包含外在状态） ----
struct Tree {
    int x, y;                        // 外在状态：位置
    shared_ptr<TreeType> type;       // 内在状态：共享的类型

    void draw() const { type->draw(x, y); }
};

// ---- 森林 ----
class Forest {
public:
    void plantTree(int x, int y, const string& name, 
                   const string& color, const string& texture) {
        auto type = TreeFactory::getTreeType(name, color, texture);
        trees_.push_back({x, y, type});
    }

    void draw() const {
        for (const auto& tree : trees_) tree.draw();
    }

    size_t getTreeCount() const { return trees_.size(); }

private:
    vector<Tree> trees_;
};

void demo() {
    cout << "===== 享元模式 (Flyweight) Demo =====" << endl;

    Forest forest;
    // 种植1000棵树，但只有3种TreeType
    for (int i = 0; i < 5; i++) {
        forest.plantTree(i * 10, i * 5, "松树", "绿色", "pine_texture.png");
        forest.plantTree(i * 10 + 5, i * 5 + 2, "橡树", "深绿", "oak_texture.png");
        forest.plantTree(i * 10 + 3, i * 5 + 4, "桦树", "黄绿", "birch_texture.png");
    }

    cout << "  树的总数: " << forest.getTreeCount() << endl;
    cout << "  TreeType缓存数(实际共享对象): " << TreeFactory::getCacheSize() << endl;
    cout << "  (15棵树只创建了3个TreeType对象，大幅节省内存!)" << endl;

    cout << endl;
}

} // namespace FlyweightPattern


// ============================================================================
// 12. 代理模式 (Proxy)
// ============================================================================
// 意图：为另一个对象提供代理或占位符，以控制对原对象的访问。
// 类型：虚拟代理（延迟加载）、保护代理（权限控制）、缓存代理等。
// ============================================================================
namespace ProxyPattern {

// ---- 接口 ----
class Image {
public:
    virtual ~Image() = default;
    virtual void display() const = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};

// ---- 真实对象（加载开销大） ----
class HighResImage : public Image {
public:
    HighResImage(const string& filename) : filename_(filename) {
        loadFromDisk();
    }

    void display() const override {
        cout << "  [显示] 高清图片: " << filename_ 
             << " (" << width_ << "x" << height_ << ")" << endl;
    }

    int getWidth() const override { return width_; }
    int getHeight() const override { return height_; }

private:
    void loadFromDisk() {
        cout << "  [加载] 从磁盘加载高清图片 " << filename_ << " (耗时操作!)" << endl;
        width_ = 3840;
        height_ = 2160;
    }

    string filename_;
    int width_ = 0, height_ = 0;
};

// ---- 虚拟代理：延迟加载 ----
class LazyImageProxy : public Image {
public:
    LazyImageProxy(const string& filename) : filename_(filename) {}

    void display() const override {
        if (!realImage_) {
            cout << "  [代理] 首次访问，开始加载..." << endl;
            realImage_ = make_unique<HighResImage>(filename_);
        }
        realImage_->display();
    }

    int getWidth() const override {
        ensureLoaded();
        return realImage_->getWidth();
    }

    int getHeight() const override {
        ensureLoaded();
        return realImage_->getHeight();
    }

private:
    void ensureLoaded() const {
        if (!realImage_) {
            realImage_ = make_unique<HighResImage>(filename_);
        }
    }

    string filename_;
    mutable unique_ptr<HighResImage> realImage_;
};

// ---- 保护代理：权限控制 ----
class ProtectedImageProxy : public Image {
public:
    ProtectedImageProxy(const string& filename, const string& userRole)
        : proxy_(filename), userRole_(userRole) {}

    void display() const override {
        if (userRole_ == "admin" || userRole_ == "viewer") {
            proxy_.display();
        } else {
            cout << "  [权限拒绝] 用户角色 '" << userRole_ << "' 无权查看图片" << endl;
        }
    }

    int getWidth() const override { return proxy_.getWidth(); }
    int getHeight() const override { return proxy_.getHeight(); }

private:
    LazyImageProxy proxy_;
    string userRole_;
};

void demo() {
    cout << "===== 代理模式 (Proxy) Demo =====" << endl;

    // 虚拟代理：延迟加载
    cout << "  --- 虚拟代理（延迟加载）---" << endl;
    LazyImageProxy img("wallpaper_4k.png");
    cout << "  (图片代理已创建，但还未加载)" << endl;
    img.display();  // 第一次访问才加载
    img.display();  // 第二次直接使用，不再加载

    // 保护代理：权限控制
    cout << "  --- 保护代理（权限控制）---" << endl;
    ProtectedImageProxy adminImg("secret.png", "admin");
    ProtectedImageProxy guestImg("secret.png", "guest");
    adminImg.display();  // 允许
    guestImg.display();  // 拒绝

    cout << endl;
}

} // namespace ProxyPattern


// ============================================================================
// ======================== 三、行为型模式 (Behavioral) ========================
// ============================================================================

// ============================================================================
// 13. 策略模式 (Strategy)
// ============================================================================
// 意图：定义一系列算法，封装每个算法，使它们可以互换。
// 类比：导航APP选路线 — 最快/最短/避开高速
// 场景：排序算法选择、支付方式、压缩算法、路径规划。
//
// 现代C++可以用 std::function 替代继承体系。
// ============================================================================
namespace StrategyPattern {

// ---- 方式一：经典继承实现 ----
class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(vector<int>& data) const = 0;
    virtual string getName() const = 0;
};

class BubbleSort : public SortStrategy {
public:
    void sort(vector<int>& data) const override {
        for (size_t i = 0; i < data.size(); i++)
            for (size_t j = 0; j < data.size() - i - 1; j++)
                if (data[j] > data[j+1]) swap(data[j], data[j+1]);
    }
    string getName() const override { return "冒泡排序"; }
};

class SelectionSort : public SortStrategy {
public:
    void sort(vector<int>& data) const override {
        for (size_t i = 0; i < data.size(); i++) {
            size_t minIdx = i;
            for (size_t j = i + 1; j < data.size(); j++)
                if (data[j] < data[minIdx]) minIdx = j;
            swap(data[i], data[minIdx]);
        }
    }
    string getName() const override { return "选择排序"; }
};

class QuickSort : public SortStrategy {
    void qsort(vector<int>& data, int lo, int hi) const {
        if (lo >= hi) return;
        int pivot = data[hi], i = lo;
        for (int j = lo; j < hi; j++)
            if (data[j] < pivot) swap(data[i++], data[j]);
        swap(data[i], data[hi]);
        qsort(data, lo, i - 1);
        qsort(data, i + 1, hi);
    }
public:
    void sort(vector<int>& data) const override {
        if (!data.empty()) qsort(data, 0, data.size() - 1);
    }
    string getName() const override { return "快速排序"; }
};

// Context
class Sorter {
public:
    void setStrategy(unique_ptr<SortStrategy> strategy) {
        strategy_ = move(strategy);
    }

    void sort(vector<int>& data) {
        if (strategy_) {
            cout << "  使用策略: " << strategy_->getName() << endl;
            strategy_->sort(data);
        }
    }

private:
    unique_ptr<SortStrategy> strategy_;
};

// ---- 方式二：现代C++ std::function 实现（更简洁） ----
class ModernSorter {
public:
    using SortFunc = function<void(vector<int>&)>;

    void setStrategy(SortFunc func, const string& name) {
        sortFunc_ = func;
        name_ = name;
    }

    void sort(vector<int>& data) {
        cout << "  [Modern] 使用策略: " << name_ << endl;
        sortFunc_(data);
    }

private:
    SortFunc sortFunc_;
    string name_;
};

void printVec(const vector<int>& v) {
    cout << "  [";
    for (size_t i = 0; i < v.size(); i++) {
        cout << v[i] << (i < v.size()-1 ? ", " : "");
    }
    cout << "]" << endl;
}

void demo() {
    cout << "===== 策略模式 (Strategy) Demo =====" << endl;

    vector<int> data = {5, 2, 8, 1, 9, 3, 7, 4, 6};

    // 经典方式
    Sorter sorter;

    auto data1 = data;
    sorter.setStrategy(make_unique<BubbleSort>());
    sorter.sort(data1);
    printVec(data1);

    auto data2 = data;
    sorter.setStrategy(make_unique<QuickSort>());  // 运行时切换策略
    sorter.sort(data2);
    printVec(data2);

    // 现代方式：lambda
    ModernSorter modernSorter;
    auto data3 = data;
    modernSorter.setStrategy([](vector<int>& d) {
        std::sort(d.begin(), d.end());
    }, "std::sort (lambda)");
    modernSorter.sort(data3);
    printVec(data3);

    cout << endl;
}

} // namespace StrategyPattern


// ============================================================================
// 14. 观察者模式 (Observer)
// ============================================================================
// 意图：定义一对多依赖关系，当被观察对象状态变化时，自动通知所有依赖者。
// 类比：微信公众号发文 → 所有粉丝收到推送
// 场景：事件系统、GUI事件、MVC数据绑定、消息队列。
// ============================================================================
namespace ObserverPattern {

// ---- 观察者接口 ----
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const string& event, const string& data) = 0;
    virtual string getName() const = 0;
};

// ---- 被观察者（Subject）----
class EventManager {
public:
    void subscribe(const string& event, shared_ptr<Observer> observer) {
        listeners_[event].push_back(observer);
    }

    void unsubscribe(const string& event, shared_ptr<Observer> observer) {
        auto& list = listeners_[event];
        list.erase(
            remove_if(list.begin(), list.end(),
                [&](const weak_ptr<Observer>& wp) {
                    auto sp = wp.lock();
                    return !sp || sp == observer;
                }),
            list.end()
        );
    }

    void notify(const string& event, const string& data) {
        if (listeners_.find(event) != listeners_.end()) {
            for (auto& wp : listeners_[event]) {
                if (auto sp = wp.lock()) {
                    sp->update(event, data);
                }
            }
        }
    }

private:
    map<string, vector<weak_ptr<Observer>>> listeners_;
};

// ---- 被观察的具体类：编辑器 ----
class TextEditor {
public:
    EventManager events;

    void openFile(const string& filename) {
        filename_ = filename;
        cout << "  [编辑器] 打开文件: " << filename << endl;
        events.notify("open", filename);
    }

    void saveFile() {
        cout << "  [编辑器] 保存文件: " << filename_ << endl;
        events.notify("save", filename_);
    }

    void modifyFile(const string& content) {
        cout << "  [编辑器] 修改内容: " << content << endl;
        events.notify("modify", content);
    }

private:
    string filename_;
};

// ---- 具体观察者 ----
class LoggingObserver : public Observer {
public:
    void update(const string& event, const string& data) override {
        cout << "    [日志] 事件=" << event << ", 数据=" << data << endl;
    }
    string getName() const override { return "LoggingObserver"; }
};

class AutoSaveObserver : public Observer {
public:
    void update(const string& event, const string& data) override {
        if (event == "modify") {
            cout << "    [自动保存] 检测到修改，自动保存中..." << endl;
        }
    }
    string getName() const override { return "AutoSaveObserver"; }
};

class NotificationObserver : public Observer {
public:
    void update(const string& event, const string& data) override {
        cout << "    [通知] 📢 " << event << ": " << data << endl;
    }
    string getName() const override { return "NotificationObserver"; }
};

void demo() {
    cout << "===== 观察者模式 (Observer) Demo =====" << endl;

    TextEditor editor;

    auto logger = make_shared<LoggingObserver>();
    auto autoSave = make_shared<AutoSaveObserver>();
    auto notifier = make_shared<NotificationObserver>();

    // 订阅事件
    editor.events.subscribe("open", logger);
    editor.events.subscribe("save", logger);
    editor.events.subscribe("modify", logger);
    editor.events.subscribe("modify", autoSave);
    editor.events.subscribe("save", notifier);

    // 触发事件 — 所有订阅者自动收到通知
    editor.openFile("design_pattern.cpp");
    editor.modifyFile("added Observer pattern");
    editor.saveFile();

    cout << endl;
}

} // namespace ObserverPattern


// ============================================================================
// 15. 命令模式 (Command)
// ============================================================================
// 意图：将请求封装成对象，支持撤销/重做、命令队列等。
// 场景：文本编辑器(Ctrl+Z)、任务队列、宏命令、事务系统。
// ============================================================================
namespace CommandPattern {

// ---- Receiver：文本编辑器 ----
class TextDocument {
public:
    void insertText(size_t pos, const string& text) {
        if (pos > content_.size()) pos = content_.size();
        content_.insert(pos, text);
    }

    void deleteText(size_t pos, size_t length) {
        if (pos < content_.size()) {
            content_.erase(pos, length);
        }
    }

    string getContent() const { return content_; }
    size_t getLength() const { return content_.size(); }

private:
    string content_;
};

// ---- Command 接口 ----
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual string describe() const = 0;
};

// ---- 具体命令：插入文本 ----
class InsertCommand : public Command {
public:
    InsertCommand(TextDocument& doc, size_t pos, const string& text)
        : doc_(doc), pos_(pos), text_(text) {}

    void execute() override {
        doc_.insertText(pos_, text_);
    }

    void undo() override {
        doc_.deleteText(pos_, text_.size());
    }

    string describe() const override {
        return "插入 \"" + text_ + "\" at " + to_string(pos_);
    }

private:
    TextDocument& doc_;
    size_t pos_;
    string text_;
};

// ---- 具体命令：删除文本 ----
class DeleteCommand : public Command {
public:
    DeleteCommand(TextDocument& doc, size_t pos, size_t length)
        : doc_(doc), pos_(pos), length_(length) {}

    void execute() override {
        deletedText_ = doc_.getContent().substr(pos_, length_);
        doc_.deleteText(pos_, length_);
    }

    void undo() override {
        doc_.insertText(pos_, deletedText_);
    }

    string describe() const override {
        return "删除 " + to_string(length_) + " 字符 at " + to_string(pos_);
    }

private:
    TextDocument& doc_;
    size_t pos_;
    size_t length_;
    string deletedText_;  // 保存删除的文本以便撤销
};

// ---- Invoker：命令历史管理 ----
class CommandHistory {
public:
    void executeCommand(unique_ptr<Command> cmd) {
        cout << "  [执行] " << cmd->describe() << endl;
        cmd->execute();
        undoStack_.push(move(cmd));
        // 执行新命令后清空重做栈
        while (!redoStack_.empty()) redoStack_.pop();
    }

    void undo() {
        if (undoStack_.empty()) {
            cout << "  [撤销] 没有可撤销的操作" << endl;
            return;
        }
        auto cmd = move(undoStack_.top());
        undoStack_.pop();
        cout << "  [撤销] " << cmd->describe() << endl;
        cmd->undo();
        redoStack_.push(move(cmd));
    }

    void redo() {
        if (redoStack_.empty()) {
            cout << "  [重做] 没有可重做的操作" << endl;
            return;
        }
        auto cmd = move(redoStack_.top());
        redoStack_.pop();
        cout << "  [重做] " << cmd->describe() << endl;
        cmd->execute();
        undoStack_.push(move(cmd));
    }

private:
    stack<unique_ptr<Command>> undoStack_;
    stack<unique_ptr<Command>> redoStack_;
};

void demo() {
    cout << "===== 命令模式 (Command) Demo =====" << endl;

    TextDocument doc;
    CommandHistory history;

    // 执行命令
    history.executeCommand(make_unique<InsertCommand>(doc, 0, "Hello"));
    cout << "    内容: \"" << doc.getContent() << "\"" << endl;

    history.executeCommand(make_unique<InsertCommand>(doc, 5, " World"));
    cout << "    内容: \"" << doc.getContent() << "\"" << endl;

    history.executeCommand(make_unique<InsertCommand>(doc, 11, "!"));
    cout << "    内容: \"" << doc.getContent() << "\"" << endl;

    // 撤销
    history.undo();
    cout << "    内容: \"" << doc.getContent() << "\"" << endl;

    history.undo();
    cout << "    内容: \"" << doc.getContent() << "\"" << endl;

    // 重做
    history.redo();
    cout << "    内容: \"" << doc.getContent() << "\"" << endl;

    cout << endl;
}

} // namespace CommandPattern


// ============================================================================
// 16. 模板方法模式 (Template Method)
// ============================================================================
// 意图：定义算法骨架，将某些步骤延迟到子类实现。
// 场景：框架设计、数据处理管道、游戏关卡流程。
//
// 关键：
//   - 基类定义 final 模板方法（不可重写）
//   - 子类实现各个 virtual 步骤
//   - "好莱坞原则"：Don't call us, we'll call you.
// ============================================================================
namespace TemplateMethodPattern {

// ---- 抽象基类：数据挖掘流程 ----
class DataMiner {
public:
    virtual ~DataMiner() = default;

    // 模板方法：定义算法骨架（final 禁止子类重写）
    void mine(const string& path) {
        cout << "  === 数据挖掘开始 ===" << endl;
        string rawData = openFile(path);
        string data = extractData(rawData);
        string analysis = analyzeData(data);
        generateReport(analysis);
        // hook方法：可选步骤
        if (shouldSendReport()) {
            sendReport(analysis);
        }
        cout << "  === 数据挖掘完成 ===" << endl;
    }

protected:
    // 必须由子类实现的步骤
    virtual string openFile(const string& path) = 0;
    virtual string extractData(const string& rawData) = 0;

    // 有默认实现但子类可覆盖的步骤
    virtual string analyzeData(const string& data) {
        cout << "  [通用分析] 对数据进行基础统计..." << endl;
        return "analysis_result";
    }

    virtual void generateReport(const string& analysis) {
        cout << "  [报告] 生成标准报告" << endl;
    }

    // Hook方法：子类可以选择性覆盖
    virtual bool shouldSendReport() { return false; }
    virtual void sendReport(const string& analysis) {
        cout << "  [发送] 发送报告到邮箱" << endl;
    }
};

// ---- 具体子类：CSV数据挖掘 ----
class CSVDataMiner : public DataMiner {
protected:
    string openFile(const string& path) override {
        cout << "  [CSV] 打开CSV文件: " << path << endl;
        return "csv_raw_data";
    }
    string extractData(const string& rawData) override {
        cout << "  [CSV] 按逗号分隔解析数据" << endl;
        return "csv_parsed_data";
    }
};

// ---- 具体子类：JSON数据挖掘 ----
class JSONDataMiner : public DataMiner {
protected:
    string openFile(const string& path) override {
        cout << "  [JSON] 打开JSON文件: " << path << endl;
        return "json_raw_data";
    }
    string extractData(const string& rawData) override {
        cout << "  [JSON] 解析JSON结构" << endl;
        return "json_parsed_data";
    }
    string analyzeData(const string& data) override {
        cout << "  [JSON] 深度分析嵌套JSON数据..." << endl;
        return "deep_analysis";
    }
    bool shouldSendReport() override { return true; }  // 覆盖hook
};

void demo() {
    cout << "===== 模板方法模式 (Template Method) Demo =====" << endl;

    cout << "  --- CSV 数据挖掘 ---" << endl;
    CSVDataMiner csvMiner;
    csvMiner.mine("data.csv");

    cout << "  --- JSON 数据挖掘 ---" << endl;
    JSONDataMiner jsonMiner;
    jsonMiner.mine("data.json");

    cout << endl;
}

} // namespace TemplateMethodPattern


// ============================================================================
// 17. 状态模式 (State)
// ============================================================================
// 意图：允许对象在内部状态改变时改变其行为，看起来像改变了类。
// 场景：TCP连接状态、游戏角色状态、订单状态机、文档审批流程。
// 
// 本质：用多态取代大量的 if-else/switch 状态判断。
// ============================================================================
namespace StatePattern {

class MediaPlayer;  // 前向声明

// ---- 状态接口 ----
class PlayerState {
public:
    virtual ~PlayerState() = default;
    virtual void play(MediaPlayer& player) = 0;
    virtual void pause(MediaPlayer& player) = 0;
    virtual void stop(MediaPlayer& player) = 0;
    virtual string getStateName() const = 0;
};

// ---- Context：媒体播放器 ----
class MediaPlayer {
public:
    MediaPlayer();

    void setState(unique_ptr<PlayerState> state) {
        cout << "  [状态转换] " << state_->getStateName() 
             << " → " << state->getStateName() << endl;
        state_ = move(state);
    }

    void play() { state_->play(*this); }
    void pause() { state_->pause(*this); }
    void stop() { state_->stop(*this); }
    string getStateName() const { return state_->getStateName(); }

private:
    unique_ptr<PlayerState> state_;
};

// ---- 具体状态 ----
class StoppedState : public PlayerState {
public:
    void play(MediaPlayer& player) override;
    void pause(MediaPlayer& player) override {
        cout << "  [已停止] 无法暂停" << endl;
    }
    void stop(MediaPlayer& player) override {
        cout << "  [已停止] 已经是停止状态" << endl;
    }
    string getStateName() const override { return "已停止"; }
};

class PlayingState : public PlayerState {
public:
    void play(MediaPlayer& player) override {
        cout << "  [播放中] 已经在播放了" << endl;
    }
    void pause(MediaPlayer& player) override;
    void stop(MediaPlayer& player) override;
    string getStateName() const override { return "播放中"; }
};

class PausedState : public PlayerState {
public:
    void play(MediaPlayer& player) override;
    void pause(MediaPlayer& player) override {
        cout << "  [已暂停] 已经是暂停状态" << endl;
    }
    void stop(MediaPlayer& player) override;
    string getStateName() const override { return "已暂停"; }
};

// 实现（需在所有状态类定义之后）
MediaPlayer::MediaPlayer() : state_(make_unique<StoppedState>()) {}

void StoppedState::play(MediaPlayer& player) {
    cout << "  [操作] ▶ 开始播放" << endl;
    player.setState(make_unique<PlayingState>());
}

void PlayingState::pause(MediaPlayer& player) {
    cout << "  [操作] ⏸ 暂停播放" << endl;
    player.setState(make_unique<PausedState>());
}

void PlayingState::stop(MediaPlayer& player) {
    cout << "  [操作] ⏹ 停止播放" << endl;
    player.setState(make_unique<StoppedState>());
}

void PausedState::play(MediaPlayer& player) {
    cout << "  [操作] ▶ 继续播放" << endl;
    player.setState(make_unique<PlayingState>());
}

void PausedState::stop(MediaPlayer& player) {
    cout << "  [操作] ⏹ 停止播放" << endl;
    player.setState(make_unique<StoppedState>());
}

void demo() {
    cout << "===== 状态模式 (State) Demo =====" << endl;

    MediaPlayer player;
    cout << "  当前状态: " << player.getStateName() << endl;

    player.play();   // 停止 → 播放
    player.pause();  // 播放 → 暂停
    player.play();   // 暂停 → 播放
    player.stop();   // 播放 → 停止
    player.pause();  // 停止状态无法暂停

    cout << endl;
}

} // namespace StatePattern


// ============================================================================
// 18. 责任链模式 (Chain of Responsibility)
// ============================================================================
// 意图：将请求沿着处理者链传递，每个处理者可以处理或传递给下一个。
// 场景：Web中间件、日志级别、审批流程、异常处理链。
// ============================================================================
namespace ChainOfResponsibilityPattern {

enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

// ---- Handler 接口 ----
class LogHandler {
public:
    virtual ~LogHandler() = default;

    LogHandler* setNext(unique_ptr<LogHandler> next) {
        next_ = move(next);
        return next_.get();
    }

    virtual void handle(LogLevel level, const string& message) {
        if (next_) {
            next_->handle(level, message);
        }
    }

protected:
    unique_ptr<LogHandler> next_;
};

// ---- 具体处理者 ----
class ConsoleHandler : public LogHandler {
public:
    ConsoleHandler(LogLevel minLevel) : minLevel_(minLevel) {}

    void handle(LogLevel level, const string& message) override {
        if (level >= minLevel_) {
            cout << "  [控制台][" << logLevelToString(level) << "] " << message << endl;
        }
        LogHandler::handle(level, message);  // 传递给下一个
    }

private:
    LogLevel minLevel_;
};

class FileHandler : public LogHandler {
public:
    FileHandler(LogLevel minLevel) : minLevel_(minLevel) {}

    void handle(LogLevel level, const string& message) override {
        if (level >= minLevel_) {
            cout << "  [文件写入][" << logLevelToString(level) << "] " << message << endl;
        }
        LogHandler::handle(level, message);
    }

private:
    LogLevel minLevel_;
};

class AlertHandler : public LogHandler {
public:
    void handle(LogLevel level, const string& message) override {
        if (level >= LogLevel::ERROR) {
            cout << "  [🚨 紧急告警][" << logLevelToString(level) << "] " << message << endl;
        }
        LogHandler::handle(level, message);
    }
};

void demo() {
    cout << "===== 责任链模式 (Chain of Responsibility) Demo =====" << endl;

    // 构建责任链：控制台(DEBUG+) → 文件(WARNING+) → 告警(ERROR)
    auto console = make_unique<ConsoleHandler>(LogLevel::DEBUG);
    auto file = make_unique<FileHandler>(LogLevel::WARNING);
    auto alert = make_unique<AlertHandler>();

    // 链式设置
    auto* filePtr = console->setNext(move(file));
    filePtr->setNext(move(alert));

    // 发送不同级别的日志
    cout << "  --- 发送 DEBUG ---" << endl;
    console->handle(LogLevel::DEBUG, "调试信息");
    
    cout << "  --- 发送 INFO ---" << endl;
    console->handle(LogLevel::INFO, "普通信息");
    
    cout << "  --- 发送 WARNING ---" << endl;
    console->handle(LogLevel::WARNING, "警告：内存使用率80%");
    
    cout << "  --- 发送 ERROR ---" << endl;
    console->handle(LogLevel::ERROR, "错误：服务器宕机!");

    cout << endl;
}

} // namespace ChainOfResponsibilityPattern


// ============================================================================
// 19. 迭代器模式 (Iterator)
// ============================================================================
// 意图：提供一种方法顺序访问聚合对象中的元素，不暴露内部表示。
// 说明：C++ STL 已完美实现迭代器模式。这里展示自定义迭代器。
// ============================================================================
namespace IteratorPattern {

// ---- 自定义范围类（支持 range-based for） ----
template<typename T>
class NumberRange {
public:
    NumberRange(T start, T end, T step = 1) 
        : start_(start), end_(end), step_(step) {}

    // 迭代器
    class Iterator {
    public:
        Iterator(T current, T step) : current_(current), step_(step) {}

        T operator*() const { return current_; }
        Iterator& operator++() { current_ += step_; return *this; }
        bool operator!=(const Iterator& other) const { return current_ < other.current_; }

    private:
        T current_;
        T step_;
    };

    Iterator begin() const { return Iterator(start_, step_); }
    Iterator end() const { return Iterator(end_, step_); }

private:
    T start_, end_, step_;
};

// ---- 二叉树中序迭代器 ----
struct TreeNode {
    int val;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
    TreeNode(int v) : val(v) {}
};

class InorderIterator {
public:
    InorderIterator(TreeNode* root) {
        pushLeft(root);
    }

    bool hasNext() const { return !stack_.empty(); }

    int next() {
        TreeNode* node = stack_.top();
        stack_.pop();
        pushLeft(node->right);
        return node->val;
    }

private:
    void pushLeft(TreeNode* node) {
        while (node) {
            stack_.push(node);
            node = node->left;
        }
    }
    stack<TreeNode*> stack_;
};

void demo() {
    cout << "===== 迭代器模式 (Iterator) Demo =====" << endl;

    // 自定义Range
    cout << "  NumberRange(1, 10, 2): ";
    for (auto n : NumberRange<int>(1, 10, 2)) {
        cout << n << " ";
    }
    cout << endl;

    // 二叉树中序遍历迭代器
    //       4
    //      / \
    //     2   6
    //    / \ / \
    //   1  3 5  7
    TreeNode n4(4), n2(2), n6(6), n1(1), n3(3), n5(5), n7(7);
    n4.left = &n2; n4.right = &n6;
    n2.left = &n1; n2.right = &n3;
    n6.left = &n5; n6.right = &n7;

    cout << "  二叉树中序遍历: ";
    InorderIterator it(&n4);
    while (it.hasNext()) {
        cout << it.next() << " ";
    }
    cout << endl;

    cout << endl;
}

} // namespace IteratorPattern


// ============================================================================
// 20. 中介者模式 (Mediator)
// ============================================================================
// 意图：用中介对象封装一系列对象交互，避免两两直接引用。
// 类比：航空管制：飞机不直接通信，通过塔台协调。
// 场景：聊天室、UI组件协调、事件总线。
// ============================================================================
namespace MediatorPattern {

class ChatRoom;  // 前向声明

// ---- Colleague 接口 ----
class User {
public:
    User(const string& name, ChatRoom* room) : name_(name), room_(room) {}
    virtual ~User() = default;

    string getName() const { return name_; }
    void send(const string& message);
    
    virtual void receive(const string& from, const string& message) {
        cout << "    [" << name_ << " 收到] " << from << ": " << message << endl;
    }

protected:
    string name_;
    ChatRoom* room_;
};

// ---- Mediator：聊天室 ----
class ChatRoom {
public:
    void join(shared_ptr<User> user) {
        cout << "  [聊天室] " << user->getName() << " 加入了聊天室" << endl;
        users_.push_back(user);
    }

    void sendMessage(const string& from, const string& message) {
        for (auto& user : users_) {
            if (user->getName() != from) {
                user->receive(from, message);
            }
        }
    }

    void sendPrivate(const string& from, const string& to, const string& message) {
        for (auto& user : users_) {
            if (user->getName() == to) {
                user->receive(from, "[私聊] " + message);
                return;
            }
        }
        cout << "  [聊天室] 用户 " << to << " 不在线" << endl;
    }

private:
    vector<shared_ptr<User>> users_;
};

void User::send(const string& message) {
    cout << "  [" << name_ << " 发送] " << message << endl;
    room_->sendMessage(name_, message);
}

void demo() {
    cout << "===== 中介者模式 (Mediator) Demo =====" << endl;

    ChatRoom room;
    
    auto alice = make_shared<User>("Alice", &room);
    auto bob = make_shared<User>("Bob", &room);
    auto charlie = make_shared<User>("Charlie", &room);

    room.join(alice);
    room.join(bob);
    room.join(charlie);

    alice->send("大家好！");
    bob->send("你好 Alice！");
    room.sendPrivate("Charlie", "Alice", "Hi，私聊你一下");

    cout << endl;
}

} // namespace MediatorPattern


// ============================================================================
// 21. 备忘录模式 (Memento)
// ============================================================================
// 意图：在不破坏封装的前提下，捕获和保存对象内部状态，以便后续恢复。
// 场景：文本编辑器撤销、游戏存档/读档、数据库事务回滚。
// ============================================================================
namespace MementoPattern {

// ---- Memento：存档 ----
class GameMemento {
public:
    GameMemento(int level, int hp, int score, const string& position)
        : level_(level), hp_(hp), score_(score), position_(position) {}

private:
    int level_;
    int hp_;
    int score_;
    string position_;

    friend class GameCharacter;  // 只有 Originator 能访问
};

// ---- Originator：游戏角色 ----
class GameCharacter {
public:
    GameCharacter(const string& name) : name_(name) {}

    void play(int newLevel, int newHp, int newScore, const string& newPos) {
        level_ = newLevel; hp_ = newHp; score_ = newScore; position_ = newPos;
    }

    void showStatus() const {
        cout << "  [" << name_ << "] Lv:" << level_ << " HP:" << hp_ 
             << " Score:" << score_ << " Pos:" << position_ << endl;
    }

    // 创建存档
    unique_ptr<GameMemento> save() const {
        cout << "  [存档] 保存当前状态..." << endl;
        return make_unique<GameMemento>(level_, hp_, score_, position_);
    }

    // 读取存档
    void restore(const GameMemento& memento) {
        level_ = memento.level_;
        hp_ = memento.hp_;
        score_ = memento.score_;
        position_ = memento.position_;
        cout << "  [读档] 恢复到存档状态" << endl;
    }

private:
    string name_;
    int level_ = 1;
    int hp_ = 100;
    int score_ = 0;
    string position_ = "起点";
};

// ---- Caretaker：存档管理器 ----
class SaveManager {
public:
    void addSave(const string& name, unique_ptr<GameMemento> memento) {
        saves_[name] = move(memento);
    }

    GameMemento* getSave(const string& name) {
        auto it = saves_.find(name);
        return it != saves_.end() ? it->second.get() : nullptr;
    }

private:
    map<string, unique_ptr<GameMemento>> saves_;
};

void demo() {
    cout << "===== 备忘录模式 (Memento) Demo =====" << endl;

    GameCharacter hero("勇者");
    SaveManager saveManager;

    // 初始状态
    hero.play(1, 100, 0, "新手村");
    hero.showStatus();

    // 存档1
    saveManager.addSave("新手村存档", hero.save());

    // 继续游戏
    hero.play(5, 80, 1500, "黑暗森林");
    hero.showStatus();

    // 存档2
    saveManager.addSave("森林存档", hero.save());

    // Boss战失败
    hero.play(5, 0, 800, "Boss房间");
    hero.showStatus();
    cout << "  💀 角色死亡！" << endl;

    // 读取存档
    auto* save = saveManager.getSave("森林存档");
    if (save) {
        hero.restore(*save);
        hero.showStatus();
    }

    cout << endl;
}

} // namespace MementoPattern


// ============================================================================
// 22. 访问者模式 (Visitor)
// ============================================================================
// 意图：在不改变元素类的前提下定义新操作。将算法与其所作用的对象分离。
// 场景：编译器AST遍历、文档格式导出、报表系统。
//
// 关键：双重分派（Double Dispatch）
// 优势：新增操作容易
// 劣势：新增元素类型困难
// ============================================================================
namespace VisitorPattern {

// 前向声明
class TextElement;
class ImageElement;
class TableElement;

// ---- 访问者接口 ----
class DocumentVisitor {
public:
    virtual ~DocumentVisitor() = default;
    virtual void visit(const TextElement& element) = 0;
    virtual void visit(const ImageElement& element) = 0;
    virtual void visit(const TableElement& element) = 0;
};

// ---- Element 接口 ----
class DocumentElement {
public:
    virtual ~DocumentElement() = default;
    virtual void accept(DocumentVisitor& visitor) const = 0;
};

// ---- 具体元素 ----
class TextElement : public DocumentElement {
public:
    TextElement(const string& text) : text_(text) {}
    void accept(DocumentVisitor& visitor) const override { visitor.visit(*this); }
    string getText() const { return text_; }

private:
    string text_;
};

class ImageElement : public DocumentElement {
public:
    ImageElement(const string& src, int w, int h) : src_(src), width_(w), height_(h) {}
    void accept(DocumentVisitor& visitor) const override { visitor.visit(*this); }
    string getSrc() const { return src_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

private:
    string src_;
    int width_, height_;
};

class TableElement : public DocumentElement {
public:
    TableElement(int rows, int cols) : rows_(rows), cols_(cols) {}
    void accept(DocumentVisitor& visitor) const override { visitor.visit(*this); }
    int getRows() const { return rows_; }
    int getCols() const { return cols_; }

private:
    int rows_, cols_;
};

// ---- 具体访问者：HTML导出 ----
class HtmlExportVisitor : public DocumentVisitor {
public:
    void visit(const TextElement& e) override {
        cout << "  <p>" << e.getText() << "</p>" << endl;
    }
    void visit(const ImageElement& e) override {
        cout << "  <img src=\"" << e.getSrc() << "\" width=\"" 
             << e.getWidth() << "\" height=\"" << e.getHeight() << "\"/>" << endl;
    }
    void visit(const TableElement& e) override {
        cout << "  <table rows=\"" << e.getRows() << "\" cols=\"" << e.getCols() << "\"/>" << endl;
    }
};

// ---- 具体访问者：Markdown导出 ----
class MarkdownExportVisitor : public DocumentVisitor {
public:
    void visit(const TextElement& e) override {
        cout << "  " << e.getText() << endl;
    }
    void visit(const ImageElement& e) override {
        cout << "  ![image](" << e.getSrc() << ")" << endl;
    }
    void visit(const TableElement& e) override {
        cout << "  | " << e.getRows() << "x" << e.getCols() << " table |" << endl;
    }
};

// ---- 具体访问者：统计分析 ----
class StatsVisitor : public DocumentVisitor {
public:
    void visit(const TextElement& e) override { textCount_++; charCount_ += e.getText().size(); }
    void visit(const ImageElement& e) override { imageCount_++; }
    void visit(const TableElement& e) override { tableCount_++; }

    void report() const {
        cout << "  [统计] 文本:" << textCount_ << " 图片:" << imageCount_ 
             << " 表格:" << tableCount_ << " 总字符:" << charCount_ << endl;
    }

private:
    int textCount_ = 0, imageCount_ = 0, tableCount_ = 0;
    size_t charCount_ = 0;
};

void demo() {
    cout << "===== 访问者模式 (Visitor) Demo =====" << endl;

    // 文档元素集合
    vector<unique_ptr<DocumentElement>> document;
    document.push_back(make_unique<TextElement>("Hello, Visitor Pattern!"));
    document.push_back(make_unique<ImageElement>("photo.png", 800, 600));
    document.push_back(make_unique<TextElement>("This is a paragraph."));
    document.push_back(make_unique<TableElement>(3, 4));

    // 使用不同访问者处理同一文档
    cout << "  --- HTML 导出 ---" << endl;
    HtmlExportVisitor htmlVisitor;
    for (const auto& elem : document) elem->accept(htmlVisitor);

    cout << "  --- Markdown 导出 ---" << endl;
    MarkdownExportVisitor mdVisitor;
    for (const auto& elem : document) elem->accept(mdVisitor);

    cout << "  --- 统计分析 ---" << endl;
    StatsVisitor statsVisitor;
    for (const auto& elem : document) elem->accept(statsVisitor);
    statsVisitor.report();

    // 新增操作只需新增Visitor子类，不用改动任何Element类！

    cout << endl;
}

} // namespace VisitorPattern


// ============================================================================
// 23. 解释器模式 (Interpreter) — 简单示例
// ============================================================================
// 意图：给定一个语言，定义其文法表示，并定义一个解释器。
// 场景：数学表达式求值、SQL解析、正则表达式。
//
// 这里实现一个简单的数学表达式解释器：支持 +, -, 数字, 变量
// ============================================================================
namespace InterpreterPattern {

// ---- 抽象表达式 ----
class Expression {
public:
    virtual ~Expression() = default;
    virtual int interpret(const map<string, int>& context) const = 0;
};

// ---- 终结符表达式：数字 ----
class NumberExpression : public Expression {
public:
    NumberExpression(int value) : value_(value) {}
    int interpret(const map<string, int>& context) const override {
        return value_;
    }

private:
    int value_;
};

// ---- 终结符表达式：变量 ----
class VariableExpression : public Expression {
public:
    VariableExpression(const string& name) : name_(name) {}
    int interpret(const map<string, int>& context) const override {
        auto it = context.find(name_);
        return it != context.end() ? it->second : 0;
    }

private:
    string name_;
};

// ---- 非终结符表达式：加法 ----
class AddExpression : public Expression {
public:
    AddExpression(unique_ptr<Expression> left, unique_ptr<Expression> right)
        : left_(move(left)), right_(move(right)) {}

    int interpret(const map<string, int>& context) const override {
        return left_->interpret(context) + right_->interpret(context);
    }

private:
    unique_ptr<Expression> left_, right_;
};

// ---- 非终结符表达式：减法 ----
class SubExpression : public Expression {
public:
    SubExpression(unique_ptr<Expression> left, unique_ptr<Expression> right)
        : left_(move(left)), right_(move(right)) {}

    int interpret(const map<string, int>& context) const override {
        return left_->interpret(context) - right_->interpret(context);
    }

private:
    unique_ptr<Expression> left_, right_;
};

// ---- 非终结符表达式：乘法 ----
class MulExpression : public Expression {
public:
    MulExpression(unique_ptr<Expression> left, unique_ptr<Expression> right)
        : left_(move(left)), right_(move(right)) {}

    int interpret(const map<string, int>& context) const override {
        return left_->interpret(context) * right_->interpret(context);
    }

private:
    unique_ptr<Expression> left_, right_;
};

void demo() {
    cout << "===== 解释器模式 (Interpreter) Demo =====" << endl;

    // 表达式: (x + 10) - (y * 2)
    auto expr = make_unique<SubExpression>(
        make_unique<AddExpression>(
            make_unique<VariableExpression>("x"),
            make_unique<NumberExpression>(10)
        ),
        make_unique<MulExpression>(
            make_unique<VariableExpression>("y"),
            make_unique<NumberExpression>(2)
        )
    );

    // 设置变量上下文
    map<string, int> context1 = {{"x", 5}, {"y", 3}};
    cout << "  表达式: (x + 10) - (y * 2)" << endl;
    cout << "  当 x=5, y=3 时: 结果 = " << expr->interpret(context1) << endl;

    map<string, int> context2 = {{"x", 20}, {"y", 7}};
    cout << "  当 x=20, y=7 时: 结果 = " << expr->interpret(context2) << endl;

    cout << endl;
}

} // namespace InterpreterPattern


// ============================================================================
// ========================= 四、现代C++进阶技巧 ==============================
// ============================================================================

// ============================================================================
// 现代C++：std::variant + std::visit 替代经典访问者模式
// ============================================================================
namespace ModernVisitor {

struct Circle {
    double radius;
    double area() const { return 3.14159 * radius * radius; }
};

struct Rectangle {
    double width, height;
    double area() const { return width * height; }
};

struct Triangle {
    double base, height;
    double area() const { return 0.5 * base * height; }
};

using Shape = variant<Circle, Rectangle, Triangle>;

void demo() {
    cout << "===== 现代C++：variant + visit (替代Visitor) =====" << endl;

    vector<Shape> shapes = {
        Circle{5.0},
        Rectangle{3.0, 4.0},
        Triangle{6.0, 8.0},
        Circle{2.5}
    };

    // 用 std::visit 实现"访问"，完全不需要虚函数！
    double totalArea = 0;
    for (const auto& shape : shapes) {
        double a = visit([](const auto& s) { return s.area(); }, shape);
        
        visit([](const auto& s) {
            using T = decay_t<decltype(s)>;
            if constexpr (is_same_v<T, Circle>) {
                cout << "  圆形 r=" << s.radius;
            } else if constexpr (is_same_v<T, Rectangle>) {
                cout << "  矩形 " << s.width << "x" << s.height;
            } else if constexpr (is_same_v<T, Triangle>) {
                cout << "  三角形 b=" << s.base << " h=" << s.height;
            }
        }, shape);
        
        cout << " → 面积=" << a << endl;
        totalArea += a;
    }
    cout << "  总面积: " << totalArea << endl;

    cout << endl;
}

} // namespace ModernVisitor


// ============================================================================
// 现代C++：CRTP 静态多态（编译期多态，零运行时开销）
// ============================================================================
namespace CRTPPattern {

// CRTP基类
template<typename Derived>
class Animal {
public:
    void speak() const {
        // 编译期多态：静态转换到派生类
        static_cast<const Derived*>(this)->speakImpl();
    }

    void info() const {
        cout << "  我是 " << static_cast<const Derived*>(this)->name() 
             << "，我的叫声是: ";
        speak();
    }
};

class Dog : public Animal<Dog> {
public:
    void speakImpl() const { cout << "汪汪!" << endl; }
    string name() const { return "狗"; }
};

class Cat : public Animal<Cat> {
public:
    void speakImpl() const { cout << "喵喵~" << endl; }
    string name() const { return "猫"; }
};

// 模板函数可以接受任何Animal<T>
template<typename T>
void makeAnimalSpeak(const Animal<T>& animal) {
    animal.info();
}

void demo() {
    cout << "===== CRTP 静态多态 Demo =====" << endl;

    Dog dog;
    Cat cat;

    makeAnimalSpeak(dog);
    makeAnimalSpeak(cat);

    // CRTP优势：
    // 1. 零虚函数开销（没有vtable查找）
    // 2. 编译期类型检查
    // 3. 可以被编译器内联优化

    cout << endl;
}

} // namespace CRTPPattern


// ============================================================================
// 主函数：运行所有设计模式示例
// ============================================================================
int main() {
    cout << "╔══════════════════════════════════════════════════╗" << endl;
    cout << "║     C++ 23种设计模式 + 现代C++进阶 完全教程       ║" << endl;
    cout << "╚══════════════════════════════════════════════════╝" << endl;
    cout << endl;

    // ======================== 创建型模式 ========================
    cout << "┌──────────────────────────────────────────────────┐" << endl;
    cout << "│            一、创建型模式 (Creational)             │" << endl;
    cout << "└──────────────────────────────────────────────────┘" << endl;
    SingletonPattern::demo();
    FactoryMethodPattern::demo();
    AbstractFactoryPattern::demo();
    BuilderPattern::demo();
    PrototypePattern::demo();

    // ======================== 结构型模式 ========================
    cout << "┌──────────────────────────────────────────────────┐" << endl;
    cout << "│            二、结构型模式 (Structural)             │" << endl;
    cout << "└──────────────────────────────────────────────────┘" << endl;
    AdapterPattern::demo();
    BridgePattern::demo();
    CompositePattern::demo();
    DecoratorPattern::demo();
    FacadePattern::demo();
    FlyweightPattern::demo();
    ProxyPattern::demo();

    // ======================== 行为型模式 ========================
    cout << "┌──────────────────────────────────────────────────┐" << endl;
    cout << "│            三、行为型模式 (Behavioral)             │" << endl;
    cout << "└──────────────────────────────────────────────────┘" << endl;
    StrategyPattern::demo();
    ObserverPattern::demo();
    CommandPattern::demo();
    TemplateMethodPattern::demo();
    StatePattern::demo();
    ChainOfResponsibilityPattern::demo();
    IteratorPattern::demo();
    MediatorPattern::demo();
    MementoPattern::demo();
    VisitorPattern::demo();
    InterpreterPattern::demo();

    // ======================== 现代C++进阶 ========================
    cout << "┌──────────────────────────────────────────────────┐" << endl;
    cout << "│            四、现代C++设计模式进阶                  │" << endl;
    cout << "└──────────────────────────────────────────────────┘" << endl;
    ModernVisitor::demo();
    CRTPPattern::demo();

    cout << "╔══════════════════════════════════════════════════╗" << endl;
    cout << "║                 所有示例运行完毕!                  ║" << endl;
    cout << "╚══════════════════════════════════════════════════╝" << endl;

    return 0;
}
