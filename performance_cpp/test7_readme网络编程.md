# 现代C++网络编程详细指导教程与代码示例教学

> The user wants a comprehensive Modern C++ Network Programming tutorial with code examples for test7.cpp. This should cover socket programming, HTTP, async I/O, etc. using modern C++ style. Since this is a standalone tutorial file like the previous ones, I'll create a complete tutorial covering networking concepts from basics to advanced.
>
> Since C++ doesn't have a standard networking library yet (Networking TS is not finalized), I'll need to use POSIX sockets (with Windows compatibility via Winsock) and demonstrate modern C++ patterns around them.
>
> Let me create a comprehensive tutorial.


已创建 test7.cpp，共 19 章，完整覆盖现代 C++ 网络编程：

| 章节 | 主题           | 核心内容                                                      |
| ---- | -------------- | ------------------------------------------------------------- |
| 1    | 平台抽象层     | Winsock/POSIX 统一、RAII 初始化                               |
| 2    | Socket RAII    | 封装 `socket_t`，自动关闭、移动语义、选项设置               |
| 3    | 地址封装       | `SocketAddress` 统一 IPv4/IPv6，`inet_pton`/`inet_ntop` |
| 4    | TCP 客户端     | `TcpClient`，`getaddrinfo` 解析、`send_all` 完整发送    |
| 5    | TCP 服务器     | `TcpServer`，bind/listen/accept、`SO_REUSEADDR`           |
| 6    | UDP 通信       | `UdpSocket`，`sendto`/`recvfrom` 封装                   |
| 7    | IO 多路复用    | `SelectSet` 封装 `select()`，跨平台                       |
| 8    | Buffer 管理    | `NetworkBuffer`，零拷贝读写、协议解析、行读取               |
| 9    | HTTP 客户端    | `HttpClient`，手写 GET/POST 请求、响应解析                  |
| 10   | HTTP 服务器    | `HttpServer`，路由注册、请求解析、响应发送                  |
| 11   | 协议序列化     | `PacketWriter`/`PacketReader`，二进制编解码               |
| 12   | 网络字节序     | 编译期交换、`NetworkOrder<T>` 强类型                        |
| 13   | DNS 解析       | `getaddrinfo` 封装，支持 IPv4/IPv6                          |
| 14   | 超时与错误     | 带超时连接、`NetResult<T>` (variant 错误处理)               |
| 15   | 多线程服务器   | Thread-per-Connection 模式                                    |
| 16   | 事件驱动服务器 | `SelectServer`，select 事件循环                             |
| 17   | 聊天室实战     | 完整聊天服务器，支持改名、广播                                |
| 18   | Redis 客户端   | RESP 协议编解码、SET/GET/PING                                 |
| 19   | 最佳实践       | 粘包/SIGPIPE/TIME_WAIT/调试工具/性能调优                      |

编译：

- **Windows**: `cl /std:c++17 /O2 /EHsc test7.cpp /link ws2_32.lib`
- **Linux**: `g++ -std=c++17 -O2 -pthread -o test7 test7.cpp`

运行后可选择：基础演示 / Echo服务器 / HTTP服务器 / 聊天室。
