// =============================================================================
// 现代 C++ 网络编程 完全指导教程与代码示例
// =============================================================================
// C++ 标准库目前没有网络库（Networking TS 尚未进入标准），
// 但我们可以用现代 C++ 封装系统 API，实现零开销、类型安全的网络编程。
//
// 本教程在 Windows Winsock2 / POSIX Socket 上构建，
// 用 RAII、模板、variant、optional 等现代手法做封装。
//
// 编译：
//   Windows (MSVC):
//     cl /std:c++17 /O2 /EHsc test7.cpp /link ws2_32.lib
//   Windows (MinGW):
//     g++ -std=c++17 -O2 -o test7 test7.cpp -lws2_32
//   Linux / Mac:
//     g++ -std=c++17 -O2 -pthread -o test7 test7.cpp
//
// 目录：
//   1.  平台抽象层 — 统一 Winsock / POSIX
//   2.  Socket 基础 — RAII 封装
//   3.  地址封装 — IPv4 / IPv6 统一处理
//   4.  TCP 客户端 — 连接与收发
//   5.  TCP 服务器 — 监听与接受连接
//   6.  UDP 通信 — 无连接数据报
//   7.  非阻塞 I/O 与 select 多路复用
//   8.  Buffer 管理 — 零拷贝缓冲设计
//   9.  HTTP 客户端 — 手写 HTTP/1.1 请求
//  10.  HTTP 服务器 — 简易 Web Server
//  11.  协议序列化 — 二进制协议编解码
//  12.  网络字节序与类型安全转换
//  13.  DNS 解析
//  14.  超时与错误处理
//  15.  多线程服务器 — Thread-per-Connection
//  16.  IO 多路复用服务器 — select/poll 事件驱动
//  17.  实战：简易聊天室
//  18.  实战：Redis 协议客户端 (RESP)
//  19.  网络编程最佳实践与调试
// =============================================================================

// ========================
// 平台头文件
// ========================
#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <poll.h>
    #include <errno.h>
    #include <signal.h>
#endif

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <memory>
#include <optional>
#include <variant>
#include <functional>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <map>
#include <unordered_map>
#include <queue>
#include <stdexcept>
#include <system_error>
#include <type_traits>


// =============================================================================
// 第1章：平台抽象层 — 统一 Winsock / POSIX
// =============================================================================
//
// Windows 使用 Winsock2 (SOCKET = unsigned, INVALID_SOCKET, closesocket)
// POSIX 使用 int 文件描述符 (fd, -1 为无效, close)
// 我们创建统一的类型和宏。

namespace net {

// --- 1.1 平台类型统一 ---

#ifdef _WIN32
    using socket_t = SOCKET;
    constexpr socket_t INVALID_SOCK = INVALID_SOCKET;

    inline int close_socket(socket_t s) { return closesocket(s); }
    inline int get_last_error() { return WSAGetLastError(); }

    // Winsock 没有 SIGPIPE
    #define MSG_NOSIGNAL 0
#else
    using socket_t = int;
    constexpr socket_t INVALID_SOCK = -1;

    inline int close_socket(socket_t s) { return close(s); }
    inline int get_last_error() { return errno; }
#endif

// --- 1.2 Winsock 初始化 (RAII) ---
//
// Windows 需要在使用 socket 前调用 WSAStartup，
// 程序结束时调用 WSACleanup。

class WinsockInit {
#ifdef _WIN32
    bool initialized_ = false;
#endif
public:
    WinsockInit() {
#ifdef _WIN32
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            throw std::runtime_error(
                "WSAStartup failed: " + std::to_string(result));
        }
        initialized_ = true;
        std::cout << "  [Winsock] 初始化成功 (v"
                  << (int)LOBYTE(wsaData.wVersion) << "."
                  << (int)HIBYTE(wsaData.wVersion) << ")\n";
#else
        // POSIX: 忽略 SIGPIPE（写入已关闭的 socket 时不崩溃）
        signal(SIGPIPE, SIG_IGN);
#endif
    }

    ~WinsockInit() {
#ifdef _WIN32
        if (initialized_) {
            WSACleanup();
            std::cout << "  [Winsock] 已清理\n";
        }
#endif
    }

    WinsockInit(const WinsockInit&) = delete;
    WinsockInit& operator=(const WinsockInit&) = delete;
};

// 使用方法：在 main 开头创建一个实例
// WinsockInit wsa; // RAII 自动初始化和清理


// =============================================================================
// 第2章：Socket 基础 — RAII 封装
// =============================================================================
//
// 裸 socket 与裸指针一样危险：
//   - 忘记 close → 资源泄漏
//   - 重复 close → 未定义行为
//   - 异常时泄漏
//
// 用 RAII 封装既安全又零开销。

// --- 2.1 Socket RAII 类 ---

class Socket {
    socket_t fd_ = INVALID_SOCK;

public:
    // 默认构造：无 socket
    Socket() = default;

    // 从裸 fd 构造（获取所有权）
    explicit Socket(socket_t fd) : fd_(fd) {}

    // 创建 socket
    Socket(int domain, int type, int protocol = 0) {
        fd_ = ::socket(domain, type, protocol);
        if (fd_ == INVALID_SOCK) {
            throw std::system_error(get_last_error(),
                std::system_category(), "socket() failed");
        }
    }

    // 禁止拷贝
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // 移动语义
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

    // 析构：自动关闭
    ~Socket() { close(); }

    void close() {
        if (fd_ != INVALID_SOCK) {
            close_socket(fd_);
            fd_ = INVALID_SOCK;
        }
    }

    // 访问底层句柄
    socket_t fd() const { return fd_; }
    socket_t release() {
        socket_t tmp = fd_;
        fd_ = INVALID_SOCK;
        return tmp;
    }

    bool valid() const { return fd_ != INVALID_SOCK; }
    explicit operator bool() const { return valid(); }

    // --- Socket 选项 ---

    void set_reuse_addr(bool enable = true) {
        int opt = enable ? 1 : 0;
        if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR,
                       reinterpret_cast<const char*>(&opt), sizeof(opt)) < 0) {
            throw std::system_error(get_last_error(),
                std::system_category(), "setsockopt(SO_REUSEADDR)");
        }
    }

    void set_nonblocking(bool enable = true) {
#ifdef _WIN32
        u_long mode = enable ? 1 : 0;
        if (ioctlsocket(fd_, FIONBIO, &mode) != 0) {
            throw std::system_error(get_last_error(),
                std::system_category(), "ioctlsocket(FIONBIO)");
        }
#else
        int flags = fcntl(fd_, F_GETFL, 0);
        if (flags == -1) {
            throw std::system_error(errno, std::system_category(), "fcntl(F_GETFL)");
        }
        flags = enable ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
        if (fcntl(fd_, F_SETFL, flags) == -1) {
            throw std::system_error(errno, std::system_category(), "fcntl(F_SETFL)");
        }
#endif
    }

    void set_tcp_nodelay(bool enable = true) {
        int opt = enable ? 1 : 0;
        if (setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY,
                       reinterpret_cast<const char*>(&opt), sizeof(opt)) < 0) {
            throw std::system_error(get_last_error(),
                std::system_category(), "setsockopt(TCP_NODELAY)");
        }
    }

    void set_recv_timeout(int milliseconds) {
#ifdef _WIN32
        DWORD tv = milliseconds;
        setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO,
                   reinterpret_cast<const char*>(&tv), sizeof(tv));
#else
        timeval tv;
        tv.tv_sec = milliseconds / 1000;
        tv.tv_usec = (milliseconds % 1000) * 1000;
        setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO,
                   reinterpret_cast<const char*>(&tv), sizeof(tv));
#endif
    }

    // --- 基础 I/O ---

    int send_data(const void* data, int len, int flags = 0) const {
        return ::send(fd_, static_cast<const char*>(data), len, flags | MSG_NOSIGNAL);
    }

    int send_string(std::string_view str, int flags = 0) const {
        return send_data(str.data(), static_cast<int>(str.size()), flags);
    }

    int recv_data(void* buf, int len, int flags = 0) const {
        return ::recv(fd_, static_cast<char*>(buf), len, flags);
    }

    // 接收到 string
    std::optional<std::string> recv_string(int max_len = 4096) const {
        std::string buf(max_len, '\0');
        int n = recv_data(buf.data(), max_len);
        if (n <= 0) return std::nullopt;
        buf.resize(n);
        return buf;
    }
};


// =============================================================================
// 第3章：地址封装 — IPv4 / IPv6 统一处理
// =============================================================================

class SocketAddress {
    sockaddr_storage storage_{};
    socklen_t len_ = 0;

public:
    SocketAddress() = default;

    // 从 IPv4 地址和端口创建
    static SocketAddress from_ipv4(const char* ip, uint16_t port) {
        SocketAddress addr;
        auto* sin = reinterpret_cast<sockaddr_in*>(&addr.storage_);
        sin->sin_family = AF_INET;
        sin->sin_port = htons(port);
        if (inet_pton(AF_INET, ip, &sin->sin_addr) != 1) {
            throw std::runtime_error(std::string("Invalid IPv4: ") + ip);
        }
        addr.len_ = sizeof(sockaddr_in);
        return addr;
    }

    // 从 IPv6 地址和端口创建
    static SocketAddress from_ipv6(const char* ip, uint16_t port) {
        SocketAddress addr;
        auto* sin6 = reinterpret_cast<sockaddr_in6*>(&addr.storage_);
        sin6->sin6_family = AF_INET6;
        sin6->sin6_port = htons(port);
        if (inet_pton(AF_INET6, ip, &sin6->sin6_addr) != 1) {
            throw std::runtime_error(std::string("Invalid IPv6: ") + ip);
        }
        addr.len_ = sizeof(sockaddr_in6);
        return addr;
    }

    // 任意地址（用于 bind）
    static SocketAddress any_ipv4(uint16_t port) {
        SocketAddress addr;
        auto* sin = reinterpret_cast<sockaddr_in*>(&addr.storage_);
        sin->sin_family = AF_INET;
        sin->sin_port = htons(port);
        sin->sin_addr.s_addr = INADDR_ANY;
        addr.len_ = sizeof(sockaddr_in);
        return addr;
    }

    // 获取底层结构
    sockaddr* data() { return reinterpret_cast<sockaddr*>(&storage_); }
    const sockaddr* data() const { return reinterpret_cast<const sockaddr*>(&storage_); }
    socklen_t size() const { return len_; }
    socklen_t* size_ptr() { return &len_; }

    int family() const { return storage_.ss_family; }

    uint16_t port() const {
        if (storage_.ss_family == AF_INET)
            return ntohs(reinterpret_cast<const sockaddr_in*>(&storage_)->sin_port);
        if (storage_.ss_family == AF_INET6)
            return ntohs(reinterpret_cast<const sockaddr_in6*>(&storage_)->sin6_port);
        return 0;
    }

    std::string ip() const {
        char buf[INET6_ADDRSTRLEN] = {};
        if (storage_.ss_family == AF_INET) {
            auto* sin = reinterpret_cast<const sockaddr_in*>(&storage_);
            inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf));
        } else if (storage_.ss_family == AF_INET6) {
            auto* sin6 = reinterpret_cast<const sockaddr_in6*>(&storage_);
            inet_ntop(AF_INET6, &sin6->sin6_addr, buf, sizeof(buf));
        }
        return std::string(buf);
    }

    std::string to_string() const {
        return ip() + ":" + std::to_string(port());
    }
};


// =============================================================================
// 第4章：TCP 客户端 — 连接与收发
// =============================================================================

class TcpClient {
    Socket socket_;

public:
    TcpClient() = default;

    // 连接到服务器
    void connect(const char* host, uint16_t port) {
        // 使用 getaddrinfo 解析主机名
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;     // IPv4 或 IPv6
        hints.ai_socktype = SOCK_STREAM; // TCP
        hints.ai_protocol = IPPROTO_TCP;

        addrinfo* result = nullptr;
        std::string port_str = std::to_string(port);

        int ret = getaddrinfo(host, port_str.c_str(), &hints, &result);
        if (ret != 0) {
            throw std::runtime_error(
                std::string("getaddrinfo failed: ") + gai_strerror(ret));
        }

        // RAII guard for addrinfo
        auto guard = std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>(
            result, freeaddrinfo);

        // 尝试连接（可能有多个地址）
        for (auto* ai = result; ai != nullptr; ai = ai->ai_next) {
            try {
                socket_ = Socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
                if (::connect(socket_.fd(), ai->ai_addr,
                             static_cast<int>(ai->ai_addrlen)) == 0) {
                    return; // 连接成功
                }
            } catch (...) {
                continue;
            }
        }

        throw std::runtime_error("Failed to connect to " +
                                  std::string(host) + ":" + port_str);
    }

    // 发送所有数据（处理部分发送）
    bool send_all(std::string_view data) {
        size_t total_sent = 0;
        while (total_sent < data.size()) {
            int sent = socket_.send_data(
                data.data() + total_sent,
                static_cast<int>(data.size() - total_sent));
            if (sent <= 0) return false;
            total_sent += sent;
        }
        return true;
    }

    // 接收数据
    std::optional<std::string> receive(int max_len = 4096) {
        return socket_.recv_string(max_len);
    }

    // 接收直到连接关闭
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

    Socket& socket() { return socket_; }
    const Socket& socket() const { return socket_; }
};

// 使用示例（代码片段，不在 main 中运行）：
//
//   TcpClient client;
//   client.connect("example.com", 80);
//   client.send_all("GET / HTTP/1.1\r\nHost: example.com\r\n\r\n");
//   auto response = client.receive_all();
//   std::cout << response << "\n";


// =============================================================================
// 第5章：TCP 服务器 — 监听与接受连接
// =============================================================================

class TcpServer {
    Socket listen_socket_;
    uint16_t port_ = 0;

public:
    TcpServer() = default;

    void start(uint16_t port, int backlog = 128) {
        port_ = port;

        listen_socket_ = Socket(AF_INET, SOCK_STREAM);
        listen_socket_.set_reuse_addr();

        auto addr = SocketAddress::any_ipv4(port);
        if (::bind(listen_socket_.fd(), addr.data(),
                   static_cast<int>(addr.size())) < 0) {
            throw std::system_error(get_last_error(),
                std::system_category(), "bind failed");
        }

        if (::listen(listen_socket_.fd(), backlog) < 0) {
            throw std::system_error(get_last_error(),
                std::system_category(), "listen failed");
        }
    }

    // 等待并接受一个连接
    struct AcceptResult {
        Socket client_socket;
        SocketAddress client_addr;
    };

    std::optional<AcceptResult> accept() {
        SocketAddress client_addr;
        socklen_t addr_len = sizeof(sockaddr_storage);

        socket_t client_fd = ::accept(listen_socket_.fd(),
                                       client_addr.data(),
                                       &addr_len);
        if (client_fd == INVALID_SOCK) {
            if (get_last_error() == EWOULDBLOCK ||
                get_last_error() == EAGAIN)
                return std::nullopt; // 非阻塞模式下无连接
            throw std::system_error(get_last_error(),
                std::system_category(), "accept failed");
        }

        AcceptResult result;
        result.client_socket = Socket(client_fd);
        result.client_addr = client_addr;
        return result;
    }

    Socket& listen_socket() { return listen_socket_; }
    uint16_t port() const { return port_; }
};


// =============================================================================
// 第6章：UDP 通信 — 无连接数据报
// =============================================================================

class UdpSocket {
    Socket socket_;

public:
    UdpSocket() : socket_(AF_INET, SOCK_DGRAM) {}

    void bind(uint16_t port) {
        auto addr = SocketAddress::any_ipv4(port);
        if (::bind(socket_.fd(), addr.data(),
                   static_cast<int>(addr.size())) < 0) {
            throw std::system_error(get_last_error(),
                std::system_category(), "UDP bind failed");
        }
    }

    // 发送到指定地址
    int send_to(std::string_view data, const SocketAddress& dest) {
        return ::sendto(socket_.fd(), data.data(),
                        static_cast<int>(data.size()), 0,
                        dest.data(), static_cast<int>(dest.size()));
    }

    // 接收，并获取发送者地址
    struct RecvResult {
        std::string data;
        SocketAddress sender;
    };

    std::optional<RecvResult> recv_from(int max_len = 65536) {
        RecvResult result;
        result.data.resize(max_len);

        socklen_t addr_len = sizeof(sockaddr_storage);
        int n = ::recvfrom(socket_.fd(), result.data.data(), max_len, 0,
                           result.sender.data(), &addr_len);
        if (n <= 0) return std::nullopt;
        result.data.resize(n);
        return result;
    }

    Socket& socket() { return socket_; }
};

// UDP 使用示例（代码片段）：
//
// 服务器端：
//   UdpSocket server;
//   server.bind(9999);
//   auto msg = server.recv_from();
//   if (msg) {
//       std::cout << "From " << msg->sender.to_string()
//                 << ": " << msg->data << "\n";
//       server.send_to("ACK", msg->sender);
//   }
//
// 客户端：
//   UdpSocket client;
//   auto dest = SocketAddress::from_ipv4("127.0.0.1", 9999);
//   client.send_to("Hello UDP!", dest);


// =============================================================================
// 第7章：非阻塞 I/O 与 select 多路复用
// =============================================================================

// --- 7.1 select 封装 ---
//
// select() 可以同时监视多个 socket 的读/写/异常事件。
// 比 poll/epoll 可移植性更好（Windows/Linux/Mac 全支持）。

class SelectSet {
    fd_set read_set_, write_set_, except_set_;
    socket_t max_fd_ = 0;

public:
    SelectSet() {
        FD_ZERO(&read_set_);
        FD_ZERO(&write_set_);
        FD_ZERO(&except_set_);
    }

    void add_read(socket_t fd) {
        FD_SET(fd, &read_set_);
        update_max(fd);
    }

    void add_write(socket_t fd) {
        FD_SET(fd, &write_set_);
        update_max(fd);
    }

    void add_except(socket_t fd) {
        FD_SET(fd, &except_set_);
        update_max(fd);
    }

    bool is_readable(socket_t fd) const { return FD_ISSET(fd, &read_set_); }
    bool is_writable(socket_t fd) const { return FD_ISSET(fd, &write_set_); }
    bool has_error(socket_t fd) const { return FD_ISSET(fd, &except_set_); }

    // 等待事件，返回就绪的 fd 数量
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

private:
    void update_max(socket_t fd) {
        if (fd > max_fd_) max_fd_ = fd;
    }
};


// =============================================================================
// 第8章：Buffer 管理 — 网络缓冲设计
// =============================================================================

// 网络读写经常需要缓冲区。
// 好的缓冲区设计减少拷贝、简化协议解析。

class NetworkBuffer {
    std::vector<char> data_;
    size_t read_pos_ = 0;  // 已读位置
    size_t write_pos_ = 0; // 已写位置

public:
    explicit NetworkBuffer(size_t initial_size = 4096)
        : data_(initial_size) {}

    // 可写区域
    char* write_ptr() { return data_.data() + write_pos_; }
    size_t writable_bytes() const { return data_.size() - write_pos_; }

    // 可读区域
    const char* read_ptr() const { return data_.data() + read_pos_; }
    size_t readable_bytes() const { return write_pos_ - read_pos_; }

    // 写入（已接收数据后调用）
    void advance_write(size_t n) {
        assert(write_pos_ + n <= data_.size());
        write_pos_ += n;
    }

    // 读出（已处理数据后调用）
    void advance_read(size_t n) {
        assert(read_pos_ + n <= write_pos_);
        read_pos_ += n;
    }

    // 追加数据
    void append(const void* data, size_t len) {
        ensure_writable(len);
        std::memcpy(write_ptr(), data, len);
        advance_write(len);
    }

    void append(std::string_view sv) {
        append(sv.data(), sv.size());
    }

    // 读出字符串
    std::string_view peek(size_t n) const {
        n = std::min(n, readable_bytes());
        return {read_ptr(), n};
    }

    std::string read_string(size_t n) {
        auto sv = peek(n);
        std::string result(sv);
        advance_read(sv.size());
        return result;
    }

    // 查找分隔符（用于协议解析）
    std::optional<size_t> find(char delimiter) const {
        for (size_t i = read_pos_; i < write_pos_; ++i) {
            if (data_[i] == delimiter)
                return i - read_pos_;
        }
        return std::nullopt;
    }

    std::optional<size_t> find(std::string_view pattern) const {
        if (pattern.empty() || readable_bytes() < pattern.size())
            return std::nullopt;
        auto sv = std::string_view(read_ptr(), readable_bytes());
        auto pos = sv.find(pattern);
        if (pos != std::string_view::npos) return pos;
        return std::nullopt;
    }

    // 读取一行（\r\n 分隔）
    std::optional<std::string> read_line() {
        auto pos = find("\r\n");
        if (!pos) return std::nullopt;
        std::string line = read_string(*pos);
        advance_read(2); // 跳过 \r\n
        return line;
    }

    // 紧凑：将未读数据移到开头
    void compact() {
        if (read_pos_ > 0) {
            size_t readable = readable_bytes();
            std::memmove(data_.data(), read_ptr(), readable);
            read_pos_ = 0;
            write_pos_ = readable;
        }
    }

    void clear() { read_pos_ = write_pos_ = 0; }

    // 从 socket 读入缓冲区
    int read_from(const Socket& sock) {
        compact();
        ensure_writable(4096);
        int n = sock.recv_data(write_ptr(), static_cast<int>(writable_bytes()));
        if (n > 0) advance_write(n);
        return n;
    }

private:
    void ensure_writable(size_t n) {
        if (writable_bytes() < n) {
            if (read_pos_ + writable_bytes() >= n) {
                compact();
            } else {
                data_.resize(write_pos_ + n);
            }
        }
    }
};


// =============================================================================
// 第9章：HTTP 客户端 — 手写 HTTP/1.1 请求
// =============================================================================

struct HttpResponse {
    int status_code = 0;
    std::string status_text;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

class HttpClient {
public:
    // 简单的 GET 请求
    static HttpResponse get(const std::string& host, uint16_t port,
                            const std::string& path) {
        TcpClient client;
        client.connect(host.c_str(), port);

        // 构建 HTTP 请求
        std::string request =
            "GET " + path + " HTTP/1.1\r\n"
            "Host: " + host + "\r\n"
            "Connection: close\r\n"
            "User-Agent: ModernCpp/1.0\r\n"
            "\r\n";

        client.send_all(request);

        // 接收完整响应
        std::string raw = client.receive_all();
        return parse_response(raw);
    }

    // POST 请求
    static HttpResponse post(const std::string& host, uint16_t port,
                             const std::string& path,
                             const std::string& content_type,
                             const std::string& body) {
        TcpClient client;
        client.connect(host.c_str(), port);

        std::string request =
            "POST " + path + " HTTP/1.1\r\n"
            "Host: " + host + "\r\n"
            "Connection: close\r\n"
            "Content-Type: " + content_type + "\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "\r\n" + body;

        client.send_all(request);
        return parse_response(client.receive_all());
    }

private:
    static HttpResponse parse_response(const std::string& raw) {
        HttpResponse resp;
        std::istringstream stream(raw);
        std::string line;

        // 状态行: HTTP/1.1 200 OK
        if (std::getline(stream, line)) {
            // 去掉 \r
            if (!line.empty() && line.back() == '\r') line.pop_back();

            auto sp1 = line.find(' ');
            auto sp2 = line.find(' ', sp1 + 1);
            if (sp1 != std::string::npos && sp2 != std::string::npos) {
                resp.status_code = std::stoi(line.substr(sp1 + 1, sp2 - sp1 - 1));
                resp.status_text = line.substr(sp2 + 1);
            }
        }

        // 头部
        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) break; // 空行 = 头部结束

            auto colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string value = line.substr(colon + 1);
                // 去掉前导空格
                if (!value.empty() && value[0] == ' ')
                    value = value.substr(1);
                resp.headers[key] = value;
            }
        }

        // Body
        std::ostringstream body_stream;
        body_stream << stream.rdbuf();
        resp.body = body_stream.str();

        return resp;
    }
};

// 使用示例：
//
//   auto resp = HttpClient::get("example.com", 80, "/");
//   std::cout << "Status: " << resp.status_code << "\n";
//   std::cout << "Body length: " << resp.body.size() << "\n";


// =============================================================================
// 第10章：HTTP 服务器 — 简易 Web Server
// =============================================================================

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

class HttpServer {
public:
    using Handler = std::function<HttpResponse(const HttpRequest&)>;

private:
    TcpServer server_;
    std::unordered_map<std::string, Handler> routes_;
    Handler default_handler_;
    std::atomic<bool> running_{false};

public:
    HttpServer() {
        // 默认 404 处理
        default_handler_ = [](const HttpRequest&) {
            HttpResponse resp;
            resp.status_code = 404;
            resp.status_text = "Not Found";
            resp.body = "<h1>404 Not Found</h1>";
            resp.headers["Content-Type"] = "text/html";
            return resp;
        };
    }

    // 注册路由
    void route(const std::string& path, Handler handler) {
        routes_[path] = std::move(handler);
    }

    // 启动服务器（阻塞）
    void start(uint16_t port) {
        server_.start(port);
        running_ = true;
        std::cout << "  HTTP Server 启动在端口 " << port << "\n";

        while (running_) {
            auto result = server_.accept();
            if (!result) continue;

            // 处理连接
            handle_connection(std::move(result->client_socket));
        }
    }

    void stop() { running_ = false; }

private:
    void handle_connection(Socket client) {
        NetworkBuffer buf;
        int n = buf.read_from(client);
        if (n <= 0) return;

        // 解析请求
        auto request = parse_request(buf);
        if (!request) return;

        // 查找路由
        Handler handler = default_handler_;
        auto it = routes_.find(request->path);
        if (it != routes_.end()) {
            handler = it->second;
        }

        // 执行处理器
        auto response = handler(*request);

        // 发送响应
        send_response(client, response);
    }

    static std::optional<HttpRequest> parse_request(NetworkBuffer& buf) {
        HttpRequest req;

        // 请求行
        auto line = buf.read_line();
        if (!line) return std::nullopt;

        std::istringstream iss(*line);
        iss >> req.method >> req.path >> req.version;

        // 头部
        while (auto header_line = buf.read_line()) {
            if (header_line->empty()) break;
            auto colon = header_line->find(':');
            if (colon != std::string::npos) {
                std::string key = header_line->substr(0, colon);
                std::string val = header_line->substr(colon + 1);
                if (!val.empty() && val[0] == ' ') val = val.substr(1);
                req.headers[key] = val;
            }
        }

        // Body (简化：读取剩余数据)
        if (buf.readable_bytes() > 0) {
            req.body = std::string(buf.read_ptr(), buf.readable_bytes());
        }

        return req;
    }

    static void send_response(const Socket& client, const HttpResponse& resp) {
        std::string raw;
        raw.reserve(1024 + resp.body.size());

        // 状态行
        raw += "HTTP/1.1 " + std::to_string(resp.status_code) + " "
               + resp.status_text + "\r\n";

        // 头部
        for (auto& [key, val] : resp.headers)
            raw += key + ": " + val + "\r\n";
        raw += "Content-Length: " + std::to_string(resp.body.size()) + "\r\n";
        raw += "Connection: close\r\n";
        raw += "\r\n";

        // Body
        raw += resp.body;

        // 发送
        size_t sent = 0;
        while (sent < raw.size()) {
            int n = client.send_data(raw.data() + sent,
                                      static_cast<int>(raw.size() - sent));
            if (n <= 0) break;
            sent += n;
        }
    }
};

// 使用示例：
//
// HttpServer server;
// server.route("/", [](const HttpRequest&) {
//     HttpResponse resp;
//     resp.status_code = 200;
//     resp.status_text = "OK";
//     resp.body = "<h1>Hello, Modern C++!</h1>";
//     resp.headers["Content-Type"] = "text/html";
//     return resp;
// });
// server.route("/api/time", [](const HttpRequest&) {
//     auto now = std::chrono::system_clock::now();
//     auto t = std::chrono::system_clock::to_time_t(now);
//     HttpResponse resp;
//     resp.status_code = 200;
//     resp.status_text = "OK";
//     resp.body = std::string("{\"time\":\"") + std::ctime(&t) + "\"}";
//     resp.headers["Content-Type"] = "application/json";
//     return resp;
// });
// server.start(8080);


// =============================================================================
// 第11章：协议序列化 — 二进制协议编解码
// =============================================================================
// 自定义协议通常用二进制格式以减少带宽和解析开销。
// 类型安全 + 网络字节序 + 零拷贝读写。

namespace protocol {

// --- 11.1 协议包格式 ---
// +--------+--------+----------+
// | Magic  |  Len   |  Payload |
// | 4 byte | 4 byte | Len byte |
// +--------+--------+----------+

constexpr uint32_t MAGIC = 0xDEADBEEF;

#pragma pack(push, 1)
struct PacketHeader {
    uint32_t magic;
    uint32_t payload_length;
};
#pragma pack(pop)

static_assert(sizeof(PacketHeader) == 8, "header must be 8 bytes");

// --- 11.2 类型安全的序列化器 ---

class PacketWriter {
    std::vector<char> buffer_;

public:
    PacketWriter() {
        // 预留 header 空间
        buffer_.resize(sizeof(PacketHeader));
    }

    // 写入基本类型（自动转网络字节序）
    void write_u8(uint8_t val) {
        buffer_.push_back(static_cast<char>(val));
    }

    void write_u16(uint16_t val) {
        uint16_t netval = htons(val);
        append(&netval, sizeof(netval));
    }

    void write_u32(uint32_t val) {
        uint32_t netval = htonl(val);
        append(&netval, sizeof(netval));
    }

    void write_string(std::string_view str) {
        write_u16(static_cast<uint16_t>(str.size()));
        append(str.data(), str.size());
    }

    // 完成：填充 header
    std::string_view finish() {
        uint32_t payload_len = static_cast<uint32_t>(
            buffer_.size() - sizeof(PacketHeader));

        PacketHeader* hdr = reinterpret_cast<PacketHeader*>(buffer_.data());
        hdr->magic = htonl(MAGIC);
        hdr->payload_length = htonl(payload_len);

        return {buffer_.data(), buffer_.size()};
    }

private:
    void append(const void* data, size_t len) {
        auto* bytes = static_cast<const char*>(data);
        buffer_.insert(buffer_.end(), bytes, bytes + len);
    }
};

class PacketReader {
    const char* data_;
    size_t size_;
    size_t pos_ = 0;

public:
    PacketReader(const char* data, size_t size)
        : data_(data), size_(size) {}

    bool read_u8(uint8_t& val) {
        if (pos_ + 1 > size_) return false;
        val = static_cast<uint8_t>(data_[pos_++]);
        return true;
    }

    bool read_u16(uint16_t& val) {
        if (pos_ + 2 > size_) return false;
        uint16_t netval;
        std::memcpy(&netval, data_ + pos_, 2);
        val = ntohs(netval);
        pos_ += 2;
        return true;
    }

    bool read_u32(uint32_t& val) {
        if (pos_ + 4 > size_) return false;
        uint32_t netval;
        std::memcpy(&netval, data_ + pos_, 4);
        val = ntohl(netval);
        pos_ += 4;
        return true;
    }

    bool read_string(std::string& val) {
        uint16_t len;
        if (!read_u16(len)) return false;
        if (pos_ + len > size_) return false;
        val.assign(data_ + pos_, len);
        pos_ += len;
        return true;
    }

    size_t remaining() const { return size_ - pos_; }
};

} // namespace protocol


// =============================================================================
// 第12章：网络字节序与类型安全转换
// =============================================================================
//
// 网络字节序 = Big-Endian
// x86/x64 = Little-Endian
// ARM 可能是任一种
//
// 始终使用 htons/htonl/ntohs/ntohl 转换。

namespace endian {

// --- 12.1 编译期字节序检测 ---

constexpr bool is_little_endian() {
    // C++20 可以用 std::endian
    // C++17 用运行时检测
    uint32_t value = 1;
    return *reinterpret_cast<const uint8_t*>(&value) == 1;
}

// --- 12.2 类型安全的字节序转换 ---

// 16位
constexpr uint16_t byte_swap_16(uint16_t val) {
    return (val >> 8) | (val << 8);
}

// 32位
constexpr uint32_t byte_swap_32(uint32_t val) {
    return ((val >> 24) & 0xFF)
         | ((val >>  8) & 0xFF00)
         | ((val <<  8) & 0xFF0000)
         | ((val << 24) & 0xFF000000);
}

// 64位
constexpr uint64_t byte_swap_64(uint64_t val) {
    return ((val >> 56) & 0xFF)
         | ((val >> 40) & 0xFF00)
         | ((val >> 24) & 0xFF0000)
         | ((val >>  8) & 0xFF000000ULL)
         | ((val <<  8) & 0xFF00000000ULL)
         | ((val << 24) & 0xFF0000000000ULL)
         | ((val << 40) & 0xFF000000000000ULL)
         | ((val << 56) & 0xFF00000000000000ULL);
}

// 编译期验证
static_assert(byte_swap_16(0x1234) == 0x3412, "16-bit swap");
static_assert(byte_swap_32(0x12345678) == 0x78563412, "32-bit swap");

// --- 12.3 强类型网络整数 ---

template <typename T>
class NetworkOrder {
    T value_; // 存储为网络字节序
public:
    // 从主机字节序构造
    static NetworkOrder from_host(T val);

    // 转回主机字节序
    T to_host() const;

    // 获取网络字节序原始值
    T raw() const { return value_; }

private:
    explicit NetworkOrder(T val) : value_(val) {}
};

template <>
NetworkOrder<uint16_t> NetworkOrder<uint16_t>::from_host(uint16_t val) {
    return NetworkOrder<uint16_t>(htons(val));
}
template <>
uint16_t NetworkOrder<uint16_t>::to_host() const {
    return ntohs(value_);
}
template <>
NetworkOrder<uint32_t> NetworkOrder<uint32_t>::from_host(uint32_t val) {
    return NetworkOrder<uint32_t>(htonl(val));
}
template <>
uint32_t NetworkOrder<uint32_t>::to_host() const {
    return ntohl(value_);
}

} // namespace endian


// =============================================================================
// 第13章：DNS 解析
// =============================================================================

namespace dns {

struct ResolveResult {
    std::string hostname;
    std::vector<std::string> addresses;
    std::string canonical_name;
};

std::optional<ResolveResult> resolve(const std::string& hostname,
                                      int family = AF_UNSPEC) {
    addrinfo hints{};
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    addrinfo* result = nullptr;
    int ret = getaddrinfo(hostname.c_str(), nullptr, &hints, &result);
    if (ret != 0) return std::nullopt;

    auto guard = std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>(
        result, freeaddrinfo);

    ResolveResult res;
    res.hostname = hostname;
    if (result->ai_canonname)
        res.canonical_name = result->ai_canonname;

    for (auto* ai = result; ai != nullptr; ai = ai->ai_next) {
        char buf[INET6_ADDRSTRLEN] = {};
        if (ai->ai_family == AF_INET) {
            auto* sin = reinterpret_cast<sockaddr_in*>(ai->ai_addr);
            inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf));
        } else if (ai->ai_family == AF_INET6) {
            auto* sin6 = reinterpret_cast<sockaddr_in6*>(ai->ai_addr);
            inet_ntop(AF_INET6, &sin6->sin6_addr, buf, sizeof(buf));
        }
        res.addresses.emplace_back(buf);
    }

    // 去重
    std::sort(res.addresses.begin(), res.addresses.end());
    res.addresses.erase(
        std::unique(res.addresses.begin(), res.addresses.end()),
        res.addresses.end());

    return res;
}

} // namespace dns


// =============================================================================
// 第14章：超时与错误处理
// =============================================================================

namespace timeout {

// --- 14.1 带超时的连接 ---

bool connect_with_timeout(Socket& sock, const SocketAddress& addr,
                          int timeout_ms) {
    // 设为非阻塞
    sock.set_nonblocking(true);

    int ret = ::connect(sock.fd(), addr.data(), static_cast<int>(addr.size()));

    if (ret == 0) {
        sock.set_nonblocking(false);
        return true; // 立即成功
    }

    // 检查是否 "正在进行中"
#ifdef _WIN32
    if (get_last_error() != WSAEWOULDBLOCK) return false;
#else
    if (errno != EINPROGRESS) return false;
#endif

    // 用 select 等待可写（连接完成或失败）
    SelectSet ss;
    ss.add_write(sock.fd());
    ss.add_except(sock.fd());

    int ready = ss.wait(timeout_ms);
    if (ready <= 0) return false; // 超时或错误

    // 检查连接结果
    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(sock.fd(), SOL_SOCKET, SO_ERROR,
               reinterpret_cast<char*>(&err), &len);

    sock.set_nonblocking(false);
    return err == 0;
}

// --- 14.2 网络错误的现代 C++ 处理 ---

enum class NetError {
    Success = 0,
    ConnectionRefused,
    Timeout,
    HostUnreachable,
    ConnectionReset,
    BrokenPipe,
    Unknown,
};

inline const char* to_string(NetError e) {
    switch (e) {
        case NetError::Success:           return "Success";
        case NetError::ConnectionRefused: return "Connection Refused";
        case NetError::Timeout:           return "Timeout";
        case NetError::HostUnreachable:   return "Host Unreachable";
        case NetError::ConnectionReset:   return "Connection Reset";
        case NetError::BrokenPipe:        return "Broken Pipe";
        case NetError::Unknown:           return "Unknown Error";
    }
    return "Unknown";
}

// 类似 Rust 的 Result<T, E>
template <typename T>
using NetResult = std::variant<T, NetError>;

template <typename T>
bool is_ok(const NetResult<T>& r) { return r.index() == 0; }

template <typename T>
T& get_ok(NetResult<T>& r) { return std::get<0>(r); }

template <typename T>
NetError get_err(const NetResult<T>& r) { return std::get<1>(r); }

} // namespace timeout


// =============================================================================
// 第15章：多线程服务器 — Thread-per-Connection
// =============================================================================

class ThreadedTcpServer {
    TcpServer server_;
    std::atomic<bool> running_{false};
    std::mutex clients_mutex_;
    std::vector<std::thread> worker_threads_;

public:
    using ConnectionHandler = std::function<void(Socket, SocketAddress)>;

    void start(uint16_t port, ConnectionHandler handler, int backlog = 128) {
        server_.start(port, backlog);
        running_ = true;

        std::cout << "  多线程 TCP 服务器启动在端口 " << port << "\n";

        while (running_) {
            auto result = server_.accept();
            if (!result) continue;

            // 为每个连接创建一个线程
            auto addr = result->client_addr;
            std::thread t([handler, sock = std::move(result->client_socket),
                           addr]() mutable {
                try {
                    handler(std::move(sock), addr);
                } catch (const std::exception& e) {
                    std::cerr << "Handler error: " << e.what() << "\n";
                }
            });
            t.detach(); // 分离线程（简化版）
            // 生产环境应该用线程池
        }
    }

    void stop() { running_ = false; }
};

// 使用示例：
//
// ThreadedTcpServer server;
// server.start(9999, [](Socket sock, SocketAddress addr) {
//     std::cout << "新连接: " << addr.to_string() << "\n";
//     sock.send_string("Welcome!\r\n");
//     while (auto data = sock.recv_string()) {
//         sock.send_string("Echo: " + *data);
//     }
// });


// =============================================================================
// 第16章：IO 多路复用服务器 — select 事件驱动
// =============================================================================

class SelectServer {
public:
    using MessageHandler = std::function<std::string(socket_t, std::string_view)>;

private:
    TcpServer server_;
    std::vector<Socket> clients_;
    std::map<socket_t, NetworkBuffer> buffers_;
    MessageHandler handler_;
    std::atomic<bool> running_{false};

public:
    void set_handler(MessageHandler h) { handler_ = std::move(h); }

    void start(uint16_t port) {
        server_.start(port);
        server_.listen_socket().set_nonblocking(true);
        running_ = true;

        std::cout << "  Select 服务器启动在端口 " << port << "\n";

        while (running_) {
            SelectSet ss;
            ss.add_read(server_.listen_socket().fd());

            // 添加所有客户端
            for (auto& client : clients_) {
                ss.add_read(client.fd());
            }

            int ready = ss.wait(100); // 100ms 超时
            if (ready < 0) break;
            if (ready == 0) continue;

            // 检查新连接
            if (ss.is_readable(server_.listen_socket().fd())) {
                accept_new_client();
            }

            // 检查客户端数据
            for (size_t i = 0; i < clients_.size(); ) {
                if (!clients_[i].valid()) {
                    remove_client(i);
                    continue;
                }

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

    void stop() { running_ = false; }

private:
    void accept_new_client() {
        auto result = server_.accept();
        if (!result) return;

        socket_t fd = result->client_socket.fd();
        result->client_socket.set_nonblocking(true);
        buffers_[fd] = NetworkBuffer(1024);
        clients_.push_back(std::move(result->client_socket));
    }

    bool handle_client_data(size_t index) {
        auto& client = clients_[index];
        auto& buf = buffers_[client.fd()];

        int n = buf.read_from(client);
        if (n <= 0) return false; // 连接关闭

        // 处理完整消息（以 \n 分隔）
        while (auto line = buf.read_line()) {
            if (handler_) {
                std::string response = handler_(client.fd(), *line);
                if (!response.empty()) {
                    client.send_string(response);
                }
            }
        }
        return true;
    }

    void remove_client(size_t index) {
        socket_t fd = clients_[index].fd();
        buffers_.erase(fd);
        clients_.erase(clients_.begin() + index);
    }
};


// =============================================================================
// 第17章：实战 — 简易聊天室
// =============================================================================

class ChatServer {
    TcpServer server_;
    std::mutex clients_mtx_;
    std::map<socket_t, std::string> clients_; // fd → nickname
    std::atomic<bool> running_{false};

public:
    void start(uint16_t port) {
        server_.start(port);
        running_ = true;
        std::cout << "  聊天服务器启动在端口 " << port << "\n";
        std::cout << "  (用 telnet localhost " << port << " 连接)\n";

        while (running_) {
            auto result = server_.accept();
            if (!result) continue;

            auto addr_str = result->client_addr.to_string();
            auto fd = result->client_socket.fd();

            {
                std::lock_guard<std::mutex> lock(clients_mtx_);
                clients_[fd] = "User_" + std::to_string(fd);
            }

            std::thread([this, sock = std::move(result->client_socket),
                         addr_str]() mutable {
                handle_client(std::move(sock), addr_str);
            }).detach();
        }
    }

    void stop() { running_ = false; }

private:
    void handle_client(Socket sock, const std::string& addr) {
        auto fd = sock.fd();
        std::string nickname;
        {
            std::lock_guard<std::mutex> lock(clients_mtx_);
            nickname = clients_[fd];
        }

        sock.send_string("欢迎来到聊天室! 你是 " + nickname + "\r\n");
        sock.send_string("输入 /nick <名字> 改名, /quit 退出\r\n");
        broadcast(nickname + " 加入了聊天室\r\n", fd);

        NetworkBuffer buf;
        while (true) {
            int n = buf.read_from(sock);
            if (n <= 0) break;

            while (auto line = buf.read_line()) {
                if (line->empty()) continue;

                // 命令处理
                if (line->substr(0, 5) == "/nick") {
                    std::string new_nick = line->substr(6);
                    if (!new_nick.empty()) {
                        std::lock_guard<std::mutex> lock(clients_mtx_);
                        std::string old = clients_[fd];
                        clients_[fd] = new_nick;
                        broadcast(old + " 改名为 " + new_nick + "\r\n", -1);
                        nickname = new_nick;
                    }
                } else if (*line == "/quit") {
                    sock.send_string("再见!\r\n");
                    goto disconnect;
                } else if (line->substr(0, 1) == "/") {
                    sock.send_string("未知命令: " + *line + "\r\n");
                } else {
                    broadcast("[" + nickname + "] " + *line + "\r\n", fd);
                }
            }
        }

    disconnect:
        {
            std::lock_guard<std::mutex> lock(clients_mtx_);
            clients_.erase(fd);
        }
        broadcast(nickname + " 离开了聊天室\r\n", -1);
    }

    void broadcast(const std::string& msg, socket_t exclude_fd) {
        // 注意：这里简化了广播逻辑
        // 完整实现需要单独的发送缓冲和写线程
        std::lock_guard<std::mutex> lock(clients_mtx_);
        for (auto& [fd, _] : clients_) {
            if (fd != exclude_fd) {
                ::send(fd, msg.data(), static_cast<int>(msg.size()), MSG_NOSIGNAL);
            }
        }
    }
};

// 运行方式：
// 1. 编译并运行本程序（选择聊天服务器模式）
// 2. 打开多个终端：telnet localhost 9999
// 3. 输入消息互聊


// =============================================================================
// 第18章：实战 — Redis 协议客户端 (RESP)
// =============================================================================
//
// RESP (REdis Serialization Protocol) 是一个简单的文本协议：
//   简单字符串: +OK\r\n
//   错误:       -ERR message\r\n
//   整数:       :1000\r\n
//   批量字符串: $6\r\nfoobar\r\n
//   数组:       *2\r\n$3\r\nGET\r\n$3\r\nkey\r\n
//   NULL:       $-1\r\n

namespace redis {

// RESP 值类型
struct RespNull {};
struct RespError { std::string message; };

using RespValue = std::variant<
    RespNull,              // null
    std::string,           // 简单字符串 / 批量字符串
    int64_t,               // 整数
    RespError,             // 错误
    std::vector<std::variant<RespNull, std::string, int64_t, RespError>> // 数组（简化）
>;

class RedisClient {
    TcpClient client_;
    NetworkBuffer buf_;

public:
    RedisClient() : buf_(8192) {}

    void connect(const char* host = "127.0.0.1", uint16_t port = 6379) {
        client_.connect(host, port);
    }

    // 发送 RESP 命令
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
    RespValue set(std::string_view key, std::string_view value) {
        return command({"SET", key, value});
    }

    RespValue get(std::string_view key) {
        return command({"GET", key});
    }

    RespValue del(std::string_view key) {
        return command({"DEL", key});
    }

    RespValue ping() {
        return command({"PING"});
    }

private:
    RespValue read_response() {
        // 确保缓冲区有数据
        while (buf_.readable_bytes() == 0) {
            int n = buf_.read_from(client_.socket());
            if (n <= 0) throw std::runtime_error("Connection lost");
        }

        char type = *buf_.read_ptr();
        buf_.advance_read(1);

        switch (type) {
            case '+': return read_simple_string();
            case '-': return read_error();
            case ':': return read_integer();
            case '$': return read_bulk_string();
            // '*': 数组（简化实现省略）
            default:
                throw std::runtime_error(
                    std::string("Unknown RESP type: ") + type);
        }
    }

    std::string read_line_from_buf() {
        while (true) {
            auto pos = buf_.find("\r\n");
            if (pos) {
                std::string line = buf_.read_string(*pos);
                buf_.advance_read(2); // \r\n
                return line;
            }
            int n = buf_.read_from(client_.socket());
            if (n <= 0) throw std::runtime_error("Connection lost");
        }
    }

    RespValue read_simple_string() {
        return read_line_from_buf();
    }

    RespValue read_error() {
        return RespError{read_line_from_buf()};
    }

    RespValue read_integer() {
        return static_cast<int64_t>(std::stoll(read_line_from_buf()));
    }

    RespValue read_bulk_string() {
        int len = std::stoi(read_line_from_buf());
        if (len == -1) return RespNull{};

        // 读取 len 字节 + \r\n
        while (buf_.readable_bytes() < static_cast<size_t>(len + 2)) {
            int n = buf_.read_from(client_.socket());
            if (n <= 0) throw std::runtime_error("Connection lost");
        }

        std::string data = buf_.read_string(len);
        buf_.advance_read(2); // \r\n
        return data;
    }
};

// RESP 值的打印
std::string resp_to_string(const RespValue& val) {
    return std::visit([](const auto& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, RespNull>)
            return "(nil)";
        else if constexpr (std::is_same_v<T, std::string>)
            return "\"" + v + "\"";
        else if constexpr (std::is_same_v<T, int64_t>)
            return "(integer) " + std::to_string(v);
        else if constexpr (std::is_same_v<T, RespError>)
            return "(error) " + v.message;
        else
            return "(array)";
    }, val);
}

} // namespace redis

// 使用示例（需要运行中的 Redis 服务器）：
//
//   redis::RedisClient client;
//   client.connect("127.0.0.1", 6379);
//
//   auto ping_result = client.ping();
//   std::cout << redis::resp_to_string(ping_result) << "\n"; // "PONG"
//
//   client.set("name", "Modern C++");
//   auto val = client.get("name");
//   std::cout << redis::resp_to_string(val) << "\n"; // "Modern C++"


// =============================================================================
// 第19章：网络编程最佳实践与调试
// =============================================================================
//
// --- 19.1 最佳实践 ---
//
// 1. 资源管理:
//    ✅ RAII 封装 socket（本教程的 Socket 类）
//    ✅ 用 unique_ptr 管理动态资源
//    ❌ 裸 socket fd + 手动 close
//
// 2. 错误处理:
//    ✅ 检查每个系统调用的返回值
//    ✅ 用 optional/variant 表达可能的失败
//    ❌ 忽略错误码
//
// 3. 缓冲区:
//    ✅ 使用 NetworkBuffer 做读写缓冲
//    ✅ 处理部分发送/接收
//    ❌ 假设 send/recv 一次完成
//
// 4. 协议设计:
//    ✅ 明确的消息边界（长度前缀 或 分隔符）
//    ✅ 版本号 + 魔数
//    ❌ 假设 TCP 保持消息边界
//
// 5. 性能:
//    ✅ TCP_NODELAY (低延迟)
//    ✅ 非阻塞 I/O + 事件循环
//    ✅ 连接复用
//    ❌ Thread-per-connection (万级连接)
//
// 6. 安全:
//    ✅ 验证所有输入长度
//    ✅ 超时机制
//    ❌ 信任客户端数据
//
// --- 19.2 常见 Bug ---
//
// Bug 1: TCP 粘包
//   TCP 是字节流，不保留消息边界。
//   两次 send("AB"), send("CD") 可能被对方一次收到 "ABCD"，
//   或 "A", "BCD"，或任意拆分。
//   解决：用长度前缀 或 分隔符。
//
// Bug 2: 部分发送
//   send(1000 字节) 可能只发了 500 字节。
//   必须循环发送直到全部完成（见 TcpClient::send_all）。
//
// Bug 3: SIGPIPE (Unix)
//   写入已关闭的 socket 会触发 SIGPIPE 信号导致进程退出。
//   解决：signal(SIGPIPE, SIG_IGN) 或 MSG_NOSIGNAL。
//
// Bug 4: TIME_WAIT
//   服务器重启时 bind 失败：Address already in use。
//   解决：SO_REUSEADDR。
//
// Bug 5: 忘记网络字节序转换
//   直接发送主机序整数 → 大小端不同的机器收到错误值。
//   解决：htonl/htons。
//
// --- 19.3 调试工具 ---
//
// 1. Wireshark     — 图形化抓包分析
// 2. tcpdump       — 命令行抓包
//    tcpdump -i any port 8080 -X
// 3. netstat/ss    — 查看连接状态
//    ss -tlnp | grep 8080
// 4. netcat (nc)   — 简单 TCP/UDP 测试
//    nc -l -p 9999        (服务端)
//    nc localhost 9999     (客户端)
// 5. curl          — HTTP 测试
//    curl -v http://localhost:8080/
//
// --- 19.4 性能调优参数 ---
//
// | 参数              | 说明                | 推荐值          |
// |-------------------|--------------------|-----------------| 
// | TCP_NODELAY       | 禁用 Nagle 算法     | 低延迟场景开启   |
// | SO_REUSEADDR      | 允许地址复用         | 服务器总是开启   |
// | SO_KEEPALIVE      | TCP 保活            | 长连接开启       |
// | SO_RCVBUF/SNDBUF  | 收发缓冲区大小       | 根据带宽调整     |
// | SO_LINGER         | 关闭行为            | 按需设置         |
// | TCP_QUICKACK      | 快速 ACK (Linux)    | 低延迟场景       |
// | TCP_FASTOPEN      | TFO (Linux 3.7+)   | 减少握手延迟     |


// =============================================================================
// 附录：网络编程核心概念速查表
// =============================================================================
//
// Socket 类型：
// ──────────────────────────────────────────────────
// | 类型         | 常量        | 协议  | 特点       |
// |-------------|------------|-------|------------|
// | 流式 Socket  | SOCK_STREAM| TCP   | 可靠、有序  |
// | 数据报 Socket| SOCK_DGRAM | UDP   | 不可靠、无序|
// | 原始 Socket  | SOCK_RAW   | IP    | 底层访问    |
//
// TCP 连接流程：
// ──────────────────────────────────────────────────
// 服务端: socket → bind → listen → accept → recv/send → close
// 客户端: socket → connect → send/recv → close
//
// IO 模型：
// ──────────────────────────────────────────────────
// | 模型          | 函数              | 特点              |
// |--------------|-------------------|------------------|
// | 阻塞 IO      | recv (默认)        | 简单，效率低       |
// | 非阻塞 IO    | recv + O_NONBLOCK  | 需要轮询          |
// | IO 多路复用   | select/poll/epoll  | 高效              |
// | 信号驱动 IO  | SIGIO              | 复杂              |
// | 异步 IO      | io_uring (Linux)   | 最高效            |
//
// 地址族：
// ──────────────────────────────────────────────────
// | 常量     | 说明             |
// |---------|-----------------|
// | AF_INET | IPv4             |
// | AF_INET6| IPv6             |
// | AF_UNIX | Unix 域 Socket   |
// =============================================================================


} // namespace net


// =============================================================================
// 演示函数
// =============================================================================

// --- DNS 解析演示 ---
void demo_dns_resolve() {
    std::vector<std::string> hosts = {
        "localhost",
        "example.com",
        "google.com"
    };

    for (const auto& host : hosts) {
        auto result = net::dns::resolve(host);
        if (result) {
            std::cout << "  " << host << " → ";
            for (size_t i = 0; i < result->addresses.size() && i < 3; ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << result->addresses[i];
            }
            if (!result->canonical_name.empty() &&
                result->canonical_name != host)
                std::cout << " (CNAME: " << result->canonical_name << ")";
            std::cout << "\n";
        } else {
            std::cout << "  " << host << " → 解析失败\n";
        }
    }
}

// --- Buffer 演示 ---
void demo_buffer() {
    net::NetworkBuffer buf(64);

    // 写入数据
    buf.append("GET / HTTP/1.1\r\n");
    buf.append("Host: example.com\r\n");
    buf.append("\r\n");

    std::cout << "  Buffer 可读: " << buf.readable_bytes() << " 字节\n";

    // 逐行读取
    int line_num = 0;
    while (auto line = buf.read_line()) {
        std::cout << "  Line " << ++line_num << ": [" << *line << "]\n";
    }
}

// --- 二进制协议演示 ---
void demo_protocol() {
    // 编码
    net::protocol::PacketWriter writer;
    writer.write_u8(1);             // 消息类型
    writer.write_u32(12345);        // 用户 ID
    writer.write_string("Hello!");  // 消息内容

    auto packet = writer.finish();
    std::cout << "  Packet 大小: " << packet.size() << " 字节\n";
    std::cout << "  Header: 8 字节, Payload: "
              << packet.size() - 8 << " 字节\n";

    // 解码（跳过 header）
    net::protocol::PacketReader reader(
        packet.data() + sizeof(net::protocol::PacketHeader),
        packet.size() - sizeof(net::protocol::PacketHeader));

    uint8_t msg_type;
    uint32_t user_id;
    std::string message;

    reader.read_u8(msg_type);
    reader.read_u32(user_id);
    reader.read_string(message);

    std::cout << "  解码: type=" << (int)msg_type
              << ", user_id=" << user_id
              << ", msg=\"" << message << "\"\n";
}

// --- 字节序演示 ---
void demo_endian() {
    std::cout << "  系统字节序: "
              << (net::endian::is_little_endian() ? "Little-Endian" : "Big-Endian")
              << "\n";

    uint16_t port = 8080;
    uint16_t net_port = htons(port);
    std::cout << "  端口 " << port << " 主机序: 0x"
              << std::hex << port << " → 网络序: 0x"
              << net_port << std::dec << "\n";

    auto n = net::endian::NetworkOrder<uint32_t>::from_host(0x12345678);
    std::cout << "  0x12345678 → 网络序 → 还原: 0x"
              << std::hex << n.to_host() << std::dec << "\n";
}

// --- TCP Echo 服务器演示 ---
void demo_echo_server(uint16_t port) {
    net::TcpServer server;
    server.start(port);

    std::cout << "  Echo 服务器启动在端口 " << port << "\n";
    std::cout << "  (在另一个终端用 telnet localhost " << port << " 连接)\n";
    std::cout << "  (按 Ctrl+C 退出)\n\n";

    while (true) {
        auto result = server.accept();
        if (!result) continue;

        auto addr_str = result->client_addr.to_string();
        std::cout << "  新连接: " << addr_str << "\n";

        auto& sock = result->client_socket;
        sock.send_string("Welcome to Echo Server! (type 'quit' to exit)\r\n");

        net::NetworkBuffer buf;
        while (true) {
            int n = buf.read_from(sock);
            if (n <= 0) break;

            while (auto line = buf.read_line()) {
                if (*line == "quit") {
                    sock.send_string("Bye!\r\n");
                    goto done;
                }
                sock.send_string("Echo: " + *line + "\r\n");
            }
        }

    done:
        std::cout << "  连接关闭: " << addr_str << "\n";
    }
}

// --- HTTP 服务器演示 ---
void demo_http_server(uint16_t port) {
    net::HttpServer server;

    server.route("/", [](const net::HttpRequest&) {
        net::HttpResponse resp;
        resp.status_code = 200;
        resp.status_text = "OK";
        resp.body =
            "<!DOCTYPE html>\n"
            "<html><head><title>Modern C++ HTTP Server</title></head>\n"
            "<body>\n"
            "<h1>Hello from Modern C++!</h1>\n"
            "<p>This is a simple HTTP server written in C++17.</p>\n"
            "<ul>\n"
            "  <li><a href=\"/api/time\">Current Time</a></li>\n"
            "  <li><a href=\"/api/info\">Server Info</a></li>\n"
            "</ul>\n"
            "</body></html>\n";
        resp.headers["Content-Type"] = "text/html; charset=utf-8";
        return resp;
    });

    server.route("/api/time", [](const net::HttpRequest&) {
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        std::string time_str = std::ctime(&t);
        if (!time_str.empty() && time_str.back() == '\n')
            time_str.pop_back();

        net::HttpResponse resp;
        resp.status_code = 200;
        resp.status_text = "OK";
        resp.body = "{\"time\":\"" + time_str + "\"}";
        resp.headers["Content-Type"] = "application/json";
        return resp;
    });

    server.route("/api/info", [](const net::HttpRequest& req) {
        net::HttpResponse resp;
        resp.status_code = 200;
        resp.status_text = "OK";
        resp.body = "{\"method\":\"" + req.method + "\","
                    "\"path\":\"" + req.path + "\","
                    "\"version\":\"" + req.version + "\"}";
        resp.headers["Content-Type"] = "application/json";
        return resp;
    });

    std::cout << "  HTTP 服务器启动\n";
    std::cout << "  浏览器访问: http://localhost:" << port << "/\n";
    std::cout << "  (按 Ctrl+C 退出)\n\n";

    server.start(port);
}


// =============================================================================
// main: 菜单选择
// =============================================================================
int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);

    // 初始化网络库
    net::WinsockInit wsa;

    std::cout << "===== 现代 C++ 网络编程教程 =====\n\n";

    std::cout << "选择演示模式:\n";
    std::cout << "  1 - 基础演示（DNS/Buffer/协议/字节序）\n";
    std::cout << "  2 - TCP Echo 服务器 (端口 9999)\n";
    std::cout << "  3 - HTTP 服务器 (端口 8080)\n";
    std::cout << "  4 - 聊天服务器 (端口 9999)\n";
    std::cout << "\n请输入 (1-4): ";

    int choice = 1;
    if (argc > 1) {
        choice = std::atoi(argv[1]);
    } else {
        std::cin >> choice;
    }

    std::cout << "\n";

    switch (choice) {
        case 1: {
            std::cout << "[DNS 解析]\n";
            demo_dns_resolve();
            std::cout << "\n[Buffer 管理]\n";
            demo_buffer();
            std::cout << "\n[二进制协议]\n";
            demo_protocol();
            std::cout << "\n[字节序]\n";
            demo_endian();
            std::cout << "\n===== 演示完成 =====\n";
            break;
        }

        case 2:
            demo_echo_server(9999);
            break;

        case 3:
            demo_http_server(8080);
            break;

        case 4: {
            net::ChatServer chat;
            chat.start(9999);
            break;
        }

        default:
            std::cout << "无效选择\n";
    }

    return 0;
}
