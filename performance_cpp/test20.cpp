// =============================================================================
// test20: 增量协议解析与半包处理（C++17）
// =============================================================================
// 目标：补充 test7 中“教学版一次性解析”的局限，演示生产常见的
//      "分片到达 / 半包 / 粘包" 下如何进行可重复调用的增量解析。
//
// 编译：
//   g++ -std=c++17 -O2 -Wall -Wextra -o test20 test20.cpp
//   cl /std:c++17 /O2 /EHsc test20.cpp
// =============================================================================

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class Buffer {
    std::vector<char> data_;
    size_t read_pos_ = 0;
    size_t write_pos_ = 0;

public:
    explicit Buffer(size_t cap = 4096) : data_(cap) {}

    void append(std::string_view sv) {
        ensure_writable(sv.size());
        std::copy(sv.begin(), sv.end(), data_.begin() + static_cast<long long>(write_pos_));
        write_pos_ += sv.size();
    }

    size_t readable_bytes() const { return write_pos_ - read_pos_; }
    const char* read_ptr() const { return data_.data() + static_cast<long long>(read_pos_); }

    void advance_read(size_t n) {
        n = std::min(n, readable_bytes());
        read_pos_ += n;
        if (read_pos_ == write_pos_) {
            read_pos_ = 0;
            write_pos_ = 0;
        }
    }

    std::optional<size_t> find(std::string_view pattern) const {
        if (pattern.empty() || readable_bytes() < pattern.size()) return std::nullopt;
        std::string_view sv(read_ptr(), readable_bytes());
        size_t pos = sv.find(pattern);
        if (pos == std::string_view::npos) return std::nullopt;
        return pos;
    }

    std::string_view peek(size_t n) const {
        n = std::min(n, readable_bytes());
        return {read_ptr(), n};
    }

private:
    void ensure_writable(size_t n) {
        size_t writable = data_.size() - write_pos_;
        if (writable >= n) return;

        size_t readable = readable_bytes();
        if (read_pos_ + writable >= n) {
            std::move(data_.begin() + static_cast<long long>(read_pos_),
                      data_.begin() + static_cast<long long>(write_pos_),
                      data_.begin());
            read_pos_ = 0;
            write_pos_ = readable;
        } else {
            data_.resize(write_pos_ + n);
        }
    }
};

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

class IncrementalHttpParser {
public:
    std::optional<HttpRequest> try_parse(Buffer& buf) {
        auto header_end = buf.find("\r\n\r\n");
        if (!header_end) return std::nullopt;

        std::string_view head = buf.peek(*header_end + 4);
        HttpRequest req;
        if (!parse_head(head, req)) return std::nullopt;

        size_t body_len = 0;
        auto it = req.headers.find("content-length");
        if (it != req.headers.end()) {
            try {
                body_len = static_cast<size_t>(std::stoul(it->second));
            } catch (...) {
                return std::nullopt;
            }
        }

        size_t total_needed = *header_end + 4 + body_len;
        if (buf.readable_bytes() < total_needed) return std::nullopt;

        if (body_len > 0) {
            std::string_view all = buf.peek(total_needed);
            req.body.assign(all.data() + static_cast<long long>(*header_end + 4), body_len);
        }

        buf.advance_read(total_needed);
        return req;
    }

private:
    static std::string lower_copy(std::string s) {
        for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    }

    static bool parse_head(std::string_view head, HttpRequest& req) {
        size_t line_end = head.find("\r\n");
        if (line_end == std::string_view::npos) return false;

        std::string request_line(head.substr(0, line_end));
        if (!parse_request_line(request_line, req)) return false;

        size_t pos = line_end + 2;
        while (pos < head.size()) {
            size_t next = head.find("\r\n", pos);
            if (next == std::string_view::npos) return false;
            if (next == pos) break;

            std::string hline(head.substr(pos, next - pos));
            auto colon = hline.find(':');
            if (colon == std::string::npos) return false;
            std::string key = lower_copy(trim(hline.substr(0, colon)));
            std::string val = trim(hline.substr(colon + 1));
            req.headers[std::move(key)] = std::move(val);
            pos = next + 2;
        }

        return true;
    }

    static bool parse_request_line(const std::string& line, HttpRequest& req) {
        size_t sp1 = line.find(' ');
        size_t sp2 = (sp1 == std::string::npos) ? std::string::npos : line.find(' ', sp1 + 1);
        if (sp1 == std::string::npos || sp2 == std::string::npos) return false;
        req.method = line.substr(0, sp1);
        req.path = line.substr(sp1 + 1, sp2 - sp1 - 1);
        req.version = line.substr(sp2 + 1);
        return true;
    }

    static std::string trim(const std::string& s) {
        size_t l = 0;
        while (l < s.size() && std::isspace(static_cast<unsigned char>(s[l]))) ++l;
        size_t r = s.size();
        while (r > l && std::isspace(static_cast<unsigned char>(s[r - 1]))) --r;
        return s.substr(l, r - l);
    }
};

int main() {
    Buffer buf;
    IncrementalHttpParser parser;

    std::string part1 = "POST /submit HTTP/1.1\r\nHost: local\r\nContent-Length: 11\r\n\r\nhello";
    std::string part2 = " worldGET /ping HTTP/1.1\r\nHost: local\r\n\r\n";

    buf.append(part1);
    auto req1 = parser.try_parse(buf);
    if (!req1) {
        std::cout << "[1] first parse pending (expected half packet)\n";
    }

    buf.append(part2);
    req1 = parser.try_parse(buf);
    if (req1) {
        std::cout << "[2] parsed req1: " << req1->method << " " << req1->path
                  << ", body='" << req1->body << "'\n";
    }

    auto req2 = parser.try_parse(buf);
    if (req2) {
        std::cout << "[3] parsed req2: " << req2->method << " " << req2->path << "\n";
    }

    std::cout << "[4] remaining bytes: " << buf.readable_bytes() << "\n";
    return 0;
}
