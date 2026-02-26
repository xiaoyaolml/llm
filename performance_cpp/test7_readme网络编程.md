# 现代 C++ 网络编程 — 完全教程

> **配套代码**: `test7.cpp` (2222 行, 19 章, C++17)
>
> **编译**:
> ```bash
> # Windows (MSVC)
> cl /std:c++17 /O2 /EHsc test7.cpp /link ws2_32.lib
>
> # Windows (MinGW)
> g++ -std=c++17 -O2 -o test7 test7.cpp -lws2_32
>
> # Linux / macOS
> g++ -std=c++17 -O2 -pthread -o test7 test7.cpp
> ```
>
> **运行**: 交互式菜单，可选 ①基础演示 ②Echo服务器 ③HTTP服务器 ④聊天室

---

## 目录

| 篇 | 章 | 主题 | 核心内容 | 难度 |
|---|---|---|---|---|
| **一、基础设施** | 1 | 平台抽象层 | Winsock/POSIX 统一、RAII 初始化 | ⭐⭐ |
| | 2 | Socket RAII | RAII 封装 socket_t、移动语义、选项设置 | ⭐⭐ |
| | 3 | 地址封装 | `SocketAddress` 统一 IPv4/IPv6 | ⭐⭐ |
| **二、传输层** | 4 | TCP 客户端 | `TcpClient`、getaddrinfo、send_all | ⭐⭐ |
| | 5 | TCP 服务器 | `TcpServer`、bind/listen/accept、SO_REUSEADDR | ⭐⭐ |
| | 6 | UDP 通信 | `UdpSocket`、sendto/recvfrom 封装 | ⭐⭐ |
| **三、IO 模型** | 7 | 非阻塞 IO 与 select | `SelectSet` 跨平台封装 | ⭐⭐⭐ |
| | 8 | Buffer 管理 | `NetworkBuffer`、零拷贝、行读取、协议解析 | ⭐⭐⭐ |
| **四、应用协议** | 9 | HTTP 客户端 | 手写 GET/POST、响应解析 | ⭐⭐⭐ |
| | 10 | HTTP 服务器 | 路由注册、请求解析、响应发送 | ⭐⭐⭐ |
| | 11 | 协议序列化 | `PacketWriter`/`PacketReader`、二进制编解码 | ⭐⭐⭐ |
| | 12 | 网络字节序 | 编译期 swap、`NetworkOrder<T>` 强类型 | ⭐⭐ |
| **五、网络服务** | 13 | DNS 解析 | getaddrinfo 封装、IPv4/IPv6 | ⭐⭐ |
| | 14 | 超时与错误 | 带超时连接、`NetResult<T>` (variant) | ⭐⭐⭐ |
| | 15 | 多线程服务器 | Thread-per-Connection 模式 | ⭐⭐⭐ |
| | 16 | 事件驱动服务器 | `SelectServer` 事件循环 | ⭐⭐⭐⭐ |
| **六、实战** | 17 | 简易聊天室 | 完整聊天服务器、改名、广播 | ⭐⭐⭐⭐ |
| | 18 | Redis 客户端 | RESP 协议编解码、SET/GET/PING | ⭐⭐⭐⭐ |
| | 19 | 最佳实践 | 粘包/SIGPIPE/TIME_WAIT/调试工具/性能调优 | ⭐⭐⭐ |

---

## 文件关系

| 文件 | 主题 | 行数 | 章节 | 与本文件关系 |
|---|---|---|---|---|
| `test6.cpp` / `test6_readme` | 多线程基础 | ~2400 | 18 章 | **前置依赖** — 线程/互斥锁基础 |
| **`test7.cpp`** / **本文件** | **网络编程** | **2222** | **19 章** | **当前** |
| `test8.cpp` / `test8_readme` | STL 深入 | - | - | **参考** — 容器/算法 |
| `test9.cpp` / `test9_readme` | 高并发设计 | - | - | **进阶** — 并发服务器/连接池 |
| `test11.cpp` / `test11_readme` | 无锁编程 | 2929 | 25 章 | **进阶** — 无锁队列用于网络IO |

---

# 一、基础设施篇

---

## 第 1 章：平台抽象层 — 统一 Winsock / POSIX

### 1.1 核心问题

C++ 标准库**至今没有网络库**（Networking TS 尚未进入标准），因此必须直接使用系统 API。但 Windows 和 POSIX 的 socket API 存在诸多差异：

| 差异点 | Windows (Winsock2) | POSIX (Linux/macOS) |
|---|---|---|
| **头文件** | `<winsock2.h>`, `<ws2tcpip.h>` | `<sys/socket.h>`, `<netinet/in.h>` |
| **类型** | `SOCKET` (unsigned) | `int` (文件描述符) |
| **无效值** | `INVALID_SOCKET` | `-1` |
| **关闭** | `closesocket()` | `close()` |
| **错误码** | `WSAGetLastError()` | `errno` |
| **初始化** | 需要 `WSAStartup()` / `WSACleanup()` | 无需 |
| **SIGPIPE** | 不存在 | 写入已关闭 socket 时触发 |

### 1.2 平台类型统一

```cpp
// test7.cpp ch1 — 平台抽象
namespace net {

#ifdef _WIN32
    using socket_t = SOCKET;
    constexpr socket_t INVALID_SOCK = INVALID_SOCKET;
    inline int close_socket(socket_t s) { return closesocket(s); }
    inline int get_last_error() { return WSAGetLastError(); }
    #define MSG_NOSIGNAL 0  // Winsock 没有 SIGPIPE
#else
    using socket_t = int;
    constexpr socket_t INVALID_SOCK = -1;
    inline int close_socket(socket_t s) { return close(s); }
    inline int get_last_error() { return errno; }
#endif
```

### 1.3 Winsock RAII 初始化

```cpp
// test7.cpp ch1 — WinsockInit
class WinsockInit {
#ifdef _WIN32
    bool initialized_ = false;
#endif
public:
    WinsockInit() {
#ifdef _WIN32
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0)
            throw std::runtime_error("WSAStartup failed");
        initialized_ = true;
#else
        signal(SIGPIPE, SIG_IGN);  // 忽略 SIGPIPE
#endif
    }
    ~WinsockInit() {
#ifdef _WIN32
        if (initialized_) WSACleanup();
#endif
    }
    // 禁止拷贝
    WinsockInit(const WinsockInit&) = delete;
    WinsockInit& operator=(const WinsockInit&) = delete;
};

// 使用: 在 main 开头创建
// WinsockInit wsa;  // RAII 自动初始化和清理
```

### 1.4 深入扩展

**为什么必须在 POSIX 上忽略 SIGPIPE？**

当向一个**已被对端关闭**的 TCP 连接写数据时：
- POSIX: 内核发送 `SIGPIPE` 信号 → 默认行为是**终止进程**
- 这对服务器是灾难性的——一个客户端断开就整个服务器崩溃
- 解决方案：
  1. `signal(SIGPIPE, SIG_IGN)` — 全局忽略（推荐）
  2. `send(fd, data, len, MSG_NOSIGNAL)` — 每次调用指定
  3. `sigaction(SIGPIPE, {SIG_IGN})` — 更精确的控制

**Winsock 版本选择：**
- `MAKEWORD(2, 2)` → Winsock 2.2，支持所有现代功能
- 不要用 Winsock 1.1 (MAKEWORD(1,1))——缺少 `select` 超时精度等功能

**跨平台编译策略：**

```
#ifdef _WIN32   → Windows (MSVC / MinGW)
#elif __linux__  → Linux
#elif __APPLE__  → macOS
#endif
```

---

## 第 2 章：Socket RAII 封装

### 2.1 设计动机

裸 socket 与裸指针一样危险：

| 问题 | 裸 socket | RAII Socket |
|---|---|---|
| 忘记 close | ✅ 资源泄漏 | ❌ 析构自动关闭 |
| 重复 close | ✅ 未定义行为 | ❌ close 后置 INVALID |
| 异常泄漏 | ✅ 异常后不会 close | ❌ 栈展开自动关闭 |
| 拷贝语义 | ✅ 双重 close | ❌ 禁止拷贝 |
| 所有权转移 | ❌ 手动管理 | ✅ 移动语义 |

### 2.2 完整 RAII Socket 类

```cpp
// test7.cpp ch2 — Socket RAII
class Socket {
    socket_t fd_ = INVALID_SOCK;

public:
    Socket() = default;
    explicit Socket(socket_t fd) : fd_(fd) {}

    // 创建 socket
    Socket(int domain, int type, int protocol = 0) {
        fd_ = ::socket(domain, type, protocol);
        if (fd_ == INVALID_SOCK)
            throw std::system_error(get_last_error(),
                std::system_category(), "socket() failed");
    }

    // 禁止拷贝
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // 移动语义 — 转移所有权
    Socket(Socket&& other) noexcept : fd_(other.fd_) {
        other.fd_ = INVALID_SOCK;
    }
    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            close();
            fd_ = other.fd_;
            other.fd_ = INVALID_SOCK;
        }
        return *this;
    }

    ~Socket() { close(); }

    void close() {
        if (fd_ != INVALID_SOCK) {
            close_socket(fd_);
            fd_ = INVALID_SOCK;
        }
    }

    socket_t fd() const { return fd_; }
    bool valid() const { return fd_ != INVALID_SOCK; }
    explicit operator bool() const { return valid(); }
```

### 2.3 Socket 选项设置

```cpp
    // SO_REUSEADDR — 服务器必备，避免 TIME_WAIT 时 bind 失败
    void set_reuse_addr(bool enable = true);

    // O_NONBLOCK / FIONBIO — 非阻塞模式
    void set_nonblocking(bool enable = true);

    // TCP_NODELAY — 禁用 Nagle 算法，降低延迟
    void set_tcp_nodelay(bool enable = true);

    // SO_RCVTIMEO — 接收超时
    void set_recv_timeout(int milliseconds);
```

### 2.4 基础 I/O 方法

```cpp
    int send_data(const void* data, int len, int flags = 0) const {
        return ::send(fd_, static_cast<const char*>(data), len, flags | MSG_NOSIGNAL);
    }

    int send_string(std::string_view str, int flags = 0) const {
        return send_data(str.data(), static_cast<int>(str.size()), flags);
    }

    int recv_data(void* buf, int len, int flags = 0) const {
        return ::recv(fd_, static_cast<char*>(buf), len, flags);
    }

    std::optional<std::string> recv_string(int max_len = 4096) const {
        std::string buf(max_len, '\0');
        int n = recv_data(buf.data(), max_len);
        if (n <= 0) return std::nullopt;
        buf.resize(n);
        return buf;
    }
```

### 2.5 深入扩展

**release() 模式 — 转移裸句柄所有权：**

```cpp
socket_t release() {
    socket_t tmp = fd_;
    fd_ = INVALID_SOCK;
    return tmp;  // 调用者接管关闭职责
}
// 用途: 与需要裸 socket 的第三方库交互
```

**与 unique_ptr 的设计对比：**

| 设计点 | `Socket` | `unique_ptr<FILE>` |
|---|---|---|
| 无效值 | `INVALID_SOCK` | `nullptr` |
| 释放操作 | `close_socket()` | `fclose()` |
| 移动语义 | ✅ | ✅ |
| 定制删除器 | 内置 | 模板参数 |

**进阶: 可考虑用 `unique_fd` 通用模式：**

```cpp
struct SocketCloser {
    void operator()(socket_t fd) const { close_socket(fd); }
};
// 但 unique_ptr 不适合非指针类型, 故自定义 Socket 更直观
```

---

## 第 3 章：地址封装 — IPv4 / IPv6 统一

### 3.1 sockaddr 家族的复杂性

系统 API 使用多种地址结构，需要大量强制转换：

```
sockaddr        ← 通用基类 (16 字节)
sockaddr_in     ← IPv4 (sin_family, sin_port, sin_addr)
sockaddr_in6    ← IPv6 (sin6_family, sin6_port, sin6_addr)
sockaddr_storage ← 足够容纳任一类型 (128 字节)
```

### 3.2 统一 SocketAddress 类

```cpp
// test7.cpp ch3 — SocketAddress
class SocketAddress {
    sockaddr_storage storage_{};
    socklen_t len_ = 0;

public:
    // 工厂方法
    static SocketAddress from_ipv4(const char* ip, uint16_t port);
    static SocketAddress from_ipv6(const char* ip, uint16_t port);
    static SocketAddress any_ipv4(uint16_t port);  // INADDR_ANY

    // 访问器
    sockaddr* data();
    const sockaddr* data() const;
    socklen_t size() const;
    int family() const;
    uint16_t port() const;  // 自动处理 ntohs
    std::string ip() const; // 自动调用 inet_ntop
    std::string to_string() const; // "1.2.3.4:8080"
};
```

### 3.3 IPv4 vs IPv6 要点

| 特性 | IPv4 | IPv6 |
|---|---|---|
| 地址长度 | 32 位 (4 字节) | 128 位 (16 字节) |
| 表示 | `192.168.1.1` | `::1`, `fe80::1%eth0` |
| family | `AF_INET` | `AF_INET6` |
| 结构 | `sockaddr_in` | `sockaddr_in6` |
| 转换函数 | `inet_pton(AF_INET, ...)` | `inet_pton(AF_INET6, ...)` |
| ANY 地址 | `INADDR_ANY` | `in6addr_any` |
| 环回 | `127.0.0.1` | `::1` |

### 3.4 深入扩展

**`inet_pton` vs `inet_aton` vs `inet_addr`：**
- `inet_addr`: 古老 API，不支持 IPv6，不能区分错误和 `255.255.255.255`
- `inet_aton`: 只支持 IPv4，非 Windows 可用
- **`inet_pton`**: 现代 API，支持 IPv4/IPv6，所有平台可用 — **唯一推荐**

**`getaddrinfo` 是更好的替代（见第 13 章）：**
- 支持 DNS 解析 + 地址转换一体
- 自动处理 IPv4/IPv6 双栈
- 返回可直接用于 `connect`/`bind` 的地址列表

---

# 二、传输层篇

---

## 第 4 章：TCP 客户端 — 连接与收发

### 4.1 TcpClient 设计

```cpp
// test7.cpp ch4 — TcpClient
class TcpClient {
    Socket socket_;

public:
    // 连接到服务器 (支持主机名/IP, 自动 IPv4/IPv6)
    void connect(const char* host, uint16_t port) {
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;      // IPv4 或 IPv6
        hints.ai_socktype = SOCK_STREAM;  // TCP

        addrinfo* result = nullptr;
        int ret = getaddrinfo(host, std::to_string(port).c_str(),
                              &hints, &result);
        if (ret != 0)
            throw std::runtime_error("getaddrinfo failed");

        // RAII guard 确保 freeaddrinfo 被调用
        auto guard = std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>(
            result, freeaddrinfo);

        // 遍历地址列表，尝试连接
        for (auto* ai = result; ai != nullptr; ai = ai->ai_next) {
            socket_ = Socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
            if (::connect(socket_.fd(), ai->ai_addr,
                         static_cast<int>(ai->ai_addrlen)) == 0)
                return;  // 连接成功
        }
        throw std::runtime_error("Failed to connect");
    }
```

### 4.2 send_all — 处理部分发送

```cpp
    // 重要: send() 不保证一次发完所有数据!
    bool send_all(std::string_view data) {
        size_t total_sent = 0;
        while (total_sent < data.size()) {
            int sent = socket_.send_data(
                data.data() + total_sent,
                static_cast<int>(data.size() - total_sent));
            if (sent <= 0) return false;  // 错误或连接关闭
            total_sent += sent;
        }
        return true;
    }
```

**为什么需要 send_all？**

```
send(10000 字节) → 实际可能只发了 5000 字节

原因:
  - 内核 send buffer 满
  - Nagle 算法延迟
  - 网络拥塞

必须: 循环发送, 直到全部完成
```

### 4.3 接收数据

```cpp
    std::optional<std::string> receive(int max_len = 4096);

    // 接收直到连接关闭 (用于 HTTP Connection: close)
    std::string receive_all() {
        std::string result;
        char buf[4096];
        while (true) {
            int n = socket_.recv_data(buf, sizeof(buf));
            if (n <= 0) break;
            result.append(buf, n);
        }
        return result;
    }
```

### 4.4 深入扩展

**getaddrinfo 的 RAII 用法——unique_ptr 带自定义删除器：**

```cpp
auto guard = std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>(
    result, freeaddrinfo);
// result 会在 guard 析构时自动被 freeaddrinfo 释放
// 即使 throw 也不会泄漏
```

**TCP 三次握手过程：**

```
客户端                          服务端
   │                              │
   │──── SYN (seq=x) ───────────→│
   │                              │
   │←─── SYN+ACK (seq=y, ack=x+1)│
   │                              │
   │──── ACK (ack=y+1) ─────────→│
   │                              │
   │     连接建立 ✓                │
```

**Happy Eyeballs (RFC 6555) — 现代连接策略：**
- `getaddrinfo` 可能返回多个地址（IPv4 + IPv6）
- Happy Eyeballs: 同时尝试 IPv4 和 IPv6，谁先连上用谁
- 本实现简化为顺序尝试，生产环境应用并行连接

---

## 第 5 章：TCP 服务器 — 监听与接受连接

### 5.1 服务端流程

```
socket()  →  bind()  →  listen()  →  accept()  →  recv/send  →  close
   ↓            ↓           ↓            ↓
 创建套接字  绑定端口    开始监听    接受连接(阻塞)
```

### 5.2 TcpServer 实现

```cpp
// test7.cpp ch5 — TcpServer
class TcpServer {
    Socket listen_socket_;
    uint16_t port_ = 0;

public:
    void start(uint16_t port, int backlog = 128) {
        listen_socket_ = Socket(AF_INET, SOCK_STREAM);
        listen_socket_.set_reuse_addr();  // 重要! 避免 TIME_WAIT

        auto addr = SocketAddress::any_ipv4(port);
        if (::bind(listen_socket_.fd(), addr.data(), addr.size()) < 0)
            throw std::system_error(..., "bind failed");

        if (::listen(listen_socket_.fd(), backlog) < 0)
            throw std::system_error(..., "listen failed");

        port_ = port;
    }

    // 等待并接受连接
    struct AcceptResult {
        Socket client_socket;
        SocketAddress client_addr;
    };

    std::optional<AcceptResult> accept();
};
```

### 5.3 关键参数

**backlog:**
- `listen(fd, backlog)` 中 backlog 指**已完成三次握手但未被 accept** 的连接队列长度
- Linux 实际值: `min(backlog, /proc/sys/net/core/somaxconn)` (默认 4096)
- 推荐: 生产环境用 128~4096

**SO_REUSEADDR:**
- 没有此选项: 服务器重启时 `bind()` 报 "Address already in use"
- 原因: 旧连接处于 **TIME_WAIT** 状态（持续 2×MSL ≈ 60~120 秒）
- 设置后: 允许绑定处于 TIME_WAIT 的地址

### 5.4 深入扩展

**TCP TIME_WAIT 状态详解：**

```
主动关闭方进入 TIME_WAIT:

  客户端                    服务端
     │── FIN ──────────→│  (主动关闭)
     │←─ ACK ───────────│
     │←─ FIN ───────────│  (被动关闭)
     │── ACK ──────────→│
     │                    │
     │  TIME_WAIT         │  CLOSED
     │  (等待 2×MSL)      │
     │  CLOSED            │

为什么需要 TIME_WAIT:
  1. 确保最后一个 ACK 到达对方
  2. 防止旧数据包被新连接误收
```

**IPv4 vs IPv6 双栈监听：**

```cpp
// 同时监听 IPv4 和 IPv6:
Socket sock(AF_INET6, SOCK_STREAM);
int off = 0;
setsockopt(sock.fd(), IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off));
// IPv6 socket 也接受 IPv4 连接 (mapped address)
```

---

## 第 6 章：UDP 通信 — 无连接数据报

### 6.1 TCP vs UDP 对比

| 特性 | TCP (SOCK_STREAM) | UDP (SOCK_DGRAM) |
|---|---|---|
| 连接 | 需要 connect/accept | 无连接 |
| 可靠性 | 保证到达、有序 | 不保证 |
| 流模型 | 字节流 (无边界) | 数据报 (有边界) |
| 开销 | 高 (三次握手 + ACK) | 低 (无握手) |
| 用途 | HTTP/数据库/文件传输 | DNS/游戏/视频流/NTP |
| 拥塞控制 | 有 | 无 |

### 6.2 UdpSocket 封装

```cpp
// test7.cpp ch6 — UdpSocket
class UdpSocket {
    Socket socket_;

public:
    UdpSocket() : socket_(AF_INET, SOCK_DGRAM) {}

    void bind(uint16_t port);

    int send_to(std::string_view data, const SocketAddress& dest) {
        return ::sendto(socket_.fd(), data.data(),
                        static_cast<int>(data.size()), 0,
                        dest.data(), static_cast<int>(dest.size()));
    }

    struct RecvResult {
        std::string data;
        SocketAddress sender;  // 自动记录发送者地址
    };

    std::optional<RecvResult> recv_from(int max_len = 65536);
};
```

### 6.3 深入扩展

**UDP 数据报的大小限制：**
- **理论最大**: 65535 - 20(IP头) - 8(UDP头) = 65507 字节
- **实际建议**: ≤ 1472 字节 (Ethernet MTU 1500 - 20 - 8)
- 超过 MTU 会触发 IP 分片 → 任何一片丢失整个报文丢失 → 实际丢包率飙升
- 游戏/实时应用: 通常用 512~1200 字节

**可靠 UDP 的实现思路 (KCP/QUIC/ENet)：**
- 序列号 + ACK (确认)
- 超时重传
- 拥塞控制
- 这就是 **QUIC** 协议 (HTTP/3) 的基础

---

# 三、IO 模型篇

---

## 第 7 章：非阻塞 I/O 与 select 多路复用

### 7.1 五种 IO 模型

```
① 阻塞 IO (默认)     — recv 阻塞直到有数据
② 非阻塞 IO          — recv 立即返回 EAGAIN
③ IO 多路复用         — select/poll/epoll 监视多个 fd
④ 信号驱动 IO         — SIGIO 通知
⑤ 异步 IO            — io_uring (Linux), IOCP (Windows)
```

### 7.2 SelectSet 跨平台封装

```cpp
// test7.cpp ch7 — SelectSet
class SelectSet {
    fd_set read_set_, write_set_, except_set_;
    socket_t max_fd_ = 0;

public:
    void add_read(socket_t fd);    // 监视可读
    void add_write(socket_t fd);   // 监视可写
    void add_except(socket_t fd);  // 监视异常

    bool is_readable(socket_t fd) const;
    bool is_writable(socket_t fd) const;
    bool has_error(socket_t fd) const;

    // 等待事件 (timeout_ms=-1 → 无限等待)
    int wait(int timeout_ms = -1) {
        timeval tv;
        timeval* ptv = nullptr;
        if (timeout_ms >= 0) {
            tv.tv_sec = timeout_ms / 1000;
            tv.tv_usec = (timeout_ms % 1000) * 1000;
            ptv = &tv;
        }
        int nfds = static_cast<int>(max_fd_ + 1);
        return ::select(nfds, &read_set_, &write_set_, &except_set_, ptv);
    }
};
```

### 7.3 select 的限制

| 限制 | 说明 | 替代方案 |
|---|---|---|
| fd 数量 | 最多 `FD_SETSIZE` (通常 1024) | `poll` 无限制 |
| 性能 | O(n) 扫描所有 fd | `epoll` O(1) |
| 每次重建 | fd_set 每次 select 后被修改 | `epoll` 状态持久 |
| 跨平台 | ✅ Windows/Linux/Mac | epoll 只 Linux |

### 7.4 深入扩展

**IO 多路复用对比表：**

| | `select` | `poll` | `epoll` (Linux) | `kqueue` (BSD/Mac) | `IOCP` (Windows) |
|---|---|---|---|---|---|
| 便携性 | ✅ 全平台 | ✅ POSIX | ❌ 仅 Linux | ❌ 仅 BSD/Mac | ❌ 仅 Windows |
| 最大 fd | FD_SETSIZE | 无限 | 无限 | 无限 | 无限 |
| 复杂度 | O(n) | O(n) | O(1) | O(1) | O(1) |
| 触发 | 水平触发 | 水平触发 | 水平/边缘 | 水平/边缘 | 完成端口 |
| 适用 | < 1000 连接 | < 10000 | > 10000 | > 10000 | > 10000 |

**epoll 基本用法 (可扩展方向)：**

```cpp
// Linux epoll 示意
int epfd = epoll_create1(0);

epoll_event ev;
ev.events = EPOLLIN | EPOLLET;  // 读 + 边缘触发
ev.data.fd = listen_fd;
epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

epoll_event events[1024];
int n = epoll_wait(epfd, events, 1024, timeout_ms);
for (int i = 0; i < n; ++i) {
    if (events[i].data.fd == listen_fd)
        accept_new_client();
    else
        handle_client(events[i].data.fd);
}
```

---

## 第 8 章：Buffer 管理 — 网络缓冲设计

### 8.1 设计动机

网络 I/O 的核心挑战：
- `recv()` 每次返回的数据量**不确定** (1 字节 ~ 缓冲区大小)
- TCP 是**字节流**，不保留消息**边界**
- 需要在用户态做**拆包** (协议解析)

### 8.2 NetworkBuffer 结构

```
  ┌────────────────────────────────────────────┐
  │  已读区域  │    可读数据    │    空闲空间    │
  │ (可回收)  │ (待处理)      │ (可写入)      │
  └────────────────────────────────────────────┘
  ↑            ↑                ↑              ↑
  0         read_pos_       write_pos_     data_.size()
```

```cpp
// test7.cpp ch8 — NetworkBuffer
class NetworkBuffer {
    std::vector<char> data_;
    size_t read_pos_ = 0;
    size_t write_pos_ = 0;

public:
    char* write_ptr();           // 可写区域起始
    size_t writable_bytes();     // 可写字节数
    const char* read_ptr();      // 可读区域起始
    size_t readable_bytes();     // 可读字节数

    void append(std::string_view sv);    // 追加数据
    std::string read_string(size_t n);   // 读出 n 字节
    std::optional<std::string> read_line(); // 读一行 (\r\n)

    std::optional<size_t> find(char delim);        // 查找字符
    std::optional<size_t> find(std::string_view p); // 查找模式

    void compact();  // 回收已读空间
    int read_from(const Socket& sock);  // 从 socket 读入
};
```

### 8.3 compact() — 回收已读空间

```
compact 前:
  [已读已读已读|可读数据|空闲]
              ↑read     ↑write

compact 后: (memmove)
  [可读数据|───── 大量空闲 ─────]
  ↑read    ↑write
```

### 8.4 协议解析模式

```cpp
// 基于分隔符(\r\n)的协议解析:
while (auto line = buf.read_line()) {
    process(*line);  // HTTP、SMTP、Redis 等文本协议
}

// 基于长度前缀的协议解析:
while (buf.readable_bytes() >= sizeof(PacketHeader)) {
    auto* hdr = reinterpret_cast<const PacketHeader*>(buf.read_ptr());
    uint32_t payload_len = ntohl(hdr->payload_length);
    if (buf.readable_bytes() < sizeof(PacketHeader) + payload_len)
        break;  // 数据不足, 等下次 recv
    process_packet(buf.read_ptr(), sizeof(PacketHeader) + payload_len);
    buf.advance_read(sizeof(PacketHeader) + payload_len);
}
```

### 8.5 深入扩展

**零拷贝思想：**
- 每次 `recv()` 直接写入 buffer 的空闲区域 → 无需中间 `char[4096]`
- 每次 `send()` 直接从 buffer 的可读区域发出 → 无需构造临时 string

**Muduo 风格 Buffer (陈硕)：**
- prepend 区域: 在头部预留 8 字节，写入 header 时无需 memmove
- 自动扩容: `vector<char>` 扩容策略
- 最佳实践: 读入时先 compact 再读，避免频繁分配

---

# 四、应用协议篇

---

## 第 9 章：HTTP 客户端 — 手写 HTTP/1.1 请求

### 9.1 HTTP 请求格式

```
GET /path HTTP/1.1\r\n          ← 请求行
Host: example.com\r\n           ← 头部
User-Agent: ModernCpp/1.0\r\n
Connection: close\r\n
\r\n                            ← 空行 = 头部结束
```

### 9.2 HttpClient 实现

```cpp
// test7.cpp ch9 — HttpClient
class HttpClient {
public:
    static HttpResponse get(const std::string& host, uint16_t port,
                            const std::string& path) {
        TcpClient client;
        client.connect(host.c_str(), port);

        std::string request =
            "GET " + path + " HTTP/1.1\r\n"
            "Host: " + host + "\r\n"
            "Connection: close\r\n"    // 服务器发完就关闭
            "User-Agent: ModernCpp/1.0\r\n"
            "\r\n";

        client.send_all(request);
        return parse_response(client.receive_all());
    }

    static HttpResponse post(const std::string& host, uint16_t port,
                             const std::string& path,
                             const std::string& content_type,
                             const std::string& body);
```

### 9.3 HTTP 响应解析

```cpp
    // 响应格式:
    // HTTP/1.1 200 OK\r\n
    // Content-Type: text/html\r\n
    // Content-Length: 1234\r\n
    // \r\n
    // <html>...</html>

    static HttpResponse parse_response(const std::string& raw) {
        HttpResponse resp;
        // 1. 解析状态行 → status_code, status_text
        // 2. 解析头部 → headers map
        // 3. 空行后的部分 → body
        return resp;
    }
```

### 9.4 深入扩展

**Connection: close vs keep-alive：**
- `close`: 服务器发完响应后关闭连接 → 客户端 `receive_all()` 直到 EOF
- `keep-alive`: 连接复用 → 必须用 `Content-Length` 或 `Transfer-Encoding: chunked` 判断 body 结束
- 本实现用 `close` 简化，生产环境应支持 keep-alive

**HTTP/1.1 vs HTTP/2 vs HTTP/3：**

| | HTTP/1.1 | HTTP/2 | HTTP/3 |
|---|---|---|---|
| 传输 | TCP | TCP | QUIC (UDP) |
| 多路复用 | 否 (管线化已废) | 是 (帧) | 是 |
| 头部压缩 | 无 | HPACK | QPACK |
| 服务器推送 | 无 | 有 | 有 |
| 队头阻塞 | 有 | TCP层有 | 无 |

**可扩展方向**: 添加 HTTPS 支持 (需 OpenSSL/LibreSSL), chunked 传输, 连接池

---

## 第 10 章：HTTP 服务器 — 简易 Web Server

### 10.1 路由注册

```cpp
// test7.cpp ch10 — HttpServer
class HttpServer {
    using Handler = std::function<HttpResponse(const HttpRequest&)>;
    std::unordered_map<std::string, Handler> routes_;
    Handler default_handler_;  // 404

public:
    void route(const std::string& path, Handler handler) {
        routes_[path] = std::move(handler);
    }

    void start(uint16_t port);

    // 使用:
    // server.route("/", [](auto& req) { return make_html_response("Hello!"); });
    // server.route("/api/time", [](auto& req) { return make_json_response(...); });
};
```

### 10.2 请求处理流程

```
客户端连接 → accept()
  → read 到 NetworkBuffer
  → parse_request() 解析方法/路径/头部/Body
  → routes_ 查找对应 Handler
  → 执行 Handler → 得到 HttpResponse
  → send_response() 发送 "HTTP/1.1 200 OK\r\n..."
  → 关闭连接
```

### 10.3 代码中的请求解析

```cpp
static std::optional<HttpRequest> parse_request(NetworkBuffer& buf) {
    HttpRequest req;
    // 读取请求行: "GET /path HTTP/1.1"
    auto line = buf.read_line();
    std::istringstream iss(*line);
    iss >> req.method >> req.path >> req.version;

    // 读取头部 (直到空行)
    while (auto header_line = buf.read_line()) {
        if (header_line->empty()) break;
        // 解析 "Key: Value"
    }

    // Body (根据 Content-Length 或剩余数据)
    return req;
}
```

### 10.4 深入扩展

**当前实现的局限与扩展方向：**

| 局限 | 扩展方向 |
|---|---|
| 单线程处理 | 线程池 / 异步 IO (第 15-16 章) |
| 无 HTTPS | 集成 OpenSSL |
| 无并发 | epoll + 非阻塞 IO |
| 无 keep-alive | 连接复用 |
| 路由只支持精确匹配 | 正则路由 / 参数路由 |
| 无中间件 | 请求/响应管线 (logging, CORS, auth) |
| 无静态文件 | sendfile() 零拷贝 |

---

## 第 11 章：协议序列化 — 二进制协议编解码

### 11.1 协议包格式 (TLV)

```
+─────────+──────────+───────────+
│  Magic  │  Length  │  Payload  │
│ 4 bytes │ 4 bytes │ N bytes   │
+─────────+──────────+───────────+
```

- **Magic**: `0xDEADBEEF` — 有效包标识，快速过滤垃圾数据
- **Length**: payload 长度 (网络字节序)
- **Payload**: 实际数据

### 11.2 PacketWriter — 编码器

```cpp
// test7.cpp ch11 — PacketWriter
class PacketWriter {
    std::vector<char> buffer_;

public:
    PacketWriter() { buffer_.resize(sizeof(PacketHeader)); } // 预留 header

    void write_u8(uint8_t val);
    void write_u16(uint16_t val);  // htons
    void write_u32(uint32_t val);  // htonl
    void write_string(std::string_view str);  // len(u16) + data

    std::string_view finish();  // 填充 header, 返回完整包
};
```

### 11.3 PacketReader — 解码器

```cpp
class PacketReader {
    const char* data_;
    size_t size_, pos_ = 0;

public:
    bool read_u8(uint8_t& val);
    bool read_u16(uint16_t& val);  // ntohs
    bool read_u32(uint32_t& val);  // ntohl
    bool read_string(std::string& val);  // len(u16) + data
    size_t remaining() const;
};
```

### 11.4 深入扩展

**常见序列化格式对比：**

| 格式 | 类型 | 大小 | 速度 | Schema | 适用 |
|---|---|---|---|---|---|
| 自定义二进制 | Binary | 最小 | 最快 | 无 | 高性能内部通信 |
| Protocol Buffers | Binary | 小 | 快 | .proto | Google RPC |
| FlatBuffers | Binary | 小 | 极快(零拷贝) | .fbs | 游戏 |
| MessagePack | Binary | 小 | 快 | 无 | 跨语言 |
| JSON | Text | 大 | 慢 | JSON Schema | Web API |
| XML | Text | 最大 | 最慢 | XSD | SOAP |

**#pragma pack(push, 1) 的作用：**

```cpp
#pragma pack(push, 1)
struct PacketHeader {
    uint32_t magic;           // offset 0
    uint32_t payload_length;  // offset 4 (无填充!)
};
#pragma pack(pop)

// 没有 pack(1) 时, 编译器可能插入 padding
// 导致 sizeof(PacketHeader) > 8 → 与对端不匹配!
```

---

## 第 12 章：网络字节序与类型安全转换

### 12.1 大端 vs 小端

```
uint32_t x = 0x12345678;

Big-Endian (网络序):    12 34 56 78  (高字节在前)
Little-Endian (x86):    78 56 34 12  (低字节在前)
```

**黄金规则**: 所有多字节整数在网络传输前**必须**转为网络字节序 (Big-Endian)。

### 12.2 编译期字节序检测与交换

```cpp
// test7.cpp ch12
constexpr uint16_t byte_swap_16(uint16_t val) {
    return (val >> 8) | (val << 8);
}

constexpr uint32_t byte_swap_32(uint32_t val) {
    return ((val >> 24) & 0xFF)
         | ((val >>  8) & 0xFF00)
         | ((val <<  8) & 0xFF0000)
         | ((val << 24) & 0xFF000000);
}

// 编译期验证
static_assert(byte_swap_16(0x1234) == 0x3412);
static_assert(byte_swap_32(0x12345678) == 0x78563412);
```

### 12.3 强类型 NetworkOrder\<T\>

```cpp
// test7.cpp ch12 — 防止误用主机序
template <typename T>
class NetworkOrder {
    T value_;  // 存储为网络字节序
public:
    static NetworkOrder from_host(T val);
    T to_host() const;
    T raw() const { return value_; }
};

// 使用:
auto n = NetworkOrder<uint32_t>::from_host(0x12345678);
uint32_t host_val = n.to_host();  // 编译器保证不会忘记转换
```

### 12.4 深入扩展

**C++20 `std::endian` (可扩展方向)：**

```cpp
#include <bit>  // C++20
if constexpr (std::endian::native == std::endian::little) {
    // 小端系统
} else {
    // 大端系统
}
// 编译期确定, 零运行时开销
```

**GCC/Clang 内置字节序交换（比手写快）：**

```cpp
__builtin_bswap16(val);  // 1 条汇编指令 (BSWAP / REV)
__builtin_bswap32(val);
__builtin_bswap64(val);
// MSVC: _byteswap_ushort / _byteswap_ulong / _byteswap_uint64
```

---

# 五、网络服务篇

---

## 第 13 章：DNS 解析

### 13.1 getaddrinfo 封装

```cpp
// test7.cpp ch13 — DNS 解析
namespace dns {

struct ResolveResult {
    std::string hostname;
    std::vector<std::string> addresses;
    std::string canonical_name;  // CNAME
};

std::optional<ResolveResult> resolve(const std::string& hostname,
                                      int family = AF_UNSPEC) {
    addrinfo hints{};
    hints.ai_family = family;       // AF_UNSPEC = IPv4 + IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;  // 获取规范名

    addrinfo* result = nullptr;
    int ret = getaddrinfo(hostname.c_str(), nullptr, &hints, &result);
    if (ret != 0) return std::nullopt;

    auto guard = std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>(
        result, freeaddrinfo);

    // 遍历结果, inet_ntop 转换为字符串
    // 去重 (同一 IP 可能出现多次)
    return res;
}

} // namespace dns
```

### 13.2 深入扩展

**DNS 查询过程：**

```
应用 → getaddrinfo()
  → /etc/hosts (本地)
  → DNS 缓存 (nscd/systemd-resolved)
  → 递归 DNS 服务器 (ISP/8.8.8.8)
    → 根 DNS → .com DNS → example.com DNS
  → 缓存结果 (TTL)
```

**异步 DNS 解析：**
- `getaddrinfo()` 是**阻塞**的 → 在高性能服务器中可能成为瓶颈
- 解决方案:
  - c-ares 库 (异步 DNS)
  - 线程池中执行 getaddrinfo
  - Linux 4.18+ 的 `io_uring` 支持异步 DNS

---

## 第 14 章：超时与错误处理

### 14.1 带超时的连接

```cpp
// test7.cpp ch14 — connect_with_timeout
bool connect_with_timeout(Socket& sock, const SocketAddress& addr,
                          int timeout_ms) {
    sock.set_nonblocking(true);

    int ret = ::connect(sock.fd(), addr.data(), addr.size());
    if (ret == 0) { sock.set_nonblocking(false); return true; }

    // EINPROGRESS (POSIX) / WSAEWOULDBLOCK (Windows) = 连接进行中
    // 用 select 等待可写 (连接完成)
    SelectSet ss;
    ss.add_write(sock.fd());
    int ready = ss.wait(timeout_ms);
    if (ready <= 0) return false;  // 超时

    // 检查 SO_ERROR
    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(sock.fd(), SOL_SOCKET, SO_ERROR, &err, &len);

    sock.set_nonblocking(false);
    return err == 0;
}
```

### 14.2 NetResult\<T\> — Rust-like 错误处理

```cpp
// test7.cpp ch14
enum class NetError {
    Success, ConnectionRefused, Timeout,
    HostUnreachable, ConnectionReset, BrokenPipe, Unknown,
};

// 类似 Rust 的 Result<T, E>
template <typename T>
using NetResult = std::variant<T, NetError>;

template <typename T>
bool is_ok(const NetResult<T>& r) { return r.index() == 0; }

// 使用:
NetResult<std::string> data = receive_data();
if (is_ok(data))
    process(get_ok(data));
else
    log_error(get_err(data));
```

### 14.3 深入扩展

**常见网络错误码对照：**

| errno (POSIX) | WSA (Windows) | 含义 |
|---|---|---|
| `ECONNREFUSED` | `WSAECONNREFUSED` | 连接被拒绝（端口未监听） |
| `ETIMEDOUT` | `WSAETIMEDOUT` | 连接超时 |
| `ECONNRESET` | `WSAECONNRESET` | 对端 RST（异常关闭） |
| `EPIPE` | — | 写入已关闭的连接 |
| `EAGAIN`/`EWOULDBLOCK` | `WSAEWOULDBLOCK` | 非阻塞操作无数据 |
| `EINPROGRESS` | `WSAEWOULDBLOCK` | 非阻塞 connect 进行中 |

**C++23 std::expected 的展望：**

```cpp
// C++23
std::expected<std::string, NetError> receive_data() {
    // ...
    if (error) return std::unexpected(NetError::Timeout);
    return data;
}
```

---

## 第 15 章：多线程服务器 — Thread-per-Connection

### 15.1 模式

```
                        ┌──── Thread → handle(client1)
accept() ──────────────┤
                        ├──── Thread → handle(client2)
        (主线程循环)     │
                        └──── Thread → handle(client3)
```

### 15.2 实现

```cpp
// test7.cpp ch15 — ThreadedTcpServer
class ThreadedTcpServer {
    TcpServer server_;
    std::atomic<bool> running_{false};

public:
    using ConnectionHandler = std::function<void(Socket, SocketAddress)>;

    void start(uint16_t port, ConnectionHandler handler) {
        server_.start(port);
        running_ = true;

        while (running_) {
            auto result = server_.accept();
            if (!result) continue;

            // 每个连接一个线程
            auto addr = result->client_addr;
            std::thread([handler, sock = std::move(result->client_socket),
                         addr]() mutable {
                handler(std::move(sock), addr);
            }).detach();
        }
    }
};
```

### 15.3 优缺点

| 优点 | 缺点 |
|---|---|
| 实现简单 | 线程创建/销毁开销大 |
| 每个连接独立处理 | 线程数 = 连接数 → 万级时 OOM |
| 阻塞 IO 即可 | 上下文切换开销 |
| 调试容易 | 线程安全问题 |

### 15.4 深入扩展

**改进方向 — 线程池：**

```cpp
// 使用固定线程池替代 detach:
ThreadPool pool(std::thread::hardware_concurrency());

while (running_) {
    auto result = server_.accept();
    pool.submit([handler, sock = std::move(result->client_socket),
                 addr = result->client_addr]() mutable {
        handler(std::move(sock), addr);
    });
}
// 优势: 线程数固定, 无创建/销毁开销, 可控并发度
```

**并发模型对比：**

| 模型 | 连接数上限 | CPU 利用率 | 复杂度 |
|---|---|---|---|
| Thread-per-Connection | ~1000 | 低 (上下文切换) | 低 |
| 线程池 | ~10000 | 中 | 中 |
| Reactor (epoll) | ~100K+ | 高 | 高 |
| Proactor (IOCP/io_uring) | ~100K+ | 最高 | 最高 |

---

## 第 16 章：IO 多路复用服务器 — select 事件驱动

### 16.1 Reactor 模式

```
事件循环:
  ┌──→ select() 等待事件
  │      │
  │      ├── listen_fd 可读 → accept_new_client()
  │      │
  │      ├── client_fd 可读 → handle_client_data()
  │      │
  │      └── client_fd 可写 → send_pending_data()
  │
  └──── 继续循环
```

### 16.2 SelectServer 实现

```cpp
// test7.cpp ch16 — SelectServer
class SelectServer {
    TcpServer server_;
    std::vector<Socket> clients_;
    std::map<socket_t, NetworkBuffer> buffers_;
    MessageHandler handler_;

public:
    void start(uint16_t port) {
        server_.start(port);
        server_.listen_socket().set_nonblocking(true);

        while (running_) {
            SelectSet ss;
            ss.add_read(server_.listen_socket().fd());
            for (auto& client : clients_)
                ss.add_read(client.fd());

            int ready = ss.wait(100);  // 100ms 超时
            if (ready <= 0) continue;

            // 新连接
            if (ss.is_readable(server_.listen_socket().fd()))
                accept_new_client();

            // 客户端数据
            for (size_t i = 0; i < clients_.size(); ) {
                if (ss.is_readable(clients_[i].fd())) {
                    if (!handle_client_data(i)) {
                        remove_client(i);
                        continue;
                    }
                }
                ++i;
            }
        }
    }
};
```

### 16.3 单线程 vs 多线程事件驱动

| | 单线程 Reactor | 多线程 Reactor |
|---|---|---|
| 线程数 | 1 (main loop) | 1 (acceptor) + N (worker) |
| 适用 | IO 密集 | IO + CPU 混合 |
| 复杂度 | 低 | 中 |
| 代表 | Redis | Nginx, Netty |

### 16.4 深入扩展

**One-loop-per-thread 模式 (Muduo 风格)：**

```
Main Thread (Acceptor):
  epoll_wait → accept → 分发给 Worker Thread

Worker Thread 1:              Worker Thread 2:
  epoll_wait → read/write       epoll_wait → read/write
  (处理 client 1,3,5...)       (处理 client 2,4,6...)
```

- 每个线程有自己的 epoll → 无锁
- 连接 round-robin 分配给 worker → 负载均衡
- 这是高性能网络框架的主流模式

---

# 六、实战篇

---

## 第 17 章：简易聊天室

### 17.1 功能设计

```
客户端连接 → 分配昵称 (User_<fd>)
  /nick <新名字>  → 改名
  /quit           → 退出
  其他消息        → 广播给所有人
```

### 17.2 核心实现

```cpp
// test7.cpp ch17 — ChatServer
class ChatServer {
    TcpServer server_;
    std::mutex clients_mtx_;
    std::map<socket_t, std::string> clients_;  // fd → nickname

    void handle_client(Socket sock, const std::string& addr) {
        auto fd = sock.fd();
        // ... 进入循环读取消息
        while (auto line = buf.read_line()) {
            if (line->substr(0, 5) == "/nick") {
                // 改名
            } else if (*line == "/quit") {
                break;
            } else {
                broadcast("[" + nickname + "] " + *line + "\r\n", fd);
            }
        }
    }

    void broadcast(const std::string& msg, socket_t exclude_fd) {
        std::lock_guard<std::mutex> lock(clients_mtx_);
        for (auto& [fd, _] : clients_) {
            if (fd != exclude_fd)
                ::send(fd, msg.data(), msg.size(), MSG_NOSIGNAL);
        }
    }
};
```

### 17.3 运行方式

```bash
# 终端 1: 启动服务器
./test7     # 选择 4 (聊天服务器)

# 终端 2: 客户端 A
telnet localhost 9999

# 终端 3: 客户端 B
telnet localhost 9999
```

### 17.4 深入扩展

**当前实现的线程安全问题：**

| 问题 | 现状 | 改进 |
|---|---|---|
| 广播时直接 send | 可能阻塞（对端慢） | 每客户端写队列 + 异步发送 |
| clients_mtx_ 粒度 | 全局锁，广播时持锁 send | 分离发送操作，减少临界区 |
| detach 线程 | 无法 join，无法优雅关闭 | 线程池 + 停止信号 |

**生产级聊天室架构：**

```
                    ┌──────────────────┐
                    │   Load Balancer  │
                    └──────┬───────────┘
               ┌───────────┼───────────┐
         ┌─────▼─────┐  ┌──▼──────┐ ┌──▼──────┐
         │  Server 1  │  │ Server 2│ │ Server 3│
         └─────┬──────┘  └────┬────┘ └────┬────┘
               └──────────┬───┘           │
                    ┌─────▼────────────────▼─┐
                    │     Redis Pub/Sub       │ ← 跨节点广播
                    └────────────────────────┘
```

---

## 第 18 章：Redis 协议客户端 (RESP)

### 18.1 RESP 协议格式

```
类型:
  +OK\r\n                          ← 简单字符串
  -ERR message\r\n                 ← 错误
  :1000\r\n                        ← 整数
  $6\r\nfoobar\r\n                 ← 批量字符串 ($长度\r\n数据\r\n)
  $-1\r\n                          ← NULL
  *2\r\n$3\r\nGET\r\n$3\r\nkey\r\n ← 数组

命令编码 (发送给 Redis):
  SET key value → *3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$5\r\nvalue\r\n
```

### 18.2 RedisClient 实现

```cpp
// test7.cpp ch18 — RedisClient
class RedisClient {
    TcpClient client_;
    NetworkBuffer buf_;

public:
    void connect(const char* host = "127.0.0.1", uint16_t port = 6379);

    // 通用命令
    RespValue command(std::initializer_list<std::string_view> args) {
        // 编码为 RESP 数组
        std::string cmd;
        cmd += "*" + std::to_string(args.size()) + "\r\n";
        for (auto& arg : args) {
            cmd += "$" + std::to_string(arg.size()) + "\r\n";
            cmd += std::string(arg) + "\r\n";
        }
        client_.send_all(cmd);
        return read_response();
    }

    // 便捷方法
    RespValue set(std::string_view key, std::string_view value);
    RespValue get(std::string_view key);
    RespValue del(std::string_view key);
    RespValue ping();
};
```

### 18.3 RESP 值类型

```cpp
struct RespNull {};
struct RespError { std::string message; };

using RespValue = std::variant<
    RespNull,       // $-1 → nil
    std::string,    // +str / $N → string
    int64_t,        // :N → integer
    RespError       // -ERR → error
>;
```

### 18.4 深入扩展

**Redis Pipeline — 批量命令优化：**

```
普通模式: 发送 → 等待响应 → 发送 → 等待响应 (RTT × N)
Pipeline: 发送N条 → 等待N个响应 (RTT × 1)

// 实现: 连续发送多条 RESP, 然后连续读取多个响应
for (auto& cmd : commands) client.send_all(cmd);
for (int i = 0; i < N; ++i) results.push_back(read_response());
```

**连接池设计：**

```cpp
class RedisPool {
    std::queue<std::unique_ptr<RedisClient>> pool_;
    std::mutex mtx_;

    std::unique_ptr<RedisClient> acquire() {
        std::lock_guard lock(mtx_);
        if (pool_.empty()) return create_new();
        auto c = std::move(pool_.front());
        pool_.pop();
        return c;
    }

    void release(std::unique_ptr<RedisClient> c) {
        std::lock_guard lock(mtx_);
        pool_.push(std::move(c));
    }
};
```

---

## 第 19 章：网络编程最佳实践与调试

### 19.1 最佳实践清单

| 类别 | ✅ 推荐 | ❌ 避免 |
|---|---|---|
| **资源管理** | RAII 封装 socket | 裸 socket_t + 手动 close |
| **错误处理** | 检查每个系统调用返回值 | 忽略错误码 |
| **缓冲区** | NetworkBuffer + send_all | 假设 send/recv 一次完成 |
| **协议设计** | 长度前缀 / 分隔符 | 假设 TCP 保持消息边界 |
| **性能** | TCP_NODELAY + 非阻塞 IO | Thread-per-Connection (万级) |
| **安全** | 验证输入长度 + 超时 | 信任客户端数据 |

### 19.2 常见 Bug 模式

**Bug 1: TCP 粘包**
```
TCP 是字节流，不保留消息边界:
  send("AB") + send("CD") → 对方可能收到 "ABCD" 或 "A"+"BCD" 或...
  
解决: 长度前缀 或 分隔符 (\r\n)
```

**Bug 2: 部分发送**
```
send(10000 字节) → 实际可能只发 5000
解决: send_all 循环发送
```

**Bug 3: SIGPIPE (Unix)**
```
写入已关闭 socket → 进程被杀
解决: signal(SIGPIPE, SIG_IGN) 或 MSG_NOSIGNAL
```

**Bug 4: TIME_WAIT bind 失败**
```
服务器重启 → "Address already in use"
解决: SO_REUSEADDR
```

**Bug 5: 字节序**
```
直接发送主机序整数 → 大小端不同的机器收到错误值
解决: htonl/htons
```

### 19.3 调试工具

| 工具 | 用途 | 命令示例 |
|---|---|---|
| **Wireshark** | 图形化抓包 | — |
| **tcpdump** | 命令行抓包 | `tcpdump -i any port 8080 -X` |
| **netstat/ss** | 查看连接状态 | `ss -tlnp \| grep 8080` |
| **netcat (nc)** | TCP/UDP 测试 | `nc -l -p 9999` (服务端) |
| **curl** | HTTP 测试 | `curl -v http://localhost:8080/` |
| **strace** | 系统调用跟踪 | `strace -e trace=network ./test7` |

### 19.4 性能调优参数

| 参数 | 说明 | 推荐 |
|---|---|---|
| `TCP_NODELAY` | 禁用 Nagle 算法 | 低延迟场景开启 |
| `SO_REUSEADDR` | 地址复用 | 服务器**总是**开启 |
| `SO_KEEPALIVE` | TCP 保活 | 长连接开启 |
| `SO_RCVBUF` / `SO_SNDBUF` | 收发缓冲区 | 根据带宽调整 |
| `SO_LINGER` | 关闭行为 | 按需设置 |
| `TCP_QUICKACK` | 快速 ACK (Linux) | 低延迟场景 |
| `TCP_FASTOPEN` | TFO (Linux 3.7+) | 减少握手延迟 |

---

## 附录 A：Socket 类型速查

| 类型 | 常量 | 协议 | 特点 |
|---|---|---|---|
| 流式 Socket | `SOCK_STREAM` | TCP | 可靠、有序、字节流 |
| 数据报 Socket | `SOCK_DGRAM` | UDP | 不可靠、无序、有边界 |
| 原始 Socket | `SOCK_RAW` | IP | 底层访问（需要 root） |

## 附录 B：TCP/UDP 连接流程

```
TCP 服务端: socket → bind → listen → accept → recv/send → close
TCP 客户端: socket → connect → send/recv → close
UDP 服务端: socket → bind → recvfrom/sendto → close
UDP 客户端: socket → sendto/recvfrom → close
```

## 附录 C：IO 模型对比

| 模型 | 函数 | 特点 | 适用 |
|---|---|---|---|
| 阻塞 IO | recv (默认) | 简单，效率低 | 客户端/少连接 |
| 非阻塞 IO | recv + O_NONBLOCK | 需要轮询 | 配合 select |
| IO 多路复用 | select/poll/epoll | 高效 | 服务器 |
| 信号驱动 IO | SIGIO | 复杂 | 少见 |
| 异步 IO | io_uring / IOCP | 最高效 | 高性能服务器 |

## 附录 D：地址族

| 常量 | 说明 |
|---|---|
| `AF_INET` | IPv4 |
| `AF_INET6` | IPv6 |
| `AF_UNIX` | Unix 域 Socket (IPC) |
| `AF_UNSPEC` | 不指定 (IPv4/IPv6 均可) |

---

## 运行输出示例

```
===== 现代 C++ 网络编程教程 =====

选择演示模式:
  1 - 基础演示（DNS/Buffer/协议/字节序）
  2 - TCP Echo 服务器 (端口 9999)
  3 - HTTP 服务器 (端口 8080)
  4 - 聊天服务器 (端口 9999)

请输入 (1-4): 1

[DNS 解析]
  localhost → 127.0.0.1
  example.com → 93.184.216.34, 2606:2800:220:1:...
  google.com → 142.250.80.46, 2607:f8b0:4004:...

[Buffer 管理]
  Buffer 可读: 38 字节
  Line 1: [GET / HTTP/1.1]
  Line 2: [Host: example.com]
  Line 3: []

[二进制协议]
  Packet 大小: 21 字节
  Header: 8 字节, Payload: 13 字节
  解码: type=1, user_id=12345, msg="Hello!"

[字节序]
  系统字节序: Little-Endian
  端口 8080 主机序: 0x1f90 → 网络序: 0x901f
  0x12345678 → 网络序 → 还原: 0x12345678

===== 演示完成 =====
```

---

## 扩展方向建议

以下是 `test7.cpp` 可进一步扩展的方向：

| 方向 | 说明 | 优先级 |
|---|---|---|
| **epoll 封装** | Linux 高性能 IO 多路复用 | ⭐⭐⭐⭐⭐ |
| **HTTPS / TLS** | 集成 OpenSSL，支持安全通信 | ⭐⭐⭐⭐⭐ |
| **线程池服务器** | 替代 Thread-per-Connection | ⭐⭐⭐⭐ |
| **HTTP keep-alive** | 连接复用 + Content-Length/chunked | ⭐⭐⭐⭐ |
| **WebSocket** | 升级握手 + 帧协议 | ⭐⭐⭐⭐ |
| **连接池** | Redis/DB 连接的复用与管理 | ⭐⭐⭐⭐ |
| **io_uring** | Linux 异步 IO (最高性能) | ⭐⭐⭐⭐ |
| **HTTP/2** | 二进制帧、头部压缩、多路复用 | ⭐⭐⭐ |
| **信号量优雅退出** | SIGINT/SIGTERM 处理 | ⭐⭐⭐ |
| **零拷贝 sendfile** | 静态文件服务 | ⭐⭐⭐ |
| **Protobuf 集成** | 替代自定义二进制协议 | ⭐⭐⭐ |
| **Unix Domain Socket** | 本地 IPC 通信 | ⭐⭐ |

---

> **核心原则**: 网络编程的本质是**管理异步状态**——连接随时可能断开、数据随时可能只到一半、多个客户端同时竞争资源。用 RAII 管理资源、用 Buffer 管理数据边界、用事件循环管理并发，才能写出健壮的网络程序。记住：**TCP 是字节流，不是消息流**——这是网络编程中最常见的误解。
